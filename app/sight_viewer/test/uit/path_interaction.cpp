/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "helper.hpp"

#include <data/image_series.hpp>
#include <data/matrix4.hpp>

#include <ui/test/gui_test.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <string>

namespace sight::sight_viewer::uit
{

namespace
{

using pose_t = std::array<double, 16>;

constexpr std::string_view CAMERA  = "global_camera";
constexpr std::string_view TOOLBAR = "fly_through_camera_toolbar/";

} // namespace

//------------------------------------------------------------------------------

/// An item of the fly through camera toolbar.
static std::string fly(std::string_view _item)
{
    return std::string(TOOLBAR) + std::string(_item);
}

//------------------------------------------------------------------------------

static bool same_pose(const pose_t& _first, const pose_t& _second)
{
    return std::ranges::equal(_first, _second, [](double _a, double _b){return std::abs(_a - _b) < 1e-4;});
}

//------------------------------------------------------------------------------

static pose_t pose_of(const data::matrix4& _matrix)
{
    pose_t pose {};
    std::ranges::copy(_matrix.values(), pose.begin());
    return pose;
}

//------------------------------------------------------------------------------

/// The points of the path, 0 once it is gone.
static std::size_t path_points(const data::image_series& _series)
{
    const auto fiducials = _series.get_fiducials();
    const auto groups    = fiducials->get_point_fiducials_group_names();
    return std::ranges::find(
        groups,
        "Path"
    ) == groups.end() ? 0 : fiducials->get_number_of_points_in_group("Path").value_or(0);
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("path_interaction")
{
    using ui::test::at;

    const auto pose = [&ui]
                      {
                          const auto camera = ui.data<data::matrix4>(CAMERA);
                          const data::mt::locked_ptr lock(camera);
                          return pose_of(*lock);
                      };
    const auto expect_pose = [&ui](std::string_view _what, auto _predicate)
                             {
                                 ui.expect_data<data::matrix4>(CAMERA).that(
                                     _what,
                                     [_predicate](const data::matrix4& _camera){return _predicate(pose_of(_camera));});
                             };
    const auto expect_path = [&ui](std::string_view _what, std::size_t _points)
                             {
                                 ui.expect_data<data::image_series>("image").that(
                                     _what,
                                     [_points](const data::image_series& _series)
            {
                return path_points(_series) == _points;
            });
                             };

    open_image(ui);
    ui.click("top_toolbar_left/volume");

    ui.set("parameter_toolbar_view/Fly through camera", true);
    ui.set(fly("Draw path"), true);

    ui.click(MAIN_SCENE);
    ui.expect(fly("Clear path")).disabled();
    expect_path("has a path of one point", 1);

    ui.click(MAIN_SCENE, at(0.65, 0.66));
    ui.expect(fly("Clear path")).enabled();
    expect_path("has a path of two points", 2);

    // Interaction mode stays off before Play.
    ui.expect(fly("Interaction mode")).disabled();
    ui.expect(fly("Interaction mode")).unchecked();

    const auto normal = pose();
    ui.click(fly("Play"));
    ui.expect(fly("Interaction mode")).enabled();
    ui.expect(fly("Interaction mode")).checked();
    expect_pose("moves along the path", [normal](const pose_t& _pose){return !same_pose(_pose, normal);});

    ui.click(fly("Pause"));
    ui.expect(fly("Play")).enabled();
    ui.expect(fly("Interaction mode")).checked();

    // The mouse turns the paused fly camera.
    const auto paused = pose();
    ui.drag(MAIN_SCENE, at(0.5, 0.5), at(0.56, 0.53));
    expect_pose("turns with the mouse", [paused](const pose_t& _pose){return !same_pose(_pose, paused);});

    // Leaving the interaction mode puts the normal camera back, and the mouse moves it again.
    ui.set(fly("Interaction mode"), false);
    expect_pose("is back to normal", [normal](const pose_t& _pose){return same_pose(_pose, normal);});
    ui.drag(MAIN_SCENE, at(0.05, 0.13), at(0.11, 0.16));
    expect_pose("moves with the mouse", [normal](const pose_t& _pose){return !same_pose(_pose, normal);});

    // Play enters the interaction mode again, and leaving it stops the playback.
    ui.click(fly("Play"));
    ui.expect(fly("Interaction mode")).checked();
    ui.set(fly("Interaction mode"), false);
    ui.expect(fly("Play")).enabled();
    ui.expect(fly("Pause")).disabled();

    ui.click(fly("Clear path"));
    ui.expect(fly("Clear path")).disabled();
    expect_path("has no path", 0);
}

} // namespace sight::sight_viewer::uit
