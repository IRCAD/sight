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

class FWTOOLS_CLASS_API IBufferManager : public ::fwCore::BaseObject
{
public:
    typedef size_t SizeType;

    fwCoreClassDefinitionsWithFactoryMacro((IBufferManager), (()), new IBufferManager );
    fwCoreAllowSharedFromThis();


    FWTOOLS_API virtual bool registerBuffer(void ** buffer, long * lockCount){return true;}
    FWTOOLS_API virtual bool unregisterBuffer(void ** buffer){return true;}

    FWTOOLS_API virtual bool allocateBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy){return true;}
    FWTOOLS_API virtual bool setBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy){return true;}
    FWTOOLS_API virtual bool reallocateBuffer(void ** buffer, SizeType newSize){return true;}
    FWTOOLS_API virtual bool destroyBuffer(void ** buffer){return true;}

    FWTOOLS_API virtual bool lockBuffer(const void * const * buffer){return true;}
    FWTOOLS_API virtual bool unlockBuffer(const void * const * buffer){return true;}

    FWTOOLS_API static IBufferManager::sptr getCurrent(){ return s_currentManager; }
    FWTOOLS_API static void setCurrent( IBufferManager::sptr currentManager ){ s_currentManager = currentManager; }

protected:

    FWTOOLS_API IBufferManager(){};
    FWTOOLS_API virtual ~IBufferManager(){};

    static IBufferManager::sptr s_currentManager;
};


}

#endif /* _FWTOOLS_IBUFFERMANAGER_HPP_ */
