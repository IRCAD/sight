/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_BUFFERMANAGER_HPP_
#define _FWMEMORY_BUFFERMANAGER_HPP_

#include <boost/filesystem/path.hpp>

#include <boost/signal.hpp>

#include <fwCore/base.hpp>

#include "fwMemory/IBufferManager.hpp"
#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/IPolicy.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

/**
 * @brief BufferManager implementation
 *
 * This implementation purpose is to manage memory load, freeing memory and
 * restoring freed buffers as needed.
 *
 * A dump policy is used to trigger memory freeing process. The restore process
 * is always triggers when a lock is requested on a dumped buffer.
 */
class FWMEMORY_CLASS_API BufferManager : public ::fwMemory::IBufferManager
{

public:

    typedef ::fwMemory::IBufferManager::SizeType SizeType;

    typedef ::boost::signal<void ()> UpdatedSignalType;
    typedef ::fwMemory::BufferInfo BufferInfo;
    typedef BufferInfo::MapType BufferInfoMapType;

    fwCoreClassDefinitionsWithFactoryMacro((BufferManager), (()), new BufferManager );
    fwCoreAllowSharedFromThis();

    /**
     * @name implementation of ::fwMemory::IBufferManager hooks
     * @{ */

    FWMEMORY_API virtual bool registerBuffer(::fwMemory::IBufferManager::BufferPtrType buffer,
                                             IBufferManager::LockCountFunctionType lockCount);
    FWMEMORY_API virtual bool unregisterBuffer(::fwMemory::IBufferManager::BufferPtrType buffer);

    FWMEMORY_API virtual bool allocateBuffer(::fwMemory::IBufferManager::BufferPtrType buffer, SizeType size,
                                             ::fwMemory::BufferAllocationPolicy::sptr policy);
    FWMEMORY_API virtual bool setBuffer(::fwMemory::IBufferManager::BufferPtrType buffer, SizeType size,
                                        ::fwMemory::BufferAllocationPolicy::sptr policy);
    FWMEMORY_API virtual bool reallocateBuffer(::fwMemory::IBufferManager::BufferPtrType buffer, SizeType newSize);
    FWMEMORY_API virtual bool destroyBuffer(::fwMemory::IBufferManager::BufferPtrType buffer);

    FWMEMORY_API virtual bool swapBuffer(::fwMemory::IBufferManager::BufferPtrType bufA,
                                         ::fwMemory::IBufferManager::BufferPtrType bufB);

    FWMEMORY_API virtual bool lockBuffer(::fwMemory::IBufferManager::ConstBufferPtrType buffer);
    FWMEMORY_API virtual bool unlockBuffer(::fwMemory::IBufferManager::ConstBufferPtrType buffer);

    /**  @} */

    FWMEMORY_API virtual std::string toString() const;


    /**
     * @brief Dump/restore a buffer
     *
     * Keeps up to date buffer's information
     *
     * @param buffer Buffer to dump/restore
     *
     * @return true on success
     * @{ */
    FWMEMORY_API bool dumpBuffer(::fwMemory::IBufferManager::ConstBufferPtrType  buffer);
    FWMEMORY_API bool restoreBuffer(::fwMemory::IBufferManager::ConstBufferPtrType  buffer);
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
    FWMEMORY_API bool writeBuffer(::fwMemory::IBufferManager::ConstBufferType buffer, SizeType size,
                                  ::boost::filesystem::path &path);
    FWMEMORY_API bool readBuffer(::fwMemory::IBufferManager::BufferType buffer, SizeType size,
                                 ::boost::filesystem::path &path);
    /**  @} */


    /**
     * @brief Returns a boost::signal emitted when an action has been hooked
     *
     * @return
     */
    UpdatedSignalType &getUpdatedSignal(){return m_updated;};


    /**
     * @brief Returns the Buffer info map
     *
     * @return
     */
    BufferInfoMapType & getBufferInfos() // UARGL
    {
        return m_bufferInfos;
    }

    /**
     * @brief Returns the Buffer info map
     *
     * @return
     */
    const BufferInfoMapType & getBufferInfos() const
    {
        return m_bufferInfos;
    }

    /**
     * @brief Returns the amount of currently dumped memory
     */
    FWMEMORY_API SizeType getDumpedBufferSize() const;

    /**
     * @brief Returns the amount of managed memory
     */
    FWMEMORY_API SizeType getManagedBufferSize() const;

    /**
     * @brief Sets the dump policy
     */
    FWMEMORY_API void setDumpPolicy( ::fwMemory::IPolicy::sptr policy );

    /**
     * @brief Returns the dump policy
     */
    FWMEMORY_API ::fwMemory::IPolicy::sptr getDumpPolicy() const;


    /**
     * @brief Returns true if 'buffer' is dumped
     */
    FWMEMORY_API bool isDumped(const ::fwMemory::IBufferManager::ConstBufferPtrType buffer) const;

    /**
     * @brief Returns the path of the file containing the dumped buffer
     */
    FWMEMORY_API ::boost::filesystem::path
        getDumpedFilePath(const ::fwMemory::IBufferManager::ConstBufferPtrType buffer) const;

protected:

    FWMEMORY_API BufferManager();
    FWMEMORY_API virtual ~BufferManager();

    /**
     * @brief Dump/restore a buffer
     *
     * Used by public dump/restoreBuffer methods
     * @{ */
    FWMEMORY_API bool dumpBuffer(BufferInfo & info, ::fwMemory::IBufferManager::BufferPtrType buffer);
    FWMEMORY_API bool restoreBuffer(BufferInfo & info, ::fwMemory::IBufferManager::BufferPtrType buffer,
                                    SizeType size = 0);
    /**  @} */


    UpdatedSignalType m_updated;

    ::fwCore::LogicStamp m_lastAccess;
    BufferInfoMapType m_bufferInfos;

    ::fwMemory::IPolicy::sptr m_dumpPolicy;
};


}

#endif /* _FWMEMORY_BUFFERMANAGER_HPP_ */
