/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/core/data/IUpdaterSrv.hpp"
#include "modules/data/config.hpp"

namespace sight::module::data
{

namespace updater
{

/**
 * @brief  Registers/unregisters the object given by the slots.
 *
 * @section Slots Slots
 * - \b add(data::Object::sptr) : Registers the given object in the OSR
 * - \b remove(data::Object::sptr) : Unregisters the object in the OSR
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="sight::module::data::updater::SObjFromSlot">
         <out key="object" uid="..." />
     </service>
     @endcode
 * @subsection Output Output
 * - \b object [sight::data::Object]: Object used to register/unregister the object given by the slots.
 */
class MODULE_DATA_CLASS_API SObjFromSlot : public module::data::IUpdaterSrv
{

public:

    SIGHT_DECLARE_SERVICE(SObjFromSlot, ::sight::module::data::IUpdaterSrv)

    /// Constructor.  Do nothing.
    MODULE_DATA_API SObjFromSlot() noexcept;

    /// Destructor. Do nothing.
    MODULE_DATA_API virtual ~SObjFromSlot() noexcept;

    /**
     * @name Slots
     * @{
     */
    static const core::com::Slots::SlotKeyType s_ADD_SLOT;
    static const core::com::Slots::SlotKeyType s_REMOVE_SLOT;
    /**
     * @}
     */

protected:

    /// Configures the service.
    MODULE_DATA_API virtual void configuring() override;

    /// Implements starting method derived from IService. Do nothing.
    MODULE_DATA_API virtual void starting() override;

    /// Implements stopping method derived from IService. Do nothing.
    MODULE_DATA_API virtual void stopping() override;

    /// Implements updating method derived from IService. Do nothing.
    MODULE_DATA_API virtual void updating() override;

    /// Implements info method derived from IService. Print classname.
    MODULE_DATA_API virtual void info( std::ostream& _sstream ) override;

    /**
     * @name Slots
     * @{
     */
    /// Registers the given object in the OSR
    void add(sight::data::Object::sptr obj);

    /// Adds the object from the composite with the key given by config.
    void remove();
    /**
     * @}
     */
};

} // updater
} // sight::module::data
