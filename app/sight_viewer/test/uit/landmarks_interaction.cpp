/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <QColor>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <algorithm>
#include <array>
#include <cmath>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace sight::sight_viewer::uit
{

//------------------------------------------------------------------------------

/// Whether a group has landmarks, all of the given color.
static bool landmarks_of_color(const data::image_series& _series, std::string_view _group, const QColor& _color)
{
    const auto landmarks = _series.get_fiducials()->query_fiducials(
        std::nullopt,
        data::fiducials_series::shape::point,
        _group
    );
    const std::array expected {_color.redF(), _color.greenF(), _color.blueF()};
    return !landmarks.empty() && std::ranges::all_of(
        landmarks,
        [&expected](const auto& _landmark)
        {
            return _landmark.m_color.has_value() && std::ranges::equal(
                expected,
                std::span(*_landmark.m_color).first(3),
                [](double _a, float _b){return std::abs(_a - static_cast<double>(_b)) < 1. / 255.;});
        });
}

//------------------------------------------------------------------------------

SIGHT_GUI_TEST_CASE("landmarks_interaction")
{
    using ui::test::at;

    const std::string tree = "annotation_srv/tree";
    const auto group = [&tree](int _index){return ui::test::item(tree, "Group_" + std::to_string(_index));};
    const auto expect_landmarks = [&ui, &tree](int _group, int _count)
                                  {
                                      ui.expect(tree).that<QTreeWidget>(
                                          "Group_" + std::to_string(_group) + " has " + std::to_string(_count)
                                          + " landmarks",
                                          [_group, _count](const QTreeWidget& _tree)
            {
                const auto* const item = _tree.topLevelItem(_group);
                return item != nullptr && item->childCount() == _count;
            });
                                  };

    // Where landmarks go, around the centre of the main scene.
    const auto spot = [](int _x, int _y){return at(0.5 + 0.215 * _x, 0.5 + 0.238 * _y);};

    open_image(ui);
    ui.click("top_toolbar_left/volume");
    ui.set("parameter_toolbar_view/Annotation", true);

    for(int i = 0 ; i < 3 ; ++i)
    {
        ui.click("annotation_srv/new_group");
    }

    ui.expect(tree).that<QTreeWidget>(
        "has three groups",
        [](const QTreeWidget& _tree)
        {
            return _tree.topLevelItemCount() == 3;
        });
    const std::array colors {QColor(255, 0, 0), QColor(0, 255, 0), QColor(0, 0, 255)};
    for(int i = 0 ; i < 3 ; ++i)
    {
        ui.set(group(i).column(1), colors.at(static_cast<std::size_t>(i)));
    }

    // Three landmarks in each group.
    ui.set(ui::test::menu_item("annotation_tools_toolbar", "Add / edit fiducial"), true);
    using spots = std::array<std::pair<int, int>, 3>;
    const std::array placed {
        spots {{{0, 0}, {-1, -1}, {0, -1}}},
        spots {{{1, -1}, {-1, 0}, {1, 0}}},
        spots {{{-1, 1}, {0, 1}, {1, 1}}},
    };
    for(int i = 0 ; i < 3 ; ++i)
    {
        ui.select(group(i));
        ui.set("annotation_srv/size", 10);
        for(const auto& [x, y] : placed.at(static_cast<std::size_t>(i)))
        {
            ui.click(MAIN_SCENE, spot(x, y));
        }

        expect_landmarks(
            i,
            3
        );
    }

    // Too small to tell in an image: each landmark has the color of its group.
    ui.expect_data<data::image_series>("image").that(
        "gives each group its color",
        [colors](const data::image_series& _series)
        {
            return landmarks_of_color(_series, "Group_0", colors[0]) && landmarks_of_color(
                _series,
                "Group_1",
                colors[1]
            )
                   && landmarks_of_color(_series, "Group_2", colors[2]);
        });

    ui.expect_image(MAIN_SCENE, reference("landmarks_interaction_placed"), edge_on_negatos());

    // A click on a landmark shows its bin, for this landmark only.
    const auto remove = [&ui, &spot](int _x, int _y)
                        {
                            ui.click(MAIN_SCENE, spot(_x, _y));
                            ui.click("main_scene_view >> landmarks_adp/bin_button");
                        };

    ui.select(group(0));
    remove(0, 0);
    remove(-1, -1);
    remove(0, -1);
    expect_landmarks(0, 0);

    ui.select(group(1));
    remove(1, 0);
    remove(-1, 0);
    expect_landmarks(1, 1);

    ui.select(group(2));
    remove(1, 1);
    expect_landmarks(2, 2);

    ui.expect_image(MAIN_SCENE, reference("landmarks_interaction_removed"), edge_on_negatos());
}

} // namespace sight::sight_viewer::uit
