/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "modules/filter/image/SLabelImageToBinaryImage.hpp"

#include <core/com/Signal.hxx>

#include <data/Image.hpp>
#include <data/Integer.hpp>
#include <data/Vector.hpp>

#include <service/macros.hpp>

#include <io/itk/itk.hpp>

#include <itkImage.h>
#include <itkUnaryFunctorImageFilter.h>

#include <algorithm>
#include <bitset>
#include <functional>

namespace sight::module::filter::image
{

fwServicesRegisterMacro(::sight::service::IOperator, ::sight::module::filter::image::SLabelImageToBinaryImage)

static const service::IService::KeyType s_LABEL_IMAGE_INPUT = "labelImage";
static const service::IService::KeyType s_BINARY_MASK_INOUT = "binaryMask";

typedef std::function< std::uint8_t(const std::uint8_t&) > FunctionType;

class LambdaFunctor
{
public:

    LambdaFunctor()
    {
    }

    LambdaFunctor(const FunctionType& _f) :
        m_function(_f)
    {
    }

    //------------------------------------------------------------------------------

    inline std::uint8_t operator() (const std::uint8_t& _in)
    {
        return m_function(_in);
    }

    // Needs to be implemented because it is called by the itkUnaryFunctorImageFilter when setting the functor.
    // Always return true to force-set the functor.
    inline bool operator != (const LambdaFunctor&)
    {
        return true;
    }

private:

    FunctionType m_function;
};

//------------------------------------------------------------------------------

SLabelImageToBinaryImage::SLabelImageToBinaryImage()
{

}

//------------------------------------------------------------------------------

SLabelImageToBinaryImage::~SLabelImageToBinaryImage()
{

}

//------------------------------------------------------------------------------

void SLabelImageToBinaryImage::configuring()
{
    const ConfigType config = this->getConfigTree();

    m_labelSetFieldName = config.get_optional<std::string>("config.<xmlattr>.labelsField");
}

//------------------------------------------------------------------------------

void SLabelImageToBinaryImage::starting()
{

}

//------------------------------------------------------------------------------

void SLabelImageToBinaryImage::updating()
{
    typedef typename ::itk::Image< std::uint8_t, 3 > ImageType;

    const auto labelImage = this->getLockedInput< data::Image >(s_LABEL_IMAGE_INPUT);
    SLM_ASSERT("No 'labelImage' input.", labelImage);

    const auto maskImage = this->getLockedInOut< data::Image >(s_BINARY_MASK_INOUT);
    SLM_ASSERT("No 'maskImage' inout.", maskImage);

    SLM_ASSERT("The label image must be a greyscale image with uint8 values.",
               labelImage->getType() == core::tools::Type::s_UINT8 && labelImage->getNumberOfComponents() == 1);

    LambdaFunctor functor;
    if(m_labelSetFieldName)
    {
        data::Vector::csptr labels = labelImage->getField< data::Vector >(m_labelSetFieldName.value());

        if(!labels)
        {
            SLM_INFO("No field named '" + m_labelSetFieldName.value() + "' in 'labelImage'. No binary mask generated.");
            return;
        }

        std::bitset< std::numeric_limits< std::uint8_t >::max() + 1 > labelSet;

        std::for_each(labels->begin(), labels->end(), [&labelSet](data::Object::csptr _o)
            {
                data::Integer::csptr intObj = data::Integer::dynamicConstCast(_o);
                SLM_ASSERT("The label vector should only contain integers.", intObj);
                const int val = intObj->value();
                SLM_ASSERT("The integers in the vector must be in the [0, 255] range.", val >= 0 && val <= 255 );
                labelSet.set(static_cast< std::uint8_t >(val), true);
            });

        functor = FunctionType([labelSet](const std::uint8_t& _in)
            {
                return labelSet[_in] ? 255 : 0;
            });
    }
    else
    {
        functor = FunctionType([](const std::uint8_t& _in)
            {
                return _in > 0 ? 255 : 0;
            });
    }

    typename ImageType::Pointer itkLabelImg = io::itk::itkImageFactory< ImageType >(labelImage.get_shared());

    ::itk::UnaryFunctorImageFilter< ImageType, ImageType, LambdaFunctor >::Pointer labelToMaskFilter =
        ::itk::UnaryFunctorImageFilter< ImageType, ImageType, LambdaFunctor >::New();

    labelToMaskFilter->SetFunctor(functor);
    labelToMaskFilter->SetInput(itkLabelImg);
    labelToMaskFilter->Update();

    typename ImageType::Pointer itkMaskImg = labelToMaskFilter->GetOutput();

    io::itk::itkImageToFwDataImage< ImageType::Pointer >(itkMaskImg, maskImage.get_shared());
    const auto modifiedSig = maskImage->signal< data::Object::ModifiedSignalType >(data::Image::s_MODIFIED_SIG);

    modifiedSig->asyncEmit();

    m_sigComputed->asyncEmit();
}

//------------------------------------------------------------------------------

void SLabelImageToBinaryImage::stopping()
{
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SLabelImageToBinaryImage::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_LABEL_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_LABEL_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image.
