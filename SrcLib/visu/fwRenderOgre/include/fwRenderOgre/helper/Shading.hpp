/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
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
     * @param _tech[in] Ogre technique
     */
    FWRENDEROGRE_API static bool isColorTechnique(::Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used for a peel pass in a depth peeling.
     * @param _tech[in] Ogre technique
     */
    FWRENDEROGRE_API static bool isPeelTechnique(::Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used in a geometric pass (as opposed to a fullscreen pass).
     * @param _tech[in] Ogre technique
     */
    FWRENDEROGRE_API static bool isGeometricTechnique(::Ogre::Technique& _tech);

    /**
     * @brief Returns true if the given technique is used in a depth-only pass.
     * @param _tech[in] Ogre technique
     */
    FWRENDEROGRE_API static bool isDepthOnlyTechnique(::Ogre::Technique& _tech);

    /**
     * @brief Constructs a suffix to use in vertex and fragment programs names.
     * @param _vertexColor[in] is vertex color enabled ?
     * @param _diffuseTexture[in] is diffuse texture bound ?
     */
    FWRENDEROGRE_API static std::string getProgramSuffix(::fwData::Material::ShadingType _mode, bool _diffuseTexture,
                                                         bool _vertexColor);

    /**
     * @brief Constructs the name of the geometry program to use in render to vertex buffer pipeline.
     * @param _primitiveType[in] type of the primitive (only triangles, quads and tetrahedrons supported right now)
     * @param _vertexColor[in] is vertex color enabled ?
     * @param _diffuseTexture[in] is diffuse texture bound ?
     * @param _diffuseTexture[in] is primitive color enabled bound ?
     */
    FWRENDEROGRE_API static std::string getR2VBGeometryProgramName(::fwData::Mesh::CellTypesEnum _primitiveType,
                                                                   bool _diffuseTexture, bool _vertexColor,
                                                                   bool _hasPrimitiveColor);

    /**
     * @brief Replace the suffix in the program name with the suffix in parameters.
     *        For instance, given
     *
     * @param _prgName[in] name of the program
     * @param _suffix[in] new suffix to use
     */
    FWRENDEROGRE_API static std::string replaceProgramSuffix(const std::string& _prgName, const std::string _suffix);

};

} // namespace helper

} // namespace fwRenderOgre

#endif // __FWRENDEROGRE_HELPER_SHADING_HPP__

