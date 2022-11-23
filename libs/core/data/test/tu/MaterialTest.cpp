/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "MaterialTest.hpp"

#include <data/Color.hpp>
#include <data/Material.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::MaterialTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void MaterialTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void MaterialTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void MaterialTest::methode1()
{
    //-----------test values
    data::Color::sptr ambient_color = data::Color::New();
    ambient_color->setRGBA(0.5F, 0.5F, 0.5F, 0.5F);

    data::Color::sptr diffuse_color = data::Color::New();
    diffuse_color->setRGBA(0.8F, 0.2F, 0.5F, 0.4F);

    auto material = data::Material::New();

    material->setAmbient(data::Object::copy(ambient_color));
    material->setDiffuse(data::Object::copy(diffuse_color));

    CPPUNIT_ASSERT_EQUAL(material->ambient()->getRGBA()[0], ambient_color->getRGBA()[0]);
    CPPUNIT_ASSERT_EQUAL(material->diffuse()->getRGBA()[0], diffuse_color->getRGBA()[0]);

    auto material2 = data::Material::New();
    CPPUNIT_ASSERT(*material != *material2);

    material2->setAmbient(data::Object::copy(ambient_color));
    material2->setDiffuse(data::Object::copy(diffuse_color));
    CPPUNIT_ASSERT(*material == *material2);
}

//------------------------------------------------------------------------------

void MaterialTest::equalityTest()
{
    auto material1 = data::Material::New();
    auto material2 = data::Material::New();

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

    TEST(setAmbient(data::Color::New(1, 0, 0)));
    TEST(setAmbient(data::Color::New(0, 1, 0)));
    TEST(setAmbient(data::Color::New(0, 0, 1)));
    TEST(setDiffuse(data::Color::New(1, 0, 0)));
    TEST(setDiffuse(data::Color::New(0, 1, 0)));
    TEST(setDiffuse(data::Color::New(0, 0, 1)));
    TEST(setDiffuseTexture(data::Image::New()));
    TEST(setShadingMode(data::Material::AMBIENT));
    TEST(setRepresentationMode(data::Material::POINT));
    TEST(setOptionsMode(data::Material::NORMALS));
    TEST(setDiffuseTextureFiltering(data::Material::LINEAR));
    TEST(setDiffuseTextureWrapping(data::Material::CLAMP));

    #undef TEST
}

} // namespace sight::data::ut
