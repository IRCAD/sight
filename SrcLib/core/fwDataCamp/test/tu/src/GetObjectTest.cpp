/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Image.hpp>
#include <fwData/Float.hpp>
#include <fwData/String.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Study.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Composite.hpp>

// #include <fwMedData/ImageSeries.hpp>

#include <fwDataTools/Patient.hpp>
#include <fwDataTools/Image.hpp>

#include <fwDataCamp/getObject.hpp>
#include <fwDataCamp/exception/NullPointer.hpp>
#include <fwDataCamp/exception/ObjectNotFound.hpp>

#include "GetObjectTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataCamp::ut::GetObjectTest );

namespace fwDataCamp
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void GetObjectTest::setUp()
{};

//-----------------------------------------------------------------------------

void GetObjectTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void GetObjectTest::getTest()
{
    // Visit 1
    ::fwData::Image::sptr img1 = ::fwData::Image::New();
    ::fwDataTools::Image::generateRandomImage(img1, ::fwTools::Type::create("int16"));
    ::fwData::Image::sptr img2 = ::fwData::Image::New();
    ::fwDataTools::Image::generateRandomImage(img2, ::fwTools::Type::create("uint8"));
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    composite->getContainer()["img1"] = img1;
    composite->getContainer()["img2"] = img2;
    ::fwData::Object::sptr subObj1 = ::fwDataCamp::getObject( composite, "@values.img2" );
    CPPUNIT_ASSERT_MESSAGE("Image must be equal" , subObj1 == img2);

    // Visit 2
    ::fwData::Float::sptr zspacing = ::fwDataCamp::getObject< ::fwData::Float >( composite, "@values.img2.spacing.2" );
    CPPUNIT_ASSERT_MESSAGE("spacing must be equal" , img2->getSpacing()[2] - 0.001 < zspacing->value() && zspacing->value() < img2->getSpacing()[2] + 0.001 );

    // Visit 3
    ::fwData::Patient::sptr patient1 = ::fwData::Patient::New();
    patient1->setFirstname( "toto" );
    ::fwData::String::sptr str= ::fwDataCamp::getObject< ::fwData::String >( patient1, "@firstname" );
    CPPUNIT_ASSERT_MESSAGE("Firstname must be equal" , patient1->getFirstname() == str->value() );

    // Visit 4
    ::fwData::Patient::sptr patient2 = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(patient2, 2, 1, 2);
    ::fwData::Reconstruction::sptr rec = ::fwDataCamp::getObject< ::fwData::Reconstruction >( patient2, "@studies.1.acquisitions.0.reconstructions.1" );
    ::fwData::Study::sptr study = patient2->getStudies()[1];
    ::fwData::Acquisition::sptr acq = study->getAcquisitions()[0];
    ::fwData::Reconstruction::sptr rec2 = acq->getReconstructions()[1];
    CPPUNIT_ASSERT_MESSAGE("Reconstruction must be equal" , rec ==  rec2 );

    // Visit 5
    composite->setField("toto", img1);
    img1->setField("titi", img2);
    ::fwData::Object::sptr subObj2 = ::fwDataCamp::getObject( composite, "@fields.toto.fields.titi" );
    CPPUNIT_ASSERT_MESSAGE("Image must be equal", subObj2 == img2 );
}

//-----------------------------------------------------------------------------

void GetObjectTest::invalidPathTest()
{
    ::fwData::Composite::sptr composite = ::fwData::Composite::New();
    ::fwData::String::sptr text = ::fwData::String::New("Text");
    (*composite)["string"] = text;

    ::fwData::Object::sptr obj = ::fwDataCamp::getObject( composite, "@values.string" );
    CPPUNIT_ASSERT_MESSAGE("fwData::String must be equal" , obj ==  text );

    // no exception version
    ::fwData::Object::sptr invalidObj = ::fwDataCamp::getObject( composite, "@values.invalidPath", false );
    CPPUNIT_ASSERT_MESSAGE("Object must not exist", !invalidObj );

    // exception version : path is invalid
    CPPUNIT_ASSERT_THROW(
            ::fwDataCamp::getObject( composite, "@values.invalidPath", true ),
             ::fwDataCamp::exception::ObjectNotFound
    );
    CPPUNIT_ASSERT_EQUAL(size_t(1), composite->size() );


    // ::fwMedData::ImageSeries::sptr imgSeries = ::fwMedData::ImageSeries::New();
    // // no exception version
    // invalidObj = ::fwDataCamp::getObject( imgSeries, "@image.type", false );
    // CPPUNIT_ASSERT_MESSAGE("Object must not exist", !invalidObj );

    // // exception version : path exist, but image object is null
    // CPPUNIT_ASSERT_THROW(
    //         ::fwDataCamp::getObject( imgSeries, "@image.type", true ),
    //          ::fwDataCamp::exception::NullPointer
    // );
}

//-----------------------------------------------------------------------------

}  // namespace ut
}  // namespace fwDataCamp
