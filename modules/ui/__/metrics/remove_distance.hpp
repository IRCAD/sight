/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include <core/macros.hpp>

#include <data/image.hpp>
#include <data/point_list.hpp>

#include <ui/__/action.hpp>

namespace sight::module::ui::metrics
{

/**
 * @brief This action removes distances from the image field.
 * Choose between removing a selected point or the last added point.
 *
 * @section Slots Slots
 * - \b remove_last_distance: Remove the last added point.
 *
 * @section XML XML Configuration
 * Example to remove a selected distance:
 * @code{.xml}
   <service type="sight::module::ui::metrics::remove_distance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 *
 * Example to remove the last added distance:
 * @code{.xml}
    <service uid="remove_distanceAct" type="sight::module::ui::metrics::remove_distance">
            <inout key="image" uid="${image}" />
    </service>

    <service uid="remove_last_distance" type="sight::module::ui::action" />

    <connect>
        <signal>removeLastDistance/clicked</signal>
        <slot>remove_distanceAct/remove_last_distance</slot>
    </connect>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::image]: image containing the distance field.
 */
class MODULE_UI_CLASS_API remove_distance final : public sight::ui::action
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(remove_distance, sight::ui::action);

    /// Initialize slots.
    MODULE_UI_API remove_distance() noexcept;

    /// Destroys the service.
    MODULE_UI_API ~remove_distance() noexcept override = default;

protected:

    /// Configures the action.
    MODULE_UI_API void configuring() override;

    /// Starts the action.
    MODULE_UI_API void starting() override;

    /// Removes a distance chosen from a gui dialog.
    MODULE_UI_API void updating() override;

    /// Stops the action.
    MODULE_UI_API void stopping() override;

private:

    /// Changes a distance value to a string.
    static std::string distance_to_str(double _dist);

    /**
     * @brief Opens a gui dialog to choose the distance to remove.
     * @param _image The image where the distance need to be removed.
     * @param _removeAll Is set to true if all distance need to be removed.
     * @return The distance to removed only if a specific distance is chosen, if all distances need to be removed, a
     * nullptr is return.
     */
    static data::point_list::sptr get_distance_to_remove(const data::image::csptr _image, bool& _remove_all);

    /**
     * @brief Sends a signal when a distance is removed.
     * @param _image The image where the distance has been added.
     * @param _distance The distance value.
     */
    static void notify_delete_distance(const data::image::csptr& _image, const data::point_list::csptr& _distance);

    /// Removes the last added distance.
    void remove_last_distance();

    data::ptr<data::image, data::access::inout> m_image {this, "image"};
};

} // namespace sight::module::ui::metrics
