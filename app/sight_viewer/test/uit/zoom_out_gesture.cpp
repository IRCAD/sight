/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <utest_data/data.hpp>

#include <QImage>

#include <QDateTime>

#include <doctest/doctest.h>

namespace sight::sight_viewer::uit
{

/// How much of the snapshot the rendered object covers, the scene being drawn on a black background.
static std::size_t rendered_area(const std::filesystem::path& _path)
{
    const QImage image(QString::fromStdString(_path.string()));

    if(image.isNull())
    {
        sight::ui::test::tester::fail("The snapshot " + _path.string() + " could not be read");
    }

    std::size_t area = 0;

    for(int y = 0 ; y < image.height() ; ++y)
    {
        for(int x = 0 ; x < image.width() ; ++x)
        {
            // The mesh is lit white over a black background, so anything but black belongs to it.
            if(qGray(image.pixel(x, y)) > 10)
            {
                ++area;
            }
        }
    }

    return area;
}

TEST_SUITE("sight_viewer")
{
//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(test, "zoom_out_gesture")
    {
        const std::string test_name = "sightViewerZoomOutGestureTest";

        const auto output_path = sight::ui::test::tester::get_image_output_path(test_name);
        const std::filesystem::path before_path(output_path / (test_name + "_before.png"));
        const std::filesystem::path after_path(output_path / (test_name + "_after.png"));

        for(const auto& path : {before_path, after_path})
        {
            if(std::filesystem::exists(path))
            {
                std::filesystem::remove(path);
            }
        }

        const std::string failure_message = start(
            test_name,
            [&before_path, &after_path](sight::ui::test::tester& _tester)
            {
                open_file(
                    _tester,
                    "VTK Legacy Files(.vtk) (*.vtk)",
                    utest_data::dir() / "sight/mesh/vtk/sphere.vtk"
                );

                // What the mesh covers before pinching. The test asserts how this changes rather than comparing
                // against a stored image: the amount of zoom a pinch produces depends on how many updates Qt's
                // recognizer coalesces, which varies with the platform, the Qt version and the machine load. Only
                // the direction of the change is actually specified behaviour.
                save_snapshot(_tester, before_path);
                const auto area_before = rendered_area(before_path);

                if(area_before == 0)
                {
                    sight::ui::test::tester::fail("The mesh was not visible before pinching");
                }

                // A pinch that took effect shrinks the mesh well beyond this; the margin only rules out noise.
                static constexpr double s_SHRUNK = 0.9;

                // Qt drops a gesture whose hot spot lies under another application's window, which pinch_gesture
                // avoids by staying on top, but not against another scenario doing the same at the same time (two
                // jobs on one CI machine). Pinch again until the mesh shrinks: an extra pinch is harmless since only
                // the direction of the change is checked.
                const auto deadline = QDateTime::currentMSecsSinceEpoch() + 3LL
                                      * sight::ui::test::tester::DEFAULT_TIMEOUT;
                std::size_t area_after = area_before;
                int pinches            = 0;

                while(static_cast<double>(area_after) > static_cast<double>(area_before) * s_SHRUNK
                      && QDateTime::currentMSecsSinceEpoch() < deadline)
                {
                    // Several views own a "scene_srv": take the one of the main 3D view.
                    _tester.take("main 3D view", "top_scenes_view/1");
                    _tester.yields("ogre scene", "scene_srv");
                    auto* const ogre_scene = _tester.get<QWidget*>();

                    _tester.interact(
                        std::make_unique<sight::ui::test::pinch_gesture>(
                            std::pair(
                                ogre_scene->rect().center() + QPoint(0, 70),
                                ogre_scene->rect().center() + QPoint(0, 1)
                            ),
                            std::pair(
                                ogre_scene->rect().center() - QPoint(0, 70),
                                ogre_scene->rect().center() - QPoint(0, 1)
                            )
                        )
                    );
                    sight::ui::test::tester::wait_for_pending_interactions();
                    ++pinches;

                    // Touch and gesture events need a minimum delay before the redraw to be taken into account.
                    QTest::qWait(500);

                    if(std::filesystem::exists(after_path))
                    {
                        std::filesystem::remove(after_path);
                    }

                    save_snapshot(_tester, after_path);
                    area_after = rendered_area(after_path);
                }

                if(static_cast<double>(area_after) > static_cast<double>(area_before) * s_SHRUNK)
                {
                    sight::ui::test::tester::fail(
                        "Pinching the fingers together should have zoomed out, but the mesh still covers "
                        + std::to_string(area_after) + " pixels against " + std::to_string(area_before)
                        + " before, after "
                        + std::to_string(pinches) + " pinches. Check that no window staying on top (assert or "
                        + "firewall dialog, notification) nor a locked session hid the scene"
                    );
                }
            },
            true
        );

        // Runs on the main thread, after start() has returned: the only doctest assertion for
        // this scenario. See sight::ui::test::base::start().
        INFO(failure_message);
        REQUIRE(failure_message.empty());
    }
} // TEST_SUITE

} // namespace sight::sight_viewer::uit
