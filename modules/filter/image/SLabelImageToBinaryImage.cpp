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

#include "modules/filter/image/SLabelImageToBinaryImage.hpp"

#include <core/com/Signal.hxx>

#include <data/Integer.hpp>
#include <data/Vector.hpp>

#include <io/itk/itk.hpp>

#include <service/macros.hpp>

#include <itkImage.h>
#include <itkUnaryFunctorImageFilter.h>

#include <algorithm>
#include <bitset>
#include <functional>
#include <utility>

namespace sight::module::filter::image
{

using FunctionType = std::function<std::uint8_t(const std::uint8_t&)>;

class LambdaFunctor
{
public:

    LambdaFunctor()
    = default;

    explicit LambdaFunctor(FunctionType _f) :
        m_function(std::move(_f))
    {
    }

    //------------------------------------------------------------------------------

    inline std::uint8_t operator()(const std::uint8_t& _in)
    {
        return m_function(_in);
    }

    // Needs to be implemented because it is called by the itkUnaryFunctorImageFilter when setting the functor.
    // Always return true to force-set the functor.
    inline bool operator!=(const LambdaFunctor& /*unused*/)
    {
        return true;
    }

private:

    FunctionType m_function;
};

//------------------------------------------------------------------------------

SLabelImageToBinaryImage::SLabelImageToBinaryImage()
= default;

//------------------------------------------------------------------------------

SLabelImageToBinaryImage::~SLabelImageToBinaryImage()
= default;

//------------------------------------------------------------------------------

void SLabelImageToBinaryImage::configuring()
{
    const ConfigType config = this->getConfiguration();

    m_labelSetFieldName = config.get_optional<std::string>("config.<xmlattr>.labelsField");
}

//------------------------------------------------------------------------------

void SLabelImageToBinaryImage::starting()
{
}

//------------------------------------------------------------------------------

void SLabelImageToBinaryImage::updating()
{
    using ImageType = typename itk::Image<std::uint8_t, 3>;

    const auto labelImage = m_labelImage.lock();
    SIGHT_ASSERT("No " << s_LABEL_IMAGE_INPUT << " input.", labelImage);

    const auto maskImage = m_binaryMask.lock();
    SIGHT_ASSERT("No " << s_BINARY_MASK_INOUT << " inout.", maskImage);

    SIGHT_ASSERT(
        "The label image must be a greyscale image with uint8 values.",
        labelImage->getType() == core::Type::UINT8 && labelImage->numComponents() == 1
    );

    LambdaFunctor functor;
    if(m_labelSetFieldName)
    {
        data::Vector::csptr labels = labelImage->getField<data::Vector>(m_labelSetFieldName.value());

        if(!labels)
        {
            SIGHT_INFO(
                "No field named '" + m_labelSetFieldName.value()
                + "' in 'labelImage'. No binary mask generated."
            );
            return;
        }

        std::bitset<std::numeric_limits<std::uint8_t>::max() + 1> labelSet;

        std::for_each(
            labels->begin(),
            labels->end(),
            [&labelSet](data::Object::csptr _o)
            {
                data::Integer::csptr intObj = data::Integer::dynamicConstCast(_o);
                SIGHT_ASSERT("The label vector should only contain integers.", intObj);
                const int val = int(intObj->value());
                SIGHT_ASSERT("The integers in the vector must be in the [0, 255] range.", val >= 0 && val <= 255);
                labelSet.set(static_cast<std::uint8_t>(val), true);
            });

        functor = LambdaFunctor(
            FunctionType(
                [labelSet](const std::uint8_t& _in)
            {
                return labelSet[_in] ? 255 : 0;
            })
        );
    }
    else
    {
        functor = LambdaFunctor(
            FunctionType(
                [](const std::uint8_t& _in)
            {
                return _in > 0 ? 255 : 0;
            })
        );
    }

    typename ImageType::Pointer itkLabelImg = io::itk::moveToItk<ImageType>(labelImage.get_shared());

    itk::UnaryFunctorImageFilter<ImageType, ImageType, LambdaFunctor>::Pointer labelToMaskFilter =
        itk::UnaryFunctorImageFilter<ImageType, ImageType, LambdaFunctor>::New();

    labelToMaskFilter->SetFunctor(functor);
    labelToMaskFilter->SetInput(itkLabelImg);
    labelToMaskFilter->Update();

    typename ImageType::Pointer itkMaskImg = labelToMaskFilter->GetOutput();

    io::itk::moveFromItk<ImageType::Pointer>(itkMaskImg, maskImage.get_shared());
    const auto modifiedSig = maskImage->signal<data::Object::ModifiedSignalType>(data::Image::s_MODIFIED_SIG);

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
    return {
        {s_LABEL_IMAGE_INPUT, data::Image::s_BUFFER_MODIFIED_SIG, IService::slots::s_UPDATE},
        {s_LABEL_IMAGE_INPUT, data::Image::s_MODIFIED_SIG, IService::slots::s_UPDATE}
    };
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image.
