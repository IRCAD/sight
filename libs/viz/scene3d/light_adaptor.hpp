/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include "viz/scene3d/adaptor.hpp"
#include "viz/scene3d/config.hpp"
#include "viz/scene3d/factory/new.hpp"
#include "viz/scene3d/registry/detail.hpp"
#include "viz/scene3d/render.hpp"
#include "viz/scene3d/transformable.hpp"

#include <data/color.hpp>
#include <data/matrix4.hpp>

#include <ui/__/container/widget.hpp>

#include <OGRE/Ogre.h>
#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreLight.h>

#include <string>

namespace sight::viz::scene3d
{

/**
 * @brief Defines the service interface managing the light adaptor.
 */
class VIZ_SCENE3D_CLASS_API light_adaptor : public viz::scene3d::adaptor,
                                            public viz::scene3d::transformable
{
public:

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' ).
     *
     * @tparam T factory product type.
     */
    template<typename T>
    class registry
    {
    public:

        registry(std::string _functor_key)
        {
            viz::scene3d::registry::get_light_registry()->add_factory(
                _functor_key,
                &viz::scene3d::light_factory::make<T>
            );
        }
    };

    /// Defines the key used for the factory.
    VIZ_SCENE3D_API static const std::string REGISTRY_KEY;

    SIGHT_DECLARE_CLASS(light_adaptor, viz::scene3d::adaptor);

    /// Defines point type light.
    VIZ_SCENE3D_API static const std::string POINT_LIGHT;

    /// Defines directional type light.
    VIZ_SCENE3D_API static const std::string DIRECTIONAL_LIGHT;

    /// Defines spot type light.
    VIZ_SCENE3D_API static const std::string SPOT_LIGHT;

    /// Defines the range of the phi and theta value of directional lights.
    VIZ_SCENE3D_API static const int OFFSET_RANGE;

    /**
     * @brief Creates a light adaptor.
     * @param _diffuse diffuse color of the light.
     * @param _specular specular color of the light.
     * @return A new light adaptor with registered inputs.
     */
    VIZ_SCENE3D_API static light_adaptor::sptr create_light_adaptor(
        data::color::sptr _diffuse,
        data::color::sptr _specular
    );

    /**
     * @brief Destroys a light adaptor.
     * @param _light_manager the light adaptor to unregister.
     */
    VIZ_SCENE3D_API static void destroy_light_adaptor(light_adaptor::sptr _light_manager);

    /// Does nothing.
    VIZ_SCENE3D_API light_adaptor();

    /// Does nothing.
    VIZ_SCENE3D_API ~light_adaptor() override;

    /**
     * @brief Gets the name of the light.
     * @return The name of the light.
     */
    VIZ_SCENE3D_API virtual const std::string& get_name() const = 0;

    /**
     * @brief Sets the name of the light.
     * @param _name the name of the light.
     */
    VIZ_SCENE3D_API virtual void set_name(const std::string& _name) = 0;

    /**
     * @brief Gets the Ogre light type .
     * @return The type of the light.
     */
    VIZ_SCENE3D_API virtual Ogre::Light::LightTypes type() const = 0;

    /**
     * @brief Sets the type of the light.
     * @param _type the type of light.
     */
    VIZ_SCENE3D_API virtual void set_type(Ogre::Light::LightTypes _type) = 0;

    /**
     * @brief Gets the Ogre diffuse color of the light.
     * @return The diffuse color of the light.
     */
    VIZ_SCENE3D_API virtual Ogre::ColourValue get_diffuse_color() const = 0;

    /**
     * @brief Sets the diffuse color of the light.
     * @param _diffuse_color the Ogre diffuse color of the light.
     */
    VIZ_SCENE3D_API virtual void set_diffuse_color(Ogre::ColourValue _diffuse_color) = 0;

    /**
     * @brief Gets the Ogre specular color of the light.
     * @return The specular color of the light.
     */
    VIZ_SCENE3D_API virtual Ogre::ColourValue get_specular_color() const = 0;

    /**
     * @brief Sets the specular color of the light.
     * @param _specular_color the Ogre specular color of the light.
     */
    VIZ_SCENE3D_API virtual void set_specular_color(Ogre::ColourValue _specular_color) = 0;

    /**
     * @brief Indicates if the light is activated.
     * @return True if the light is activated.
     */
    VIZ_SCENE3D_API virtual bool is_switched_on() const = 0;

    /**
     * @brief Sets the light activation status.
     * @param _on set to true to enable the light.
     */
    VIZ_SCENE3D_API virtual void switch_on(bool _on) = 0;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around x axis.
     * @return The theta offset of the light.
     */
    VIZ_SCENE3D_API virtual float get_theta_offset() const = 0;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around x axis.
     * @param _theta_offset the value of the theta offset.
     * @pre The type of the light must be Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    VIZ_SCENE3D_API virtual void set_theta_offset(float _theta_offset) = 0;

    /**
     * @brief Gets the angle in degrees defining the rotation of the light around y axis.
     * @return The phi of the light.
     */
    VIZ_SCENE3D_API virtual float get_phi_offset() const = 0;

    /**
     * @brief Sets the angle in degrees defining the rotation of the light around y axis.
     * @param _phi_offset the phi of the theta offset.
     * @pre The type of the light must be Ogre::Light::LT_DIRECTIONAL to used this value.
     */
    VIZ_SCENE3D_API virtual void set_phi_offset(float _phi_offset) = 0;

    /**
     * @brief Enables the light visual feedback.
     * @param _enable the visual feedback visibility state.
     */
    VIZ_SCENE3D_API virtual void enable_visual_feedback(bool _enable) = 0;

    /**
     * @brief Indicates if the visual feedback is enabled.
     * @return True if the visual feedback is activated.
     */
    VIZ_SCENE3D_API virtual bool is_visual_feedback_on() const = 0;

protected:

    data::ptr<data::color, data::access::inout> m_diffuse {this, "diffuseColor", true};
    data::ptr<data::color, data::access::inout> m_specular {this, "specularColor", true};
};

} // namespace sight::viz::scene3d
