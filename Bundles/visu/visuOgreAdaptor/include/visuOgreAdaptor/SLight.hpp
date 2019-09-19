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
#include <fwRenderOgre/ITransformable.hpp>

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
 * - \b setDoubleParameter(double, string): Calls a double parameter slot according to the given key.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="lightAdaptor" type="::visuOgreAdaptor::SLight">
        <inout key="diffuseColor" uid="diffuseColorUid" />
        <inout key="specularColor" uid="specularColorUid" />
        <config name="sceneLight" transform="..." switchedOn="yes" thetaOffset="30.5" phiOffset="45" />
    </service>
 * @endcode
 * @subsection In-Out In-Out
 * - \b diffuseColor [::fwData::Color]: diffuse color of the light.
 * - \b specularColor [::fwData::Color]: specular color of the light.
 * @subsection Configuration Configuration:
 * - \b layer (mandatory): defines the light's layer.
 * - \b name (mandatory): defines a name for the associated Ogre light.
 * - \b transform (optional): transform applied to the frustum's scene node
 * - \b switchedOn (optional, bool, default="yes"): defines if the light is activated or not.
 * - \b thetaOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around x axis.
 * - \b phiOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around y axis.
 */
class VISUOGREADAPTOR_CLASS_API SLight : public ::fwRenderOgre::ILight,
                                         public ::fwRenderOgre::ITransformable
{

public:

    fwCoreServiceMacro(SLight, ::fwRenderOgre::ILight);

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_X_OFFSET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_Y_OFFSET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SLight() noexcept;

    /// Factory Constructor.
    VISUOGREADAPTOR_API SLight(::fwRenderOgre::ILight::Key key);

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SLight() noexcept;

    /// Connect ::fwData::Object::s_MODIFIED_SIG of all data to s_UPDATE_SLOT
    VISUOGREADAPTOR_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:
    /// Starting method. Do nothing
    VISUOGREADAPTOR_API void starting() override;

    /// Stopping method
    VISUOGREADAPTOR_API void stopping() override;

    /// Configures the service.
    VISUOGREADAPTOR_API void configuring() override;

    /// Updates the light position and orientation
    VISUOGREADAPTOR_API void updating() override;

    /// Light name getter.
    VISUOGREADAPTOR_API virtual const std::string& getName() const override;

    /// Light name setter.
    VISUOGREADAPTOR_API virtual void setName(const std::string& _name) override;

    /// Light type getter.
    VISUOGREADAPTOR_API virtual ::Ogre::Light::LightTypes getType() const override;

    /// Light type setter.
    VISUOGREADAPTOR_API virtual void setType(::Ogre::Light::LightTypes _type) override;

    /// Diffuse color setter using fwData::Color.
    VISUOGREADAPTOR_API void setDiffuseColor(::fwData::Color::sptr _diffuseColor);

    /// Diffuse color getter.
    VISUOGREADAPTOR_API virtual ::Ogre::ColourValue getDiffuseColor() const override;

    /// Diffuse color setter using Ogre::ColourValue.
    VISUOGREADAPTOR_API virtual void setDiffuseColor(::Ogre::ColourValue _diffuseColor) override;

    /// Specular color setter using fwData::Color.
    VISUOGREADAPTOR_API void setSpecularColor(::fwData::Color::sptr _specularColor);

    /// Specular color getter.
    VISUOGREADAPTOR_API virtual ::Ogre::ColourValue getSpecularColor() const override;

    /// Specular color setter using Ogre::ColourValue.
    VISUOGREADAPTOR_API virtual void setSpecularColor(::Ogre::ColourValue _specularColor) override;

    /// Parent tranform id setter.
    VISUOGREADAPTOR_API virtual void setParentTransformName(
        const fwRenderOgre::SRender::OgreObjectIdType&) override;

    /// Light activation flag getter.
    VISUOGREADAPTOR_API virtual bool isSwitchedOn() const override;

    /// Light activation flag setter.
    VISUOGREADAPTOR_API virtual void switchOn(bool _on) override;

    /// Theta offset getter.
    VISUOGREADAPTOR_API virtual float getThetaOffset() const override;

    /// Theta offset setter.
    VISUOGREADAPTOR_API virtual void setThetaOffset(float _thetaOffset) override;

    /// Phi offset getter.
    VISUOGREADAPTOR_API virtual float getPhiOffset() const override;

    /// Phi offset setter.
    VISUOGREADAPTOR_API virtual void setPhiOffset(float _phiOffset) override;

    /// Indicates if the light is attached to a parent node or not.
    VISUOGREADAPTOR_API virtual bool isOrphanNode() const override;

private:

    void setDoubleParameter(double _val, std::string _key);

    /// Creates a transform Service, and attaches it to a corresponding scene node in the scene.
    void createTransformService();

    /// Ogre light managed by this adaptor.
    ::Ogre::Light* m_light;

    /// Name of the associated Ogre light.
    std::string m_lightName;

    /// Type of the associated Ogre light.
    ::Ogre::Light::LightTypes m_lightType;

    /// Diffuse color of the associated Ogre light.
    ::fwData::Color::sptr m_lightDiffuseColor;

    /// Specular color of the associated Ogre light.
    ::fwData::Color::sptr m_lightSpecularColor;

    /// If we can't retrieve the parent transform adaptor, we will use a scene node without any parent.
    bool m_useOrphanNode;

    /// Light activation flag.
    bool m_switchedOn;

    /// Angle in degrees defining the rotation of the light around x axis.
    float m_thetaOffset;

    /// Angle in degrees defining the rotation of the light around y axis.
    float m_phiOffset;

    /// Node used to attach the light
    ::Ogre::SceneNode* m_lightNode {nullptr};
};

//------------------------------------------------------------------------------
// Inline method(s)

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
    this->setTransformId(_id);
    m_useOrphanNode = false;
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
    return m_useOrphanNode;
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
