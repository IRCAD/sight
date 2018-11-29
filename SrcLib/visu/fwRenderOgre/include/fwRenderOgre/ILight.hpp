/************************************************************************
 *
 * Copyright (C) 2014-2017 IRCAD France
 * Copyright (C) 2014-2017 IHU Strasbourg
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

#ifndef __FWRENDEROGRE_ILIGHT_HPP__
#define __FWRENDEROGRE_ILIGHT_HPP__

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/factory/new.hpp"
#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/registry/detail.hpp"
#include "fwRenderOgre/SRender.hpp"

#include <fwData/Color.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGui/container/fwContainer.hpp>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreVector3.h>

#include <string>

namespace fwRenderOgre
{

/**
 * @brief Defines the service interface managing the light adaptor.
 *
 */
class FWRENDEROGRE_CLASS_API ILight : public ::fwRenderOgre::IAdaptor
{

public:

    typedef ::fwRenderOgre::lightFactory::Key Key;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar(std::string functorKey)
        {
            ::fwRenderOgre::registry::getLightRegistry()->addFactory(functorKey, &::fwRenderOgre::lightFactory::New<T>);
        }
    };

    fwCoreNonInstanciableClassDefinitionsMacro( (ILight)(::fwRenderOgre::IAdaptor) )

    typedef std::string FactoryRegistryKeyType;

    FWRENDEROGRE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWRENDEROGRE_API static const std::string s_POINT_LIGHT;
    FWRENDEROGRE_API static const std::string s_DIRECTIONAL_LIGHT;
    FWRENDEROGRE_API static const std::string s_SPOT_LIGHT;

    FWRENDEROGRE_API static const int s_OFFSET_RANGE;

    /// ILight factory method.
    FWRENDEROGRE_API static ILight::sptr createLightAdaptor(::fwData::TransformationMatrix3D::sptr _transform,
                                                            ::fwData::Color::sptr _diffuse,
                                                            ::fwData::Color::sptr _specular);

    /// ILight stopping and unregistrating method.
    FWRENDEROGRE_API static void destroyLightAdaptor(ILight::sptr _lightManager);

    /// Constructor. Do nothing.
    FWRENDEROGRE_API ILight();

    /// Destructor. Do nothing.
    FWRENDEROGRE_API virtual ~ILight();

    /// Light name getter.
    FWRENDEROGRE_API virtual const std::string& getName() const = 0;
    /// Light name setter.
    FWRENDEROGRE_API virtual void setName(const std::string& _name)
        = 0;
    /// Light type getter.
    FWRENDEROGRE_API virtual ::Ogre::Light::LightTypes getType() const = 0;
    /// Light type setter.
    FWRENDEROGRE_API virtual void setType(::Ogre::Light::LightTypes _type)
        = 0;
    /// Diffuse color getter.
    FWRENDEROGRE_API virtual ::Ogre::ColourValue getDiffuseColor() const = 0;
    /// Diffuse color setter using Ogre::ColourValue.
    FWRENDEROGRE_API virtual void setDiffuseColor(::Ogre::ColourValue _diffuseColor)
        = 0;
    /// Specular color getter.
    FWRENDEROGRE_API virtual ::Ogre::ColourValue getSpecularColor() const = 0;
    /// Specular color setter using Ogre::ColourValue.
    FWRENDEROGRE_API virtual void setSpecularColor(::Ogre::ColourValue _specularColor)
        = 0;
    /// Parent tranform id setter.
    FWRENDEROGRE_API virtual void setParentTransformName(
        const ::fwRenderOgre::SRender::OgreObjectIdType& _parentTransformName) = 0;
    /// Light activation flag getter.
    FWRENDEROGRE_API virtual bool isSwitchedOn() const = 0;
    /// Light activation flag setter.
    FWRENDEROGRE_API virtual void switchOn(bool _on) = 0;
    /// Theta offset getter.
    FWRENDEROGRE_API virtual float getThetaOffset() const = 0;
    /// Theta offset setter.
    FWRENDEROGRE_API virtual void setThetaOffset(float _theta) = 0;
    /// Phi offset getter.
    FWRENDEROGRE_API virtual float getPhiOffset() const = 0;
    /// Phi offset setter.
    FWRENDEROGRE_API virtual void setPhiOffset(float _phi) = 0;
    /// Indicates if the light is attached to a parent node or not.
    FWRENDEROGRE_API virtual bool isOrphanNode() const = 0;
};

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_ILIGHT_HPP__

