/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/DictionaryOrgan.hpp>

#include "DictionaryOrganTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( DictionaryOrganTest );

void DictionaryOrganTest::setUp()
{
	// Set up context before running a test.

}
void DictionaryOrganTest::tearDown()
{
	// Clean up after the test run.
}

void DictionaryOrganTest::methode1()
{
	const std::string CREFSTRUCTURETYPE	= "CRefStructureType" ;
	const std::string CREFSALABEL	= "CRefSALabel" ;
	const bool ISMEDICALSTRUCTURE	= true ;
	const ::boost::uint32_t AOCOLOR	= 10 ;
	const double AVGVOLUME	= 12.23 ;
	const double VOLSTDDEVIATION	= 21.01 ;
	const ::boost::uint32_t  NBEXAMS	= 9 ;
	const std::string CREFPOSITION	= "crefposition" ;
	const ::boost::filesystem::path CREFICONPATH	= ::boost::filesystem::path("C:/TMP") ;
	const double RED	= 23.17 ;
	const double GREEN	= 12.88 ;
	const double BLUE	= 4.002 ;
	const double ALPHA	= 35.06 ;
	const ::boost::filesystem::path CREFTEXTUREPATH = ::boost::filesystem::path("C:/TMP");
	const ::boost::uint32_t AVGTRIANGLENB	= 5 ;
	const double SEGMIN	= 1.897 ;
	const double SEGMAX	= 2.144 ;
	const std::string CREFMODALITY	= "crefmodality" ;
	const double SURFACEELASTICITY	= 3.455 ;
	const double SURFACERUPTURE	= 3.33333 ;
	const double INTERIORRUPTURE	= 2.22222 ;
	const std::string CREFCOMMENT	= "crefcomment" ;
	std::string CREFBELONGSTO = "crefbelongsto" ;
	std::vector< std::string > VECTORCREFBELONGSTO ;
	VECTORCREFBELONGSTO.push_back(CREFBELONGSTO);

	// process
	::fwData::DictionaryOrgan::NewSptr p1;
	p1->setCRefStructureType(					CREFSTRUCTURETYPE);
	p1->setCRefSALabel(				CREFSALABEL);
	p1->setIsMedicalStructure(					ISMEDICALSTRUCTURE);
	p1->setAOColor(					AOCOLOR);
	p1->setAvgVolume(					AVGVOLUME);
	p1->setVolStdDeviation(					VOLSTDDEVIATION);
	p1->setNbExams(					NBEXAMS);
	p1->setCRefPosition(					CREFPOSITION);
	p1->setCRefIconPath(					CREFICONPATH);
	p1->setRed(					RED);
	p1->setGreen(					GREEN);
	p1->setBlue(					BLUE);
	p1->setAlpha(				ALPHA);
	p1->setCRefTexturePath(				CREFTEXTUREPATH );
	p1->setAvgTriangleNb(					AVGTRIANGLENB);
	p1->setSegMin(					SEGMIN);
	p1->setSegMax(					SEGMAX);
	p1->setCRefModality(					CREFMODALITY);
	p1->setSurfaceElasticity(					SURFACEELASTICITY);
	p1->setSurfaceRupture(					SURFACERUPTURE);
	p1->setInteriorRupture(					INTERIORRUPTURE);
	p1->setCRefComment(					CREFCOMMENT);
	p1->setCRefBelongsTo(					VECTORCREFBELONGSTO);


	// check
	CPPUNIT_ASSERT_EQUAL(p1->getCRefStructureType(),	CREFSTRUCTURETYPE);
	CPPUNIT_ASSERT_EQUAL(p1->getSALabel(),	CREFSALABEL);
	CPPUNIT_ASSERT_EQUAL(p1->getIsMedicalStructure(),	ISMEDICALSTRUCTURE);
	CPPUNIT_ASSERT_EQUAL(p1->getAOColor(),	AOCOLOR);
	CPPUNIT_ASSERT_EQUAL(p1->getAvgVolume(),	AVGVOLUME);
	CPPUNIT_ASSERT_EQUAL(p1->getVolStdDeviation(),	VOLSTDDEVIATION);
	CPPUNIT_ASSERT_EQUAL(p1->getNbExams(),	NBEXAMS);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefPosition(),	CREFPOSITION);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefIconPath(),	CREFICONPATH);
	CPPUNIT_ASSERT_EQUAL(p1->getRed(),	RED);
	CPPUNIT_ASSERT_EQUAL(p1->getGreen(),	GREEN);
	CPPUNIT_ASSERT_EQUAL(p1->getBlue(),	BLUE);
	CPPUNIT_ASSERT_EQUAL(p1->getAlpha(),	ALPHA);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefTexturePath(),	CREFTEXTUREPATH );
	CPPUNIT_ASSERT_EQUAL(p1->getAvgTriangleNb(),	AVGTRIANGLENB);
	CPPUNIT_ASSERT_EQUAL(p1->getSegMin(),	SEGMIN);
	CPPUNIT_ASSERT_EQUAL(p1->getSegMax(),	SEGMAX);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefModality(), CREFMODALITY);
	CPPUNIT_ASSERT_EQUAL(p1->getSurfaceElasticity(),	SURFACEELASTICITY);
	CPPUNIT_ASSERT_EQUAL(p1->getSurfaceRupture(),	SURFACERUPTURE);
	CPPUNIT_ASSERT_EQUAL(p1->getInteriorRupture(), INTERIORRUPTURE);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefComment(),	CREFCOMMENT);
	CPPUNIT_ASSERT_EQUAL(p1->getCRefBelongsTo().back(),	CREFBELONGSTO);


}
