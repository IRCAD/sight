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

#include "test.hpp"

#include <data/image_series.hpp>

#include <service/registry.hpp>

#include <ui/test/helper/button.hpp>

#include <utest_data/data.hpp>

#include <doctest/doctest.h>

#include <QAction>

#include <algorithm>
#include <string>

namespace sight::sight_viewer::uit
{

TEST_SUITE("sight_viewer")
{
    TEST_CASE_FIXTURE(test, "path_interaction")
    {
        namespace helper = sight::ui::test::helper;

        const std::string failure_message = start(
            "sightViewerPathInteractionTest",
            [&](sight::ui::test::tester& _tester)
            {
                open_file(
                    _tester,
                    "Inr (.inr) (*.inr *.inr.gz)",
                    utest_data::dir() / "sight/image/inr/image.inr.gz"
                );
                helper::button::push(_tester, "top_toolbar_left/volume");

                sight::service::base::sptr path_service;
                _tester.take("3D scene", "top_scenes_view/1");
                _tester.doubt<QWidget*>(
                    "The path service is started",
                    [&](QWidget*)
                {
                    for(const auto& service : sight::service::get_services(
                            "sight::module::viz::scene3d_qt::adaptor::fiducials::path"
                    ))
                    {
                        if(service->started()
                           && service->get_id().find("volumeRendering") != std::string::npos
                           && service->get_id().ends_with("-path_adp"))
                        {
                            path_service = service;
                            return true;
                        }
                    }

                    return false;
                });

                auto image = path_service->inout<sight::data::image_series>("data.image_series").lock().get_shared();
                if(!image)
                {
                    sight::ui::test::tester::fail("The path service has no image series");
                }

                helper::button::push(_tester, "parameter_toolbar_view/Fly through camera");
                helper::button::push(_tester, "fly_through_camera_toolbar/Draw path");

                helper::selector::from_parent("top_scenes_view/1", "scene_srv").select(_tester);
                const QPoint scene_center = sight::ui::test::tester::center_of(_tester.get<QWidget*>());
                _tester.interact(
                    std::make_unique<sight::ui::test::mouse_click>(Qt::LeftButton, Qt::NoModifier, scene_center)
                );

                _tester.take("Clear path action", "fly_through_camera_toolbar/Clear path");
                _tester.doubt<QAction*>(
                    "One point does not create a path",
                    [](QAction* _action){return !_action->isEnabled();});
                _tester.doubt<QAction*>(
                    "The Path group contains one point",
                    [image](QAction*)
                {
                    return image->get_fiducials()->get_number_of_points_in_group("Path") == 1;
                });

                helper::selector::from_parent("top_scenes_view/1", "scene_srv").select(_tester);
                _tester.interact(
                    std::make_unique<sight::ui::test::mouse_click>(
                        Qt::LeftButton,
                        Qt::NoModifier,
                        scene_center + QPoint(100, 100)
                    )
                );

                _tester.take("Clear path action", "fly_through_camera_toolbar/Clear path");
                _tester.doubt<QAction*>(
                    "Two points create a path",
                    [](QAction* _action){return _action->isEnabled();});
                _tester.doubt<QAction*>(
                    "The Path group contains two points",
                    [image](QAction*)
                {
                    return image->get_fiducials()->get_number_of_points_in_group("Path") == 2;
                });

                helper::button::push(_tester, "fly_through_camera_toolbar/Clear path");
                _tester.take("Clear path action", "fly_through_camera_toolbar/Clear path");
                _tester.doubt<QAction*>(
                    "Clearing the path disables the action",
                    [](QAction* _action){return !_action->isEnabled();});
                _tester.doubt<QAction*>(
                    "Clearing the path removes its points",
                    [image](QAction*)
                {
                    const auto fiducials = image->get_fiducials();
                    const auto groups    = fiducials->get_point_fiducials_group_names();
                    if(std::ranges::find(groups, "Path") == groups.end())
                    {
                        return true;
                    }

                    return fiducials->get_number_of_points_in_group("Path") == 0;
                });
            },
            true
        );

        INFO(failure_message);
        REQUIRE(failure_message.empty());
    }
}

} // namespace sight::sight_viewer::uit
