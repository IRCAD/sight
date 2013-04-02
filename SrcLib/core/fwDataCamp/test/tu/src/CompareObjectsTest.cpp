/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/shared_ptr.hpp>

#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Study.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Composite.hpp>


#include <fwDataTools/Patient.hpp>
#include <fwDataTools/Image.hpp>

#include <fwDataCamp/visitor/CompareObjects.hpp>

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
    m_patientRef = ::fwData::Patient::New();
    ::fwDataTools::Patient::generatePatient(m_patientRef, 1, 1, 1);

    m_patientComp = ::fwData::Patient::New();
    m_patientComp->deepCopy(m_patientRef); 

    // HACK : Acquisition::netID not managed in deep copy
    ::fwData::Acquisition::sptr acq = m_patientComp->getStudies()[0]->getAcquisitions()[0];
    acq->setNetID(m_patientRef->getStudies()[0]->getAcquisitions()[0]->getNetID());
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

}  // namespace ut
}  // namespace fwDataCamp
