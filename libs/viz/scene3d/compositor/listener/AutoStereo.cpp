/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
 * Copyright (C) 2017-2021 IHU Strasbourg
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

#include "viz/scene3d/compositor/listener/AutoStereo.hpp"

#include "viz/scene3d/helper/Camera.hpp"
#include "viz/scene3d/helper/Shading.hpp"
#include "viz/scene3d/helper/Technique.hpp"

#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCamera.h>
#include <OGRE/OgreGpuProgramManager.h>
#include <OGRE/OgreMaterial.h>
#include <OGRE/OgreMatrix4.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreTechnique.h>

namespace sight::viz::scene3d
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
    auto& mtlManager = ::Ogre::MaterialManager::getSingleton();
    // We need to clean the VR techniques because we want to set the correct textures
    // Cleaning the texture forces the listener to be triggered and then to create the techniques with the new textures
    for(auto& techMatPair : m_createdTechniques)
    {
        ::Ogre::MaterialPtr mtl = mtlManager.getByName(techMatPair.second, RESOURCE_GROUP);

        if(mtl == nullptr)
        {
            // The material is already deleted and so is the technique.
            continue;
        }

        ::Ogre::Technique* tech = techMatPair.first;

        const ::Ogre::Material::Techniques techniques = mtl->getTechniques();

        std::vector<unsigned short> removeTechniqueVector;

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
        for(auto it = removeTechniqueVector.rbegin() ; it != removeTechniqueVector.rend() ; ++it)
        {
            mtl->removeTechnique(*it);
        }
    }
}

//------------------------------------------------------------------------------

::Ogre::Technique* AutoStereoCompositorListener::handleSchemeNotFound(
    unsigned short /*_schemeIndex*/,
    const ::Ogre::String& _schemeName,
    ::Ogre::Material* _originalMaterial,
    unsigned short /*_lodIndex*/,
    const ::Ogre::Renderable* /*_renderable*/
)
{
    ::Ogre::Technique* newTech = nullptr;
    if(_schemeName.find("AutoStereo") != std::string::npos)
    {
        const std::string passIdStr = _schemeName.substr(_schemeName.size() - 1);

        ::Ogre::Technique* matchingTech = nullptr;

        if(::Ogre::StringUtil::startsWith(_schemeName, "VolumeEntries"))
        {
            // Volume entries technique names follow this pattern : VolumeEntries<AutoStereo>_<technique><viewport>
            const size_t techNamePos   = _schemeName.find("_") + 1;
            const size_t techNameSize  = _schemeName.size() - 1 - techNamePos;
            const std::string techName = _schemeName.substr(techNamePos, techNameSize);

            auto entryPointsMtl = ::Ogre::MaterialManager::getSingleton().getByName("RayEntryPoints", RESOURCE_GROUP);

            matchingTech = entryPointsMtl->getTechnique(techName);
        }
        else
        {
            matchingTech = _originalMaterial->getTechnique(0);
        }

        newTech = viz::scene3d::helper::Technique::copyToMaterial(matchingTech, _schemeName, _originalMaterial);

        const auto pass = newTech->getPass(0);
        {
            const auto vpBaseName       = pass->getVertexProgramName();
            const auto vpSourceFileName = pass->getVertexProgram()->getSourceFile();
            const auto vpNewName        = vpBaseName + "+AutoStereo";

            viz::scene3d::helper::Shading::GpuProgramParametersType parameters;
            parameters.push_back(std::make_pair<std::string, std::string>("preprocessor_defines", "AUTOSTEREO=1"));

            viz::scene3d::helper::Shading::createProgramFrom(
                vpNewName,
                vpSourceFileName,
                parameters,
                ::Ogre::GPT_VERTEX_PROGRAM,
                vpBaseName
            );

            pass->setVertexProgram(vpNewName);
            auto vpParams = pass->getVertexProgramParameters();

            // We use a shared parameters block to upload the custom projection matrices
            const std::string projParamName = "ProjectionMatrixParam/" + passIdStr;

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
        }

        const auto fpBaseName = pass->getFragmentProgramName();
        if(::Ogre::StringUtil::startsWith(fpBaseName, "RTV_FP"))
        {
            const auto fpSourceFileName = pass->getFragmentProgram()->getSourceFile();
            const auto fpNewName        = fpBaseName + "+AutoStereo";

            viz::scene3d::helper::Shading::GpuProgramParametersType parameters;
            parameters.push_back(std::make_pair<std::string, std::string>("preprocessor_defines", "AUTOSTEREO=1"));

            viz::scene3d::helper::Shading::createProgramFrom(
                fpNewName,
                fpSourceFileName,
                parameters,
                ::Ogre::GPT_FRAGMENT_PROGRAM,
                fpBaseName
            );

            pass->setFragmentProgram(fpNewName);
            auto fpParams = pass->getFragmentProgramParameters();

            // We use a shared parameters block to upload the custom projection matrices
            const std::string invProjParamName = "InverseProjectionMatrixParam/" + passIdStr;
            const std::string projParamName    = "ProjectionMatrixParam/" + passIdStr;

            auto& gpuProgramMgr            = ::Ogre::GpuProgramManager::getSingleton();
            const auto& sharedParameterMap = gpuProgramMgr.getAvailableSharedParameters();

            if(sharedParameterMap.find(projParamName) == sharedParameterMap.end())
            {
                auto params = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters(projParamName);
                params->addConstantDefinition("u_proj", ::Ogre::GCT_MATRIX_4X4);
            }

            if(sharedParameterMap.find(invProjParamName) == sharedParameterMap.end())
            {
                auto params = ::Ogre::GpuProgramManager::getSingleton().createSharedParameters(invProjParamName);
                params->addConstantDefinition("u_invProj", ::Ogre::GCT_MATRIX_4X4);
            }

            fpParams->addSharedParameters(projParamName);
            fpParams->addSharedParameters(invProjParamName);
            fpParams->setNamedAutoConstant("u_worldView", ::Ogre::GpuProgramParameters::ACT_WORLDVIEW_MATRIX);
            fpParams->setNamedAutoConstant(
                "u_invWorldView",
                ::Ogre::GpuProgramParameters::ACT_INVERSE_WORLDVIEW_MATRIX
            );

            ::Ogre::TextureUnitState* texUnitState = pass->getTextureUnitState("entryPoints");

            SIGHT_ASSERT("No texture named 'entryPoints' in " + _originalMaterial->getName(), texUnitState);
            texUnitState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);

            const auto compName = "VolumeEntries" + std::to_string(m_viewpointNumber);
            texUnitState->setCompositorReference(compName, compName + "Texture" + passIdStr);
        }

        m_createdTechniques.push_back(std::make_pair(newTech, _originalMaterial->getName()));
    }

    return newTech;
}

//------------------------------------------------------------------------------

} // namespace listener

} // namespace compositor

} // namespace sight::viz::scene3d
