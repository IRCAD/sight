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

#include "fwData/Acquisition.hpp"
#include "fwData/Image.hpp"
#include "fwData/Reconstruction.hpp"
#include "AcquisitionTest.h"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( AcquisitionTest );

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

	const ::boost::uint8_t  BITSPERPIXEL		= 48 ;
	const float SLICETHICKNESS		= 0.8f ;
	const ::boost::uint8_t  AXE					= 12 ;
	const bool UNSIGNEDFLAG			= true ;
	const ::boost::uint32_t  ACQUISITIONINDEX	= 5345 ;
	const std::string IMAGETYPE		= "refimageType" ;
	const std::string IMAGEFORMAT	= "refimageFormat" ;
	const std::string CREATIONDATE	= "2007-Feb-21 15:00:00";
	const bool ISMAIN		= true ;
	const bool ISNORMALDIR	= true ;
	const std::string UID	= "uid324" ;
	const ::boost::int32_t  DBID		= 6354 ;
	const ::boost::uint32_t  LABOID		= 357 ;
	const ::boost::uint32_t  NETID		= 21 ;
	const std::string DATESENDTOLABOAT	= "2007-Feb-22 10:05:00";
	const std::string DATERECEIVEFROMLABOAT	= "2007-Feb-22 10:05:00";
	const std::string DATESENDTOBDDAT	= "2007-Feb-22 10:05:00";
	const std::string DATEDISPONIBILITYAT = "2007-Feb-22 10:05:00";
	const double PATIENTSIZE		= 45.1352 ;
	const double PATIENTWEIGHT		= 89.6545 ;
	const double RADIATIONS		= 89.6545 ;
	const std::string MEDICALPRINTER		= "MedicalPrinter" ;
	const std::string MEDICALPRINTERCORP	= "MedicalPrinterCorp" ;
	const std::string PATIENTPOSITION		= "PatientPosition" ;


	//-----------process
	a1->setBitsPerPixel(BITSPERPIXEL);
	a1->setSliceThickness(SLICETHICKNESS);
	a1->setAxe(AXE	);
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
	CPPUNIT_ASSERT_EQUAL(a1->getBitsPerPixel(),  BITSPERPIXEL ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefBitsPerPixel(),  BITSPERPIXEL ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefBitsPerPixel(),  BITSPERPIXEL ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getSliceThickness(),  SLICETHICKNESS ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefSliceThickness(),  SLICETHICKNESS ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefSliceThickness(),  SLICETHICKNESS ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getAxe(	),  AXE ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefAxe(	),  AXE ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefAxe(	),  AXE ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getUnsignedFlag(), UNSIGNEDFLAG  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefUnsignedFlag(), UNSIGNEDFLAG  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefUnsignedFlag(), UNSIGNEDFLAG  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getAcquisitionIndex(),  ACQUISITIONINDEX ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefAcquisitionIndex(),  ACQUISITIONINDEX ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefAcquisitionIndex(),  ACQUISITIONINDEX ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getImageType(), IMAGETYPE  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefImageType(), IMAGETYPE  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefImageType(), IMAGETYPE  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getImageFormat(),  IMAGEFORMAT ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefImageFormat(),  IMAGEFORMAT ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefImageFormat(),  IMAGEFORMAT ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCreationDate()), CREATIONDATE  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefCreationDate()), CREATIONDATE  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefCreationDate()), CREATIONDATE  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getIsMain(), ISMAIN  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefIsMain(), ISMAIN  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefIsMain(), ISMAIN  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getIsNormalDir(),  ISNORMALDIR ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefIsNormalDir(),  ISNORMALDIR ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefIsNormalDir(),  ISNORMALDIR ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getUID(), UID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefUID(), UID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefUID(), UID  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getDbID(), DBID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefDbID(), DBID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefDbID(), DBID  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getLaboID(),  LABOID ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefLaboID(),  LABOID ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefLaboID(),  LABOID ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getNetID(), NETID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefNetID(), NETID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefNetID(), NETID  ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getDateSendToLaboAt()),  DATESENDTOLABOAT ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefDateSendToLaboAt()),  DATESENDTOLABOAT ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefDateSendToLaboAt()),  DATESENDTOLABOAT ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getDateReceiveFromLaboAt()), DATERECEIVEFROMLABOAT  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefDateReceiveFromLaboAt()), DATERECEIVEFROMLABOAT  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefDateReceiveFromLaboAt()), DATERECEIVEFROMLABOAT  ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getDateSendToBDDAt()),  DATESENDTOBDDAT ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefDateSendToBDDAt()),  DATESENDTOBDDAT ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefDateSendToBDDAt()),  DATESENDTOBDDAT ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getDateDisponibilityAt()), DATEDISPONIBILITYAT  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefDateDisponibilityAt()), DATEDISPONIBILITYAT  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefDateDisponibilityAt()), DATEDISPONIBILITYAT  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getPatientSize(),  PATIENTSIZE ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefPatientSize(),  PATIENTSIZE ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefPatientSize(),  PATIENTSIZE ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getPatientWeight(),  PATIENTWEIGHT ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefPatientWeight(),  PATIENTWEIGHT ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefPatientWeight(),  PATIENTWEIGHT ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getRadiations(),  RADIATIONS ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefRadiations(),  RADIATIONS ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefRadiations(),  RADIATIONS ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getMedicalPrinter(), MEDICALPRINTER  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefMedicalPrinter(), MEDICALPRINTER  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefMedicalPrinter(), MEDICALPRINTER  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getMedicalPrinterCorp(),  MEDICALPRINTERCORP ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefMedicalPrinterCorp(),  MEDICALPRINTERCORP ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefMedicalPrinterCorp(),  MEDICALPRINTERCORP ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getPatientPosition(),  PATIENTPOSITION ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefPatientPosition(),  PATIENTPOSITION ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefPatientPosition(),  PATIENTPOSITION ) ;


}
void AcquisitionTest::methode2()
{
	//-----------test values
	::fwData::Acquisition::NewSptr a1;

	const ::boost::uint8_t  BITSPERPIXEL		= 48 ;
	const float SLICETHICKNESS		= 0.8f ;
	const ::boost::uint8_t  AXE					= 12 ;
	const bool UNSIGNEDFLAG			= true ;
	const ::boost::uint32_t  ACQUISITIONINDEX	= 5345 ;
	const std::string IMAGETYPE		= "refimageType" ;
	const std::string IMAGEFORMAT	= "refimageFormat" ;
	const std::string CREATIONDATE	= "2007-Feb-21 15:00:00";
	const bool ISMAIN		= true ;
	const bool ISNORMALDIR	= true ;
	const std::string UID	= "uid324" ;
	const ::boost::int32_t  DBID		= 6354 ;
	const ::boost::uint32_t  LABOID		= 357 ;
	const ::boost::uint32_t  NETID		= 21 ;
	const std::string DATESENDTOLABOAT	= "2007-Feb-22 10:05:00";
	const std::string DATERECEIVEFROMLABOAT	= "2007-Feb-22 10:05:00";
	const std::string DATESENDTOBDDAT	= "2007-Feb-22 10:05:00";
	const std::string DATEDISPONIBILITYAT = "2007-Feb-22 10:05:00";
	const double PATIENTSIZE		= 45.1352 ;
	const double PATIENTWEIGHT		= 89.6545 ;
	const double RADIATIONS		= 89.6545 ;
	const std::string MEDICALPRINTER		= "MedicalPrinter" ;
	const std::string MEDICALPRINTERCORP	= "MedicalPrinterCorp" ;
	const std::string PATIENTPOSITION		= "PatientPosition" ;


	//-----------process
	a1->setCRefBitsPerPixel(BITSPERPIXEL);
	a1->setCRefSliceThickness(SLICETHICKNESS);
	a1->setCRefAxe(AXE	);
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
	CPPUNIT_ASSERT_EQUAL(a1->getBitsPerPixel(),  BITSPERPIXEL ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefBitsPerPixel(),  BITSPERPIXEL ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefBitsPerPixel(),  BITSPERPIXEL ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getSliceThickness(),  SLICETHICKNESS ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefSliceThickness(),  SLICETHICKNESS ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefSliceThickness(),  SLICETHICKNESS ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getAxe(	),  AXE ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefAxe(	),  AXE ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefAxe(	),  AXE ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getUnsignedFlag(), UNSIGNEDFLAG  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefUnsignedFlag(), UNSIGNEDFLAG  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefUnsignedFlag(), UNSIGNEDFLAG  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getAcquisitionIndex(),  ACQUISITIONINDEX ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefAcquisitionIndex(),  ACQUISITIONINDEX ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefAcquisitionIndex(),  ACQUISITIONINDEX ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getImageType(), IMAGETYPE  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefImageType(), IMAGETYPE  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefImageType(), IMAGETYPE  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getImageFormat(),  IMAGEFORMAT ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefImageFormat(),  IMAGEFORMAT ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefImageFormat(),  IMAGEFORMAT ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCreationDate()), CREATIONDATE  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefCreationDate()), CREATIONDATE  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefCreationDate()), CREATIONDATE  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getIsMain(), ISMAIN  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefIsMain(), ISMAIN  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefIsMain(), ISMAIN  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getIsNormalDir(),  ISNORMALDIR ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefIsNormalDir(),  ISNORMALDIR ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefIsNormalDir(),  ISNORMALDIR ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getUID(), UID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefUID(), UID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefUID(), UID  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getDbID(), DBID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefDbID(), DBID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefDbID(), DBID  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getLaboID(),  LABOID ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefLaboID(),  LABOID ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefLaboID(),  LABOID ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getNetID(), NETID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefNetID(), NETID  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefNetID(), NETID  ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getDateSendToLaboAt()),  DATESENDTOLABOAT ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefDateSendToLaboAt()),  DATESENDTOLABOAT ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefDateSendToLaboAt()),  DATESENDTOLABOAT ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getDateReceiveFromLaboAt()), DATERECEIVEFROMLABOAT  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefDateReceiveFromLaboAt()), DATERECEIVEFROMLABOAT  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefDateReceiveFromLaboAt()), DATERECEIVEFROMLABOAT  ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getDateSendToBDDAt()),  DATESENDTOBDDAT ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefDateSendToBDDAt()),  DATESENDTOBDDAT ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefDateSendToBDDAt()),  DATESENDTOBDDAT ) ;

	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getDateDisponibilityAt()), DATEDISPONIBILITYAT  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getCRefDateDisponibilityAt()), DATEDISPONIBILITYAT  ) ;
	CPPUNIT_ASSERT_EQUAL(::boost::posix_time::to_simple_string(a1->getRefDateDisponibilityAt()), DATEDISPONIBILITYAT  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getPatientSize(),  PATIENTSIZE ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefPatientSize(),  PATIENTSIZE ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefPatientSize(),  PATIENTSIZE ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getPatientWeight(),  PATIENTWEIGHT ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefPatientWeight(),  PATIENTWEIGHT ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefPatientWeight(),  PATIENTWEIGHT ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getRadiations(),  RADIATIONS ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefRadiations(),  RADIATIONS ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefRadiations(),  RADIATIONS ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getMedicalPrinter(), MEDICALPRINTER  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefMedicalPrinter(), MEDICALPRINTER  ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefMedicalPrinter(), MEDICALPRINTER  ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getMedicalPrinterCorp(),  MEDICALPRINTERCORP ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefMedicalPrinterCorp(),  MEDICALPRINTERCORP ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefMedicalPrinterCorp(),  MEDICALPRINTERCORP ) ;

	CPPUNIT_ASSERT_EQUAL(a1->getPatientPosition(),  PATIENTPOSITION ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getCRefPatientPosition(),  PATIENTPOSITION ) ;
	CPPUNIT_ASSERT_EQUAL(a1->getRefPatientPosition(),  PATIENTPOSITION ) ;


}

void AcquisitionTest::Image()
{

	::fwData::Acquisition::NewSptr a1;
	::fwData::Image::sptr i1(new ::fwData::Image);

	a1->setImage(i1) ;

	//check
	CPPUNIT_ASSERT_EQUAL(a1->getImage(),  i1);

}

void AcquisitionTest::StructAnat()
{
	::fwData::Acquisition::NewSptr a1;
	::fwData::Image::sptr i1(new ::fwData::Image);

	a1->setStructAnat(i1) ;

	//check
	CPPUNIT_ASSERT_EQUAL(a1->getStructAnat(),  i1);
}

void AcquisitionTest::Reconstruction()
{
	::boost::uint32_t  NB = 0 ;
	::fwData::Acquisition::NewSptr a1;
	::fwData::Reconstruction::NewSptr r1 ;
	//check
	CPPUNIT_ASSERT_EQUAL(a1->getReconstructionSize(),  NB);

	a1->addReconstruction(r1) ;
	NB = 1 ;

	//check
	CPPUNIT_ASSERT_EQUAL(a1->getReconstructionSize(),  NB);
}

