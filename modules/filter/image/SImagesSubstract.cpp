/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <io/itk/itk.hpp>

#include <core/com/Signal.hxx>

#include <core/spyLog.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

// Services tools
#include <service/macros.hpp>

#include "SImagesSubstract.hpp"

#include <itkSubtractImageFilter.h>

namespace sight::module::filter::image
{

//------------------------------------------------------------------------------

void SImagesSubstract::configuring()
{
}

//------------------------------------------------------------------------------

void SImagesSubstract::starting()
{
}

//------------------------------------------------------------------------------

void SImagesSubstract::stopping()
{
}

//------------------------------------------------------------------------------

void SImagesSubstract::updating()
{
    core::tools::Type REQUESTED_TYPE = core::tools::Type::create("int16");

    const auto image1 = m_image1.lock();
    const auto image2 = m_image2.lock();
    auto imageResult  = m_result.lock();

    // Test if the both images have the same type and it is signed short.
    const bool isSameType = (image1->getType() == image2->getType() && image1->getType() == REQUESTED_TYPE);

    if(isSameType)
    {
        // test if the both images have the same size.
        const bool isSameSize = (image1->getSize() == image2->getSize());
        if(isSameSize)
        {
            typedef itk::Image<std::int16_t, 3> ImageType;

            ImageType::Pointer itkImage1 = io::itk::moveToItk<ImageType>(image1.get_shared());
            SIGHT_ASSERT("Unable to convert data::Image to itkImage", itkImage1);

            ImageType::Pointer itkImage2 = io::itk::moveToItk<ImageType>(image2.get_shared());
            SIGHT_ASSERT("Unable to convert data::Image to itkImage", itkImage2);

            ImageType::Pointer output;

            //Create filter
            typedef itk::SubtractImageFilter<ImageType, ImageType, ImageType> SubtractImageFilterType;
            SubtractImageFilterType::Pointer filter;
            filter = SubtractImageFilterType::New();
            assert(filter);

            filter->SetInput1(itkImage1);
            filter->SetInput2(itkImage2);
            filter->Update();
            output = filter->GetOutput();
            assert(output->GetSource());
            io::itk::moveFromItk<ImageType>(output, imageResult.get_shared(), true);

            auto sig = imageResult->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                "Both images must have the same size.",
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
        }
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Both Images must have signed short as type.",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::filter::image
