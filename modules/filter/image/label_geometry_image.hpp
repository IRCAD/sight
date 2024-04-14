/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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
#include <data/point_list.hpp>

#include <service/filter.hpp>

namespace sight::module::filter::image
{

/**
 * @brief Gets centroids of shapes from a labeled image and adds them as a 'landmark' field to it.
 *
 * This service works in 2 modes.
 * When we have clusters in the service configuration, the centroid are pushed in a pointList. Otherwise, we use
 * landmarks.
 *
 * @section Slots Slots
 * - \b updateSelectedPointList(std::string value, std::string key): Push the selected pointList in the output.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="sight::module::filter::image::label_geometry_image">
           <inout key="image" uid="..."/>
           <out key="pointList" uid"..."/>
           <clusters>
            <cluster>1,2,3</cluster>
            <cluster>20,17,9</cluster>
            <cluster>...</cluster>
           </clusters>
       </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::image]: labeled image to which the 'landmark' field will be added.
 *
 * @subsection Out Out
 * - \b pointList [sight::data::point_list]: PointList containing centroid points position. Each Point contains a label
 * in
 * field. This pointList is optional.
 *
 * @subsection Configuration Configuration
 * - \b clusters: Must contains a group a clusters.
 * - \b cluster: Contains a list of std::size_t labels value.
 */
class label_geometry_image final : public service::filter
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(label_geometry_image, service::filter);

    /// Initializes slots.
    label_geometry_image();

    /// Destroys the service.
    ~label_geometry_image() override;

protected:

    /// Does nothing.
    void starting() override;

    /// Configures the service.
    void configuring() override;

    /// Does nothing.
    void stopping() override;

    /// Compute centroids.
    void updating() override;

private:

    /**
     * @brief Update the selected point list
     */
    void update_selected_point_list(std::string _value, std::string _key);

    std::vector<std::vector<std::size_t> > m_l_point_list_labels;
    std::vector<data::point_list::sptr> m_l_point_list_centroids;

    sight::data::ptr<sight::data::image, sight::data::access::inout> m_image {this, "image"};
    sight::data::ptr<sight::data::point_list, sight::data::access::out> m_point_list {this, "pointList"};
};

} // namespace sight::module::filter::image
