/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "visuOgreAdaptor/config.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/interactor/TrackballInteractor.hpp>

#include <memory>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor lets the user move the camera around a point of interest using the mouse and keyboard.
 *
 * See @see fwRenderOgre::interactor::TrackballInteractor to learn more about the interactions provided by this service.
 *
 * @warning may not work as intended when used at the same time as `visuOgreAdaptor::Camera` on the same layer.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="::visuOgreAdaptor::STrackballCamera">
        <config layer="..." priority="0" />
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): layer on which the trackball interactions are added.
 * - \b priority (optional, int, default=0): interaction priority, higher priority interactions are performed first.
 */
class VISUOGREADAPTOR_CLASS_API STrackballCamera final : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceMacro(STrackballCamera, ::fwRenderOgre::IAdaptor)

    /// Constructor.
    VISUOGREADAPTOR_API STrackballCamera() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~STrackballCamera() noexcept;

private:

    /// Configures the layer and the interaction priority.
    virtual void configuring() override;

    /// Adds trackball interactions to the layer.
    virtual void starting() override;

    /// Updates the service. Unused here.
    virtual void updating() noexcept override;

    /// Removes trackball interactions from the layer.
    virtual void stopping() override;

    /// Contains the interaction handler.
    std::shared_ptr< ::fwRenderOgre::interactor::TrackballInteractor > m_trackball;

    /// Defines the interaction priority.
    int m_priority { 0 };

};

} // namespace visuOgreAdaptor.
