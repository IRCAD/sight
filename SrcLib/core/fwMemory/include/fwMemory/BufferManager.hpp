/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_BUFFERMANAGER_HPP_
#define _FWMEMORY_BUFFERMANAGER_HPP_

#include <boost/filesystem/path.hpp>

#include <boost/signal.hpp>

#include <fwCore/base.hpp>

#include <fwTools/IBufferManager.hpp>

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
class FWMEMORY_CLASS_API BufferManager : public ::fwTools::IBufferManager
{

public:

    typedef ::fwTools::IBufferManager::SizeType SizeType;

    typedef ::boost::signal<void ()> UpdatedSignalType;
    typedef ::fwMemory::BufferInfo BufferInfo;
    typedef BufferInfo::MapType BufferInfoMapType;

    fwCoreClassDefinitionsWithFactoryMacro((BufferManager), (()), new BufferManager );
    fwCoreAllowSharedFromThis();

    /**
     * @name implementation of ::fwTools::IBufferManager hooks
     * @{ */

    FWMEMORY_API virtual bool registerBuffer(void ** buffer, long * lockCount);
    FWMEMORY_API virtual bool unregisterBuffer(void ** buffer);

    FWMEMORY_API virtual bool allocateBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy);
    FWMEMORY_API virtual bool setBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy);
    FWMEMORY_API virtual bool reallocateBuffer(void ** buffer, SizeType newSize);
    FWMEMORY_API virtual bool destroyBuffer(void ** buffer);

    FWMEMORY_API virtual bool swapBuffer(void ** bufA, void ** bufB);

    FWMEMORY_API virtual bool lockBuffer(const void * const * buffer);
    FWMEMORY_API virtual bool unlockBuffer(const void * const * buffer);

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
    FWMEMORY_API bool dumpBuffer(const void * const *  buffer);
    FWMEMORY_API bool restoreBuffer(const void * const *  buffer);
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
    FWMEMORY_API bool writeBuffer(const void * buffer, SizeType size, ::boost::filesystem::path &path);
    FWMEMORY_API bool readBuffer(void * buffer, SizeType size, ::boost::filesystem::path &path);
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
    FWMEMORY_API bool isDumped(const void * const * const  buffer) const;

    /**
     * @brief Returns the path of the file containing the dumped buffer
     */
    FWMEMORY_API ::boost::filesystem::path getDumpedFilePath(const void * const * const  buffer) const;

protected:

    FWMEMORY_API BufferManager();
    FWMEMORY_API virtual ~BufferManager();

    /**
     * @brief Dump/restore a buffer
     *
     * Used by public dump/restoreBuffer methods
     * @{ */
    FWMEMORY_API bool dumpBuffer(BufferInfo & info, void ** buffer);
    FWMEMORY_API bool restoreBuffer(BufferInfo & info, void ** buffer, SizeType size = 0);
    /**  @} */


    UpdatedSignalType m_updated;

    ::fwCore::LogicStamp m_lastAccess;
    BufferInfoMapType m_bufferInfos;

    ::fwMemory::IPolicy::sptr m_dumpPolicy;
};


}

#endif /* _FWMEMORY_BUFFERMANAGER_HPP_ */
