/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
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

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwData/Material.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <string>

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor displays the frustum of an ::arData::Camera
 *
 * @section Slots Slots
 * -\b updateVisibility(bool): Sets whether the frustum is shown or not.
 * -\b toggleVisibility(): Toggles whether the frustum is shown or not.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SFrustum">
        <in key="camera" uid="..." />
        <config layer="..." transform="..." near="..." far="..."/>
    </service>
   @endcode
 * @subsection Input Input:
 * - \b camera [::arData::Camera]:  camera containing calibration information.
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the frustum's layer
 * - \b transform (optional): transform applied to the frustum's scene node
 * - \b near (optional): near clipping distance of the ::Ogre::Camera
 * - \b far (optional): far clipping distance of the ::Ogre::Camera
 * - \b color (optional): frustum's color
 */
class VISUOGREADAPTOR_CLASS_API SFrustum : public ::fwRenderOgre::IAdaptor,
                                           public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SFrustum, ::fwRenderOgre::IAdaptor);

    /**
     * @name Slots API
     * @{
     */
    /// Enable/disable visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    /// Toggle visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;
    /** @} */

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SFrustum() noexcept;
    /// Destructor: Does nothing
    VISUOGREADAPTOR_API virtual ~SFrustum() noexcept;

    /// Connect ::fwData::Object::s_MODIFIED_SIG and s_INTRINSIC_CALIBRATED_SIG of the camera data to s_UPDATE_SLOT
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Slot: Sets visibility of the frustum
    VISUOGREADAPTOR_API void updateVisibility(bool);
    /// Slot: Toggles visibility of the frustum
    VISUOGREADAPTOR_API void toggleVisibility();

private:

    /// Configures.
    void configuring() override;
    /// Manually creates a frustum.
    void starting() override;
    /// Deletes the frustum.
    void stopping() override;
    /// Checks if the camera has changed, and updates it if it has.
    void updating() override;

    /// Sets ::Ogre::Camera from arData::Camera parameters
    void setOgreCamFromData();

    /// Ogre's camera (frustum) representing arData::Camera position and parameters
    ::Ogre::Camera* m_ogreCamera {nullptr};
    /// Pointer to the Material data
    ::fwData::Material::sptr m_material {nullptr};
    /// Hides or displays the frustum
    bool m_visibility {true};
    /// Near clipping
    float m_near {1.f};
    /// Far clipping
    float m_far {20.f};
    /// Color of frustum
    std::string m_color {"#ff0000ff"};

};

} //namespace visuOgreAdaptor
