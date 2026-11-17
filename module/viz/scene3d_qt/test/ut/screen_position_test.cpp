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

#include <module/viz/scene3d_qt/screen_position.hpp>

#include <doctest/doctest.h>

#include <QApplication>

#include <array>
#include <limits>
#include <memory>
#include <string>

// cspell: ignore QWIDGETSIZE_MAX

namespace sight::module::viz::scene3d_qt::ut
{

namespace
{

constexpr int MAX_COORD = QWIDGETSIZE_MAX;
constexpr float NAN_F   = std::numeric_limits<float>::quiet_NaN();
constexpr float INF_F   = std::numeric_limits<float>::infinity();

struct app_fixture
{
    app_fixture()
    {
        static std::string arg0 = "screen_position_test";
#ifdef __linux
        static std::string arg1 = "-platform";
        static std::string arg2 = "offscreen";
        static std::array argv {arg0.data(), arg1.data(), arg2.data(), static_cast<char*>(nullptr)};
#else
        static std::array argv {arg0.data(), static_cast<char*>(nullptr)};
#endif
        static int argc = static_cast<int>(argv.size() - 1);

        if(qApp == nullptr)
        {
            app = std::make_unique<QApplication>(argc, argv.data());
        }
    }

    std::unique_ptr<QApplication> app;
};

/// A 100x20 popup in a 400x300 parent, and a helper building a rect from logical coordinates, as place_near() divides
/// the projected ones by the device pixel ratio.
struct popup_fixture : app_fixture
{
    popup_fixture()
    {
        parent.resize(400, 300);
        popup.resize(100, 20);
        popup.move(12, 34);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] screen_rect_t rect(float _left, float _top, float _right, float _bottom) const
    {
        const auto ratio = static_cast<float>(popup.devicePixelRatioF());
        return {Ogre::Vector2(_left, _top) * ratio, Ogre::Vector2(_right, _bottom) * ratio};
    }

    QWidget parent;
    QWidget popup {&parent};
};

} // namespace

TEST_SUITE("sight::module::viz::scene3d_qt::screen_position")
{
    TEST_CASE("is_projectable")
    {
        CHECK(is_projectable({Ogre::Vector2(-10.F, 20.F), Ogre::Vector2(30.F, 40.F)}));
        CHECK(is_projectable({Ogre::Vector2(-1e30F, 1e30F), Ogre::Vector2(1e30F, -1e30F)}));

        CHECK_FALSE(is_projectable({Ogre::Vector2(NAN_F, 20.F), Ogre::Vector2(30.F, 40.F)}));
        CHECK_FALSE(is_projectable({Ogre::Vector2(10.F, NAN_F), Ogre::Vector2(30.F, 40.F)}));
        CHECK_FALSE(is_projectable({Ogre::Vector2(10.F, 20.F), Ogre::Vector2(NAN_F, 40.F)}));
        CHECK_FALSE(is_projectable({Ogre::Vector2(10.F, 20.F), Ogre::Vector2(30.F, NAN_F)}));
        CHECK_FALSE(is_projectable({Ogre::Vector2(INF_F, 20.F), Ogre::Vector2(30.F, 40.F)}));
        CHECK_FALSE(is_projectable({Ogre::Vector2(10.F, 20.F), Ogre::Vector2(30.F, -INF_F)}));
    }

    TEST_CASE("to_widget_coord")
    {
        CHECK(to_widget_coord(0.) == 0);
        CHECK(to_widget_coord(12.7) == 12);
        CHECK(to_widget_coord(-12.7) == -12);
        CHECK(to_widget_coord(MAX_COORD) == MAX_COORD);
        CHECK(to_widget_coord(-MAX_COORD) == -MAX_COORD);

        // Far off-screen projections are clamped instead of overflowing int.
        CHECK(to_widget_coord(6e9) == MAX_COORD);
        CHECK(to_widget_coord(-6e9) == -MAX_COORD);
        CHECK(to_widget_coord(std::numeric_limits<qreal>::infinity()) == MAX_COORD);
        CHECK(to_widget_coord(-std::numeric_limits<qreal>::infinity()) == -MAX_COORD);

        CHECK(to_widget_coord(std::numeric_limits<qreal>::quiet_NaN()) == 0);
    }

    TEST_CASE_FIXTURE(popup_fixture, "place_near_above")
    {
        // Left edge on the rect center, bottom edge on the rect top.
        CHECK(place_near(popup, parent, rect(140.F, 100.F, 160.F, 120.F)));
        CHECK(popup.pos() == QPoint(150, 80));
    }

    TEST_CASE_FIXTURE(popup_fixture, "place_near_below")
    {
        // Not enough room above the rect, the top edge goes on the rect bottom.
        CHECK(place_near(popup, parent, rect(140.F, 10.F, 160.F, 30.F)));
        CHECK(popup.pos() == QPoint(150, 30));
    }

    TEST_CASE_FIXTURE(popup_fixture, "place_near_inside_parent")
    {
        CHECK(place_near(popup, parent, rect(370.F, 100.F, 390.F, 120.F)));
        CHECK(popup.pos() == QPoint(300, 80));

        CHECK(place_near(popup, parent, rect(-60.F, 100.F, -40.F, 120.F)));
        CHECK(popup.pos() == QPoint(0, 80));

        // A parent narrower than the popup keeps it on its left edge.
        parent.resize(50, 300);
        CHECK(place_near(popup, parent, rect(20.F, 100.F, 30.F, 120.F)));
        CHECK(popup.pos() == QPoint(0, 80));
    }

    TEST_CASE_FIXTURE(popup_fixture, "place_near_far_off_screen")
    {
        CHECK(place_near(popup, parent, rect(1e12F, 1e12F, 1e12F, 1e12F)));
        CHECK(popup.pos() == QPoint(300, MAX_COORD));

        CHECK(place_near(popup, parent, rect(-1e12F, -1e12F, -1e12F, -1e12F)));
        CHECK(popup.pos() == QPoint(0, -MAX_COORD));
    }

    TEST_CASE_FIXTURE(popup_fixture, "place_near_not_projectable")
    {
        CHECK_FALSE(place_near(popup, parent, {Ogre::Vector2(NAN_F, NAN_F), Ogre::Vector2(NAN_F, NAN_F)}));
        CHECK_FALSE(place_near(popup, parent, rect(140.F, 100.F, INF_F, 120.F)));
        CHECK(popup.pos() == QPoint(12, 34));
    }
} // TEST_SUITE

} // namespace sight::module::viz::scene3d_qt::ut
