/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "uiMeasurement/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{

namespace action
{

/**
 */

/**
 * @brief   This action removes distances from the image field.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiMeasurement::action::RemoveDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image containing the distance field.
 */
class UIMEASUREMENT_CLASS_API RemoveDistance : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (RemoveDistance)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API RemoveDistance() noexcept;

    UIMEASUREMENT_API virtual ~RemoveDistance() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

private:
    void notifyNewDistance(const ::fwData::Image::csptr& image, const ::fwData::PointList::sptr& distance) const;

    void notifyDeleteDistance(const ::fwData::Image::csptr& image, const ::fwData::PointList::csptr& distance) const;
};

} // namespace action

} // namespace uiMeasurement
