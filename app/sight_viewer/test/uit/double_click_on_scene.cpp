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

#include <ui/test/gui_test.hpp>

SIGHT_GUI_TEST_CASE("double_click_on_scene")
{
    // scene_srv is in each of the four scene views: the view scope picks the main one.
    const std::string scene = "main_scene_view >> scene_srv/view";

    // Keep a copy of the initial geometry; no QWidget crosses onto the scenario thread.
    const auto maximized_size = ui.read(scene, &QWidget::size);

    ui.double_click(scene);
    ui.expect(scene).that(
        "is restored",
        [maximized_size](const QWidget& _widget)
    {
        const QSize current_size = _widget.size();
        return current_size.width() * current_size.height()
               < maximized_size.width() * maximized_size.height();
    });

    // The second double-click returns the view to the size we recorded.
    ui.double_click(scene);
    ui.expect(scene).that(
        "is maximized again",
        [maximized_size](const QWidget& _widget){return _widget.size() == maximized_size;});
}
