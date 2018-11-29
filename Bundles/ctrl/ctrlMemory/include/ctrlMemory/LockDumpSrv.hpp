/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ctrlMemory/config.hpp"

#include <fwData/ObjectLock.hpp>

#include <fwServices/IController.hpp>

namespace ctrlMemory
{

/**
 * @brief  This services dump lock object. Unlock it on stopping.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ctrlMemory::LockDumpSrv">
       <inout key="target" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b target [::fwData::Object]: object to dump lock.
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
