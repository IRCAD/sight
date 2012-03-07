/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/ROITraits.hpp>
#include <fwData/StructureTraits.hpp>

#include "ROITraitsTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::ROITraitsTest );

namespace fwData
{
namespace ut
{

void ROITraitsTest::setUp()
{
    // Set up context before running a test.

}
void ROITraitsTest::tearDown()
{
    // Clean up after the test run.
}

void ROITraitsTest::creation()
{
    const std::string ID = "roiID";
    const std::string EXP = "skin_0";
    ::fwData::ROITraits::NewSptr roiTraits;
    roiTraits->setIdentifier(ID);
    ::fwData::StructureTraits::NewSptr structure;
    roiTraits->setStructureTraits(structure);
    roiTraits->setEvaluatedExp(EXP);
    ::fwData::Node::NewSptr opNode;
    roiTraits->setMaskOpNode(opNode);

    CPPUNIT_ASSERT_EQUAL(ID, roiTraits->getRefIdentifier());
    CPPUNIT_ASSERT_EQUAL(EXP, roiTraits->getEvaluatedExp());
    CPPUNIT_ASSERT(structure == roiTraits->getStructureTraits() );
    CPPUNIT_ASSERT(opNode == roiTraits->getMaskOpNode() );
}

} //namespace ut
} //namespace fwData

