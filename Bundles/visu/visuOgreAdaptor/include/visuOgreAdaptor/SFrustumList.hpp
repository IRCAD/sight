/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"
#include <visuOgreAdaptor/SFrustum.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <boost/circular_buffer.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief SFrustumList display a new Frustum each times the transform is updated.
 * The number of Frustum is fixed, if the maximum number of Frustum is reached the older one will be replaced.
 *
 * @section Slots Slots
 * - \b UpdateVisibilitySlotType(bool): update visibility of frustums with the boolean parameter
 * - \b ToggleVisibilitySlotType(): switch visibility from previsous state.

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
 * -\b layer (mandatory): defines the frustum's layer
 * -\b near (optional): near clipping of the Ogre::Camera
 * -\b far (optional): far clipping of the Ogre::Camera
 * -\b color (optional): frustums's color
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

    /// Add new frustum slot (connected to the TransformationMatrix3D::Modified signal)
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_ADD_FRUSTUM_SLOT;

    /// Slot for enable/disable visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;

    /// Slot for toggle visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;

    /** @} */

    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:
    /// This method is used to configure the service.
    VISUOGREADAPTOR_API void configuring() override;

    /// Do nothing.
    VISUOGREADAPTOR_API void starting() override;

    /// Do nothing.
    VISUOGREADAPTOR_API void stopping() override;

    /// Do nothing.
    VISUOGREADAPTOR_API void updating() override;

private:

    /// clear frustum list
    void clear();
    /// Sets visibility of the frustums
    void updateVisibility(bool);
    /// Toggle visibility of the frustums
    void toggleVisibility();

    /// Adds a frustum in the list and display it
    void addFrustum();

    /// iterate over frustums to change their visibility
    void updateAllVisibility();

    /// Visibility
    bool m_visibility;
    /// Near clipping
    float m_near;
    /// Far clipping
    float m_far;
    /// color of frustum
    std::string m_color;

    /// max capacity of frustum list
    unsigned int m_capacity;
    /// circular list of frustum adaptors
    ::boost::circular_buffer< ::Ogre::Camera* > m_frustumList;

    /// Used to generate unique ID for each Ogre::Camera.
    size_t m_currentCamIndex;

    /// Adaptor to create an ogre Ogre::Material from fwData::Material.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;

};

} // namespace visuOgreAdaptor
