/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "core/crypto/secure_string.hpp"

#include "exception/Read.hpp"

#include "minizip/mz.h"
#include "minizip/mz_os.h"
#include "minizip/mz_strm.h"
#include "minizip/mz_strm_os.h"
#include "minizip/mz_zip.h"
#include "minizip/mz_zip_rw.h"

#include <core/exceptionmacros.hpp>

#include <boost/iostreams/stream.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace sight::io::zip
{

namespace
{

class RawArchiveReader final : public ArchiveReader
{
public:

    SIGHT_DECLARE_CLASS(RawArchiveReader, ArchiveReader);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    RawArchiveReader()                                   = delete;
    RawArchiveReader(const RawArchiveReader&)            = delete;
    RawArchiveReader(RawArchiveReader&&)                 = delete;
    RawArchiveReader& operator=(const RawArchiveReader&) = delete;
    RawArchiveReader& operator=(RawArchiveReader&&)      = delete;

    /// Constructor. It open the archive and create all resources needed to access it.
    /// @param archivePath path of the archive file. The file will be kept opened as long as the instance lives.
    explicit RawArchiveReader(const std::filesystem::path& root) :
        ArchiveReader(root),
        m_root(root)
    {
    }

    ~RawArchiveReader() override = default;

    //------------------------------------------------------------------------------

    std::unique_ptr<std::istream> openFile(
        const std::filesystem::path& filePath,
        [[maybe_unused]] const core::crypto::secure_string& password = ""
    ) override
    {
        return std::make_unique<std::ifstream>(m_root / filePath.relative_path(), std::ios::in | std::ios::binary);
    }

    //------------------------------------------------------------------------------

    void extractAllTo(const std::filesystem::path& outputPath, const core::crypto::secure_string& /*password*/) override
    {
        std::filesystem::copy(m_root, outputPath, std::filesystem::copy_options::recursive);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool isRaw() const override
    {
        return true;
    }

private:

    /// Path of the root directory
    const std::filesystem::path m_root;
};

class ZipHandle final
{
public:

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    ZipHandle()                            = delete;
    ZipHandle(const ZipHandle&)            = delete;
    ZipHandle(ZipHandle&&)                 = delete;
    ZipHandle& operator=(const ZipHandle&) = delete;
    ZipHandle& operator=(ZipHandle&&)      = delete;

    inline explicit ZipHandle(const std::filesystem::path& archive_path) :
        m_archive_path(archive_path.string())
    {
        // Create zip reader instance
        mz_zip_reader_create(&m_zip_reader);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read(
                "Cannot create zip reader instance",
                MZ_MEM_ERROR
            ),
            m_zip_reader == nullptr
        );

        const auto result = mz_zip_reader_open_file(m_zip_reader, m_archive_path.c_str());

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read(
                "Cannot open archive '" + m_archive_path + "'. Error code: " + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    inline ~ZipHandle()
    {
        // Close zip handle
        const auto result = mz_zip_reader_close(m_zip_reader);

        // Cleanup
        mz_zip_reader_delete(&m_zip_reader);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read(
                "Cannot close writer for archive '"
                + m_archive_path
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    // Path to the archive converted to string because on Windows std::filesystem::path.c_str() returns a wchar*
    const std::string m_archive_path;

    // Zip writer handle
    void* m_zip_reader {nullptr};
};

class ZipFileHandle final
{
public:

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    ZipFileHandle()                                = delete;
    ZipFileHandle(const ZipFileHandle&)            = delete;
    ZipFileHandle(ZipFileHandle&&)                 = delete;
    ZipFileHandle& operator=(const ZipFileHandle&) = delete;
    ZipFileHandle& operator=(ZipFileHandle&&)      = delete;

    inline ZipFileHandle(
        std::shared_ptr<ZipHandle> zip_handle,
        const std::filesystem::path& file_path,
        core::crypto::secure_string password = ""
    ) :
        m_filePath(file_path.string()),
        m_password(std::move(password)),
        m_zipHandle(std::move(zip_handle))
    {
        // Set encryption
        mz_zip_reader_set_password(m_zipHandle->m_zip_reader, m_password.empty() ? nullptr : m_password.c_str());

        if(const auto result = mz_zip_reader_locate_entry(m_zipHandle->m_zip_reader, m_filePath.c_str(), 0);
           result != MZ_OK)
        {
            SIGHT_THROW_EXCEPTION(
                exception::Read(
                    "Cannot locate file '"
                    + m_filePath
                    + "' in archive '"
                    + m_zipHandle->m_archive_path
                    + "'. Error code: "
                    + std::to_string(result),
                    result
                )
            );
        }

        // NOLINTBEGIN(readability-else-after-return)
        if(const auto result = mz_zip_reader_entry_open(m_zipHandle->m_zip_reader);
           result == MZ_PASSWORD_ERROR)
        {
            const auto& message =
                "File '"
                + m_filePath
                + "' from archive '"
                + m_zipHandle->m_archive_path
                + "' is password protected and the provided one does not match. Error code: "
                + std::to_string(result);

            // Log the exception as debug, as it is expected to happen when trying to open a password protected file
            SIGHT_DEBUG(message);

            // Do not use SIGHT_THROW_EXCEPTION to avoid polluting the logs
            throw(exception::BadPassword(message, result));
        }
        else if(result != MZ_OK)
        {
            SIGHT_THROW_EXCEPTION(
                exception::Read(
                    "Cannot open file '"
                    + m_filePath
                    + "' from archive '"
                    + m_zipHandle->m_archive_path
                    + "'. Error code: "
                    + std::to_string(result),
                    result
                )
            );
        }

        // NOLINTEND(readability-else-after-return)
    }

    inline ~ZipFileHandle()
    {
        const auto result = mz_zip_reader_entry_close(m_zipHandle->m_zip_reader);

        // Restore defaults
        mz_zip_reader_set_password(m_zipHandle->m_zip_reader, nullptr);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read(
                "Cannot close file '"
                + m_filePath
                + "' from archive '"
                + m_zipHandle->m_archive_path
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

private:

    friend class ZipSource;

    // Path to the file converted to string because on Windows std::filesystem::path.c_str() returns a wchar*
    const std::string m_filePath;

    // The password must be kept alive since minizip doesn't copy it
    const core::crypto::secure_string m_password;

    // Zip handles pack which contains the zip writer
    const std::shared_ptr<ZipHandle> m_zipHandle;
};

class ZipSource final
{
public:

    // Needed by Boost
    using char_type = char;
    using category  = boost::iostreams::source_tag;

    // BEWARE: Boost make shallow copies of the ZipSource...
    explicit ZipSource(std::shared_ptr<ZipFileHandle> zip_file_handle) :
        m_zipFileHandle(std::move(zip_file_handle))
    {
    }

    // Boost use this to read things
    std::streamsize read(char* buffer, std::streamsize size)
    {
        const auto read = mz_zip_reader_entry_read(
            m_zipFileHandle->m_zipHandle->m_zip_reader,
            buffer,
            std::int32_t(size)
        );

        SIGHT_THROW_EXCEPTION_IF(
            exception::Read(
                "Cannot read in file '"
                + m_zipFileHandle->m_filePath
                + "' in archive '"
                + m_zipFileHandle->m_zipHandle->m_archive_path
                + "'. Error code: "
                + std::to_string(read),
                read
            ),
            read < 0
        );

        return read;
    }

private:

    const std::shared_ptr<ZipFileHandle> m_zipFileHandle;
};

class ZipArchiveReader final : public ArchiveReader
{
public:

    SIGHT_DECLARE_CLASS(ZipArchiveReader, ArchiveReader);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    ZipArchiveReader()                                   = delete;
    ZipArchiveReader(const ZipArchiveReader&)            = delete;
    ZipArchiveReader(ZipArchiveReader&&)                 = delete;
    ZipArchiveReader& operator=(const ZipArchiveReader&) = delete;
    ZipArchiveReader& operator=(ZipArchiveReader&&)      = delete;

    inline explicit ZipArchiveReader(const std::filesystem::path& archive_path) :
        ArchiveReader(archive_path),
        m_zipHandle(std::make_shared<ZipHandle>(archive_path))
    {
    }

    ~ZipArchiveReader() override = default;

    //------------------------------------------------------------------------------

    inline std::unique_ptr<std::istream> openFile(
        const std::filesystem::path& file_path,
        const core::crypto::secure_string& password = ""
    ) override
    {
        const auto zip_file_handle = std::make_shared<ZipFileHandle>(
            m_zipHandle,
            file_path,
            password
        );

        return std::make_unique<boost::iostreams::stream<ZipSource> >(zip_file_handle);
    }

    //------------------------------------------------------------------------------

    inline void extractAllTo(
        const std::filesystem::path& outputPath,
        const core::crypto::secure_string& password
    ) override
    {
        mz_zip_reader_set_password(m_zipHandle->m_zip_reader, password.empty() ? nullptr : password.c_str());
        std::vector<std::filesystem::path> extractedFiles;
        mz_zip_reader_set_entry_cb(
            m_zipHandle->m_zip_reader,
            &extractedFiles,
            [](void*, void* extractedFilesPtr, mz_zip_file*, const char* path)
                {
                    auto* files = reinterpret_cast<std::vector<std::filesystem::path>*>(extractedFilesPtr);
                    files->emplace_back(path);
                    return MZ_OK;
                });

        // NOLINTBEGIN(readability-else-after-return)
        if(const auto result = mz_zip_reader_save_all(m_zipHandle->m_zip_reader, outputPath.string().c_str());
           result == MZ_PASSWORD_ERROR)
        {
            SIGHT_THROW_EXCEPTION(
                exception::BadPassword(
                    "Archive '"
                    + m_zipHandle->m_archive_path
                    + "' is password protected and the provided one does not match. Error code: "
                    + std::to_string(result),
                    result
                )
            );
        }
        else if(result == MZ_DATA_ERROR && password.empty())
        {
            throw exception::BadPassword(
                      "Cannot extract archive '" + m_zipHandle->m_archive_path + "', it is probably password protected. Error code: " + std::to_string(
                          result
                      ),
                      result
            );
        }
        else if(result != MZ_OK)
        {
            SIGHT_THROW_EXCEPTION(
                exception::Read(
                    "Cannot extract archive '"
                    + m_zipHandle->m_archive_path
                    + "'. Error code: "
                    + std::to_string(result),
                    result
                )
            );
        }

        // NOLINTEND(readability-else-after-return)

        mz_zip_reader_set_password(m_zipHandle->m_zip_reader, nullptr);
        mz_zip_reader_set_entry_cb(m_zipHandle->m_zip_reader, nullptr, nullptr);

        // For some reasons, minizip saves the output files without any permissions.
        // Set user read and user write at the very least.
        std::ranges::for_each(
            extractedFiles,
            [](const std::filesystem::path& entry)
                {
                    std::filesystem::permissions(
                        entry,
                        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
                        std::filesystem::perm_options::add
                    );
                });
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline bool isRaw() const override
    {
        return false;
    }

private:

    std::shared_ptr<ZipHandle> m_zipHandle;
};

} // anonymous namespace

ArchiveReader::ArchiveReader(const std::filesystem::path& archive_path) :
    Archive(archive_path)
{
}

//------------------------------------------------------------------------------

ArchiveReader::uptr ArchiveReader::get(
    const std::filesystem::path& archivePath,
    const ArchiveFormat format
)
{
    if(format == ArchiveFormat::FILESYSTEM)
    {
        return std::make_unique<RawArchiveReader>(archivePath);
    }

    return std::make_unique<ZipArchiveReader>(archivePath);
}

} // namespace sight::io::zip
