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


#include "fwData/Study.hpp"
#include "fwData/Patient.hpp"
#include "fwData/Acquisition.hpp"
#include "fwData/Image.hpp"
#include "fwData/Reconstruction.hpp"

#include "ImageTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ImageTest );

void ImageTest::setUp()
{
    // Set up context before running a test.
}

void ImageTest::tearDown()
{
    // Clean up after the test run.
}


void ImageTest::methode1() //test des setters et getters
{
    const ::boost::uint8_t DIMENSION = 2 ;
    //const fwTools::DynamicType PIXELTYPE =  4 ;
    const bool MANAGESBUFF= true ;
    double CREFSPACING = 2.5 ;
    std::vector<double> VECTORCREFSPACING ;
    VECTORCREFSPACING.push_back( CREFSPACING ) ;
    double CREFORIGIN = 2.7 ;
    std::vector<double> VECTORCREFORIGIN     ;
    VECTORCREFORIGIN.push_back( CREFORIGIN ) ;
    ::boost::int32_t CREFSIZE = 42 ;
    std::vector< ::boost::int32_t > VECTORCREFSIZE  ;
    VECTORCREFSIZE.push_back( CREFSIZE ) ;
    const ::boost::filesystem::path CREFFILENAME    = ::boost::filesystem::path("C:/TMP") ;
    const double WINDOWCENTER = 10.10 ;
    const double WINDOWWIDTH = 11.34 ;
    const double RESCALEINTERCEPT = 0.75 ;



    // process
    ::fwData::Image::NewSptr p1;


    p1->setDimension(DIMENSION);
    //p1->setPixelType( PIXELTYPE);
    p1->setManagesBuff( MANAGESBUFF);
    p1->setCRefSpacing(VECTORCREFSPACING);
    p1->setCRefOrigin(VECTORCREFORIGIN);
    p1->setCRefSize(VECTORCREFSIZE);
    p1->setCRefFilename(CREFFILENAME);
    p1->setWindowCenter(WINDOWCENTER);
    p1->setWindowWidth( WINDOWWIDTH );
    p1->setRescaleIntercept(RESCALEINTERCEPT);


    // check
    CPPUNIT_ASSERT_EQUAL(p1->getDimension(),DIMENSION);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefDimension(),DIMENSION);
    CPPUNIT_ASSERT_EQUAL(p1->getRefDimension(),DIMENSION);

    /*
        CPPUNIT_ASSERT_EQUAL(p1->getPixelType(),        PIXELTYPE);
        CPPUNIT_ASSERT_EQUAL(p1->getCRefPixelType(),        PIXELTYPE);
        CPPUNIT_ASSERT_EQUAL(p1->getRefPixelType(),     PIXELTYPE);
     */

    CPPUNIT_ASSERT_EQUAL(p1->getManagesBuff(),MANAGESBUFF);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefManagesBuff(),MANAGESBUFF);
    CPPUNIT_ASSERT_EQUAL(p1->getRefManagesBuff(),MANAGESBUFF);

    CPPUNIT_ASSERT_EQUAL(p1->getSpacing().back(),CREFSPACING);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefSpacing().back(),CREFSPACING);
    CPPUNIT_ASSERT_EQUAL(p1->getRefSpacing().back(),CREFSPACING);

    CPPUNIT_ASSERT_EQUAL(p1->getOrigin().back(),CREFORIGIN);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefOrigin().back(),CREFORIGIN);
    CPPUNIT_ASSERT_EQUAL(p1->getRefOrigin().back(),CREFORIGIN);

    CPPUNIT_ASSERT_EQUAL(p1->getSize().back(),CREFSIZE);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefSize().back(),CREFSIZE);
    CPPUNIT_ASSERT_EQUAL(p1->getRefSize().back(),CREFSIZE);

    CPPUNIT_ASSERT_EQUAL(p1->getFilename(),CREFFILENAME);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefFilename(),CREFFILENAME);
    CPPUNIT_ASSERT_EQUAL(p1->getRefFilename(),CREFFILENAME);

    CPPUNIT_ASSERT_EQUAL(p1->getWindowCenter(),WINDOWCENTER);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefWindowCenter(),WINDOWCENTER);
    CPPUNIT_ASSERT_EQUAL(p1->getRefWindowCenter(),WINDOWCENTER);

    CPPUNIT_ASSERT_EQUAL(p1->getWindowWidth(),WINDOWWIDTH);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefWindowWidth(),WINDOWWIDTH);
    CPPUNIT_ASSERT_EQUAL(p1->getRefWindowWidth(),WINDOWWIDTH);

    CPPUNIT_ASSERT_EQUAL(p1->getRescaleIntercept(),     RESCALEINTERCEPT);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefRescaleIntercept(),     RESCALEINTERCEPT);
    CPPUNIT_ASSERT_EQUAL(p1->getRefRescaleIntercept(),      RESCALEINTERCEPT);

}


