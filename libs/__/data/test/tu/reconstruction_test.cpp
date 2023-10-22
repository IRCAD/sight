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

#include "reconstruction_test.hpp"

#include <data/image.hpp>
#include <data/reconstruction.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::reconstruction_test);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void reconstruction_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void reconstruction_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void reconstruction_test::methode1() //test des setters et getters
{
    const bool is_visible            = true;
    const std::string organ_name     = "OrganName";
    const std::string structure_type = "structure_t";

    // process
    auto p1 = std::make_shared<data::reconstruction>();

    p1->setIsVisible(is_visible);
    p1->setOrganName(organ_name);
    p1->set_structure_type(structure_type);

    // check
    CPPUNIT_ASSERT_EQUAL(p1->getIsVisible(), is_visible);
    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), organ_name);
    CPPUNIT_ASSERT_EQUAL(p1->get_structure_type(), structure_type);

    auto p2 = std::make_shared<data::reconstruction>();
    CPPUNIT_ASSERT(*p1 != *p2);

    p2->setIsVisible(is_visible);
    p2->setOrganName(organ_name);
    p2->set_structure_type(structure_type);
}

//------------------------------------------------------------------------------

void reconstruction_test::image()
{
    data::reconstruction::sptr p1 = std::make_shared<data::reconstruction>();
    data::image::sptr i1(std::make_shared<data::image>());

    p1->setImage(i1);
    CPPUNIT_ASSERT_EQUAL(p1->getImage(), i1);
}

//------------------------------------------------------------------------------

void reconstruction_test::equalityTest()
{
    auto reconstruction1 = std::make_shared<data::reconstruction>();
    auto reconstruction2 = std::make_shared<data::reconstruction>();

    CPPUNIT_ASSERT(*reconstruction1 == *reconstruction2 && !(*reconstruction1 != *reconstruction2));

    // NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
    #define TEST(op) \
    reconstruction1->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Reconstructions should be different when using " #op " on the first one", \
        *reconstruction1 != *reconstruction2 && !(*reconstruction1 == *reconstruction2) \
    ); \
    reconstruction2->op; \
    CPPUNIT_ASSERT_MESSAGE( \
        "Reconstructions should be equal when using " #op " on both", \
        *reconstruction1 == *reconstruction2 && !(*reconstruction1 != *reconstruction2) \
    );

    TEST(setIsVisible(true));
    TEST(setOrganName("1"));
    TEST(set_structure_type("2"));
    TEST(setImage(std::make_shared<data::image>()));
    TEST(setMesh(std::make_shared<data::mesh>()));
    auto material = std::make_shared<data::material>();
    material->setAmbient(std::make_shared<data::color>(3.F, 4.F, 5.F));
    material->setDiffuse(std::make_shared<data::color>(6.F, 7.F, 8.F));
    material->setShadingMode(data::material::AMBIENT);
    material->setRepresentationMode(data::material::POINT);
    material->setOptionsMode(data::material::NORMALS);
    material->setDiffuseTextureFiltering(data::material::LINEAR);
    material->setDiffuseTextureWrapping(data::material::CLAMP);
    TEST(setMaterial(material));
    TEST(setComputedMaskVolume(9));

    #undef TEST
}

} // namespace sight::data::ut
