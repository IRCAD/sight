/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/listener/AutoStereo.hpp"

#include "fwRenderOgre/helper/Camera.hpp"
#include "fwRenderOgre/helper/Shading.hpp"
#include "fwRenderOgre/helper/Technique.hpp"

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

AutoStereoCompositorListener::AutoStereoCompositorListener(std::uint8_t _viewpointNumber) :
    m_viewpointNumber(_viewpointNumber)
{
}

//-----------------------------------------------------------------------------

AutoStereoCompositorListener::~AutoStereoCompositorListener()
{
    // We need to clean the VR techniques because we want to set the correct textures
    // Cleaning the texture forces the listener to be triggered and then to create the techniques with the new textures
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
        const std::string passIdStr = _schemeName.substr(_schemeName.size() - 1);

        ::Ogre::Technique* matchingTech = nullptr;

        if(::Ogre::StringUtil::startsWith( _schemeName, "VolumeEntries"))
        {
            // Volume entries technique names follow this pattern : VolumeEntries<AutoStereo>_<technique><viewport>
            const size_t techNamePos   = _schemeName.find("_") + 1;
            const size_t techNameSize  = _schemeName.size() - 1 - techNamePos;
            const std::string techName = _schemeName.substr(techNamePos, techNameSize);

            auto entryPointsMtl = ::Ogre::MaterialManager::getSingleton().getByName("RayEntryPoints");

            matchingTech = entryPointsMtl->getTechnique(techName);
        }
        else
        {
            matchingTech = _originalMaterial->getTechnique(0);
        }

        newTech = ::fwRenderOgre::helper::Technique::copyToMaterial(matchingTech, _schemeName, _originalMaterial);

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

                SLM_ASSERT("No texture named 'entryPoints' in " + _originalMaterial->getName(), texUnitState);
                texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);

                const auto volNodeName = _originalMaterial->getName().substr(8);
                const auto compName    = volNodeName + "_VolumeEntries" + std::to_string(m_viewpointNumber);
                texUnitState->setCompositorReference(compName, compName + "Texture" + passIdStr);
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

        m_createdTechniques.push_back(newTech);
    }

    return newTech;
}

//------------------------------------------------------------------------------

} // namespace listener

} // namespace compositor

} // namespace fwRenderOgre
