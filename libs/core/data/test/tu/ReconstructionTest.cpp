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
    auto p1 = data::Reconstruction::New();

    p1->setIsVisible(is_visible);
    p1->setOrganName(organ_name);
    p1->setStructureType(structure_type);

    // check
    CPPUNIT_ASSERT_EQUAL(p1->getIsVisible(), is_visible);
    CPPUNIT_ASSERT_EQUAL(p1->getOrganName(), organ_name);
    CPPUNIT_ASSERT_EQUAL(p1->getStructureType(), structure_type);

    auto p2 = data::Reconstruction::New();
    CPPUNIT_ASSERT(*p1 != *p2);

    p2->setIsVisible(is_visible);
    p2->setOrganName(organ_name);
    p2->setStructureType(structure_type);
}

//------------------------------------------------------------------------------

void ReconstructionTest::image()
{
    data::Reconstruction::sptr p1 = data::Reconstruction::New();
    data::Image::sptr i1(data::Image::New());

    p1->setImage(i1);
    CPPUNIT_ASSERT_EQUAL(p1->getImage(), i1);
}

} // namespace sight::data::ut
