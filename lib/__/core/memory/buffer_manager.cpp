/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "core/memory/buffer_manager.hpp"

#include "core/memory/policy/never_dump.hpp"
#include "core/memory/stream/in/buffer.hpp"
#include "core/memory/stream/in/raw.hpp"

#include <core/com/signal.hxx>
#include <core/lazy_instantiator.hpp>
#include <core/os/temp_path.hpp>
#include <core/thread/worker.hpp>
#include <core/tools/system.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iosfwd>
#include <iostream>

namespace sight::core::memory
{

SPTR(void) get_lock(const buffer_manager::sptr& _manager, buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    return _manager->lock_buffer(_buffer_ptr).get();
}

//-----------------------------------------------------------------------------

buffer_manager::sptr buffer_manager::get()
{
    return core::lazy_instantiator<buffer_manager>::get_instance();
}

//-----------------------------------------------------------------------------

buffer_manager::buffer_manager() :
    m_updated_sig(std::make_shared<updated_signal_t>()),
    m_dump_policy(std::make_shared<core::memory::policy::never_dump>()),
    m_worker(core::thread::worker::make())
{
}

//-----------------------------------------------------------------------------

buffer_manager::~buffer_manager()
{
    m_worker->stop();
    // TODO restore dumped buffers
}

//-----------------------------------------------------------------------------

std::shared_future<void> buffer_manager::register_buffer(buffer_manager::buffer_ptr_t _buffer_ptr)
{
    return m_worker->post_task<void>([this, _buffer_ptr](auto&& ...){register_buffer_impl(_buffer_ptr);});
}

//------------------------------------------------------------------------------

void buffer_manager::register_buffer_impl(buffer_manager::buffer_ptr_t _buffer_ptr)
{
    m_buffer_infos.insert(buffer_info_map_t::value_type(_buffer_ptr, buffer_info()));
    m_updated_sig->async_emit();
}

//-----------------------------------------------------------------------------

std::shared_future<void> buffer_manager::unregister_buffer(buffer_manager::buffer_ptr_t _buffer_ptr)
{
    return m_worker->post_task<void>([this, _buffer_ptr](auto&& ...){unregister_buffer_impl(_buffer_ptr);});
}

//------------------------------------------------------------------------------

void buffer_manager::unregister_buffer_impl(buffer_manager::buffer_ptr_t _buffer_ptr)
{
    SIGHT_ASSERT(
        "There are still " << m_buffer_infos[_buffer_ptr].lock_count() << " locks on this BufferObject ("
        << this << ")",
        m_buffer_infos[_buffer_ptr].lock_counter.expired()
    );

    m_buffer_infos.erase(_buffer_ptr);
    m_updated_sig->async_emit();
}

//-----------------------------------------------------------------------------

std::shared_future<void> buffer_manager::allocate_buffer(
    buffer_manager::buffer_ptr_t _buffer_ptr,
    size_t _size,
    const core::memory::buffer_allocation_policy::sptr& _policy
)
{
    return m_worker->post_task<void>(
        [this, _buffer_ptr, _size, _policy](auto&& ...)
        {
            allocate_buffer_impl(_buffer_ptr, _size, _policy);
        });
}

//------------------------------------------------------------------------------

void buffer_manager::allocate_buffer_impl(
    buffer_manager::buffer_ptr_t _buffer_ptr,
    size_t _size,
    const core::memory::buffer_allocation_policy::sptr& _policy
)
{
    buffer_info& info = m_buffer_infos[_buffer_ptr];
    SIGHT_ASSERT("Buffer has already been allocated", info.loaded && (*_buffer_ptr == nullptr));

    if(!info.loaded)
    {
        info.clear();
    }

    m_dump_policy->allocation_request(info, _buffer_ptr, _size);

    try
    {
        _policy->allocate(*_buffer_ptr, _size);
    }
    catch(...)
    {
        info.clear();
        throw;
    }

    info.istream_factory =
        std::make_shared<core::memory::stream::in::buffer>(
            *_buffer_ptr,
            _size,
            [capture0 = this->get_sptr(), _buffer_ptr](auto&& ...){return get_lock(capture0, _buffer_ptr);});

    info.last_access.modified();
    info.size          = _size;
    info.buffer_policy = _policy;
    m_updated_sig->async_emit();
}

//-----------------------------------------------------------------------------

std::shared_future<void> buffer_manager::set_buffer(
    buffer_manager::buffer_ptr_t _buffer_ptr,
    core::memory::buffer_manager::buffer_t _buffer,
    size_t _size,
    const core::memory::buffer_allocation_policy::sptr& _policy
)
{
    return m_worker->post_task<void>(
        [this, _buffer_ptr, _buffer, _size, _policy](auto&& ...)
        {
            set_buffer_impl(_buffer_ptr, _buffer, _size, _policy);
        });
}

//------------------------------------------------------------------------------

void buffer_manager::set_buffer_impl(
    buffer_manager::buffer_ptr_t _buffer_ptr,
    core::memory::buffer_manager::buffer_t _buffer,
    size_t _size,
    const core::memory::buffer_allocation_policy::sptr& _policy
)
{
    buffer_info& info = m_buffer_infos[_buffer_ptr];

    SIGHT_ASSERT("Buffer is already set", *_buffer_ptr == nullptr && info.loaded);

    m_dump_policy->set_request(info, _buffer_ptr, _size);

    if(!info.loaded)
    {
        info.clear();
    }

    *_buffer_ptr = _buffer;

    info.last_access.modified();
    info.size          = _size;
    info.buffer_policy = _policy;
    info.file_format   = core::memory::other;
    info.fs_file.clear();
    info.istream_factory =
        std::make_shared<core::memory::stream::in::buffer>(
            *_buffer_ptr,
            _size,
            [capture0 = this->get_sptr(), _buffer_ptr](auto&& ...){return get_lock(capture0, _buffer_ptr);});
    info.user_stream_factory = false;
    m_updated_sig->async_emit();
}

//-----------------------------------------------------------------------------

std::shared_future<void> buffer_manager::reallocate_buffer(
    buffer_manager::buffer_ptr_t _buffer_ptr,
    size_t _new_size
)
{
    return m_worker->post_task<void>(
        [this, _buffer_ptr, _new_size](auto&& ...)
        {
            reallocate_buffer_impl(_buffer_ptr, _new_size);
        });
}

//------------------------------------------------------------------------------

void buffer_manager::reallocate_buffer_impl(buffer_manager::buffer_ptr_t _buffer_ptr, size_t _new_size)
{
    buffer_info& info = m_buffer_infos[_buffer_ptr];
    SIGHT_ASSERT("Buffer must be allocated or dumped", (*_buffer_ptr != nullptr) || !info.loaded);

    m_dump_policy->reallocate_request(info, _buffer_ptr, _new_size);

    try
    {
        if(info.loaded)
        {
            info.buffer_policy->reallocate(*_buffer_ptr, _new_size);
        }
        else
        {
            this->restore_buffer(info, _buffer_ptr, _new_size);
        }
    }
    catch(core::memory::exception::memory&)
    {
        m_updated_sig->async_emit();
        throw;
    }

    info.istream_factory =
        std::make_shared<core::memory::stream::in::buffer>(
            *_buffer_ptr,
            _new_size,
            [capture0 = this->get_sptr(), _buffer_ptr](auto&& ...){return get_lock(capture0, _buffer_ptr);});

    info.last_access.modified();
    info.size = _new_size;

    m_updated_sig->async_emit();
}

//-----------------------------------------------------------------------------

std::shared_future<void> buffer_manager::destroy_buffer(buffer_manager::buffer_ptr_t _buffer_ptr)
{
    return m_worker->post_task<void>([this, _buffer_ptr](auto&& ...){destroy_buffer_impl(_buffer_ptr);});
}

//------------------------------------------------------------------------------

void buffer_manager::destroy_buffer_impl(buffer_manager::buffer_ptr_t _buffer_ptr)
{
    buffer_info& info = m_buffer_infos[_buffer_ptr];
    SIGHT_ASSERT("Buffer must be allocated or dumped", (*_buffer_ptr != nullptr) || !info.loaded);

    m_dump_policy->destroy_request(info, _buffer_ptr);
    info.buffer_policy->destroy(*_buffer_ptr);

    info.clear();
    info.last_access.modified();
    m_updated_sig->async_emit();
}

//-----------------------------------------------------------------------------

std::shared_future<void> buffer_manager::swap_buffer(
    buffer_manager::buffer_ptr_t _buf_a,
    buffer_manager::buffer_ptr_t _buf_b
)
{
    return m_worker->post_task<void>([this, _buf_a, _buf_b](auto&& ...){swap_buffer_impl(_buf_a, _buf_b);});
}

//------------------------------------------------------------------------------

void buffer_manager::swap_buffer_impl(buffer_manager::buffer_ptr_t _buf_a, buffer_manager::buffer_ptr_t _buf_b)
{
    buffer_info& info_a = m_buffer_infos[_buf_a];
    buffer_info& info_b = m_buffer_infos[_buf_b];

    std::swap(*_buf_a, *_buf_b);
    std::swap(info_a.size, info_b.size);
    std::swap(info_a.loaded, info_b.loaded);
    std::swap(info_a.fs_file, info_b.fs_file);
    std::swap(info_a.buffer_policy, info_b.buffer_policy);
    std::swap(info_a.istream_factory, info_b.istream_factory);
    std::swap(info_a.user_stream_factory, info_b.user_stream_factory);
    info_a.last_access.modified();
    info_b.last_access.modified();
}

//-----------------------------------------------------------------------------

struct auto_unlock
{
    auto_unlock(
        const buffer_manager::sptr& _manager,
        buffer_manager::const_buffer_ptr_t _buffer_ptr,
        const buffer_info& _info
    ) :
        m_manager(_manager),
        m_buffer_ptr(_buffer_ptr)
    {
        if(!_info.loaded)
        {
            bool restored = _manager->restore_buffer(_buffer_ptr).get();
            SIGHT_ASSERT(
                "restore not OK ( " << restored << " && " << *_buffer_ptr << " != 0 ).",
                restored && *_buffer_ptr != nullptr
            );
            SIGHT_NOT_USED(restored);
        }
    }

