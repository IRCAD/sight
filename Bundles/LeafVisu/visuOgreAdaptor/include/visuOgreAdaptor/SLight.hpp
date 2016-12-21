/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SLIGHT_HPP__
#define __VISUOGREADAPTOR_SLIGHT_HPP__

#include "visuOgreAdaptor/STransform.hpp"
#include "visuOgreAdaptor/config.hpp"

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
        <inout key="transform" uid="lightTFUid" />
        <inout key="diffuseColor" uid="diffuseColorUid" />
        <inout key="specularColor" uid="specularColorUid" />
        <config name="sceneLight" parentTransformId="cameraTF" switchedOn="yes" thetaOffset="30.5" phiOffset="45" />
    </service>
 * @endcode
 * With :
 * - \b name (optional): defines a name for the associated Ogre light.
 * - \b parentTransformId (optional): name of the parent transform node.
 * - \b switchedOn (optional, bool, default="yes"): defines if the light is activated or not.
 * Only if a parent transform node is configured :
 * - \b thetaOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around x axis.
 * - \b phiOffset (optional, float, default=0.0): angle in degrees defining the rotation of the light around y axis.
 */
class VISUOGREADAPTOR_CLASS_API SLight : public ::fwRenderOgre::ILight,
                                         public ::fwRenderOgre::ITransformable
{

public:

    fwCoreServiceClassDefinitionsMacro((SLight)(::fwRenderOgre::ILight));

    /**
     * @name Slots API
     * @{
     */
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_X_OFFSET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_Y_OFFSET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    /** @} */

    /// Constructor.
    VISUOGREADAPTOR_API SLight() throw();

    /// Factory Constructor.
    VISUOGREADAPTOR_API SLight(::fwRenderOgre::ILight::Key key);

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SLight() throw();

protected:
    /// Starting method. Do nothing
    VISUOGREADAPTOR_API void doStart() throw(fwTools::Failed);

    /// Stopping method
    VISUOGREADAPTOR_API void doStop() throw(fwTools::Failed);

    /// Configures the service.
    VISUOGREADAPTOR_API void doConfigure() throw(fwTools::Failed);

    /// Swaping method, only asks for an updating.
    VISUOGREADAPTOR_API void doSwap() throw(fwTools::Failed);

    /// Updates the light position and orientation
    VISUOGREADAPTOR_API void doUpdate() throw(fwTools::Failed);

    /// Light name getter.
    VISUOGREADAPTOR_API virtual const std::string& getName() const;

    /// Light name setter.
    VISUOGREADAPTOR_API virtual void setName(std::string _name);

    /// Light type getter.
    VISUOGREADAPTOR_API virtual ::Ogre::Light::LightTypes getType() const;

    /// Light type setter.
    VISUOGREADAPTOR_API virtual void setType(::Ogre::Light::LightTypes _type);

    /// Diffuse color setter using fwData::Color.
    VISUOGREADAPTOR_API void setDiffuseColor(::fwData::Color::sptr _diffuseColor);

    /// Diffuse color getter.
    VISUOGREADAPTOR_API virtual ::Ogre::ColourValue getDiffuseColor() const;

    /// Diffuse color setter using Ogre::ColourValue.
    VISUOGREADAPTOR_API virtual void setDiffuseColor(::Ogre::ColourValue _diffuseColor);

    /// Specular color setter using fwData::Color.
    VISUOGREADAPTOR_API void setSpecularColor(::fwData::Color::sptr _specularColor);

    /// Specular color getter.
    VISUOGREADAPTOR_API virtual ::Ogre::ColourValue getSpecularColor() const;

    /// Specular color setter using Ogre::ColourValue.
    VISUOGREADAPTOR_API virtual void setSpecularColor(::Ogre::ColourValue _specularColor);

    /// Parent tranform id setter.
    VISUOGREADAPTOR_API virtual void setParentTransformName(
        const fwRenderOgre::SRender::OgreObjectIdType& _parentTransformName);

    /// Light activation flag getter.
    VISUOGREADAPTOR_API virtual bool isSwitchedOn() const;

    /// Light activation flag setter.
    VISUOGREADAPTOR_API virtual void switchOn(bool _on);

    /// Theta offset getter.
    VISUOGREADAPTOR_API virtual float getThetaOffset() const;

    /// Theta offset setter.
    VISUOGREADAPTOR_API virtual void setThetaOffset(float _thetaOffset);

    /// Phi offset getter.
    VISUOGREADAPTOR_API virtual float getPhiOffset() const;

    /// Phi offset setter.
    VISUOGREADAPTOR_API virtual void setPhiOffset(float _phiOffset);

    /// Indicates if the light is attached to a parent node or not.
    VISUOGREADAPTOR_API virtual bool isOrphanNode() const;

private:

    void setDoubleParameter(double _val, std::string _key);

    /// Creates a transform Service, and attaches it to a corresponding scene node in the scene.
    void createTransformService();

    /// Returns a shared pointer of the attached transform service.
    ::visuOgreAdaptor::STransform::sptr getTransformService();

    /// Attaches a movable object to the associated scene node
    void attachNode(Ogre::MovableObject* _node);

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
};

//------------------------------------------------------------------------------
// Inline method(s)

inline const std::string& SLight::getName() const
{
    return m_lightName;
}

//------------------------------------------------------------------------------

inline void SLight::setName(std::string _name)
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

inline void SLight::setParentTransformName(const ::fwRenderOgre::SRender::OgreObjectIdType& _parentTransformName)
{
    this->setParentTransformId(_parentTransformName);
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

inline ::visuOgreAdaptor::STransform::sptr SLight::getTransformService()
{
    return ::visuOgreAdaptor::STransform::dynamicCast(m_transformService.lock());
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SLIGHT_HPP__
