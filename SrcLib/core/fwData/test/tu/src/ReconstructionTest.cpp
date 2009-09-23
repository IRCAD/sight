/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>
#include <vector>
#include <ostream>
#include <map>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "fwData/Study.hpp"
#include "fwData/Patient.hpp"
#include "fwData/Acquisition.hpp"
#include "fwData/Image.hpp"
#include "fwData/Reconstruction.hpp"
#include "ReconstructionTest.hpp"



// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ReconstructionTest );

void ReconstructionTest::setUp()
{
	// Set up context before running a test.
}

//------------------------------------------------------------------------------

void ReconstructionTest::tearDown()
{
	// Clean up after the test run.
}

//------------------------------------------------------------------------------

void ReconstructionTest::methode1() //test des setters et getters
{
	const bool ISVISIBLE		= true ;
	const double RED			= 24.65 ;
	const double GREEN			= 24.65 ;
	const double BLUE			= 24.65 ;
	const double ALPHA			= 12.80 ;
	const std::string CREFRECONSTRUCTIONFORMAT		= "ReconstructionFormat" ;
	const std::string CREFORGANNAME		= "OrganName" ;
	const std::string CREFSTRUCTURETYPE		= "StructureType" ;
	const bool ISCLOSED		= true ;
	const bool ISAUTOMATIC		= true ;
	const double AVGVOLUME		= 48.94 ;
	const double VOLSTDDEVIATION		= 32.02 ;
	const double VOLPCTCONFIDENCE		= 10.10 ;
	const std::string CREFRECONSTRUCTIONTIME		= "ReconstructionTime" ;
	const bool MASKGENERATED		= true ;
	const boost::uint8_t LEVEL		= 2 ;
	const boost::uint16_t LABEL		= 4 ;
	const bool GENERATED3D		= true ;
	const std::string CREFTYPE3D		= "CRefType3D" ;
	const boost::filesystem::path CREFPATH	= boost::filesystem::path("C:/TMP") ;
	const boost::int32_t DBID		= 5 ;


	// process
	::fwData::Reconstruction::NewSptr p1;


	p1->setIsVisible(					ISVISIBLE);
	p1->setCRefReconstructionFormat(					CREFRECONSTRUCTIONFORMAT);
	p1->setCRefOrganName(					CREFORGANNAME);
	p1->setCRefStructureType(					CREFSTRUCTURETYPE);
	p1->setIsClosed(				ISCLOSED	);
	p1->setIsAutomatic(					ISAUTOMATIC);
	p1->setAvgVolume(					AVGVOLUME);
	p1->setVolStdDeviation(					VOLSTDDEVIATION);
	p1->setVolPctConfidence(					VOLPCTCONFIDENCE);
	p1->setCRefReconstructionTime(					CREFRECONSTRUCTIONTIME);
	p1->setMaskGenerated(					MASKGENERATED);
	p1->setLevel(					LEVEL);
	p1->setLabel(					LABEL);
	p1->setGenerated3D(					GENERATED3D);
	p1->setCRefType3D(					CREFTYPE3D);
	p1->setCRefPath(					CREFPATH);
	p1->setDbID(					DBID);

	// check
	CPPUNIT_ASSERT_EQUAL(p1->getIsVisible(),			ISVISIBLE);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIsVisible(),			ISVISIBLE);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIsVisible(),			ISVISIBLE);

	CPPUNIT_ASSERT_EQUAL(p1->getReconstructionFormat(),		CREFRECONSTRUCTIONFORMAT);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefReconstructionFormat(),		CREFRECONSTRUCTIONFORMAT);
	CPPUNIT_ASSERT_EQUAL(p1->getRefReconstructionFormat(),		CREFRECONSTRUCTIONFORMAT);

	CPPUNIT_ASSERT_EQUAL(p1->getOrganName(),		CREFORGANNAME);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefOrganName(),		CREFORGANNAME);
	CPPUNIT_ASSERT_EQUAL(p1->getRefOrganName(),		CREFORGANNAME);

	CPPUNIT_ASSERT_EQUAL(p1->getStructureType(),		CREFSTRUCTURETYPE);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefStructureType(),		CREFSTRUCTURETYPE);
	CPPUNIT_ASSERT_EQUAL(p1->getRefStructureType(),		CREFSTRUCTURETYPE);

	CPPUNIT_ASSERT_EQUAL(p1->getIsClosed(),		ISCLOSED);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIsClosed(),		ISCLOSED);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIsClosed(),		ISCLOSED);

	CPPUNIT_ASSERT_EQUAL(p1->getIsAutomatic(),				ISAUTOMATIC);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIsAutomatic(),				ISAUTOMATIC);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIsAutomatic(),				ISAUTOMATIC);

	CPPUNIT_ASSERT_EQUAL(p1->getAvgVolume(),			AVGVOLUME);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefAvgVolume(),			AVGVOLUME);
	CPPUNIT_ASSERT_EQUAL(p1->getRefAvgVolume(),			AVGVOLUME);

	CPPUNIT_ASSERT_EQUAL(p1->getVolStdDeviation(),				VOLSTDDEVIATION);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefVolStdDeviation(),				VOLSTDDEVIATION);
	CPPUNIT_ASSERT_EQUAL(p1->getRefVolStdDeviation(),				VOLSTDDEVIATION);

	CPPUNIT_ASSERT_EQUAL(p1->getVolPctConfidence(),		VOLPCTCONFIDENCE);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefVolPctConfidence(),		VOLPCTCONFIDENCE);
	CPPUNIT_ASSERT_EQUAL(p1->getRefVolPctConfidence(),		VOLPCTCONFIDENCE);

	CPPUNIT_ASSERT_EQUAL(p1->getReconstructionTime(),		CREFRECONSTRUCTIONTIME);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefReconstructionTime(),		CREFRECONSTRUCTIONTIME);
	CPPUNIT_ASSERT_EQUAL(p1->getRefReconstructionTime(),		CREFRECONSTRUCTIONTIME);

	CPPUNIT_ASSERT_EQUAL(p1->getMaskGenerated(),		MASKGENERATED);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefMaskGenerated(),		MASKGENERATED);
	CPPUNIT_ASSERT_EQUAL(p1->getRefMaskGenerated(),		MASKGENERATED);

	CPPUNIT_ASSERT_EQUAL(p1->getLevel(),			LEVEL ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefLevel(),			LEVEL ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefLevel(),			LEVEL ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getLabel(),			LABEL ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefLabel(),			LABEL ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefLabel(),			LABEL ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getGenerated3D(),			GENERATED3D ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefGenerated3D(),			GENERATED3D ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefGenerated3D(),			GENERATED3D ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getType3D(),			CREFTYPE3D ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefType3D(),			CREFTYPE3D ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefType3D(),			CREFTYPE3D ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getPath(),			CREFPATH ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefPath(),			CREFPATH ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefPath(),			CREFPATH ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getDbID(),			DBID ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefDbID(),			DBID ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefDbID(),			DBID ) ;
}

