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

//-----------------------------------------------------------------------------

AutoStereoCompositorListener::~AutoStereoCompositorListener()
{
    // We need to clean the VR techniques because we want to set the correct textures
    // Cleanng the texture forces the listener to be triggered and then to create the techniques with the new textures
    for(auto& tech : m_createdTechniques)
    {
        ::Ogre::Material* mtl = tech->getParent();

        const ::Ogre::Material::Techniques techniques = mtl->getTechniques();

        std::vector< unsigned short > removeTechniqueVector;

        unsigned short index = 0;
        for(const auto technique : techniques)
        {
            if(tech == technique)
            {
                removeTechniqueVector.push_back(index);
            }
            ++index;
        }

        // Remove in inverse order otherwise the index we stored becomes invalid ;-)
        for(auto it = removeTechniqueVector.rbegin(); it != removeTechniqueVector.rend(); ++it )
        {
            mtl->removeTechnique(*it);
        }
    }
    m_renderTargets = nullptr;
}

//------------------------------------------------------------------------------

::Ogre::Technique* AutoStereoCompositorListener::handleSchemeNotFound(unsigned short /*_schemeIndex*/,
                                                                      const ::Ogre::String& _schemeName,
                                                                      ::Ogre::Material* _originalMaterial,
                                                                      unsigned short /*_lodIndex*/,
                                                                      const ::Ogre::Renderable* /*_renderable*/)
{
    ::Ogre::Technique* newTech = nullptr;
    if(_schemeName.find("AutoStereo") != std::string::npos)
    {
        const bool isRayTracedMtl = ::Ogre::StringUtil::startsWith( _originalMaterial->getName(), "RTV_");
        if(  isRayTracedMtl && m_renderTargets == nullptr )
        {
            return nullptr;
        }
        if( !isRayTracedMtl && m_renderTargets != nullptr )
        {
            return nullptr;
        }

        const std::string passIdStr = _schemeName.substr(11);
        const int passId            = std::stoi(passIdStr);
        if( m_renderTargets != nullptr && static_cast<unsigned>(passId) >= m_renderTargets->size())
        {
            // We reach this branch when switching between two stereo modes
            // because the new compositor is instantiated but the volume adaptor is not yet restarted
            return nullptr;
        }

        ::Ogre::Technique* defaultTech = _originalMaterial->getTechnique(0);
        newTech                        = this->copyTechnique(defaultTech, _schemeName, _originalMaterial);

        const auto pass = newTech->getPass(0);
        {
            const auto vpBaseName       = pass->getVertexProgramName();
            const auto vpSourceFileName = pass->getVertexProgram()->getSourceFile();
            const auto vpNewName        = vpBaseName + "+AutoStereo";

            ::fwRenderOgre::helper::Shading::GpuProgramParametersType parameters;
            parameters.push_back(std::make_pair<std::string, std::string>("preprocessor_defines", "AUTOSTEREO=1"));

            ::fwRenderOgre::helper::Shading::createProgramFrom(vpNewName, vpSourceFileName, parameters,
                                                               ::Ogre::GPT_VERTEX_PROGRAM, vpBaseName);

            pass->setVertexProgram(vpNewName);
            auto vpParams = pass->getVertexProgramParameters();

            // We use a shared parameters block to upload the custom projection matrices
            const std::string projParamName = "ProjectionMatrixParam/"+passIdStr;

            auto& gpuProgramMgr            = ::Ogre::GpuProgramManager::getSingleton();
            const auto& sharedParameterMap = gpuProgramMgr.getAvailableSharedParameters();

            if(sharedParameterMap.find(projParamName) == sharedParameterMap.end())
            {
                auto projParam = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters(projParamName);

                // define the shared param structure
                projParam->addConstantDefinition("u_proj", ::Ogre::GCT_MATRIX_4X4);
            }

            vpParams->addSharedParameters(projParamName);
            vpParams->setNamedAutoConstant("u_worldView", ::Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);

            if( ::Ogre::StringUtil::startsWith( vpBaseName, "RTV_VP") )
            {
                ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("entryPoints");

                OSLM_ASSERT("No texture named " << passId, texUnitState);
                texUnitState->setTextureName((*m_renderTargets)[static_cast<size_t>(passId)]->getName());
                m_createdTechniques.push_back(newTech);
            }
        }

        const auto fpBaseName = pass->getFragmentProgramName();
        if( ::Ogre::StringUtil::startsWith( fpBaseName, "RTV_FP") )
        {
            const auto fpSourceFileName = pass->getFragmentProgram()->getSourceFile();
            const auto fpNewName        = fpBaseName + "+AutoStereo";

            ::fwRenderOgre::helper::Shading::GpuProgramParametersType parameters;
            parameters.push_back(std::make_pair<std::string, std::string>("preprocessor_defines", "AUTOSTEREO=1"));

            ::fwRenderOgre::helper::Shading::createProgramFrom(fpNewName, fpSourceFileName, parameters,
                                                               ::Ogre::GPT_FRAGMENT_PROGRAM, fpBaseName);

            pass->setFragmentProgram(fpNewName);
            auto fpParams = pass->getFragmentProgramParameters();

            // We use a shared parameters block to upload the custom projection matrices
            const std::string projParamName = "InverseProjectionMatrixParam/"+passIdStr;

            auto& gpuProgramMgr            = ::Ogre::GpuProgramManager::getSingleton();
            const auto& sharedParameterMap = gpuProgramMgr.getAvailableSharedParameters();

            if(sharedParameterMap.find(projParamName) == sharedParameterMap.end())
            {
                auto params = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters(projParamName);

                // define the shared param structure
                params->addConstantDefinition("u_invProj", ::Ogre::GCT_MATRIX_4X4);
            }

            fpParams->addSharedParameters(projParamName);
            fpParams->setNamedAutoConstant("u_invWorldView",
                                           ::Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEW_MATRIX);
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
    (void)(pass);
    SLM_ASSERT("Empty pass", pass);

    return newTech;
}

//------------------------------------------------------------------------------

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre
