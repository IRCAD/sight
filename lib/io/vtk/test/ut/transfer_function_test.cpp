/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <data/transfer_function.hpp>

#include <io/vtk/helper/transfer_function.hpp>

#include <doctest/doctest.h>

#include <array>
#include <cmath>

TEST_SUITE("sight::io::vtk::transfer_function")
{
//------------------------------------------------------------------------------

    static sight::data::transfer_function::sptr create_tf_color()
    {
        sight::data::transfer_function::sptr tf = std::make_shared<sight::data::transfer_function>();

        tf->set_background_color(sight::data::transfer_function::color_t(1.0, 0.3, 0.6, 0.1));
        tf->set_name("color_t");
        tf->set_level(0.0);
        tf->set_window(400.0);

        auto tf_data = tf->pieces().emplace_back(std::make_shared<sight::data::transfer_function_piece>());
        tf_data->set_clamped(false);
        tf_data->set_interpolation_mode(sight::data::transfer_function::interpolation_mode::linear);
        tf_data->set_level(0.0);
        tf_data->set_window(400.0);

        tf_data->insert({-200, sight::data::transfer_function::color_t(1.0, 0.0, 0.0, 0.0)});
        tf_data->insert({0, sight::data::transfer_function::color_t(0.0, 1.0, 0.0, 0.0)});
        tf_data->insert({100, sight::data::transfer_function::color_t(0.0, 0.0, 1.0, 0.5)});
        tf_data->insert({200, sight::data::transfer_function::color_t(0.0, 1.0, 1.0, 1.0)});

        return tf;
    }

//------------------------------------------------------------------------------

    TEST_CASE("to_vtk_lookup_table")
    {
        sight::data::transfer_function::sptr tf = create_tf_color();
        vtkSmartPointer<vtkLookupTable> lt      = vtkSmartPointer<vtkLookupTable>::New();

        std::array<double, 3> color {};
        double opacity         = NAN;
        double color_tolerance = 1.0 / 255.0;

        sight::io::vtk::helper::transfer_function::to_vtk_lookup_table(tf, lt, true, 4096);
        lt->GetColor(0, color.data());
        opacity = lt->GetOpacity(0);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), opacity);

        lt->GetColor(200, color.data());
        opacity = lt->GetOpacity(200);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), opacity);

        lt->GetColor(150, color.data());
        opacity = lt->GetOpacity(150);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(0.5).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(0.75).epsilon(color_tolerance), opacity);

        tf->pieces()[0]->set_interpolation_mode(sight::data::transfer_function::interpolation_mode::nearest);
        sight::io::vtk::helper::transfer_function::to_vtk_lookup_table(tf, lt, true, 4096);
        lt->GetColor(120, color.data());
        opacity = lt->GetOpacity(120);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(0.5).epsilon(color_tolerance), opacity);

        tf->set_level(200);
        sight::io::vtk::helper::transfer_function::to_vtk_lookup_table(tf, lt, true, 4096);
        lt->GetColor(200, color.data());
        opacity = lt->GetOpacity(200);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), opacity);

        tf->set_level(0);
        tf->set_window(800);
        sight::io::vtk::helper::transfer_function::to_vtk_lookup_table(tf, lt, true, 4096);
        lt->GetColor(200, color.data());
        opacity = lt->GetOpacity(200);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(0.5).epsilon(color_tolerance), opacity);

        sight::io::vtk::helper::transfer_function::to_vtk_lookup_table(tf, lt, false, 4096);
        lt->GetColor(200, color.data());
        opacity = lt->GetOpacity(200);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), opacity);
    }

//------------------------------------------------------------------------------

    TEST_CASE("to_bw_vtk_lookup_table")
    {
        vtkSmartPointer<vtkLookupTable> lt = vtkSmartPointer<vtkLookupTable>::New();
        sight::io::vtk::helper::transfer_function::to_bw_vtk_lookup_table(0.0, 100.0, lt, 4096);

        std::array<double, 3> color {};
        double opacity         = NAN;
        double color_tolerance = 1.0 / 255.0;

        lt->GetColor(0, color.data());
        opacity = lt->GetOpacity(0);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(0.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), opacity);

        lt->GetColor(100, color.data());
        opacity = lt->GetOpacity(100);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), opacity);

        lt->GetColor(50, color.data());
        opacity = lt->GetOpacity(50);
        CHECK_EQ(doctest::Approx(0.5).epsilon(color_tolerance), color[0]);
        CHECK_EQ(doctest::Approx(0.5).epsilon(color_tolerance), color[1]);
        CHECK_EQ(doctest::Approx(0.5).epsilon(color_tolerance), color[2]);
        CHECK_EQ(doctest::Approx(1.0).epsilon(color_tolerance), opacity);
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
