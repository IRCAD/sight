/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include <sight/core/config.hpp>

#include "core/memory/buffer_info.hpp"
#include "core/memory/file_holder.hpp"

#include <core/base.hpp>
#include <core/base_object.hpp>
#include <core/com/signal.hpp>
#include <core/mt/types.hpp>

#include <filesystem>
#include <future>

namespace sight::core::thread
{

class worker;

} // namespace sight::core::thread

namespace sight::core::memory
{

namespace policy
{

class base;

}

namespace stream::in
{

class factory;

} // namespace stream::in

class buffer_manager;

class key
{
friend SPTR(buffer_manager) get();

key()
= default;
};

/**
 * @brief BufferManager implementation
 *
 * This implementation purpose is to manage memory load, freeing memory and
 * restoring freed buffers as needed.
 *
 * A dump policy is used to trigger memory freeing process. The restore process
 * is always triggers when a lock is requested on a dumped buffer.
 */
class SIGHT_CORE_CLASS_API buffer_manager : public core::base_object
{
public:

    using buffer_t           = void*;
    using const_buffer_t     = const void*;
    using buffer_ptr_t       = buffer_t*;
    using const_buffer_ptr_t = const void* const*;

    using size_t = buffer_info::size_t;

    using updated_signal_t = core::com::signal<void ()>;

    using buffer_info_map_t = std::map<const_buffer_ptr_t, buffer_info>;

    SIGHT_DECLARE_CLASS(buffer_manager, core::base_object);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    buffer_manager();
    ~buffer_manager() override;

    enum loading_mode_type
    {
        direct,
        lazy
    };

    struct buffer_stats
    {
        size_t total_dumped;
        size_t total_managed;
    };

    struct stream_info
    {
        size_t size {};
        SPTR(std::istream) stream;
        /// path of the file containing the dumped buffer
        core::memory::file_holder fs_file;
        /// format of the dumped file
        core::memory::file_format_type format {other};
        /// true if stream has been set by the user
        bool user_stream {};
    };

    /**
     * @brief Hook called when a new BufferObject is created
     *
     * @param _buffer_ptr BufferObject's buffer pointer.
     */
    SIGHT_CORE_API virtual std::shared_future<void> register_buffer(buffer_ptr_t _buffer_ptr);

    /**
     * @brief Hook called when a BufferObject is destroyed
     *
     * @param _buffer_ptr BufferObject's buffer pointer.
     */
    SIGHT_CORE_API virtual std::shared_future<void> unregister_buffer(buffer_ptr_t _buffer_ptr);

    /**
     * @brief Hook called when an allocation is requested from a BufferObject
     *
     * @param _buffer_ptr BufferObject's buffer pointer
     * @param _size requested size for allocation
     * @param _policy BufferObject's allocation policy
     */
    SIGHT_CORE_API virtual std::shared_future<void> allocate_buffer(
        buffer_ptr_t _buffer_ptr,
        size_t _size,
        const core::memory::buffer_allocation_policy::sptr& _policy
    );

    /**
     * @brief Hook called when a request is made to set BufferObject's buffer from an external buffer
     *
     * @param _buffer_ptr BufferObject's buffer pointer
     * @param _buffer new buffer
     * @param _size requested size for allocation
     * @param _policy BufferObject's allocation policy
     */
    SIGHT_CORE_API virtual std::shared_future<void> set_buffer(
        buffer_ptr_t _buffer_ptr,
        buffer_t _buffer,
        size_t _size,
        const core::memory::buffer_allocation_policy::sptr& _policy
    );

    /**
     * @brief Hook called when a reallocation is requested from a BufferObject
     *
     * @param _buffer_ptr BufferObject's buffer pointer
     * @param _new_size requested size for reallocation
     */
    SIGHT_CORE_API virtual std::shared_future<void> reallocate_buffer(buffer_ptr_t _buffer_ptr, size_t _new_size);

    /**
     * @brief Hook called when a destruction is requested from a BufferObject
     *
     * @param _buffer_ptr BufferObject's buffer pointer
     */
    SIGHT_CORE_API virtual std::shared_future<void> destroy_buffer(buffer_ptr_t _buffer_ptr);

    /**
     * @brief Hook called when a request to swap two BufferObject contents is made
     *
     * @param _buf_a First BufferObject's buffer
     * @param _buf_b Second BufferObject's buffer
     */
    SIGHT_CORE_API virtual std::shared_future<void> swap_buffer(buffer_ptr_t _buf_a, buffer_ptr_t _buf_b);

    /**
     * @brief Hook called when a BufferObject is locked
     *
     * @param _buffer_ptr BufferObject's buffer pointer
     *
     * @return false if the BufferManager supported the action
     */
    SIGHT_CORE_API virtual std::shared_future<SPTR(void)> lock_buffer(const_buffer_ptr_t _buffer_ptr);

    /**
     * @brief Hook called when a BufferObject lock is released
     *
     * @param _buffer_ptr BufferObject's buffer pointer
     *
     * @return false if the BufferManager supported the action
     */
    SIGHT_CORE_API virtual std::shared_future<bool> unlock_buffer(const_buffer_ptr_t _buffer_ptr);

    /**
     * @brief returns BufferManager status string
     */
    SIGHT_CORE_API virtual std::shared_future<std::string> to_string() const;

    /**
     * @brief Dump/restore a buffer
     *
     * Keeps up to date buffer's information
     *
     * @param _buffer_ptr Buffer to dump/restore
     *
     * @return true on success
     * @{ */
    SIGHT_CORE_API std::shared_future<bool> dump_buffer(const_buffer_ptr_t _buffer_ptr);
    SIGHT_CORE_API std::shared_future<bool> restore_buffer(const_buffer_ptr_t _buffer_ptr);
    /**  @} */

