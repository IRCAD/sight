/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "core/config.hpp"
#include "core/memory/BufferInfo.hpp"
#include "core/memory/FileHolder.hpp"
#include <core/base.hpp>
#include <core/BaseObject.hpp>
#include <core/com/Signal.hpp>
#include <core/mt/types.hpp>

#include <filesystem>
#include <future>

namespace fwThread
{
class Worker;
}

namespace sight::core::memory
{

class IPolicy;

namespace stream
{
namespace in
{
class IFactory;
}
}

class BufferManager;

class Key
{
friend SPTR(BufferManager) getDefault();

Key()
{
}
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
class CORE_CLASS_API BufferManager : public core::BaseObject
{
public:

    typedef void* BufferType;
    typedef const void* ConstBufferType;
    typedef BufferType* BufferPtrType;
    typedef void const* const* ConstBufferPtrType;

    typedef BufferInfo::SizeType SizeType;

    typedef core::com::Signal< void () > UpdatedSignalType;

    typedef std::map< ConstBufferPtrType, BufferInfo > BufferInfoMapType;

    fwCoreClassMacro(BufferManager, core::BaseObject)
    fwCoreAllowSharedFromThis();

    BufferManager();
    virtual ~BufferManager();

    typedef enum
    {
        DIRECT,
        LAZY
    } LoadingModeType;

    struct BufferStats
    {
        SizeType totalDumped;
        SizeType totalManaged;
    };

    struct StreamInfo
    {
        SizeType size;
        SPTR(std::istream) stream;
        /// path of the file containing the dumped buffer
        core::memory::FileHolder fsFile;
        /// format of the dumped file
        core::memory::FileFormatType format;
        /// true if stream has been set by the user
        bool userStream;
    };

    /**
     * @brief Hook called when a new BufferObject is created
     *
     * @param bufferPtr BufferObject's buffer pointer.
     */
    CORE_API virtual std::shared_future<void> registerBuffer(BufferPtrType bufferPtr);

    /**
     * @brief Hook called when a BufferObject is destroyed
     *
     * @param bufferPtr BufferObject's buffer pointer.
     */
    CORE_API virtual std::shared_future<void> unregisterBuffer(BufferPtrType bufferPtr);

    /**
     * @brief Hook called when an allocation is requested from a BufferObject
     *
     * @param bufferPtr BufferObject's buffer pointer
     * @param size requested size for allocation
     * @param policy BufferObject's allocation policy
     */
    CORE_API virtual std::shared_future<void> allocateBuffer(BufferPtrType bufferPtr, SizeType size,
                                                             const core::memory::BufferAllocationPolicy::sptr& policy);

    /**
     * @brief Hook called when a request is made to set BufferObject's buffer from an external buffer
     *
     * @param bufferPtr BufferObject's buffer pointer
     * @param buffer new buffer
     * @param size requested size for allocation
     * @param policy BufferObject's allocation policy
     */
    CORE_API virtual std::shared_future<void> setBuffer(BufferPtrType bufferPtr,
                                                        core::memory::BufferManager::BufferType buffer,
                                                        SizeType size,
                                                        const core::memory::BufferAllocationPolicy::sptr& policy);

    /**
     * @brief Hook called when a reallocation is requested from a BufferObject
     *
     * @param bufferPtr BufferObject's buffer pointer
     * @param newSize requested size for reallocation
     */
    CORE_API virtual std::shared_future<void> reallocateBuffer(BufferPtrType bufferPtr, SizeType newSize);

    /**
     * @brief Hook called when a destruction is requested from a BufferObject
     *
     * @param bufferPtr BufferObject's buffer pointer
     */
    CORE_API virtual std::shared_future<void> destroyBuffer(BufferPtrType bufferPtr);

    /**
     * @brief Hook called when a request to swap two BufferObject contents is made
     *
     * @param bufA First BufferObject's buffer
     * @param bufB Second BufferObject's buffer
     */
    CORE_API virtual std::shared_future<void> swapBuffer(BufferPtrType bufA, BufferPtrType bufB);

    /**
     * @brief Hook called when a BufferObject is locked
     *
     * @param bufferPtr BufferObject's buffer pointer
     *
     * @return false if the BufferManager supported the action
     */
    CORE_API virtual std::shared_future<SPTR(void)> lockBuffer(ConstBufferPtrType bufferPtr);

    /**
     * @brief Hook called when a BufferObject lock is released
     *
     * @param bufferPtr BufferObject's buffer pointer
     *
     * @return false if the BufferManager supported the action
     */
    CORE_API virtual std::shared_future<bool> unlockBuffer(ConstBufferPtrType bufferPtr);

    /**
     * @brief returns BufferManager status string
     */
    CORE_API virtual std::shared_future<std::string> toString() const;

    /**
     * @brief Dump/restore a buffer
     *
     * Keeps up to date buffer's information
     *
     * @param bufferPtr Buffer to dump/restore
     *
     * @return true on success
     * @{ */
    CORE_API std::shared_future<bool> dumpBuffer(ConstBufferPtrType bufferPtr);
    CORE_API std::shared_future<bool> restoreBuffer(ConstBufferPtrType bufferPtr);
    /**  @} */

