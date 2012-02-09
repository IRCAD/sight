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

//------------------------------------------------------------------------------

void ImageTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ImageTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ImageTest::testGetterSetter()
{
    const size_t DIMENSION = 2 ;
    ::fwTools::Type TYPE = ::fwTools::Type::create("int16")  ;
    double CREFSPACING = 2.5 ;
    std::vector<double> VECTORSPACING(DIMENSION, CREFSPACING) ;
    double CREFORIGIN = 2.7 ;
    std::vector<double> VECTORORIGIN(DIMENSION, CREFORIGIN)     ;
    ::boost::int32_t CREFSIZE = 42 ;
    ::fwData::Image::SizeType VECTORSIZE(DIMENSION, CREFSIZE)  ;
    const double WINDOWCENTER = 10.10 ;
    const double WINDOWWIDTH = 11.34 ;



    // process
    ::fwData::Image::NewSptr img1;

    img1->setType( TYPE);
    img1->setSpacing(VECTORSPACING);
    img1->setOrigin(VECTORORIGIN);
    img1->setSize(VECTORSIZE);
    img1->setWindowCenter(WINDOWCENTER);
    img1->setWindowWidth( WINDOWWIDTH );


    // check
    CPPUNIT_ASSERT_EQUAL(img1->getNumberOfDimensions(),DIMENSION);

    CPPUNIT_ASSERT(img1->getType() == TYPE);

    CPPUNIT_ASSERT(img1->getSpacing() == VECTORSPACING);

    CPPUNIT_ASSERT(img1->getOrigin() == VECTORORIGIN);

    CPPUNIT_ASSERT(img1->getSize() == VECTORSIZE);

    CPPUNIT_ASSERT_EQUAL(img1->getWindowCenter(),WINDOWCENTER);

    CPPUNIT_ASSERT_EQUAL(img1->getWindowWidth(),WINDOWWIDTH);

}

//------------------------------------------------------------------------------

void ImageTest::testAllocation()
{
    const ::boost::uint8_t DIMENSION = 3 ;
    ::fwTools::Type TYPE = ::fwTools::Type::create("int16")  ;
    ::fwData::Image::SizeType VECTORSIZE(DIMENSION)  ;
    VECTORSIZE[0] = 14;
    VECTORSIZE[1] = 15;
    VECTORSIZE[2] = 26;
    size_t SIZE = 14*15*26*TYPE.sizeOf();

    // process
    ::fwData::Image::NewSptr img1;

    img1->setType(TYPE);
    img1->setSize(VECTORSIZE);

    img1->allocate();

    ::fwData::Array::sptr array = img1->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT_EQUAL(SIZE, array->getSizeInBytes());
    CPPUNIT_ASSERT_EQUAL(SIZE, img1->getSizeInBytes());



    ::fwData::Image::NewSptr img2;
    img2->allocate(VECTORSIZE[0], VECTORSIZE[1], VECTORSIZE[2], TYPE);

    array = img2->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT(array->getSizeInBytes() == SIZE);
    CPPUNIT_ASSERT(img2->getSizeInBytes() == SIZE);


    ::fwData::Image::NewSptr img3;
    img3->allocate(VECTORSIZE, TYPE);

    array = img3->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE);
    CPPUNIT_ASSERT(array->getType() == TYPE);
    CPPUNIT_ASSERT(array->getSizeInBytes() == SIZE);
    CPPUNIT_ASSERT(img3->getSizeInBytes() == SIZE);
}

//------------------------------------------------------------------------------

void ImageTest::testReallocation()
{
    const ::boost::uint8_t DIMENSION = 3 ;
    ::fwTools::Type TYPE = ::fwTools::Type::create("int16")  ;
    ::fwData::Image::SizeType VECTORSIZE(DIMENSION)  ;
    VECTORSIZE[0] = 14;
    VECTORSIZE[1] = 15;
    VECTORSIZE[2] = 26;
    size_t SIZE = 14*15*26*TYPE.sizeOf();

    // process
    ::fwData::Image::NewSptr img1;

    img1->setType(TYPE);
    img1->setSize(VECTORSIZE);

}

//------------------------------------------------------------------------------

