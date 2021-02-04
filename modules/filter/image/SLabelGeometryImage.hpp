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

#include "opImageFilter/config.hpp"

#include <fwData/PointList.hpp>

#include <fwServices/IOperator.hpp>

namespace opImageFilter
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
       <service uid="..." type="::opImageFilter::SLabelGeometryImage">
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
 * - \b image [::fwData::Image]: labeled image to which the 'landmark' field will be added.
 *
 * @subsection Out Out
 * - \b pointList [::fwData::PointList]: PointList containing centroid points position. Each Point contains a label in
 * field. This pointList is optional.
 *
 * @subsection Configuration Configuration
 * - \b clusters: Must contains a group a clusters.
 * - \b cluster: Contains a list of size_t labels value.
 */
class OPIMAGEFILTER_CLASS_API SLabelGeometryImage final : public ::fwServices::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SLabelGeometryImage, ::fwServices::IOperator)

    /// Initializes slots.
    OPIMAGEFILTER_API SLabelGeometryImage();

    /// Destroys the service.
    OPIMAGEFILTER_API ~SLabelGeometryImage() override;

protected:

    /// Does nothing.
    OPIMAGEFILTER_API void starting() override;

    /// Configures the service.
    OPIMAGEFILTER_API void configuring() override;

    /// Does nothing.
    OPIMAGEFILTER_API void stopping() override;

    /// Compute centroids.
    OPIMAGEFILTER_API void updating() override;

private:

    /**
     * @brief Update the selected point list
     * @param value
     * @param key
     */
    void updateSelectedPointList(std::string value, std::string key);

    std::vector< std::vector<size_t> > m_lPointListLabels;
    std::vector< ::fwData::PointList::sptr > m_lPointListCentroids;
};

} // namespace opImageFilter
