/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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
void bugWorkAroundLabeling(
    IMAGEINPTR ImageIn,
    IMAGELABELPTR Labeled,
    typename IMAGEINPTR::ObjectType::PixelType background
)
{
    itk::ImageRegionIterator<typename IMAGEINPTR::ObjectType> i_it(ImageIn, ImageIn->GetLargestPossibleRegion());
    itk::ImageRegionIterator<typename IMAGELABELPTR::ObjectType> l_it(Labeled, Labeled->GetLargestPossibleRegion());

    while(!i_it.IsAtEnd())
    {
        if(i_it.Get() == background)
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
    IMAGE_TYPE _lowerThreshold,
    IMAGE_TYPE _upperThreshold
)
{
    // ITK threshold
    typedef itk::BinaryThresholdImageFilter<itk::Image<IMAGE_TYPE, DIM>,
                                            itk::Image<std::uint8_t, DIM> > BinaryThresholdImageFilter;
    typename BinaryThresholdImageFilter::Pointer thresFilter = BinaryThresholdImageFilter::New();
    thresFilter->SetInput(_image);
    thresFilter->SetInsideValue(std::numeric_limits<std::uint8_t>::max());
    thresFilter->SetOutsideValue(std::numeric_limits<std::uint8_t>::min());
    thresFilter->SetLowerThreshold(_lowerThreshold);
    thresFilter->SetUpperThreshold(_upperThreshold);
    thresFilter->Update();

    return thresFilter->GetOutput();
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
    typedef itk::Image<IMAGE_TYPE, DIM> ImageType;
    typedef itk::MedianImageFilter<ImageType, ImageType> MedianImageFilter;
    typename MedianImageFilter::Pointer medianFilter = MedianImageFilter::New();

    typename MedianImageFilter::RadiusType radius;
    radius[0] = _x;
    radius[1] = _y;
    radius[2] = _z;

    medianFilter->SetInput(_image);
    medianFilter->SetRadius(radius);
    medianFilter->Update();

    return medianFilter->GetOutput();
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE, unsigned int DIM>
typename itk::Image<std::uint8_t, DIM>::Pointer labeling(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    unsigned int _numLabels
)
{
    // ITK median filter
    typedef itk::Image<IMAGE_TYPE, DIM> ImageType;
    typedef itk::Image<std::uint8_t, 3> LabelImageType;

    // ITK Labeling
    // Connected component filter
    typedef itk::ConnectedComponentImageFilter<ImageType, LabelImageType> ConnectedComponentFilter;
    typename ConnectedComponentFilter::Pointer filterCC = ConnectedComponentFilter::New();
    filterCC->SetInput(_image);
    filterCC->SetBackgroundValue(0); // ignored by ITK !!! fixed by (*)
    filterCC->SetFullyConnected(true);
    filterCC->Update();

    typename LabelImageType::Pointer labeledImg = filterCC->GetOutput();
    bugWorkAroundLabeling(_image, labeledImg, 0); // (*)

    // Relabels connected component filter
    typedef itk::RelabelComponentImageFilter<LabelImageType, LabelImageType> RelabelFilter;
    typename RelabelFilter::Pointer relabelFilter = RelabelFilter::New();
    relabelFilter->SetInPlace(true); // can be set inplace because it is an internal filter
    relabelFilter->SetInput(labeledImg);
    relabelFilter->Update();

    // Output
    typename LabelImageType::Pointer imgOut = relabelFilter->GetOutput();
    typename itk::ImageRegionIterator<LabelImageType> itkItOut(imgOut, imgOut->GetBufferedRegion());

    for(itkItOut.GoToBegin() ; !itkItOut.IsAtEnd() ; ++itkItOut)
    {
        bool isPixel = itkItOut.Get() != itk::NumericTraits<IMAGE_TYPE>::Zero;

        isPixel = isPixel && itkItOut.Get() <= static_cast<IMAGE_TYPE>(_numLabels);

        if(!isPixel)
        {
            itkItOut.Set(itk::NumericTraits<IMAGE_TYPE>::Zero);
        }
    }

    return imgOut;
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
    typedef itk::Image<IMAGE_TYPE, DIM> ImageType;
    typedef itk::BinaryBallStructuringElement<IMAGE_TYPE, 3> StructuringElementType;
    typedef itk::GrayscaleMorphologicalClosingImageFilter<ImageType, ImageType,
                                                          StructuringElementType> ITKFilterType;
    typename ImageType::Pointer itkOutputImage;
    typename ITKFilterType::Pointer filter = ITKFilterType::New();

    typename ITKFilterType::Pointer::ObjectType::KernelType structuringElement;
    typename ITKFilterType::Pointer::ObjectType::KernelType::SizeType size;
    size[0] = _x;
    size[1] = _y;
    size[2] = _z;

    structuringElement.SetRadius(size);
    structuringElement.CreateStructuringElement();
    filter->SetKernel(structuringElement);
    filter->SetInput(_image);
    filter->Update();
    itkOutputImage = filter->GetOutput();

    return itkOutputImage;
}

//------------------------------------------------------------------------------

template<typename IMAGE_TYPE, unsigned int DIM>
typename itk::Image<IMAGE_TYPE, DIM>::Pointer fillHole2D(
    typename itk::Image<IMAGE_TYPE, DIM>::Pointer _image,
    unsigned int _direction,
    IMAGE_TYPE _foreground
)
{
    typedef itk::Image<IMAGE_TYPE, DIM> Image3D;
    typedef itk::Image<IMAGE_TYPE, 2> Image2D;
    unsigned long nbPlanes = _image->GetBufferedRegion().GetSize(_direction);

    for(unsigned long plane = 0 ; plane < nbPlanes ; ++plane)
    {
        itk::ImageRegion<3> regionToExtract = _image->GetBufferedRegion();

        typedef typename itk::ExtractImageFilter<Image3D, Image2D> ExtractFilter;
        typename ExtractFilter::Pointer extractor = ExtractFilter::New();

        // extracts plane along other "direction"
        regionToExtract.SetSize(_direction, 0);
        regionToExtract.SetIndex(_direction, static_cast<long>(plane));
        extractor->InPlaceOff();
        extractor->SetInput(_image);
        extractor->SetExtractionRegion(regionToExtract);
        extractor->SetDirectionCollapseToIdentity();
        extractor->Update();

        typename Image2D::Pointer image2D = extractor->GetOutput();

        typedef typename itk::BinaryFillholeImageFilter<Image2D> FillHoleFilter;
        typename FillHoleFilter::Pointer fillHole = FillHoleFilter::New();

        fillHole->SetInput(image2D);
        fillHole->SetForegroundValue(_foreground);
        fillHole->SetFullyConnected(true);
        fillHole->Update();

        itk::ImageRegionConstIterator<Image2D> img2DIt(fillHole->GetOutput(),
                                                       fillHole->GetOutput()->GetBufferedRegion());

        // creates a non "empty" region
        regionToExtract.SetSize(_direction, 1);
        itk::ImageRegionIterator<Image3D> img3DIt(_image, regionToExtract);
        img3DIt.GoToBegin();
        img2DIt.GoToBegin();
        while(img3DIt.IsAtEnd() == false)
        {
            img3DIt.Set(img2DIt.Get());
            ++img3DIt;
            ++img2DIt;
        }
    }

    return _image;
}

} // namespace sight::filter::image.
