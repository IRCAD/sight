/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/shared_ptr.hpp>

#include <fwCore/Exception.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Float.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Image.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Study.hpp>

#include <fwDataTools/Patient.hpp>
#include <fwDataTools/Image.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwTools/Type.hpp>

#include "CompareObjectsTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwDataCamp::ut::CompareObjectsTest );

namespace fwDataCamp
{
namespace ut
{

//-----------------------------------------------------------------------------

// Set up context before running a test.
void CompareObjectsTest::setUp()
{
    ::fwTools::Type type = ::fwTools::Type::create< float >();
    ::fwData::Image::sptr image = ::fwData::Image::New();
    ::fwDataTools::Image::generateRandomImage(image, type);

    m_patientRef = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(m_patientRef, 1, 1, 1);
    m_patientRef->getStudies()[0]->getAcquisitions()[0]->setImage(image);

    m_patientComp = ::fwData::Object::copy(m_patientRef);
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::comparePatientTest()
{
    {
        visitor::CompareObjects visitor;
        visitor.compare(m_patientRef, m_patientComp);

        SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)0);
    }

    {
        const std::string name = m_patientComp->getName() + "X";
        const std::string firstname = m_patientComp->getFirstname() + "X";

        m_patientComp->setName(name);
        m_patientComp->setFirstname(firstname);
        m_patientComp->setIsMale(!m_patientComp->getIsMale());

        visitor::CompareObjects visitor;
        visitor.compare(m_patientRef, m_patientComp);

        SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)3);

        CPPUNIT_ASSERT(props->find("name") != props->end());
        CPPUNIT_ASSERT_EQUAL((*props)["name"], name);

        CPPUNIT_ASSERT(props->find("firstname") != props->end());
        CPPUNIT_ASSERT_EQUAL((*props)["firstname"], firstname);

