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
 * - \b addFrustum(bool): Adds a frustum in the list and displays it.
 * - \b clear(): Clears frustum list.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::visuOgreAdaptor::SFrustumList">
            <in key="camera" uid="..." />
            <in key="transform" uid="..." />
            <config layer="default" near="0.1" far="300" color="#f8e119" transform="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b camera [::arData::Camera]: ::arData::Camera that handles calibration parameters
 * - \b transform [::fwData::TransformationMatrix3D]: each time this transform is modified, a frustum is created.
 *
 * @subsection Configuration Configuration:
 * -\b layer (mandatory): defines the frustum's layer
 * -\b near (optional): near clipping distance of the ::Ogre::Camera
 * -\b far (optional): far clipping distance of the ::Ogre::Camera
 * -\b color (optional): color of frustums
 * -\b transform (optional): transform applied to the frustumList's scene node
 */
class VISUOGREADAPTOR_CLASS_API SFrustumList : public ::fwRenderOgre::IAdaptor,
                                               public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceMacro(SFrustumList, ::fwRenderOgre::IAdaptor);

    /**
     * @name Slots API
     * @{
     */
    /// Clear all frustums slot.
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_CLEAR_SLOT;
    /// Adds a frustum in the list and displays it.
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_ADD_FRUSTUM_SLOT;
    /// Enable/disable visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    /// Toggle visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SFrustumList() noexcept;
    /// Destructor.
    VISUOGREADAPTOR_API virtual ~SFrustumList() noexcept;

    /// Connects ::fwData::TransformationMatrix3D::s_MODIFIED_SIG to the s_ADD_FRUSTUM_SLOT slot.
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    /// configures the adaptor.
    VISUOGREADAPTOR_API void configuring() override;
    /// starts the adaptor and initializes material.
    VISUOGREADAPTOR_API void starting() override;
    /// stops the adaptor and clears data.
    VISUOGREADAPTOR_API void stopping() override;
    /// updates the adaptor by attaching new cameras to scene nodes (called after addFrustum slot).
    VISUOGREADAPTOR_API void updating() override;

    /// Slot: Clears frustum list.
    void clear();
    /// Slot: Shows/hides all frustums.
    void updateVisibility(bool);
    /// Slot: Switches visibilty of frustums.
    void toggleVisibility();
    /// Slot: Adds a frustum in the list and displays it.
    void addFrustum();

    /// Iterates over frustums to change their visibility.
    void updateAllVisibility();

    /// Handles current visibilty.
    bool m_visibility {true};
    /// Near clipping distance.
    float m_near {1.f};
    /// Far clipping distance.
    float m_far {20.f};
    /// Frustums color (default blue).
    std::string m_color {"#0000ffff"};
    /// Maximum capacity of frustum list.
    unsigned int m_capacity {50};
    /// Circular list of frustum adaptors.
    ::boost::circular_buffer< ::Ogre::Camera* > m_frustumList {};
    /// Used to generate unique ID for each ::Ogre::Camera.
    size_t m_currentCamIndex {0};
    /// Adaptor to create an ogre ::Ogre::Material from ::fwData::Material.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor {nullptr};
    /// Pointer to the Material data.
    ::fwData::Material::sptr m_material {nullptr};
    /// The scene node where all frustums are attached.
    ::Ogre::SceneNode* m_sceneNode { nullptr };

};

} // namespace visuOgreAdaptor
