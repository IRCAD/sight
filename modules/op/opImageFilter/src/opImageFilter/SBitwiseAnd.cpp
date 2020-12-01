/************************************************************************
 *
 * Copyright (C) 2018-2020 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "opImageFilter/SBitwiseAnd.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>

#include <fwDataTools/helper/ImageGetter.hpp>

#include <fwItkIO/itk.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>
#include <fwTools/IntegerTypes.hpp>

#include <itkAndImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>

namespace opImageFilter
{

fwServicesRegisterMacro(::fwServices::IOperator, ::opImageFilter::SBitwiseAnd, ::fwData::Image)

static const ::fwServices::IService::KeyType s_IMAGE_IN = "image";
static const ::fwServices::IService::KeyType s_MASK_IN = "mask";

static const ::fwServices::IService::KeyType s_OUTPUTIMAGE_OUT = "outputImage";

struct AndImageFilterParameters
{
    ::fwData::Image::csptr inputImage;
    ::fwData::Image::csptr mask;
    ::fwData::Image::sptr outputImage;
};

//------------------------------------------------------------------------------

template<typename PIXELTYPE>
struct AndImageFilter
{
    //------------------------------------------------------------------------------

    template<class MASK_PIXELTYPE>
    void operator()(AndImageFilterParameters& params)
    {
        ::fwData::Image::csptr inputImage = params.inputImage;
        ::fwData::Image::csptr mask       = params.mask;
        ::fwData::Image::sptr outputImage = params.outputImage;

        const unsigned int dimension = 3;
        SLM_ASSERT("Only image dimension 3 managed.", inputImage->getNumberOfDimensions() == dimension);

        typedef typename ::itk::Image<PIXELTYPE, dimension> InputImageType;
        typedef typename ::itk::Image<MASK_PIXELTYPE, dimension>  MaskImageType;
        typedef typename ::itk::Image<PIXELTYPE, dimension> OutputImageType;

        typename InputImageType::Pointer itkInputImage = ::fwItkIO::itkImageFactory<InputImageType>( inputImage );
        typename MaskImageType::Pointer itkMaskImage   = ::fwItkIO::itkImageFactory<MaskImageType>( mask );
        typename OutputImageType::Pointer itkOutputImage;

        // We assume that the mask pixel type has a lower size in bits than the image pixel type
        // Cast mask pixel type to the image pixel type
        typedef itk::CastImageFilter< MaskImageType, InputImageType> FilterType;
        typename FilterType::Pointer caster = FilterType::New();
        caster->SetInput( itkMaskImage);

        // Rescale the image so that the output range of the casted mask image is in the same range as the input image.
        typedef itk::RescaleIntensityImageFilter< InputImageType, InputImageType> RescaleType;
        typename RescaleType::Pointer rescaler = RescaleType::New();
        rescaler->SetInput( caster->GetOutput() );
        rescaler->SetOutputMinimum(0);
        rescaler->SetOutputMaximum(std::numeric_limits<PIXELTYPE>::max());
        rescaler->Update();

        typename InputImageType::Pointer itkMaskImageCasted = rescaler->GetOutput();

        typedef typename ::itk::AndImageFilter<InputImageType, InputImageType, OutputImageType> ITKFilterType;
        typename ITKFilterType::Pointer filter = ITKFilterType::New();
        filter->SetInput1( itkInputImage );
        filter->SetInput2( itkMaskImageCasted );
        itkOutputImage = filter->GetOutput();
        filter->Update();

        itkOutputImage->GetSource()->Update();
        ::fwItkIO::dataImageFactory< OutputImageType >( itkOutputImage, outputImage );
    }
};

//------------------------------------------------------------------------------

struct AndImageFilterCaller
{
    //------------------------------------------------------------------------------

    template<class PIXELTYPE>
    void operator() (AndImageFilterParameters& params)
    {
        const ::fwTools::DynamicType maskType = params.mask->getPixelType();
        ::fwTools::Dispatcher< ::fwTools::IntegerTypes, AndImageFilter<PIXELTYPE> >::invoke(maskType, params);
    }
};

//-----------------------------------------------------------------------------

SBitwiseAnd::SBitwiseAnd()
{
}

//-----------------------------------------------------------------------------

SBitwiseAnd::~SBitwiseAnd()
{
}

//-----------------------------------------------------------------------------

void SBitwiseAnd::configuring()
{
}

//-----------------------------------------------------------------------------

void SBitwiseAnd::starting()
{
}

//-----------------------------------------------------------------------------

void SBitwiseAnd::updating()
{
    const auto image = this->getLockedInput< ::fwData::Image >(s_IMAGE_IN);
    SLM_ASSERT("image does not exist.", image);

    const auto mask = this->getLockedInput< ::fwData::Image >(s_MASK_IN);
    SLM_ASSERT("mask does not exist.", mask);

    ::fwDataTools::helper::ImageGetter imageHelper(image.get_shared());
    ::fwDataTools::helper::ImageGetter maskHelper(mask.get_shared());

    ::fwData::Image::sptr outputImage = ::fwData::Image::New();

    AndImageFilterParameters params;
    params.inputImage  = image.get_shared();
    params.mask        = mask.get_shared();
    params.outputImage = outputImage;

    ::fwTools::DynamicType type = image->getPixelType();
    ::fwTools::Dispatcher< ::fwTools::IntegerTypes, AndImageFilterCaller >::invoke(type, params);

    this->setOutput(s_OUTPUTIMAGE_OUT, outputImage);

    m_sigComputed->asyncEmit();
}

//-----------------------------------------------------------------------------

void SBitwiseAnd::stopping()
{
}

//-----------------------------------------------------------------------------

} // namespace opImageFilter.
