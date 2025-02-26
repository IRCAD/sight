/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include "archive_writer.hpp"

#include "exception/write.hpp"

#include "minizip/mz.h"
#include "minizip/mz_os.h"
#include "minizip/mz_strm.h"
#include "minizip/mz_strm_os.h"
#include "minizip/mz_zip.h"
#include "minizip/mz_zip_rw.h"

#include <core/exceptionmacros.hpp>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4702)
#endif

#include <boost/iostreams/stream.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <cstdlib>
#include <cstring>
#include <fstream>
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
const bool LEGACY_COMPRESSION =
    []
    {
        const char* const env_legacy_compression = std::getenv("SIGHT_LEGACY_COMPRESSION");

        return env_legacy_compression != nullptr && std::strncmp(env_legacy_compression, "1", 1) == 0;
    }();

/// Convert argument to minizip dialect
inline std::tuple<std::uint16_t, std::int16_t> to_minizip_parameter(method _method, level _level)
{
    std::uint16_t minizip_method = MZ_COMPRESS_METHOD_ZSTD;
    std::int16_t minizip_level   = MZ_COMPRESS_LEVEL_DEFAULT;

    switch(_method)
    {
        case method::store:
            minizip_method = MZ_COMPRESS_METHOD_STORE;
            minizip_level  = MZ_COMPRESS_LEVEL_DEFAULT;

            break;

        case method::deflate:
            minizip_method = MZ_COMPRESS_METHOD_DEFLATE;

            switch(_level)
            {
                case level::best:
                case level::ultra:
                    minizip_level = MZ_COMPRESS_LEVEL_BEST;
                    break;

                case level::fast:
                    minizip_level = MZ_COMPRESS_LEVEL_FAST;
                    break;

                default:
                    minizip_level = MZ_COMPRESS_LEVEL_DEFAULT;
                    break;
            }

            break;

        case method::zstd:
            minizip_method = MZ_COMPRESS_METHOD_ZSTD;

            switch(_level)
            {
                case level::ultra:
                    minizip_level = 22;
                    break;

                case level::best:
                    minizip_level = 19;
                    break;

                case level::fast:
                    minizip_level = 1;
                    break;

                default:
                    minizip_level = 5;
                    break;
            }

            break;

        default:
            return to_minizip_parameter(method::zstd, _level);
    }

    if(LEGACY_COMPRESSION && minizip_method != MZ_COMPRESS_METHOD_DEFLATE)
    {
        return to_minizip_parameter(method::deflate, _level);
    }

    return {minizip_method, minizip_level};
}

class raw_archive_writer final : public archive_writer
{
public:

    SIGHT_DECLARE_CLASS(raw_archive_writer, archive_writer);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    raw_archive_writer()                                     = delete;
    raw_archive_writer(const raw_archive_writer&)            = delete;
    raw_archive_writer(raw_archive_writer&&)                 = delete;
    raw_archive_writer& operator=(const raw_archive_writer&) = delete;
    raw_archive_writer& operator=(raw_archive_writer&&)      = delete;

    explicit raw_archive_writer(const std::filesystem::path& _root) :
        archive_writer(_root),
        m_root(_root)
    {
        // Create the sub directories if needed
        std::filesystem::create_directories(_root);
    }

    ~raw_archive_writer() override = default;

    //------------------------------------------------------------------------------

    std::unique_ptr<std::ostream> open_file(
        const std::filesystem::path& _file_path,
        [[maybe_unused]] const core::crypto::secure_string& _password = "",
        [[maybe_unused]] const method _method                         = method::DEFAULT,
        [[maybe_unused]] const level _level                           = level::DEFAULT
    ) override
    {
        // Create the sub directories if needed
        const std::filesystem::path& full_path = m_root / _file_path.relative_path();

        if(full_path.has_parent_path())
        {
            std::filesystem::create_directories(full_path.parent_path());
        }

        return std::make_unique<std::ofstream>(full_path, std::ios::out | std::ios::binary | std::ios::trunc);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool is_raw() const override
    {
        return true;
    }

private:

    /// Path of the root directory
    const std::filesystem::path m_root;
};

class zip_handle final
{
public:

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    zip_handle()                             = delete;
    zip_handle(const zip_handle&)            = delete;
    zip_handle(zip_handle&&)                 = delete;
    zip_handle& operator=(const zip_handle&) = delete;
    zip_handle& operator=(zip_handle&&)      = delete;

