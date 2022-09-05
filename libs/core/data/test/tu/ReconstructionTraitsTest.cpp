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

#include "ReconstructionTraitsTest.hpp"

#include <data/ReconstructionTraits.hpp>
#include <data/StructureTraits.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::data::ut::ReconstructionTraitsTest);

namespace sight::data::ut
{

//------------------------------------------------------------------------------

void ReconstructionTraitsTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ReconstructionTraitsTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ReconstructionTraitsTest::creation()
{
    const std::string ID                       = "roiID";
    const std::string EXP                      = "skin_0";
    data::ReconstructionTraits::sptr recTraits = data::ReconstructionTraits::New();
    recTraits->setIdentifier(ID);
    data::StructureTraits::sptr structure = data::StructureTraits::New();
    recTraits->setStructureTraits(structure);

    CPPUNIT_ASSERT(!recTraits->getMaskOpNode());
    CPPUNIT_ASSERT(!recTraits->getMeshOpNode());

    data::Node::sptr opMaskNode = data::Node::New();
    recTraits->setMaskOpNode(opMaskNode);

    data::Node::sptr opMeshNode = data::Node::New();
    recTraits->setMeshOpNode(opMeshNode);

    CPPUNIT_ASSERT_EQUAL(ID, recTraits->getIdentifier());
    CPPUNIT_ASSERT(structure == recTraits->getStructureTraits());
    CPPUNIT_ASSERT(opMaskNode == recTraits->getMaskOpNode());
    CPPUNIT_ASSERT(opMeshNode == recTraits->getMeshOpNode());

    auto recTraits2 = data::ReconstructionTraits::New();
    CPPUNIT_ASSERT(*recTraits != *recTraits2);

    recTraits2->setIdentifier(ID);
    recTraits2->setStructureTraits(structure);
    recTraits2->setMaskOpNode(opMaskNode);
    recTraits2->setMeshOpNode(opMeshNode);

    CPPUNIT_ASSERT(*recTraits == *recTraits2);
}

} // namespace sight::data::ut
