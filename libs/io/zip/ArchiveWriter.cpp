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

#include "ArchiveWriter.hpp"

#include "exception/Write.hpp"

#include "minizip/zip.h"

#include <core/exceptionmacros.hpp>

#include <boost/iostreams/stream.hpp>

#include <tuple>

namespace sight::io::zip
{

/// Convert argument to minizip dialect
static inline std::tuple<int, int> to_minizip_parameter(Method method, Level level)
{
    int minizipMethod = MZ_COMPRESS_METHOD_ZSTD;
    int minizipLevel  = MZ_COMPRESS_LEVEL_DEFAULT;

    switch(method)
    {
        case Method::STORE:
            minizipMethod = MZ_COMPRESS_METHOD_STORE;
            minizipLevel  = MZ_COMPRESS_LEVEL_DEFAULT;

            break;

        case Method::DEFLATE:
            minizipMethod = MZ_COMPRESS_METHOD_DEFLATE;

            switch(level)
            {
                case Level::BEST:
                case Level::ULTRA:
                    minizipLevel = MZ_COMPRESS_LEVEL_BEST;
                    break;

                case Level::FAST:
                    minizipLevel = MZ_COMPRESS_LEVEL_FAST;
                    break;

                default:
                    minizipLevel = MZ_COMPRESS_LEVEL_DEFAULT;
                    break;
            }

            break;

        case Method::ZSTD:
            minizipMethod = MZ_COMPRESS_METHOD_ZSTD;

            switch(level)
            {
                case Level::ULTRA:
                    minizipLevel = 22;
                    break;

                case Level::BEST:
                    minizipLevel = 19;
                    break;

                case Level::FAST:
                    minizipLevel = 1;
                    break;

                default:
                    minizipLevel = 5;
                    break;
            }

            break;

        default:
            minizipMethod = MZ_COMPRESS_METHOD_ZSTD;
            minizipLevel  = MZ_COMPRESS_LEVEL_DEFAULT;

            break;
    }

    return {minizipMethod, minizipLevel};
}

/// Open / create a new file in a zip archive
static inline int openNewFileInZip(
    zipFile zipFileHandle,
    const std::filesystem::path& filePath,
    const core::crypto::secure_string& password,
    const int method,
    const int level
)
{
    zip_fileinfo zipFileInfo;
    const char* const key = password.empty() ? nullptr : password.c_str();

    // std::filesystem::path.c_str() returns w_char_t on Windows...
    const std::string& filePathString = filePath.string();

    return zipOpenNewFileInZip5(
        zipFileHandle, // zipFile file
        filePathString.c_str(), // const char *filename : the filename in zip
        &zipFileInfo, // Inst zip_fileinfo *zipfi: contain supplemental information
        nullptr, // (UNUSED) const void *extrafield_local: buffer to store the local header extra field data
        0, // (UNUSED) uint16_t size_extrafield_local: size of extrafield_local buffer
        nullptr, // const void *extrafield_global: buffer to store the global header extra field data
        0, // uint16_t size_extrafield_global: size of extrafield_local buffer
        nullptr, // const char *comment: buffer for comment string
        method, // int method: contain the compression method
        level, // int level: contain the level of compression
        0, // int raw: use 0 to disable
        0, // (UNUSED) int windowBits: use default value
        0, // (UNUSED) int memLevel: use default value
        0, // (UNUSED) int strategy: use default value
        key, // const char *password: NULL means no encryption
        0, // (UNUSED) unsigned long crc_for_crypting,
        0, // unsigned long version_madeby
        0, // uint16_t flag_base: use default value
        1 // int zip64: use 0 to disable
    );
}

//------------------------------------------------------------------------------

/// Implementation class
class ArchiveWriterImpl final : public ArchiveWriter
{
public:

    SIGHT_DECLARE_CLASS(ArchiveWriterImpl, ArchiveWriter)

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    ArchiveWriterImpl()                                = delete;
    ArchiveWriterImpl(const ArchiveWriter&)            = delete;
    ArchiveWriterImpl(ArchiveWriter&&)                 = delete;
    ArchiveWriterImpl& operator=(const ArchiveWriter&) = delete;
    ArchiveWriterImpl& operator=(ArchiveWriter&&)      = delete;

    /// Constructor. It opens the archive and creates all resources needed to access it.
    /// @param archivePath path of the archive file. The file will be kept opened as long as the instance lives.
    ArchiveWriterImpl(const std::filesystem::path& archivePath) :
        m_archivePath(archivePath)
    {
        const std::string& archivePathString = archivePath.string();
        if(std::filesystem::exists(archivePath))
        {
            m_zipFile = zipOpen(archivePathString.c_str(), APPEND_STATUS_ADDINZIP);
        }
        else
        {
            m_zipFile = zipOpen(archivePathString.c_str(), APPEND_STATUS_CREATE);
        }

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write("Archive '" + archivePathString + "' cannot be opened."),
            m_zipFile == nullptr
        );
    }

    /// Destructor
    ~ArchiveWriterImpl() override
    {
        if(m_zipFile != nullptr)
        {
            // Close the the zip archive.
            // Last parameter is the global comment which is unused for now.
            const int result = zipClose(m_zipFile, nullptr);

            SIGHT_THROW_EXCEPTION_IF(
                exception::Write("Archive '" + m_archivePath.string() + "' cannot be closed."),
                result != UNZ_OK
            );

            m_zipFile = nullptr;
        }

        // Remove cached element
        cache_clean(m_archivePath);
    }

