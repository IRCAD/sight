/************************************************************************
 *
 * Copyright (C) 2019-2020 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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
 * - \b updateVisibility(bool): Sets whether the vector is shown or not.
 * - \b toggleVisibility(): Toggle whether the vector is shown or not.
 * - \b updateLength(float): set the length of the vector.
 * - \b show(): Shows the vector.
 * - \b hide(): Hides the vector.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::visuOgreAdaptor::SVector">
        <config layer="..." transform="..." length="30" color="#FFFFFF" visible="true"/>
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the mesh's layer.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the STransform adaptor.
 * - \b length (optional, float, default=1.): axis length in mm.
 * - \b color (optional, hexadecimal, default=#FFFFFF): color of the vector.
 * - \b visible (optional, bool, default=true): visibility of the vector.
 */
class VISUOGREADAPTOR_CLASS_API SVector final :
    public ::fwRenderOgre::IAdaptor,
    public ::fwRenderOgre::ITransformable
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SVector, ::fwRenderOgre::IAdaptor)

    /// Initialise slots.
    VISUOGREADAPTOR_API SVector() noexcept;

    /// Does nothing.
    VISUOGREADAPTOR_API ~SVector() noexcept override;

protected:

    /// Configures the adaptor
    VISUOGREADAPTOR_API void configuring() override;

    /// Initialize the main node and the material.
    VISUOGREADAPTOR_API void starting() override;

    /// Deletes resources.
    VISUOGREADAPTOR_API void stopping() override;

    /// Create with the specified length the vector.
    VISUOGREADAPTOR_API void updating() override;

    /**
     * @brief Sets the vector visibility.
     * @param _visible the visibility status of the vector.
     */
    VISUOGREADAPTOR_API void setVisible(bool _visible) override;

private:

    /**
     * @brief Updates the length of the vector.
     * @param _length length of the vector.
     */
    void updateLength(float _length);

    /// Create the vector.
    void createVector();

    /// Deletes the vector resources.
    void deleteVector();

    /// Contains the material data.
    ::fwData::Material::sptr m_material {nullptr};

    /// Defines the axis length (in mm).
    float m_length {1.f};

    /// Defines the color of the vector.
    std::string m_color {"#FFFFFF"};

    /// Contains the line along the z axis.
    ::Ogre::ManualObject* m_line {nullptr};

    /// Contains the arrow of the z axis.
    ::Ogre::ManualObject* m_cone {nullptr};

    /// Contains the scene node where all of our manual objects are attached.
    ::Ogre::SceneNode* m_sceneNode {nullptr};

    /// Contains the material used to draw the vector.
    ::visuOgreAdaptor::SMaterial::sptr m_materialAdaptor {nullptr};

};

} // namespace visuOgreAdaptor.
