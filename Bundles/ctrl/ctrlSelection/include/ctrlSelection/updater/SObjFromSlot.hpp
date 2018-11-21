/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ctrlSelection/config.hpp"
#include "ctrlSelection/IUpdaterSrv.hpp"

namespace ctrlSelection
{

namespace updater
{

/**
 * @brief  Registers/unregisters the object given by the slots.
 *
 * @section Slots Slots
 * - \b add(::fwData::Object::sptr) : Registers the given object in the OSR
 * - \b remove(::fwData::Object::sptr) : Unregisters the object in the OSR
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="::ctrlSelection::updater::SObjFromSlot">
         <out key="object" uid="..." />
     </service>
     @endcode
 * @subsection Output Output
 * - \b object [::fwData::Object]: Object used to register/unregister the object given by the slots.
 */
class CTRLSELECTION_CLASS_API SObjFromSlot : public ::ctrlSelection::IUpdaterSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SObjFromSlot)(::ctrlSelection::IUpdaterSrv) )

    /// Constructor.  Do nothing.
    CTRLSELECTION_API SObjFromSlot() noexcept;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SObjFromSlot() noexcept;

    /**
     * @name Slots
     * @{
     */
    static const ::fwCom::Slots::SlotKeyType s_ADD_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_REMOVE_SLOT;
    /**
     * @}
     */

protected:

    /// Configures the service.
    CTRLSELECTION_API virtual void configuring() override;

    /// Implements starting method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void starting() override;

    /// Implements stopping method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void stopping() override;

    /// Implements updating method derived from IService. Do nothing.
    CTRLSELECTION_API virtual void updating() override;

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream& _sstream ) override;

    /**
     * @name Slots
     * @{
     */
    /// Registers the given object in the OSR
    void add(::fwData::Object::sptr obj);

    /// Adds the object from the composite with the key given by config.
    void remove();
    /**
     * @}
     */
};

} // updater
} // ctrlSelection
