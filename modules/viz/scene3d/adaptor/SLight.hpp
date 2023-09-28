/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/viz/scene3d/adaptor/STransform.hpp"
#include "modules/viz/scene3d/config.hpp"

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/Material.hpp>

#include <viz/scene3d/ILight.hpp>

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
    <service uid="lightAdaptor" type="sight::module::viz::scene3d::adaptor::SLight">
        <inout key="diffuseColor" uid="diffuseColorUid" />
        <inout key="specularColor" uid="specularColorUid" />
        <config name="sceneLight" transform="..." switchedOn="true" thetaOffset="30.5" phiOffset="45" />
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b diffuseColor [sight::data::Color]: diffuse color of the light.
 * - \b specularColor [sight::data::Color]: specular color of the light.
 *
 * @subsection Configuration Configuration:
 * - \b name (mandatory, string): defines a name for the associated Ogre light.
 * - \b transform (optional, string, default=""): transform applied to the frustum's scene node.
 * - \b switchedOn (optional, bool, default=true): defines if the light is activated or not.
 * - \b thetaOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around x axis.
 * - \b phiOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around y axis.
 */
class MODULE_VIZ_SCENE3D_CLASS_API SLight final : public sight::viz::scene3d::ILight
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(SLight, sight::viz::scene3d::ILight);

    /// Creates the service.
    MODULE_VIZ_SCENE3D_API SLight() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE3D_API ~SLight() noexcept final = default;

    /**
     * @brief Gets the name of the light.
     * @return The name of the light.
     */
    MODULE_VIZ_SCENE3D_API const std::string& getName() const final;

    /**
     * @brief Sets the name of the light.
     * @param _name the new light name.
     */
    MODULE_VIZ_SCENE3D_API void setName(const std::string& _name) final;

    /**
     * @brief Gets the type of the light.
     * @return The light type.
     */
    MODULE_VIZ_SCENE3D_API Ogre::Light::LightTypes getType() const final;

    /**
     * @brief Sets the type of the light.
     * @param _type the new light type.
     */
    MODULE_VIZ_SCENE3D_API void setType(Ogre::Light::LightTypes _type) final;

    /**
     * @brief Gets the diffuse color of the light.
     * @return The light diffuse color.
     */
    MODULE_VIZ_SCENE3D_API Ogre::ColourValue getDiffuseColor() const final;

    /**
     * @brief Sets the diffuse color of the light.
     * @param _diffuseColor the new light diffuse color.
     */
    MODULE_VIZ_SCENE3D_API void setDiffuseColor(Ogre::ColourValue _diffuseColor) final;

    /**
     * @brief Gets the specular color of the light.
     * @return The light specular color.
     */
    MODULE_VIZ_SCENE3D_API Ogre::ColourValue getSpecularColor() const final;

    /**
     * @brief Sets the specular color of the light.
     * @param _specularColor the new light specular color.
     */
    MODULE_VIZ_SCENE3D_API void setSpecularColor(Ogre::ColourValue _specularColor) final;

    /**
     * @brief Gets the light activation state.
     * @return The light activation state.
     */
    MODULE_VIZ_SCENE3D_API bool isSwitchedOn() const final;

    /**
     * @brief Sets the light activation state.
     * @param _on the light new activation state.
     */
    MODULE_VIZ_SCENE3D_API void switchOn(bool _on) final;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around x axis.
     * @return The theta offset of the light.
     */
    MODULE_VIZ_SCENE3D_API float getThetaOffset() const final;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around x axis.
     * @param _thetaOffset the value of the theta offset.
     * @pre The type of the light must be Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    MODULE_VIZ_SCENE3D_API void setThetaOffset(float _thetaOffset) final;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around y axis.
     * @return The phi of the light.
     */
    MODULE_VIZ_SCENE3D_API float getPhiOffset() const final;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around y axis.
     * @param _phiOffset the phi of the theta offset.
     * @pre The type of the light must be Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    MODULE_VIZ_SCENE3D_API void setPhiOffset(float _phiOffset) final;

    /**
     * @brief Enables the light visual feedback.
     * @param _enable the visual feedback visibility state.
     */
    MODULE_VIZ_SCENE3D_API void enableVisualFeedback(bool _enable) final;

    /**
     * @brief Indicates if the visual feedback is enabled.
     * @return True if the visual feedback is activated.
     */
    MODULE_VIZ_SCENE3D_API bool isVisualFeedbackOn() const final;

protected:

    /// Configures the service.
    MODULE_VIZ_SCENE3D_API void configuring() final;

    /// Adds a new light to the scene manager.
    MODULE_VIZ_SCENE3D_API void starting() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::Color::MODIFIED_SIG of s_DIFFUSE_COLOR_INOUT to service::slots::UPDATE
     * Connect data::Color::MODIFIED_SIG of s_DIFFUSE_COLOR_INOUT to service::slots::UPDATE
     */
    MODULE_VIZ_SCENE3D_API service::connections_t getAutoConnections() const final;

    /// Updates the light position and orientation.
    MODULE_VIZ_SCENE3D_API void updating() final;

    /// Removes the light from the scene manager.
    MODULE_VIZ_SCENE3D_API void stopping() final;

private:

    /// Contains the Ogre light managed by this adaptor.
    Ogre::Light* m_light {nullptr};

    /// Defines the name of the associated Ogre light.
    std::string m_lightName;

    /// Sets the type of the associated Ogre light.
    Ogre::Light::LightTypes m_lightType {Ogre::Light::LT_DIRECTIONAL};

    /// Enables the light.
    bool m_switchedOn {true};

    /// Defines the angle in degrees defining the rotation of the light around x axis.
    float m_thetaOffset {0.F};

    /// Defines the angle in degrees defining the rotation of the light around y axis.
    float m_phiOffset {0.F};

    /// Contains the node used to attach the light
    Ogre::SceneNode* m_lightNode {nullptr};

    /// Defines the visual feedback visibility state.
    bool m_visualFeedback {false};

    /// Contains the material used for the feedback visualization.
    data::Material::sptr m_material {nullptr};

    /// Contains the sphere at the light position.
    Ogre::ManualObject* m_lightPosition {nullptr};

    /// Defines the length of the visual feedback object.
    float m_length {50.F};

    /// Contains objects used for the directional light visual feedback.
    std::pair<Ogre::ManualObject*, Ogre::ManualObject*> m_directionalFeedback {nullptr, nullptr};
};

//------------------------------------------------------------------------------

inline const std::string& SLight::getName() const
{
    return m_lightName;
}

//------------------------------------------------------------------------------

inline void SLight::setName(const std::string& _name)
{
    m_lightName = _name;
}

//------------------------------------------------------------------------------

inline Ogre::Light::LightTypes SLight::getType() const
{
    return m_lightType;
}

//------------------------------------------------------------------------------

inline Ogre::ColourValue SLight::getDiffuseColor() const
{
    return m_light->getDiffuseColour();
}

//------------------------------------------------------------------------------

inline Ogre::ColourValue SLight::getSpecularColor() const
{
    return m_light->getSpecularColour();
}

//------------------------------------------------------------------------------

inline bool SLight::isSwitchedOn() const
{
    return m_switchedOn;
}

//------------------------------------------------------------------------------

inline float SLight::getThetaOffset() const
{
    return m_thetaOffset;
}

//------------------------------------------------------------------------------

inline float SLight::getPhiOffset() const
{
    return m_phiOffset;
}

//------------------------------------------------------------------------------

inline bool SLight::isVisualFeedbackOn() const
{
    return m_visualFeedback;
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
