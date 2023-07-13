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

#include "ArchiveWriter.hpp"

#include "exception/Write.hpp"

#include "minizip/mz.h"
#include "minizip/mz_os.h"
#include "minizip/mz_strm.h"
#include "minizip/mz_strm_os.h"
#include "minizip/mz_zip.h"
#include "minizip/mz_zip_rw.h"

#include <core/exceptionmacros.hpp>

#include <boost/iostreams/stream.hpp>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <tuple>

/**
 * Do not mark `madeby` as incorrect.
 * cspell:ignore madeby
 */
namespace sight::io::zip
{

namespace
{

// Force deflate if env variable is set
const bool s_legacy_compression =
    []
    {
        const char* const env_legacy_compression = std::getenv("SIGHT_LEGACY_COMPRESSION");

        return env_legacy_compression != nullptr && std::strncmp(env_legacy_compression, "1", 1) == 0;
    }();

/// Convert argument to minizip dialect
inline std::tuple<std::uint16_t, std::int16_t> toMinizipParameter(Method method, Level level)
{
    std::uint16_t minizipMethod = MZ_COMPRESS_METHOD_ZSTD;
    std::int16_t minizipLevel   = MZ_COMPRESS_LEVEL_DEFAULT;

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
            return toMinizipParameter(Method::ZSTD, level);
    }

    if(s_legacy_compression && minizipMethod != MZ_COMPRESS_METHOD_DEFLATE)
    {
        return toMinizipParameter(Method::DEFLATE, level);
    }

    return {minizipMethod, minizipLevel};
}

class RawArchiveWriter final : public ArchiveWriter
{
public:

    SIGHT_DECLARE_CLASS(RawArchiveWriter, ArchiveWriter);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    RawArchiveWriter()                                   = delete;
    RawArchiveWriter(const RawArchiveWriter&)            = delete;
    RawArchiveWriter(RawArchiveWriter&&)                 = delete;
    RawArchiveWriter& operator=(const RawArchiveWriter&) = delete;
    RawArchiveWriter& operator=(RawArchiveWriter&&)      = delete;

    explicit RawArchiveWriter(const std::filesystem::path& root) :
        ArchiveWriter(root),
        m_root(root)
    {
        // Create the sub directories if needed
        std::filesystem::create_directories(root);
    }

    ~RawArchiveWriter() override = default;

    //------------------------------------------------------------------------------

    std::unique_ptr<std::ostream> openFile(
        const std::filesystem::path& file_path,
        [[maybe_unused]] const core::crypto::secure_string& password = "",
        [[maybe_unused]] const Method method                         = Method::DEFAULT,
        [[maybe_unused]] const Level level                           = Level::DEFAULT
    ) override
    {
        // Create the sub directories if needed
        const std::filesystem::path& fullPath = m_root / file_path.relative_path();

        if(fullPath.has_parent_path())
        {
            std::filesystem::create_directories(fullPath.parent_path());
        }

        return std::make_unique<std::ofstream>(fullPath, std::ios::out | std::ios::binary | std::ios::trunc);
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

    inline ZipHandle(const std::filesystem::path& archive_path, const Archive::ArchiveFormat format) :
        m_archive_path(archive_path.string()),
        m_format(format)
    {
        // Create the sub directories if needed
        if(archive_path.has_parent_path())
        {
            std::filesystem::create_directories(archive_path.parent_path());
        }

        // Create zip writer instance
        mz_zip_writer_create(&m_zip_writer);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot create zip writer instance",
                MZ_MEM_ERROR
            ),
            m_zip_writer == nullptr
        );

        const Method default_method = m_format == Archive::ArchiveFormat::COMPATIBLE
                                      ? Method::DEFLATE
                                      : m_format == Archive::ArchiveFormat::OPTIMIZED
                                      ? Method::ZSTD
                                      : Method::DEFAULT;

        // Set default options
        auto [minizipMethod, minizipLevel] = toMinizipParameter(default_method, Level::DEFAULT);

        mz_zip_writer_set_compress_method(m_zip_writer, minizipMethod);
        mz_zip_writer_set_compress_level(m_zip_writer, minizipLevel);
        mz_zip_writer_set_zip_cd(
            m_zip_writer,
            s_legacy_compression || m_format == Archive::ArchiveFormat::COMPATIBLE ? 0 : 1
        );

