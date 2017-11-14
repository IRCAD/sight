/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/Material.hpp"

#include "fwRenderOgre/helper/Shading.hpp"

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
    m_hasMeshNormal(true),
    m_hasVertexColor(false),
    m_hasPrimitiveColor(false),
    m_primitiveType(::fwData::Mesh::TRIANGLE),
    m_meshBoundingBox(::Ogre::Vector3::ZERO, ::Ogre::Vector3::ZERO),
    m_normalLengthFactor(0.1f),
    m_templateName(_templateName)
{
    m_material = ::Ogre::MaterialManager::getSingleton().create(
        _name, ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    ::Ogre::MaterialPtr ogreMaterial = ::Ogre::MaterialManager::getSingleton().getByName(_templateName);

    SLM_ASSERT( "Material '" + _templateName + "'' not found", ogreMaterial );

    // Then we copy these parameters in m_material.
    // We can now alter this new instance without changing the default material
    ogreMaterial.get()->copyDetailsTo(m_material);
}

//------------------------------------------------------------------------------

Material::~Material()
{

}

//------------------------------------------------------------------------------

void Material::updateOptionsMode(int _optionsMode)
{
    this->cleanTransparencyTechniques();
    // First remove the normals pass if there is already one
    this->removePass(s_NORMALS_PASS);

    const ::Ogre::Material::Techniques techniques = m_material->getTechniques();
    const ::Ogre::Real normalLength               = this->computeNormalLength();

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
                normalsPass->getGeometryProgramParameters()->setNamedConstant("u_normalLength", normalLength);
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

    const ::Ogre::Material::Techniques techniques = m_material->getTechniques();

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

            ::Ogre::Technique::Passes passIt = tech->getPasses();

            for(const auto ogrePass : passIt)
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
    ::fwData::Material::ShadingType mode = static_cast< ::fwData::Material::ShadingType >(_shadingMode);

    ::Ogre::String permutation = ::fwRenderOgre::helper::Shading::getPermutation(mode, _hasDiffuseTexture,
                                                                                 m_hasVertexColor);
    ::Ogre::String r2vbGSName = ::fwRenderOgre::helper::Shading::getR2VBGeometryProgramName(m_primitiveType,
                                                                                            _hasDiffuseTexture,
                                                                                            m_hasVertexColor,
                                                                                            m_hasPrimitiveColor);

    this->cleanTransparencyTechniques();

    // Iterate through each technique found in the material and switch the shading mode
    const ::Ogre::Material::Techniques techniques = m_material->getTechniques();
    for(const auto tech : techniques)
    {
        SLM_ASSERT("Technique is not set", tech);

        ::Ogre::Technique::Passes passIt = tech->getPasses();

        for(const auto ogrePass : passIt)
        {
            // Nothing to do for edge and normal passes
            if (ogrePass->getName() == s_EDGE_PASS || ogrePass->getName() == s_NORMALS_PASS )
            {
                continue;
            }

            if(m_primitiveType != ::fwData::Mesh::TRIANGLE || m_hasPrimitiveColor)
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

                    ::Ogre::TexturePtr tex = ::Ogre::dynamic_pointer_cast< ::Ogre::Texture>( result.first );

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
                    ::Ogre::Vector2 size(static_cast<float>(tex->getWidth()),
                                         static_cast<float>(tex->getHeight() - 1));
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
                    if(_hasDiffuseTexture && colorPass)
                    {
                        int useTextureAlpha = static_cast<int>(_useTextureAlpha);
                        ogrePass->getFragmentProgramParameters()->setNamedConstant("u_useTextureAlpha",
                                                                                   useTextureAlpha);
                    }

                    ::Ogre::GpuProgramParametersSharedPtr vp = ogrePass->getVertexProgramParameters();

                    if(vp->_findNamedConstantDefinition("u_numLights"))
                    {
                        vp->setNamedConstant("u_numLights", _numLights);
                    }

                    if(!ogrePass->getFragmentProgramName().empty())
                    {
                        ::Ogre::GpuProgramParametersSharedPtr fp = ogrePass->getFragmentProgramParameters();

                        if(fp->_findNamedConstantDefinition("u_numLights"))
                        {
                            fp->setNamedConstant("u_numLights", _numLights);
                        }
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void Material::updateRGBAMode(fwData::Material::sptr fw_material)
{
    //Set up Material colors
    ::fwData::Color::sptr f4sAmbient = fw_material->ambient();
    ::fwData::Color::sptr f4sDiffuse = fw_material->diffuse();

    ::Ogre::ColourValue ambient(f4sAmbient->red(), f4sAmbient->green(), f4sAmbient->blue(), f4sAmbient->alpha());
    m_material->setAmbient(ambient);

    ::Ogre::ColourValue diffuse(f4sDiffuse->red(), f4sDiffuse->green(), f4sDiffuse->blue(), f4sDiffuse->alpha());
    m_material->setDiffuse(diffuse);

    ::Ogre::ColourValue specular(.2f, .2f, .2f, 1.f);
    m_material->setSpecular( specular );
    m_material->setShininess( 25 );
}

//------------------------------------------------------------------------------

::Ogre::Real Material::computeNormalLength()
{
    // TODO: use length
    const ::Ogre::Vector3 meshBBoxSize = m_meshBoundingBox.getSize();
    const ::Ogre::Real averageSize     = ( meshBBoxSize.x + meshBBoxSize.y + meshBBoxSize.z ) /
                                         static_cast< ::Ogre::Real >(3.0);
    return averageSize * m_normalLengthFactor;
}

//------------------------------------------------------------------------------

void Material::setDiffuseTexture(const ::Ogre::TexturePtr& _texture)
{
    this->cleanTransparencyTechniques();

    const ::Ogre::Material::Techniques techniques = m_material->getTechniques();

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

//-----------------------------------------------------------------------------

void Material::removePass(const std::string& _name)
{
    SLM_ASSERT("Material is not set", m_material);

    const ::Ogre::Material::Techniques techniques = m_material->getTechniques();
    for(const auto technique : techniques)
    {
        SLM_ASSERT("Technique is not set", technique);

        ::Ogre::Technique::Passes passIt = technique->getPasses();
        std::vector< ::Ogre::Pass* > removePassVector;

        // Collect the passes to remove
        for(const auto ogrePass : passIt)
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

    const ::Ogre::Material::Techniques techniques = m_material->getTechniques();

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
