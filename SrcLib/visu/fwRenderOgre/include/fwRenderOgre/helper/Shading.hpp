/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDEROGRE_HELPER_SHADING_HPP__
#define __FWRENDEROGRE_HELPER_SHADING_HPP__

#include "fwRenderOgre/config.hpp"

#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>

#include <OGRE/OgreTechnique.h>

#include <string>

namespace fwRenderOgre
{

namespace helper
{

class Shading
{
public:
    union ConstantValueType
    {
        std::array<bool, 4> b;
        std::array<int, 4> i;
        std::array<float, 4> f;
        std::array<double, 4> d;
    };
    typedef std::tuple< ::Ogre::String, ::Ogre::GpuConstantType,
                        ::Ogre::GpuProgramType, ConstantValueType > ShaderConstantType;
    typedef std::vector<ShaderConstantType> ShaderConstantsType;

    /**
     * @brief Returns true if the given technique computes a pixel color.
     * @param[in] _tech Ogre technique
     */
    FWRENDEROGRE_API static bool isColorTechnique(const ::Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used for a peel pass in a depth peeling.
     * @param[in] _tech Ogre technique
     */
    FWRENDEROGRE_API static bool isPeelTechnique(const ::Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used in a geometric pass (as opposed to a fullscreen pass).
     * @param[in] _tech Ogre technique
     */
    FWRENDEROGRE_API static bool isGeometricTechnique(const ::Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used in a depth-only pass.
     * @param[in] _tech Ogre technique
     */
    FWRENDEROGRE_API static bool isDepthOnlyTechnique(const ::Ogre::Technique& _tech);

    /**
     * @brief Constructs a suffix to use in vertex and fragment programs names.
     * @param[in] _vertexColor is vertex color enabled ?
     * @param[in] _diffuseTexture is diffuse texture bound ?
     */
    FWRENDEROGRE_API static std::string getPermutation(::fwData::Material::ShadingType _mode, bool _diffuseTexture,
                                                       bool _vertexColor);

    /**
     * @brief Constructs the name of the geometry program to use in render to vertex buffer pipeline.
     * @param[in] _primitiveType type of the primitive (only triangles, quads and tetrahedrons supported right now)
     * @param[in] _diffuseTexture is diffuse texture bound ?
     * @param[in] _vertexColor is vertex color enabled ?
     * @param[in] _hasPrimitiveColor is primitive color enabled bound ?
     */
    FWRENDEROGRE_API static std::string getR2VBGeometryProgramName(::fwData::Mesh::CellTypesEnum _primitiveType,
                                                                   bool _diffuseTexture, bool _vertexColor,
                                                                   bool _hasPrimitiveColor);

    /**
     * @brief Modify the program name according to the permutation given in parameter.
     *        For instance, given "HybridTransparency/peel_Ambient+VT_FP" and "Flat" permutation,
     *        the returned program name will be "HybridTransparency/peel_Flat_FP"
     *
     * @param[in] _name name of the program
     * @param[in] _permutation new permutation to use
     */
    FWRENDEROGRE_API static std::string setPermutationInProgramName(const std::string& _name,
                                                                    const std::string& _permutation);

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
    FWRENDEROGRE_API static std::string setTechniqueInProgramName(const std::string& _name, const std::string& _tech);

    /**
     * @brief Find all shader constants of a material.
     *
     * @param[in] _material Ogre material
     * @return vector of constants, each element is a tuple with the constant name its definition and the shader type.
     */
    FWRENDEROGRE_API static ShaderConstantsType findMaterialConstants(::Ogre::Material& _material);

    /**
     * @brief Create a fw4spl data that can be used to interact with a shader parameter.
     *
     * @param[in] _params shader parameters
     * @param[in] _shaderType shader type (vertex, fragment or geometry)
     * @param[in] _enableLightConstants indicates whether or not the method should look into the light related constants
     * @return vector of constants, each element is a tuple with the constant name, its definition and the shader type.
     */
    FWRENDEROGRE_API static ShaderConstantsType findShaderConstants(::Ogre::GpuProgramParametersSharedPtr params,
                                                                    ::Ogre::GpuProgramType _shaderType,
                                                                    bool _enableLightConstants = false);

    /**
     * @brief Create a fw4spl data that can be used to interact with a shader parameter.
     *
     * @param[in] _type type of the shader parameter
     * @param[in] _value value of the shader parameter
     */
    FWRENDEROGRE_API static SPTR(::fwData::Object) createObjectFromShaderParameter(::Ogre::GpuConstantType _type,
                                                                                   ConstantValueType _value);

    /**
     * @brief Loops through the constants of a shader and updates the specified constant when it has been found.
     *
     * @param[in] _parameters shader parameters
     * @param[in] _shaderType type of the shader (vertex, fragment or geometry)
     * @param[in] _uniformName name of the constant to update
     * @param[in] _uniform updated value for the constant
     * @param[in] _enableLightParams indicates whether or not the method should look into the light related uniforms
     */
    template<class T>
    FWRENDEROGRE_API static void updateUniform(::Ogre::GpuProgramParametersSharedPtr _parameters,
                                               ::Ogre::GpuProgramType _shaderType,
                                               std::string _uniformName, T _uniform,
                                               bool _enableLightParams = true);
};

//-----------------------------------------------------------------------------

template<class T>
void Shading::updateUniform(::Ogre::GpuProgramParametersSharedPtr _parameters,
                            ::Ogre::GpuProgramType _shaderType, std::string _uniformName, T _uniform,
                            bool _enableLightParams)
{
    ::fwRenderOgre::helper::Shading::ShaderConstantsType constants =
        ::fwRenderOgre::helper::Shading::findShaderConstants(_parameters, _shaderType, _enableLightParams);

    std::for_each(constants.begin(), constants.end(),
                  [&](::fwRenderOgre::helper::Shading::ShaderConstantType constantTuple)
            {
                if(std::get<0>(constantTuple) == _uniformName)
                {
                    _parameters->setNamedConstant(_uniformName, _uniform);
                }
            });
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_HELPER_SHADING_HPP__

