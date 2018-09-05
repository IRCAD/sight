/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include <fwData/Image.hpp>

#include <fwItkIO/itk.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntrinsicTypes.hpp>

#include <itkCastImageFilter.h>

template < typename OUTPUT_PIXELTYPE >
struct ItkImageCaster
{
    using OutputImageType = ::itk::Image<OUTPUT_PIXELTYPE, 3>;

    struct Params
    {
        ::fwData::Image::csptr i_img;
        typename OutputImageType::Pointer o_img;
    };

    //------------------------------------------------------------------------------

    template< typename INPUT_PIXELTYPE >
    void operator()(Params& p)
    {
        using InputImageType = ::itk::Image<INPUT_PIXELTYPE, 3>;

        // Convert to ITK.
        typename InputImageType::Pointer tmp = ::fwItkIO::itkImageFactory< InputImageType >(p.i_img);

        // Cast to the desired pixel type.
        auto castFilter = ::itk::CastImageFilter<InputImageType, OutputImageType>::New();
        castFilter->SetInput(tmp);
        castFilter->Update();
        p.o_img = castFilter->GetOutput();
    }
};

//------------------------------------------------------------------------------

template <typename OUTPUT_PIXELTYPE>
typename ::itk::Image<OUTPUT_PIXELTYPE, 3>::Pointer castTo(const ::fwData::Image::csptr& _img)
{
    using CasterType = ItkImageCaster<OUTPUT_PIXELTYPE>;

    typename CasterType::Params p;
    p.i_img = _img;

    const ::fwTools::DynamicType inType = _img->getPixelType();

    ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes, CasterType >::invoke(inType, p);

    return p.o_img;
}
