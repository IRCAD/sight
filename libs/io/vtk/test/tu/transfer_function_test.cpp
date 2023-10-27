/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "transfer_function_test.hpp"

#include <data/transfer_function.hpp>

#include <io/vtk/helper/transfer_function.hpp>

#include <array>
#include <cmath>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::io::vtk::ut::transfer_function_test);

namespace sight::io::vtk::ut
{

//------------------------------------------------------------------------------

void transfer_function_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void transfer_function_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

data::transfer_function::sptr transfer_function_test::createTFColor()
{
    data::transfer_function::sptr tf = std::make_shared<data::transfer_function>();

    tf->setBackgroundColor(data::transfer_function::color_t(1.0, 0.3, 0.6, 0.1));
    tf->setName("color_t");
    tf->setLevel(0.0);
    tf->setWindow(400.0);

    auto tf_data = tf->pieces().emplace_back(std::make_shared<data::transfer_function_piece>());
    tf_data->setClamped(false);
    tf_data->setInterpolationMode(data::transfer_function::InterpolationMode::LINEAR);
    tf_data->setLevel(0.0);
    tf_data->setWindow(400.0);

    tf_data->insert({-200, data::transfer_function::color_t(1.0, 0.0, 0.0, 0.0)});
    tf_data->insert({0, data::transfer_function::color_t(0.0, 1.0, 0.0, 0.0)});
    tf_data->insert({100, data::transfer_function::color_t(0.0, 0.0, 1.0, 0.5)});
    tf_data->insert({200, data::transfer_function::color_t(0.0, 1.0, 1.0, 1.0)});

    return tf;
}

//------------------------------------------------------------------------------

void transfer_function_test::toVtkLookupTableTest()
{
    data::transfer_function::sptr tf   = this->createTFColor();
    vtkSmartPointer<vtkLookupTable> lt = vtkSmartPointer<vtkLookupTable>::New();

    std::array<double, 3> color {};
    double opacity         = NAN;
    double color_tolerance = 1.0 / 255.0;

    io::vtk::helper::transfer_function::toVtkLookupTable(tf, lt, true, 4096);
    lt->GetColor(0, color.data());
    opacity = lt->GetOpacity(0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, opacity, color_tolerance);

    lt->GetColor(200, color.data());
    opacity = lt->GetOpacity(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, opacity, color_tolerance);

    lt->GetColor(150, color.data());
    opacity = lt->GetOpacity(150);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.75, opacity, color_tolerance);

    tf->pieces()[0]->setInterpolationMode(data::transfer_function::InterpolationMode::NEAREST);
    io::vtk::helper::transfer_function::toVtkLookupTable(tf, lt, true, 4096);
    lt->GetColor(120, color.data());
    opacity = lt->GetOpacity(120);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, opacity, color_tolerance);

    tf->setLevel(200);
    io::vtk::helper::transfer_function::toVtkLookupTable(tf, lt, true, 4096);
    lt->GetColor(200, color.data());
    opacity = lt->GetOpacity(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, opacity, color_tolerance);

    tf->setLevel(0);
    tf->setWindow(800);
    io::vtk::helper::transfer_function::toVtkLookupTable(tf, lt, true, 4096);
    lt->GetColor(200, color.data());
    opacity = lt->GetOpacity(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, opacity, color_tolerance);

    io::vtk::helper::transfer_function::toVtkLookupTable(tf, lt, false, 4096);
    lt->GetColor(200, color.data());
    opacity = lt->GetOpacity(200);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, opacity, color_tolerance);
}

//------------------------------------------------------------------------------

void transfer_function_test::toBWVtkLookupTableTest()
{
    vtkSmartPointer<vtkLookupTable> lt = vtkSmartPointer<vtkLookupTable>::New();
    io::vtk::helper::transfer_function::toBWVtkLookupTable(0.0, 100.0, lt, 4096);

    std::array<double, 3> color {};
    double opacity         = NAN;
    double color_tolerance = 1.0 / 255.0;

    lt->GetColor(0, color.data());
    opacity = lt->GetOpacity(0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, opacity, color_tolerance);

    lt->GetColor(100, color.data());
    opacity = lt->GetOpacity(100);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, opacity, color_tolerance);

    lt->GetColor(50, color.data());
    opacity = lt->GetOpacity(50);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, color[0], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, color[1], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, color[2], color_tolerance);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, opacity, color_tolerance);
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk::ut
