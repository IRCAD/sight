/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include "filter/image/detail/filters.hpp"

#include <io/itk/itk.hpp>

#include <itkBinaryBallStructuringElement.h>
#include <itkBinaryFillholeImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkConnectedComponentImageFilter.h>
#include <itkExtractImageFilter.h>
#include <itkFloodFilledImageFunctionConditionalConstIterator.h>
#include <itkGrayscaleMorphologicalClosingImageFilter.h>
#include <itkMedianImageFilter.h>
#include <itkRelabelComponentImageFilter.h>

namespace sight::filter::image::detail
{

//------------------------------------------------------------------------------

template<typename IMAGEINPTR, typename IMAGELABELPTR>
void bug_work_around_labeling(
    IMAGEINPTR _image_in,
    IMAGELABELPTR _labeled,
    typename IMAGEINPTR::ObjectType::PixelType _background
)
{
    itk::ImageRegionIterator<typename IMAGEINPTR::ObjectType> i_it(_image_in, _image_in->GetLargestPossibleRegion());
    itk::ImageRegionIterator<typename IMAGELABELPTR::ObjectType> l_it(_labeled, _labeled->GetLargestPossibleRegion());

    while(!i_it.IsAtEnd())
    {
        if(i_it.Get() == _background)
        {
            l_it.Set(itk::NumericTraits<typename IMAGELABELPTR::ObjectType::PixelType>::Zero);
        }

        ++i_it;
        ++l_it;
    }
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE, unsigned int DIM>
itk::Image<std::uint8_t, DIM>::Pointer threshold(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    IMAGE_TYPE _lower_threshold,
    IMAGE_TYPE _upper_threshold
)
{
    // ITK threshold
    using binary_threshold_image_filter = itk::BinaryThresholdImageFilter<itk::Image<IMAGE_TYPE, DIM>,
                                                                          itk::Image<std::uint8_t, DIM> >;
    typename binary_threshold_image_filter::Pointer threshold_filter = binary_threshold_image_filter::New();
    threshold_filter->SetInput(_image);
    threshold_filter->SetInsideValue(std::numeric_limits<std::uint8_t>::max());
    threshold_filter->SetOutsideValue(std::numeric_limits<std::uint8_t>::min());
    threshold_filter->SetLowerThreshold(_lower_threshold);
    threshold_filter->SetUpperThreshold(_upper_threshold);
    threshold_filter->Update();

    return threshold_filter->GetOutput();
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE, unsigned int DIM>
itk::Image<IMAGE_TYPE, DIM>::Pointer median(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    std::size_t _x,
    std::size_t _y,
    std::size_t _z
)
{
    // ITK median filter
    using image_t             = itk::Image<IMAGE_TYPE, DIM>;
    using median_image_filter = itk::MedianImageFilter<image_t, image_t>;
    typename median_image_filter::Pointer median_filter = median_image_filter::New();

    typename median_image_filter::radius_t radius;
    radius[0] = _x;
    radius[1] = _y;
    radius[2] = _z;

    median_filter->SetInput(_image);
    median_filter->SetRadius(radius);
    median_filter->Update();

    return median_filter->GetOutput();
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE, unsigned int DIM>
itk::Image<std::uint8_t, DIM>::Pointer labeling(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    unsigned int _num_labels
)
{
    // ITK median filter
    using image_t       = itk::Image<IMAGE_TYPE, DIM>;
    using label_image_t = itk::Image<std::uint8_t, 3>;

    // ITK labeling
    // Connected component filter
    auto filter_cc = itk::ConnectedComponentImageFilter<image_t, label_image_t>::New();
    filter_cc->SetInput(_image);
    filter_cc->SetBackgroundValue(0); // ignored by ITK !!! fixed by (*)
    filter_cc->SetFullyConnected(true);
    filter_cc->Update();

    label_image_t::Pointer labeled_img = filter_cc->GetOutput();
    bug_work_around_labeling(_image, labeled_img, 0); // (*)

    // Relabels connected component filter
    auto relabel_filter = itk::RelabelComponentImageFilter<label_image_t, label_image_t>::New();
    relabel_filter->SetInPlace(true); // can be set inplace because it is an internal filter
    relabel_filter->SetInput(labeled_img);
    relabel_filter->Update();

    // Output
    auto* img_out = relabel_filter->GetOutput();
    for(auto& pixel : itk::ImageRegionRange<label_image_t>(*img_out))
    {
        bool is_pixel = pixel != itk::NumericTraits<IMAGE_TYPE>::Zero;
        is_pixel = is_pixel && pixel <= static_cast<IMAGE_TYPE>(_num_labels);

        if(!is_pixel)
        {
            pixel = std::uint8_t(itk::NumericTraits<IMAGE_TYPE>::Zero);
        }
    }

    return img_out;
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE, unsigned int DIM>
itk::Image<IMAGE_TYPE, DIM>::Pointer closing(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    std::size_t _x,
    std::size_t _y,
    std::size_t _z
)
{
    // ITK median filter
    using image_t               = itk::Image<IMAGE_TYPE, DIM>;
    using structuring_element_t = itk::BinaryBallStructuringElement<IMAGE_TYPE, 3>;
    using itk_filter_type       = itk::GrayscaleMorphologicalClosingImageFilter<image_t, image_t,
                                                                                structuring_element_t>;
    typename image_t::Pointer itk_output_image;
    typename itk_filter_type::Pointer filter = itk_filter_type::New();

    typename itk_filter_type::Pointer::ObjectType::KernelType structuring_element;
    typename itk_filter_type::Pointer::ObjectType::KernelType::size_t size;
    size[0] = _x;
    size[1] = _y;
    size[2] = _z;

    structuring_element.SetRadius(size);
    structuring_element.CreateStructuringElement();
    filter->SetKernel(structuring_element);
    filter->SetInput(_image);
    filter->Update();
    itk_output_image = filter->GetOutput();

    return itk_output_image;
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE, unsigned int DIM>
itk::Image<IMAGE_TYPE, DIM>::Pointer fill_hole_2d(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    unsigned int _direction,
    IMAGE_TYPE _foreground
)
{
    using image3_d = itk::Image<IMAGE_TYPE, DIM>;
    using image2_d = itk::Image<IMAGE_TYPE, 2>;
    std::uint64_t nb_planes = _image->GetBufferedRegion().GetSize(_direction);

    for(std::uint64_t plane = 0 ; plane < nb_planes ; ++plane)
    {
        itk::ImageRegion<3> region_to_extract = _image->GetBufferedRegion();

        auto extractor = itk::ExtractImageFilter<image3_d, image2_d>::New();

        // extracts plane along other "direction"
        region_to_extract.SetSize(_direction, 0);
        region_to_extract.SetIndex(_direction, static_cast<std::int64_t>(plane));
        extractor->InPlaceOff();
        extractor->set_input(_image);
        extractor->SetExtractionRegion(region_to_extract);
        extractor->SetDirectionCollapseToIdentity();
        extractor->Update();

        auto image_2d  = extractor->GetOutput();
        auto fill_hole = itk::BinaryFillholeImageFilter<image2_d>::New();

        fill_hole->set_input(image_2d);
        fill_hole->SetForegroundValue(_foreground);
        fill_hole->SetFullyConnected(true);
        fill_hole->Update();

        itk::ImageRegionConstIterator<image2_d> img_2d_it(fill_hole->GetOutput(),
                                                          fill_hole->GetOutput()->GetBufferedRegion());

        // creates a non "empty" region
        region_to_extract.SetSize(_direction, 1);
        itk::ImageRegionIterator<image3_d> img_3d_it(_image, region_to_extract);
        img_3d_it.GoToBegin();
        img_2d_it.GoToBegin();
        while(img_3d_it.IsAtEnd() == false)
        {
            img_3d_it.Set(img_2d_it.Get());
            ++img_3d_it;
            ++img_2d_it;
        }
    }

    return _image;
}

} // namespace sight::filter::image::detail
