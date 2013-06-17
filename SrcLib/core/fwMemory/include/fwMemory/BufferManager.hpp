/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_BUFFERMANAGER_HPP__
#define __FWMEMORY_BUFFERMANAGER_HPP__

#include <boost/filesystem/path.hpp>

#include <boost/signal.hpp>

#include <fwCore/base.hpp>
#include <fwCore/mt/types.hpp>
#include <fwCore/BaseObject.hpp>

#include "fwMemory/FileHolder.hpp"
#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

class IPolicy;

namespace stream {
namespace in {
class IFactory ;
}}

/**
 * @brief BufferManager implementation
 *
 * This implementation purpose is to manage memory load, freeing memory and
 * restoring freed buffers as needed.
 *
 * A dump policy is used to trigger memory freeing process. The restore process
 * is always triggers when a lock is requested on a dumped buffer.
 */
class FWMEMORY_CLASS_API BufferManager : public ::fwCore::BaseObject
{

public:

    typedef void* BufferType;
    typedef const void* ConstBufferType;
    typedef BufferType* BufferPtrType;
    typedef void const * const * ConstBufferPtrType;

    typedef BufferInfo::SizeType SizeType;


    typedef ::boost::signal<void ()> UpdatedSignalType;

    typedef std::map< ConstBufferPtrType, BufferInfo > BufferInfoMapType;

    fwCoreClassDefinitionsWithFactoryMacro((BufferManager), (()), new BufferManager );
    fwCoreAllowSharedFromThis();


    typedef enum {
        DIRECT,
        LAZY
    } LoadingModeType;

    /**
     * @brief Hook called when a new BufferObject is created
     *
     * @param buffer BufferObject's buffer
     * @param lockCount BufferObject lock counter.
     */
    FWMEMORY_API virtual void registerBuffer(BufferPtrType bufferPtr);

    /**
     * @brief Hook called when a BufferObject is destroyed
     *
     * @param buffer BufferObject's buffer
     */
    FWMEMORY_API virtual void unregisterBuffer(BufferPtrType bufferPtr);

    /**
     * @brief Hook called when an allocation is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     * @param size requested size for allocation
     * @param policy BufferObject's allocation policy
     */
    FWMEMORY_API virtual void allocateBuffer(BufferPtrType bufferPtr, SizeType size,
                                             const ::fwMemory::BufferAllocationPolicy::sptr &policy) ;

    /**
     * @brief Hook called when a request is made to set BufferObject's buffer from an external buffer
     *
     * @param bufferPtr BufferObject's buffer pointer
     * @param buffer new buffer
     * @param size requested size for allocation
     * @param policy BufferObject's allocation policy
     */
    FWMEMORY_API virtual void setBuffer(BufferPtrType bufferPtr, ::fwMemory::BufferManager::BufferType buffer,
                                        SizeType size, const ::fwMemory::BufferAllocationPolicy::sptr &policy) ;

    /**
     * @brief Hook called when a reallocation is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     * @param newSize requested size for reallocation
     */
    FWMEMORY_API virtual void reallocateBuffer(BufferPtrType bufferPtr, SizeType newSize) ;


    /**
     * @brief Hook called when a destruction is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     */
    FWMEMORY_API virtual void destroyBuffer(BufferPtrType bufferPtr) ;


    /**
     * @brief Hook called when a request to swap two BufferObject contents is made
     *
     * @param bufA First BufferObject's buffer
     * @param bufB Second BufferObject's buffer
     */
    FWMEMORY_API virtual void swapBuffer(BufferPtrType bufA, BufferPtrType bufB) ;


    /**
     * @brief Hook called when a BufferObject is locked
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    FWMEMORY_API virtual bool lockBuffer(ConstBufferPtrType bufferPtr, const BufferInfo::CounterType &lockCount) ;


    /**
     * @brief Hook called when a BufferObject lock is released
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    FWMEMORY_API virtual bool unlockBuffer(ConstBufferPtrType bufferPtr) ;


    /**
     * @brief returns BufferManager status string
     */
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
    FWMEMORY_API bool dumpBuffer(ConstBufferPtrType  buffer);
    FWMEMORY_API bool restoreBuffer(ConstBufferPtrType  buffer);
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
    FWMEMORY_API bool writeBuffer(ConstBufferType buffer, SizeType size,
                                  ::boost::filesystem::path &path);
    FWMEMORY_API bool readBuffer(BufferType buffer, SizeType size,
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
    FWMEMORY_API void setDumpPolicy( const SPTR(::fwMemory::IPolicy) &policy );

    /**
     * @brief Returns the dump policy
     */
    FWMEMORY_API SPTR(::fwMemory::IPolicy) getDumpPolicy() const;


    /**
     * @brief Returns true if 'buffer' is loaded
     */
    FWMEMORY_API bool isLoaded(const ConstBufferPtrType bufferPtr) const;

    /**
     * @brief Returns the path of the file containing the dumped buffer
     */
    FWMEMORY_API ::boost::filesystem::path getDumpedFilePath(const ConstBufferPtrType bufferPtr) const;

    /**
     * @brief Returns the format of the dumped file
     */
    FWMEMORY_API ::fwMemory::FileFormatType getDumpedFileFormat(const ConstBufferPtrType bufferPtr) const;

    /**
     * @brief Returns the current BufferManager instance
     * @note This method is thread-safe.
     */
    FWMEMORY_API static BufferManager::sptr getCurrent();

    /**
     * @brief sets the current BufferManager instance
     *
     * @param currentManager BufferManager instance
     * @note This method is thread-safe.
     */
    FWMEMORY_API static void setCurrent( const BufferManager::sptr &currentManager );

    FWMEMORY_API SPTR(std::istream) getIStream(const ConstBufferPtrType bufferPtr) const;

    FWMEMORY_API void setIStreamFactory(BufferPtrType bufferPtr,
                                        const SPTR(::fwMemory::stream::in::IFactory) &factory,
                                        SizeType size,
                                        ::fwMemory::FileHolder fsFile,
                                        ::fwMemory::FileFormatType format,
                                        const ::fwMemory::BufferAllocationPolicy::sptr &policy
                                       );

    bool hasUserStreamFactory(const BufferManager::ConstBufferPtrType bufferPtr) const;

    LoadingModeType getLoadingMode() const { return m_loadingMode; }
    void setLoadingMode(LoadingModeType mode) { m_loadingMode = mode; }

protected:

    FWMEMORY_API BufferManager();
    FWMEMORY_API virtual ~BufferManager();

    /**
     * @brief Dump/restore a buffer
     *
     * Used by public dump/restoreBuffer methods
     * @{ */
    FWMEMORY_API bool dumpBuffer(BufferInfo & info, BufferPtrType bufferPtr);
    FWMEMORY_API bool restoreBuffer(BufferInfo & info, BufferPtrType bufferPtr, SizeType size = 0);
    /**  @} */


    UpdatedSignalType m_updated;

    ::fwCore::LogicStamp m_lastAccess;
    BufferInfoMapType m_bufferInfos;

    SPTR(::fwMemory::IPolicy) m_dumpPolicy;

    LoadingModeType m_loadingMode;


    FWMEMORY_API static BufferManager::sptr s_currentManager;

    static ::fwCore::mt::ReadWriteMutex s_mutex;
};


}

#endif /* __FWMEMORY_BUFFERMANAGER_HPP__ */
