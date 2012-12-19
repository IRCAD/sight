/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_IBUFFERMANAGER_HPP_
#define _FWTOOLS_IBUFFERMANAGER_HPP_

#include <boost/function.hpp>

#include <fwCore/base.hpp>
#include <fwCore/mt/types.hpp>

#include "fwTools/BufferAllocationPolicy.hpp"
#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @brief IBufferManager Interface
 *
 * Provides interface for a buffer manager. A BufferManager is able to hook
 * BufferObjects actions an to change it's behaviors. Especially, a
 * BufferObjects can delegate allocation, reallocation and destruction to a
 * BufferManager.
 *
 * A BufferManager can be aware of BufferObjects usages thanks to lock/unlock
 * hooks.
 *
 * For each hook, the BufferManager return a boolean. If the BufferManager did
 * nothing, true is returned and the BufferObject is informed that it may behave
 * as usual. If false is returned, the BufferObject is informed that the action
 * have been delegated to the BufferManager.
 */
class FWTOOLS_CLASS_API IBufferManager : public ::fwCore::BaseObject
{
public:
    typedef size_t SizeType;
    typedef boost::function< long() > LockCountFunctionType;


    fwCoreClassDefinitionsWithFactoryMacro((IBufferManager), (()), new IBufferManager );
    fwCoreAllowSharedFromThis();


    /**
     * @brief Hook called when a new BufferObject is created
     *
     * @param buffer BufferObject's buffer
     * @param lockCount BufferObject lock counter.
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool registerBuffer(void ** buffer, LockCountFunctionType lockCount){
        FwCoreNotUsedMacro(buffer);
        FwCoreNotUsedMacro(lockCount);
        return true;
    }

    /**
     * @brief Hook called when a BufferObject is destroyed
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool unregisterBuffer(void ** buffer){
        FwCoreNotUsedMacro(buffer);
        return true;
    }

    /**
     * @brief Hook called when an allocation is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     * @param size requested size for allocation
     * @param policy BufferObject's allocation policy
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool allocateBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy)
    {
        FwCoreNotUsedMacro(buffer);
        FwCoreNotUsedMacro(size);
        FwCoreNotUsedMacro(policy);
        return true;
    }

    /**
     * @brief Hook called when a request is made to set BufferObject's buffer from an external buffer
     *
     * @param buffer BufferObject's buffer
     * @param size requested size for allocation
     * @param policy BufferObject's allocation policy
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool setBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy)
    {
        FwCoreNotUsedMacro(buffer);
        FwCoreNotUsedMacro(size);
        FwCoreNotUsedMacro(policy);
        return true;
    }

    /**
     * @brief Hook called when a reallocation is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     * @param newSize requested size for reallocation
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool reallocateBuffer(void ** buffer, SizeType newSize)
    {
        FwCoreNotUsedMacro(buffer);
        FwCoreNotUsedMacro(newSize);
        return true;
    }


    /**
     * @brief Hook called when a destruction is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool destroyBuffer(void ** buffer)
    {
        FwCoreNotUsedMacro(buffer);
        return true;
    }


    /**
     * @brief Hook called when a request to swap two BufferObject contents is made
     *
     * @param bufA First BufferObject's buffer
     * @param bufB Second BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool swapBuffer(void ** bufA, void ** bufB)
    {
        FwCoreNotUsedMacro(bufA);
        FwCoreNotUsedMacro(bufB);
        return true;
    }


    /**
     * @brief Hook called when a BufferObject is locked
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool lockBuffer(const void * const * buffer)
    {
        FwCoreNotUsedMacro(buffer);
        return true;
    }


    /**
     * @brief Hook called when a BufferObject lock is released
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    virtual bool unlockBuffer(const void * const * buffer)
    {
        FwCoreNotUsedMacro(buffer);
        return true;
    }


    /**
     * @brief Returns the current BufferManager instance
     * @note This method is thread-safe.
     */
    FWTOOLS_API static IBufferManager::sptr getCurrent();

    /**
     * @brief sets the current BufferManager instance
     *
     * @param currentManager BufferManager instance
     * @note This method is thread-safe.
     */
    FWTOOLS_API static void setCurrent( IBufferManager::sptr currentManager );


    /**
     * @brief returns an information string about BufferManager state
     */
    virtual std::string toString() const { return ""; };


protected:

    IBufferManager(){};
    virtual ~IBufferManager(){};

    FWTOOLS_API static IBufferManager::sptr s_currentManager;

    static ::fwCore::mt::ReadWriteMutex s_mutex;
};


}

#endif /* _FWTOOLS_IBUFFERMANAGER_HPP_ */
