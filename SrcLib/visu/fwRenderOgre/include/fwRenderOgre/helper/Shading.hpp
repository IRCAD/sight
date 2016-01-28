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
    FWRENDEROGRE_API static std::string getProgramSuffix(::fwData::Material::ShadingType _mode, bool _diffuseTexture,
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
     * @brief Replace the suffix in the program name with the suffix in parameters.
     *
     * @param[in] _prgName name of the program
     * @param[in] _suffix new suffix to use
     */
    FWRENDEROGRE_API static std::string replaceProgramSuffix(const std::string& _prgName, const std::string& _suffix);

    /**
     * @brief Replace the prefix in the program name with the prefix in parameters.
     *        Actually this correspond to replacing the technique and the pass name.
     *
     * @param[in] _prgName name of the program
     * @param[in] _suffix new suffix to use
     */
    FWRENDEROGRE_API static std::string replaceProgramPrefix(const std::string& _prgName, const std::string& prefix);
};

} // namespace helper

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_HELPER_SHADING_HPP__

