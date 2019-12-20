/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderOgre/ILight.hpp>

#include <OGRE/OgreMovableObject.h>

namespace Ogre
{
class Light;
}

namespace visuOgreAdaptor
{

/**
 * @brief Adaptor for a light.
 *
 * @section Slots Slots
 * - \b setThetaOffset(float): Called when the theta offset is changed and moves the light accordingly.
 * - \b setPhiOffset(float): Called when the phi offset is changed and moves the light accordingly.
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
 * - \b layer (mandatory): defines the light's layer.
 * - \b name (mandatory): defines a name for the associated Ogre light.
 * - \b transform (optional): transform applied to the frustum's scene node.
 * - \b switchedOn (optional, bool, default="yes"): defines if the light is activated or not.
 * - \b thetaOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around x axis.
 * - \b phiOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around y axis.
 */
class VISUOGREADAPTOR_CLASS_API SLight : public ::fwRenderOgre::ILight
{

public:

    fwCoreServiceMacro(SLight, ::fwRenderOgre::ILight)

    /// Creates the service.
    VISUOGREADAPTOR_API SLight() noexcept;

    /// Creates the service.
    VISUOGREADAPTOR_API SLight(::fwRenderOgre::ILight::Key key);

    /// Destroys the service.
    VISUOGREADAPTOR_API virtual ~SLight() noexcept;

    /**
     * @brief Gets the name of the light.
     * @return The name of the light.
     */
    VISUOGREADAPTOR_API virtual const std::string& getName() const override final;

    /**
     * @brief Sets the name of the light.
     * @param _name The new light name.
     */
    VISUOGREADAPTOR_API virtual void setName(const std::string& _name) override final;

    /**
     * @brief Gets the type of the light.
     * @return The light type.
     */
    VISUOGREADAPTOR_API virtual ::Ogre::Light::LightTypes getType() const override final;

    /**
     * @brief Sets the type of the light.
     * @param _type The new light type.
     */
    VISUOGREADAPTOR_API virtual void setType(::Ogre::Light::LightTypes _type) override final;

    /**
     * @brief Gets the diffuse color of the light.
     * @return The light diffuse color.
     */
    VISUOGREADAPTOR_API virtual ::Ogre::ColourValue getDiffuseColor() const override final;

    /**
     * @brief Sets the diffuse color of the light.
     * @param _diffuseColor The new light diffuse color.
     */
    VISUOGREADAPTOR_API void setDiffuseColor(::fwData::Color::sptr _diffuseColor);

    /**
     * @brief Sets the diffuse color of the light.
     * @param _diffuseColor The new light diffuse color.
     */
    VISUOGREADAPTOR_API virtual void setDiffuseColor(::Ogre::ColourValue _diffuseColor) override final;

    /**
     * @brief Gets the specular color of the light.
     * @return The light specular color.
     */
    VISUOGREADAPTOR_API virtual ::Ogre::ColourValue getSpecularColor() const override final;

    /**
     * @brief Sets the specular color of the light.
     * @param _specularColor The new light specular color.
     */
    VISUOGREADAPTOR_API void setSpecularColor(::fwData::Color::sptr _specularColor);

    /**
     * @brief Sets the specular color of the light.
     * @param _specularColor The new light specular color.
     */
    VISUOGREADAPTOR_API virtual void setSpecularColor(::Ogre::ColourValue _specularColor) override final;

    /**
     * @brief Gets the light activation state.
     * @return The light activation state.
     */
    VISUOGREADAPTOR_API virtual bool isSwitchedOn() const override final;

    /**
     * @brief Sets the light activation state.
     * @param _on The light new activation state.
     */
    VISUOGREADAPTOR_API virtual void switchOn(bool _on) override final;

    /**
     * @brief Get the theta offset of the light (used for directional light).
     * @return The theta offset of the light.
     */
    VISUOGREADAPTOR_API virtual float getThetaOffset() const override final;

    /**
     * @brief Sets the theta offset of the light (used for directional light).
     * @param _thetaOffset The new theta offset of the light.
     */
    VISUOGREADAPTOR_API virtual void setThetaOffset(float _thetaOffset) override final;