void ImageTest::methode2()
{
    const ::boost::uint8_t DIMENSION        = 2 ;
    //const fwTools::DynamicType PIXELTYPE =  4 ;
    const bool MANAGESBUFF  = false ;
    double CREFSPACING = 2.5 ;
    std::vector<double> VECTORCREFSPACING ;
    VECTORCREFSPACING.push_back( CREFSPACING ) ;
    double CREFORIGIN = 2.7 ;
    std::vector<double> VECTORCREFORIGIN     ;
    VECTORCREFORIGIN.push_back( CREFORIGIN ) ;
    ::boost::int32_t CREFSIZE = 42 ;
    std::vector< ::boost::int32_t > VECTORCREFSIZE  ;
    VECTORCREFSIZE.push_back( CREFSIZE ) ;
    const ::boost::filesystem::path CREFFILENAME = ::boost::filesystem::path("C:/TMP") ;
    const double WINDOWCENTER = 10.10 ;
    const double WINDOWWIDTH = 11.34 ;
    const double RESCALEINTERCEPT = 0.75 ;



    // process
    ::fwData::Image::NewSptr p1;


    p1->setCRefDimension(DIMENSION);
    //p1->setCRefPixelType( PIXELTYPE);
    p1->setCRefManagesBuff( MANAGESBUFF);
    p1->setCRefSpacing(VECTORCREFSPACING);
    p1->setCRefOrigin(VECTORCREFORIGIN);
    p1->setCRefSize(VECTORCREFSIZE);
    p1->setCRefFilename(CREFFILENAME);
    p1->setCRefWindowCenter(WINDOWCENTER);
    p1->setCRefWindowWidth( WINDOWWIDTH );
    p1->setCRefRescaleIntercept(RESCALEINTERCEPT);


    // check
    CPPUNIT_ASSERT_EQUAL(p1->getDimension(),DIMENSION);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefDimension(),DIMENSION);
    CPPUNIT_ASSERT_EQUAL(p1->getRefDimension(),DIMENSION);

    /*
        CPPUNIT_ASSERT_EQUAL(p1->getPixelType(),PIXELTYPE);
        CPPUNIT_ASSERT_EQUAL(p1->getCRefPixelType(),PIXELTYPE);
        CPPUNIT_ASSERT_EQUAL(p1->getRefPixelType(),PIXELTYPE);
     */

    CPPUNIT_ASSERT_EQUAL(p1->getManagesBuff(),MANAGESBUFF);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefManagesBuff(),MANAGESBUFF);
    CPPUNIT_ASSERT_EQUAL(p1->getRefManagesBuff(),MANAGESBUFF);

    CPPUNIT_ASSERT_EQUAL(p1->getSpacing().back(),CREFSPACING);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefSpacing().back(),CREFSPACING);
    CPPUNIT_ASSERT_EQUAL(p1->getRefSpacing().back(),CREFSPACING);

    CPPUNIT_ASSERT_EQUAL(p1->getOrigin().back(),CREFORIGIN);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefOrigin().back(),CREFORIGIN);
    CPPUNIT_ASSERT_EQUAL(p1->getRefOrigin().back(),CREFORIGIN);

    CPPUNIT_ASSERT_EQUAL(p1->getSize().back(),CREFSIZE);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefSize().back(),CREFSIZE);
    CPPUNIT_ASSERT_EQUAL(p1->getRefSize().back(),CREFSIZE);

    CPPUNIT_ASSERT_EQUAL(p1->getFilename(),CREFFILENAME);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefFilename(),CREFFILENAME);
    CPPUNIT_ASSERT_EQUAL(p1->getRefFilename(),CREFFILENAME);

    CPPUNIT_ASSERT_EQUAL(p1->getWindowCenter(),WINDOWCENTER);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefWindowCenter(),WINDOWCENTER);
    CPPUNIT_ASSERT_EQUAL(p1->getRefWindowCenter(),WINDOWCENTER);

    CPPUNIT_ASSERT_EQUAL(p1->getWindowWidth(),WINDOWWIDTH);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefWindowWidth(),WINDOWWIDTH);
    CPPUNIT_ASSERT_EQUAL(p1->getRefWindowWidth(),WINDOWWIDTH);

    CPPUNIT_ASSERT_EQUAL(p1->getRescaleIntercept(),RESCALEINTERCEPT);
    CPPUNIT_ASSERT_EQUAL(p1->getCRefRescaleIntercept(),RESCALEINTERCEPT);
    CPPUNIT_ASSERT_EQUAL(p1->getRefRescaleIntercept(),RESCALEINTERCEPT);

}

/*
void ImageTest::Buffer()
{
    ::boost::shared_ptr< ::fwData::Image> i1 (::fwData::Image::New());

    void * BUFFER = (void *)43 ;

    i1->setBuffer( BUFFER ) ;
    CPPUNIT_ASSERT_EQUAL( i1->getBuffer(), BUFFER ) ;
}
 */
