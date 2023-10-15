/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "material_test.hpp"

#include <data/color.hpp>
#include <data/material.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::material_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void material_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void material_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void material_test::methode1()
{
    //-----------test values
    data::color::sptr ambient_color = std::make_shared<data::color>();
    ambient_color->setRGBA(0.5F, 0.5F, 0.5F, 0.5F);

    data::color::sptr diffuse_color = std::make_shared<data::color>();
    diffuse_color->setRGBA(0.8F, 0.2F, 0.5F, 0.4F);

    auto material = std::make_shared<data::material>();

    material->setAmbient(data::object::copy(ambient_color));
    material->setDiffuse(data::object::copy(diffuse_color));

    CPPUNIT_ASSERT_EQUAL(material->ambient()->getRGBA()[0], ambient_color->getRGBA()[0]);
    CPPUNIT_ASSERT_EQUAL(material->diffuse()->getRGBA()[0], diffuse_color->getRGBA()[0]);

    auto material2 = std::make_shared<data::material>();
    CPPUNIT_ASSERT(*material != *material2);

    material2->setAmbient(data::object::copy(ambient_color));
    material2->setDiffuse(data::object::copy(diffuse_color));
    CPPUNIT_ASSERT(*material == *material2);
}

//------------------------------------------------------------------------------

void material_test::equalityTest()
{
    auto material1 = std::make_shared<data::material>();
    auto material2 = std::make_shared<data::material>();

    CPPUNIT_ASSERT(*material1 == *material2 && !(*material1 != *material2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
    material1->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Materials should be different when using " #op " on the first one", \
        *material1 != *material2 && !(*material1 == *material2) \
    ); \
    material2->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Materials should be equal when using " #op " on both", \
        *material1 == *material2 && !(*material1 != *material2) \
    );

    TEST(setAmbient(std::make_shared<data::color>(1.F, 0.F, 0.F)));
    TEST(setAmbient(std::make_shared<data::color>(0.F, 1.F, 0.F)));
    TEST(setAmbient(std::make_shared<data::color>(0.F, 0.F, 1.F)));
    TEST(setDiffuse(std::make_shared<data::color>(1.F, 0.F, 0.F)));
    TEST(setDiffuse(std::make_shared<data::color>(0.F, 1.F, 0.F)));
    TEST(setDiffuse(std::make_shared<data::color>(0.F, 0.F, 1.F)));
    TEST(setDiffuseTexture(std::make_shared<data::image>()));
    TEST(setShadingMode(data::material::AMBIENT));
    TEST(setRepresentationMode(data::material::POINT));
    TEST(setOptionsMode(data::material::NORMALS));
    TEST(setDiffuseTextureFiltering(data::material::LINEAR));
    TEST(setDiffuseTextureWrapping(data::material::CLAMP));

    #undef TEST
}

} // namespace sight::data::ut