    ~auto_unlock()
    {
        try
        {
            m_manager->unlock_buffer(m_buffer_ptr);
        }
        catch([[maybe_unused]] const std::exception& e)
        {
            SIGHT_ASSERT("Unlock Failed" << e.what(), 0);
        }
        catch(...)
        {
            SIGHT_ASSERT("Unlock Failed", 0);
        }
    }

    buffer_manager::sptr m_manager;
    buffer_manager::const_buffer_ptr_t m_buffer_ptr;
};

//------------------------------------------------------------------------------

std::shared_future<SPTR(void)> buffer_manager::lock_buffer(buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    return m_worker->post_task<SPTR(void)>([this, _buffer_ptr](auto&& ...){return lock_buffer_impl(_buffer_ptr);});
}

SPTR(void) buffer_manager::lock_buffer_impl(buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    buffer_info& info = m_buffer_infos[_buffer_ptr];

    m_dump_policy->lock_request(info, _buffer_ptr);

    SPTR(void) counter = info.lock_counter.lock();
    if(!counter)
    {
        counter           = std::make_shared<auto_unlock>(this->get_sptr(), _buffer_ptr, info);
        info.lock_counter = counter;
    }

    m_last_access.modified();

    m_updated_sig->async_emit();

    return counter;
}

//-----------------------------------------------------------------------------

std::shared_future<bool> buffer_manager::unlock_buffer(buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    return m_worker->post_task<bool>([this, _buffer_ptr](auto&& ...){return unlock_buffer_impl(_buffer_ptr);});
}

//------------------------------------------------------------------------------

bool buffer_manager::unlock_buffer_impl(buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    buffer_info& info = m_buffer_infos[_buffer_ptr];
    m_dump_policy->unlock_request(info, _buffer_ptr);

    m_updated_sig->async_emit();
    return true;
}

//-----------------------------------------------------------------------------

std::shared_future<bool> buffer_manager::dump_buffer(buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    return m_worker->post_task<bool>([this, _buffer_ptr](auto&& ...){return dump_buffer_impl(_buffer_ptr);});
}

//------------------------------------------------------------------------------

bool buffer_manager::dump_buffer_impl(buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto* casted_buffer = const_cast<buffer_manager::buffer_ptr_t>(_buffer_ptr);
    buffer_info& info   = m_buffer_infos[casted_buffer];
    return this->dump_buffer(info, casted_buffer);
}

//-----------------------------------------------------------------------------

bool buffer_manager::dump_buffer(buffer_info& _info, buffer_manager::buffer_ptr_t _buffer_ptr)
{
    if(!_info.loaded || _info.lock_count() > 0 || _info.size == 0)
    {
        return false;
    }

    const std::filesystem::path& dumped_file = core::os::temp_file::unique_path();

    _info.lock_counter.reset();

    if(sight::core::memory::buffer_manager::write_buffer_impl(*_buffer_ptr, _info.size, dumped_file))
    {
        _info.fs_file             = core::memory::file_holder(dumped_file, true);
        _info.file_format         = core::memory::raw;
        _info.istream_factory     = std::make_shared<core::memory::stream::in::raw>(_info.fs_file);
        _info.user_stream_factory = false;
        _info.buffer_policy->destroy(*_buffer_ptr);
        *_buffer_ptr = nullptr;
        _info.loaded = false;

        m_dump_policy->dump_success(_info, _buffer_ptr);

        m_updated_sig->async_emit();
    }

    return !_info.loaded;
}

//-----------------------------------------------------------------------------

std::shared_future<bool> buffer_manager::restore_buffer(buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    return m_worker->post_task<bool>([this, _buffer_ptr](auto&& ...){return restore_buffer_impl(_buffer_ptr);});
}

//------------------------------------------------------------------------------

bool buffer_manager::restore_buffer_impl(buffer_manager::const_buffer_ptr_t _buffer_ptr)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto* casted_buffer = const_cast<buffer_manager::buffer_ptr_t>(_buffer_ptr);
    buffer_info& info   = m_buffer_infos[casted_buffer];
    return this->restore_buffer(info, casted_buffer);
}

