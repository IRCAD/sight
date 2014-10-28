/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwStructuralPatch/creator/fwMedData/Equipment1.hpp"
#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/helper/Object.hpp"
#include "fwAtoms/String.hpp"

#include "creator/fwMedData/Equipment1Test.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwStructuralPatch::ut::creator::fwMedData::Equipment1Test );

namespace fwStructuralPatch
{
namespace ut
{
namespace creator
{
namespace fwMedData
{

//------------------------------------------------------------------------------

void Equipment1Test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void Equipment1Test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void Equipment1Test::createTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    Equipment1::sptr equipmentCreator = Equipment1::New();

    CPPUNIT_ASSERT(equipmentCreator);
    const std::string& classname = equipmentCreator->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::Equipment") == classname );
    const std::string& objectVersion =  equipmentCreator->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersion );

    ::fwAtoms::Object::sptr equipment = equipmentCreator->create();

    const std::string& equipmentClassname = ::fwAtomsPatch::helper::getClassname(equipment);
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::Equipment") == equipmentClassname );
    const std::string& equipmentObjectVersion = ::fwAtomsPatch::helper::getVersion(equipment);
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == equipmentObjectVersion );

    CPPUNIT_ASSERT(equipment->getAttribute("institution_name"));
}

//------------------------------------------------------------------------------

void Equipment1Test::copyTest()
{
    using namespace ::fwStructuralPatch::creator::fwMedData;
    Equipment1 equipmentCreator;

    const std::string& classname = equipmentCreator.getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::Equipment") == classname );
    const std::string& objectVersion =  equipmentCreator.getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersion );

    Equipment1* equipmentCreator2 = new Equipment1(equipmentCreator);

    CPPUNIT_ASSERT(equipmentCreator2);
    const std::string& classnameEquipmentCreator2 = equipmentCreator2->getObjectClassname();
    CPPUNIT_ASSERT_MESSAGE("Classname must be equal" , std::string("::fwMedData::Equipment") == classnameEquipmentCreator2 );

    const std::string& objectVersionEquipmentCreator2 =  equipmentCreator2->getObjectVersion();
    CPPUNIT_ASSERT_MESSAGE("ObjectVersion must be equal" , std::string("1") == objectVersionEquipmentCreator2 );

    delete(equipmentCreator2);

}

//------------------------------------------------------------------------------

} //namespace fwMedData
} //namespace creator
} //namespace ut
} //namespace fwStructuralPatch
