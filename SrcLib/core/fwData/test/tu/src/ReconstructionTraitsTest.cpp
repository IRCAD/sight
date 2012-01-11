/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/ReconstructionTraits.hpp>
#include <fwData/StructureTraits.hpp>

#include "ReconstructionTraitsTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ReconstructionTraitsTest );

void ReconstructionTraitsTest::setUp()
{
    // Set up context before running a test.

}
void ReconstructionTraitsTest::tearDown()
{
    // Clean up after the test run.
}

void ReconstructionTraitsTest::creation()
{
    const std::string ID = "roiID";
    const std::string EXP = "skin_0";
    ::fwData::ReconstructionTraits::NewSptr recTraits;
    recTraits->setIdentifier(ID);
    ::fwData::StructureTraits::NewSptr structure;
    recTraits->setStructureTraits(structure);

    CPPUNIT_ASSERT(!recTraits->getMaskOpNode());
    CPPUNIT_ASSERT(!recTraits->getMeshOpNode());

    ::fwData::Node::NewSptr opMaskNode;
    recTraits->setMaskOpNode(opMaskNode);

    ::fwData::Node::NewSptr opMeskNode;
    recTraits->setMeshOpNode(opMeskNode);

    CPPUNIT_ASSERT_EQUAL(ID, recTraits->getRefIdentifier());
    CPPUNIT_ASSERT(structure == recTraits->getStructureTraits() );
    CPPUNIT_ASSERT(opMaskNode == recTraits->getMaskOpNode() );
    CPPUNIT_ASSERT(opMeskNode == recTraits->getMeshOpNode() );
}
