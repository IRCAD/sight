/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include <core/com/signals.hpp>

#include <ui/__/action.hpp>

#include <viz/scene3d/layer.hpp>

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
    <service uid="..." type="sight::module::ui::viz::stereo_toggler" >
        <config stereoMode="interlaced" />
    </service>
 *
 * @subsection Configuration Configuration
 * - \b stereoMode (values=interlaced|AutoStereo5|AutoStereo8): mode to activate. 'auto_stereo<N>' modes are
 * for glasses-free stereo screens with N viewpoints. The 'interlaced' mode is for typical polarized screens with
 * interlaced image pairs.
 *
 */
class MODULE_UI_VIZ_CLASS_API stereo_toggler : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(stereo_toggler, sight::ui::action);

    /// Constructor.
    MODULE_UI_VIZ_API stereo_toggler();

    /// Destructor.
    MODULE_UI_VIZ_API ~stereo_toggler() override;

protected:

    /// Configures the service.
    MODULE_UI_VIZ_API void configuring() override;

    /// Starts the action service.
    MODULE_UI_VIZ_API void starting() override;

    /// Enables stereo if active, disables it otherwise.
    MODULE_UI_VIZ_API void updating() override;

    /// Stops the action service.
    MODULE_UI_VIZ_API void stopping() override;

private:

    using stereo_mode_t       = sight::viz::scene3d::compositor::core::stereo_mode_t;
    using stereo_active_sig_t = core::com::signal<void (bool)>;

    /// layer in which we enable/disable stereo.
    std::string m_layer_id;

    /// Mode that is toggled.
    sight::viz::scene3d::compositor::core::stereo_mode_t m_stereo_mode {stereo_mode_t::none};

    /// Sent at each update, notifies if stereo is enabled.
    stereo_active_sig_t::sptr m_stereo_active_sig;
};

} // namespace sight::module::ui::viz
