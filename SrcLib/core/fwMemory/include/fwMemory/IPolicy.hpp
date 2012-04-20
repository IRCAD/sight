/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_IPOLICY_HPP_
#define FWMEMORY_IPOLICY_HPP_

#include <boost/shared_ptr.hpp>

#include <fwTools/IBufferManager.hpp>

#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

/**
 * @brief Define the memory dump policy interface
 */
class FWMEMORY_CLASS_API IPolicy
{
public :

    typedef SPTR(IPolicy) sptr;

    // FWMEMORY_API IPolicy() ;
    // FWMEMORY_API virtual ~IPolicy() ;


    virtual void allocationRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size ) = 0 ;
    virtual void setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size ) = 0 ;
    virtual void reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize ) = 0 ;
    virtual void destroyRequest( BufferInfo &info, void **buffer ) = 0 ;
    virtual void lockRequest( BufferInfo &info, void **buffer ) = 0 ;
    virtual void unlockRequest( BufferInfo &info, void **buffer ) = 0 ;

    virtual void dumpSuccess( BufferInfo &info, void **buffer ) = 0 ;
    virtual void restoreSuccess( BufferInfo &info, void **buffer ) = 0 ;

    virtual void setManager(::fwTools::IBufferManager::sptr manager) = 0;
};

} // namespace fwMemory

#endif // FWMEMORY_IPOLICY_HPP_

