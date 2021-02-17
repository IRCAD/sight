/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#include "viz/scene3d/config.hpp"
#include "viz/scene3d/factory/new.hpp"
#include "viz/scene3d/IAdaptor.hpp"
#include "viz/scene3d/ITransformable.hpp"
#include "viz/scene3d/registry/detail.hpp"
#include "viz/scene3d/SRender.hpp"

#include <data/Color.hpp>
#include <data/Matrix4.hpp>

#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreLight.h>
#include <OGRE/OgreVector3.h>

#include <ui/base/container/fwContainer.hpp>

#include <string>

namespace sight::viz::scene3d
{

/**
 * @brief Defines the service interface managing the light adaptor.
 */
class VIZ_SCENE3D_CLASS_API ILight : public viz::scene3d::IAdaptor,
                                     public viz::scene3d::ITransformable
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
            viz::scene3d::registry::getLightRegistry()->addFactory(functorKey, &viz::scene3d::lightFactory::New<T>);
        }
    };

    /// Defines the key used for the factory.
    VIZ_SCENE3D_API static const std::string REGISTRY_KEY;

    typedef viz::scene3d::lightFactory::Key Key;

    fwCoreClassMacro(ILight, viz::scene3d::IAdaptor)

    /// Defines point type light.
    VIZ_SCENE3D_API static const std::string s_POINT_LIGHT;

    /// Defines directional type light.
    VIZ_SCENE3D_API static const std::string s_DIRECTIONAL_LIGHT;

    /// Defines spto type light.
    VIZ_SCENE3D_API static const std::string s_SPOT_LIGHT;

    /// Defines the range of the phi and theta value of directional lights.
    VIZ_SCENE3D_API static const int s_OFFSET_RANGE;

    /**
     * @brief Creates a light adaptor.
     * @param _diffuse diffuse color of the light.
     * @param _specular specular color of the light.
     * @return A new ligh adaptor with registered inputs.
     */
    VIZ_SCENE3D_API static ILight::sptr createLightAdaptor(data::Color::sptr _diffuse,
                                                           data::Color::sptr _specular);

    /**
     * @brief Destroys a light adaptor.
     * @param _lightManager the light adaptor to unregister.
     */
    VIZ_SCENE3D_API static void destroyLightAdaptor(ILight::sptr _lightManager);

    /// Does nothing.
    VIZ_SCENE3D_API ILight();

    /// Does nothing.
    VIZ_SCENE3D_API virtual ~ILight();

    /**
     * @brief Gets the name of the light.
     * @return The name of the light.
     */
    VIZ_SCENE3D_API virtual const std::string& getName() const = 0;

    /**
     * @brief Sets the name of the light.
     * @param _name the name of the light.
     */
    VIZ_SCENE3D_API virtual void setName(const std::string& _name) = 0;

    /**
     * @brief Gets the Orge light type .
     * @return The type of the light.
     */
    VIZ_SCENE3D_API virtual ::Ogre::Light::LightTypes getType() const = 0;

    /**
     * @brief Sets the type of the light.
     * @param _type the type of light.
     */
    VIZ_SCENE3D_API virtual void setType(::Ogre::Light::LightTypes _type) = 0;

    /**
     * @brief Gets the Ogre diffuse color of the light.
     * @return The diffuse color of the light.
     */
    VIZ_SCENE3D_API virtual ::Ogre::ColourValue getDiffuseColor() const = 0;

    /**
     * @brief Sets the diffuse color of the light.
     * @param _diffuseColor the Ogre diffuse color of the light.
     */
    VIZ_SCENE3D_API virtual void setDiffuseColor(::Ogre::ColourValue _diffuseColor) = 0;

    /**
     * @brief Gets the Ogre specular color of the light.
     * @return The specular color of the light.
     */
    VIZ_SCENE3D_API virtual ::Ogre::ColourValue getSpecularColor() const = 0;

    /**
     * @brief Sets the specular color of the light.
     * @param _diffuseColor the Ogre specular color of the light.
     */
    VIZ_SCENE3D_API virtual void setSpecularColor(::Ogre::ColourValue _specularColor) = 0;

    /**
     * @brief Indicates if the light is activated.
     * @return True if the light is activated.
     */
    VIZ_SCENE3D_API virtual bool isSwitchedOn() const = 0;

    /**
     * @brief Sets the light activation status.
     * @param _on set to true to enable the light.
     */
    VIZ_SCENE3D_API virtual void switchOn(bool _on) = 0;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around x axis.
     * @return The theta offset of the light.
     */
    VIZ_SCENE3D_API virtual float getThetaOffset() const = 0;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around x axis.
     * @param _thetaOffset the value of the thta offset.
     * @pre The type of the light must be ::Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    VIZ_SCENE3D_API virtual void setThetaOffset(float _thetaOffset) = 0;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around y axis.
     * @return The phi of the light.
     */
    VIZ_SCENE3D_API virtual float getPhiOffset() const = 0;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around y axis.
     * @param _phiOffset the phi of the thta offset.
     * @pre The type of the light must be ::Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    VIZ_SCENE3D_API virtual void setPhiOffset(float _phiOffset) = 0;

    /**
     * @brief Enables the light visual feedback.
     * @param _enable the visual feedback visibility state.
     */
    VIZ_SCENE3D_API virtual void enableVisualFeedback(bool _enable) = 0;

    /**
     * @brief Indicates if the visual feedback is enabled.
     * @return True if the visual feedback is activated.
     */
    VIZ_SCENE3D_API virtual bool isVisualFeedbackOn() const = 0;

};

} // namespace sight::viz::scene3d
