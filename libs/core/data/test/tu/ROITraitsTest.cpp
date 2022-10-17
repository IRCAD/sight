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

#include "ROITraitsTest.hpp"

#include <data/ROITraits.hpp>
#include <data/StructureTraits.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ROITraitsTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void ROITraitsTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ROITraitsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ROITraitsTest::creation()
{
    const std::string ID  = "roiID";
    const std::string EXP = "skin_0";
    auto roiTraits        = data::ROITraits::New();
    roiTraits->setIdentifier(ID);
    auto structure = data::StructureTraits::New();
    roiTraits->setStructureTraits(structure);
    roiTraits->setEvaluatedExp(EXP);
    auto opNode = data::Node::New();
    roiTraits->setMaskOpNode(opNode);

    CPPUNIT_ASSERT_EQUAL(ID, roiTraits->getIdentifier());
    CPPUNIT_ASSERT_EQUAL(EXP, roiTraits->getEvaluatedExp());
    CPPUNIT_ASSERT(structure == roiTraits->getStructureTraits());
    CPPUNIT_ASSERT(opNode == roiTraits->getMaskOpNode());

    auto roiTraits2 = data::ROITraits::New();
    CPPUNIT_ASSERT(*roiTraits != *roiTraits2);

    roiTraits2->setIdentifier(ID);
    roiTraits2->setStructureTraits(structure);
    roiTraits2->setEvaluatedExp(EXP);
    roiTraits2->setMaskOpNode(opNode);
}

} // namespace sight::data::ut
