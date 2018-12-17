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

#include <boost/filesystem/path.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief This action allows to load landmarks from a file
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiMeasurement::action::LoadLandmark" >
        <inout key="image" uid="..." />
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: Image containig the landmarks field.
 *
 * @deprecated This service is deprecated and will be removed, use SReader to load the landmarks
 */
class UIMEASUREMENT_CLASS_API LoadLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (LoadLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API LoadLandmark() noexcept;

    UIMEASUREMENT_API virtual ~LoadLandmark() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    void info(std::ostream& _sstream ) override;

    void load(const ::boost::filesystem::path& path);
};

} // namespace action

} // namespace uiMeasurement
