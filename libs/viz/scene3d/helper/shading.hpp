/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include <sight/viz/scene3d/config.hpp>

#include <data/material.hpp>
#include <data/mesh.hpp>

#include <OGRE/OgreTechnique.h>

#include <string>

namespace sight::viz::scene3d::helper
{

class shading
{
public:

    union constant_value_t
    {
        std::array<bool, 4> b;
        std::array<int, 4> i;
        std::array<float, 4> f;
        std::array<double, 4> d;
    };
    using shader_constant_t = std::tuple<Ogre::String, Ogre::GpuConstantType, Ogre::GpuProgramType,
                                         constant_value_t>;
    using shader_constants_t       = std::vector<shader_constant_t>;
    using gpu_program_parameters_t = std::vector<std::pair<std::string, std::string> >;

    /**
     * @brief Constructs a suffix to use in vertex and fragment programs names.
     * @param[in] _diffuse_texture is diffuse texture bound ?
     * @param[in] _vertex_color is vertex color enabled ?
     */
    SIGHT_VIZ_SCENE3D_API static std::string get_permutation(
        data::material::shading_t _mode,
        bool _diffuse_texture,
        bool _vertex_color
    );

    /**
     * @brief Constructs the name of the geometry program to use in render to vertex buffer pipeline.
     * @param[in] _primitive_type type of the primitive (only triangles, quads and tetrahedrons supported right now)
     * @param[in] _diffuse_texture is diffuse texture bound ?
     * @param[in] _vertex_color is vertex color enabled ?
     * @param[in] _has_primitive_color is primitive color enabled bound ?
     */
    SIGHT_VIZ_SCENE3D_API static std::string get_r2vb_geometry_program_name(
        data::mesh::cell_type_t _primitive_type,
        bool _diffuse_texture,
        bool _vertex_color,
        bool _has_primitive_color
    );

    /**
     * @brief Modify the program name according to the permutation given in parameter.
     *        For instance, given "HybridTransparency/peel_Ambient+VT_FP" and "Flat" permutation,
     *        the returned program name will be "HybridTransparency/peel_Flat_FP"
     *
     * @param[in] _name name of the program
     * @param[in] _permutation new permutation to use
     */
    SIGHT_VIZ_SCENE3D_API static std::string set_permutation_in_program_name(
        const std::string& _name,
        const std::string& _permutation
    );

    /**
     * @brief Replace the prefix in the program name with the prefix in parameters.
     *        Actually this corresponds to replace the technique and the pass name.
     *
     *        For instance, given "HybridTransparency/peel_Ambient+VT_FP" and "Default" technique,
     *        the returned program name will be "Default/Ambient+VT_FP"
     *
     * @param[in] _name name of the program
     * @param[in] _tech new suffix to use
     * @todo Fix this comment
     */
    SIGHT_VIZ_SCENE3D_API static std::string set_technique_in_program_name(
        const std::string& _name,
        const std::string& _tech
    );

    /**
     * @brief Find all shader constants of a material.
     *
     * @param[in] _material Ogre material
     * @return vector of constants, each element is a tuple with the constant name its definition and the shader type.
     */
    SIGHT_VIZ_SCENE3D_API static shader_constants_t find_material_constants(Ogre::Material& _material);

    /**
     * @brief Create a Sight data that can be used to interact with a shader parameter.
     *
     * @param[in] _params shader parameters
     * @param[in] _shader_type shader type (vertex, fragment or geometry)
     * @param[in] _enable_light_constants indicates whether or not the method should look into the light related
     * constants
     * @return vector of constants, each element is a tuple with the constant name, its definition and the shader type.
     */
    SIGHT_VIZ_SCENE3D_API static shader_constants_t find_shader_constants(
        Ogre::GpuProgramParametersSharedPtr _params,
        Ogre::GpuProgramType _shader_type,
        bool _enable_light_constants = false
    );

    /**
     * @brief Create a Sight data that can be used to interact with a shader parameter.
     *
     * @param[in] _type type of the shader parameter
     * @param[in] _value value of the shader parameter
     */
    SIGHT_VIZ_SCENE3D_API static SPTR(data::object) create_object_from_shader_parameter(
        Ogre::GpuConstantType _type,
        constant_value_t _value
    );

    /**
     * @brief Create a gpu program variant based from an existing gpu program.
     *        Typically used when we want to add some preprocessor defines.
     *
     * @param[in] _name name of gpu program resource
     * @param[in] _source_file_name name of the glsl source file
     * @param[in] _parameters parameters of the program
     * @param[in] _shader_type shader type (vertex, fragment or geometry)
     * @param[in] _base_name name of the base gpu program
     */
    SIGHT_VIZ_SCENE3D_API static Ogre::GpuProgramPtr create_program_from(
        const std::string& _name,
        const std::string& _source_file_name,
        const gpu_program_parameters_t& _parameters,
        Ogre::GpuProgramType _shader_type,
        const std::string& _base_name
    );
};

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
