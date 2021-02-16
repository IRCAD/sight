/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <data/Image.hpp>

#include <ui/base/IAction.hpp>

namespace sight::modules::ui::base::metrics
{

/**
 * @brief This action allows to show/hide image distances.
 *
 * @section Slots Slots
 * - \b showDistance(bool): slot called to check/uncheck the action when the distance are visible or not.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="::sight::modules::ui::base::metrics::SShowDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::Image]: image containing the distances.
 */
class MODULE_UI_BASE_CLASS_API SShowDistance final : public ::sight::ui::base::IAction
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SShowDistance, sight::ui::base::IAction)

    /// Initialize slots.
    MODULE_UI_BASE_API SShowDistance() noexcept;

    /// Destroys the service.
    MODULE_UI_BASE_API ~SShowDistance() noexcept override;

protected:

    MODULE_UI_BASE_API void configuring() override;

    MODULE_UI_BASE_API void starting() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect data::Image::s_DISTANCE_DISPLAYED_SIG of s_IMAGE_INOUT to s_SHOW_DISTANCE_SLOT
     */
    MODULE_UI_BASE_API KeyConnectionsMap getAutoConnections() const override;

    /// Shos or hide distances.
    MODULE_UI_BASE_API void updating() override;

    /// Stops the action.
    MODULE_UI_BASE_API void stopping() override;

private:

    /// SLOT: shows or hides distance from the image field.
    void showDistance(bool);

};

} // namespace sight::modules::ui::base::metrics
