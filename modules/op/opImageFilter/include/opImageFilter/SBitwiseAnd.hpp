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

#pragma once

#include "opImageFilter/config.hpp"

#include <fwServices/IOperator.hpp>

namespace opImageFilter
{
/**
 * @brief Implements the AND bitwise operator pixel-wise between two images.
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="::opImageFilter::SBitwiseAnd">
           <in key="image" uid="..."/>
           <in key="mask" uid="..."/>
           <out key="outputImage" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [::fwData::Image]: input image
 * - \b mask [::fwData::Image]: mask
 *
 * @subsection Output Output
 * - \b outputImage [::fwData::Image]: result of operation 'AND' between input image and mask
 */
class OPIMAGEFILTER_CLASS_API SBitwiseAnd final : public ::fwServices::IOperator
{
public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SBitwiseAnd, ::fwServices::IOperator)

    /// Does nothing.
    OPIMAGEFILTER_API SBitwiseAnd();

    /// Destroys the service.
    OPIMAGEFILTER_API ~SBitwiseAnd() override;

protected:

    /// Does nothing.
    OPIMAGEFILTER_API void starting() override;

    /// Does nothing.
    OPIMAGEFILTER_API void configuring() override;

    /// Does nothing.
    OPIMAGEFILTER_API void stopping() override;

    /// Computes centroids.
    OPIMAGEFILTER_API void updating() override;

};

} // namespace opImageFilter.