//-----------------------------------------------------------------------------

bool buffer_manager::restore_buffer(
    buffer_info& _info,
    buffer_manager::buffer_ptr_t _buffer_ptr,
    buffer_manager::size_t _alloc_size
)
{
    _alloc_size = ((_alloc_size) != 0U ? _alloc_size : _info.size);
    if(!_info.loaded)
    {
        if(*_buffer_ptr == nullptr)
        {
            _info.buffer_policy->allocate(*_buffer_ptr, _alloc_size);
        }
        else
        {
            _info.buffer_policy->reallocate(*_buffer_ptr, _alloc_size);
        }

        char* char_buf  = static_cast<char*>(*_buffer_ptr);
        size_t size     = std::min(_alloc_size, _info.size);
        bool not_failed = false;
        {
            SPTR(std::istream) stream = (*_info.istream_factory)();
            std::istream& is = *stream;
            const auto read  = static_cast<size_t>(is.read(char_buf, static_cast<std::streamsize>(size)).gcount());

            SIGHT_THROW_IF(" Bad file size, expected: " << size << ", was: " << read, size - read != 0);
            not_failed = !is.fail();
        }

        if(not_failed)
        {
            _info.loaded = true;
            _info.fs_file.clear();
            _info.last_access.modified();

            m_dump_policy->restore_success(_info, _buffer_ptr);

            _info.file_format     = core::memory::other;
            _info.istream_factory =
                std::make_shared<core::memory::stream::in::buffer>(
                    *_buffer_ptr,
                    _alloc_size,
                    [capture0 = this->get_sptr(), _buffer_ptr](auto&& ...){return get_lock(capture0, _buffer_ptr);});
            _info.user_stream_factory = false;
            m_updated_sig->async_emit();
            return true;
        }
    }

    return false;
}

