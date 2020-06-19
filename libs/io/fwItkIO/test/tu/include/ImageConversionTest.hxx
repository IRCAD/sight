/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWITKIO_UT_IMAGECONVERSIONTEST_HXX__
#define __FWITKIO_UT_IMAGECONVERSIONTEST_HXX__

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
    for(unsigned char k = 0; k<5; k++)
    {
        ::fwData::Image::sptr image = ::fwData::Image::New();
        ::fwTest::generator::Image::generateRandomImage(image, ::fwTools::Type::create<TYPE>());

        typedef itk::Image< TYPE, 3 > ImageType;
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

#endif // __FWITKIO_UT_IMAGECONVERSIONTEST_HXX__
