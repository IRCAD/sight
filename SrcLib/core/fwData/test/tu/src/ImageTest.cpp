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
    ::fwTools::Type TYPE1 = ::fwTools::Type::create("int16")  ;
    ::fwTools::Type TYPE2 = ::fwTools::Type::create("int64")  ;
    ::fwTools::Type TYPE3 = ::fwTools::Type::create("uint8")  ;
    ::fwData::Image::SizeType VECTORSIZE1(DIMENSION, 10)  ;
    ::fwData::Image::SizeType VECTORSIZE2(DIMENSION, 20)  ;
    ::fwData::Image::SizeType VECTORSIZE3(DIMENSION, 5)  ;
    size_t SIZE1 = 10*10*10*TYPE1.sizeOf();
    size_t SIZE2 = 20*20*20*TYPE2.sizeOf();
    size_t SIZE3 = 5*5*5*TYPE3.sizeOf();

    // process
    ::fwData::Image::NewSptr img1;

    img1->allocate(VECTORSIZE1, TYPE1);
    ::fwData::Array::sptr array = img1->getDataArray();
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE1);
    CPPUNIT_ASSERT(array->getType() == TYPE1);
    CPPUNIT_ASSERT(array->getSizeInBytes() == SIZE1);
    CPPUNIT_ASSERT(img1->getSizeInBytes() == SIZE1);

    img1->allocate(VECTORSIZE2, TYPE2);
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE2);
    CPPUNIT_ASSERT(array->getType() == TYPE2);
    CPPUNIT_ASSERT(array->getSizeInBytes() == SIZE2);
    CPPUNIT_ASSERT(img1->getSizeInBytes() == SIZE2);

    img1->allocate(VECTORSIZE3, TYPE3);
    CPPUNIT_ASSERT(array->getSize() == VECTORSIZE3);
    CPPUNIT_ASSERT(array->getType() == TYPE3);
    CPPUNIT_ASSERT(array->getSizeInBytes() == SIZE3);
    CPPUNIT_ASSERT(img1->getSizeInBytes() == SIZE3);
}

//------------------------------------------------------------------------------

void ImageTest::testPixelType()
{
    ::fwData::Image::NewSptr img1;
    ::fwTools::DynamicType DT1;

    img1->setType(::fwTools::Type::create("int8"));
    DT1.setType< signed char >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("int16"));
    DT1.setType< signed short >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("int32"));
    DT1.setType< signed int >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("uint8"));
    DT1.setType< unsigned char >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("uint16"));
    DT1.setType< unsigned short >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("uint32"));
    DT1.setType< unsigned int >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("float"));
    DT1.setType< float >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());

    img1->setType(::fwTools::Type::create("double"));
    DT1.setType< double >();
    CPPUNIT_ASSERT(DT1 == img1->getPixelType());
}

//------------------------------------------------------------------------------

void ImageTest::testSetGetPixel()
{
    ::fwData::Image::NewSptr img;
    const ::boost::uint8_t DIMENSION = 3 ;
    ::fwTools::Type TYPE = ::fwTools::Type::create("int16")  ;
    ::fwData::Image::SizeType VECTORSIZE(DIMENSION)  ;
    VECTORSIZE[0] = 10;
    VECTORSIZE[1] = 20;
    VECTORSIZE[2] = 30;

    img->allocate(VECTORSIZE, TYPE);

    ::fwData::Array::sptr array = img->getDataArray();

    // test 1 : use getPixelBuffer
    short count = 0;
    short *iter = array->begin<short>();
    for (; iter != array->end<short>() ; ++iter)
    {
        *iter = count++;
    }

    for (unsigned int x=0 ; x<VECTORSIZE[0] ; ++x)
    {
        for (unsigned int y=0 ; y<VECTORSIZE[1] ; ++y)
        {
            for (unsigned int z=0 ; z<VECTORSIZE[2] ; ++z)
            {
                short val = static_cast<short>(x+y*VECTORSIZE[0]+z*VECTORSIZE[0]*VECTORSIZE[1]);
                ::fwData::Image::IndexType index = val;
                CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<short*>(img->getPixelBuffer(x,y,z)));
                CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<short*>(img->getPixelBuffer(index)));
                CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<short*>(::fwData::Image::getPixelBuffer(reinterpret_cast< ::fwData::Image::BufferType* >(img->getBuffer()), index, img->getType().sizeOf())));
                CPPUNIT_ASSERT_EQUAL(val, *reinterpret_cast<short*>(img->getPixelBufferCopy(index).get()));
            }
        }
    }

    // test 2 : use setPixelBuffer
    for (unsigned int x=0 ; x<VECTORSIZE[0] ; ++x)
    {
        for (unsigned int y=0 ; y<VECTORSIZE[1] ; ++y)
        {
            for (unsigned int z=0 ; z<VECTORSIZE[2] ; ++z)
            {
                ::fwData::Image::IndexType index = x+y*VECTORSIZE[0]+z*VECTORSIZE[0]*VECTORSIZE[1];
                short val = static_cast<short>(index * 2);
                if(x%2 == 0) // to test different setters
                {
                    img->setPixelBuffer(index, reinterpret_cast< ::fwData::Image::BufferType* >(&val));
                }
                else
                {
                    ::fwData::Image::setPixelBuffer(reinterpret_cast< ::fwData::Image::BufferType* >(img->getBuffer()),
                            reinterpret_cast< ::fwData::Image::BufferType* >(&val),
                            index, img->getType().sizeOf());
                }
            }
        }
    }

    count = 0;
    iter = array->begin<short>();
    for (; iter != array->end<short>() ; ++iter)
    {
        CPPUNIT_ASSERT_EQUAL(static_cast<short>(count++ *2), *iter);
    }
}

//------------------------------------------------------------------------------

