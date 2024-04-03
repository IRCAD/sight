/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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
class show_distance final : public sight::ui::action
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(show_distance, sight::ui::action);

    /// Initialize slots.
    show_distance() noexcept;

    /// Destroys the service.
    ~show_distance() noexcept override;

protected:

    void configuring() override;

    void starting() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect data::image::DISTANCE_DISPLAYED_SIG of s_IMAGE_INOUT to SHOW_DISTANCE_SLOT
     */
    connections_t auto_connections() const override;

    /// Shows or hides distances.
    void updating() override;

    /// Stops the action.
    void stopping() override;

private:

    /// SLOT: shows or hides distance from the image field.
    void show(bool /*unused*/);

    static constexpr std::string_view IMAGE = "image";

    data::ptr<data::image, data::access::inout> m_image {this, IMAGE};
};

} // namespace sight::module::ui::metrics
