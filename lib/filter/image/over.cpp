/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "over.hpp"

#include <core/compare.hpp>
#include <core/tools/dispatcher.hpp>

#include <io/itk/itk.hpp>

#include <itkBinaryGeneratorImageFilter.h>
#include <itkImageDuplicator.h>

namespace sight::filter::image
{

//-----------------------------------------------------------------------------

void over(
    const std::vector<data::image::csptr>& _images,
    data::image& _output_image
)
{
    SIGHT_THROW_IF("At least one input image is required.", _images.empty());
    SIGHT_THROW_IF("The first input image is null.", !_images.front());

    const auto& reference = *_images.front();
    SIGHT_THROW_IF("Only 3D images are supported.", reference.num_dimensions() != 3);

    for(std::size_t i = 1 ; i < _images.size() ; ++i)
    {
        SIGHT_THROW_IF("An input image is null.", !_images[i]);
        const auto& image = *_images[i];

        SIGHT_THROW_IF("All input images must be 3D.", image.num_dimensions() != 3);
        SIGHT_THROW_IF("All input images must have the same pixel type.", image.type() != reference.type());
        SIGHT_THROW_IF("All input images must have the same size.", image.size() != reference.size());
        SIGHT_THROW_IF(
            "All input images must have the same pixel format.",
            image.pixel_format() != reference.pixel_format()
        );
        SIGHT_THROW_IF(
            "All input images must have the same spacing.",
            !core::is_equal(
                image.spacing(),
                reference.spacing()
            )
        );
        SIGHT_THROW_IF(
            "All input images must have the same origin.",
            !core::is_equal(image.origin(), reference.origin())
        );
        SIGHT_THROW_IF(
            "All input images must have the same orientation.",
            !core::is_equal(image.orientation(), reference.orientation())
        );
    }

    auto apply_over =
        []<class PIXEL_TYPE>(
            const std::vector<data::image::csptr>& _images,
            data::image& _output_image)
        {
            constexpr unsigned int dimension = 3;
            using image_t = itk::Image<PIXEL_TYPE, dimension>;

            auto accumulated = io::itk::move_to_itk<image_t>(*_images.front());
            for(std::size_t i = 1 ; i < _images.size() ; ++i)
            {
                auto next        = io::itk::move_to_itk<image_t>(*_images[i]);
                auto over_filter = itk::BinaryGeneratorImageFilter<image_t, image_t, image_t>::New();
                over_filter->SetInput1(accumulated);
                over_filter->SetInput2(next);
                over_filter->SetFunctor(
                    [](const PIXEL_TYPE& _previous, const PIXEL_TYPE& _next)
                {
                    return _next != PIXEL_TYPE(0) ? _next : _previous;
                });
                over_filter->InPlaceOff();
                over_filter->Update();

                accumulated = over_filter->GetOutput();
                accumulated->DisconnectPipeline();
            }

            if(_images.size() == 1)
            {
                auto duplicator = itk::ImageDuplicator<image_t>::New();
                duplicator->SetInputImage(accumulated);
                duplicator->Update();
                accumulated = duplicator->GetOutput();
                accumulated->DisconnectPipeline();
            }

            io::itk::move_from_itk<image_t>(accumulated, _output_image);
        };

    core::tools::dispatcher<core::tools::supported_dispatcher_types, decltype(apply_over)>::invoke(
        reference.type(),
        _images,
        _output_image
    );
}

//-----------------------------------------------------------------------------

} // namespace sight::filter::image
