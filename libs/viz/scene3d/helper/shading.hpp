/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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
    typedef std::tuple<Ogre::String, Ogre::GpuConstantType,
                       Ogre::GpuProgramType, constant_value_t> shader_constant_t;
    typedef std::vector<shader_constant_t> shader_constants_t;
    typedef std::vector<std::pair<std::string, std::string> > gpu_program_parameters_t;

    /**
     * @brief Returns true if the given technique computes a pixel color.
     * @param[in] _tech Ogre technique
     */
    VIZ_SCENE3D_API static bool isColorTechnique(const Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used for a peel pass in a depth peeling.
     * @param[in] _tech Ogre technique
     */
    VIZ_SCENE3D_API static bool isPeelTechnique(const Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used in a geometric pass (as opposed to a fullscreen pass).
     * @param[in] _tech Ogre technique
     */
    VIZ_SCENE3D_API static bool isGeometricTechnique(const Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used in a depth-only pass.
     * @param[in] _tech Ogre technique
     */
    VIZ_SCENE3D_API static bool isDepthOnlyTechnique(const Ogre::Technique& _tech);

    /**
     * @brief Constructs a suffix to use in vertex and fragment programs names.
     * @param[in] _vertexColor is vertex color enabled ?
     * @param[in] _diffuseTexture is diffuse texture bound ?
     */
    VIZ_SCENE3D_API static std::string getPermutation(
        data::material::shading_t _mode,
        bool _diffuse_texture,
        bool _vertex_color
    );

    /**
     * @brief Constructs the name of the geometry program to use in render to vertex buffer pipeline.
     * @param[in] _primitiveType type of the primitive (only triangles, quads and tetrahedrons supported right now)
     * @param[in] _diffuseTexture is diffuse texture bound ?
     * @param[in] _vertexColor is vertex color enabled ?
     * @param[in] _hasPrimitiveColor is primitive color enabled bound ?
     */
    VIZ_SCENE3D_API static std::string getR2VBGeometryProgramName(
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
    VIZ_SCENE3D_API static std::string setPermutationInProgramName(
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
     * @param[in] _suffix new suffix to use
     */
    VIZ_SCENE3D_API static std::string setTechniqueInProgramName(const std::string& _name, const std::string& _tech);

    /**
     * @brief Find all shader constants of a material.
     *
     * @param[in] _material Ogre material
     * @return vector of constants, each element is a tuple with the constant name its definition and the shader type.
     */
    VIZ_SCENE3D_API static shader_constants_t findMaterialConstants(Ogre::Material& _material);

    /**
     * @brief Create a Sight data that can be used to interact with a shader parameter.
     *
     * @param[in] _params shader parameters
     * @param[in] _shaderType shader type (vertex, fragment or geometry)
     * @param[in] _enableLightConstants indicates whether or not the method should look into the light related constants
     * @return vector of constants, each element is a tuple with the constant name, its definition and the shader type.
     */
    VIZ_SCENE3D_API static shader_constants_t findShaderConstants(
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
    VIZ_SCENE3D_API static SPTR(data::object) createObjectFromShaderParameter(
        Ogre::GpuConstantType _type,
        constant_value_t _value
    );

    /**
     * @brief Create a gpu program variant based from an existing gpu program.
     *        Typically used when we want to add some preprocessor defines.
     *
     * @param[in] _name name of gpu program resource
     * @param[in] _sourceFileName name of the glsl source file
     * @param[in] _sourceFileName name of the glsl source file
     * @param[in] _parameters parameters of the program
     * @param[in] _shaderType shader type (vertex, fragment or geometry)
     * @param[in] _baseName name of the base gpu program
     */
    VIZ_SCENE3D_API static Ogre::GpuProgramPtr createProgramFrom(
        const std::string& _name,
        const std::string& _source_file_name,
        const gpu_program_parameters_t& _parameters,
        Ogre::GpuProgramType _shader_type,
        const std::string& _base_name
    );
};

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene3d::helper
