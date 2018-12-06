/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "uiVisuOgre/config.hpp"

#include <fwCom/Signals.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwRenderOgre/Layer.hpp>

namespace uiVisuOgre
{

/**
 * @brief Action to enable/disable stereo in an ogre scene layer.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiVisuOgre::SStereoToggler" >
        <config layer="default" stereoMode="interlaced" />
    </service>
 *
 * @subsection Configuration Configuration
 * - \b layer : layer in which to activate stereo rendering.
 * - \b stereoMode (values=interlaced|AutoStereo5|AutoStereo8): mode to activate.
 *
 */
class UIVISUOGRE_CLASS_API SStereoToggler : public ::fwGui::IActionSrv
{
public:

    fwCoreServiceClassDefinitionsMacro( (SStereoToggler)(::fwGui::IActionSrv) )

    /// Constructor.
    UIVISUOGRE_API SStereoToggler();

    /// Destructor.
    UIVISUOGRE_API virtual ~SStereoToggler() override;

protected:

    /// Configures the service.
    UIVISUOGRE_API virtual void configuring() override;

    /// Starts the action service.
    UIVISUOGRE_API virtual void starting() override;

    /// Enables stereo if active, disables it otherwise.
    UIVISUOGRE_API virtual void updating() override;

    /// Stops the action service.
    UIVISUOGRE_API virtual void stopping() override;

private:

    using StereoModeType      = ::fwRenderOgre::compositor::Core::StereoModeType;
    using StereoActiveSigType = ::fwCom::Signal< void(bool) >;

    /// Layer in which we enable/disable stereo.
    std::string m_layerId;

    /// Mode that is toggled.
    ::fwRenderOgre::compositor::Core::StereoModeType m_stereoMode { StereoModeType::NONE };

    StereoActiveSigType::sptr m_stereoActiveSig;

};

} // namespace uiVisuOgre
