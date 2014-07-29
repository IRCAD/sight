/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwTools/Type.hpp>
#include <fwTools/System.hpp>
#include <fwData/Array.hpp>

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Equipment.hpp>

#include "DataCampHelper.hpp"
#include "fwDicomDataCamp/DicomSeriesCampTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDicomDataCamp::ut::DicomSeriesCampTest );

namespace fwDicomDataCamp
{
namespace ut
{

//------------------------------------------------------------------------------

void DicomSeriesCampTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void DicomSeriesCampTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void DicomSeriesCampTest::propertiesTest()
{
    const std::string instance_uid = "123456789";
    const std::string modality = "CT";
    const std::string date = "20130214";
    const std::string time = "143328";
    const std::string performing_physicians_name = "John Doe";
    const std::string description = "description";
    //DicomSeries
    const std::string dicom_availability = "PATHS";
    const std::string number_of_instances = "100";
    ::DataCampHelper::PropertiesNameType dataProperties = ::boost::assign::list_of("fields")
                                                                  ("dicom_availability")
                                                                  ("number_of_instances")
                                                                  ("local_dicom_paths")
                                                                  ("dicom_binaries")
                                                                  ("patient")
                                                                  ("study")
                                                                  ("equipment")
                                                                  ("instance_uid")
                                                                  ("modality")
                                                                  ("date")
                                                                  ("time")
                                                                  ("performing_physicians_name")
                                                                  ("description");


    ::fwData::Array::sptr binary = ::fwData::Array::New();
    ::boost::filesystem::path path = "mypath";

    ::fwMedData::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);

    ::fwDicomData::DicomSeries::sptr obj = ::fwDicomData::DicomSeries::New();
    obj->setInstanceUID(instance_uid);
    obj->setModality(modality);
    obj->setDate(date);
    obj->setTime(time);
    obj->setPerformingPhysiciansName(performing_physicians_names);
    obj->setDescription(description);
    obj->setNumberOfInstances(100);
    obj->setDicomAvailability(::fwDicomData::DicomSeries::PATHS);
    obj->addDicomPath(42, path);
    obj->addBinary(path.string(),binary);



    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@modality", modality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@study", obj->getStudy());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@equipment", obj->getEquipment());

    // Dicom availability
    //TODO: Enum are not supported yet, so we cannot make unit tests on this part...
    //::DataCampHelper::compareSimplePropertyValue(obj, "@dicom_availability", std::string("PATH"));

    // Number of instances
    ::std::stringstream ss;
    ss << obj->getNumberOfInstances();
    ::DataCampHelper::compareSimplePropertyValue(obj, "@number_of_instances", ss.str());

    // Local dicom paths
    ::DataCampHelper::compareSimplePropertyValue(obj, "@local_dicom_paths.42", "mypath");

    // Dicom binaries
    ::DataCampHelper::compareObjectPropertyValue(obj, "@dicom_binaries.mypath", obj->getDicomBinaries().at("mypath"));


}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwDicomDataCamp
