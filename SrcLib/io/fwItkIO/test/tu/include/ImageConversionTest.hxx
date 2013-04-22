/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/helper/compare.hpp>

#include <itkIO/itk.hpp>

namespace fwItkIO
{
namespace ut
{

//-----------------------------------------------------------------------------

template< class TYPE>
void ImageConversionTest::stressTestForAType()
{
    for(unsigned char k=0; k<5; k++)
    {
        ::fwData::Image::sptr image = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create<TYPE>());

        typedef itk::Image< TYPE , 3 > ImageType;
        typename ImageType::Pointer itkImage = ::itkIO::itkImageFactory<ImageType>( image );

        ::fwData::Image::NewSptr image2;
        bool image2ManagesHisBuffer = false;
        ::itkIO::dataImageFactory< ImageType >( itkImage, image2, image2ManagesHisBuffer );
        CPPUNIT_ASSERT(::fwTest::helper::compare(image, image2));


        bool image3ManagesHisBuffer = false;
        ::fwData::Image::sptr image3 = ::itkIO::dataImageFactory< ImageType >( itkImage, image3ManagesHisBuffer );
        CPPUNIT_ASSERT(::fwTest::helper::compare(image, image3));
    }
}

} //namespace ut
} //namespace fwItkIO
