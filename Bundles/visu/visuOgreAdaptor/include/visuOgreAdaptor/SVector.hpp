/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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
 * @brief This adaptor shows a simple vector.
 *
 * @section Slots Slots
 * -\b updateVisibility(bool): Sets whether the vector is shown or not.
 * -\b toggleVisibility(): Toggle whether the vector is shown or not.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SVector">
        <config layer="default" transform="transformUID" length="30" color="#FFFFFF" />
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the mesh's layer.
 * - \b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor.
 * - \b length (optional): (float) axis length in mm (default 50).
 * - \b color (optional): (hexadecimal) color of the vector (default #FFFFFF).
 *
 */
class VISUOGREADAPTOR_CLASS_API SVector : public ::fwRenderOgre::IAdaptor,
                                          public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro((SVector)(::fwRenderOgre::IAdaptor))

    /// Constructor: Sets default parameters and initializes necessary members.
    VISUOGREADAPTOR_API SVector() noexcept;
    /// Destructor: Does nothing
    VISUOGREADAPTOR_API virtual ~SVector() noexcept;

    /// Slot to set visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;
    /// Slot to toggle visibility
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;
    /// Slot: update length of the line
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_LENGTH_SLOT;

    /// Sets visibility of the vector
    VISUOGREADAPTOR_API void updateVisibility(bool _isVisible);
    /// Toggle visibility of the vector
    VISUOGREADAPTOR_API void toggleVisibility();
    /// Returns proposals to connect service slots to associated object signals
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;
    /// Update the length of the vector.
    VISUOGREADAPTOR_API void updateLength(float _length);

private:

    /// Configures the adaptor
    void configuring() override;
    /// Manually creates a Mesh in the Default Ogre Ressource group
    void starting() override;
    /// Deletes the mesh after unregistering the service, and shutting connections.
    void stopping() override;
    /// Checks if the fwData::Mesh has changed, and updates it if it has.
    void updating() override;

    void createVector();
    void deleteVector();

    /// Pointer to the Material data
    ::fwData::Material::sptr m_material { nullptr };
    /// Axis length (in mm)
    float m_length { 1.f };
    /// Handles axis visibility.
    bool m_isVisible { true };
    /// Color of the vector
    std::string m_color {"#FFFFFF"};

    /// Line along the x axis
    ::Ogre::ManualObject* m_line { nullptr };
    /// Arrow of the x axis
    ::Ogre::ManualObject* m_cone { nullptr };
    /// Scene node where all of our manual objects are attached
    ::Ogre::SceneNode* m_sceneNode { nullptr };
    /// Material used to draw the vector.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor;

};

} //namespace visuOgreAdaptor
