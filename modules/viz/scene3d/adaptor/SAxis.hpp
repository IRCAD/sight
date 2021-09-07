/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <core/com/Slot.hpp>

#include <viz/scene3d/IAdaptor.hpp>
#include <viz/scene3d/ITransformable.hpp>
#include <viz/scene3d/Text.hpp>

#include <Ogre.h>

namespace sight::data
{

class Material;

}

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptor shows a simple coordinate system.
 *
 * @section Slots Slots
 * - \b updateVisibility(bool): sets whether the axis is shown or not.
 * - \b toggleVisibility(): toggle whether the axis is shown or not.
 * - \b show(): shows the axis.
 * - \b hide(): hides the axis.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::viz::scene3d::adaptor::SAxis">
        <config layer="default" transform="transformUID" length="30" label="true" />
    </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the mesh's layer.
 * - \b transform (optional, string, default=""): the name of the Ogre transform node where to attach the mesh, as it
 *      was specified in the STransform adaptor.
 * - \b visible (optional, bool, default=true): the visibility of the axis.
 * - \b origin (optional, bool, default=false): the origin visibility.
 * - \b originColor (optional, hexadecimal, default=#FFFFFF): the color of the axis origin.
 * - \b length (optional, float, default=50.f): axis length in scene units.
 * - \b label (optional, bool, default=true): display axis names.
 * - \b fontSize (optional, unsigned int, default=16): label font size in points.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SAxis final :
    public sight::viz::scene3d::IAdaptor,
    public sight::viz::scene3d::ITransformable
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SAxis, ::sight::viz::scene3d::IAdaptor);

    /// Sets default parameters and initializes necessary members.
    MODULE_VIZ_SCENE3D_API SAxis() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SAxis() noexcept override;

protected:

    /// Configures the adaptor.
    MODULE_VIZ_SCENE3D_API void configuring() override;

    /// Creates manual objects in the default ogre resource group.
    MODULE_VIZ_SCENE3D_API void starting() override;

    /// Sends a render request.
    MODULE_VIZ_SCENE3D_API void updating() override;

    /// Deletes ogre's resources.
    MODULE_VIZ_SCENE3D_API void stopping() override;

    /**
     * @brief Sets the axis visibility.
     * @param _visible the visibility status of the axis.
     */
    MODULE_VIZ_SCENE3D_API void setVisible(bool _visible) override;

private:

    /// Contains the material data.
    data::Material::sptr m_material {nullptr};

    /// Defines the axis length in scene units.
    float m_length {50.f};

    /// Enables the visibility of axis labels.
    bool m_enableLabel {true};

    /// Contains the origin of the axis.
    ::Ogre::ManualObject* m_origin {nullptr};

    /// Enables the origin visibility.
    bool m_originVisibility {false};

    /// Defines the origin color.
    std::string m_originColor {"#FFFFFF"};

    /// Contains the line along the x axis.
    ::Ogre::ManualObject* m_xLine {nullptr};

    /// Contains the line along the y axis.
    ::Ogre::ManualObject* m_yLine {nullptr};

    /// Contains the line along the z axis.
    ::Ogre::ManualObject* m_zLine {nullptr};

    /// Contains the arrow of the x axis.
    ::Ogre::ManualObject* m_xCone {nullptr};

    /// Contains the arrow of the y axis.
    ::Ogre::ManualObject* m_yCone {nullptr};

    /// Contains the arrow of the z axis.
    ::Ogre::ManualObject* m_zCone {nullptr};

    /// Contains the scene node where all of manual objects are attached.
    ::Ogre::SceneNode* m_sceneNode {nullptr};

    /// Stores labels attached to each axis.
    std::array<sight::viz::scene3d::Text*, 3> m_axisLabels {{nullptr, nullptr, nullptr}};

    /// Defines labels font size in points.
    size_t m_fontSize {16};

    /// Defines the TrueType font source file.
    std::string m_fontSource {"DejaVuSans.ttf"};
};

} // namespace sight::module::viz::scene3d::adaptor.
