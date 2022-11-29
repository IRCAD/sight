/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/SRender.hpp"

#include <core/com/helper/SigSlotConnection.hpp>

#include <service/IHasServices.hpp>
#include <service/IService.hpp>

#include <OGRE/OgreSceneManager.h>

#include <string>

namespace sight::viz::scene3d
{

/**
 * @brief Interface providing behavior of Ogre adaptor services
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): sets whether the adaptor is shown or not.
 * - \b toggleVisibility(): toggle whether the adaptor is shown or not.
 * - \b show(): shows the adaptor.
 * - \b hide(): hides the hide.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="...">
        <config visible="true" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class VIZ_SCENE3D_CLASS_API IAdaptor :
    public service::IService,
    public service::IHasServices
{
friend class SRender;

public:

    /// Configuration helper string that stores "config.<xmlattr>."
    VIZ_SCENE3D_API static const std::string s_CONFIG;

    /// Generates default methods.
    SIGHT_DECLARE_CLASS(IAdaptor, service::IService);

    /// Sets the layer ID.
    VIZ_SCENE3D_API void setLayerID(const std::string& _id);

    /// @returns the layer ID of this adaptor.
    VIZ_SCENE3D_API const std::string& getLayerID() const;

    /// Sets the render service using this adaptor.
    VIZ_SCENE3D_API void setRenderService(SRender::sptr _service);

    /// @returns the render service using this adaptor
    VIZ_SCENE3D_API SRender::sptr getRenderService() const;

    /// @returns the layer where this adaptor is attached.
    VIZ_SCENE3D_API Layer::sptr getLayer() const;

    /**
     * @brief SLOT: sets the visibility of the adaptor.
     * @param _isVisible the visibility status.
     * @see setVisible(bool)
     */
    VIZ_SCENE3D_API void updateVisibility(bool _isVisible);

protected:

    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_SHOW_SLOT;
    VIZ_SCENE3D_API static const core::com::Slots::SlotKeyType s_HIDE_SLOT;

    /// Initializes slots.
    VIZ_SCENE3D_API IAdaptor() noexcept;

    /// Destroys the adaptor.
    VIZ_SCENE3D_API ~IAdaptor() noexcept override = default;

    /**
     * @brief Write information in a stream.
     *
     * This method is used by operator<<(std::ostream & _sstream, IService& _service)
     * to avoid declaration of << by all services.
     */
    VIZ_SCENE3D_API void info(std::ostream& _sstream) override;

    /// Parses common adaptor parameters.
    VIZ_SCENE3D_API void configureParams();

    /// Registers the adaptor into its render service.
    VIZ_SCENE3D_API void initialize();

    /**
     * @brief Gets the Ogre SceneManager
     * @return Ogre default scene manager
     */
    VIZ_SCENE3D_API Ogre::SceneManager* getSceneManager();

    /// Asks the render service to update rendering.
    VIZ_SCENE3D_API virtual void requestRender();

    /// SLOT: toggles the visibility of the adaptor.
    VIZ_SCENE3D_API void toggleVisibility();

    /// SLOT: shows the adaptor.
    VIZ_SCENE3D_API void show();

    /// SLOT: hides the adaptor.
    VIZ_SCENE3D_API void hide();

    /// Sets the visibility of the adaptor.
    VIZ_SCENE3D_API virtual void setVisible(bool _visible);

    /// Defines the layer ID:
    std::string m_layerID;

    /// TEMP: configured layer ID until we remove it
    std::string m_cfgLayerID;

    /// Contains the t=render service which this adaptor is attached.
    viz::scene3d::SRender::wptr m_renderService;

    /// Enables the adaptor visibility.
    bool m_isVisible {true};
};

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d.
