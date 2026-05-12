/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <sight/utest_data/config.hpp>

#include <data/image.hpp>

#include <map>
#include <string>

namespace sight::utest_data::image
{

static bool sight_utest_data_image_debug = false;

//------------------------------------------------------------------------------

/** @brief Debug function to print image content to the standard output.
 * This is meant to help debugging unit tests with very small test images.
 * It can output any pixel type as long as a translation map is provided.
 * Example of output for a 6x4 image with pixel values 0 and 80:
 *
 * xxxOOO
 * xxOOOO
 * xOOOOO
 * OOOOOO
 *
 * By default the output of this function is disabled.
 * To enable it, set the variable SIGHT_UTEST_DATA_IMAGE_DEBUG to true.
 *
 * @tparam T1 Pixel type of the image.
 * @tparam T2 Type of the translated value.
 * @param _image Image to print.
 * @param _translator Map translating pixel values to strings for display, i.e. {{0, 'x'}, {80, 'o'}}.
 */
template<class T1, class T2>
void cout_debug(sight::data::image& _image, const std::map<T1, T2>& _translator)
{
    if(not sight_utest_data_image_debug)
    {
        return;
    }

    std::cout << "Image debug: " << _image.get_id() << std::endl;
    std::cout << std::endl;

    const auto size = _image.size();
    auto it         = _image.cbegin<T1>();
    for(std::size_t k = 0 ; k < size[2] ; k++)
    {
        for(std::size_t j = 0 ; j < size[1] ; j++)
        {
            for(std::size_t i = 0 ; i < size[0] ; i++)
            {
                std::cout << _translator.at(*it);
                ++it;
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }
}

} // namespace sight::utest_data::image
