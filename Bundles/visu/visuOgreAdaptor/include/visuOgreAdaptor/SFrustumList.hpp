/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SFrustum.hpp"

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <boost/circular_buffer.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief SFrustumList displays a new Frustum each times the transform is updated.
 * The number of Frustum is fixed, if the maximum number of Frustum is reached the oldest one will be replaced.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): Sets whether frustums are shown or not.
 * - \b toggleVisibility(): Toggles whether frustums are shown or not.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::SFrustumList">
            <in key="camera" uid="..." />
            <in key="transform" uid="..." />
            <config layer="default" near="0.1" far="300" color="#f8e119" />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b camera [::arData::Camera]: ::arData::Camera that handles calibration parameters
 * - \b transform [::fwData::TransformationMatrix3D]: each time this transform is modified, a frustum is created.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the frustum's layer
 * - \b near (optional): near clipping distance of the ::Ogre::Camera
 * - \b far (optional): far clipping distance of the ::Ogre::Camera
 * - \b color (optional): color of frustums
 */
class VISUOGREADAPTOR_CLASS_API SFrustumList : public ::fwRenderOgre::IAdaptor,
                                               public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro((SFrustumList)(::fwRenderOgre::IAdaptor));

    /// Constructor.
    VISUOGREADAPTOR_API SFrustumList() noexcept;

    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SFrustumList() noexcept;

    /**
     * @name Slots API
     * @{
     */
    ///Clear all frustums slot
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CLEAR_SLOT;

    /// Add new frustum slot (connected to the ::fwData::TransformationMatrix3D::s_MODIFIED_SIG signal)
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_ADD_FRUSTUM_SLOT;

    /// Slot to enable/disable visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;

    /// Slot toggling visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;

    /** @} */

    /// Connects ::fwData::TransformationMatrix3D::s_MODIFIED_SIG to the addFrustum slot
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Key for camera
    static const std::string s_CAMERA_NAME_INPUT;
    /// Key for transform
    static const std::string s_TRANSFORM_INPUT;

protected:
    /// configures the adaptor
    VISUOGREADAPTOR_API void configuring() override;

    /// starts the adaptor and initializes material
    VISUOGREADAPTOR_API void starting() override;

    /// stops the adaptor and clears data
    VISUOGREADAPTOR_API void stopping() override;

    /// updates the adaptor by attaching new cameras to scene nodes (called after addFrustum slot)
    VISUOGREADAPTOR_API void updating() override;

private:

    ///Slot: Clears frustum list
    void clear();
    ///Slot: Shows/hides all frustums
    void updateVisibility(bool);
    ///Slot: Switches visibilty of frustums
    void toggleVisibility();
    ///Slot: Adds a frustum in the list and displays it
    void addFrustum();

    /// Iterates over frustums to change their visibility
    void updateAllVisibility();

    /// Handles current visibilty
    bool m_visibility;
    /// Near clipping distance
    float m_near;
    /// Far clipping distance
    float m_far;
    /// Frustums color (default blue)
    std::string m_color;
    /// Maximum capacity of frustum list
    unsigned int m_capacity;
    /// Circular list of frustum adaptors
    ::boost::circular_buffer< ::Ogre::Camera* > m_frustumList;
    /// Used to generate unique ID for each ::Ogre::Camera.
    size_t m_currentCamIndex;
    /// Adaptor to create an ogre ::Ogre::Material from ::fwData::Material.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;
    /// Pointer to the Material data
    ::fwData::Material::sptr m_material;
    /// The scene node where all frustums are attached
    ::Ogre::SceneNode* m_sceneNode { nullptr };
};

} // namespace visuOgreAdaptor
