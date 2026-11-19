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

#include "helper.hpp"

#include <ui/test/detail/image.hpp>
#include <ui/test/gui_test.hpp>

#include <QImage>

#include <filesystem>
#include <stdexcept>

namespace sight::sight_viewer::uit
{

using namespace std::chrono_literals;
using ui::test::detail::qt_path;

/// Clicks the real Snapshot button and returns the image it writes, once complete.
static QImage save_snapshot(ui::test::scenario& _ui, const std::filesystem::path& _snapshot)
{
    std::filesystem::create_directories(_snapshot.parent_path());
    _ui.click("top_toolbar_view/Snapshot");
    _ui.choose_file(_snapshot);
    return _ui.read_png(_snapshot, 60s);
}

//------------------------------------------------------------------------------

/// Snapshot omits the on-screen background and overlays, so it has its own reference of a fixed size.
static void expect_snapshot(
    const QImage& _saved,
    const std::filesystem::path& _reference,
    const std::filesystem::path& _output
)
{
    const QImage expected(qt_path(_reference));
    if(expected.isNull())
    {
        throw std::runtime_error("the Snapshot reference cannot be read");
    }

    const auto metrics = ui::test::detail::compare_images(_saved, expected, {});
    if(!metrics.matches())
    {
        ui::test::detail::save_image(expected, _output / "snapshot-expected.png");
        ui::test::detail::save_image(
            ui::test::detail::image_difference(_saved, expected),
            _output / "snapshot-diff.png"
        );
        throw std::runtime_error("Snapshot differs from its reference: " + metrics.describe());
    }
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("zoom_out")
{
    ui.expect_image_changes(MAIN_SCENE, [&]{open_mesh(ui);});
    ui.expect_image_changes(MAIN_SCENE, [&]{ui.wheel(MAIN_SCENE, -7);});
    const auto snapshot = save_snapshot(ui, ui.output_directory() / "snapshot.png");

    ui.expect_image(MAIN_SCENE, reference("zoom_out"));
    expect_snapshot(snapshot, reference("zoom_out_snapshot"), ui.output_directory());
}

} // namespace sight::sight_viewer::uit
