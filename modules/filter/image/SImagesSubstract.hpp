/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/filter/image/config.hpp"

#include <service/IOperator.hpp>

namespace sight::modules::filter::image
{

/**
 * @brief Compute the substraction of two images.

 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::sight::modules::filter::image::SImagesSubstract">
       <in key="image1" uid="..." />
       <in key="image2" uid="..." />
       <inout key="result" uid="..." />
   </service>
   @endcode
 * @subsection In In
 * - \b image1 [sight::data::Image]: first image.
 * - \b image2 [sight::data::Image]: second image.
 * @subsection InOut InOut
 * - \b result [sight::data::Image]: substract image.
 */

class MODULE_FILTER_IMAGE_CLASS_API SImagesSubstract final : public service::IOperator
{
public:

    fwCoreServiceMacro(SImagesSubstract, ::sight::service::IOperator)

    MODULE_FILTER_IMAGE_API SImagesSubstract() noexcept          = default;
    MODULE_FILTER_IMAGE_API virtual ~SImagesSubstract() noexcept = default;

protected:

    MODULE_FILTER_IMAGE_API virtual void configuring() override;

    /// Overrides
    MODULE_FILTER_IMAGE_API virtual void starting() override;

    /// Overrides
    MODULE_FILTER_IMAGE_API virtual void stopping() override;

    /// Overrides
    MODULE_FILTER_IMAGE_API virtual void updating() override;

    /// Overrides
    MODULE_FILTER_IMAGE_API virtual void swapping() override;
};

} // namespace sight::modules::filter::image
