/************************************************************************
 *
 * Copyright (C) 2019-2021 IRCAD France
 * Copyright (C) 2019-2021 IHU Strasbourg
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

#include "modules/viz/ogre/config.hpp"

#include <viz/ogre/IAdaptor.hpp>
#include <viz/ogre/interactor/TrackballInteractor.hpp>

#include <memory>

namespace sight::modules::viz::ogre::adaptor
{

/**
 * @brief This adaptor lets the user move the camera around a point of interest using the mouse and keyboard.
 *
 * See @see sight::viz::ogre::interactor::TrackballInteractor to learn more about the interactions provided by this
 * service.
 *
 * @warning may not work as intended when used at the same time as `modules::viz::ogre::adaptor::Camera` on the same
 * layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="::sight::modules::viz::ogre::adaptor::STrackballCamera">
        <config layer="..." priority="0" layerOrderDependant="true" />
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): layer on which the trackball interactions are added.
 * - \b priority (optional, int, default=0): interaction priority, higher priority interactions are performed first.
 * - \b layerOrderDependant (optional, bool, default=true): define if interaction must take into account above layers.
 */
class MODULE_VIZ_OGRE_CLASS_API STrackballCamera final : public sight::viz::ogre::IAdaptor
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(STrackballCamera, sight::viz::ogre::IAdaptor)

    /// Constructor.
    MODULE_VIZ_OGRE_API STrackballCamera() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_OGRE_API ~STrackballCamera() noexcept override;

protected:

    /// Configures the layer and the interaction priority.
    MODULE_VIZ_OGRE_API void configuring() override;

    /// Adds trackball interactions to the layer.
    MODULE_VIZ_OGRE_API void starting() override;

    /// Updates the service. Unused here.
    MODULE_VIZ_OGRE_API void updating() noexcept override;

    /// Removes trackball interactions from the layer.
    MODULE_VIZ_OGRE_API void stopping() override;

private:

    /// Contains the interaction handler.
    std::shared_ptr< sight::viz::ogre::interactor::TrackballInteractor > m_trackball;

    /// Defines the interaction priority.
    int m_priority { 0 };

    /// Defines if the interaction must take into account above layers.
    bool m_layerOrderDependant { true };

};

} // namespace sight::modules::viz::ogre::adaptor.
