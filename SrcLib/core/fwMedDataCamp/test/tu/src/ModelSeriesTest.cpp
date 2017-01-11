/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ModelSeriesTest.hpp"

#include "DataCampHelper.hpp"

#include <fwMedDataCamp/Version.hpp>

#include <fwData/Reconstruction.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <boost/assign/std/vector.hpp>

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::ModelSeriesTest );

namespace fwMedDataCamp
{
namespace ut
{

void ModelSeriesTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwMedDataCamp
    const int version = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(version);
}

void ModelSeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ModelSeriesTest::propertiesTest()
{
    const std::string instance_uid               = "123456789";
    const std::string modality                   = "CT";
    const std::string date                       = "20130214";
    const std::string time                       = "143328";
    const std::string performing_physicians_name = "John Doe";
    const std::string description                = "description";
    ::DataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                              ("reconstruction_db")
                                                              ("patient")
                                                              ("study")
                                                              ("equipment")
                                                              ("instance_uid")
                                                              ("modality")
                                                              ("date")
                                                              ("time")
                                                              ("performing_physicians_name")
                                                              ("description");

    ::fwMedData::ModelSeries::ReconstructionVectorType recDB;
    recDB.push_back(::fwData::Reconstruction::New());
    recDB.push_back(::fwData::Reconstruction::New());
    ::fwMedData::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);
    ::fwMedData::ModelSeries::sptr obj = ::fwMedData::ModelSeries::New();

    obj->setInstanceUID(instance_uid);
    obj->setModality(modality);
    obj->setDate(date);
    obj->setTime(time);
    obj->setPerformingPhysiciansName(performing_physicians_names);
    obj->setDescription(description);
    obj->setReconstructionDB(recDB);

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@modality", modality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@study", obj->getStudy());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@reconstruction_db.0", recDB[0]);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@reconstruction_db.1", recDB[1]);
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
