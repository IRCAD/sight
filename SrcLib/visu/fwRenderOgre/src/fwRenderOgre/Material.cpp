/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "fwRenderOgre/Material.hpp"

#include "fwRenderOgre/helper/Shading.hpp"
#include <fwRenderOgre/Layer.hpp>

#include <OgreMaterialManager.h>
#include <OgreTechnique.h>
#include <OgreTextureManager.h>

namespace fwRenderOgre
{

static const std::string s_EDGE_PASS    = "EdgePass";
static const std::string s_NORMALS_PASS = "NormalsPass";

//-----------------------------------------------------------------------------

const std::string Material::DEFAULT_MATERIAL_TEMPLATE_NAME = "Default";

//------------------------------------------------------------------------------

Material::Material(const std::string& _name, const std::string& _templateName) :
    m_templateName(_templateName)
{
    m_material = ::Ogre::MaterialManager::getSingleton().create(
        _name, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    const ::Ogre::MaterialPtr ogreMaterial = ::Ogre::MaterialManager::getSingleton().getByName(_templateName);

    SLM_ASSERT( "Material '" + _templateName + "'' not found", ogreMaterial );

    // Then we copy these parameters in m_material.
    // We can now alter this new instance without changing the default material
    ogreMaterial.get()->copyDetailsTo(m_material);
}

//------------------------------------------------------------------------------

Material::~Material()
{
    m_material.reset();
}

//------------------------------------------------------------------------------

void Material::updateOptionsMode(int _optionsMode)
{
    this->cleanTransparencyTechniques();
    // First remove the normals pass if there is already one
    this->removePass(s_NORMALS_PASS);

    const ::Ogre::Material::Techniques& techniques = m_material->getTechniques();

    if(_optionsMode != ::fwData::Material::STANDARD)
    {
        for(const auto currentTechnique : techniques)
        {
            // We need the first pass of the current technique in order to copy its rendering states in the normals pass
            ::Ogre::Pass* firstPass = currentTechnique->getPass(0);
            SLM_ASSERT("Pass is null", firstPass);

            const bool depthOnly = ::fwRenderOgre::helper::Shading::isDepthOnlyTechnique(*currentTechnique);

            if( ::fwRenderOgre::helper::Shading::isGeometricTechnique(*currentTechnique) || depthOnly )
            {
                // We copy the first pass, thus keeping all rendering states
                ::Ogre::Pass* normalsPass = currentTechnique->createPass();
                *normalsPass              = *firstPass;
                normalsPass->setName(s_NORMALS_PASS);

                // Vertex shader
                normalsPass->setVertexProgram("Default/Normal_VP");

                std::string gpName = depthOnly ? "DepthPeeling/depthMap/" : "";
                gpName += (_optionsMode == ::fwData::Material::NORMALS) ?
                          "VerticesNormalsDisplay_GP" :
                          "CellsNormalsDisplay_GP";

                normalsPass->setGeometryProgram(gpName);

                if(!depthOnly)
                {
                    std::string fpName = normalsPass->getFragmentProgramName();
                    fpName = ::fwRenderOgre::helper::Shading::setPermutationInProgramName(fpName, "Edge_Normal");
                    normalsPass->setFragmentProgram(fpName);
                }

                // Updates the normal length according to the bounding box's size
                normalsPass->getGeometryProgramParameters()->setNamedConstant("u_sceneSize", m_meshSize);
            }
        }
    }
    else
    {
        for(const auto currentTechnique : techniques)
        {
            // We need the first pass of the current technique in order to copy its rendering states in the normals pass
            const ::Ogre::Pass* firstPass = currentTechnique->getPass(0);
            SLM_ASSERT("Pass is null", firstPass);

            if(firstPass->hasGeometryProgram())
            {
                ::Ogre::GpuProgramParametersSharedPtr gp = firstPass->getGeometryProgramParameters();

                if(gp && gp->_findNamedConstantDefinition("u_billboardSize"))
                {
                    gp->setNamedConstant("u_billboardSize", m_meshSize);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void Material::updatePolygonMode(int _polygonMode)
{
    // This is necessary to load and compile the material, otherwise the following technique iterator
    // is null when we call this method on the first time (from starting() for instance)
    m_material->touch();

    this->cleanTransparencyTechniques();

    // First remove a previous normal pass if it exists
    this->removePass(s_EDGE_PASS);

    const ::Ogre::Material::Techniques& techniques = m_material->getTechniques();

    if(_polygonMode == ::fwData::Material::EDGE)
    {
        for(const auto tech : techniques)
        {
            SLM_ASSERT("Technique is not set", tech);

            ::Ogre::Pass* firstPass = tech->getPass(0);
            SLM_ASSERT("No pass found", firstPass);

            firstPass->setPolygonMode(::Ogre::PM_SOLID);
            firstPass->setPointSpritesEnabled(false);

            ::Ogre::Pass* edgePass = tech->getPass(s_EDGE_PASS);
            if(!edgePass)
            {
                // We copy the first pass, thus keeping all rendering states
                edgePass  = tech->createPass();
                *edgePass = *firstPass;
                edgePass->setName(s_EDGE_PASS);

                // Then we switch the vertex shader...
                edgePass->setVertexProgram("Default/Edge_VP");

                // ... and the fragment shader
                std::string fpName = edgePass->getFragmentProgramName();
                fpName = ::fwRenderOgre::helper::Shading::setPermutationInProgramName(fpName, "Edge_Normal");
                edgePass->setFragmentProgram(fpName);

                edgePass->setPolygonMode(::Ogre::PM_WIREFRAME);
            }
        }
    }
    else
    {
        for(const auto tech : techniques)
        {
            SLM_ASSERT("Technique is not set", tech);

            const ::Ogre::Technique::Passes& passes = tech->getPasses();

            for(const auto ogrePass : passes)
            {
                switch( _polygonMode )
                {
                    case ::fwData::Material::SURFACE:
                        ogrePass->setPolygonMode(::Ogre::PM_SOLID );
                        ogrePass->setPointSpritesEnabled(false);
                        break;

                    case ::fwData::Material::WIREFRAME:
                        ogrePass->setPolygonMode(::Ogre::PM_WIREFRAME);
                        break;

                    case ::fwData::Material::POINT:
                        ogrePass->setPolygonMode(::Ogre::PM_POINTS);
                        ogrePass->setPointSpritesEnabled(false);
                        ogrePass->setPointSize(1.f);
                        break;

                    default:
                        if( _polygonMode != ::fwData::Material::EDGE )
                        {
                            SLM_ASSERT("Unhandled material representation mode : " << _polygonMode, false );
                        }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void Material::updateShadingMode( int _shadingMode, int _numLights, bool _hasDiffuseTexture, bool _useTextureAlpha )
{
    const bool isR2VB = m_primitiveType != ::fwData::Mesh::TRIANGLE || m_hasPrimitiveColor;

    // If we have UVs with R2VB, then for the sake of simplicity we generate UVs whatever the material is
    // Otherwise we would have to update the whole R2VB pipeline each time a texture is set/unset
    const bool needDiffuseTexture = isR2VB ? m_hasUV : m_hasUV && _hasDiffuseTexture;

    const ::fwData::Material::ShadingType mode = static_cast< ::fwData::Material::ShadingType >(_shadingMode);

    const ::Ogre::String permutation = ::fwRenderOgre::helper::Shading::getPermutation(mode, needDiffuseTexture,
                                                                                       m_hasVertexColor);
    const ::Ogre::String r2vbGSName = ::fwRenderOgre::helper::Shading::getR2VBGeometryProgramName(m_primitiveType,
                                                                                                  needDiffuseTexture,
                                                                                                  m_hasVertexColor,
                                                                                                  m_hasPrimitiveColor);

    this->cleanTransparencyTechniques();

    // Iterate through each technique found in the material and switch the shading mode
    const ::Ogre::Material::Techniques& techniques = m_material->getTechniques();
    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        const ::Ogre::Technique::Passes& passes = tech->getPasses();

        for(const auto ogrePass : passes)
        {
            // Nothing to do for edge and normal passes
            if (ogrePass->getName() == s_EDGE_PASS || ogrePass->getName() == s_NORMALS_PASS )
            {
                continue;
            }

            if(isR2VB)
            {
                // We need a geometry shader (primitive generation and per-primitive color)
                // and thus we rely on the render to vertex buffer pipeline

                ogrePass->setVertexProgram("R2VB/" + permutation + "_VP");
                ogrePass->setGeometryProgram(r2vbGSName);
                ogrePass->setFragmentProgram("");

                if(m_hasPrimitiveColor)
                {
                    const auto result = ::Ogre::TextureManager::getSingleton().createOrRetrieve(
                        m_perPrimitiveColorTextureName,
                        ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);

                    SLM_ASSERT("Texture should have been created before in SMesh", !result.second);

                    const ::Ogre::TexturePtr tex = ::Ogre::dynamic_pointer_cast< ::Ogre::Texture>( result.first );

                    const std::string texUnitName = "PerPrimitiveColor";
                    ::Ogre::TextureUnitState* texUnitState = ogrePass->getTextureUnitState(texUnitName);

                    if(texUnitState == nullptr)
                    {
                        OSLM_DEBUG("create unit state: " << m_perPrimitiveColorTextureName);

                        ::Ogre::TextureUnitState* texUnitState = ogrePass->createTextureUnitState();
                        texUnitState->setName(texUnitName);
                        texUnitState->setTexture(tex);
                        texUnitState->setTextureFiltering(::Ogre::TFO_NONE);
                        texUnitState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);

                        const auto unitStateCount = ogrePass->getNumTextureUnitStates();

                        // Unit state is set to 10 in the material file, but the real index is set here
                        // Ogre packs texture unit indices so we can't use spare indices
                        ogrePass->getGeometryProgramParameters()->setNamedConstant("u_colorPrimitiveTexture",
                                                                                   unitStateCount - 1);
                    }

                    // Set size outside the scope of texture creation because the size could vary
                    const ::Ogre::Vector2 size(static_cast<float>(tex->getWidth()),
                                               static_cast<float>(tex->getHeight()));
                    ogrePass->getGeometryProgramParameters()->setNamedConstant("u_colorPrimitiveTextureSize", size);
                }
            }
            else
            {
                if(m_templateName == DEFAULT_MATERIAL_TEMPLATE_NAME)
                {
                    // "Regular" pipeline
                    std::string vpName = ogrePass->getVertexProgramName();
                    vpName = ::fwRenderOgre::helper::Shading::setPermutationInProgramName(vpName, permutation);
                    ogrePass->setVertexProgram(vpName);

                    std::string fpName = ogrePass->getFragmentProgramName();
                    fpName = ::fwRenderOgre::helper::Shading::setPermutationInProgramName(fpName, permutation);
                    ogrePass->setFragmentProgram(fpName);

                    const bool colorPass = ::fwRenderOgre::helper::Shading::isColorTechnique(*tech);

                    // Updates the u_hasTextureAlpha flag uniform according to the configuration of the texture adaptor
                    if(needDiffuseTexture && colorPass)
                    {
                        int useTextureAlpha = static_cast<int>(_useTextureAlpha);
                        ogrePass->getFragmentProgramParameters()->setNamedConstant("u_useTextureAlpha",
                                                                                   useTextureAlpha);
                    }

                    ::Ogre::GpuProgramParametersSharedPtr vp = ogrePass->getVertexProgramParameters();

                    if(vp->_findNamedConstantDefinition("u_iNumLights"))
                    {
                        vp->setNamedConstant("u_iNumLights", _numLights);
                    }

                    if(!ogrePass->getFragmentProgramName().empty())
                    {
                        ::Ogre::GpuProgramParametersSharedPtr fp = ogrePass->getFragmentProgramParameters();

                        if(fp->_findNamedConstantDefinition("u_iNumLights"))
                        {
                            fp->setNamedConstant("u_iNumLights", _numLights);
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void Material::updateRGBAMode(fwData::Material::sptr _sightMaterial)
{
    //Set up Material colors
    ::fwData::Color::csptr sightAmbient = _sightMaterial->ambient();
    ::fwData::Color::csptr sightDiffuse = _sightMaterial->diffuse();

    const ::Ogre::ColourValue ambient(sightAmbient->red(), sightAmbient->green(),
                                      sightAmbient->blue(), sightAmbient->alpha());
    m_material->setAmbient(ambient);

    const ::Ogre::ColourValue diffuse(sightDiffuse->red(), sightDiffuse->green(),
                                      sightDiffuse->blue(), sightDiffuse->alpha());
    m_material->setDiffuse(diffuse);

    const ::Ogre::ColourValue specular(.2f, .2f, .2f, 1.f);
    m_material->setSpecular( specular );
    m_material->setShininess( 25 );
}

//------------------------------------------------------------------------------

void Material::setDiffuseTexture(const ::Ogre::TexturePtr& _texture)
{
    this->cleanTransparencyTechniques();

    const ::Ogre::Material::Techniques& techniques = m_material->getTechniques();

    for(const auto technique : techniques)
    {
        SLM_ASSERT("Technique is not set", technique);

        if(::fwRenderOgre::helper::Shading::isColorTechnique(*technique))
        {
            ::Ogre::Pass* pass                     = technique->getPass(0);
            ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("diffuseTexture");

            if(texUnitState)
            {
                if(_texture)
                {
                    texUnitState->setTexture(_texture);
                }
                else
                {

                    texUnitState->setTextureName("");
                }
            }
            else
            {
                SLM_ERROR("No 'diffuseTexture' texture unit state found in .material."
                          " STexture adaptor will not work properly.");
            }
        }
    }

}

//------------------------------------------------------------------------------

void Material::setTemplate(const std::string& _templateName)
{
    const ::Ogre::MaterialPtr ogreMaterial = ::Ogre::MaterialManager::getSingleton().getByName(_templateName);

    SLM_ASSERT( "Material '" + _templateName + "'' not found", ogreMaterial );

    // Then we copy these parameters in m_material.
    // We can now alter this new instance without changing the default material
    ogreMaterial.get()->copyDetailsTo(m_material);
}

//-----------------------------------------------------------------------------

void Material::removePass(const std::string& _name)
{
    SLM_ASSERT("Material is not set", m_material);

    const ::Ogre::Material::Techniques& techniques = m_material->getTechniques();
    for(const auto technique : techniques)
    {
        SLM_ASSERT("Technique is not set", technique);

        const ::Ogre::Technique::Passes& passes = technique->getPasses();
        std::vector< ::Ogre::Pass* > removePassVector;

        // Collect the passes to remove
        for(const auto ogrePass : passes)
        {
            if(ogrePass->getName() == _name)
            {
                removePassVector.push_back(ogrePass);
                continue;
            }
        }

        // Perform the removal
        for(auto edgePass : removePassVector)
        {
            technique->removePass(edgePass->getIndex());
        }
    }
}

//-----------------------------------------------------------------------------

void Material::cleanTransparencyTechniques()
{
    SLM_ASSERT("Material is not set", m_material);

    const ::Ogre::Material::Techniques& techniques = m_material->getTechniques();

    std::vector< unsigned short > removeTechniqueVector;

    unsigned short index = 0;

    for(const auto technique : techniques)
    {
        SLM_ASSERT("Technique is not set", technique);

        auto scheme = technique->getSchemeName();
        if( ::Ogre::StringUtil::startsWith( scheme, "CelShadingDepthPeeling", false) ||
            ::Ogre::StringUtil::startsWith( scheme, "DepthPeeling", false) ||
            ::Ogre::StringUtil::startsWith( scheme, "DualDepthPeeling", false) ||
            ::Ogre::StringUtil::startsWith( scheme, "HybridTransparency", false) ||
            ::Ogre::StringUtil::startsWith( scheme, "WeightedBlended", false) )
        {
            removeTechniqueVector.push_back(index);
        }
        ++index;
    }

    // Remove in inverse order otherwise the index we stored becomes invalid ;-)
    for(auto it = removeTechniqueVector.rbegin(); it != removeTechniqueVector.rend(); ++it )
    {
        m_material->removeTechnique(*it);
    }
}

} // namespace fwRenderOgre
