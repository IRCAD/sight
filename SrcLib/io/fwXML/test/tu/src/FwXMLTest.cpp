/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwXML/Serializer.hpp>
#include <fwXML/policy/NeverSplitPolicy.hpp>
#include <fwXML/policy/UniquePathPolicy.hpp>

#include "FwXMLTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( FwXMLTest );

//------------------------------------------------------------------------------

void FwXMLTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void FwXMLTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void FwXMLTest::testFwXML()
{
    const ::boost::filesystem::path PATH = "fwxmlTest/patient.xml";

    // create patientDB
    ::fwData::PatientDB::sptr pPatientDB = createPatientDB();

    // save patientDB in fwXML
    ::boost::filesystem::create_directories( PATH.parent_path() );

    fwXML::Serializer serializer;
    ::boost::shared_ptr< ::fwXML::NeverSplitPolicy > spolicy ( new ::fwXML::NeverSplitPolicy() );
    serializer.setSplitPolicy( spolicy );

#if BOOST_FILESYSTEM_VERSION > 2
    ::boost::shared_ptr< ::fwXML::UniquePathPolicy > pPathPolicy ( new ::fwXML::UniquePathPolicy( PATH.filename().string() ) );
#else
    ::boost::shared_ptr< ::fwXML::UniquePathPolicy > pPathPolicy ( new ::fwXML::UniquePathPolicy( PATH.leaf() ) );
#endif
    serializer.setPathPolicy( pPathPolicy );

    serializer.rootFolder() = PATH.parent_path().string();
    bool doSaveSchema = false;
    serializer.serialize(pPatientDB, doSaveSchema);


    // load patientDB
    ::fwData::PatientDB::sptr pPatientDB2 = ::fwData::PatientDB::dynamicCast(serializer.deSerialize(PATH));

    // check patientDB
    CPPUNIT_ASSERT(pPatientDB2);
    CPPUNIT_ASSERT_EQUAL(pPatientDB->getPatientSize(), pPatientDB2->getPatientSize());

    ::fwData::Patient::sptr pPatient = *pPatientDB->getPatients().first;
    ::fwData::Patient::sptr pPatient2 = *pPatientDB2->getPatients().first;
    CPPUNIT_ASSERT_EQUAL(pPatient->getName(), pPatient2->getName());
    CPPUNIT_ASSERT_EQUAL(pPatient->getFirstname(), pPatient2->getFirstname());
    CPPUNIT_ASSERT_EQUAL(pPatient->getBirthdate(), pPatient2->getBirthdate());
    CPPUNIT_ASSERT_EQUAL(pPatient->getIDDicom(), pPatient2->getIDDicom());
    CPPUNIT_ASSERT_EQUAL(pPatient->getIsMale(), pPatient2->getIsMale());
    CPPUNIT_ASSERT_EQUAL(pPatient->getDbID(), pPatient2->getDbID());

    ::fwData::Study::sptr pStudy = *pPatient->getStudies().first;
    ::fwData::Study::sptr pStudy2 = *pPatient2->getStudies().first;
    CPPUNIT_ASSERT_EQUAL(pStudy->getHospital(), pStudy2->getHospital());
    CPPUNIT_ASSERT_EQUAL(pStudy->getModality(), pStudy2->getModality());
    CPPUNIT_ASSERT_EQUAL(pStudy->getAcquisitionZone(), pStudy2->getAcquisitionZone());
    CPPUNIT_ASSERT_EQUAL(pStudy->getRefRISId(), pStudy2->getRefRISId());
    CPPUNIT_ASSERT_EQUAL(pStudy->getUID(), pStudy2->getUID());
    CPPUNIT_ASSERT_EQUAL(pStudy->getDbID(), pStudy2->getDbID());
    CPPUNIT_ASSERT_EQUAL(pStudy->getAcquisitionSize(), pStudy2->getAcquisitionSize());

    ::fwData::Acquisition::sptr pAcq = *pStudy->getAcquisitions().first;
    ::fwData::Acquisition::sptr pAcq2 = *pStudy2->getAcquisitions().first;
    CPPUNIT_ASSERT_EQUAL(pAcq->getBitsPerPixel(), pAcq2->getBitsPerPixel());
    CPPUNIT_ASSERT_EQUAL(pAcq->getSliceThickness(), pAcq2->getSliceThickness());
    CPPUNIT_ASSERT_EQUAL(pAcq->getAxe(), pAcq2->getAxe());
    CPPUNIT_ASSERT_EQUAL(pAcq->getUnsignedFlag(), pAcq2->getUnsignedFlag());
    CPPUNIT_ASSERT_EQUAL(pAcq->getAcquisitionIndex(), pAcq2->getAcquisitionIndex());
    CPPUNIT_ASSERT_EQUAL(pAcq->getImageType(), pAcq2->getImageType());
    CPPUNIT_ASSERT_EQUAL(pAcq->getImageFormat(), pAcq2->getImageFormat());
    CPPUNIT_ASSERT_EQUAL(pAcq->getIsMain(), pAcq2->getIsMain());
    CPPUNIT_ASSERT_EQUAL(pAcq->getIsNormalDir(), pAcq2->getIsNormalDir());
    CPPUNIT_ASSERT_EQUAL(pAcq->getUID(), pAcq2->getUID());
    CPPUNIT_ASSERT_EQUAL(pAcq->getDbID(), pAcq2->getDbID());
    CPPUNIT_ASSERT_EQUAL(pAcq->getLaboID(), pAcq2->getLaboID());
    CPPUNIT_ASSERT_EQUAL(pAcq->getNetID(), pAcq2->getNetID());
    CPPUNIT_ASSERT_EQUAL(pAcq->getDateSendToLaboAt(), pAcq2->getDateSendToLaboAt());
    CPPUNIT_ASSERT_EQUAL(pAcq->getDateReceiveFromLaboAt(), pAcq2->getDateReceiveFromLaboAt());
    CPPUNIT_ASSERT_EQUAL(pAcq->getDateSendToBDDAt(), pAcq2->getDateSendToBDDAt());
    CPPUNIT_ASSERT_EQUAL(pAcq->getDateDisponibilityAt(), pAcq2->getDateDisponibilityAt());
    CPPUNIT_ASSERT_EQUAL(pAcq->getPatientSize(), pAcq2->getPatientSize());
    CPPUNIT_ASSERT_EQUAL(pAcq->getPatientWeight(), pAcq2->getPatientWeight());
    CPPUNIT_ASSERT_EQUAL(pAcq->getRadiations(), pAcq2->getRadiations());
    CPPUNIT_ASSERT_EQUAL(pAcq->getMedicalPrinter(), pAcq2->getMedicalPrinter());
    CPPUNIT_ASSERT_EQUAL(pAcq->getMedicalPrinterCorp(), pAcq2->getMedicalPrinterCorp());
    CPPUNIT_ASSERT_EQUAL(pAcq->getPatientPosition(), pAcq2->getPatientPosition());

    ::fwData::Image::sptr image = pAcq->getImage();
    ::fwData::Image::sptr image2 = pAcq2->getImage();
    CPPUNIT_ASSERT_EQUAL(image->getDimension(), image2->getDimension());
    CPPUNIT_ASSERT_EQUAL(image->getManagesBuff(), image2->getManagesBuff());
    CPPUNIT_ASSERT_EQUAL(image->getSpacing().back(), image2->getSpacing().back());
    CPPUNIT_ASSERT_EQUAL(image->getOrigin().back(), image2->getOrigin().back());
    CPPUNIT_ASSERT_EQUAL(image->getSize().back(), image2->getSize().back());
    CPPUNIT_ASSERT_EQUAL(image->getFilename(), image2->getFilename());
    CPPUNIT_ASSERT_EQUAL(image->getWindowCenter(), image2->getWindowCenter());
    CPPUNIT_ASSERT_EQUAL(image->getWindowWidth(), image2->getWindowWidth());
    CPPUNIT_ASSERT_EQUAL(image->getRescaleIntercept(), image2->getRescaleIntercept());

    ::boost::filesystem::remove_all( PATH.parent_path().string() );
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr FwXMLTest::createPatientDB()
{
    // patient informations
    const std::string PATIENT_NAME          = "NomPatient1" ;
    const std::string PATIENT_FIRSTNAME     = "PrenomPatient1" ;
    const std::string PATIENT_IDDICOM       = "IDDICOM" ;
    const std::string PATIENT_BIRTHDATE     = "2000-Feb-21 15:00:00";
    const bool PATIENT_SEX                  = true;
    const ::boost::int32_t  PATIENT_DBID    = 457;

    // studies informations
    const std::string STUDY1_HOSPITAL       = "hopital" ;
    const std::string STUDY1_MODALITY       = "modality" ;
    const std::string STUDY1_ZONE           = "IDDICOM" ;
    const std::string STUDY1_RISID          = "risid569" ;
    const std::string STUDY1_UID            = "UID569" ;
    const ::boost::int32_t  STUDY1_DBID     = 156 ;

    const std::string STUDY2_HOSPITAL       = "hopital2" ;
    const std::string STUDY2_MODALITY       = "modality2" ;
    const std::string STUDY2_ZONE           = "IDDICOM2" ;
    const std::string STUDY2_RISID          = "risid321" ;
    const std::string STUDY2_UID            = "UID321" ;
    const ::boost::int32_t  STUDY2_DBID     = 153 ;

    // aquisitions informations
    const ::boost::uint8_t  ACQ1_BITSPERPIXEL   = 48 ;
    const float ACQ1_SLICETHICKNESS             = 0.8f ;
    const ::boost::uint8_t  ACQ1_AXE            = 12 ;
    const bool ACQ1_UNSIGNEDFLAG                = true ;
    const ::boost::uint32_t ACQ1_INDEX          = 5345 ;
    const std::string ACQ1_IMAGETYPE            = "refimageType" ;
    const std::string ACQ1_IMAGEFORMAT          = "refimageFormat" ;
    const std::string ACQ1_CREATIONDATE         = "2007-Feb-26 15:00:00";
    const bool ACQ1_ISMAIN                      = true ;
    const bool ACQ1_ISNORMALDIR                 = true ;
    const std::string ACQ1_UID                  = "uid324" ;
    const ::boost::int32_t ACQ1_DBID            = 6354 ;
    const ::boost::uint32_t ACQ1_LABOID         = 357 ;
    const ::boost::uint32_t ACQ1_NETID          = 21 ;
    const std::string ACQ1_DATESENDTOLABOAT     = "2007-Feb-26 10:05:00";
    const std::string ACQ1_DATERECEIVEFROMLABOAT= "2007-Feb-27 10:05:00";
    const std::string ACQ1_DATESENDTOBDDAT      = "2007-Feb-27 10:05:00";
    const std::string ACQ1_DATEDISPONIBILITYAT  = "2007-Feb-28 10:05:00";
    const double ACQ1_PATIENTSIZE               = 45.1352 ;
    const double ACQ1_PATIENTWEIGHT             = 89.6545 ;
    const double ACQ1_RADIATIONS                = 89.6545 ;
    const std::string ACQ1_MEDICALPRINTER       = "MedicalPrinter" ;
    const std::string ACQ1_MEDICALPRINTERCORP   = "MedicalPrinterCorp" ;
    const std::string ACQ1_PATIENTPOSITION      = "PatientPosition" ;

    // images informations
    const ::boost::uint8_t IMG1_DIMENSION   = 2 ;
    const bool IMG1_MANAGESBUFF             = true ;
    double IMG1_CREFSPACING                 = 2.5 ;
    std::vector<double> IMG1_VECTORCREFSPACING ;
    IMG1_VECTORCREFSPACING.push_back( IMG1_CREFSPACING ) ;
    double IMG1_CREFORIGIN                  = 3 ;
    std::vector<double> IMG1_VECTORCREFORIGIN    ;
    IMG1_VECTORCREFORIGIN.push_back( IMG1_CREFORIGIN ) ;
    ::boost::int32_t IMG1_CREFSIZE = 42 ;
    std::vector< ::boost::int32_t > IMG1_VECTORCREFSIZE  ;
    IMG1_VECTORCREFSIZE.push_back( IMG1_CREFSIZE ) ;
    const ::boost::filesystem::path IMG1_CREFFILENAME   = ::boost::filesystem::path("D:/TMP") ;
    const double IMG1_WINDOWCENTER = 86 ;
    const double IMG1_WINDOWWIDTH = 345 ;
    const double IMG1_RESCALEINTERCEPT = 1 ;

    const ::boost::uint8_t IMG2_DIMENSION   = 3 ;
    const bool IMG2_MANAGESBUFF             = false ;
    double IMG2_CREFSPACING                 = 3.1 ;
    std::vector<double> IMG2_VECTORCREFSPACING ;
    IMG2_VECTORCREFSPACING.push_back( IMG2_CREFSPACING ) ;
    double IMG2_CREFORIGIN                  = 1.0 ;
    std::vector<double> IMG2_VECTORCREFORIGIN    ;
    IMG2_VECTORCREFORIGIN.push_back( IMG2_CREFORIGIN ) ;
    ::boost::int32_t IMG2_CREFSIZE = 42 ;
    std::vector< ::boost::int32_t > IMG2_VECTORCREFSIZE  ;
    IMG2_VECTORCREFSIZE.push_back( IMG2_CREFSIZE ) ;
    const ::boost::filesystem::path IMG2_CREFFILENAME   = ::boost::filesystem::path("D:/TMP") ;
    const double IMG2_WINDOWCENTER = 75 ;
    const double IMG2_WINDOWWIDTH = 500 ;
    const double IMG2_RESCALEINTERCEPT = 2 ;

    ::boost::int32_t IMG3_CREFSIZE = 450 ;
    std::vector< ::boost::int32_t > IMG3_VECTORCREFSIZE  ;
    IMG3_VECTORCREFSIZE.push_back( IMG3_CREFSIZE ) ;

    //create patientDB
    ::fwData::PatientDB::NewSptr pPatientDB;
    ::fwData::Patient::NewSptr pPatient;
    ::fwData::Patient::NewSptr pPatient2;
    pPatientDB->addPatient(pPatient);
    pPatientDB->addPatient(pPatient2);

    pPatient->setCRefName( PATIENT_NAME );
    pPatient->setCRefFirstname( PATIENT_FIRSTNAME );
    pPatient->setCRefIDDicom(   PATIENT_IDDICOM );
    pPatient->setCRefBirthdate( ::boost::posix_time::time_from_string( PATIENT_BIRTHDATE ));
    pPatient->setIsMale( PATIENT_SEX );
    pPatient->setDbID( PATIENT_DBID );

    ::fwData::Study::NewSptr pStudy1;
    ::fwData::Study::NewSptr pStudy2;
    ::fwData::Study::NewSptr pStudy3;
    pPatient->addStudy(pStudy1);
    pPatient->addStudy(pStudy2);
    pPatient->addStudy(pStudy3);

    pStudy1->setCRefHospital( STUDY1_HOSPITAL );
    pStudy1->setCRefModality( STUDY1_MODALITY );
    pStudy1->setCRefAcquisitionZone( STUDY1_ZONE );
    pStudy1->setCRefRISId( STUDY1_RISID );
    pStudy1->setCRefUID( STUDY1_UID );
    pStudy1->setDbID( STUDY1_DBID );

    pStudy2->setCRefHospital( STUDY2_HOSPITAL );
    pStudy2->setCRefModality( STUDY2_MODALITY );
    pStudy2->setCRefAcquisitionZone( STUDY2_ZONE );
    pStudy2->setCRefRISId( STUDY2_RISID );
    pStudy2->setCRefUID( STUDY2_UID );
    pStudy2->setDbID( STUDY2_DBID );

    ::fwData::Acquisition::NewSptr pAcq1;
    ::fwData::Acquisition::NewSptr pAcq2;
    ::fwData::Acquisition::NewSptr pAcq3;
    pStudy1->addAcquisition(pAcq1);
    pStudy1->addAcquisition(pAcq2);
    pStudy2->addAcquisition(pAcq3);

    pAcq1->setBitsPerPixel(ACQ1_BITSPERPIXEL);
    pAcq1->setSliceThickness(ACQ1_SLICETHICKNESS);
    pAcq1->setAxe(ACQ1_AXE);
    pAcq1->setUnsignedFlag(ACQ1_UNSIGNEDFLAG);
    pAcq1->setAcquisitionIndex(ACQ1_INDEX);
    pAcq1->setCRefImageType(ACQ1_IMAGETYPE);
    pAcq1->setCRefImageFormat(ACQ1_IMAGEFORMAT);
    pAcq1->setCRefCreationDate( ::boost::posix_time::time_from_string(ACQ1_CREATIONDATE) );
    pAcq1->setIsMain(ACQ1_ISMAIN);
    pAcq1->setIsNormalDir(ACQ1_ISNORMALDIR);
    pAcq1->setCRefUID(ACQ1_UID);
    pAcq1->setDbID(ACQ1_DBID);
    pAcq1->setLaboID(ACQ1_LABOID);
    pAcq1->setNetID(ACQ1_NETID);
    pAcq1->setCRefDateSendToLaboAt( ::boost::posix_time::time_from_string(ACQ1_DATESENDTOLABOAT) );
    pAcq1->setCRefDateReceiveFromLaboAt(::boost::posix_time::time_from_string(ACQ1_DATERECEIVEFROMLABOAT));
    pAcq1->setCRefDateSendToBDDAt(::boost::posix_time::time_from_string(ACQ1_DATESENDTOBDDAT));
    pAcq1->setCRefDateDisponibilityAt(::boost::posix_time::time_from_string(ACQ1_DATEDISPONIBILITYAT));
    pAcq1->setPatientSize(ACQ1_PATIENTSIZE);
    pAcq1->setPatientWeight(ACQ1_PATIENTWEIGHT);
    pAcq1->setRadiations(ACQ1_RADIATIONS);
    pAcq1->setCRefMedicalPrinter(ACQ1_MEDICALPRINTER);
    pAcq1->setCRefMedicalPrinterCorp(ACQ1_MEDICALPRINTERCORP);
    pAcq1->setCRefPatientPosition(ACQ1_PATIENTPOSITION);

    ::fwData::Image::NewSptr pImage1;
    ::fwData::Image::NewSptr pImage2;
    ::fwData::Image::NewSptr pImage3;
    pAcq1->setImage(pImage1);
    pAcq2->setImage(pImage2);
    pAcq3->setImage(pImage3);

    pImage1->setDimension(IMG1_DIMENSION);
    pImage1->setManagesBuff(IMG1_MANAGESBUFF);
    pImage1->setCRefSpacing(IMG1_VECTORCREFSPACING);
    pImage1->setCRefOrigin(IMG1_VECTORCREFORIGIN);
    pImage1->setCRefSize(IMG1_VECTORCREFSIZE);
    pImage1->setCRefFilename(IMG1_CREFFILENAME);
    pImage1->setWindowCenter(IMG1_WINDOWCENTER);
    pImage1->setWindowWidth(IMG1_WINDOWWIDTH    );
    pImage1->setRescaleIntercept(IMG1_RESCALEINTERCEPT);

    pImage2->setDimension(IMG2_DIMENSION);
    pImage2->setManagesBuff(IMG2_MANAGESBUFF);
    pImage2->setCRefSpacing(IMG2_VECTORCREFSPACING);
    pImage2->setCRefOrigin(IMG2_VECTORCREFORIGIN);
    pImage2->setCRefSize(IMG2_VECTORCREFSIZE);
    pImage2->setCRefFilename(IMG2_CREFFILENAME);
    pImage2->setWindowCenter(IMG2_WINDOWCENTER);
    pImage2->setWindowWidth(IMG2_WINDOWWIDTH    );
    pImage2->setRescaleIntercept(IMG2_RESCALEINTERCEPT);

    // image must have size > 0
    pImage3->setCRefSize(IMG3_VECTORCREFSIZE);

    return pPatientDB;
}

//------------------------------------------------------------------------------
