/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDataCamp/visitor/CompareObjects.hpp>

#include <fwTest/generator/Image.hpp>
#include <fwTest/helper/compare.hpp>

#include <fwItkIO/itk.hpp>

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
        typename ImageType::Pointer itkImage = ::fwItkIO::itkImageFactory<ImageType>( image );

        ::fwTest::helper::ExcludeSetType exclude;
        exclude.insert("array.isOwner");
        exclude.insert("window_center");
        exclude.insert("window_width");

        ::fwData::Image::sptr image2 = ::fwData::Image::New();
        bool image2ManagesHisBuffer = false;
        ::fwItkIO::dataImageFactory< ImageType >( itkImage, image2, image2ManagesHisBuffer );
        CPPUNIT_ASSERT(::fwTest::helper::compare(image, image2, exclude));

        bool image3ManagesHisBuffer = false;
        ::fwData::Image::sptr image3 = ::fwItkIO::dataImageFactory< ImageType >( itkImage, image3ManagesHisBuffer );
        CPPUNIT_ASSERT(::fwTest::helper::compare(image, image3, exclude));
    }
}

} //namespace ut
} //namespace fwItkIO
