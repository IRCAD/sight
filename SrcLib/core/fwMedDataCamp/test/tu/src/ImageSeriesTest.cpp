/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ImageSeriesTest.hpp"

#include "DataCampHelper.hpp"

#include <fwMedDataCamp/Version.hpp>

#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwTest/generator/Image.hpp>

#include <boost/assign/std/vector.hpp>

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::ImageSeriesTest );

namespace fwMedDataCamp
{
namespace ut
{

//------------------------------------------------------------------------------

void ImageSeriesTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwMedDataCamp
    const int version = ::fwMedDataCamp::Version::s_CURRENT_VERSION;
    FwCoreNotUsedMacro(version);
}

//------------------------------------------------------------------------------

void ImageSeriesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageSeriesTest::propertiesTest()
{
    const std::string instance_uid                            = "123456789";
    const std::string modality                                = "CT";
    const std::string date                                    = "20130214";
    const std::string time                                    = "143328";
    const std::string performing_physicians_name              = "John Doe";
    const std::string description                             = "description";
    const ::DataCampHelper::PropertiesNameType dataProperties = list_of("fields")
                                                                    ("image")
                                                                    ("patient")
                                                                    ("study")
                                                                    ("equipment")
                                                                    ("instance_uid")
                                                                    ("modality")
                                                                    ("date")
                                                                    ("time")
                                                                    ("performing_physicians_name")
                                                                    ("description")
                                                                    ("dicom_reference");

    ::fwData::Image::sptr img = ::fwData::Image::New();
    ::fwTest::generator::Image::generateRandomImage(img, ::fwTools::Type::create("uint8"));
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

    ::DataCampHelper::visitProperties(obj->getClassname(), dataProperties);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@modality", modality);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@date", date);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@time", time);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    ::DataCampHelper::compareSimplePropertyValue(obj, "@description", description);
    ::DataCampHelper::compareObjectPropertyValue(obj, "@image", obj->getImage());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@study", obj->getStudy());
    ::DataCampHelper::compareObjectPropertyValue(obj, "@equipment", obj->getEquipment());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwMedDataCamp
