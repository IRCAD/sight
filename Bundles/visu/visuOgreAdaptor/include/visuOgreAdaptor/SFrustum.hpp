/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
 * -\b layer (mandatory): defines the frustum's layer
 * -\b transform (optional): transform applied to the frustum's scene node
 * -\b near (optional): near clipping distance of the ::Ogre::Camera
 * -\b far (optional): far clipping distance of the ::Ogre::Camera
 * -\b color (optional): frustum's color
 */
class VISUOGREADAPTOR_CLASS_API SFrustum : public ::fwRenderOgre::IAdaptor,
                                           public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro((SFrustum)(::fwRenderOgre::IAdaptor))

    /// Slot to enable/disable visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void (bool)> UpdateVisibilitySlotType;
    /// Slot to toggle visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void ()> ToggleVisibilitySlotType;

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SFrustum() noexcept;
    /// Destructor: Does nothing
    VISUOGREADAPTOR_API virtual ~SFrustum() noexcept;

    /// Connect ::fwData::Object::s_MODIFIED_SIG and s_INTRINSIC_CALIBRATED_SIG of the camera data to s_UPDATE_SLOT
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Sets visibility of the frustum
    VISUOGREADAPTOR_API void updateVisibility(bool);
    /// Toggles visibility of the frustum
    VISUOGREADAPTOR_API void toggleVisibility();

    /// Key for the camera
    VISUOGREADAPTOR_API static const std::string s_CAMERA_INPUT;
    /// Key for the near plane
    VISUOGREADAPTOR_API static const std::string s_NEAR_CONFIG;
    /// Key for the far plane
    VISUOGREADAPTOR_API static const std::string s_FAR_CONFIG;
    /// Key for color
    VISUOGREADAPTOR_API static const std::string s_COLOR_CONFIG;

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
    ::Ogre::Camera* m_ogreCamera;
    /// Pointer to the Material data
    ::fwData::Material::sptr m_material;
    /// Hides or displays the frustum
    bool m_visibility;
    /// Near clipping
    float m_near;
    /// Far clipping
    float m_far;
    /// Color of frustum
    std::string m_color;

};

} //namespace visuOgreAdaptor
