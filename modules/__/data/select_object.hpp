/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "service/controller.hpp"

namespace sight::module::data
{

/**
 * @brief  Registers or unregisters an single output object using the add/remove slots.
 * This service is mainly used to select an object.
 *
 * @section Slots Slots
 * - \b add(data::object::sptr) : Registers the given object.
 * - \b remove(data::object::sptr) : Unregisters the object.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service type="sight::module::data::select_object">
         <out key="object" uid="..." />
     </service>
     @endcode
 * @subsection Output Output
 * - \b object [sight::data::object]: Object used to register/unregister the object given by the slots.
 */
class select_object : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(select_object, service::controller);

    /// Constructor.  Do nothing.
    select_object() noexcept;

    /// Destructor. Do nothing.
    ~select_object() noexcept override;

    /**
     * @name Slots
     * @{
     */
    static const core::com::slots::key_t ADD_SLOT;
    static const core::com::slots::key_t REMOVE_SLOT;
    /// @}

protected:

    /// Configures the service.
    void configuring() override;

    /// Implements starting method derived from base. Do nothing.
    void starting() override;

    /// Implements stopping method derived from base. Do nothing.
    void stopping() override;

    /// Implements updating method derived from base. Do nothing.
    void updating() override;

    /// Implements info method derived from base. Print classname.
    void info(std::ostream& _sstream) override;

    /**
     * @name Slots
     * @{
     */
    /// Registers the given object in the OSR
    void add(sight::data::object::sptr _obj);

    /// Adds the object from the map with the key given by config.
    void remove();
    /// @}

private:

    sight::data::ptr<sight::data::object, sight::data::access::out> m_object {this, "object"};
};

} // namespace sight::module::data
