/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLMEMORY_LOCKDUMPSRV_HPP__
#define __CTRLMEMORY_LOCKDUMPSRV_HPP__

#include "ctrlMemory/config.hpp"

#include <fwData/ObjectLock.hpp>

#include <fwServices/IController.hpp>

namespace ctrlMemory
{

/**
 * @brief  This services dump lock object. Unlock it on stopping.
 */
class CTRLMEMORY_CLASS_API LockDumpSrv : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro( (LockDumpSrv)(::fwServices::IController) );

    /// Constructor. Does nothing
    CTRLMEMORY_API LockDumpSrv() noexcept;

    /// Destructor. Does nothing
    CTRLMEMORY_API virtual ~LockDumpSrv() noexcept;

protected:

    /// Uses ::fwData::ObjectLock to dump lock the associated data
    CTRLMEMORY_API virtual void starting() override;

    /// Dump unlock the associated data
    CTRLMEMORY_API virtual void stopping() override;

    /// Does nothing
    CTRLMEMORY_API virtual void reconfiguring() override;

    /// Does nothing
    CTRLMEMORY_API virtual void updating() override;

    /// Does nothing
    CTRLMEMORY_API virtual void info( std::ostream& _sstream ) override;

    /// Does nothing
    CTRLMEMORY_API virtual void configuring() override;

private:

    ::fwData::ObjectLock m_objLock;

};

} // ctrlMemory

#endif // __CTRLMEMORY_LOCKDUMPSRV_HPP__