    /**
     * @brief Write/read a buffer
     *
     * The buffer is written/read in a raw format
     *
     * @param buffer Buffer to write/read
     * @param size Buffer's size
     * @param path file to  write/read
     *
     * @return true on success
     * @{ */
    CORE_API std::shared_future<bool> writeBuffer(ConstBufferType buffer, SizeType size,
                                                  std::filesystem::path& path);
    CORE_API std::shared_future<bool> readBuffer(BufferType buffer, SizeType size,
                                                 std::filesystem::path& path);
    /**  @} */

    /**
     * @brief Returns a boost::signal emitted when an action has been hooked
     *
     * @return
     */
    SPTR(UpdatedSignalType) getUpdatedSignal(){
        return m_updatedSig;
    };

    /**
     * @brief Returns the Buffer info map
     *
     * @return
     */
    CORE_API std::shared_future<BufferInfoMapType> getBufferInfos() const;

    /**
     * @brief Returns managed buffers statistics
     */
    CORE_API std::shared_future<BufferStats> getBufferStats() const;
    CORE_API static BufferStats computeBufferStats(const BufferInfoMapType& bufferInfo);

    /**
     * @brief Sets the dump policy
     */
    CORE_API void setDumpPolicy( const SPTR(core::memory::IPolicy)& policy );

    /**
     * @brief Returns the dump policy
     */
    CORE_API SPTR(core::memory::IPolicy) getDumpPolicy() const;

    /**
     * @brief Returns stream info
     */
    CORE_API std::shared_future<StreamInfo> getStreamInfo(const ConstBufferPtrType bufferPtr) const;

    CORE_API std::shared_future<void> setIStreamFactory(BufferPtrType bufferPtr,
                                                        const SPTR(core::memory::stream::in::IFactory)& factory,
                                                        SizeType size,
                                                        core::memory::FileHolder fsFile,
                                                        core::memory::FileFormatType format,
                                                        const core::memory::BufferAllocationPolicy::sptr& policy
                                                        );

    CORE_API LoadingModeType getLoadingMode() const;
    CORE_API void setLoadingMode(LoadingModeType mode);

    /**
     * @brief Returns the current BufferManager instance
     * @note This method is thread-safe.
     */
    CORE_API static BufferManager::sptr getDefault();

    //------------------------------------------------------------------------------

    core::mt::ReadWriteMutex& getMutex() const
    {
        return m_mutex;
    }
protected:

    /**
     * @brief BufferManager'a Implementation
     * @{ */
    virtual void registerBufferImpl(BufferPtrType bufferPtr);
    virtual void unregisterBufferImpl(BufferPtrType bufferPtr);
    virtual void allocateBufferImpl(BufferPtrType bufferPtr, SizeType size,
                                    const core::memory::BufferAllocationPolicy::sptr& policy);
    virtual void setBufferImpl(BufferPtrType bufferPtr, core::memory::BufferManager::BufferType buffer, SizeType size,
                               const core::memory::BufferAllocationPolicy::sptr& policy);
    virtual void reallocateBufferImpl(BufferPtrType bufferPtr, SizeType newSize);
    virtual void destroyBufferImpl(BufferPtrType bufferPtr);
    virtual void swapBufferImpl(BufferPtrType bufA, BufferPtrType bufB);
    virtual SPTR(void) lockBufferImpl(ConstBufferPtrType bufferPtr);
    virtual bool unlockBufferImpl(ConstBufferPtrType bufferPtr);
    virtual std::string toStringImpl() const;
    bool dumpBufferImpl(ConstBufferPtrType buffer);
    bool restoreBufferImpl(ConstBufferPtrType buffer);
    bool writeBufferImpl(ConstBufferType buffer, SizeType size, std::filesystem::path& path);
    bool readBufferImpl(BufferType buffer, SizeType size, std::filesystem::path& path);
    BufferInfoMapType getBufferInfosImpl() const;
    StreamInfo getStreamInfoImpl(const ConstBufferPtrType bufferPtr) const;
    void setIStreamFactoryImpl(BufferPtrType bufferPtr,
                               const SPTR(core::memory::stream::in::IFactory)& factory,
                               SizeType size,
                               core::memory::FileHolder fsFile,
                               core::memory::FileFormatType format,
                               const core::memory::BufferAllocationPolicy::sptr& policy
                               );
    /**  @} */

    /**
     * @brief Dump/restore a buffer
     *
     * Used by public dump/restoreBuffer methods
     * @{ */
    CORE_API bool dumpBuffer(BufferInfo& info, BufferPtrType bufferPtr);
    CORE_API bool restoreBuffer(BufferInfo& info, BufferPtrType bufferPtr, SizeType size = 0);
    /**  @} */

    SPTR(UpdatedSignalType) m_updatedSig;

    core::LogicStamp m_lastAccess;
    BufferInfoMapType m_bufferInfos;

    SPTR(core::memory::IPolicy) m_dumpPolicy;

    LoadingModeType m_loadingMode;

    SPTR(core::thread::Worker) m_worker;

    /// Mutex to protect concurrent access in BufferManager
    mutable core::mt::ReadWriteMutex m_mutex;
};

}
