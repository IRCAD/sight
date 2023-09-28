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

#include "ReconstructionTest.hpp"

#include <data/Image.hpp>
#include <data/Reconstruction.hpp>

#include <exception>
#include <iostream>
#include <map>
#include <ostream>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ReconstructionTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void ReconstructionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ReconstructionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ReconstructionTest::methode1() //test des setters et getters
{
    const bool is_visible            = true;
    const std::string organ_name     = "OrganName";
    const std::string structure_type = "StructureType";

    // process
    auto p1 = std::make_shared<data::Reconstruction>();

    p1->setIsVisible(is_visible);
    p1->setOrganName(organ_name);
    p1->setStructureType(structure_type);

    // check
    CPPUNIT_ASSERT_EQUAL(p1->getIsVisible(), is_visible);
    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), organ_name);
    CPPUNIT_ASSERT_EQUAL(p1->getStructureType(), structure_type);

    auto p2 = std::make_shared<data::Reconstruction>();
    CPPUNIT_ASSERT(*p1 != *p2);

    p2->setIsVisible(is_visible);
    p2->setOrganName(organ_name);
    p2->setStructureType(structure_type);
}

//------------------------------------------------------------------------------

void ReconstructionTest::image()
{
    data::Reconstruction::sptr p1 = std::make_shared<data::Reconstruction>();
    data::Image::sptr i1(std::make_shared<data::Image>());

    p1->setImage(i1);
    CPPUNIT_ASSERT_EQUAL(p1->getImage(), i1);
}

//------------------------------------------------------------------------------

void ReconstructionTest::equalityTest()
{
    auto reconstruction1 = std::make_shared<data::Reconstruction>();
    auto reconstruction2 = std::make_shared<data::Reconstruction>();

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
    TEST(setStructureType("2"));
    TEST(setImage(std::make_shared<data::Image>()));
    TEST(setMesh(std::make_shared<data::Mesh>()));
    auto material = std::make_shared<data::Material>();
    material->setAmbient(std::make_shared<data::Color>(3.F, 4.F, 5.F));
    material->setDiffuse(std::make_shared<data::Color>(6.F, 7.F, 8.F));
    material->setShadingMode(data::Material::AMBIENT);
    material->setRepresentationMode(data::Material::POINT);
    material->setOptionsMode(data::Material::NORMALS);
    material->setDiffuseTextureFiltering(data::Material::LINEAR);
    material->setDiffuseTextureWrapping(data::Material::CLAMP);
    TEST(setMaterial(material));
    TEST(setComputedMaskVolume(9));

    #undef TEST
}

} // namespace sight::data::ut
