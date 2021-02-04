/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/viz/ogre/adaptor/SMaterial.hpp"
#include "modules/viz/ogre/config.hpp"

#include <Ogre.h>

#include <viz/ogre/IAdaptor.hpp>
#include <viz/ogre/ITransformable.hpp>

namespace sight::data
{
class Material;
}

namespace sight::modules::viz::ogre::adaptor
{

/**
 * @brief This adaptor shows a simple line.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): Sets whether the line is shown or not.
 * - \b toggleVisibility(): Toggle whether the line is shown or not.
 * - \b show(): shows the line.
 * - \b hide(): hides the line.
 * - \b updateLength(float): Update the line length
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::sight::modules::viz::ogre::adaptor::SLine">
        <config layer="default" transform="transformUID" length="30" dashLength="2.5" color="#0000FF" dashed="false" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the line's layer
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the STransform adaptor
 * - \b length (optional, float, default=50.0): length of the line in mm (default 50)
 * - \b dashLength (optional, float, default=2.5): length of a dash
 * - \b color (optional, hexadecimal, default=#FFFFFF): color of the line
 * - \b dashed (optional, bool, default=false): display a dashed line instead of a solid line
 * - \b visible (optional, bool, default=true): the visibility of the adaptor.
 */
class MODULE_VIZ_OGRE_CLASS_API SLine final :
    public sight::viz::ogre::IAdaptor,
    public sight::viz::ogre::ITransformable
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SLine, ::sight::viz::ogre::IAdaptor)

    /// Sets default parameters and initializes necessary members.
    MODULE_VIZ_OGRE_API SLine() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_OGRE_API ~SLine() noexcept override;

protected:

    /// Configures the adaptor
    MODULE_VIZ_OGRE_API void configuring() override;

    /// Creates a mesh in the Default Ogre resource group
    MODULE_VIZ_OGRE_API void starting() override;

    /// Checks if the data::Mesh has changed, and updates it if it has.
    MODULE_VIZ_OGRE_API void updating() override;

    /// Deletes the mesh after unregistering the service, and shutting connections.
    MODULE_VIZ_OGRE_API void stopping() override;

    /**
     * @brief Sets the line visibility.
     * @param _visible the visibility status of the line.
     */
    MODULE_VIZ_OGRE_API void setVisible(bool _visible) override;

private:

    /**
     * @brief Attachs a node in the scene graph.
     * @param _node node to attach.
     */
    void attachNode(::Ogre::MovableObject* _node);

    /**
     * @brief Draws a line.
     * @param _existingLine use true if the line already exist.
     */
    void drawLine(bool _existingLine);

    /**
     * @brief SLOT: updates length of the line.
     * @param _length length of the line (in mm).
     */
    void updateLength(float _length);

    /// Contains the Ogre material adaptor.
    modules::viz::ogre::adaptor::SMaterial::sptr m_materialAdaptor { nullptr };

    /// Contains the material data.
    data::Material::sptr m_material { nullptr };

    /// Contains the manual object of the line.
    ::Ogre::ManualObject* m_line { nullptr };

    /// Defines the length of the line (in mm).
    float m_length { 50.f };

    /// Defines the color of the line.
    ::Ogre::ColourValue m_color;

    /// Enables if the line is dashed or not.
    bool m_dashed { false };

    /// Defines the length of one dash.
    float m_dashLength { 2.5f };

};

} // namespace sight::modules::viz::ogre::adaptor.
