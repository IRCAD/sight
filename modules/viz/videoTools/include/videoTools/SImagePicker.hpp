/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "videoTools/config.hpp"

#include <core/base.hpp>
#include <core/HiResClock.hpp>
#include <core/tools/fwID.hpp>

#include <data/Point.hpp>
#include <data/PointList.hpp>
#include <data/tools/PickingInfo.hpp>

#include <services/IController.hpp>

namespace videoTools
{
/**
 * @brief This service listens to a picking interaction and fills the clicked point list.
 *
 * @section Slots Slots
 * - \b getInteraction(data::tools::PickingInfo info): retrieves the 2d coordinates (z= 0) from picking info.
 *
 * @code{.xml}
   <service uid="..." type="::videoTools::SImagePicker" >
        <inout key="pointList" uid="..." />
        <inout key="pixelPointList" uid="..." />
        <config videoReference="center" />
   </service>
   @endcode
 * @section InOut InOut
 * - \b pointList [data::PointList] : Pointlist of clicked points, real coordinates given by picker.
 * - \b pixelPointList [data::PointList] : PointList of clicked points, transformed in pixel world.
 *
 * @section Configuration Configuration
 * - \b videoReference: specifies where is the reference system of picker interactor
 * it can be at center of the video plane, or on the top left corner (eg: "center" or "top_left").
 *
 * This is needed to transform picked points from "scene" reference to pixel.
 *
 * Here's an illustration of center & top_left coordinate system
 *
 * the origin of coordinates system is represented by an ascii "+"
 *
 *      CENTER
 *          X
 *    -------------
 *   |     ^       |
 * Y |     + >     |
 *   |             |
 *    -------------
 *
 *     TOP_LEFT
 *          X
 *   + >----------
 *   v            |
 * Y |            |
 *   |            |
 *    ------------
 **/
class VIDEOTOOLS_CLASS_API SImagePicker : public services::IController
{
public:

    fwCoreServiceMacro(SImagePicker, services::IController)

    /// Creates the video reference map..
    VIDEOTOOLS_API SImagePicker() noexcept;

    /// Destroys the service.
    VIDEOTOOLS_API virtual ~SImagePicker() noexcept;

protected:

    /// Does nothing.
    VIDEOTOOLS_API void configuring() override;

    /// Does nothing.
    VIDEOTOOLS_API void starting() override;

    /// Does nothing.
    VIDEOTOOLS_API void updating() override;

    /// Does nothing.
    VIDEOTOOLS_API void stopping() override;

private:

    /// Manages several video coordinate system.
    typedef enum VideoReference
    {
        TOP_LEFT = 0,
        CENTER,
    } VideoReferenceType;

    /// Adds a point in both pointlists.
    void addPoint(const std::array<double, 3>& currentPoint);

    /// Removes the last point in both pointlists.
    void removeLastPoint();

    /// Slot: retrieves the 2d coordinates from the interaction point.
    void getInteraction(data::tools::PickingInfo info);

    /// Map that handles conversion between xml configuration string and VideoReferenceType
    std::map<std::string, VideoReferenceType > m_videoRefMap;

    /// Manages video coordinate system
    VideoReferenceType m_videoRef { VideoReferenceType::CENTER };

};

} //namespace videoTools
