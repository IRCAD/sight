/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "filter/image/filters.hpp"

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

namespace sight::filter::image
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
typename itk::Image<std::uint8_t, DIM>::Pointer threshold(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    IMAGE_TYPE _lower_threshold,
    IMAGE_TYPE _upper_threshold
)
{
    // ITK threshold
    typedef itk::BinaryThresholdImageFilter<itk::Image<IMAGE_TYPE, DIM>,
                                            itk::Image<std::uint8_t, DIM> > BinaryThresholdImageFilter;
    typename BinaryThresholdImageFilter::Pointer threshold_filter = BinaryThresholdImageFilter::New();
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
typename itk::Image<IMAGE_TYPE, DIM>::Pointer median(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    std::size_t _x,
    std::size_t _y,
    std::size_t _z
)
{
    // ITK median filter
    typedef itk::Image<IMAGE_TYPE, DIM> image_t;
    typedef itk::MedianImageFilter<image_t, image_t> MedianImageFilter;
    typename MedianImageFilter::Pointer median_filter = MedianImageFilter::New();

    typename MedianImageFilter::radius_t radius;
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
typename itk::Image<std::uint8_t, DIM>::Pointer labeling(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    unsigned int _num_labels
)
{
    // ITK median filter
    typedef itk::Image<IMAGE_TYPE, DIM> image_t;
    typedef itk::Image<std::uint8_t, 3> label_image_t;

    // ITK labeling
    // Connected component filter
    typedef itk::ConnectedComponentImageFilter<image_t, label_image_t> ConnectedComponentFilter;
    typename ConnectedComponentFilter::Pointer filter_cc = ConnectedComponentFilter::New();
    filter_cc->SetInput(_image);
    filter_cc->SetBackgroundValue(0); // ignored by ITK !!! fixed by (*)
    filter_cc->SetFullyConnected(true);
    filter_cc->Update();

    typename label_image_t::Pointer labeled_img = filter_cc->GetOutput();
    bug_work_around_labeling(_image, labeled_img, 0); // (*)

    // Relabels connected component filter
    typedef itk::RelabelComponentImageFilter<label_image_t, label_image_t> RelabelFilter;
    typename RelabelFilter::Pointer relabel_filter = RelabelFilter::New();
    relabel_filter->SetInPlace(true); // can be set inplace because it is an internal filter
    relabel_filter->SetInput(labeled_img);
    relabel_filter->Update();

    // Output
    typename label_image_t::Pointer img_out = relabel_filter->GetOutput();
    typename itk::ImageRegionIterator<label_image_t> itk_it_out(img_out, img_out->GetBufferedRegion());

    for(itk_it_out.GoToBegin() ; !itk_it_out.IsAtEnd() ; ++itk_it_out)
    {
        bool is_pixel = itk_it_out.Get() != itk::NumericTraits<IMAGE_TYPE>::Zero;

        is_pixel = is_pixel && itk_it_out.Get() <= static_cast<IMAGE_TYPE>(_num_labels);

        if(!is_pixel)
        {
            itk_it_out.Set(decltype(itk_it_out) ::PixelType(itk::NumericTraits<IMAGE_TYPE>::Zero));
        }
    }

    return img_out;
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE, unsigned int DIM>
typename itk::Image<IMAGE_TYPE, DIM>::Pointer closing(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    std::size_t _x,
    std::size_t _y,
    std::size_t _z
)
{
    // ITK median filter
    typedef itk::Image<IMAGE_TYPE, DIM> image_t;
    typedef itk::BinaryBallStructuringElement<IMAGE_TYPE, 3> structuring_element_t;
    typedef itk::GrayscaleMorphologicalClosingImageFilter<image_t, image_t,
                                                          structuring_element_t> ITKFilterType;
    typename image_t::Pointer itk_output_image;
    typename ITKFilterType::Pointer filter = ITKFilterType::New();

    typename ITKFilterType::Pointer::ObjectType::KernelType structuring_element;
    typename ITKFilterType::Pointer::ObjectType::KernelType::size_t size;
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
typename itk::Image<IMAGE_TYPE, DIM>::Pointer fill_hole2_d(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    unsigned int _direction,
    IMAGE_TYPE _foreground
)
{
    typedef itk::Image<IMAGE_TYPE, DIM> Image3D;
    typedef itk::Image<IMAGE_TYPE, 2> Image2D;
    std::uint64_t nb_planes = _image->GetBufferedRegion().GetSize(_direction);

    for(std::uint64_t plane = 0 ; plane < nb_planes ; ++plane)
    {
        itk::ImageRegion<3> region_to_extract = _image->GetBufferedRegion();

        typedef typename itk::ExtractImageFilter<Image3D, Image2D> ExtractFilter;
        typename ExtractFilter::Pointer extractor = ExtractFilter::New();

        // extracts plane along other "direction"
        region_to_extract.SetSize(_direction, 0);
        region_to_extract.SetIndex(_direction, static_cast<std::int64_t>(plane));
        extractor->InPlaceOff();
        extractor->set_input(_image);
        extractor->SetExtractionRegion(region_to_extract);
        extractor->SetDirectionCollapseToIdentity();
        extractor->Update();

        typename Image2D::Pointer image2_d = extractor->GetOutput();

        typedef typename itk::BinaryFillholeImageFilter<Image2D> FillHoleFilter;
        typename FillHoleFilter::Pointer fill_hole = FillHoleFilter::New();

        fill_hole->set_input(image2_d);
        fill_hole->SetForegroundValue(_foreground);
        fill_hole->SetFullyConnected(true);
        fill_hole->Update();

        itk::ImageRegionConstIterator<Image2D> img2_d_it(fill_hole->GetOutput(),
                                                         fill_hole->GetOutput()->GetBufferedRegion());

        // creates a non "empty" region
        region_to_extract.SetSize(_direction, 1);
        itk::ImageRegionIterator<Image3D> img3_d_it(_image, region_to_extract);
        img3_d_it.GoToBegin();
        img2_d_it.GoToBegin();
        while(img3_d_it.IsAtEnd() == false)
        {
            img3_d_it.Set(img2_d_it.Get());
            ++img3_d_it;
            ++img2_d_it;
        }
    }

    return _image;
}

} // namespace sight::filter::image.
