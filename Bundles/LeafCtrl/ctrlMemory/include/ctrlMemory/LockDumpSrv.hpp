/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLMEMORY_LOCKDUMPSRV_HPP_
#define _CTRLMEMORY_LOCKDUMPSRV_HPP_

#include <vector>

#include <fwTools/BufferObject.hpp>

#include <fwData/ObjectLock.hpp>

#include <fwServices/IService.hpp>

#include "ctrlMemory/config.hpp"

namespace ctrlMemory
{

/**
 * @class  LockDumpSrv.
 * @brief  This services dump lock object. Unlock it on stopping.
 * @date   2012-2013.
 */
class CTRLMEMORY_CLASS_API LockDumpSrv : public ::fwServices::IController
{

public :

    fwCoreServiceClassDefinitionsMacro ( (LockDumpSrv)(::fwServices::IController) ) ;

    /// Constructor. Does nothing
    CTRLMEMORY_API LockDumpSrv() throw() ;

    /// Destructor. Does nothing
    CTRLMEMORY_API virtual ~LockDumpSrv() throw() ;

protected:

    /// Uses ::fwData::ObjectLock to dump lock the associated data
    CTRLMEMORY_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Dump unlock the associated data
    CTRLMEMORY_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Does nothing
    CTRLMEMORY_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Does nothing
    CTRLMEMORY_API virtual void updating() throw ( ::fwTools::Failed );

    /// Does nothing
    CTRLMEMORY_API virtual void info( std::ostream &_sstream );

    /// Does nothing
    CTRLMEMORY_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Does nothing
    CTRLMEMORY_API virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private :

    ::fwData::ObjectLock m_objLock;

};

} // ctrlMemory

#endif // _CTRLMEMORY_LOCKDUMPSRV_HPP_
