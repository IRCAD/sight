/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

#include "archive_reader.hpp"

#include "core/crypto/secure_string.hpp"

#include "exception/read.hpp"

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

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace sight::io::zip
{

namespace
{

class raw_archive_reader final : public archive_reader
{
public:

    SIGHT_DECLARE_CLASS(raw_archive_reader, archive_reader);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    raw_archive_reader()                                     = delete;
    raw_archive_reader(const raw_archive_reader&)            = delete;
    raw_archive_reader(raw_archive_reader&&)                 = delete;
    raw_archive_reader& operator=(const raw_archive_reader&) = delete;
    raw_archive_reader& operator=(raw_archive_reader&&)      = delete;

    /// Constructor. It open the archive and create all resources needed to access it.
    /// @param _root path of the archive file. The file will be kept opened as long as the instance lives.
    explicit raw_archive_reader(const std::filesystem::path& _root) :
        archive_reader(_root),
        m_root(_root)
    {
    }

    ~raw_archive_reader() final = default;

    //------------------------------------------------------------------------------

    std::unique_ptr<std::istream> open_file(
        const std::filesystem::path& _file_path,
        [[maybe_unused]] const core::crypto::secure_string& _password = ""
    ) final
    {
        return std::make_unique<std::ifstream>(m_root / _file_path.relative_path(), std::ios::in | std::ios::binary);
    }

    //------------------------------------------------------------------------------

    std::string read_file(
        const std::filesystem::path& _file_path,
        [[maybe_unused]] const core::crypto::secure_string& _password = ""
    ) final
    {
        std::ifstream file(m_root / _file_path.relative_path(), std::ios::in | std::ios::binary | std::ios::ate);

        const std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string content(std::size_t(size), 0);
        file.read(content.data(), size);

        return content;
    }

    //------------------------------------------------------------------------------

    void extract_all_to(
        const std::filesystem::path& _output_path,
        const core::crypto::secure_string& /*password*/
    ) final
    {
        std::filesystem::copy(m_root, _output_path, std::filesystem::copy_options::recursive);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool is_raw() const final
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

    explicit zip_handle(const std::filesystem::path& _archive_path) :
        m_archive_path(_archive_path.string()),
        m_zip_reader(mz_zip_reader_create())
    {
        SIGHT_THROW_EXCEPTION_IF(
            exception::read(
                "Cannot create zip reader instance",
                MZ_MEM_ERROR
            ),
            m_zip_reader == nullptr
        );

        const auto result = mz_zip_reader_open_file(m_zip_reader, m_archive_path.c_str());

        SIGHT_THROW_EXCEPTION_IF(
            exception::read(
                "Cannot open archive '" + m_archive_path + "'. Error code: " + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

    ~zip_handle()
    {
        // Close zip handle
        const auto result = mz_zip_reader_close(m_zip_reader);

        // Cleanup
        mz_zip_reader_delete(&m_zip_reader);

        SIGHT_THROW_EXCEPTION_IF(
            exception::read(
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
        core::crypto::secure_string _password = ""
    ) :
        m_file_path(_file_path.string()),
        m_password(std::move(_password)),
        m_zip_handle(std::move(_zip_handle))
    {
        // Set encryption
        mz_zip_reader_set_password(m_zip_handle->m_zip_reader, m_password.empty() ? nullptr : m_password.c_str());

        if(const auto result = mz_zip_reader_locate_entry(m_zip_handle->m_zip_reader, m_file_path.c_str(), 0);
           result != MZ_OK)
        {
            SIGHT_THROW_EXCEPTION(
                exception::read(
                    "Cannot locate file '"
                    + m_file_path
                    + "' in archive '"
                    + m_zip_handle->m_archive_path
                    + "'. Error code: "
                    + std::to_string(result),
                    result
                )
            );
        }

        // NOLINTBEGIN(readability-else-after-return)
        if(const auto result = mz_zip_reader_entry_open(m_zip_handle->m_zip_reader);
           result == MZ_PASSWORD_ERROR)
        {
            const auto& message =
                "File '"
                + m_file_path
                + "' from archive '"
                + m_zip_handle->m_archive_path
                + "' is password protected and the provided one does not match. Error code: "
                + std::to_string(result);

            // Log the exception as debug, as it is expected to happen when trying to open a password protected file
            SIGHT_DEBUG(message);

            // Do not use SIGHT_THROW_EXCEPTION to avoid polluting the logs
            throw(exception::bad_password(message, result));
        }
        else if(result != MZ_OK)
        {
            SIGHT_THROW_EXCEPTION(
                exception::read(
                    "Cannot open file '"
                    + m_file_path
                    + "' from archive '"
                    + m_zip_handle->m_archive_path
                    + "'. Error code: "
                    + std::to_string(result),
                    result
                )
            );
        }

        // NOLINTEND(readability-else-after-return)
    }

    ~zip_file_handle()
    {
        const auto result = mz_zip_reader_entry_close(m_zip_handle->m_zip_reader);

        // Restore defaults
        mz_zip_reader_set_password(m_zip_handle->m_zip_reader, nullptr);

        SIGHT_THROW_EXCEPTION_IF(
            exception::read(
                "Cannot close file '"
                + m_file_path
                + "' from archive '"
                + m_zip_handle->m_archive_path
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );
    }

private:

    friend class zip_source;
    friend class zip_archive_reader;

    // Path to the file converted to string because on Windows std::filesystem::path.c_str() returns a wchar*
    const std::string m_file_path;

    // The password must be kept alive since minizip doesn't copy it
    const core::crypto::secure_string m_password;

    // Zip handles pack which contains the zip writer
    const std::shared_ptr<zip_handle> m_zip_handle;
};

class zip_source final
{
public:

    // Needed by Boost
    using char_type = char;
    using category  = boost::iostreams::source_tag;

    // BEWARE: Boost make shallow copies of the ZipSource...
    explicit zip_source(std::shared_ptr<zip_file_handle> _zip_file_handle) :
        m_zip_file_handle(std::move(_zip_file_handle))
    {
    }

    // Boost use this to read things
    std::streamsize read(char* _buffer, std::streamsize _size)
    {
        const std::int32_t block_size = std::int32_t(
            std::min(_size, std::streamsize(std::numeric_limits<std::int32_t>::max()))
        );

        const auto read = mz_zip_reader_entry_read(
            m_zip_file_handle->m_zip_handle->m_zip_reader,
            _buffer,
            block_size
        );

        SIGHT_THROW_EXCEPTION_IF(
            exception::read(
                "Cannot read in file '"
                + m_zip_file_handle->m_file_path
                + "' in archive '"
                + m_zip_file_handle->m_zip_handle->m_archive_path
                + "'. Error code: "
                + std::to_string(read),
                read
            ),
            read < 0
        );

        return std::streamsize(read);
    }

private:

    const std::shared_ptr<zip_file_handle> m_zip_file_handle;
};

class zip_archive_reader final : public archive_reader
{
public:

    SIGHT_DECLARE_CLASS(zip_archive_reader, archive_reader);

    /// Delete default constructors and assignment operators, as we don't want to allow resources duplication
    zip_archive_reader()                                     = delete;
    zip_archive_reader(const zip_archive_reader&)            = delete;
    zip_archive_reader(zip_archive_reader&&)                 = delete;
    zip_archive_reader& operator=(const zip_archive_reader&) = delete;
    zip_archive_reader& operator=(zip_archive_reader&&)      = delete;

    explicit zip_archive_reader(const std::filesystem::path& _archive_path) :
        archive_reader(_archive_path),
        m_zip_handle(std::make_shared<zip_handle>(_archive_path))
    {
    }

    ~zip_archive_reader() final = default;

    //------------------------------------------------------------------------------

    std::unique_ptr<std::istream> open_file(
        const std::filesystem::path& _file_path,
        const core::crypto::secure_string& _password = ""
    ) final
    {
        const auto file_handle = std::make_shared<zip_file_handle>(
            m_zip_handle,
            _file_path,
            _password
        );

        return std::make_unique<boost::iostreams::stream<zip_source> >(file_handle);
    }

    //------------------------------------------------------------------------------

    std::string read_file(
        const std::filesystem::path& _file_path,
        const core::crypto::secure_string& _password = ""
    ) final
    {
        const auto file_handle = std::make_shared<zip_file_handle>(
            m_zip_handle,
            _file_path,
            _password
        );

        mz_zip_file* file_info = nullptr;
        const auto result      = mz_zip_reader_entry_get_info(file_handle->m_zip_handle->m_zip_reader, &file_info);

        SIGHT_THROW_EXCEPTION_IF(
            exception::read(
                "Cannot get file info of file '"
                + file_handle->m_file_path
                + "' in archive '"
                + file_handle->m_zip_handle->m_archive_path
                + "'. Error code: "
                + std::to_string(result),
                result
            ),
            result != MZ_OK
        );

        std::string content(std::size_t(file_info->uncompressed_size), 0);

        std::size_t remaining_size = content.size();
        char* remaining_buffer     = content.data();

        std::int32_t block_size = std::int32_t(
            std::min(remaining_size, std::size_t(std::numeric_limits<std::int32_t>::max()))
        );

        while(remaining_size > 0)
        {
            const auto read = mz_zip_reader_entry_read(
                file_handle->m_zip_handle->m_zip_reader,
                remaining_buffer,
                block_size
            );

            if(read == 0)
            {
                break;
            }

            SIGHT_THROW_EXCEPTION_IF(
                exception::read(
                    "Cannot read in file '"
                    + file_handle->m_file_path
                    + "' in archive '"
                    + file_handle->m_zip_handle->m_archive_path
                    + "'. Error code: "
                    + std::to_string(read),
                    read
                ),
                read < 0
            );

            remaining_size   -= std::size_t(read);
            remaining_buffer += read;
        }

        SIGHT_THROW_EXCEPTION_IF(
            exception::read(
                "Cannot read in file '"
                + file_handle->m_file_path
                + "' in archive '"
                + file_handle->m_zip_handle->m_archive_path
                + "'. Remaining: "
                + std::to_string(remaining_size),
                MZ_WRITE_ERROR
            ),
            remaining_size > 0
        );

        return content;
    }

    //------------------------------------------------------------------------------

    void extract_all_to(
        const std::filesystem::path& _output_path,
        const core::crypto::secure_string& _password
    ) final
    {
        mz_zip_reader_set_password(m_zip_handle->m_zip_reader, _password.empty() ? nullptr : _password.c_str());
        std::vector<std::filesystem::path> extracted_files;
        mz_zip_reader_set_entry_cb(
            m_zip_handle->m_zip_reader,
            &extracted_files,
            [](void*, void* _extracted_files_ptr, mz_zip_file*, const char* _path)
                {
                    auto* files = reinterpret_cast<std::vector<std::filesystem::path>*>(_extracted_files_ptr);
                    files->emplace_back(_path);
                    return MZ_OK;
                });

        // NOLINTBEGIN(readability-else-after-return)
        if(const auto result = mz_zip_reader_save_all(m_zip_handle->m_zip_reader, _output_path.string().c_str());
           result == MZ_PASSWORD_ERROR)
        {
            SIGHT_THROW_EXCEPTION(
                exception::bad_password(
                    "Archive '"
                    + m_zip_handle->m_archive_path
                    + "' is password protected and the provided one does not match. Error code: "
                    + std::to_string(result),
                    result
                )
            );
        }
        else if(result == MZ_DATA_ERROR && _password.empty())
        {
            throw exception::bad_password(
                      "Cannot extract archive '" + m_zip_handle->m_archive_path
                      + "', it is probably password protected. Error code: " + std::to_string(
                          result
                      ),
                      result
            );
        }
        else if(result != MZ_OK)
        {
            SIGHT_THROW_EXCEPTION(
                exception::read(
                    "Cannot extract archive '"
                    + m_zip_handle->m_archive_path
                    + "'. Error code: "
                    + std::to_string(result),
                    result
                )
            );
        }

        // NOLINTEND(readability-else-after-return)

        mz_zip_reader_set_password(m_zip_handle->m_zip_reader, nullptr);
        mz_zip_reader_set_entry_cb(m_zip_handle->m_zip_reader, nullptr, nullptr);

        // For some reasons, minizip saves the output files without any permissions.
        // Set user read and user write at the very least.
        std::ranges::for_each(
            extracted_files,
            [](const std::filesystem::path& _entry)
                {
                    std::filesystem::permissions(
                        _entry,
                        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
                        std::filesystem::perm_options::add
                    );
                });
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] bool is_raw() const final
    {
        return false;
    }

private:

    std::shared_ptr<zip_handle> m_zip_handle;
};

} // anonymous namespace

archive_reader::archive_reader(const std::filesystem::path& _archive_path) :
    archive(_archive_path)
{
}

//------------------------------------------------------------------------------

archive_reader::uptr archive_reader::get(
    const std::filesystem::path& _archive_path,
    const archive_format _format
)
{
    if(_format == archive_format::filesystem)
    {
        return std::make_unique<raw_archive_reader>(_archive_path);
    }

    return std::make_unique<zip_archive_reader>(_archive_path);
}

} // namespace sight::io::zip
