/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "visuOgreAdaptor/config.hpp"
#include "visuOgreAdaptor/STransform.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>

#include <fwData/Material.hpp>

#include <fwRenderOgre/ILight.hpp>

#include <OGRE/OgreMovableObject.h>

namespace Ogre
{
class Light;
}

namespace visuOgreAdaptor
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
    <service uid="lightAdaptor" type="::visuOgreAdaptor::SLight">
        <inout key="diffuseColor" uid="diffuseColorUid" />
        <inout key="specularColor" uid="specularColorUid" />
        <config name="sceneLight" transform="..." switchedOn="yes" thetaOffset="30.5" phiOffset="45" />
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b diffuseColor [::fwData::Color]: diffuse color of the light.
 * - \b specularColor [::fwData::Color]: specular color of the light.
 *
 * @subsection Configuration Configuration:
 * - \b layer (mandatory, string): defines the light's layer.
 * - \b name (mandatory, string): defines a name for the associated Ogre light.
 * - \b transform (optional, string, default=""): transform applied to the frustum's scene node.
 * - \b switchedOn (optional, bool, default=true): defines if the light is activated or not.
 * - \b thetaOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around x axis.
 * - \b phiOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around y axis.
 */
class VISUOGREADAPTOR_CLASS_API SLight final : public ::fwRenderOgre::ILight
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SLight, ::fwRenderOgre::ILight)

    /// Creates the service.
    VISUOGREADAPTOR_API SLight() noexcept;

    /// Creates the service.
    VISUOGREADAPTOR_API SLight(::fwRenderOgre::ILight::Key key);

    /// Destroys the adaptor.
    VISUOGREADAPTOR_API ~SLight() noexcept override;

    /**
     * @brief Gets the name of the light.
     * @return The name of the light.
     */
    VISUOGREADAPTOR_API const std::string& getName() const override;

    /**
     * @brief Sets the name of the light.
     * @param _name the new light name.
     */
    VISUOGREADAPTOR_API void setName(const std::string& _name) override;

    /**
     * @brief Gets the type of the light.
     * @return The light type.
     */
    VISUOGREADAPTOR_API ::Ogre::Light::LightTypes getType() const override;

    /**
     * @brief Sets the type of the light.
     * @param _type the new light type.
     */
    VISUOGREADAPTOR_API void setType(::Ogre::Light::LightTypes _type) override;

    /**
     * @brief Gets the diffuse color of the light.
     * @return The light diffuse color.
     */
    VISUOGREADAPTOR_API ::Ogre::ColourValue getDiffuseColor() const override;

    /**
     * @brief Sets the diffuse color of the light.
     * @param _diffuseColor the new light diffuse color.
     */
    VISUOGREADAPTOR_API void setDiffuseColor(::Ogre::ColourValue _diffuseColor) override;

    /**
     * @brief Gets the specular color of the light.
     * @return The light specular color.
     */
    VISUOGREADAPTOR_API ::Ogre::ColourValue getSpecularColor() const override;

    /**
     * @brief Sets the specular color of the light.
     * @param _specularColor the new light specular color.
     */
    VISUOGREADAPTOR_API void setSpecularColor(::Ogre::ColourValue _specularColor) override;

    /**
     * @brief Gets the light activation state.
     * @return The light activation state.
     */
    VISUOGREADAPTOR_API bool isSwitchedOn() const override;

    /**
     * @brief Sets the light activation state.
     * @param _on the light new activation state.
     */
    VISUOGREADAPTOR_API void switchOn(bool _on) override;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around x axis.
     * @return The theta offset of the light.
     */
    VISUOGREADAPTOR_API float getThetaOffset() const override;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around x axis.
     * @param _thetaOffset the value of the thta offset.
     * @pre The type of the light must be ::Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    VISUOGREADAPTOR_API void setThetaOffset(float _thetaOffset) override;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around y axis.
     * @return The phi of the light.
     */
    VISUOGREADAPTOR_API float getPhiOffset() const override;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around y axis.
     * @param _phiOffset the phi of the thta offset.
     * @pre The type of the light must be ::Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    VISUOGREADAPTOR_API void setPhiOffset(float _phiOffset) override;

    /**
     * @brief Enables the light visual feedback.
     * @param _enable the visual feedback visibility state.
     */
    VISUOGREADAPTOR_API void enableVisualFeedback(bool _enable) override;

    /**
     * @brief Indicates if the visual feedback is enabled.
     * @return True if the visual feedback is activated.
     */
    VISUOGREADAPTOR_API bool isVisualFeedbackOn() const override;

protected:

    /// Configures the service.
    VISUOGREADAPTOR_API void configuring() override;

    /// Adds a new light to the scene manager.
    VISUOGREADAPTOR_API void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::TransformationMatrix3D::s_MODIFIED_SIG of s_TRANSFORM_INOUT to s_UPDATE_SLOT
     * Connect ::fwData::Color::s_MODIFIED_SIG of s_DIFFUSE_COLOR_INOUT to s_UPDATE_SLOT
     * Connect ::fwData::Color::s_MODIFIED_SIG of s_DIFFUSE_COLOR_INOUT to s_UPDATE_SLOT
     */
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Updates the light position and orientation.
    VISUOGREADAPTOR_API void updating() override;

    /// Removes the light from the scene manager.
    VISUOGREADAPTOR_API void stopping() override;

private:

    /// Containes the Ogre light managed by this adaptor.
    ::Ogre::Light* m_light { nullptr };

    /// Defines the name of the associated Ogre light.
    std::string m_lightName;

    /// Sets the type of the associated Ogre light.
    ::Ogre::Light::LightTypes m_lightType { ::Ogre::Light::LT_DIRECTIONAL };

    /// Enables the light.
    bool m_switchedOn { true };

    /// Defines the angle in degrees defining the rotation of the light around x axis.
    float m_thetaOffset { 0.f };

    /// Defines the angle in degrees defining the rotation of the light around y axis.
    float m_phiOffset { 0.f };

    /// Contains the node used to attach the light
    ::Ogre::SceneNode* m_lightNode { nullptr };

    /// Defines the visual feedback visibility state.
    bool m_visualFeedback { false };

    /// Contains the material used for the feedback visualization.
    ::fwData::Material::sptr m_material { nullptr };

    /// Contains the sphere at the light position.
    ::Ogre::ManualObject* m_lightPosition { nullptr };

    /// Defines the length of the visual feedback object.
    float m_length { 50.f };

    /// Contains objects used for the directional light visual feedback.
    std::pair< ::Ogre::ManualObject*, ::Ogre::ManualObject* > m_directionalFeedback { nullptr, nullptr };
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

inline ::Ogre::Light::LightTypes SLight::getType() const
{
    return m_lightType;
}

//------------------------------------------------------------------------------

inline ::Ogre::ColourValue SLight::getDiffuseColor() const
{
    return m_light->getDiffuseColour();
}

//------------------------------------------------------------------------------

inline ::Ogre::ColourValue SLight::getSpecularColor() const
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

} // namespace visuOgreAdaptor.
