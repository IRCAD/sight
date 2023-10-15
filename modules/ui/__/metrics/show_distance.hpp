/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "modules/ui/__/config.hpp"

#include <data/image.hpp>

#include <ui/__/action.hpp>

namespace sight::module::ui::metrics
{

/**
 * @brief This action allows to show/hide image distances.
 *
 * @section Slots Slots
 * - \b showDistance(bool): slot called to check/uncheck the action when the distance are visible or not.
 *
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::metrics::show_distance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::image]: image containing the distances.
 */
class MODULE_UI_CLASS_API show_distance final : public sight::ui::action
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(show_distance, sight::ui::action);

    /// Initialize slots.
    MODULE_UI_API show_distance() noexcept;

    /// Destroys the service.
    MODULE_UI_API ~show_distance() noexcept override;

protected:

    MODULE_UI_API void configuring() override;

    MODULE_UI_API void starting() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect data::image::DISTANCE_DISPLAYED_SIG of s_IMAGE_INOUT to SHOW_DISTANCE_SLOT
     */
    MODULE_UI_API connections_t auto_connections() const override;

    /// Shows or hides distances.
    MODULE_UI_API void updating() override;

    /// Stops the action.
    MODULE_UI_API void stopping() override;

private:

    /// SLOT: shows or hides distance from the image field.
    void showDistance(bool /*unused*/);

    static constexpr std::string_view s_IMAGE = "image";

    data::ptr<data::image, data::Access::inout> m_image {this, s_IMAGE};
};

} // namespace sight::module::ui::metrics
