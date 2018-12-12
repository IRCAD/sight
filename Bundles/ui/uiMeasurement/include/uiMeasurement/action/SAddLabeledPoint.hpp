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

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action adds labeled point into a PointList.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type=":uiMeasurement::action::SAddLabeledPoint">
       <inout key="pointlist" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b pointlist [::fwData::PointList]: point list containing the points.
 */
class UIMEASUREMENT_CLASS_API SAddLabeledPoint : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SAddLabeledPoint)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API SAddLabeledPoint() noexcept;

    UIMEASUREMENT_API virtual ~SAddLabeledPoint() noexcept;

protected:

    UIMEASUREMENT_API void configuring() override;

    UIMEASUREMENT_API void starting() override;

    UIMEASUREMENT_API void updating() override;

    UIMEASUREMENT_API void stopping() override;

    UIMEASUREMENT_API void info(std::ostream& _sstream ) override;

private:

    /// Helper to build point label
    bool defineLabel(std::string& name);

    int m_actionCheckId;

    unsigned int m_count;
};

} // namespace action
} // namespace uiMeasurement
