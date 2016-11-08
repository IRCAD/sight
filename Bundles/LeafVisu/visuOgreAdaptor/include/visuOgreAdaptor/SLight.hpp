/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SLIGHT_HPP__
#define __VISUOGREADAPTOR_SLIGHT_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Color.hpp>

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
 * - \b updateXOffset(float): Called when the x offset is changed and moves the light accordingly.
 * - \b updateYOffset(float): Called when the y offset is changed and moves the light accordingly.
 * - \b setDoubleParameter(double, string): Calls a double parameter slot according to the given key.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="lightAdaptor" class="::visuOgreAdaptor::SLight" objectId="lightTF">
        <in key="lighTF" uid="lightTFUid" />
        <in key="lightColor" uid="lightColorUid" />
        <config name="sceneLight" parentTransformId="cameraTF" xOffset="30.5" yOffset="45" />
    </service>
 * @endcode
 * With :
 * - \b name (optional): defines a name for the associated Ogre light.
 * - \b parentTransformId (optional): name of the parent transform node.
 * Only if a parent transform node is configured :
 * - \b xOffset (optional, float, default=0.0): Angle in degrees defining the rotation of the light around x axis.
 * - \b yOffset (optional, float, default=0.0): Angle in degrees defining the rotation of the light around y axis.
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
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_X_OFFSET_SLOT;
    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_Y_OFFSET_SLOT;
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

    /// Light name setter.
    VISUOGREADAPTOR_API void setName(std::string _name);

    /// Light type setter.
    VISUOGREADAPTOR_API void setType(::Ogre::Light::LightTypes _type);

    /// Light direction setter.
    VISUOGREADAPTOR_API void setDirection(::Ogre::Vector3 _direction);

    /// Diffuse color setter using fwData::Color.
    VISUOGREADAPTOR_API void setDiffuseColor(::fwData::Color::sptr _diffuseColor);

    /// Diffuse color setter using Ogre::ColourValue.
    VISUOGREADAPTOR_API void setDiffuseColor(::Ogre::ColourValue _diffuseColor);

    /// Specular color setter using fwData::Color.
    VISUOGREADAPTOR_API void setSpecularColor(::fwData::Color::sptr _specularColor);

    /// Specular color setter using Ogre::ColourValue.
    VISUOGREADAPTOR_API void setSpecularColor(::Ogre::ColourValue _specularColor);

    /// Parent tranform id setter.
    VISUOGREADAPTOR_API void setParentTransformName(const fwRenderOgre::SRender::OgreObjectIdType& _parentTransformName);

private:

    void updateXOffset(float _xOffset);
    void updateYOffset(float _yOffset);
    void setDoubleParameter(double _val, std::string _key);

    /// Creates a transform Service, and attaches it to a corresponding scene node in the scene.
    void createTransformService();

    /// Attaches a movable object to the associated scene node
    void attachNode(Ogre::MovableObject* _node);

    /// Ogre light managed by this adaptor.
    ::Ogre::Light* m_light;

    /// Name of the associated Ogre light.
    std::string m_lightName;

    /// Type of the associated Ogre light.
    ::Ogre::Light::LightTypes m_lightType;

    /// Direction of the associated Ogre light.
    ::Ogre::Vector3 m_lightDirection;

    /// Diffuse color of the associated Ogre light.
    ::fwData::Color::csptr m_lightDiffuseColor;

    /// Specular color of the associated Ogre light.
    ::fwData::Color::csptr m_lightSpecularColor;

    /// If we can't retrieve the parent transform adaptor, we will use a scene node without any parent.
    bool m_useOrphanNode;

    /// Angle in degrees defining the rotation of the light around x axis.
    float m_xOffset;

    /// Angle in degrees defining the rotation of the light around y axis.
    float m_yOffset;
};

//------------------------------------------------------------------------------
// Inline method(s)

inline void SLight::setName(std::string _name)
{
    m_lightName = _name;
}

//------------------------------------------------------------------------------

inline void SLight::setType(::Ogre::Light::LightTypes _type)
{
    m_lightType = _type;
}

//------------------------------------------------------------------------------

inline void SLight::setDirection(::Ogre::Vector3 _direction)
{
    m_lightDirection = _direction;
}

//------------------------------------------------------------------------------

inline void SLight::setDiffuseColor(::fwData::Color::sptr _diffuseColor)
{
    m_lightDiffuseColor = _diffuseColor;
}

//------------------------------------------------------------------------------

inline void SLight::setSpecularColor(::fwData::Color::sptr _specularColor)
{
    m_lightSpecularColor = _specularColor;
}

//------------------------------------------------------------------------------

void SLight::setParentTransformName(const ::fwRenderOgre::SRender::OgreObjectIdType& _parentTransformName)
{
    this->setParentTransformId(_parentTransformName);
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SLIGHT_HPP__
