/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action adds landmaks on image.
 *
 * @section XML XML configuration
 * @code{.xml}
    <service uid="..." type="::uiMeasurement::action::AddLandmark" >
       <inout key="image" uid="..."/>
    </service>
   @endcode
 *
 *  * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to which the landmarks are added.
 *
 */
class UIMEASUREMENT_CLASS_API AddLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceMacro(AddLandmark,  ::fwGui::IActionSrv);

    UIMEASUREMENT_API AddLandmark() noexcept;

    UIMEASUREMENT_API virtual ~AddLandmark() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    UIMEASUREMENT_API void info(std::ostream& _sstream ) override;

private:
    int m_actionCheckId;
};

} // namespace action
} // namespace uiMeasurement
