/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "modules/filter/image/config.hpp"

#include <data/PointList.hpp>

#include <service/IOperator.hpp>

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
       <service uid="..." type="::sight::module::filter::image::SLabelGeometryImage">
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
 * - \b image [sight::data::Image]: labeled image to which the 'landmark' field will be added.
 *
 * @subsection Out Out
 * - \b pointList [sight::data::PointList]: PointList containing centroid points position. Each Point contains a label
 * in
 * field. This pointList is optional.
 *
 * @subsection Configuration Configuration
 * - \b clusters: Must contains a group a clusters.
 * - \b cluster: Contains a list of size_t labels value.
 */
class MODULE_FILTER_IMAGE_CLASS_API SLabelGeometryImage final : public service::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SLabelGeometryImage, service::IOperator)

    /// Initializes slots.
    MODULE_FILTER_IMAGE_API SLabelGeometryImage();

    /// Destroys the service.
    MODULE_FILTER_IMAGE_API ~SLabelGeometryImage() override;

protected:

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void starting() override;

    /// Configures the service.
    MODULE_FILTER_IMAGE_API void configuring() override;

    /// Does nothing.
    MODULE_FILTER_IMAGE_API void stopping() override;

    /// Compute centroids.
    MODULE_FILTER_IMAGE_API void updating() override;

private:

    /**
     * @brief Update the selected point list
     * @param value
     * @param key
     */
    void updateSelectedPointList(std::string value, std::string key);

    std::vector< std::vector<size_t> > m_lPointListLabels;
    std::vector< data::PointList::sptr > m_lPointListCentroids;
};

} // namespace sight::module::filter::image
