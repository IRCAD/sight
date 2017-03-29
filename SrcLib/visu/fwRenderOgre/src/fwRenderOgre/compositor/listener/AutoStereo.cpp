/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/listener/AutoStereo.hpp"

#include "fwRenderOgre/helper/Camera.hpp"
#include "fwRenderOgre/helper/Shading.hpp"

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>

namespace fwRenderOgre
{
namespace compositor
{
namespace listener
{

//-----------------------------------------------------------------------------

AutoStereoCompositorListener::AutoStereoCompositorListener() :
    m_renderTargets(nullptr)
{
}

//-----------------------------------------------------------------------------

AutoStereoCompositorListener::AutoStereoCompositorListener(std::vector<Ogre::TexturePtr>* renderTargets) :
    m_renderTargets(renderTargets)
{
}

//------------------------------------------------------------------------------

::Ogre::Technique* AutoStereoCompositorListener::handleSchemeNotFound(unsigned short _schemeIndex,
                                                                      const ::Ogre::String& _schemeName,
                                                                      ::Ogre::Material* _originalMaterial,
                                                                      unsigned short _lodIndex,
                                                                      const ::Ogre::Renderable* _renderable)
{
    ::Ogre::Technique* newTech = nullptr;
    if(_schemeName.find("AutoStereo") != std::string::npos)
    {
        const bool isRayTracedMtl = ::Ogre::StringUtil::startsWith( _originalMaterial->getName(), "RayTracedVolume");
        if(  isRayTracedMtl && m_renderTargets == nullptr )
        {
            return nullptr;
        }
        if( !isRayTracedMtl && m_renderTargets != nullptr )
        {
            return nullptr;
        }

        const std::string passIdStr = _schemeName.substr(11);
        const int passId = std::stoi(passIdStr);

        ::Ogre::Technique* defaultTech = _originalMaterial->getTechnique(0);
        newTech                        = this->copyTechnique(defaultTech, _schemeName, _originalMaterial);

        const auto pass           = newTech->getPass(0);
        const auto baseName       = pass->getVertexProgramName();
        const auto sourceFileName = pass->getVertexProgram()->getSourceFile();
        const auto newName        = baseName + "+AutoStereo";

        ::fwRenderOgre::helper::Shading::GpuProgramParametersType parameters;
        parameters.push_back(std::make_pair<std::string, std::string>("preprocessor_defines", "AUTOSTEREO=1"));

        ::fwRenderOgre::helper::Shading::createProgramFrom(newName, sourceFileName, parameters,
                                                           ::Ogre::GPT_VERTEX_PROGRAM, baseName);

        pass->setVertexProgram(newName);

        auto vertexParams = pass->getVertexProgramParameters();

        // First check that we did not already instanced Shared parameters
        // This can happen when reinstancing this class (e.g. switching 3D mode)
        const std::string projParamName = "ProjectionMatrixParam/"+passIdStr;

        auto& gpuProgramMgr = ::Ogre::GpuProgramManager::getSingleton();
        const auto& sharedParameterMap = gpuProgramMgr.getAvailableSharedParameters();

        if(sharedParameterMap.find(projParamName) == sharedParameterMap.end())
        {
            m_projectionParam = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters(projParamName);

            // define the shared param structure
            m_projectionParam->addConstantDefinition("u_proj", ::Ogre::GCT_MATRIX_4X4);
        }
        else
        {
            m_projectionParam = ::Ogre::GpuProgramManager::getSingleton().getSharedParameters(projParamName);
        }

        vertexParams->addSharedParameters(projParamName);
        vertexParams->setNamedAutoConstant("u_worldView", ::Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);

        if(baseName == "RayTracedVolume_VP")
        {
            ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("entryPoints");

            OSLM_ASSERT("No texture named " << passId, texUnitState);
            texUnitState->setTextureName((*m_renderTargets)[static_cast<size_t>(passId)]->getName());
        }
    }

    return newTech;
}

// ----------------------------------------------------------------------------

Ogre::Technique* AutoStereoCompositorListener::copyTechnique(::Ogre::Technique* _tech,
                                                             const ::Ogre::String& _schemeName,
                                                             ::Ogre::Material* _originalMaterial)
{
    ::Ogre::Technique* newTech = _originalMaterial->createTechnique();
    *newTech                   = *_tech;
    newTech->setName(_schemeName);
    newTech->setSchemeName(_schemeName);

    ::Ogre::Pass* pass = newTech->getPass(0);
    SLM_ASSERT("Empty pass", pass);

    return newTech;
}
//------------------------------------------------------------------------------

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre
