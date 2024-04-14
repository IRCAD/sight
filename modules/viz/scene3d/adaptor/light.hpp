/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/transform.hpp"

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/material.hpp>

#include <viz/scene3d/light_adaptor.hpp>

#include <OGRE/OgreMovableObject.h>

namespace Ogre
{

class Light;

} // namespace Ogre

namespace sight::module::viz::scene3d::adaptor
{

/**
 * @brief This adaptors adds a light to the scene manager.
 *
 * @section Slots Slots
 * - \b setThetaOffset(float): called when the theta offset is changed and moves the light accordingly.
 * - \b setPhiOffset(float): called when the phi offset is changed and moves the light accordingly.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="lightAdaptor" type="sight::module::viz::scene3d::adaptor::light">
        <inout key="diffuseColor" uid="diffuseColorUid" />
        <inout key="specularColor" uid="specularColorUid" />
        <config name="sceneLight" transform="..." switchedOn="true" thetaOffset="30.5" phiOffset="45" />
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b diffuseColor [sight::data::color]: diffuse color of the light.
 * - \b specularColor [sight::data::color]: specular color of the light.
 *
 * @subsection Configuration Configuration:
 * - \b name (mandatory, string): defines a name for the associated Ogre light.
 * - \b transform (optional, string, default=""): transform applied to the frustum's scene node.
 * - \b switchedOn (optional, bool, default=true): defines if the light is activated or not.
 * - \b thetaOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around x axis.
 * - \b phiOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around y axis.
 */
class light final : public sight::viz::scene3d::light_adaptor
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(light, sight::viz::scene3d::light_adaptor);

    /// Creates the service.
    light() noexcept;

    /// Destroys the adaptor.
    ~light() noexcept final = default;

    /**
     * @brief Gets the name of the light.
     * @return The name of the light.
     */
    const std::string& get_name() const final;

    /**
     * @brief Sets the name of the light.
     * @param _name the new light name.
     */
    void set_name(const std::string& _name) final;

    /**
     * @brief Gets the type of the light.
     * @return The light type.
     */
    Ogre::Light::LightTypes type() const final;

    /**
     * @brief Sets the type of the light.
     * @param _type the new light type.
     */
    void set_type(Ogre::Light::LightTypes _type) final;

    /**
     * @brief Gets the diffuse color of the light.
     * @return The light diffuse color.
     */
    Ogre::ColourValue get_diffuse_color() const final;

    /**
     * @brief Sets the diffuse color of the light.
     * @param _diffuse_color the new light diffuse color.
     */
    void set_diffuse_color(Ogre::ColourValue _diffuse_color) final;

    /**
     * @brief Gets the specular color of the light.
     * @return The light specular color.
     */
    Ogre::ColourValue get_specular_color() const final;

    /**
     * @brief Sets the specular color of the light.
     * @param _specular_color the new light specular color.
     */
    void set_specular_color(Ogre::ColourValue _specular_color) final;

    /**
     * @brief Gets the light activation state.
     * @return The light activation state.
     */
    bool is_switched_on() const final;

    /**
     * @brief Sets the light activation state.
     * @param _on the light new activation state.
     */
    void switch_on(bool _on) final;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around x axis.
     * @return The theta offset of the light.
     */
    float get_theta_offset() const final;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around x axis.
     * @param _theta_offset the value of the theta offset.
     * @pre The type of the light must be Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    void set_theta_offset(float _theta_offset) final;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around y axis.
     * @return The phi of the light.
     */
    float get_phi_offset() const final;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around y axis.
     * @param _phi_offset the phi of the theta offset.
     * @pre The type of the light must be Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    void set_phi_offset(float _phi_offset) final;

    /**
     * @brief Enables the light visual feedback.
     * @param _enable the visual feedback visibility state.
     */
    void enable_visual_feedback(bool _enable) final;

    /**
     * @brief Indicates if the visual feedback is enabled.
     * @return True if the visual feedback is activated.
     */
    bool is_visual_feedback_on() const final;

protected:

    /// Configures the service.
    void configuring() final;

    /// Adds a new light to the scene manager.
    void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::color::MODIFIED_SIG of s_DIFFUSE_COLOR_INOUT to service::slots::UPDATE
     * Connect data::color::MODIFIED_SIG of s_DIFFUSE_COLOR_INOUT to service::slots::UPDATE
     */
    service::connections_t auto_connections() const final;

    /// Updates the light position and orientation.
    void updating() final;

    /// Removes the light from the scene manager.
    void stopping() final;

private:

    /// Contains the Ogre light managed by this adaptor.
    Ogre::Light* m_light {nullptr};

    /// Defines the name of the associated Ogre light.
    std::string m_light_name;

    /// Sets the type of the associated Ogre light.
    Ogre::Light::LightTypes m_light_type {Ogre::Light::LT_DIRECTIONAL};

    /// Enables the light.
    bool m_switched_on {true};

    /// Defines the angle in degrees defining the rotation of the light around x axis.
    float m_theta_offset {0.F};

    /// Defines the angle in degrees defining the rotation of the light around y axis.
    float m_phi_offset {0.F};

    /// Contains the node used to attach the light
    Ogre::SceneNode* m_light_node {nullptr};

    /// Defines the visual feedback visibility state.
    bool m_visual_feedback {false};

    /// Contains the material used for the feedback visualization.
    data::material::sptr m_material {nullptr};

    /// Contains the sphere at the light position.
    Ogre::ManualObject* m_light_position {nullptr};

    /// Defines the length of the visual feedback object.
    float m_length {50.F};

    /// Contains objects used for the directional light visual feedback.
    std::pair<Ogre::ManualObject*, Ogre::ManualObject*> m_directional_feedback {nullptr, nullptr};
};

//------------------------------------------------------------------------------

inline const std::string& light::get_name() const
{
    return m_light_name;
}

//------------------------------------------------------------------------------

inline void light::set_name(const std::string& _name)
{
    m_light_name = _name;
}

//------------------------------------------------------------------------------

inline Ogre::Light::LightTypes light::type() const
{
    return m_light_type;
}

//------------------------------------------------------------------------------

inline Ogre::ColourValue light::get_diffuse_color() const
{
    return m_light->getDiffuseColour();
}

//------------------------------------------------------------------------------

inline Ogre::ColourValue light::get_specular_color() const
{
    return m_light->getSpecularColour();
}

//------------------------------------------------------------------------------

inline bool light::is_switched_on() const
{
    return m_switched_on;
}

//------------------------------------------------------------------------------

inline float light::get_theta_offset() const
{
    return m_theta_offset;
}

//------------------------------------------------------------------------------

inline float light::get_phi_offset() const
{
    return m_phi_offset;
}

//------------------------------------------------------------------------------

inline bool light::is_visual_feedback_on() const
{
    return m_visual_feedback;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
