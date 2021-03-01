/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "modules/ui/viz/config.hpp"

#include <core/com/Signals.hpp>

#include <ui/base/IAction.hpp>

#include <viz/scene3d/Layer.hpp>

namespace sight::module::ui::viz
{

/**
 * @brief Action to enable/disable stereo in an ogre scene layer.
 *
 * @section Signals Signals
 * - \b stereoActive(bool): notifies if stereo has been enabled.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::sight::module::ui::viz::SStereoToggler" >
        <config layer="default" stereoMode="interlaced" />
    </service>
 *
 * @subsection Configuration Configuration
 * - \b layer : layer in which to activate stereo rendering.
 * - \b stereoMode (values=interlaced|AutoStereo5|AutoStereo8): mode to activate. 'AutoStereo<N>' modes are
 * for glasses-free stereo screens with N viewpoints. The 'interlaced' mode is for typical polarized screens with
 * interlaced image pairs.
 *
 */
class MODULE_UI_VIZ_CLASS_API SStereoToggler : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SStereoToggler, ::sight::ui::base::IAction)

    /// Constructor.
    MODULE_UI_VIZ_API SStereoToggler();

    /// Destructor.
    MODULE_UI_VIZ_API virtual ~SStereoToggler() override;

protected:

    /// Configures the service.
    MODULE_UI_VIZ_API virtual void configuring() override;

    /// Starts the action service.
    MODULE_UI_VIZ_API virtual void starting() override;

    /// Enables stereo if active, disables it otherwise.
    MODULE_UI_VIZ_API virtual void updating() override;

    /// Stops the action service.
    MODULE_UI_VIZ_API virtual void stopping() override;

private:

    using StereoModeType      = sight::viz::scene3d::compositor::Core::StereoModeType;
    using StereoActiveSigType = core::com::Signal< void(bool) >;

    /// Layer in which we enable/disable stereo.
    std::string m_layerId;

    /// Mode that is toggled.
    sight::viz::scene3d::compositor::Core::StereoModeType m_stereoMode { StereoModeType::NONE };

    /// Sent at each update, notifies if stereo is enabled.
    StereoActiveSigType::sptr m_stereoActiveSig;

};

} // namespace sight::module::ui::viz
