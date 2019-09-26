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
 * -\b updateLength(float): set the length of the vector.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SVector">
        <config layer="..." transform="..." length="30" color="#FFFFFF" />
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the mesh's layer.
 * - \b transform (optional): the name of the Ogre transform node where to attach the mesh, as it was specified
 * in the STransform adaptor.
 * - \b length (optional, default=1.f): axis length in mm.
 * - \b color (optional, default=#FFFFFF): color of the vector.
 *
 */
class VISUOGREADAPTOR_CLASS_API SVector : public ::fwRenderOgre::IAdaptor,
                                          public ::fwRenderOgre::ITransformable
{
public:

    fwCoreServiceClassDefinitionsMacro((SVector)(::fwRenderOgre::IAdaptor))

    /**
     * @name Slots API
     * @{
     */
    /// Slot used to set visibility.
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT;

    /// Slot used to toggle visibility.
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_TOGGLE_VISIBILITY_SLOT;

    /// Slot used to update length of the line.
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_LENGTH_SLOT;
    /**
     * @}
     */

    /// Initialise slots.
    VISUOGREADAPTOR_API SVector() noexcept;

    /// Does nothing.
    VISUOGREADAPTOR_API virtual ~SVector() noexcept;

private:

    /// Configures the adaptor
    void configuring() override;

    /// Initialize the main node and the material.
    void starting() override;

    /// Deletes resources.
    void stopping() override;

    /// Create with the specified length the vector.
    void updating() override;

    /**
     * @brief Sets whether the vector is to be seen or not.
     * @param _isVisible Set to true to show the vector.
     */
    void updateVisibility(bool _isVisible);

    /// Toogle the vector visibility.
    void toggleVisibility();

    /**
     * @brief Returns proposals to connect service slots to associated object signals.
     * @return An empty connection map.
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /**
     * @brief Update the length of the vector.
     * @param _length Length of the vector
     */
    void updateLength(float _length);

    /// Create the vector.
    void createVector();

    /// Delete resources.
    void deleteVector();

    /// Pointer to the Material data
    ::fwData::Material::sptr m_material {nullptr};

    /// Axis length (in mm)
    float m_length {1.f};

    /// Handles axis visibility.
    bool m_isVisible {true};

    /// Color of the vector
    std::string m_color {"#FFFFFF"};

    /// Line along the z axis
    ::Ogre::ManualObject* m_line {nullptr};

    /// Arrow of the z axis
    ::Ogre::ManualObject* m_cone {nullptr};

    /// Scene node where all of our manual objects are attached
    ::Ogre::SceneNode* m_sceneNode {nullptr};

    /// Material used to draw the vector.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor {nullptr};

};

} //namespace visuOgreAdaptor
