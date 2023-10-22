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
inline std::tuple<std::uint16_t, std::int16_t> to_minizip_parameter(Method _method, Level _level)
{
    std::uint16_t minizip_method = MZ_COMPRESS_METHOD_ZSTD;
    std::int16_t minizip_level   = MZ_COMPRESS_LEVEL_DEFAULT;

    switch(_method)
    {
        case Method::STORE:
            minizip_method = MZ_COMPRESS_METHOD_STORE;
            minizip_level  = MZ_COMPRESS_LEVEL_DEFAULT;

            break;

        case Method::DEFLATE:
            minizip_method = MZ_COMPRESS_METHOD_DEFLATE;

            switch(_level)
            {
                case Level::BEST:
                case Level::ULTRA:
                    minizip_level = MZ_COMPRESS_LEVEL_BEST;
                    break;

                case Level::FAST:
                    minizip_level = MZ_COMPRESS_LEVEL_FAST;
                    break;

                default:
                    minizip_level = MZ_COMPRESS_LEVEL_DEFAULT;
                    break;
            }

            break;

        case Method::ZSTD:
            minizip_method = MZ_COMPRESS_METHOD_ZSTD;

            switch(_level)
            {
                case Level::ULTRA:
                    minizip_level = 22;
                    break;

                case Level::BEST:
                    minizip_level = 19;
                    break;

                case Level::FAST:
                    minizip_level = 1;
                    break;

                default:
                    minizip_level = 5;
                    break;
            }

            break;

        default:
            return to_minizip_parameter(Method::ZSTD, _level);
    }

    if(s_legacy_compression && minizip_method != MZ_COMPRESS_METHOD_DEFLATE)
    {
        return to_minizip_parameter(Method::DEFLATE, _level);
    }

    return {minizip_method, minizip_level};
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

    explicit RawArchiveWriter(const std::filesystem::path& _root) :
        ArchiveWriter(_root),
        m_ROOT(_root)
    {
        // Create the sub directories if needed
        std::filesystem::create_directories(_root);
    }

    ~RawArchiveWriter() override = default;

    //------------------------------------------------------------------------------

    std::unique_ptr<std::ostream> openFile(
        const std::filesystem::path& _file_path,
        [[maybe_unused]] const core::crypto::secure_string& _password = "",
        [[maybe_unused]] const Method _method                         = Method::DEFAULT,
        [[maybe_unused]] const Level _level                           = Level::DEFAULT
    ) override
    {
        // Create the sub directories if needed
        const std::filesystem::path& full_path = m_ROOT / _file_path.relative_path();

        if(full_path.has_parent_path())
        {
            std::filesystem::create_directories(full_path.parent_path());
        }

        return std::make_unique<std::ofstream>(full_path, std::ios::out | std::ios::binary | std::ios::trunc);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool isRaw() const override
    {
        return true;
    }

private:

    /// Path of the root directory
    const std::filesystem::path m_ROOT;
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

    inline ZipHandle(const std::filesystem::path& _archive_path, const Archive::ArchiveFormat _format) :
        M_ARCHIVE_PATH(_archive_path.string()),
        M_FORMAT(_format),
        m_zip_writer(mz_zip_writer_create())
    {
        // Create the sub directories if needed
        if(_archive_path.has_parent_path())
        {
            std::filesystem::create_directories(_archive_path.parent_path());
        }

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot create zip writer instance",
                MZ_MEM_ERROR
            ),
            m_zip_writer == nullptr
        );

        const Method default_method = M_FORMAT == Archive::ArchiveFormat::COMPATIBLE
                                      ? Method::DEFLATE
                                      : M_FORMAT == Archive::ArchiveFormat::OPTIMIZED
                                      ? Method::ZSTD
                                      : Method::DEFAULT;

        // Set default options
        auto [minizipMethod, minizipLevel] = to_minizip_parameter(default_method, Level::DEFAULT);

        mz_zip_writer_set_compress_method(m_zip_writer, minizipMethod);
        mz_zip_writer_set_compress_level(m_zip_writer, minizipLevel);
        mz_zip_writer_set_zip_cd(
            m_zip_writer,
            s_legacy_compression || M_FORMAT == Archive::ArchiveFormat::COMPATIBLE ? 0 : 1
        );

        // Open the zip file
        auto result = mz_zip_writer_open_file(m_zip_writer, M_ARCHIVE_PATH.c_str(), 0, 0);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot open archive '" + M_ARCHIVE_PATH + "'. Error code: " + std::to_string(result),
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
                + M_ARCHIVE_PATH
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    // Path to the archive converted to string because on Windows std::filesystem::path.c_str() returns a wchar*
    const std::string M_ARCHIVE_PATH;

    // Default compression method
    const Archive::ArchiveFormat M_FORMAT {Archive::ArchiveFormat::DEFAULT};

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
        std::shared_ptr<ZipHandle> _zip_handle,
        const std::filesystem::path& _file_path,
        core::crypto::secure_string _password = "",
        const Method _method                  = Method::DEFAULT,
        const Level _level                    = Level::DEFAULT
    ) :
        m_FILE_NAME(_file_path.string()),
        m_PASSWORD(std::move(_password)),
        m_ZIP_HANDLE(std::move(_zip_handle))
    {
        // Translate to minizip dialect
        auto [minizipMethod, minizipLevel] = to_minizip_parameter(
            _method != Method::DEFAULT
            ? _method
            : m_ZIP_HANDLE->M_FORMAT == Archive::ArchiveFormat::COMPATIBLE
            ? Method::DEFLATE
            : m_ZIP_HANDLE->M_FORMAT == Archive::ArchiveFormat::OPTIMIZED
            ? Method::ZSTD
            : Method::DEFAULT,
            _level
        );

        // Set compression method and level
        mz_zip_writer_set_compress_method(m_ZIP_HANDLE->m_zip_writer, minizipMethod);
        mz_zip_writer_set_compress_level(m_ZIP_HANDLE->m_zip_writer, minizipLevel);

        // Set encryption
        const bool use_encryption = !m_PASSWORD.empty();
        mz_zip_writer_set_aes(m_ZIP_HANDLE->m_zip_writer, use_encryption ? 1 : 0);
        mz_zip_writer_set_password(m_ZIP_HANDLE->m_zip_writer, use_encryption ? m_PASSWORD.c_str() : nullptr);

        const auto now = time(nullptr);

        mz_zip_file zip_file;
        std::memset(&zip_file, 0, sizeof(zip_file));

        zip_file.version_madeby     = MZ_VERSION_MADEBY;
        zip_file.flag               = MZ_ZIP_FLAG_UTF8;
        zip_file.compression_method = minizipMethod;
        zip_file.modified_date      = now;
        zip_file.accessed_date      = now;
        zip_file.creation_date      = now;
        zip_file.filename           = m_FILE_NAME.c_str();

        if(use_encryption)
        {
            zip_file.aes_version  = MZ_AES_VERSION;
            zip_file.aes_strength = MZ_AES_STRENGTH_256;
        }

        const auto result = mz_zip_writer_entry_open(m_ZIP_HANDLE->m_zip_writer, &zip_file);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot write file '"
                + m_FILE_NAME
                + "' in archive '"
                + m_ZIP_HANDLE->M_ARCHIVE_PATH
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    inline ~ZipFileHandle()
    {
        const auto result = mz_zip_writer_entry_close(m_ZIP_HANDLE->m_zip_writer);

        // Restore defaults
        mz_zip_writer_set_compress_method(m_ZIP_HANDLE->m_zip_writer, MZ_COMPRESS_METHOD_ZSTD);
        mz_zip_writer_set_compress_level(m_ZIP_HANDLE->m_zip_writer, MZ_COMPRESS_LEVEL_DEFAULT);
        mz_zip_writer_set_aes(m_ZIP_HANDLE->m_zip_writer, 0);
        mz_zip_writer_set_password(m_ZIP_HANDLE->m_zip_writer, nullptr);

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot close file '"
                + m_FILE_NAME
                + "' in archive '"
                + m_ZIP_HANDLE->M_ARCHIVE_PATH
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
    const std::string m_FILE_NAME;

    // The password must be kept alive since minizip doesn't copy it
    const core::crypto::secure_string m_PASSWORD;

    // Zip handles pack which contains the zip writer
    const std::shared_ptr<ZipHandle> m_ZIP_HANDLE;
};

class ZipSink final
{
public:

    // Needed by Boost
    using char_type = char;
    using category  = boost::iostreams::sink_tag;

    // BEWARE: Boost make shallow copies of the ZipSink...
    explicit ZipSink(std::shared_ptr<ZipFileHandle> _zip_file_handle) :
        m_ZIP_FILE_HANDLE(std::move(_zip_file_handle))
    {
    }

    // Boost use this to write things
    std::streamsize write(const char* _buffer, std::streamsize _size)
    {
        const auto written = mz_zip_writer_entry_write(
            m_ZIP_FILE_HANDLE->m_ZIP_HANDLE->m_zip_writer,
            _buffer,
            std::int32_t(_size)
        );

        SIGHT_THROW_EXCEPTION_IF(
            exception::Write(
                "Cannot write in file '"
                + m_ZIP_FILE_HANDLE->m_FILE_NAME
                + "' in archive '"
                + m_ZIP_FILE_HANDLE->m_ZIP_HANDLE->M_ARCHIVE_PATH
                + "'. Error code: "
                + std::to_string(written),
                written
            ),
            written < 0
        );

        return written;
    }

private:

    const std::shared_ptr<ZipFileHandle> m_ZIP_FILE_HANDLE;
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

    inline ZipArchiveWriter(const std::filesystem::path& _archive_path, const ArchiveFormat _format) :
        ArchiveWriter(_archive_path),
        m_zipHandle(std::make_shared<ZipHandle>(_archive_path, _format))
    {
    }

    ~ZipArchiveWriter() override = default;

    //------------------------------------------------------------------------------

    inline std::unique_ptr<std::ostream> openFile(
        const std::filesystem::path& _file_path,
        const core::crypto::secure_string& _password = "",
        const Method _method                         = Method::DEFAULT,
        const Level _level                           = Level::DEFAULT
    ) override
    {
        const auto zip_file_handle = std::make_shared<ZipFileHandle>(
            m_zipHandle,
            _file_path,
            _password,
            _method,
            _level
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

ArchiveWriter::ArchiveWriter(const std::filesystem::path& _archive_path) :
    Archive(_archive_path)
{
}

//------------------------------------------------------------------------------

ArchiveWriter::uptr ArchiveWriter::get(
    const std::filesystem::path& _archive_path,
    const ArchiveFormat _format
)
{
    if(_format == ArchiveFormat::FILESYSTEM)
    {
        return std::make_unique<RawArchiveWriter>(_archive_path);
    }

    return std::make_unique<ZipArchiveWriter>(_archive_path, _format);
}

} // namespace sight::io::zip
