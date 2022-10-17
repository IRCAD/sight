/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#pragma once

#include <io/itk/itk.hpp>

#include <utestData/generator/Image.hpp>

namespace sight::io::itk::ut
{

//-----------------------------------------------------------------------------

template<class TYPE>
void ImageConversionTest::stressTestForAType()
{
    for(unsigned char k = 0 ; k < 5 ; k++)
    {
        data::Image::sptr image = data::Image::New();
        utestData::generator::Image::generateRandomImage(image, core::Type::get<TYPE>());

        typedef ::itk::Image<TYPE, 3> ImageType;
        typename ImageType::Pointer itkImage = io::itk::moveToItk<ImageType>(image);

        data::Image::sptr image2 = data::Image::New();
        io::itk::moveFromItk<ImageType>(itkImage, image2, false);

        image2->setWindowCenter(image->getWindowCenter());
        image2->setWindowWidth(image->getWindowWidth());

        CPPUNIT_ASSERT(*image == *image2);

        data::Image::sptr image3 = io::itk::moveFromItk<ImageType>(itkImage, false);
        image3->setWindowCenter(image->getWindowCenter());
        image3->setWindowWidth(image->getWindowWidth());

        CPPUNIT_ASSERT(*image == *image3);
    }
}

} // namespace sight::io::itk::ut
