/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/assign/std/vector.hpp>

#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/Image.hpp>

#include <fwDataCamp/Version.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include "fwMedDataCamp/MedDataCampHelper.hpp"
#include "fwMedDataCamp/ImageTest.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::ImageTest );

namespace fwMedDataCamp
{
namespace ut
{

void ImageTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
}

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageTest::propertiesTest()
{
    const std::string instance_uid = "123456789";
    const std::string modality = "CT";
    const std::string date = "20130214";
    const std::string time = "143328";
    const std::string performing_physicians_name = "John Doe";
    const std::string description = "description";
    const MedDataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                        ("image")
                                                                        ("patient")
                                                                        ("study")
                                                                        ("equipment")
                                                                        ("instance_uid")
                                                                        ("modality")
                                                                        ("date")
                                                                        ("time")
                                                                        ("performing_physicians_name")
                                                                        ("description");

    ::fwData::Image::sptr img = ::fwData::Image::New();
    ::fwDataTools::Image::generateRandomImage(img, ::fwTools::Type::create("uint8"));
    ::fwMedData::DicomValuesType performing_physicians_names;
    performing_physicians_names.push_back(performing_physicians_name);

    ::fwMedData::ImageSeries::sptr obj = ::fwMedData::ImageSeries::New();
    obj->setInstanceUID(instance_uid);
    obj->setModality(modality);
    obj->setDate(date);
    obj->setTime(time);
    obj->setPerformingPhysiciansName(performing_physicians_names);
    obj->setDescription(description);
    obj->setImage(img);

    MedDataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@modality", modality);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    MedDataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    MedDataCampHelper::compareObjectPropertyValue(obj, "@image", obj->getImage());
    MedDataCampHelper::compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    MedDataCampHelper::compareObjectPropertyValue(obj, "@study", obj->getStudy());
    MedDataCampHelper::compareObjectPropertyValue(obj, "@equipment", obj->getEquipment());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