//-----------------------------------------------------------------------------

std::shared_future<bool> buffer_manager::write_buffer(
    buffer_manager::const_buffer_t _buffer,
    size_t _size,
    std::filesystem::path& _path
)
{
    return m_worker->post_task<bool>(
        [_buffer, _size, _path](auto&& ...)
        {
            return buffer_manager::write_buffer_impl(_buffer, _size, _path);
        });
}

//------------------------------------------------------------------------------

bool buffer_manager::write_buffer_impl(
    buffer_manager::const_buffer_t _buffer,
    size_t _size,
    const std::filesystem::path& _path
)
{
    std::ofstream fs(_path, std::ios::binary | std::ios::trunc);
    SIGHT_THROW_IF("Memory management : Unable to open " << _path, !fs.good());
    const char* char_buf = static_cast<const char*>(_buffer);
    fs.write(char_buf, static_cast<std::streamsize>(_size));
    fs.close();
    return !fs.bad();
}

//-----------------------------------------------------------------------------

std::shared_future<bool> buffer_manager::read_buffer(
    buffer_manager::buffer_t _buffer,
    size_t _size,
    std::filesystem::path& _path
)
{
    return m_worker->post_task<bool>(
        [_buffer, _size, _path](auto&& ...)
        {
            return buffer_manager::read_buffer_impl(_buffer, _size, _path);
        });
}

