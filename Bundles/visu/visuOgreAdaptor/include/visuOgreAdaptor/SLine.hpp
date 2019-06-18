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
        <config layer="default" transform="transformUID" length="30" width="2.5" color="#0000FF" dotted="false" />
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the line's layer
 * - \b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor
 * - \b length (optional): (float) length of the line in mm (default 50)
 * - \b width (optional): (float) size from the hole in the line
 * - \b color (optional): (string) color of the line
 * - \b dotted (optional): (bool) dotted point
 *
 */

class VISUOGREADAPTOR_CLASS_API SLine : public ::fwRenderOgre::IAdaptor,
                                        public ::fwRenderOgre::ITransformable
{
public:
    fwCoreServiceClassDefinitionsMacro((SLine)(::fwRenderOgre::IAdaptor))

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
    VISUOGREADAPTOR_API void updateLength(float length);

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

    /// Draw a line
    void drawLine(bool exsitingLine);

    /// Set the material
    void setMaterial();

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
    ::fwData::Material::sptr m_material {nullptr};

    /// Material Adaptor
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor {nullptr};

    /// ManualObject defining the SLine
    ::Ogre::ManualObject* m_line {nullptr};
    /// Handles the length of the line (in mm)
    float m_length {50.f};
    /// Handles the color of the line
    ::Ogre::ColourValue m_color;
    /// Handles the visibility of the line
    bool m_isVisible {true};
    /// Dotted line
    bool m_dotted {false};
    /// Width for dotted line
    float m_width {2.5f};

};

} //namespace visuOgreAdaptor
