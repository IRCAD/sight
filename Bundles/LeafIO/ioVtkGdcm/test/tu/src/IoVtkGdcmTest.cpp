/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwData/PatientDB.hpp>

#include <fwTest/Data.hpp>

#include "IoVtkGdcmTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( IoVtkGdcmTest );

//------------------------------------------------------------------------------

void IoVtkGdcmTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void IoVtkGdcmTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void IoVtkGdcmTest::testReader()
{

    ::boost::filesystem::path dicomDataPath = ::fwTest::Data::dir() / "fw4spl/Patient/Dicom/image_281433";

    ::fwData::PatientDB::NewSptr patientDB;
    ::fwRuntime::EConfigurationElement::NewSptr readerCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr configCfg("config");

    ::fwRuntime::EConfigurationElement::NewSptr ioVtkGdcmPathCfg("path");
    ioVtkGdcmPathCfg->setAttributeValue("value", dicomDataPath.string());
    configCfg->addConfigurationElement(ioVtkGdcmPathCfg);
    readerCfg->addConfigurationElement(configCfg);

    ::fwServices::IService::sptr srv = ::fwServices::registry::ServiceFactory::getDefault()->create( "::io::IReader", "::ioVtkGdcm::DicomPatientDBReaderService" );
    CPPUNIT_ASSERT(srv);

    ::fwServices::OSR::registerService( patientDB , srv );

    srv->setConfiguration( readerCfg );
    srv->configure();
    srv->start();
    srv->update();

    // Patient expected
    std::string nameExpected("anonymous");
    std::string firstnameExpected("anonymous");
    bool isMaleExpected = false;
    ::boost::uint32_t nbPatientExpected = 1;
    ::boost::uint32_t nbStudyExpected = 1;
    ::boost::uint32_t nbSeriesExpected = 1;

    // Patient read.
    ::boost::uint32_t  nbPatient = patientDB->getPatientSize();
    ::fwData::PatientDB::PatientIterator patientIter;
    patientIter = patientDB->getPatients().first;
    ::fwData::Patient::StudyIterator studyIter;
    studyIter = (*patientIter)->getStudies().first;

    CPPUNIT_ASSERT_EQUAL(nbPatient, nbPatientExpected);
    CPPUNIT_ASSERT_EQUAL((*patientIter)->getName(), nameExpected);
    CPPUNIT_ASSERT_EQUAL((*patientIter)->getFirstname(), firstnameExpected);
    CPPUNIT_ASSERT_EQUAL((*patientIter)->getIsMale(), isMaleExpected);
    CPPUNIT_ASSERT_EQUAL((*patientIter)->getStudySize(), nbStudyExpected);
    CPPUNIT_ASSERT_EQUAL((*studyIter)->getAcquisitionSize(), nbSeriesExpected);

    srv->stop();
    ::fwServices::OSR::unregisterService( srv );

}

//------------------------------------------------------------------------------