//------------------------------------------------------------------------------

bool buffer_manager::read_buffer_impl(
    buffer_manager::buffer_t _buffer,
    size_t _size,
    const std::filesystem::path& _path
)
{
    std::ifstream fs(_path, std::ios::in | std::ios::binary | std::ios::ate);
    SIGHT_THROW_IF("Unable to read " << _path, !fs.good());

    std::streampos file_size = fs.tellg();
    fs.seekg(0, std::ios::beg);

    SIGHT_THROW_IF(
        _path << ": Bad file size, expected: " << _size << ", was: " << file_size,
        _size - static_cast<size_t>(file_size) != 0
    );

    char* char_buf = static_cast<char*>(_buffer);
    fs.read(char_buf, static_cast<std::streamsize>(_size));

    fs.close();
    return !fs.bad();
}

//-----------------------------------------------------------------------------

std::shared_future<std::string> buffer_manager::to_string() const
{
    return m_worker->post_task<std::string>([this](auto&& ...){return to_string_impl();});
}

//------------------------------------------------------------------------------

std::string buffer_manager::to_string_impl() const
{
    std::stringstream sstr("");
    sstr << "nb Elem = " << m_buffer_infos.size() << std::endl;
    sstr << std::setw(18) << "Buffer" << "->" << std::setw(18) << "Address" << " "
    << std::setw(10) << "Size" << " "
    << std::setw(18) << "Policy" << " "
    << std::setw(6) << "Access" << " "
    << std::setw(4) << "Lock" << " "
    << "DumpStatus" << " "
    << "File" << " "
    << std::endl;
    for(const auto& item : m_buffer_infos)
    {
        const buffer_info& info = item.second;
        sstr << std::setw(18) << item.first << "->" << std::setw(18) << *(item.first) << " "
        << std::setw(10) << info.size << " "
        << std::setw(18) << info.buffer_policy << " "
        << std::setw(6) << info.last_access << " "
        << std::setw(4) << info.lock_count() << " "
        << ((info.loaded) ? "   " : "not") << " loaded "
        << std::filesystem::path(info.fs_file) << " "
        << std::endl;
    }

    return sstr.str();
}

//-----------------------------------------------------------------------------

void buffer_manager::set_dump_policy(const core::memory::policy::base::sptr& _policy)
{
    m_dump_policy = _policy;
    _policy->refresh();
}

//-----------------------------------------------------------------------------

core::memory::policy::base::sptr buffer_manager::get_dump_policy() const
{
    return m_dump_policy;
}

//-----------------------------------------------------------------------------

std::shared_future<buffer_manager::buffer_info_map_t> buffer_manager::get_buffer_infos() const
{
    return m_worker->post_task<buffer_info_map_t>([this](auto&& ...){return get_buffer_infos_impl();});
}

//------------------------------------------------------------------------------

buffer_manager::buffer_info_map_t buffer_manager::get_buffer_infos_impl() const
{
    return m_buffer_infos;
}

//-----------------------------------------------------------------------------

