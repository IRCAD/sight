/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <boost/assign/std/vector.hpp>

#include <fwData/GenericFieldBase.hpp>
#include <fwData/Vector.hpp>
#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/Image.hpp>

#include <fwDataCamp/Version.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#include <fwAtomConversion/RetreiveObjectVisitor.hpp>

#include "fwMedDataCamp/SeriesPropertiesTest.hpp"

using namespace ::boost::assign;

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMedDataCamp::ut::SeriesPropertiesTest );

namespace fwMedDataCamp
{
namespace ut
{

//------------------------------------------------------------------------------

typedef std::vector<std::string> PropertiesNameType;
void visitProperties(const std::string& className, const PropertiesNameType& vecProp)
{
    const ::camp::Class& metaClass = ::camp::classByName(className);

    CPPUNIT_ASSERT_EQUAL( vecProp.size(), metaClass.propertyCount()) ;
    BOOST_FOREACH(const std::string& property, vecProp)
    {
        CPPUNIT_ASSERT_MESSAGE("Missing property "+ property+" in fwMedData::Equipment",
                metaClass.hasProperty(property));
    }
}

//------------------------------------------------------------------------------

void compareSimplePropertyValue(::fwData::Object::sptr obj,
                                const std::string& propertyPath,
                                const std::string& value)
{
    ::fwData::GenericFieldBase::sptr field;
    field = ::fwAtomConversion::getSubObject< ::fwData::GenericFieldBase >(obj, propertyPath);
    CPPUNIT_ASSERT_MESSAGE("Retrieve failed for property "+propertyPath, field);
    CPPUNIT_ASSERT_EQUAL( value, field->toString());
}

//------------------------------------------------------------------------------

void compareObjectPropertyValue(::fwData::Object::sptr obj,
                                const std::string& propertyPath,
                                ::fwData::Object::sptr value)
{
    ::fwData::Object::sptr subObj;
    subObj = ::fwAtomConversion::getSubObject(obj, propertyPath);
    CPPUNIT_ASSERT_MESSAGE("Retrieve failed for property "+propertyPath, subObj);
    CPPUNIT_ASSERT_MESSAGE("Retrieve property "+propertyPath+" not equal with value", value == subObj);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void SeriesPropertiesTest::setUp()
{
    // Set up context before running a test.
    //Force link with fwDataCamp
    const int version = ::fwDataCamp::Version::s_CURRENT_VERSION;
}

void SeriesPropertiesTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SeriesPropertiesTest::equipmentPropertiesTest()
{
    const std::string institution_name = "InstitutionName";
    const PropertiesNameType dataProperties = list_of("fields")
                                                     ("institution_name");

    ::fwMedData::Equipment::sptr obj = ::fwMedData::Equipment::New();
    obj->setInstitutionName(institution_name);

    visitProperties(obj->getClassname(), dataProperties);
    compareSimplePropertyValue(obj, "@institution_name", institution_name);

}

//------------------------------------------------------------------------------

void SeriesPropertiesTest::imageSeriesPropertiesTest()
{
    const std::string instance_uid = "123456789";
    const std::string modality = "CT";
    const std::string date = "20130214";
    const std::string time = "143328";
    const std::string performing_physicians_name = "John Doe";
    const std::string description = "description";
    const PropertiesNameType dataProperties = list_of("fields")
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

    visitProperties(obj->getClassname(), dataProperties);
    compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    compareSimplePropertyValue(obj, "@modality", modality);
    compareSimplePropertyValue(obj, "@date", date);
    compareSimplePropertyValue(obj, "@time", time);
    compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    compareSimplePropertyValue(obj, "@description", description);
    compareObjectPropertyValue(obj, "@image", obj->getImage());
    compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    compareObjectPropertyValue(obj, "@study", obj->getStudy());
    compareObjectPropertyValue(obj, "@equipment", obj->getEquipment());
}

//------------------------------------------------------------------------------

void SeriesPropertiesTest::modelSeriesPropertiesTest()
{
    const std::string instance_uid = "123456789";
    const std::string modality = "CT";
    const std::string date = "20130214";
    const std::string time = "143328";
    const std::string performing_physicians_name = "John Doe";
    const std::string description = "description";
    PropertiesNameType dataProperties = list_of("fields")
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

    visitProperties(obj->getClassname(), dataProperties);
    compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    compareSimplePropertyValue(obj, "@modality", modality);
    compareSimplePropertyValue(obj, "@date", date);
    compareSimplePropertyValue(obj, "@time", time);
    compareSimplePropertyValue(obj, "@performing_physicians_name.0", performing_physicians_names[0]);
    compareSimplePropertyValue(obj, "@description", description);
    compareObjectPropertyValue(obj, "@patient", obj->getPatient());
    compareObjectPropertyValue(obj, "@study", obj->getStudy());
    compareObjectPropertyValue(obj, "@reconstruction_db.0", recDB[0]);
    compareObjectPropertyValue(obj, "@reconstruction_db.1", recDB[1]);
}

//------------------------------------------------------------------------------

void SeriesPropertiesTest::patientPropertiesTest()
{
    const std::string name = "Patient Name";
    const std::string id = "42";
    const std::string birth_date = "19830214";
    const std::string sex = "M";
    const PropertiesNameType dataProperties = list_of("fields")
                                                     ("name")
                                                     ("id")
                                                     ("birth_date")
                                                     ("sex");

    ::fwMedData::Patient::sptr obj = ::fwMedData::Patient::New();
    obj->setName(name);
    obj->setId(id);
    obj->setBirthdate(birth_date);
    obj->setSex(sex);

    visitProperties(obj->getClassname(), dataProperties);
    compareSimplePropertyValue(obj, "@name", name);
    compareSimplePropertyValue(obj, "@id", id);
    compareSimplePropertyValue(obj, "@birth_date", birth_date);
    compareSimplePropertyValue(obj, "@sex", sex);
}

//------------------------------------------------------------------------------

void SeriesPropertiesTest::studyPropertiesTest()
{
    const std::string instance_uid = "123456789";
    const std::string date = "20130214";
    const std::string time = "143328";
    const std::string referring_physician_name = "John Doe";
    const std::string description = "description";
    const std::string patient_age = "42";
    const PropertiesNameType dataProperties = list_of("fields")
                                                     ("instance_uid")
                                                     ("date")
                                                     ("time")
                                                     ("referring_physician_name")
                                                     ("description")
                                                     ("patient_age");

    ::fwMedData::Study::sptr obj = ::fwMedData::Study::New();
    obj->setInstanceUID(instance_uid);
    obj->setDate(date);
    obj->setTime(time);
    obj->setReferringPhysicianName(referring_physician_name);
    obj->setDescription(description);
    obj->setPatientAge(patient_age);

    visitProperties(obj->getClassname(), dataProperties);
    compareSimplePropertyValue(obj, "@instance_uid", instance_uid);
    compareSimplePropertyValue(obj, "@date", date);
    compareSimplePropertyValue(obj, "@time", time);
    compareSimplePropertyValue(obj, "@referring_physician_name", referring_physician_name);
    compareSimplePropertyValue(obj, "@description", description);
    compareSimplePropertyValue(obj, "@patient_age", patient_age);
}

//------------------------------------------------------------------------------


} //namespace ut
} //namespace fwMedDataCamp