        // Open the zip file
        auto result = mz_zip_writer_open_file(m_zip_writer, m_archive_path.c_str(), 0, 0);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot open archive '" + m_archive_path + "'. Error code: " + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    inline ~ZipHandle()
    {
        // Close zip handle
        const auto result = mz_zip_writer_close(m_zip_writer);

        // Cleanup
        mz_zip_writer_delete(&m_zip_writer);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
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

    // Default compression method
    const Archive::ArchiveFormat m_format {Archive::ArchiveFormat::DEFAULT};

    // Zip writer handle
    void* m_zip_writer {nullptr};
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
        core::crypto::secure_string password = "",
        const Method method                  = Method::DEFAULT,
        const Level level                    = Level::DEFAULT
    ) :
        m_fileName(file_path.string()),
        m_password(std::move(password)),
        m_zipHandle(std::move(zip_handle))
    {
        // Translate to minizip dialect
        auto [minizipMethod, minizipLevel] = toMinizipParameter(
            method != Method::DEFAULT
            ? method
            : m_zipHandle->m_format == Archive::ArchiveFormat::COMPATIBLE
            ? Method::DEFLATE
            : m_zipHandle->m_format == Archive::ArchiveFormat::OPTIMIZED
            ? Method::ZSTD
            : Method::DEFAULT,
            level
        );

        // Set compression method and level
        mz_zip_writer_set_compress_method(m_zipHandle->m_zip_writer, minizipMethod);
        mz_zip_writer_set_compress_level(m_zipHandle->m_zip_writer, minizipLevel);

        // Set encryption
        const bool use_encryption = !m_password.empty();
        mz_zip_writer_set_aes(m_zipHandle->m_zip_writer, use_encryption ? 1 : 0);
        mz_zip_writer_set_password(m_zipHandle->m_zip_writer, use_encryption ? m_password.c_str() : nullptr);

        const auto now = time(nullptr);

        mz_zip_file zip_file;
        std::memset(&zip_file, 0, sizeof(zip_file));

        zip_file.version_madeby     = MZ_VERSION_MADEBY;
        zip_file.flag               = MZ_ZIP_FLAG_UTF8;
        zip_file.compression_method = minizipMethod;
        zip_file.modified_date      = now;
        zip_file.accessed_date      = now;
        zip_file.creation_date      = now;
        zip_file.filename           = m_fileName.c_str();

        if(use_encryption)
        {
            zip_file.aes_version         = MZ_AES_VERSION;
            zip_file.aes_encryption_mode = MZ_AES_ENCRYPTION_MODE_256;
        }

        const auto result = mz_zip_writer_entry_open(m_zipHandle->m_zip_writer, &zip_file);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot write file '"
                + m_fileName
                + "' in archive '"
                + m_zipHandle->m_archive_path
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    inline ~ZipFileHandle()
    {
        const auto result = mz_zip_writer_entry_close(m_zipHandle->m_zip_writer);

        // Restore defaults
        mz_zip_writer_set_compress_method(m_zipHandle->m_zip_writer, MZ_COMPRESS_METHOD_ZSTD);
        mz_zip_writer_set_compress_level(m_zipHandle->m_zip_writer, MZ_COMPRESS_LEVEL_DEFAULT);
        mz_zip_writer_set_aes(m_zipHandle->m_zip_writer, 0);
        mz_zip_writer_set_password(m_zipHandle->m_zip_writer, nullptr);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot close file '"
                + m_fileName
                + "' in archive '"
                + m_zipHandle->m_archive_path
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

private:

    friend class ZipSink;

    // Path to the file converted to string because on Windows std::filesystem::path.c_str() returns a wchar*
    const std::string m_fileName;

    // The password must be kept alive since minizip doesn't copy it
    const core::crypto::secure_string m_password;

    // Zip handles pack which contains the zip writer
    const std::shared_ptr<ZipHandle> m_zipHandle;
};

class ZipSink final
{
public:

    // Needed by Boost
    using char_type = char;
    using category  = boost::iostreams::sink_tag;

    // BEWARE: Boost make shallow copies of the ZipSink...
    explicit ZipSink(std::shared_ptr<ZipFileHandle> zip_file_handle) :
        m_zipFileHandle(std::move(zip_file_handle))
    {
    }

    // Boost use this to write things
    std::streamsize write(const char* buffer, std::streamsize size)
    {
        const auto written = mz_zip_writer_entry_write(
            m_zipFileHandle->m_zipHandle->m_zip_writer,
            buffer,
            std::int32_t(size)
        );

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot write in file '"
                + m_zipFileHandle->m_fileName
                + "' in archive '"
                + m_zipFileHandle->m_zipHandle->m_archive_path
                + "'. Error code: "
                + std::to_string(written),
                written
            ),
            written < 0
        );

        return written;
    }

private:

    const std::shared_ptr<ZipFileHandle> m_zipFileHandle;
};

class ZipArchiveWriter final : public ArchiveWriter
{
public:

    SIGHT_DECLARE_CLASS(ZipArchiveWriter, ArchiveWriter);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    ZipArchiveWriter()                                   = delete;
    ZipArchiveWriter(const ZipArchiveWriter&)            = delete;
    ZipArchiveWriter(ZipArchiveWriter&&)                 = delete;
    ZipArchiveWriter& operator=(const ZipArchiveWriter&) = delete;
    ZipArchiveWriter& operator=(ZipArchiveWriter&&)      = delete;

    inline ZipArchiveWriter(const std::filesystem::path& archive_path, const ArchiveFormat format) :
        ArchiveWriter(archive_path),
        m_zipHandle(std::make_shared<ZipHandle>(archive_path, format))
    {
    }

    ~ZipArchiveWriter() override = default;

    //------------------------------------------------------------------------------

    inline std::unique_ptr<std::ostream> openFile(
        const std::filesystem::path& file_path,
        const core::crypto::secure_string& password = "",
        const Method method                         = Method::DEFAULT,
        const Level level                           = Level::DEFAULT
    ) override
    {
        const auto zip_file_handle = std::make_shared<ZipFileHandle>(
            m_zipHandle,
            file_path,
            password,
            method,
            level
        );

        return std::make_unique<boost::iostreams::stream<ZipSink> >(zip_file_handle);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] inline bool isRaw() const override
    {
        return false;
    }

private:

    std::shared_ptr<ZipHandle> m_zipHandle;
};

} // namespace

ArchiveWriter::ArchiveWriter(const std::filesystem::path& archive_path) :
    Archive(archive_path)
{
}

//------------------------------------------------------------------------------

ArchiveWriter::uptr ArchiveWriter::get(
    const std::filesystem::path& archivePath,
    const ArchiveFormat format
)
{
    if(format == ArchiveFormat::FILESYSTEM)
    {
        return std::make_unique<RawArchiveWriter>(archivePath);
    }

    return std::make_unique<ZipArchiveWriter>(archivePath, format);
}

} // namespace sight::io::zip