std::shared_future<buffer_manager::buffer_stats> buffer_manager::get_buffer_stats() const
{
    return m_worker->post_task<buffer_manager::buffer_stats>(
        [this](auto&& ...){return buffer_manager::compute_buffer_stats(m_buffer_infos);});
}

//------------------------------------------------------------------------------

buffer_manager::buffer_stats buffer_manager::compute_buffer_stats(const buffer_info_map_t& _buffer_info)
{
    buffer_stats stats = {0, 0};
    for(const auto& item : _buffer_info)
    {
        const auto& info = item.second;
        if(!info.loaded)
        {
            stats.total_dumped += info.size;
        }

        stats.total_managed += info.size;
    }

    return stats;
}

//-----------------------------------------------------------------------------

std::shared_future<void> buffer_manager::set_istream_factory(
    buffer_ptr_t _buffer_ptr,
    const SPTR(core::memory::stream::in::factory)& _factory,
    size_t _size,
    core::memory::file_holder _fs_file,
    core::memory::file_format_type _format,
    const core::memory::buffer_allocation_policy::sptr& _policy
)
{
    return m_worker->post_task<void>(
        [this, _buffer_ptr, _factory, _size, _fs_file, _format, _policy](auto&& ...)
        {
            set_istream_factory_impl(_buffer_ptr, _factory, _size, _fs_file, _format, _policy);
        });
}

//------------------------------------------------------------------------------

void buffer_manager::set_istream_factory_impl(
    buffer_ptr_t _buffer_ptr,
    const SPTR(core::memory::stream::in::factory)& _factory,
    size_t _size,
    core::memory::file_holder _fs_file,
    core::memory::file_format_type _format,
    const core::memory::buffer_allocation_policy::sptr& _policy
)
{
    auto iter_info = m_buffer_infos.find(_buffer_ptr);
    SIGHT_THROW_IF("Buffer is not managed by core::memory::buffer_manager.", iter_info == m_buffer_infos.end());
    buffer_info& info = iter_info->second;

    SIGHT_ASSERT("Buffer is already set", *_buffer_ptr == nullptr && info.loaded);

    m_dump_policy->set_request(info, _buffer_ptr, _size);

    info.istream_factory     = _factory;
    info.user_stream_factory = true;
    info.size                = _size;
    info.fs_file             = _fs_file;
    info.file_format         = _format;
    info.buffer_policy       = _policy;
    info.loaded              = false;

    m_dump_policy->dump_success(info, _buffer_ptr);

    switch(m_loading_mode)
    {
        case buffer_manager::direct:
            this->restore_buffer(_buffer_ptr);
            break;

        case buffer_manager::lazy:
            m_updated_sig->async_emit();
            break;

        default:
            SIGHT_ASSERT("You shall not pass", 0);
    }
}

//-----------------------------------------------------------------------------

std::shared_future<buffer_manager::stream_info> buffer_manager::get_stream_info(
    const buffer_manager::const_buffer_ptr_t _buffer_ptr
) const
{
    return m_worker->post_task<buffer_manager::stream_info>(
        [this, _buffer_ptr](auto&& ...){return get_stream_info_impl(_buffer_ptr);});
}

//------------------------------------------------------------------------------

buffer_manager::stream_info buffer_manager::get_stream_info_impl(const buffer_manager::const_buffer_ptr_t _buffer_ptr)
const
{
    stream_info stream_info;
    auto iter_info = m_buffer_infos.find(_buffer_ptr);
    SIGHT_THROW_IF("Buffer is not managed by core::memory::buffer_manager.", iter_info == m_buffer_infos.end());
    const buffer_info& info = iter_info->second;
    stream_info.size        = info.size;
    stream_info.fs_file     = info.fs_file;
    stream_info.format      = info.file_format;
    stream_info.user_stream = info.user_stream_factory;

    if(info.istream_factory)
    {
        stream_info.stream = (*info.istream_factory)();
    }

    return stream_info;
}

//-----------------------------------------------------------------------------

buffer_manager::loading_mode_type buffer_manager::get_loading_mode() const
{
    return m_loading_mode;
}

//------------------------------------------------------------------------------

void buffer_manager::set_loading_mode(loading_mode_type _mode)
{
    m_loading_mode = _mode;
}

} //namespace sight::core::memory
