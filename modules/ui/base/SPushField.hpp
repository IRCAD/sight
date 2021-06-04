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

#include "modules/ui/base/config.hpp"

#include <core/tools/Failed.hpp>

#include <ui/base/IAction.hpp>

#include <map>
#include <set>

namespace sight::module::ui::base
{

/**
 * @brief   To add or remove an object in composite with specific key.
 *
 * @section XML Configuration
 *
 * @code{.xml}
       <service impl="::sight::module::ui::base::SPushField">
           <inout key="source" uid="compositeId" />
           <out key="destination" uid="objectId" />
           <push field="item" />
       </service>
   @endcode
 */
class MODULE_UI_BASE_CLASS_API SPushField : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SPushField, ::sight::ui::base::IAction);

    /// Constructor. Do nothing.
    MODULE_UI_BASE_API SPushField() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_BASE_API virtual ~SPushField() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect source Composite::s_ADDED_FIELDS_SIG to this::s_UPDATE_OBJECTS_SLOT
     * Connect source Composite::s_REMOVED_FIELDS_SIG to this::s_UPDATE_OBJECTS_SLOT
     */
    MODULE_UI_BASE_API KeyConnectionsMap getAutoConnections() const override;

protected:

    /// Configure the service.
    void configuring() override;

    /// Register the action and check if the action is executable.
    void starting() override;

    /// Unregister the action.
    void stopping() override;

    /// Extract the object.
    void updating() override;

private:

    /**
     * @brief Slot: called when source composite objects are updated, enabled/disabled the action if the defined objects
     * are present/not present.
     */
    void updateObjects();

    /// Key in the source composite to extract
    std::string m_field;
};

} // namespace sight::module::ui::base
