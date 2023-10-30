/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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
#include "viz/scene3d/texture.hpp"
#include "viz/scene3d/vr/summed_area_table.hpp"

#include <optional>

namespace sight::viz::scene3d::vr
{

/**
 * @brief Computes the illumination volume for extinction based shading.
 */
class VIZ_SCENE3D_CLASS_API illum_ambient_occlusion_sat final
{
public:

    using sptr = std::shared_ptr<illum_ambient_occlusion_sat>;

    /// Internal wrapper holding the Summed Area Table (SAT) parameters
    struct sat_parameters_t
    {
        /// Ratio used to determine the size of the SAT regarding of the associated image size.
        float size_ratio = 0.25F;

        /// Number of shells used to compute the volume illumination from the SAT.
        unsigned shells = 4;

        /// Radius of the shells used to compute the volume illumination from the SAT.
        unsigned radius = 4;

        /// Angle used to define the soft shadows cones.
        float angle = 0.1F;

        /// Number of samples along the soft shadows cones.
        unsigned samples = 50;
    };

    /// Initializes the illumination volume with the give dimensions.
    VIZ_SCENE3D_API illum_ambient_occlusion_sat(
        std::string _parent_id,
        Ogre::SceneManager* _scene_manager,
        bool _ao                                    = false,
        bool _shadows                               = false,
        std::optional<sat_parameters_t> _parameters = {});

    /// Destructor, does nothing.
    VIZ_SCENE3D_API ~illum_ambient_occlusion_sat();

    /// Computes a new SAT with a different resolution given by the size ratio.
    VIZ_SCENE3D_API void update_sat_from_ratio(float _sat_size_ratio);

    /// Recomputes the illumination volume using the current SAT.
    VIZ_SCENE3D_API void update_volume_illumination();

    /// Recomputes the SAT and the illumination volume when the image or the TF changed.
    VIZ_SCENE3D_API void sat_update(
        const texture::sptr& _img,
        const viz::scene3d::transfer_function::sptr& _tf,
        float _sample_distance
    );

    /// Ambient occlusion / color bleeding usage setter
    VIZ_SCENE3D_API void set_ao(bool _ao);

    /// Soft shadows usage setter.
    VIZ_SCENE3D_API void set_shadows(bool _shadows);

    /// Shells number setter.
    VIZ_SCENE3D_API void set_nb_shells(unsigned _nb_shells);

    /// Shell radius setter.
    VIZ_SCENE3D_API void set_shell_radius(unsigned _shell_radius);

    /// Cone angle setter.
    VIZ_SCENE3D_API void set_cone_angle(float _cone_angle);

    /// Cone samples number setter.
    VIZ_SCENE3D_API void set_samples_along_cone(unsigned _samples_along_cone);

    ///@brief Parameters
    [[nodiscard]] VIZ_SCENE3D_API const sat_parameters_t& parameters() const;

    ///@brief Returns the texture used to store the illumination volume.
    [[nodiscard]] const Ogre::TexturePtr& get_illumination_volume() const;

private:

    /// Allocates or resize the texture used to store the illumination volume.
    void update_texture();

    /// texture holding the illumination volume.
    Ogre::TexturePtr m_illumination_volume;

    /// Sets ambient occlusion / color bleeding usage.
    bool m_ao;

    /// Sets soft shadows usage.
    bool m_shadows;

    /// The index of the slice to which we currently render.
    int m_current_slice_index {};

    ///@brief Parameters
    sat_parameters_t m_parameters;

    /// Prefix used to name the buffers.
    std::string m_parent_id;

    /// Resource name of the source buffer, i.e. "__IlluminationVolume".
    static inline const std::string BUFFER_NAME = "__IlluminationVolume";

    /// Camera used
    std::unique_ptr<Ogre::Camera> m_dummy_camera {nullptr};

    /// scene manager.
    Ogre::SceneManager* m_scene_manager;

    /// SAT used to compute the illumination volume.
    viz::scene3d::vr::summed_area_table m_sat;
};

//-----------------------------------------------------------------------------
// Inline method(s)

inline const Ogre::TexturePtr& illum_ambient_occlusion_sat::get_illumination_volume() const
{
    return m_illumination_volume;
}

//------------------------------------------------------------------------------

inline void illum_ambient_occlusion_sat::set_ao(bool _ao)
{
    m_ao = _ao;
}

//-----------------------------------------------------------------------------

inline void illum_ambient_occlusion_sat::set_shadows(bool _shadows)
{
    m_shadows = _shadows;
}

//-----------------------------------------------------------------------------

inline void illum_ambient_occlusion_sat::set_nb_shells(unsigned _nb_shells)
{
    m_parameters.shells = _nb_shells;
}

//-----------------------------------------------------------------------------

inline void illum_ambient_occlusion_sat::set_shell_radius(unsigned _shell_radius)
{
    m_parameters.radius = _shell_radius;
}

//-----------------------------------------------------------------------------

inline void illum_ambient_occlusion_sat::set_cone_angle(float _cone_angle)
{
    m_parameters.angle = _cone_angle;
}

//-----------------------------------------------------------------------------

inline void illum_ambient_occlusion_sat::set_samples_along_cone(unsigned _samples_along_cone)
{
    m_parameters.samples = _samples_along_cone;
}

//------------------------------------------------------------------------------

inline auto illum_ambient_occlusion_sat::parameters() const -> const sat_parameters_t&
{
    return m_parameters;
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::vr
