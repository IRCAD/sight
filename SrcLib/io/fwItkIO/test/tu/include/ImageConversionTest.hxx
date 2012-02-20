/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataTools/Image.hpp>

#include <itkIO/itk.hpp>

template< class TYPE>
void ImageConversionTest::stressTestForAType()
{
    for(unsigned char k=0; k<5; k++)
    {
        ::fwData::Image::NewSptr image;
        ::fwDataTools::Image::generateRandomImage(image, ::fwTools::Type::create<TYPE>());

        typedef itk::Image< TYPE , 3 > ImageType;
        typename ImageType::Pointer itkImage = ::itkIO::itkImageFactory<ImageType>( image );

        ::fwData::Image::NewSptr image2;
        bool image2ManagesHisBuffer = false;
        ::itkIO::dataImageFactory< ImageType >( itkImage, image2, image2ManagesHisBuffer );
        CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image2));


        bool image3ManagesHisBuffer = false;
        ::fwData::Image::sptr image3 = ::itkIO::dataImageFactory< ImageType >( itkImage, image3ManagesHisBuffer );
        CPPUNIT_ASSERT(::fwDataTools::Image::compareImage(image, image3));
    }
}
