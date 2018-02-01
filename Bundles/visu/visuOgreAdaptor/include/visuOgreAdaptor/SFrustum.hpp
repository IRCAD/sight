/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
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
 * @brief This adaptor displays the frustum of an arData::Camera
 *
 * @section Slots Slots
 * -\b updateVisibility(bool): Sets whether the frustum is shown or not.
 * -\b toggleVisibility(): Toggles whether the frustum is shown or not.
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SFrustum">
        <in key="camera" uid="..." />
        <config layer="..." transform="..." near="..." far="..."/>
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * -\b layer (mandatory): defines the frustum's layer
 * -\b transform (optional): transform applied to the frustum's scene node
 * -\b near (optional): near clipping distance of the Ogre::Camera
 * -\b far (optional): far clipping distance of the Ogre::Camera
 * -\b color (optional): frustum's color
 */
class VISUOGREADAPTOR_CLASS_API SFrustum : public ::fwRenderOgre::IAdaptor,
                                           public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro((SFrustum)(::fwRenderOgre::IAdaptor));

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
    /// Sets visibility of the frustum
    VISUOGREADAPTOR_API void updateVisibility(bool);
    /// Toggles visibility of the frustum
    VISUOGREADAPTOR_API void toggleVisibility();

    /// Key for the camera
    VISUOGREADAPTOR_API static const std::string s_INPUT_CAMERA;
    /// Key for the near plane
    VISUOGREADAPTOR_API static const std::string s_CONFIG_NEAR;
    /// Key for the far plane
    VISUOGREADAPTOR_API static const std::string s_CONFIG_FAR;
    /// Key for color
    VISUOGREADAPTOR_API static const std::string s_CONFIG_COLOR;

private:

    /// Configures.
    void configuring() override;
    /// Manually creates a frustum.
    void starting() override;
    /// Deletes the frustum.
    void stopping() override;
    /// Checks if the camera has changed, and updates it if it has.
    void updating() override;

    /// Set data to Ogre::Camera
    void setOgreCamFromData();

    /// Ogre's camera used to displayed frustum
    Ogre::Camera* m_ogreCam;
    /// Adaptor to create an ogre Ogre::Material from fwData::Material.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;
    /// Frustum's visibility
    bool m_visibility;
    /// Near clipping
    float m_near;
    /// Far clipping
    float m_far;
    /// color of frustum
    std::string m_color;
};

} //namespace visuOgreAdaptor
