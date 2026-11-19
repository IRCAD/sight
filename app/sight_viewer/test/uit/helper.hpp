/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
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

#include <ui/test/image.hpp>
#include <ui/test/scenario.hpp>

#include <utest_data/data.hpp>

#include <QSize>
#include <QWidget>

#include <chrono> // NOLINT(misc-include-cleaner): the duration literals
#include <filesystem>
#include <string>
#include <string_view>

namespace sight::sight_viewer::uit
{

inline constexpr std::string_view MAIN_SCENE = "main_scene_view >> scene_srv/view";

//------------------------------------------------------------------------------

/// A reference of the main scene, or another image of this application.
[[nodiscard]] inline std::filesystem::path reference(std::string_view _name)
{
    return utest_data::dir() / "sight/ui/sight_viewer" / (std::string(_name) + ".png");
}

//------------------------------------------------------------------------------

/// Loads a file through the real dialog, then waits for the action that loading enables.
inline void open_file(
    ui::test::scenario& _ui,
    const std::filesystem::path& _path,
    std::string_view _format,
    std::string_view _ready_action
)
{
    using namespace std::chrono_literals;
    _ui.set("parameter_toolbar_view/Data", true);
    _ui.click("data_tools_row_1/Load Files");
    _ui.choose_file(_path, _format);
    _ui.expect(_ready_action).within(180s).enabled();
}

//------------------------------------------------------------------------------

inline void open_image(ui::test::scenario& _ui)
{
    open_file(
        _ui,
        utest_data::dir() / "sight/image/inr/image.inr.gz",
        "Inr (.inr) (*.inr *.inr.gz)",
        "top_toolbar_left/volume"
    );
}

//------------------------------------------------------------------------------

inline void open_mesh(ui::test::scenario& _ui)
{
    open_file(
        _ui,
        utest_data::dir() / "sight/mesh/vtk/sphere.vtk",
        "VTK Legacy Files(.vtk) (*.vtk)",
        "top_toolbar_left/mesh"
    );
}

//------------------------------------------------------------------------------

/// A part of the slice selector of the negato of a scene view, its slider by default.
[[nodiscard]] inline std::string negato(std::string_view _scene_view, std::string_view _part = "slider")
{
    return std::string(_scene_view) + " >> negato_slicer_srv/slice_selector/" + std::string(_part);
}

//------------------------------------------------------------------------------

/// A negato seen edge on is a line of one pixel, which one driver draws and another does not, as the edges of the
/// orientation marker: up to 0.25% of the main scene. Another slice changes far more.
[[nodiscard]] inline ui::test::image_threshold edge_on_negatos()
{
    return ui::test::threshold(16, 0.005, "negatos seen edge on are drawn or not depending on the driver");
}

//------------------------------------------------------------------------------

/// The views beside the main scene share the window by the minimum sizes of their widgets, which depend on the style
/// and the fonts of each platform: a capture of the main scene shows it alone, with the size it had alone before.
inline void show_main_scene_alone(ui::test::scenario& _ui, QSize _alone)
{
    _ui.click("top_toolbar_view/Maximize");
    _ui.expect(MAIN_SCENE).that<QWidget>("is alone", [_alone](const QWidget& _scene){return _scene.size() == _alone;});
}

//------------------------------------------------------------------------------

/// Puts every negato on its first slice, so that a scenario starts from a known scene.
inline void reset_negatos(ui::test::scenario& _ui)
{
    for(const std::string_view scene_view : {"axial_scene_view", "sagittal_scene_view", "frontal_scene_view"})
    {
        _ui.set(negato(scene_view), 0);
        _ui.expect(negato(scene_view, "label")).text_contains("0 / ");
    }
}

} // namespace sight::sight_viewer::uit