//------------------------------------------------------------------------------

void ReconstructionTest::methode2()
{
	const bool ISVISIBLE		= true ;
	const double RED			= 24.65 ;
	const double GREEN			= 24.65 ;
	const double BLUE			= 24.65 ;
	const double ALPHA			= 12.80 ;
	const std::string CREFRECONSTRUCTIONFORMAT		= "ReconstructionFormat" ;
	const std::string CREFORGANNAME		= "OrganName" ;
	const std::string CREFSTRUCTURETYPE		= "StructureType" ;
	const bool ISCLOSED		= true ;
	const bool ISAUTOMATIC		= true ;
	const double AVGVOLUME		= 48.94 ;
	const double VOLSTDDEVIATION		= 32.02 ;
	const double VOLPCTCONFIDENCE		= 10.10 ;
	const std::string CREFRECONSTRUCTIONTIME		= "ReconstructionTime" ;
	const bool MASKGENERATED		= true ;
	const ::boost::uint8_t LEVEL		= 2 ;
	const ::boost::uint16_t LABEL		= 4 ;
	const bool GENERATED3D		= true ;
	const std::string CREFTYPE3D		= "CRefType3D" ;
	const ::boost::filesystem::path CREFPATH	=  ::boost::filesystem::path("C:/TMP") ;
	const ::boost::int32_t DBID		= 5 ;


	// process
	::fwData::Reconstruction::NewSptr p1;


	p1->setCRefIsVisible(					ISVISIBLE);
	p1->setCRefReconstructionFormat(					CREFRECONSTRUCTIONFORMAT);
	p1->setCRefOrganName(					CREFORGANNAME);
	p1->setCRefStructureType(					CREFSTRUCTURETYPE);
	p1->setCRefIsClosed(				ISCLOSED	);
	p1->setCRefIsAutomatic(					ISAUTOMATIC);
	p1->setCRefAvgVolume(					AVGVOLUME);
	p1->setCRefVolStdDeviation(					VOLSTDDEVIATION);
	p1->setCRefVolPctConfidence(					VOLPCTCONFIDENCE);
	p1->setCRefReconstructionTime(					CREFRECONSTRUCTIONTIME);
	p1->setCRefMaskGenerated(					MASKGENERATED);
	p1->setCRefLevel(					LEVEL);
	p1->setCRefLabel(					LABEL);
	p1->setCRefGenerated3D(					GENERATED3D);
	p1->setCRefType3D(					CREFTYPE3D);
	p1->setCRefPath(					CREFPATH);
	p1->setCRefDbID(					DBID);

	// check
	CPPUNIT_ASSERT_EQUAL(p1->getIsVisible(),			ISVISIBLE);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIsVisible(),			ISVISIBLE);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIsVisible(),			ISVISIBLE);

	CPPUNIT_ASSERT_EQUAL(p1->getReconstructionFormat(),		CREFRECONSTRUCTIONFORMAT);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefReconstructionFormat(),		CREFRECONSTRUCTIONFORMAT);
	CPPUNIT_ASSERT_EQUAL(p1->getRefReconstructionFormat(),		CREFRECONSTRUCTIONFORMAT);

	CPPUNIT_ASSERT_EQUAL(p1->getOrganName(),		CREFORGANNAME);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefOrganName(),		CREFORGANNAME);
	CPPUNIT_ASSERT_EQUAL(p1->getRefOrganName(),		CREFORGANNAME);

	CPPUNIT_ASSERT_EQUAL(p1->getStructureType(),		CREFSTRUCTURETYPE);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefStructureType(),		CREFSTRUCTURETYPE);
	CPPUNIT_ASSERT_EQUAL(p1->getRefStructureType(),		CREFSTRUCTURETYPE);

	CPPUNIT_ASSERT_EQUAL(p1->getIsClosed(),		ISCLOSED);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIsClosed(),		ISCLOSED);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIsClosed(),		ISCLOSED);

	CPPUNIT_ASSERT_EQUAL(p1->getIsAutomatic(),				ISAUTOMATIC);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIsAutomatic(),				ISAUTOMATIC);
	CPPUNIT_ASSERT_EQUAL(p1->getRefIsAutomatic(),				ISAUTOMATIC);

	CPPUNIT_ASSERT_EQUAL(p1->getAvgVolume(),			AVGVOLUME);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefAvgVolume(),			AVGVOLUME);
	CPPUNIT_ASSERT_EQUAL(p1->getRefAvgVolume(),			AVGVOLUME);

	CPPUNIT_ASSERT_EQUAL(p1->getVolStdDeviation(),				VOLSTDDEVIATION);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefVolStdDeviation(),				VOLSTDDEVIATION);
	CPPUNIT_ASSERT_EQUAL(p1->getRefVolStdDeviation(),				VOLSTDDEVIATION);

	CPPUNIT_ASSERT_EQUAL(p1->getVolPctConfidence(),		VOLPCTCONFIDENCE);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefVolPctConfidence(),		VOLPCTCONFIDENCE);
	CPPUNIT_ASSERT_EQUAL(p1->getRefVolPctConfidence(),		VOLPCTCONFIDENCE);

	CPPUNIT_ASSERT_EQUAL(p1->getReconstructionTime(),		CREFRECONSTRUCTIONTIME);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefReconstructionTime(),		CREFRECONSTRUCTIONTIME);
	CPPUNIT_ASSERT_EQUAL(p1->getRefReconstructionTime(),		CREFRECONSTRUCTIONTIME);

	CPPUNIT_ASSERT_EQUAL(p1->getMaskGenerated(),		MASKGENERATED);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefMaskGenerated(),		MASKGENERATED);
	CPPUNIT_ASSERT_EQUAL(p1->getRefMaskGenerated(),		MASKGENERATED);

	CPPUNIT_ASSERT_EQUAL(p1->getLevel(),			LEVEL ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefLevel(),			LEVEL ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefLevel(),			LEVEL ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getLabel(),			LABEL ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefLabel(),			LABEL ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefLabel(),			LABEL ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getGenerated3D(),			GENERATED3D ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefGenerated3D(),			GENERATED3D ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefGenerated3D(),			GENERATED3D ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getType3D(),			CREFTYPE3D ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefType3D(),			CREFTYPE3D ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefType3D(),			CREFTYPE3D ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getPath(),			CREFPATH ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefPath(),			CREFPATH ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefPath(),			CREFPATH ) ;

	CPPUNIT_ASSERT_EQUAL(p1->getDbID(),			DBID ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getCRefDbID(),			DBID ) ;
	CPPUNIT_ASSERT_EQUAL(p1->getRefDbID(),			DBID ) ;
}

//------------------------------------------------------------------------------

void ReconstructionTest::image()
{
	::fwData::Reconstruction::NewSptr p1;
	::fwData::Image::sptr i1(new ::fwData::Image) ;

	p1->setImage(i1) ;
	CPPUNIT_ASSERT_EQUAL(p1->getImage(), i1 ) ;

}


