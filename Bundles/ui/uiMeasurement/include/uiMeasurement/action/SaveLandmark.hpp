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
 * @brief This action allows to save the landmarks stored in image.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiMeasurement::action::SaveLandmark" >
        <in key="image" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b image [::fwData::Image]: Image containig the landmarks field.
 *
 * @deprecated This service is deprecated and will be removed, use SWriter to write the landmarks
 */
class UIMEASUREMENT_CLASS_API SaveLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (SaveLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API SaveLandmark() noexcept;

    UIMEASUREMENT_API virtual ~SaveLandmark() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    void info(std::ostream& _sstream ) override;

    void save(const ::boost::filesystem::path& path);

};

} // namespace action

} // namespace uiMeasurement
