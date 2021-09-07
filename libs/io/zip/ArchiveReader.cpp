/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ArchiveReader.hpp"

#include "exception/Read.hpp"

#include "minizip/unzip.h"

#include <core/exceptionmacros.hpp>

#include <boost/iostreams/stream.hpp>

#include <mutex>
#include <set>

namespace sight::io::zip
{

/// Implementation class
class ArchiveReaderImpl final : public ArchiveReader
{
public:

    SIGHT_DECLARE_CLASS(ArchiveReaderImpl, ArchiveReader);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    ArchiveReaderImpl()                                    = delete;
    ArchiveReaderImpl(const ArchiveReaderImpl&)            = delete;
    ArchiveReaderImpl(ArchiveReaderImpl&&)                 = delete;
    ArchiveReaderImpl& operator=(const ArchiveReaderImpl&) = delete;
    ArchiveReaderImpl& operator=(ArchiveReaderImpl&&)      = delete;

    /// Constructor. It open the archive and create all resources needed to access it.
    /// @param archivePath path of the archive file. The file will be kept opened as long as the instance lives.
    ArchiveReaderImpl(const std::filesystem::path& archivePath) :
        m_archivePath(archivePath)
    {
        const std::string& archivePathString = archivePath.string();

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read("Archive '" + archivePathString + "' doesn't exist."),
            !std::filesystem::exists(archivePath)
        );

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read("Archive '" + archivePathString + "' is already opened."),
            is_locked(archivePath)
        );

        // Lock the archive to disallow opening it again
        lock(archivePath);

        m_unzFile = unzOpen(archivePathString.c_str());

