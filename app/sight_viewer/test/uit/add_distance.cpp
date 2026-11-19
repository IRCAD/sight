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

#include "helper.hpp"

#include <data/fiducials_series.hpp>
#include <data/image_series.hpp>

#include <ui/test/gui_test.hpp>

#include <cstddef>
#include <optional>
#include <utility>
#include <vector>

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

/// The ends of the distance of the image, one after the other, while it has exactly one.
static std::optional<std::vector<double> > distance_ends(const data::image_series& _series)
{
    const auto rulers = _series.get_fiducials()->query_fiducials(std::nullopt, data::fiducials_series::shape::ruler);
    if(rulers.size() != 1 || !rulers.front().m_contour_data.has_value())
    {
        return std::nullopt;
    }

    return rulers.front().m_contour_data;
}

//------------------------------------------------------------------------------

/// One end of a distance, out of both.
static std::vector<double> end_of(const std::vector<double>& _ends, std::size_t _index)
{
    const auto start = _ends.begin() + static_cast<std::ptrdiff_t>(3 * _index);
    return {start, start + 3};
}

//------------------------------------------------------------------------------

/// Whether exactly one end of the one distance moved from where both were. Their order in the data is not the order
/// in which they were drawn.
static auto one_end_moved(std::vector<double> _before)
{
    return [before = std::move(_before)](const data::image_series& _series)
           {
               const auto now = distance_ends(_series);
               return now.has_value() && now->size() == 6 && before.size() == 6
                      && (end_of(*now, 0) != end_of(before, 0)) != (end_of(*now, 1) != end_of(before, 1));
           };
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("add_distance")
{
    using ui::test::at;

    open_image(ui);
    const auto alone = ui.read(MAIN_SCENE, &QWidget::size);
    ui.click("top_toolbar_left/volume");
    ui.click("top_toolbar_view/Restore");
    reset_negatos(ui);
    show_main_scene_alone(ui, alone);

    ui.set("parameter_toolbar_view/Annotation", true);
    ui.set(ui::test::menu_item("annotation_tools_toolbar", "Add / edit distance"), true);

    const auto ends = [&ui]
                      {
                          const auto series = ui.data<data::image_series>("image");
                          const data::mt::locked_ptr lock(series);
                          return distance_ends(*lock).value_or(std::vector<double> {});
                      };

    // Both ends on the axial negato, then each one moved, the first one next to the second. Too small for an image
    // change: the distance tells.
    const auto first  = at(0.40, 0.40);
    const auto second = at(0.63, 0.40);
    ui.drag(MAIN_SCENE, first, second);
    ui.expect_data<data::image_series>("image").that(
        "has one distance",
        [](const data::image_series& _series){return distance_ends(_series).has_value();});

    for(const auto& [from, to] : {std::pair {second, at(0.63, 0.48)}, std::pair {first, at(0.64, 0.49)}})
    {
        auto before = ends();
        ui.drag(MAIN_SCENE, from, to);
        ui.expect_data<data::image_series>("image").that("one end moved", one_end_moved(std::move(before)));
    }

    // Zoomed in on both ends.
    ui.expect_image_changes(MAIN_SCENE, [&]{ui.wheel(MAIN_SCENE, 4);});
    ui.expect_image(MAIN_SCENE, reference("add_distance"));
}

} // namespace sight::sight_viewer::uit
