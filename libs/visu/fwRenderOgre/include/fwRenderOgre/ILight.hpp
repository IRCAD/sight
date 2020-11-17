/************************************************************************
 *
 * Copyright (C) 2014-2020 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "fwRenderOgre/config.hpp"
#include "fwRenderOgre/factory/new.hpp"
#include "fwRenderOgre/IAdaptor.hpp"
#include "fwRenderOgre/ITransformable.hpp"
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
 */
class FWRENDEROGRE_CLASS_API ILight : public ::fwRenderOgre::IAdaptor,
                                      public ::fwRenderOgre::ITransformable
{

public:

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' ).
     *
     * @tparam T Factory product type.
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

    /// Defines the key used for the factory.
    FWRENDEROGRE_API static const std::string REGISTRY_KEY;

    typedef ::fwRenderOgre::lightFactory::Key Key;

    fwCoreClassMacro(ILight, ::fwRenderOgre::IAdaptor)

    /// Defines point type light.
    FWRENDEROGRE_API static const std::string s_POINT_LIGHT;

    /// Defines directional type light.
    FWRENDEROGRE_API static const std::string s_DIRECTIONAL_LIGHT;

    /// Defines spto type light.
    FWRENDEROGRE_API static const std::string s_SPOT_LIGHT;

    /// Defines the range of the phi and theta value of directional lights.
    FWRENDEROGRE_API static const int s_OFFSET_RANGE;

    /**
     * @brief Creates a light adaptor.
     * @param _diffuse diffuse color of the light.
     * @param _specular specular color of the light.
     * @return A new ligh adaptor with registered inputs.
     */
    FWRENDEROGRE_API static ILight::sptr createLightAdaptor(::fwData::Color::sptr _diffuse,
                                                            ::fwData::Color::sptr _specular);

    /**
     * @brief Destroys a light adaptor.
     * @param _lightManager the light adaptor to unregister.
     */
    FWRENDEROGRE_API static void destroyLightAdaptor(ILight::sptr _lightManager);

    /// Does nothing.
    FWRENDEROGRE_API ILight();

    /// Does nothing.
    FWRENDEROGRE_API virtual ~ILight();

    /**
     * @brief Gets the name of the light.
     * @return The name of the light.
     */
    FWRENDEROGRE_API virtual const std::string& getName() const = 0;

    /**
     * @brief Sets the name of the light.
     * @param _name the name of the light.
     */
    FWRENDEROGRE_API virtual void setName(const std::string& _name) = 0;

    /**
     * @brief Gets the Orge light type .
     * @return The type of the light.
     */
    FWRENDEROGRE_API virtual ::Ogre::Light::LightTypes getType() const = 0;

    /**
     * @brief Sets the type of the light.
     * @param _type the type of light.
     */
    FWRENDEROGRE_API virtual void setType(::Ogre::Light::LightTypes _type) = 0;

    /**
     * @brief Gets the Ogre diffuse color of the light.
     * @return The diffuse color of the light.
     */
    FWRENDEROGRE_API virtual ::Ogre::ColourValue getDiffuseColor() const = 0;

    /**
     * @brief Sets the diffuse color of the light.
     * @param _diffuseColor the Ogre diffuse color of the light.
     */
    FWRENDEROGRE_API virtual void setDiffuseColor(::Ogre::ColourValue _diffuseColor) = 0;

    /**
     * @brief Gets the Ogre specular color of the light.
     * @return The specular color of the light.
     */
    FWRENDEROGRE_API virtual ::Ogre::ColourValue getSpecularColor() const = 0;

    /**
     * @brief Sets the specular color of the light.
     * @param _diffuseColor the Ogre specular color of the light.
     */
    FWRENDEROGRE_API virtual void setSpecularColor(::Ogre::ColourValue _specularColor) = 0;

    /**
     * @brief Indicates if the light is activated.
     * @return True if the light is activated.
     */
    FWRENDEROGRE_API virtual bool isSwitchedOn() const = 0;

    /**
     * @brief Sets the light activation status.
     * @param _on set to true to enable the light.
     */
    FWRENDEROGRE_API virtual void switchOn(bool _on) = 0;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around x axis.
     * @return The theta offset of the light.
     */
    FWRENDEROGRE_API virtual float getThetaOffset() const = 0;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around x axis.
     * @param _thetaOffset the value of the thta offset.
     * @pre The type of the light must be ::Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    FWRENDEROGRE_API virtual void setThetaOffset(float _thetaOffset) = 0;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around y axis.
     * @return The phi of the light.
     */
    FWRENDEROGRE_API virtual float getPhiOffset() const = 0;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around y axis.
     * @param _phiOffset the phi of the thta offset.
     * @pre The type of the light must be ::Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    FWRENDEROGRE_API virtual void setPhiOffset(float _phiOffset) = 0;

    /**
     * @brief Enables the light visual feedback.
     * @param _enable the visual feedback visibility state.
     */
    FWRENDEROGRE_API virtual void enableVisualFeedback(bool _enable) = 0;

    /**
     * @brief Indicates if the visual feedback is enabled.
     * @return True if the visual feedback is activated.
     */
    FWRENDEROGRE_API virtual bool isVisualFeedbackOn() const = 0;

};

} // namespace fwRenderOgre