        if(m_unzFile == nullptr)
        {
            unlock(archivePath);
            SIGHT_THROW_EXCEPTION(exception::Read("Archive '" + archivePathString + "' cannot be opened."));
        }
    }

    /// Destructor
    ~ArchiveReaderImpl() override
    {
        if(m_unzFile != nullptr)
        {
            const int result = unzClose(m_unzFile);

            SIGHT_THROW_EXCEPTION_IF(
                exception::Read("Archive '" + m_archivePath.string() + "' cannot be closed."),
                result != UNZ_OK
            );

            m_unzFile = nullptr;
        }

        // Unlock the archive
        unlock(m_archivePath);
    }

    /// Returns a std::istream to read an archived file
    /// @param filePath path of an archived file.
    /// @param password the password needed to decrypt the file.
    std::unique_ptr<std::istream> openFile(
        const std::filesystem::path& filePath,
        const core::crypto::secure_string& password = ""
    ) override
    {
        // Boost iostreams Source. It allows easy istream implementation
        class ZipSource
        {
        public:

            typedef char char_type;
            typedef boost::iostreams::source_tag category;

            /// Delete default constructor
            ZipSource() = delete;

            // Boost iostreams Source do not like constructor with more than one parameter
            // So, we use a sub struct to hold parameters and class attributes.
            struct Parameters
            {
                ArchiveReaderImpl* const m_archive;
                const std::filesystem::path m_filePath;
                const core::crypto::secure_string m_password;

                Parameters(
                    ArchiveReaderImpl* const archive,
                    const std::filesystem::path& filePath,
                    const core::crypto::secure_string& password
                ) :
                    m_archive(archive),
                    m_filePath(filePath),
                    m_password(password)
                {
                }
            };

            ZipSource(const Parameters& parameters) :
                m_attributes(parameters),
                m_handleKeeper(std::make_shared<const HandleKeeper>(m_attributes))
            {
            }

            //------------------------------------------------------------------------------

            std::streamsize read(char* buffer, std::streamsize size)
            {
                const int read = unzReadCurrentFile(
                    m_attributes.m_archive->m_unzFile,
                    buffer,
                    static_cast<std::uint32_t>(size)
                );

                SIGHT_THROW_EXCEPTION_IF(
                    exception::Read(
                        "Cannot read file '"
                        + m_attributes.m_filePath.string()
                        + "' in archive '"
                        + m_attributes.m_archive->m_archivePath.string()
                        + "'."
                    ),
                    read < 0
                );

                return read;
            }

        private:

            // Used to destroy the handle to the zip file, so it is only deleted once
            // BEWARE Boost makes copy of ZipSource !
            struct HandleKeeper
            {
                HandleKeeper(const Parameters& parameters) :
                    m_attributes(parameters)
                {
                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Read(
                            "File '"
                            + m_attributes.m_filePath.string()
                            + "' in archive '"
                            + m_attributes.m_archive->m_archivePath.string()
                            + "' is already opened."
                        ),
                        m_attributes.m_archive->is_locked(m_attributes.m_filePath)
                    );

                    // Lock the file to disallow opening it again
                    m_attributes.m_archive->lock(m_attributes.m_filePath);

                    // Locate the file
                    int result = unzLocateFile(
                        m_attributes.m_archive->m_unzFile,
                        m_attributes.m_filePath.string().c_str(),
                        nullptr
                    );

                    if(result != MZ_OK)
                    {
                        // In case of error, unlock the file
                        m_attributes.m_archive->unlock(m_attributes.m_filePath);
                    }

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Read(
                            "File '"
                            + m_attributes.m_filePath.string()
                            + "' doesn't exist in archive '"
                            + m_attributes.m_archive->m_archivePath.string()
                            + "'"
                        ),
                        result != UNZ_OK
                    );

                    // Open the file
                    if(m_attributes.m_password.empty())
                    {
                        result = unzOpenCurrentFile(m_attributes.m_archive->m_unzFile);
                    }
                    else
                    {
                        result = unzOpenCurrentFilePassword(
                            m_attributes.m_archive->m_unzFile,
                            m_attributes.m_password.c_str()
                        );
                    }

                    if(result != MZ_OK)
                    {
                        // In case of error, unlock the file
                        m_attributes.m_archive->unlock(m_attributes.m_filePath);

                        SIGHT_THROW_EXCEPTION_IF(
                            exception::BadPassword(
                                "The password used to open file '"
                                + m_attributes.m_filePath.string()
                                + "' in archive '"
                                + m_attributes.m_archive->m_archivePath.string()
                                + "' is wrong."
                            ),
                            result == MZ_PASSWORD_ERROR
                        );

                        SIGHT_THROW_EXCEPTION(
                            exception::Read(
                                "Cannot open file '"
                                + m_attributes.m_filePath.string()
                                + "' in archive '"
                                + m_attributes.m_archive->m_archivePath.string()
                                + "'.\n\n Error code: "
                                + std::to_string(result)
                            )
                        );
                    }
                }

                ~HandleKeeper()
                {
                    // Do not forget to clean file handle in zip
                    const int result = unzCloseCurrentFile(m_attributes.m_archive->m_unzFile);

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Read(
                            "Cannot close file '"
                            + m_attributes.m_filePath.string()
                            + "' in archive '"
                            + m_attributes.m_archive->m_archivePath.string()
                            + "'."
                        ),
                        result != MZ_OK
                    );

                    m_attributes.m_archive->unlock(m_attributes.m_filePath);
                }

                // Store constructor parameters
                const Parameters m_attributes;
            };

            // Store constructor parameters
            const Parameters m_attributes;

            // Used to destroy the zip handle
            const std::shared_ptr<const HandleKeeper> m_handleKeeper;
        };

        const ZipSource::Parameters parameters(
            this,
            filePath,
            password
        );

        // Creating a ZipSource also lock the archive mutex.
        // Due to its design, minizip only allows one archive with the same path with one file operation.
        return std::make_unique<boost::iostreams::stream<ZipSource> >(parameters);
    }

private:

    /// Locks a file
    void lock(const std::filesystem::path& filePath)
    {
        // Normalize path
        const std::filesystem::path normalizedPath = filePath.lexically_normal();

        std::lock_guard lock(m_fileMutex);
        m_files.insert(normalizedPath);
    }

    /// Unlocks a file
    void unlock(const std::filesystem::path& filePath)
    {
        // Normalize path
        const std::filesystem::path normalizedPath = filePath.lexically_normal();

        std::lock_guard lock(m_fileMutex);
        m_files.erase(normalizedPath);
    }

    /// Returns true if a file is locked
    bool is_locked(const std::filesystem::path& filePath)
    {
        // Normalize path
        const std::filesystem::path normalizedPath = filePath.lexically_normal();

        std::lock_guard lock(m_fileMutex);
        return m_files.find(normalizedPath) != m_files.end();
    }

    /// Internal class
    friend class ZipSource;

    /// Path of the archive file
    std::filesystem::path m_archivePath;

    /// Archive context handle
    void* m_unzFile {nullptr};

    /// Set of opened file
    std::set<std::filesystem::path> m_files;

    /// Protect opened file set
    std::mutex m_fileMutex;
};

//------------------------------------------------------------------------------

ArchiveReader::sptr ArchiveReader::shared(const std::filesystem::path& archivePath)
{
    return std::make_shared<ArchiveReaderImpl>(archivePath);
}

} // namespace sight::io::zip
