/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ReconstructionTraitsTest.hpp"

#include <fwData/ReconstructionTraits.hpp>
#include <fwData/StructureTraits.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ReconstructionTraitsTest );

namespace fwData
{
namespace ut
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
    const std::string ID  = "roiID";
    const std::string EXP = "skin_0";
    ::fwData::ReconstructionTraits::sptr recTraits = ::fwData::ReconstructionTraits::New();
    recTraits->setIdentifier(ID);
    ::fwData::StructureTraits::sptr structure = ::fwData::StructureTraits::New();
    recTraits->setStructureTraits(structure);

    CPPUNIT_ASSERT(!recTraits->getMaskOpNode());
    CPPUNIT_ASSERT(!recTraits->getMeshOpNode());

    ::fwData::Node::sptr opMaskNode = ::fwData::Node::New();
    recTraits->setMaskOpNode(opMaskNode);

    ::fwData::Node::sptr opMeskNode = ::fwData::Node::New();
    recTraits->setMeshOpNode(opMeskNode);

    CPPUNIT_ASSERT_EQUAL(ID, recTraits->getIdentifier());
    CPPUNIT_ASSERT(structure == recTraits->getStructureTraits() );
    CPPUNIT_ASSERT(opMaskNode == recTraits->getMaskOpNode() );
    CPPUNIT_ASSERT(opMeskNode == recTraits->getMeshOpNode() );
}

} //namespace ut
} //namespace fwData
