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

#include <core/type.hpp>
#include <data/image.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <type_traits>

namespace sight::utest_data::image
{

static bool sight_utest_data_image_debug = false;

struct comparison_metrics
{
    double mean_intensity_difference {0.};
    std::size_t changed_voxels {0};
    double reference_total_variation {0.};
    double compared_total_variation {0.};
};

//------------------------------------------------------------------------------

/**
 * Compares the intensity mean, changed voxels, and total spatial variation of two scalar images.
 * @tparam pixel_type Scalar pixel type stored in both images.
 * @param _reference Reference image.
 * @param _compared Image to compare with the reference.
 * @throws std::invalid_argument if the images have different sizes or are not scalar images of pixel_type.
 */
template<typename pixel_type>
comparison_metrics compare(const sight::data::image& _reference, const sight::data::image& _compared)
{
    static_assert(std::is_arithmetic_v<pixel_type>);

    if(_reference.size() != _compared.size())
    {
        throw std::invalid_argument("Cannot compare images with different sizes.");
    }

    if(_reference.type() != sight::core::type::get<pixel_type>()
       or _compared.type() != sight::core::type::get<pixel_type>()
       or _reference.num_components() != 1
       or _compared.num_components() != 1)
    {
        throw std::invalid_argument("Cannot compare images with different or non-scalar pixel types.");
    }

    const auto& size = _reference.size();
    const auto depth = size[2] == 0 ? std::size_t {1} : size[2];
    if(size[0] == 0 or size[1] == 0)
    {
        throw std::invalid_argument("Cannot compare empty images.");
    }

    [[maybe_unused]] const auto reference_lock = _reference.dump_lock();
    [[maybe_unused]] const auto compared_lock  =
        &_reference == &_compared ? decltype(reference_lock) {} : _compared.dump_lock();

    const auto reference = _reference.cbegin<pixel_type>();
    const auto compared  = _compared.cbegin<pixel_type>();

    comparison_metrics metrics;
    double reference_sum = 0.;
    double compared_sum  = 0.;

    for(std::size_t z = 0 ; z < depth ; ++z)
    {
        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const auto index           = x + y * size[0] + z * size[0] * size[1];
                const auto reference_value = static_cast<double>(reference[index]);
                const auto compared_value  = static_cast<double>(compared[index]);

                reference_sum          += reference_value;
                compared_sum           += compared_value;
                metrics.changed_voxels += reference_value != compared_value ? 1 : 0;

                const auto accumulate_variation = [&](std::size_t _neighbour_index)
                                                  {
                                                      metrics.reference_total_variation += std::abs(
                                                          reference_value
                                                          - static_cast<double>(reference[_neighbour_index])
                                                      );
                                                      metrics.compared_total_variation += std::abs(
                                                          compared_value
                                                          - static_cast<double>(compared[_neighbour_index])
                                                      );
                                                  };

                if(x + 1 < size[0])
                {
                    accumulate_variation(index + 1);
                }

                if(y + 1 < size[1])
                {
                    accumulate_variation(index + size[0]);
                }

                if(z + 1 < depth)
                {
                    accumulate_variation(index + size[0] * size[1]);
                }
            }
        }
    }

    const auto voxel_count = static_cast<double>(size[0] * size[1] * depth);
    metrics.mean_intensity_difference = std::abs(reference_sum - compared_sum) / voxel_count;
    return metrics;
}

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
