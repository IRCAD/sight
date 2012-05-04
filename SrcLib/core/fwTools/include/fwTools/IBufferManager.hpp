/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_IBUFFERMANAGER_HPP_
#define _FWTOOLS_IBUFFERMANAGER_HPP_

#include <fwCore/base.hpp>

#include "fwTools/BufferAllocationPolicy.hpp"
#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @brief IBufferManager Interface
 *
 * Provides interface for a buffer manager. A BufferManager is able to hook
 * BufferObjects actions an to change it's behaviours. Especially, a
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
    FWTOOLS_API virtual bool registerBuffer(void ** buffer, long * lockCount){return true;}

    /**
     * @brief Hook called when a BufferObject is destroyed
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    FWTOOLS_API virtual bool unregisterBuffer(void ** buffer){return true;}

    /**
     * @brief Hook called when an allocation is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     * @param size requested size for allocation
     * @param policy BufferObject's allocation policy
     *
     * @return false if the BufferManager supported the action
     */
    FWTOOLS_API virtual bool allocateBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy){return true;}

    /**
     * @brief Hook called when a request is made to set BufferObject's buffer from an external buffer
     *
     * @param buffer BufferObject's buffer
     * @param size requested size for allocation
     * @param policy BufferObject's allocation policy
     *
     * @return false if the BufferManager supported the action
     */
    FWTOOLS_API virtual bool setBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy){return true;}

    /**
     * @brief Hook called when a reallocation is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     * @param newSize requested size for reallocation
     *
     * @return false if the BufferManager supported the action
     */
    FWTOOLS_API virtual bool reallocateBuffer(void ** buffer, SizeType newSize){return true;}

    /**
     * @brief Hook called when a destruction is requested from a BufferObject
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    FWTOOLS_API virtual bool destroyBuffer(void ** buffer){return true;}

    /**
     * @brief Hook called when a request to swap two BufferObject contents is made
     *
     * @param bufA First BufferObject's buffer
     * @param bufB Second BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    FWTOOLS_API virtual bool swapBuffer(void ** bufA, void ** bufB){return true;}

    /**
     * @brief Hook called when a BufferObject is locked
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    FWTOOLS_API virtual bool lockBuffer(const void * const * buffer){return true;}

    /**
     * @brief Hook called when a BufferObject lock is released
     *
     * @param buffer BufferObject's buffer
     *
     * @return false if the BufferManager supported the action
     */
    FWTOOLS_API virtual bool unlockBuffer(const void * const * buffer){return true;}

    /**
     * @brief Returns the current BufferManager instance
     */
    FWTOOLS_API static IBufferManager::sptr getCurrent(){ return s_currentManager; }

    /**
     * @brief sets the current BufferManager instance
     *
     * @param currentManager BufferManager instance
     *
     */
    FWTOOLS_API static void setCurrent( IBufferManager::sptr currentManager ){ s_currentManager = currentManager; }


    /**
     * @brief returns an information string about BufferManager state
     */
    FWTOOLS_API virtual std::string toString() const { return ""; };

protected:

    FWTOOLS_API IBufferManager(){};
    FWTOOLS_API virtual ~IBufferManager(){};

    static IBufferManager::sptr s_currentManager;
};


}

#endif /* _FWTOOLS_IBUFFERMANAGER_HPP_ */
