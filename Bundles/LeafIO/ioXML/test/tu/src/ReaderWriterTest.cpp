/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fstream>
#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/System.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/AppConfigManager.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <fwData/Image.hpp>
#include <fwData/PatientDB.hpp>

#include <fwDataTools/Image.hpp>
#include <fwDataTools/Patient.hpp>

#include <fwTest/Data.hpp>

#include <fwXML/writer/fwxmlextension.hpp>

#include "ReaderWriterTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ReaderWriterTest );


::fwRuntime::EConfigurationElement::sptr getIOConfiguration(const ::boost::filesystem::path &file)
{
    ::fwRuntime::EConfigurationElement::NewSptr readerSrvCfg("service");
    ::fwRuntime::EConfigurationElement::NewSptr readerCfg("file");
    readerCfg->setValue(file.string());
    readerSrvCfg->addConfigurationElement(readerCfg);
    return readerSrvCfg;
}

void runImageSrv(const std::string &srvtype, const std::string &srvname, const ::fwRuntime::EConfigurationElement::sptr cfg, ::fwData::Object::sptr object)
{

    ::fwServices::IService::sptr srv;
    srv = ::fwServices::registry::ServiceFactory::getDefault()->create( srvtype, srvname );

    CPPUNIT_ASSERT_MESSAGE(srvname,srv);

    ::fwServices::OSR::registerService( object , srv );
    srv->setConfiguration( cfg );
    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    ::fwServices::OSR::unregisterService( srv );
}

//------------------------------------------------------------------------------

void ReaderWriterTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ReaderWriterTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ReaderWriterTest::testReader()
{

#define READ_FILE(filename, objtype, reader)                                                       \
    {                                                                                              \
        OSLM_WARN("testing : <" << filename << "> (" << #objtype << ") with <" << reader << ">" ); \
        const ::boost::filesystem::path file = ::fwTest::Data::dir() / filename;                   \
        ::fwData::objtype::sptr object = ::fwData::objtype::New();                                 \
        runImageSrv("::io::IReader", reader, getIOConfiguration(file), object);                    \
    }                                                                                              \

    READ_FILE( "fw4spl/PatientDB/yaf/generated-patientdb.yaf"    , PatientDB, "::ioXML::FwXMLGenericReaderService");
    READ_FILE( "fw4spl/PatientDB/yaf/generated-patientdb.yaf"    , PatientDB, "::ioXML::FwXMLPatientDBReaderService");

    READ_FILE( "fw4spl/Patient/yaf/generated-patient.yaf"        , Patient  , "::ioXML::FwXMLGenericReaderService");

    READ_FILE( "fw4spl/aquisition/yaf/generated-acquisition.yaf" , Acquisition, "::ioXML::FwXMLGenericReaderService");

    // READ_FILE( "fw4spl/image/yaf/image-double.yaf" , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-float.yaf"  , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-int16.yaf"  , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-int32.yaf"  , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-int64.yaf"  , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-int8.yaf"   , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-uint16.yaf" , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-uint32.yaf" , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-uint64.yaf" , Image, "::ioXML::FwXMLGenericReaderService");
    // READ_FILE( "fw4spl/image/yaf/image-uint8.yaf"  , Image, "::ioXML::FwXMLGenericReaderService");


}

//------------------------------------------------------------------------------

void ReaderWriterTest::testWriter()
{


#define testAcquisitionMacro(fname)                                                                          \
    {                                                                                                        \
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder() / fname ;                 \
    ::fwData::Acquisition::sptr object = ::fwData::Acquisition::New();                                       \
    ::fwDataTools::Patient::generateAcquisition(object, 10);                                                 \
    runImageSrv("::io::IWriter", "::ioXML::FwXMLGenericWriterService",getIOConfiguration(file), object);     \
    runImageSrv("::io::IReader", "::ioXML::FwXMLGenericReaderService", getIOConfiguration(file), object);    \
    ::boost::filesystem::remove(file);                                                                       \
    }



#define testImageMacro(T, fname)                                                                          \
    {                                                                                                     \
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder() / fname ;              \
    ::fwData::Image::sptr object = ::fwData::Image::New();                                                \
    ::fwDataTools::Image::generateRandomImage(object, ::fwTools::Type(T) );                               \
    runImageSrv("::io::IWriter", "::ioXML::FwXMLGenericWriterService",getIOConfiguration(file), object);  \
    runImageSrv("::io::IReader", "::ioXML::FwXMLGenericReaderService", getIOConfiguration(file), object); \
    ::boost::filesystem::remove(file);                                                                    \
    }


#define testPatientMacro(fname)                                                                                 \
    {                                                                                                           \
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder() / fname ;                    \
    ::fwData::Patient::sptr object = ::fwData::Patient::New();                                                  \
    ::fwDataTools::Patient::generatePatient(object, 3,2,5);                                                     \
    runImageSrv("::io::IWriter", "::ioXML::FwXMLGenericWriterService",getIOConfiguration(file), object);        \
    runImageSrv("::io::IWriter", "::ioXML::FwXMLPatient2PatientDBWriterService",getIOConfiguration(file), object); \
    ::boost::filesystem::remove(file);                                                                          \
    }


    // runImageSrv("::io::IReader", "::ioXML::FwXMLGenericReaderService", getIOConfiguration(file), object);       \
    // runImageSrv("::io::IReader", "::ioXML::FwXMLGenericReaderService", getIOConfiguration(file), pdb);          \
    // runImageSrv("::io::IReader", "::ioXML::FwXMLPatientDBReaderService", getIOConfiguration(file), pdb);        \


#define testPatientDBMacro(fname)                                                                        \
    {                                                                                                    \
    const ::boost::filesystem::path file = ::fwTools::System::getTemporaryFolder() /  fname ;            \
    ::fwData::PatientDB::sptr pdb = ::fwData::PatientDB::New();                                          \
    ::fwData::Patient::sptr object;                                                                      \
    for (int i = 0; i<3 ; ++i)                                                                           \
    {                                                                                                    \
        object = ::fwData::Patient::New();                                                               \
        ::fwDataTools::Patient::generatePatient(object, 3,2,5);                                          \
        pdb->addPatient(object);                                                                         \
    }                                                                                                    \
    runImageSrv("::io::IWriter", "::ioXML::FwXMLGenericWriterService",  getIOConfiguration(file), pdb);  \
    runImageSrv("::io::IWriter", "::ioXML::FwXMLPatientDBWriterService",getIOConfiguration(file), pdb);  \
    ::boost::filesystem::remove(file);                                                                   \
    }


    // runImageSrv("::io::IReader", "::ioXML::FwXMLGenericReaderService", getIOConfiguration(file), pdb);   \
    // runImageSrv("::io::IReader", "::ioXML::FwXMLPatientDBReaderService", getIOConfiguration(file), pdb); \


    testPatientMacro  ("generated-patient.yaf");
    testPatientDBMacro("generated-patientdb.yaf");
    testAcquisitionMacro("generated-acquisition.yaf");

    testImageMacro("generated-image.yaf", "double" );
    testImageMacro("generated-image.yaf", "float" );
    testImageMacro("generated-image.yaf", "int16" );
    testImageMacro("generated-image.yaf", "int32" );
    testImageMacro("generated-image.yaf", "int64" );
    testImageMacro("generated-image.yaf", "int8" );
    testImageMacro("generated-image.yaf", "uint16" );
    testImageMacro("generated-image.yaf", "uint32" );
    testImageMacro("generated-image.yaf", "uint64" );
    testImageMacro("generated-image.yaf", "uint8" );
}

//------------------------------------------------------------------------------
