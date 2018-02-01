/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwCom/Slot.hpp>

#include <fwRenderOgre/IAdaptor.hpp>
#include <fwRenderOgre/ITransformable.hpp>

#include <Ogre.h>

namespace fwData
{
class Material;
}

namespace visuOgreAdaptor
{

/**
 * @brief This adaptor shows a simple coordinate system.
 *
 * @section Slots Slots
 * -\b updateVisibility(bool): Sets whether the axis is shown or not.
 * -\b toggleVisibility(): Toggle whether the axis is shown or not.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SAxis">
        <config layer="default" transform="transformUID" length="30" />
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * -\b layer (mandatory): defines the mesh's layer
 * -\b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor
 * -\b length (optional): (float) length of the axis in mm (default 50)
 *
 */
class VISUOGREADAPTOR_CLASS_API SAxis : public ::fwRenderOgre::IAdaptor,
                                        public ::fwRenderOgre::ITransformable
{
public:
    fwCoreServiceClassDefinitionsMacro((SAxis)(::fwRenderOgre::IAdaptor));

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SAxis() noexcept;
    /// Destructor: Does nothing
    VISUOGREADAPTOR_API virtual ~SAxis() noexcept;

    /// Slot to set visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void (bool)> UpdateVisibilitySlotType;
    /// Slot to toggle visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void ()> ToggleVisibilitySlotType;

    /// Sets visibility of axis
    VISUOGREADAPTOR_API void updateVisibility(bool isVisible);
    /// Toggle visibility of axis
    VISUOGREADAPTOR_API void toggleVisibility();
    /// Returns if the axis is visible in the scene or not.
    VISUOGREADAPTOR_API bool getVisibility() const;
    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Key for length
    VISUOGREADAPTOR_API static const std::string s_CONFIG_LENGTH;

private:

    /// Configures the adaptor
    void configuring() override;
    /// Manually creates a Mesh in the Default Ogre Ressource group
    void starting() override;
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;
    /// Checks if the fwData::Mesh has changed, and updates it if it has.
    void updating() override;

    /// Attach a node in the scene graph
    void attachNode(::Ogre::MovableObject* _node);

    /// Pointer to a SMaterial adaptor
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;
    /// Pointer to the Material data
    ::fwData::Material::sptr m_material;
    /// Handle the length of each axes (in mm)
    float m_length;
    /// Handles the visibility of the axis
    bool m_isVisible;

};

} //namespace visuOgreAdaptor