    /**
     * @brief Write/read a buffer
     *
     * The buffer is written/read in a raw format
     *
     * @param _buffer Buffer to write/read
     * @param _size Buffer's size
     * @param _path file to  write/read
     *
     * @return true on success
     * @{ */
    SIGHT_CORE_API std::shared_future<bool> write_buffer(
        const_buffer_t _buffer,
        size_t _size,
        std::filesystem::path& _path
    );
    SIGHT_CORE_API std::shared_future<bool> read_buffer(
        buffer_t _buffer,
        size_t _size,
        std::filesystem::path& _path
    );
    /**  @} */

    /**
     * @brief Returns a boost::signal emitted when an action has been hooked
     *
     * @return
     */
    SPTR(updated_signal_t) get_updated_signal()
    {
        return m_updated_sig;
    };

    /**
     * @brief Returns the Buffer info map
     *
     * @return
     */
    SIGHT_CORE_API std::shared_future<buffer_info_map_t> get_buffer_infos() const;

    /**
     * @brief Returns managed buffers statistics
     */
    SIGHT_CORE_API std::shared_future<buffer_stats> get_buffer_stats() const;
    SIGHT_CORE_API static buffer_stats compute_buffer_stats(const buffer_info_map_t& _buffer_info);

    /**
     * @brief Sets the dump policy
     */
    SIGHT_CORE_API void set_dump_policy(const SPTR(core::memory::policy::base)& _policy);

    /**
     * @brief Returns the dump policy
     */
    SIGHT_CORE_API SPTR(core::memory::policy::base) get_dump_policy() const;

    /**
     * @brief Returns stream info
     */
    SIGHT_CORE_API std::shared_future<stream_info> get_stream_info(const_buffer_ptr_t _buffer_ptr) const;

    SIGHT_CORE_API std::shared_future<void> set_istream_factory(
        buffer_ptr_t _buffer_ptr,
        const SPTR(core::memory::stream::in::factory)& _factory,
        size_t _size,
        core::memory::file_holder _fs_file,
        core::memory::file_format_type _format,
        const core::memory::buffer_allocation_policy::sptr& _policy
    );

    SIGHT_CORE_API loading_mode_type get_loading_mode() const;
    SIGHT_CORE_API void set_loading_mode(loading_mode_type _mode);

    /**
     * @brief Returns the current BufferManager instance
     * @note This method is thread-safe.
     */
    SIGHT_CORE_API static buffer_manager::sptr get();

    //------------------------------------------------------------------------------

    core::mt::read_write_mutex& get_mutex() const
    {
        return m_mutex;
    }

protected:

    /**
     * @brief BufferManager'a Implementation
     * @{ */
    virtual void register_buffer_impl(buffer_ptr_t _buffer_ptr);
    virtual void unregister_buffer_impl(buffer_ptr_t _buffer_ptr);
    virtual void allocate_buffer_impl(
        buffer_ptr_t _buffer_ptr,
        size_t _size,
        const core::memory::buffer_allocation_policy::sptr& _policy
    );
    virtual void set_buffer_impl(
        buffer_ptr_t _buffer_ptr,
        buffer_t _buffer,
        size_t _size,
        const core::memory::buffer_allocation_policy::sptr& _policy
    );
    virtual void reallocate_buffer_impl(buffer_ptr_t _buffer_ptr, size_t _new_size);
    virtual void destroy_buffer_impl(buffer_ptr_t _buffer_ptr);
    virtual void swap_buffer_impl(buffer_ptr_t _buf_a, buffer_ptr_t _buf_b);
    virtual SPTR(void) lock_buffer_impl(const_buffer_ptr_t _buffer_ptr);
    virtual bool unlock_buffer_impl(const_buffer_ptr_t _buffer_ptr);
    virtual std::string to_string_impl() const;
    bool dump_buffer_impl(const_buffer_ptr_t _buffer);
    bool restore_buffer_impl(const_buffer_ptr_t _buffer);
    static bool write_buffer_impl(const_buffer_t _buffer, size_t _size, const std::filesystem::path& _path);
    static bool read_buffer_impl(buffer_t _buffer, size_t _size, const std::filesystem::path& _path);
    buffer_info_map_t get_buffer_infos_impl() const;
    stream_info get_stream_info_impl(const_buffer_ptr_t _buffer_ptr) const;
    void set_istream_factory_impl(
        buffer_ptr_t _buffer_ptr,
        const SPTR(core::memory::stream::in::factory)& _factory,
        size_t _size,
        core::memory::file_holder _fs_file,
        core::memory::file_format_type _format,
        const core::memory::buffer_allocation_policy::sptr& _policy
    );
    /**  @} */

    /**
     * @brief Dump/restore a buffer
     *
     * Used by public dump/restoreBuffer methods
     * @{ */
    SIGHT_CORE_API bool dump_buffer(buffer_info& _info, buffer_ptr_t _buffer_ptr);
    SIGHT_CORE_API bool restore_buffer(buffer_info& _info, buffer_ptr_t _buffer_ptr, size_t _size = 0);
    /**  @} */

    SPTR(updated_signal_t) m_updated_sig;

    core::logic_stamp m_last_access;
    buffer_info_map_t m_buffer_infos;

    SPTR(core::memory::policy::base) m_dump_policy;

    loading_mode_type m_loading_mode {buffer_manager::direct};

    SPTR(core::thread::worker) m_worker;

    /// Mutex to protect concurrent access in BufferManager
    mutable core::mt::read_write_mutex m_mutex;
};

} // namespace sight::core::memory
