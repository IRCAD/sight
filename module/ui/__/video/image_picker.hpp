/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include <core/base.hpp>
#include <core/clock.hpp>
#include <core/id.hpp>

#include <data/camera.hpp>
#include <data/point.hpp>
#include <data/point_list.hpp>
#include <data/tools/picking_info.hpp>

#include <service/controller.hpp>

namespace sight::module::ui::video
{

/**
 * @brief This service listens to a picking interaction and fills the clicked point list.
 *
 * @section Slots Slots
 * - \b get_interaction(data::tools::picking_info info): retrieves the 2d coordinates (z= 0) from picking info.
 *
 * @code{.xml}
   <service uid="..." type="sight::module::ui::video::image_picker" >
        <inout key="pointList" uid="..." />
        <inout key="pixelPointList" uid="..." />
        <in key="camera" uid="..." />
        <config videoReference="center" useCtrlModifier="true" singlePointMode="false" />
   </service>
   @endcode
 * @section InOut InOut
 * - \b pointList [sight::data::point_list] : Pointlist of clicked points, real coordinates given by picker.
 * - \b pixelPointList [sight::data::point_list] : PointList of clicked points, transformed in pixel world.
 *
 * @section Configuration Configuration
 * - \b videoReference: specifies where is the reference system of picker interactor.
 * - \b useCtrlModifier: whether to require or not the control modifier when clicking a point.
 * - \b singlePointMode: whether to enable singlePointMode or not. In this mode, clicking adds a point,
 *                       then removes it, then adds it again and so on.
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
class image_picker : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(image_picker, service::controller);

    /// Creates the video reference map..
    image_picker() noexcept;

    /// Destroys the service.
    ~image_picker() noexcept override;

protected:

    /// Does nothing.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Does nothing.
    void updating() override;

    /// Does nothing.
    void stopping() override;

private:

    /// Manages several video coordinate system.
    enum video_reference_t
    {
        top_left = 0,
        center
    };

    /// Adds a point in both pointlists.
    void add_point(const std::array<double, 3>& _current_point);

    /// Removes the last point in both pointlists.
    void remove_last_point();

    /// Slot: retrieves the 2d coordinates from the interaction point.
    void get_interaction(data::tools::picking_info _info);

    /// Map that handles conversion between xml configuration string and video_reference_t
    std::map<std::string, video_reference_t> m_video_ref_map;

    /// Manages video coordinate system
    video_reference_t m_video_ref {video_reference_t::center};

    // Whether to require or not the use of the control modifier when clicking.
    bool m_use_ctrl_modifier {true};

    // Whether to enable or not use of single point mode (see the service description for more information).
    bool m_single_point_mode {false};

    data::ptr<data::point_list, sight::data::access::inout> m_point_list {this, "pointList"};
    data::ptr<data::point_list, sight::data::access::inout> m_pixel_point_list {this, "pixelPointList"};
    data::ptr<data::camera, sight::data::access::in> m_camera {this, "camera"};
};

} //namespace sight::module::ui::video
