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
#include <fwRenderOgre/Text.hpp>

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
        <config layer="default" transform="transformUID" length="30" label="true" />
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b layer (mandatory) : defines the mesh's layer
 * - \b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor
 * - \b length (optional) : (float) axis length in mm (default 50)
 * - \b label (optional) : (boolean) display axis names (default : true)
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

private:

    /// Configures the adaptor
    void configuring() override;
    /// Manually creates a Mesh in the Default Ogre Ressource group
    void starting() override;
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;
    /// Checks if the fwData::Mesh has changed, and updates it if it has.
    void updating() override;

    /// Pointer to the Material data
    ::fwData::Material::sptr m_material {nullptr};
    /// Axis length (in mm)
    float m_length { 50.f };
    /// Handles axis visibility.
    bool m_isVisible { true };
    /// Handles the visibility of axis labels
    bool m_enableLabel { true };

    ::Ogre::ManualObject* xLine { nullptr };
    ::Ogre::ManualObject* yLine { nullptr };
    ::Ogre::ManualObject* zLine { nullptr };

    ::Ogre::ManualObject* xCone { nullptr };
    ::Ogre::ManualObject* yCone { nullptr };
    ::Ogre::ManualObject* zCone { nullptr };

    /// Scene node where all of our manual objects are attached
    ::Ogre::SceneNode* m_sceneNode { nullptr };

    std::array< ::fwRenderOgre::Text*, 3> m_axisLabels {{ nullptr, nullptr, nullptr }};
};

} //namespace visuOgreAdaptor