    /// Returns a std::ostream to read an archived file
    /// @param filePath path of the file inside the archive.
    /// @param password the password needed to encrypt the file.
    /// @param method the compression algorithm to use.
    /// @param level the compression level to use.
    std::unique_ptr<std::ostream> openFile(
        const std::filesystem::path& filePath,
        const core::crypto::secure_string& password = "",
        const Method method                         = Method::ZSTD,
        const Level level                           = Level::DEFAULT
    ) override
    {
        // Boost iostreams Sink. It allows easy ostream implementation
        class ZipSink
        {
        public:

            typedef char char_type;
            typedef boost::iostreams::sink_tag category;

            /// Delete default constructor
            ZipSink() = delete;

            // Boost iostreams Sink do not like constructor with more than one parameter
            // So, we use a sub struct to hold parameters and class attributes.
            struct Parameters
            {
                const ArchiveWriterImpl::sptr m_archive;
                const std::filesystem::path m_filePath;
                const core::crypto::secure_string m_password;
                const int m_method;
                const int m_level;

                Parameters(
                    const ArchiveWriterImpl::sptr& archive,
                    const std::filesystem::path& filePath,
                    const core::crypto::secure_string& password,
                    const int m_method,
                    const int m_level
                ) :
                    m_archive(archive),
                    m_filePath(filePath),
                    m_password(password),
                    m_method(m_method),
                    m_level(m_level)
                {
                }
            };

            ZipSink(const std::shared_ptr<const Parameters>& parameters) :
                m_attributes(parameters),
                m_fileKeeper(std::make_shared<const ZipFileKeeper>(m_attributes)),
                m_lockGuard(std::make_shared<std::lock_guard<std::mutex> >(m_attributes->m_archive->m_operationMutex))
            {
            }

            //------------------------------------------------------------------------------

            std::streamsize write(const char* buffer, std::streamsize size)
            {
                // Write the data to the archived file
                const int result = zipWriteInFileInZip(
                    m_attributes->m_archive->m_zipFile,
                    buffer,
                    static_cast<std::uint32_t>(size)
                );

                SIGHT_THROW_EXCEPTION_IF(
                    exception::Write(
                        "Cannot write file '"
                        + m_attributes->m_filePath.string()
                        + "' in archive '"
                        + m_attributes->m_archive->m_archivePath.string()
                        + "'."
                    ),
                    result != ZIP_OK
                );

                return size;
            }

        private:

            // Used to create and destroy minizip file handle
            struct ZipFileKeeper
            {
                // Store constructor parameters
                const std::shared_ptr<const Parameters> m_attributes;

                ZipFileKeeper(const std::shared_ptr<const Parameters>& parameters) :
                    m_attributes(parameters)
                {
                    // Open / create the new file inside the archive
                    const int result = openNewFileInZip(
                        m_attributes->m_archive->m_zipFile,
                        m_attributes->m_filePath,
                        m_attributes->m_password,
                        m_attributes->m_method,
                        m_attributes->m_level
                    );

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Write(
                            "Cannot open file '"
                            + m_attributes->m_filePath.string()
                            + "' in archive '"
                            + m_attributes->m_archive->m_archivePath.string()
                            + "'."
                        ),
                        result != MZ_OK
                    );
                }

                ~ZipFileKeeper()
                {
                    // Do not forget to clean file handle in zip
                    const int result = zipCloseFileInZip(m_attributes->m_archive->m_zipFile);

                    SIGHT_THROW_EXCEPTION_IF(
                        exception::Write(
                            "Cannot close file '"
                            + m_attributes->m_filePath.string()
                            + "' in archive '"
                            + m_attributes->m_archive->m_archivePath.string()
                            + "'."
                        ),
                        result != MZ_OK
                    );
                }
            };

            // Store constructor parameters
            const std::shared_ptr<const Parameters> m_attributes;

            // Used to create and destroy minizip file handle
            const std::shared_ptr<const ZipFileKeeper> m_fileKeeper;

            // Locks the archive mutex so nobody could open another file.
            const std::shared_ptr<const std::lock_guard<std::mutex> > m_lockGuard;
        };

        // Translate to minizip dialect
        auto [minizipMethod, minizipLevel] = to_minizip_parameter(method, level);

        // ZipSink parameters
        auto parameters = std::make_shared<const ZipSink::Parameters>(
            ArchiveWriterImpl::dynamicCast(ArchiveWriterImpl::shared(m_archivePath)),
            filePath,
            password,
            minizipMethod,
            minizipLevel
        );

        // Creating a ZipSink also lock the archive mutex.
        // Due to its design, minizip only allows one archive with the same path with one file operation.
        return std::make_unique<boost::iostreams::stream<ZipSink> >(parameters);
    }

private:

    /// Internal class
    friend class ZipSink;

    /// Path of the archive file
    std::filesystem::path m_archivePath;

    /// Archive context handle
    zipFile m_zipFile {nullptr};

    /// Mutex to prevent multiple read/write operation on same file
    std::mutex m_operationMutex;
};

//------------------------------------------------------------------------------

ArchiveWriter::sptr ArchiveWriter::shared(const std::filesystem::path& archivePath)
{
    // First try the cache
    ArchiveWriter::sptr newArchive;
    Archive::sptr cachedArchive = cache_find(archivePath);

    if(cachedArchive)
    {
        // Try to cast in the correct type
        newArchive = ArchiveWriter::dynamicCast(cachedArchive);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write("Archive '" + archivePath.string() + "' is already opened in READ mode."),
            !newArchive
        );
    }

    // If null create a new one
    if(!newArchive)
    {
        // This is a trick to have access to a protected constructor and still allowed to use std::make_shared
        newArchive = std::make_shared<ArchiveWriterImpl>(archivePath);

        // Store the created archive for later use
        cache_store(archivePath, newArchive);
    }

    return newArchive;
}

} // namespace sight::io::zip