        CPPUNIT_ASSERT(props->find("is_male") != props->end());
        CPPUNIT_ASSERT_EQUAL((*props)["is_male"], std::string(m_patientComp->getIsMale() ? "true" : "false"));
    }
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareStudyTest()
{
    ::fwData::Study::sptr study = m_patientComp->getStudies()[0];
    study->setAcquisitionZone("Thorax");
    study->setHospital("Unknown");

    visitor::CompareObjects visitor;
    visitor.compare(m_patientRef, m_patientComp);

    SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)2);

    CPPUNIT_ASSERT(props->find("studies.0.acquisition_zone") != props->end());
    CPPUNIT_ASSERT_EQUAL((*props)["studies.0.acquisition_zone"], std::string("Thorax"));

    CPPUNIT_ASSERT(props->find("studies.0.hospital") != props->end());
    CPPUNIT_ASSERT_EQUAL((*props)["studies.0.hospital"], std::string("Unknown"));
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareImageTest()
{
    ::fwData::Image::sptr img = m_patientComp->getStudies()[0]->getAcquisitions()[0]->getImage();

    ::fwData::Image::SpacingType spacing = img->getSpacing();
    spacing[0] = 42;
    img->setSpacing(spacing);

    ::fwData::Image::OriginType origin = img->getOrigin();
    origin[2] = 1664;
    img->setOrigin(origin);

    visitor::CompareObjects visitor;
    visitor.compare(m_patientRef, m_patientComp);

    SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)2);

    CPPUNIT_ASSERT(props->find("studies.0.acquisitions.0.image.spacing.0") != props->end());
    CPPUNIT_ASSERT_EQUAL((*props)["studies.0.acquisitions.0.image.spacing.0"], std::string("42"));

    CPPUNIT_ASSERT(props->find("studies.0.acquisitions.0.image.origin.2") != props->end());
    CPPUNIT_ASSERT_EQUAL((*props)["studies.0.acquisitions.0.image.origin.2"], std::string("1664"));
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareReconstructionTest()
{
    ::fwData::Reconstruction::sptr rec =
        m_patientComp->getStudies()[0]->getAcquisitions()[0]->getReconstructions()[0];

    rec->setOrganName("Unknown organ name");
    rec->setIsAutomatic(!rec->getIsAutomatic());
    rec->setIsVisible(!rec->getIsVisible());

    visitor::CompareObjects visitor;
    visitor.compare(m_patientRef, m_patientComp);

    SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
    CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)3);

    CPPUNIT_ASSERT(props->find("studies.0.acquisitions.0.reconstructions.0.organ_name") != props->end());
    CPPUNIT_ASSERT_EQUAL((*props)["studies.0.acquisitions.0.reconstructions.0.organ_name"],
            std::string("Unknown organ name"));

    CPPUNIT_ASSERT(props->find("studies.0.acquisitions.0.reconstructions.0.is_automatic") != props->end());
    CPPUNIT_ASSERT_EQUAL((*props)["studies.0.acquisitions.0.reconstructions.0.is_automatic"],
            std::string(rec->getIsAutomatic() ? "true" : "false"));

    CPPUNIT_ASSERT(props->find("studies.0.acquisitions.0.reconstructions.0.is_visible") != props->end());
    CPPUNIT_ASSERT_EQUAL((*props)["studies.0.acquisitions.0.reconstructions.0.is_visible"],
            std::string(rec->getIsVisible() ? "true" : "false"));
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareBufferTest()
{
    ::fwData::Image::sptr imgRef = ::fwData::Image::New();
    ::fwData::Image::sptr imgComp = ::fwData::Image::New();

    ::fwTools::Type typeRef = ::fwTools::Type::create< float >();
    ::fwTools::Type typeComp = ::fwTools::Type::create< double >();

    {
        ::fwDataTools::Image::generateRandomImage(imgRef, typeRef);
        ::fwDataTools::Image::generateRandomImage(imgComp, typeComp);

        visitor::CompareObjects visitor;
        visitor.compare(imgRef, imgComp);

        SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT(!props->empty());
        CPPUNIT_ASSERT(props->find("array.buffer") != props->end());
    }

    {
        imgComp = ::fwData::Object::copy(imgRef);
        visitor::CompareObjects visitor;
        visitor.compare(imgRef, imgComp);

        SPTR(visitor::CompareObjects::PropsMapType) props = visitor.getDifferences();
        CPPUNIT_ASSERT_EQUAL(props->size(), (size_t)0);
    }
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::compareEmpty()
{
    ::fwData::Composite::sptr compRef = ::fwData::Composite::New();
    ::fwData::Composite::sptr compComp = ::fwData::Composite::New();

    (*compRef)["float"] = ::fwData::Float::New(0.0f);
    (*compComp)["int"] = ::fwData::Integer::New(0);

    visitor::CompareObjects visitor;
    visitor.compare(compComp, compRef);

    SPTR(visitor::CompareObjects::PropsMapType) diffs = visitor.getDifferences();
    CPPUNIT_ASSERT_EQUAL(diffs->size(), (size_t)2);
    CPPUNIT_ASSERT(diffs->find("values.float.value") != diffs->end());
    CPPUNIT_ASSERT(diffs->find("values.int.value") != diffs->end());
    CPPUNIT_ASSERT_EQUAL((*diffs)["values.float.value"], visitor::CompareObjects::s_MISSING_PROPERTY);
    CPPUNIT_ASSERT_EQUAL((*diffs)["values.int.value"], visitor::CompareObjects::s_MISSING_PROPERTY);
}

//-----------------------------------------------------------------------------

void CompareObjectsTest::exceptionTest()
{
    ::fwData::Image::sptr img = ::fwData::Image::New();
    ::fwData::Study::sptr study = ::fwData::Study::New();

    visitor::CompareObjects visitor;
    CPPUNIT_ASSERT_THROW(visitor.compare(img, study), ::fwCore::Exception);

    CPPUNIT_ASSERT_EQUAL(visitor.getReferenceProps().size(), (size_t)0);
    CPPUNIT_ASSERT_EQUAL(visitor.getComparedProps().size(), (size_t)0);
    CPPUNIT_ASSERT_EQUAL(visitor.getDifferences()->size(), (size_t)0);
}

//-----------------------------------------------------------------------------

}  // namespace ut
}  // namespace fwDataCamp

