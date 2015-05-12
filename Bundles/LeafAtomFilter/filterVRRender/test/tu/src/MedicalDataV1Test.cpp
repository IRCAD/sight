/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

#include <fwAtomsFilter/IFilter.hpp>

#include "MedicalDataV1Test.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::filterVRRender::ut::MedicalDataV1Test );

namespace filterVRRender
{
namespace ut
{

//------------------------------------------------------------------------------

void MedicalDataV1Test::setUp()
{
}

//------------------------------------------------------------------------------

void MedicalDataV1Test::tearDown()
{
}

//------------------------------------------------------------------------------

void MedicalDataV1Test::applyTest()
{
    ::fwAtomsFilter::IFilter::sptr filter = ::fwAtomsFilter::factory::New("VRRenderMedicalDataV1");
    SLM_ASSERT("Failed to instantiate filter", filter);

    ::fwAtoms::Map::sptr valuesPatientDB = ::fwAtoms::Map::New();
    valuesPatientDB->insert("patient1", ::fwAtoms::Object::New());
    valuesPatientDB->insert("patient2", ::fwAtoms::Object::New());

    ::fwAtoms::Map::sptr valuesProcessingDB = ::fwAtoms::Map::New();
    valuesProcessingDB->insert("processing1", ::fwAtoms::Object::New());
    valuesProcessingDB->insert("processing2", ::fwAtoms::Object::New());

    ::fwAtoms::Map::sptr valuesPlanningDB = ::fwAtoms::Map::New();
    valuesPlanningDB->insert("planning1", ::fwAtoms::Object::New());
    valuesPlanningDB->insert("planning2", ::fwAtoms::Object::New());


    // Create patientDB
    ::fwAtoms::Object::sptr patientDB = ::fwAtoms::Object::New();
    patientDB->setAttribute("values", valuesPatientDB);

    // Create processingDB
    ::fwAtoms::Object::sptr processingDB = ::fwAtoms::Object::New();
    processingDB->setAttribute("values", valuesProcessingDB);

    // Create planningDB
    ::fwAtoms::Object::sptr planningDB = ::fwAtoms::Object::New();
    planningDB->setAttribute("values", valuesPlanningDB);

    // Create medical workspace
    ::fwAtoms::Map::sptr valuesMedicalWorkspace = ::fwAtoms::Map::New();
    valuesMedicalWorkspace->insert("processingDB", processingDB);
    valuesMedicalWorkspace->insert("planningDB", planningDB);
    valuesMedicalWorkspace->insert("patientDB", patientDB);

    ::fwAtoms::Object::sptr medicalWorkspace = ::fwAtoms::Object::New();
    ::fwAtomsPatch::helper::setClassname(medicalWorkspace, "::fwData::Composite");
    medicalWorkspace->setAttribute("values", valuesMedicalWorkspace);

    filter->apply(medicalWorkspace);

    CPPUNIT_ASSERT(valuesPlanningDB->empty());
    CPPUNIT_ASSERT(valuesProcessingDB->empty());
    CPPUNIT_ASSERT(valuesPatientDB->size() == 2);
    CPPUNIT_ASSERT(valuesMedicalWorkspace->find("processingDB") != valuesMedicalWorkspace->end());
    CPPUNIT_ASSERT(valuesMedicalWorkspace->find("planningDB") != valuesMedicalWorkspace->end());
    CPPUNIT_ASSERT(valuesMedicalWorkspace->find("patientDB") != valuesMedicalWorkspace->end());
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace filterVRRender

