/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

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
 * @brief This adaptor shows a simple line.
 *
 * @section Slots Slots
 * -\b updateVisibility(bool): Sets whether the line is shown or not.
 * -\b toggleVisibility(): Toggle whether the line is shown or not.
 * -\b updateLength(float): Update the line length
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SLine">
        <config layer="default" transform="transformUID" length="30" color="#0000FF"/>
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * -\b layer (mandatory): defines the mesh's layer
 * -\b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor
 * -\b length (optional): (float) length of the line in mm (default 50)
 *
 */
class VISUOGREADAPTOR_CLASS_API SLine : public ::fwRenderOgre::IAdaptor,
                                        public ::fwRenderOgre::ITransformable
{
public:
    fwCoreServiceClassDefinitionsMacro((SLine)(::fwRenderOgre::IAdaptor));

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SLine() noexcept;
    /// Destructor: Does nothing
    VISUOGREADAPTOR_API virtual ~SLine() noexcept;

    /// Slot to set visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void (bool)> UpdateVisibilitySlotType;

    /// Slot to toggle visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;
    typedef ::fwCom::Slot<void ()> ToggleVisibilitySlotType;

    /// Returns if the line is visible in the scene or not.
    VISUOGREADAPTOR_API bool getVisibility() const;

    /// Slot to set the line length
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_LENGTH_SLOT;
    typedef ::fwCom::Slot<void (float)> UpdateLengthSlotType;

    /// Slot: update length of the line
    VISUOGREADAPTOR_API void updateLength (float length );

protected:
    /// Configures the adaptor
    void configuring() override;
    /// Manually creates a Mesh in the Default Ogre Ressource group
    void starting() override;
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;
    /// Checks if the fwData::Mesh has changed, and updates it if it has.
    void updating() override;

private:
    /// Attach a node in the scene graph
    void attachNode(::Ogre::MovableObject* _node);

    /**
     * @name Slots methods
     * @{
     */
    /// Slot: sets visibility of the line
    void updateVisibility(bool isVisible);
    /// Slot: toggles visibility of line
    void toggleVisibility();
    /** @} */

    /// Pointer to the Material data
    ::fwData::Material::sptr m_material;

    /// ManualObject defining the SLine
    ::Ogre::ManualObject* m_line;
    /// Handles the length of the line (in mm)
    float m_length;
    /// Handles the color of the line
    ::Ogre::ColourValue m_color;
    /// Handles the visibility of the line
    bool m_isVisible;

};

} //namespace visuOgreAdaptor