    zip_handle(const std::filesystem::path& _archive_path, const archive::archive_format _format) :
        m_archive_path(_archive_path.string()),
        m_format(_format),
        m_zip_writer(mz_zip_writer_create())
    {
        // Create the sub directories if needed
        if(_archive_path.has_parent_path())
        {
            std::filesystem::create_directories(_archive_path.parent_path());
        }

        SIGHT_THROW_EXCEPTION_IF(
            exception::write(
                "Cannot create zip writer instance",
                MZ_MEM_ERROR
            ),
            m_zip_writer == nullptr
        );

        const method default_method = m_format == archive::archive_format::compatible
                                      ? method::deflate
                                      : m_format == archive::archive_format::optimized
                                      ? method::zstd
                                      : method::DEFAULT;

        // Set default options
        auto [minizipMethod, minizipLevel] = to_minizip_parameter(default_method, level::DEFAULT);

        mz_zip_writer_set_compress_method(m_zip_writer, minizipMethod);
        mz_zip_writer_set_compress_level(m_zip_writer, minizipLevel);
        mz_zip_writer_set_zip_cd(
            m_zip_writer,
            LEGACY_COMPRESSION || m_format == archive::archive_format::compatible ? 0 : 1
        );

        // Open the zip file
        auto result = mz_zip_writer_open_file(m_zip_writer, m_archive_path.c_str(), 0, 0);

        SIGHT_THROW_EXCEPTION_IF(
            exception::write(
                "Cannot open archive '" + m_archive_path + "'. Error code: " + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    ~zip_handle()
    {
        // Close zip handle
        const auto result = mz_zip_writer_close(m_zip_writer);

        // Cleanup
        mz_zip_writer_delete(&m_zip_writer);

        SIGHT_THROW_EXCEPTION_IF(
            exception::write(
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
    const archive::archive_format m_format {archive::archive_format::DEFAULT};

    // Zip writer handle
    void* m_zip_writer {nullptr};
};

class zip_file_handle final
{
public:

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    zip_file_handle()                                  = delete;
    zip_file_handle(const zip_file_handle&)            = delete;
    zip_file_handle(zip_file_handle&&)                 = delete;
    zip_file_handle& operator=(const zip_file_handle&) = delete;
    zip_file_handle& operator=(zip_file_handle&&)      = delete;

    zip_file_handle(
        std::shared_ptr<zip_handle> _zip_handle,
        const std::filesystem::path& _file_path,
        core::crypto::secure_string _password = "",
        const method _method                  = method::DEFAULT,
        const level _level                    = level::DEFAULT
    ) :
        m_file_name(_file_path.string()),
        m_password(std::move(_password)),
        m_zip_handle(std::move(_zip_handle))
    {
        // Translate to minizip dialect
        auto [minizipMethod, minizipLevel] = to_minizip_parameter(
            _method != method::DEFAULT
            ? _method
            : m_zip_handle->m_format == archive::archive_format::compatible
            ? method::deflate
            : m_zip_handle->m_format == archive::archive_format::optimized
            ? method::zstd
            : method::DEFAULT,
            _level
        );

        // Set compression method and level
        mz_zip_writer_set_compress_method(m_zip_handle->m_zip_writer, minizipMethod);
        mz_zip_writer_set_compress_level(m_zip_handle->m_zip_writer, minizipLevel);

        // Set encryption
        const bool use_encryption = !m_password.empty();
        mz_zip_writer_set_aes(m_zip_handle->m_zip_writer, use_encryption ? 1 : 0);
        mz_zip_writer_set_password(m_zip_handle->m_zip_writer, use_encryption ? m_password.c_str() : nullptr);

        const auto now = time(nullptr);

        mz_zip_file zip_file;
        std::memset(&zip_file, 0, sizeof(zip_file));

        zip_file.version_madeby     = MZ_VERSION_MADEBY;
        zip_file.flag               = MZ_ZIP_FLAG_UTF8;
        zip_file.compression_method = minizipMethod;
        zip_file.modified_date      = now;
        zip_file.accessed_date      = now;
        zip_file.creation_date      = now;
        zip_file.filename           = m_file_name.c_str();

        if(use_encryption)
        {
            zip_file.aes_version  = MZ_AES_VERSION;
            zip_file.aes_strength = MZ_AES_STRENGTH_256;
        }

        const auto result = mz_zip_writer_entry_open(m_zip_handle->m_zip_writer, &zip_file);

        SIGHT_THROW_EXCEPTION_IF(
            exception::write(
                "Cannot write file '"
                + m_file_name
                + "' in archive '"
                + m_zip_handle->m_archive_path
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    ~zip_file_handle()
    {
        const auto result = mz_zip_writer_entry_close(m_zip_handle->m_zip_writer);

        // Restore defaults
        mz_zip_writer_set_compress_method(m_zip_handle->m_zip_writer, MZ_COMPRESS_METHOD_ZSTD);
        mz_zip_writer_set_compress_level(m_zip_handle->m_zip_writer, MZ_COMPRESS_LEVEL_DEFAULT);
        mz_zip_writer_set_aes(m_zip_handle->m_zip_writer, 0);
        mz_zip_writer_set_password(m_zip_handle->m_zip_writer, nullptr);

        SIGHT_THROW_EXCEPTION_IF(
            exception::write(
                "Cannot close file '"
                + m_file_name
                + "' in archive '"
                + m_zip_handle->m_archive_path
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

private:

    friend class zip_sink;

    // Path to the file converted to string because on Windows std::filesystem::path.c_str() returns a wchar*
    const std::string m_file_name;

    // The password must be kept alive since minizip doesn't copy it
    const core::crypto::secure_string m_password;

    // Zip handles pack which contains the zip writer
    const std::shared_ptr<zip_handle> m_zip_handle;
};

class zip_sink final
{
public:

    // Needed by Boost
    using char_type = char;
    using category  = boost::iostreams::sink_tag;

    // BEWARE: Boost make shallow copies of the zip_sink...
    explicit zip_sink(std::shared_ptr<zip_file_handle> _zip_file_handle) :
        m_zip_file_handle(std::move(_zip_file_handle))
    {
    }

    // Boost use this to write things
    std::streamsize write(const char* _buffer, std::streamsize _size)
    {
        const auto written = mz_zip_writer_entry_write(
            m_zip_file_handle->m_zip_handle->m_zip_writer,
            _buffer,
            std::int32_t(_size)
        );

        SIGHT_THROW_EXCEPTION_IF(
            exception::write(
                "Cannot write in file '"
                + m_zip_file_handle->m_file_name
                + "' in archive '"
                + m_zip_file_handle->m_zip_handle->m_archive_path
                + "'. Error code: "
                + std::to_string(written),
                written
            ),
            written < 0
        );

        return written;
    }

private:

    const std::shared_ptr<zip_file_handle> m_zip_file_handle;
};

class zip_archive_writer final : public archive_writer
{
public:

    SIGHT_DECLARE_CLASS(zip_archive_writer, archive_writer);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    zip_archive_writer()                                     = delete;
    zip_archive_writer(const zip_archive_writer&)            = delete;
    zip_archive_writer(zip_archive_writer&&)                 = delete;
    zip_archive_writer& operator=(const zip_archive_writer&) = delete;
    zip_archive_writer& operator=(zip_archive_writer&&)      = delete;

    zip_archive_writer(const std::filesystem::path& _archive_path, const archive_format _format) :
        archive_writer(_archive_path),
        m_zip_handle(std::make_shared<zip_handle>(_archive_path, _format))
    {
    }

    ~zip_archive_writer() override = default;

    //------------------------------------------------------------------------------

    std::unique_ptr<std::ostream> open_file(
        const std::filesystem::path& _file_path,
        const core::crypto::secure_string& _password = "",
        const method _method                         = method::DEFAULT,
        const level _level                           = level::DEFAULT
    ) override
    {
        const auto zip_file_handle = std::make_shared<io::zip::zip_file_handle>(
            m_zip_handle,
            _file_path,
            _password,
            _method,
            _level
        );

        return std::make_unique<boost::iostreams::stream<zip_sink> >(zip_file_handle);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool is_raw() const override
    {
        return false;
    }

private:

    std::shared_ptr<zip_handle> m_zip_handle;
};

} // namespace

archive_writer::archive_writer(const std::filesystem::path& _archive_path) :
    archive(_archive_path)
{
}

//------------------------------------------------------------------------------

archive_writer::uptr archive_writer::get(
    const std::filesystem::path& _archive_path,
    const archive_format _format
)
{
    if(_format == archive_format::filesystem)
    {
        return std::make_unique<raw_archive_writer>(_archive_path);
    }

    return std::make_unique<zip_archive_writer>(_archive_path, _format);
}

} // namespace sight::io::zip
