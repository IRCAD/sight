/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>
#include <fwData/Reconstruction.hpp>
#include "AcquisitionTest.h"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::AcquisitionTest );

namespace fwData
{
namespace ut
{

void AcquisitionTest::setUp()
{
    // Set up context before running a test.

}
void AcquisitionTest::tearDown()
{
    // Clean up after the test run.
}

void AcquisitionTest::methode1() //test des attributs
{
    //-----------test values
    ::fwData::Acquisition::NewSptr a1;

    const ::boost::uint8_t  BITSPERPIXEL        = 48 ;
    const float SLICETHICKNESS      = 0.8f ;
    const ::boost::uint8_t  AXE                 = 12 ;
    const bool UNSIGNEDFLAG         = true ;
    const ::boost::uint32_t  ACQUISITIONINDEX   = 5345 ;
    const std::string IMAGETYPE     = "refimageType" ;
    const std::string IMAGEFORMAT   = "refimageFormat" ;
    const std::string CREATIONDATE  = "2007-Feb-21 15:00:00";
    const bool ISMAIN       = true ;
    const bool ISNORMALDIR  = true ;
    const std::string UID   = "uid324" ;
    const ::boost::int32_t  DBID        = 6354 ;
    const ::boost::uint32_t  LABOID     = 357 ;
    const ::boost::uint32_t  NETID      = 21 ;
    const std::string DATESENDTOLABOAT  = "2007-Feb-22 10:05:00";
    const std::string DATERECEIVEFROMLABOAT = "2007-Feb-22 10:05:00";
    const std::string DATESENDTOBDDAT   = "2007-Feb-22 10:05:00";
    const std::string DATEDISPONIBILITYAT = "2007-Feb-22 10:05:00";
    const double PATIENTSIZE        = 45.1352 ;
    const double PATIENTWEIGHT      = 89.6545 ;
    const double RADIATIONS     = 89.6545 ;
    const std::string MEDICALPRINTER        = "MedicalPrinter" ;
    const std::string MEDICALPRINTERCORP    = "MedicalPrinterCorp" ;
    const std::string PATIENTPOSITION       = "PatientPosition" ;


    //-----------process
    a1->setBitsPerPixel(BITSPERPIXEL);
    a1->setSliceThickness(SLICETHICKNESS);
    a1->setAxe(AXE  );
    a1->setUnsignedFlag(UNSIGNEDFLAG);
    a1->setAcquisitionIndex(ACQUISITIONINDEX);
    a1->setCRefImageType(IMAGETYPE);
    a1->setCRefImageFormat(IMAGEFORMAT);
    a1->setCRefCreationDate( ::boost::posix_time::time_from_string(CREATIONDATE) );
    a1->setIsMain(ISMAIN);
    a1->setIsNormalDir(ISNORMALDIR);
    a1->setCRefUID(UID);
    a1->setDbID(DBID);
    a1->setLaboID(LABOID);
    a1->setNetID(NETID);
    a1->setCRefDateSendToLaboAt( ::boost::posix_time::time_from_string(DATESENDTOLABOAT) );
    a1->setCRefDateReceiveFromLaboAt(::boost::posix_time::time_from_string(DATERECEIVEFROMLABOAT));
    a1->setCRefDateSendToBDDAt(::boost::posix_time::time_from_string(DATESENDTOBDDAT));
    a1->setCRefDateDisponibilityAt(::boost::posix_time::time_from_string(DATEDISPONIBILITYAT));
    a1->setPatientSize(PATIENTSIZE);
    a1->setPatientWeight(PATIENTWEIGHT);
    a1->setRadiations(RADIATIONS);
    a1->setCRefMedicalPrinter(MEDICALPRINTER);
    a1->setCRefMedicalPrinterCorp(MEDICALPRINTERCORP);
    a1->setCRefPatientPosition(PATIENTPOSITION);


    //-------------check
    CPPUNIT_ASSERT_EQUAL(BITSPERPIXEL ,  a1->getBitsPerPixel()) ;
    CPPUNIT_ASSERT_EQUAL(BITSPERPIXEL ,  a1->getCRefBitsPerPixel()) ;
    CPPUNIT_ASSERT_EQUAL(BITSPERPIXEL ,  a1->getRefBitsPerPixel()) ;

    CPPUNIT_ASSERT_EQUAL(SLICETHICKNESS ,  a1->getSliceThickness()) ;
    CPPUNIT_ASSERT_EQUAL(SLICETHICKNESS ,  a1->getCRefSliceThickness()) ;
    CPPUNIT_ASSERT_EQUAL(SLICETHICKNESS ,  a1->getRefSliceThickness()) ;

    CPPUNIT_ASSERT_EQUAL(AXE ,  a1->getAxe(    )) ;
    CPPUNIT_ASSERT_EQUAL(AXE ,  a1->getCRefAxe(    )) ;
    CPPUNIT_ASSERT_EQUAL(AXE ,  a1->getRefAxe( )) ;

    CPPUNIT_ASSERT_EQUAL(UNSIGNEDFLAG  , a1->getUnsignedFlag()) ;
    CPPUNIT_ASSERT_EQUAL(UNSIGNEDFLAG  , a1->getCRefUnsignedFlag()) ;
    CPPUNIT_ASSERT_EQUAL(UNSIGNEDFLAG  , a1->getRefUnsignedFlag()) ;

    CPPUNIT_ASSERT_EQUAL(ACQUISITIONINDEX ,  a1->getAcquisitionIndex()) ;
    CPPUNIT_ASSERT_EQUAL(ACQUISITIONINDEX ,  a1->getCRefAcquisitionIndex()) ;
    CPPUNIT_ASSERT_EQUAL(ACQUISITIONINDEX ,  a1->getRefAcquisitionIndex()) ;

    CPPUNIT_ASSERT_EQUAL(IMAGETYPE  , a1->getImageType()) ;
    CPPUNIT_ASSERT_EQUAL(IMAGETYPE  , a1->getCRefImageType()) ;
    CPPUNIT_ASSERT_EQUAL(IMAGETYPE  , a1->getRefImageType()) ;

    CPPUNIT_ASSERT_EQUAL(IMAGEFORMAT ,  a1->getImageFormat()) ;
    CPPUNIT_ASSERT_EQUAL(IMAGEFORMAT ,  a1->getCRefImageFormat()) ;
    CPPUNIT_ASSERT_EQUAL(IMAGEFORMAT ,  a1->getRefImageFormat()) ;

    CPPUNIT_ASSERT_EQUAL(CREATIONDATE  , ::boost::posix_time::to_simple_string(a1->getCreationDate())) ;
    CPPUNIT_ASSERT_EQUAL(CREATIONDATE  , ::boost::posix_time::to_simple_string(a1->getCRefCreationDate())) ;
    CPPUNIT_ASSERT_EQUAL(CREATIONDATE  , ::boost::posix_time::to_simple_string(a1->getRefCreationDate())) ;

    CPPUNIT_ASSERT_EQUAL(ISMAIN  , a1->getIsMain()) ;
    CPPUNIT_ASSERT_EQUAL(ISMAIN  , a1->getCRefIsMain()) ;
    CPPUNIT_ASSERT_EQUAL(ISMAIN  , a1->getRefIsMain()) ;

    CPPUNIT_ASSERT_EQUAL(ISNORMALDIR ,  a1->getIsNormalDir()) ;
    CPPUNIT_ASSERT_EQUAL(ISNORMALDIR ,  a1->getCRefIsNormalDir()) ;
    CPPUNIT_ASSERT_EQUAL(ISNORMALDIR ,  a1->getRefIsNormalDir()) ;

    CPPUNIT_ASSERT_EQUAL(UID  , a1->getUID()) ;
    CPPUNIT_ASSERT_EQUAL(UID  , a1->getCRefUID()) ;
    CPPUNIT_ASSERT_EQUAL(UID  , a1->getRefUID()) ;

    CPPUNIT_ASSERT_EQUAL(DBID  , a1->getDbID()) ;
    CPPUNIT_ASSERT_EQUAL(DBID  , a1->getCRefDbID()) ;
    CPPUNIT_ASSERT_EQUAL(DBID  , a1->getRefDbID()) ;

    CPPUNIT_ASSERT_EQUAL(LABOID ,  a1->getLaboID()) ;
    CPPUNIT_ASSERT_EQUAL(LABOID ,  a1->getCRefLaboID()) ;
    CPPUNIT_ASSERT_EQUAL(LABOID ,  a1->getRefLaboID()) ;

    CPPUNIT_ASSERT_EQUAL(NETID  , a1->getNetID()) ;
    CPPUNIT_ASSERT_EQUAL(NETID  , a1->getCRefNetID()) ;
    CPPUNIT_ASSERT_EQUAL(NETID  , a1->getRefNetID()) ;

    CPPUNIT_ASSERT_EQUAL(DATESENDTOLABOAT ,  ::boost::posix_time::to_simple_string(a1->getDateSendToLaboAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATESENDTOLABOAT ,  ::boost::posix_time::to_simple_string(a1->getCRefDateSendToLaboAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATESENDTOLABOAT ,  ::boost::posix_time::to_simple_string(a1->getRefDateSendToLaboAt())) ;

    CPPUNIT_ASSERT_EQUAL(DATERECEIVEFROMLABOAT  , ::boost::posix_time::to_simple_string(a1->getDateReceiveFromLaboAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATERECEIVEFROMLABOAT  , ::boost::posix_time::to_simple_string(a1->getCRefDateReceiveFromLaboAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATERECEIVEFROMLABOAT  , ::boost::posix_time::to_simple_string(a1->getRefDateReceiveFromLaboAt())) ;

    CPPUNIT_ASSERT_EQUAL(DATESENDTOBDDAT ,  ::boost::posix_time::to_simple_string(a1->getDateSendToBDDAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATESENDTOBDDAT ,  ::boost::posix_time::to_simple_string(a1->getCRefDateSendToBDDAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATESENDTOBDDAT ,  ::boost::posix_time::to_simple_string(a1->getRefDateSendToBDDAt())) ;

    CPPUNIT_ASSERT_EQUAL(DATEDISPONIBILITYAT  , ::boost::posix_time::to_simple_string(a1->getDateDisponibilityAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATEDISPONIBILITYAT  , ::boost::posix_time::to_simple_string(a1->getCRefDateDisponibilityAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATEDISPONIBILITYAT  , ::boost::posix_time::to_simple_string(a1->getRefDateDisponibilityAt())) ;

    CPPUNIT_ASSERT_EQUAL(PATIENTSIZE ,  a1->getPatientSize()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTSIZE ,  a1->getCRefPatientSize()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTSIZE ,  a1->getRefPatientSize()) ;

    CPPUNIT_ASSERT_EQUAL(PATIENTWEIGHT ,  a1->getPatientWeight()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTWEIGHT ,  a1->getCRefPatientWeight()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTWEIGHT ,  a1->getRefPatientWeight()) ;

    CPPUNIT_ASSERT_EQUAL(RADIATIONS ,  a1->getRadiations()) ;
    CPPUNIT_ASSERT_EQUAL(RADIATIONS ,  a1->getCRefRadiations()) ;
    CPPUNIT_ASSERT_EQUAL(RADIATIONS ,  a1->getRefRadiations()) ;

    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTER  , a1->getMedicalPrinter()) ;
    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTER  , a1->getCRefMedicalPrinter()) ;
    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTER  , a1->getRefMedicalPrinter()) ;

    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTERCORP ,  a1->getMedicalPrinterCorp()) ;
    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTERCORP ,  a1->getCRefMedicalPrinterCorp()) ;
    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTERCORP ,  a1->getRefMedicalPrinterCorp()) ;

    CPPUNIT_ASSERT_EQUAL(PATIENTPOSITION ,  a1->getPatientPosition()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTPOSITION ,  a1->getCRefPatientPosition()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTPOSITION ,  a1->getRefPatientPosition()) ;


}
void AcquisitionTest::methode2()
{
    //-----------test values
    ::fwData::Acquisition::NewSptr a1;

    const ::boost::uint8_t  BITSPERPIXEL        = 48 ;
    const float SLICETHICKNESS      = 0.8f ;
    const ::boost::uint8_t  AXE                 = 12 ;
    const bool UNSIGNEDFLAG         = true ;
    const ::boost::uint32_t  ACQUISITIONINDEX   = 5345 ;
    const std::string IMAGETYPE     = "refimageType" ;
    const std::string IMAGEFORMAT   = "refimageFormat" ;
    const std::string CREATIONDATE  = "2007-Feb-21 15:00:00";
    const bool ISMAIN       = true ;
    const bool ISNORMALDIR  = true ;
    const std::string UID   = "uid324" ;
    const ::boost::int32_t  DBID        = 6354 ;
    const ::boost::uint32_t  LABOID     = 357 ;
    const ::boost::uint32_t  NETID      = 21 ;
    const std::string DATESENDTOLABOAT  = "2007-Feb-22 10:05:00";
    const std::string DATERECEIVEFROMLABOAT = "2007-Feb-22 10:05:00";
    const std::string DATESENDTOBDDAT   = "2007-Feb-22 10:05:00";
    const std::string DATEDISPONIBILITYAT = "2007-Feb-22 10:05:00";
    const double PATIENTSIZE        = 45.1352 ;
    const double PATIENTWEIGHT      = 89.6545 ;
    const double RADIATIONS     = 89.6545 ;
    const std::string MEDICALPRINTER        = "MedicalPrinter" ;
    const std::string MEDICALPRINTERCORP    = "MedicalPrinterCorp" ;
    const std::string PATIENTPOSITION       = "PatientPosition" ;


    //-----------process
    a1->setCRefBitsPerPixel(BITSPERPIXEL);
    a1->setCRefSliceThickness(SLICETHICKNESS);
    a1->setCRefAxe(AXE  );
    a1->setCRefUnsignedFlag(UNSIGNEDFLAG);
    a1->setCRefAcquisitionIndex(ACQUISITIONINDEX);
    a1->setCRefImageType(IMAGETYPE);
    a1->setCRefImageFormat(IMAGEFORMAT);
    a1->setCRefCreationDate(::boost::posix_time::time_from_string(CREATIONDATE));
    a1->setCRefIsMain(ISMAIN);
    a1->setCRefIsNormalDir(ISNORMALDIR);
    a1->setCRefUID(UID);
    a1->setCRefDbID(DBID);
    a1->setCRefLaboID(LABOID);
    a1->setCRefNetID(NETID);
    a1->setCRefDateSendToLaboAt(::boost::posix_time::time_from_string(DATESENDTOLABOAT));
    a1->setCRefDateReceiveFromLaboAt(::boost::posix_time::time_from_string(DATERECEIVEFROMLABOAT));
    a1->setCRefDateSendToBDDAt(::boost::posix_time::time_from_string(DATESENDTOBDDAT));
    a1->setCRefDateDisponibilityAt(::boost::posix_time::time_from_string(DATEDISPONIBILITYAT));
    a1->setCRefPatientSize(PATIENTSIZE);
    a1->setCRefPatientWeight(PATIENTWEIGHT);
    a1->setCRefRadiations(RADIATIONS);
    a1->setCRefMedicalPrinter(MEDICALPRINTER);
    a1->setCRefMedicalPrinterCorp(MEDICALPRINTERCORP);
    a1->setCRefPatientPosition(PATIENTPOSITION);


    //-------------check
    CPPUNIT_ASSERT_EQUAL(BITSPERPIXEL ,  a1->getBitsPerPixel()) ;
    CPPUNIT_ASSERT_EQUAL(BITSPERPIXEL ,  a1->getCRefBitsPerPixel()) ;
    CPPUNIT_ASSERT_EQUAL(BITSPERPIXEL ,  a1->getRefBitsPerPixel()) ;

    CPPUNIT_ASSERT_EQUAL(SLICETHICKNESS ,  a1->getSliceThickness()) ;
    CPPUNIT_ASSERT_EQUAL(SLICETHICKNESS ,  a1->getCRefSliceThickness()) ;
    CPPUNIT_ASSERT_EQUAL(SLICETHICKNESS ,  a1->getRefSliceThickness()) ;

    CPPUNIT_ASSERT_EQUAL(AXE ,  a1->getAxe(    )) ;
    CPPUNIT_ASSERT_EQUAL(AXE ,  a1->getCRefAxe(    )) ;
    CPPUNIT_ASSERT_EQUAL(AXE ,  a1->getRefAxe( )) ;

    CPPUNIT_ASSERT_EQUAL(UNSIGNEDFLAG  , a1->getUnsignedFlag()) ;
    CPPUNIT_ASSERT_EQUAL(UNSIGNEDFLAG  , a1->getCRefUnsignedFlag()) ;
    CPPUNIT_ASSERT_EQUAL(UNSIGNEDFLAG  , a1->getRefUnsignedFlag()) ;

    CPPUNIT_ASSERT_EQUAL(ACQUISITIONINDEX ,  a1->getAcquisitionIndex()) ;
    CPPUNIT_ASSERT_EQUAL(ACQUISITIONINDEX ,  a1->getCRefAcquisitionIndex()) ;
    CPPUNIT_ASSERT_EQUAL(ACQUISITIONINDEX ,  a1->getRefAcquisitionIndex()) ;

    CPPUNIT_ASSERT_EQUAL(IMAGETYPE  , a1->getImageType()) ;
    CPPUNIT_ASSERT_EQUAL(IMAGETYPE  , a1->getCRefImageType()) ;
    CPPUNIT_ASSERT_EQUAL(IMAGETYPE  , a1->getRefImageType()) ;

    CPPUNIT_ASSERT_EQUAL(IMAGEFORMAT ,  a1->getImageFormat()) ;
    CPPUNIT_ASSERT_EQUAL(IMAGEFORMAT ,  a1->getCRefImageFormat()) ;
    CPPUNIT_ASSERT_EQUAL(IMAGEFORMAT ,  a1->getRefImageFormat()) ;

    CPPUNIT_ASSERT_EQUAL(CREATIONDATE  , ::boost::posix_time::to_simple_string(a1->getCreationDate())) ;
    CPPUNIT_ASSERT_EQUAL(CREATIONDATE  , ::boost::posix_time::to_simple_string(a1->getCRefCreationDate())) ;
    CPPUNIT_ASSERT_EQUAL(CREATIONDATE  , ::boost::posix_time::to_simple_string(a1->getRefCreationDate())) ;

    CPPUNIT_ASSERT_EQUAL(ISMAIN  , a1->getIsMain()) ;
    CPPUNIT_ASSERT_EQUAL(ISMAIN  , a1->getCRefIsMain()) ;
    CPPUNIT_ASSERT_EQUAL(ISMAIN  , a1->getRefIsMain()) ;

    CPPUNIT_ASSERT_EQUAL(ISNORMALDIR ,  a1->getIsNormalDir()) ;
    CPPUNIT_ASSERT_EQUAL(ISNORMALDIR ,  a1->getCRefIsNormalDir()) ;
    CPPUNIT_ASSERT_EQUAL(ISNORMALDIR ,  a1->getRefIsNormalDir()) ;

    CPPUNIT_ASSERT_EQUAL(UID  , a1->getUID()) ;
    CPPUNIT_ASSERT_EQUAL(UID  , a1->getCRefUID()) ;
    CPPUNIT_ASSERT_EQUAL(UID  , a1->getRefUID()) ;

    CPPUNIT_ASSERT_EQUAL(DBID  , a1->getDbID()) ;
    CPPUNIT_ASSERT_EQUAL(DBID  , a1->getCRefDbID()) ;
    CPPUNIT_ASSERT_EQUAL(DBID  , a1->getRefDbID()) ;

    CPPUNIT_ASSERT_EQUAL(LABOID ,  a1->getLaboID()) ;
    CPPUNIT_ASSERT_EQUAL(LABOID ,  a1->getCRefLaboID()) ;
    CPPUNIT_ASSERT_EQUAL(LABOID ,  a1->getRefLaboID()) ;

    CPPUNIT_ASSERT_EQUAL(NETID  , a1->getNetID()) ;
    CPPUNIT_ASSERT_EQUAL(NETID  , a1->getCRefNetID()) ;
    CPPUNIT_ASSERT_EQUAL(NETID  , a1->getRefNetID()) ;

    CPPUNIT_ASSERT_EQUAL(DATESENDTOLABOAT ,  ::boost::posix_time::to_simple_string(a1->getDateSendToLaboAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATESENDTOLABOAT ,  ::boost::posix_time::to_simple_string(a1->getCRefDateSendToLaboAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATESENDTOLABOAT ,  ::boost::posix_time::to_simple_string(a1->getRefDateSendToLaboAt())) ;

    CPPUNIT_ASSERT_EQUAL(DATERECEIVEFROMLABOAT  , ::boost::posix_time::to_simple_string(a1->getDateReceiveFromLaboAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATERECEIVEFROMLABOAT  , ::boost::posix_time::to_simple_string(a1->getCRefDateReceiveFromLaboAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATERECEIVEFROMLABOAT  , ::boost::posix_time::to_simple_string(a1->getRefDateReceiveFromLaboAt())) ;

    CPPUNIT_ASSERT_EQUAL(DATESENDTOBDDAT ,  ::boost::posix_time::to_simple_string(a1->getDateSendToBDDAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATESENDTOBDDAT ,  ::boost::posix_time::to_simple_string(a1->getCRefDateSendToBDDAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATESENDTOBDDAT ,  ::boost::posix_time::to_simple_string(a1->getRefDateSendToBDDAt())) ;

    CPPUNIT_ASSERT_EQUAL(DATEDISPONIBILITYAT  , ::boost::posix_time::to_simple_string(a1->getDateDisponibilityAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATEDISPONIBILITYAT  , ::boost::posix_time::to_simple_string(a1->getCRefDateDisponibilityAt())) ;
    CPPUNIT_ASSERT_EQUAL(DATEDISPONIBILITYAT  , ::boost::posix_time::to_simple_string(a1->getRefDateDisponibilityAt())) ;

    CPPUNIT_ASSERT_EQUAL(PATIENTSIZE ,  a1->getPatientSize()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTSIZE ,  a1->getCRefPatientSize()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTSIZE ,  a1->getRefPatientSize()) ;

    CPPUNIT_ASSERT_EQUAL(PATIENTWEIGHT ,  a1->getPatientWeight()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTWEIGHT ,  a1->getCRefPatientWeight()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTWEIGHT ,  a1->getRefPatientWeight()) ;

    CPPUNIT_ASSERT_EQUAL(RADIATIONS ,  a1->getRadiations()) ;
    CPPUNIT_ASSERT_EQUAL(RADIATIONS ,  a1->getCRefRadiations()) ;
    CPPUNIT_ASSERT_EQUAL(RADIATIONS ,  a1->getRefRadiations()) ;

    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTER  , a1->getMedicalPrinter()) ;
    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTER  , a1->getCRefMedicalPrinter()) ;
    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTER  , a1->getRefMedicalPrinter()) ;

    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTERCORP ,  a1->getMedicalPrinterCorp()) ;
    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTERCORP ,  a1->getCRefMedicalPrinterCorp()) ;
    CPPUNIT_ASSERT_EQUAL(MEDICALPRINTERCORP ,  a1->getRefMedicalPrinterCorp()) ;

    CPPUNIT_ASSERT_EQUAL(PATIENTPOSITION ,  a1->getPatientPosition()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTPOSITION ,  a1->getCRefPatientPosition()) ;
    CPPUNIT_ASSERT_EQUAL(PATIENTPOSITION ,  a1->getRefPatientPosition()) ;
}

void AcquisitionTest::Image()
{

    ::fwData::Acquisition::NewSptr a1;
    ::fwData::Image::sptr i1 = ::fwData::Image::New();

    a1->setImage(i1) ;

    //check
    CPPUNIT_ASSERT_EQUAL(a1->getImage(),  i1);

}

void AcquisitionTest::StructAnat()
{
    ::fwData::Acquisition::NewSptr a1;
    ::fwData::Image::sptr i1 = ::fwData::Image::New();

    a1->setStructAnat(i1) ;

    //check
    CPPUNIT_ASSERT_EQUAL(a1->getStructAnat(),  i1);
}

void AcquisitionTest::Reconstruction()
{
    ::fwData::Acquisition::ReconstructionContainerType::size_type  NB = 0 ;
    ::fwData::Acquisition::NewSptr a1;
    ::fwData::Reconstruction::NewSptr r1 ;
    //check
    CPPUNIT_ASSERT_EQUAL(NB, a1->getNumberOfReconstructions());

    a1->addReconstruction(r1) ;
    NB = 1 ;

    //check
    CPPUNIT_ASSERT_EQUAL(NB, a1->getNumberOfReconstructions());
}

} //namespace ut
} //namespace fwData
