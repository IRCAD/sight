/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <utest_data/generator/image.hpp>

namespace sight::io::itk::ut
{

//-----------------------------------------------------------------------------

template<class TYPE>
void ImageConversionTest::stressTestForAType()
{
    for(unsigned char k = 0 ; k < 5 ; k++)
    {
        data::image::sptr image = std::make_shared<data::image>();
        utest_data::generator::image::generateRandomImage(image, core::type::get<TYPE>());

        typedef ::itk::Image<TYPE, 3> image_t;
        typename image_t::Pointer itk_image = io::itk::move_to_itk<image_t>(image);

        data::image::sptr image2 = std::make_shared<data::image>();
        io::itk::move_from_itk<image_t>(itk_image, image2, false);

        image2->setWindowCenter(image->getWindowCenter());
        image2->setWindowWidth(image->getWindowWidth());

        CPPUNIT_ASSERT(*image == *image2);

        data::image::sptr image3 = io::itk::move_from_itk<image_t>(itk_image, false);
        image3->setWindowCenter(image->getWindowCenter());
        image3->setWindowWidth(image->getWindowWidth());

        CPPUNIT_ASSERT(*image == *image3);
    }
}

} // namespace sight::io::itk::ut