    /**
     * @brief Get the phi offset of the light (used for directional light).
     * @return The phi offset of the light.
     */
    VISUOGREADAPTOR_API virtual float getPhiOffset() const override final;

    /**
     * @brief Sets the phi offset of the light (used for directional light).
     * @param _phiOffset The new phi offset of the light.
     */
    VISUOGREADAPTOR_API virtual void setPhiOffset(float _phiOffset) override final;

    /**
     * @brief Indicates if the light is attached to a parent node or not.
     * @return True if the light is attached.
     */
    [[deprecated("Deprecated method. Removed in sight 21.0")]]
    VISUOGREADAPTOR_API virtual bool isOrphanNode() const override final;

    /// Parent tranform id setter.
    [[deprecated("Deprecated method. Removed in sight 21.0")]]
    VISUOGREADAPTOR_API virtual void setParentTransformName(
        const fwRenderOgre::SRender::OgreObjectIdType&) override final;

private:

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect ::fwData::Object::s_MODIFIED_SIG of s_TRANSFORM_INOUT to ::visuOgreAdaptor::SLight::s_UPDATE_SLOT
     * Connect ::fwData::Object::s_MODIFIED_SIG of s_DIFFUSE_COLOR_INOUT to ::visuOgreAdaptor::SLight::s_UPDATE_SLOT
     * Connect ::fwData::Object::s_MODIFIED_SIG of s_DIFFUSE_COLOR_INOUT to ::visuOgreAdaptor::SLight::s_UPDATE_SLOT
     */
    ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override final;

    /// Configures the service.
    virtual void configuring() override final;

    /// Adds a new light to the scene manager.
    virtual void starting() override final;

    /// Updates the light position and orientation.
    virtual void updating() override final;

    /// Removes the light from the scene manager.
    virtual void stopping() override final;

    /// Ogre light managed by this adaptor.
    ::Ogre::Light* m_light {nullptr};

    /// Name of the associated Ogre light.
    std::string m_lightName;

    /// Type of the associated Ogre light.
    ::Ogre::Light::LightTypes m_lightType {::Ogre::Light::LT_DIRECTIONAL};

    /// Diffuse color of the associated Ogre light.
    ::fwData::Color::sptr m_lightDiffuseColor {nullptr};

    /// Specular color of the associated Ogre light.
    ::fwData::Color::sptr m_lightSpecularColor {nullptr};

    /// Light activation flag.
    bool m_switchedOn {true};

    /// Angle in degrees defining the rotation of the light around x axis.
    float m_thetaOffset {0.f};

    /// Angle in degrees defining the rotation of the light around y axis.
    float m_phiOffset {0.f};

    /// Node used to attach the light
    ::Ogre::SceneNode* m_lightNode {nullptr};
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

inline void SLight::setType(::Ogre::Light::LightTypes _type)
{
    m_lightType = _type;
}

//------------------------------------------------------------------------------

inline ::Ogre::ColourValue SLight::getDiffuseColor() const
{
    return m_light->getDiffuseColour();
}

//------------------------------------------------------------------------------

inline void SLight::setDiffuseColor(::fwData::Color::sptr _diffuseColor)
{
    m_lightDiffuseColor = _diffuseColor;
}

//------------------------------------------------------------------------------

inline ::Ogre::ColourValue SLight::getSpecularColor() const
{
    return m_light->getSpecularColour();
}

//------------------------------------------------------------------------------

inline void SLight::setSpecularColor(::fwData::Color::sptr _specularColor)
{
    m_lightSpecularColor = _specularColor;
}

//------------------------------------------------------------------------------

inline void SLight::setParentTransformName(const ::fwRenderOgre::SRender::OgreObjectIdType& _id)
{
    FW_DEPRECATED_MSG("This method is no longer supported", "21.0");
    this->setTransformId(_id);
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

inline bool SLight::isOrphanNode() const
{
    FW_DEPRECATED_MSG("This method is no longer supported", "21.0");
    return false;
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
