/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderOgre/compositor/MaterialMgrListener.hpp"

#include "fwRenderOgre/helper/Shading.hpp"

#include <fwCore/spyLog.hpp>

#include <OGRE/OgreHighLevelGpuProgram.h>
#include <OGRE/OgreHighLevelGpuProgramManager.h>
#include <OGRE/OgreTechnique.h>

namespace fwRenderOgre
{

namespace compositor
{

// ----------------------------------------------------------------------------

MaterialMgrListener::~MaterialMgrListener()
{
}

// ----------------------------------------------------------------------------

::Ogre::Technique* fwRenderOgre::compositor::MaterialMgrListener::handleSchemeNotFound(unsigned short _schemeIndex,
                                                                                       const ::Ogre::String &_schemeName,
                                                                                       ::Ogre::Material *_originalMaterial,
                                                                                       unsigned short _lodIndex,
                                                                                       const ::Ogre::Renderable *
                                                                                       _renderable)
{
    // Don't know what is this for exactly, but we don't care so we filter this out
    if(_schemeName == "ShaderGeneratorDefaultScheme")
    {
        return nullptr;
    }

    auto tokens = ::Ogre::StringUtil::split(_schemeName, "/");
    // DepthPeeling, DualDepthPeeling, WeightedBlended or HybridTransparency
    const auto algoName = tokens[0];
    // peel, depthMap, transmittanceBlend, etc...
    const auto algoPassName = tokens[1];

    ::Ogre::Technique* newTech     = nullptr;
    ::Ogre::Technique* defaultTech = _originalMaterial->getTechnique(0);
    ::Ogre::Technique* depthTech   = _originalMaterial->getTechnique("depth");
    if(depthTech == nullptr)
    {
        SLM_ERROR( "Missing 'depth' technique for material '" + _originalMaterial->getName() +
                   "' Order Independent Transparency will probably not be supported.");
        depthTech = defaultTech;
    }

    // The R2VB material does not need to fill the OIT schemes, though Ogre get us here to know what to do
    // We simply return the main technique in this case
    ::Ogre::Technique::PassIterator passIt = defaultTech->getPassIterator();
    while ( passIt.hasMoreElements() )
    {
        ::Ogre::Pass* pass = passIt.getNext();

        if(  ::Ogre::StringUtil::startsWith(pass->getGeometryProgramName(), "R2VB/" ) )
        {
            return defaultTech;
        }
    }

    if(_schemeName == "DepthPeeling/depthMap" ||
       _schemeName == "HybridTransparency/backDepth")
    {
        newTech = this->copyTechnique(depthTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_REPLACE);
            pass->setFragmentProgram("DepthPeeling/depthMap_FP");

            if(algoName == "HybridTransparency")
            {
                pass->setDepthFunction(::Ogre::CMPF_GREATER_EQUAL);
            }
        }
    }
    else if( ::Ogre::StringUtil::startsWith(_schemeName, "DepthPeeling/peel", false) ||
             ::Ogre::StringUtil::startsWith(_schemeName, "CelShadingDepthPeeling/peel", false) ||
             ::Ogre::StringUtil::startsWith(_schemeName, "HybridTransparency/peel", false) )
    {
        newTech = this->copyTechnique(defaultTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            pass->setDepthCheckEnabled(true);
            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_REPLACE);

            if(algoName == "CelShadingDepthPeeling" && pass->getName() != "NormalsPass" &&
               pass->getName() != "EdgePass")
            {
                auto vpName  = pass->getVertexProgramName();
                auto newName = ::fwRenderOgre::helper::Shading::setTechniqueInProgramName(vpName, algoName);
                pass->setVertexProgram(newName);
            }

            // replace fragment program and build it if needed
            auto fpName  = pass->getFragmentProgramName();
            auto newName = ::fwRenderOgre::helper::Shading::setTechniqueInProgramName(fpName, algoName + "/peel");
            this->ensureFPCreated(newName, algoName, algoPassName, fpName);
            pass->setFragmentProgram(newName);

            auto numTexUnit = pass->getNumTextureUnitStates();
            ::Ogre::TextureUnitState* texState = pass->createTextureUnitState();
            texState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
            texState->setTextureFiltering(::Ogre::TFO_NONE);
            texState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);

            // Modify texture input according to the requested pass
            if(algoPassName == "peelInit")
            {
                texState->setCompositorReference(algoName, "peelInit", 0);
            }
            else if(algoPassName == "peelPing")
            {
                texState->setCompositorReference(algoName, "pongBuffer", 1);
            }
            else if(algoPassName == "peelPong")
            {
                texState->setCompositorReference(algoName, "pingBuffer", 1);
            }
            else
            {
                OSLM_FATAL("DepthPeeling logical error");
            }

            auto params = pass->getFragmentProgramParameters();
            params->setNamedConstant("u_nearestDepthBuffer", numTexUnit);
            params->setNamedAutoConstant("u_vpWidth", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_WIDTH);
            params->setNamedAutoConstant("u_vpHeight", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_HEIGHT);
            params->setNamedAutoConstant("u_diffuse", ::Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
        }
    }
    else if(_schemeName == "WeightedBlended/occlusionMap" ||
            _schemeName == "HybridTransparency/occlusionMap")
    {
        newTech = this->copyTechnique(depthTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_REPLACE);

            pass->setFragmentProgram(algoName + "/occlusionMap_FP");
        }
    }
    else if(_schemeName == "WeightedBlended/weightBlend" ||
            _schemeName == "HybridTransparency/weightBlend")
    {
        newTech = this->copyTechnique(defaultTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            // replace fragment program and build it if needed
            auto fpName  = pass->getFragmentProgramName();
            auto newName = ::fwRenderOgre::helper::Shading::setTechniqueInProgramName(fpName, algoName+ "/weightBlend");
            this->ensureFPCreated(newName, algoName, algoPassName, fpName);
            pass->setFragmentProgram(newName);

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_ADD);

            auto numTexUnit = pass->getNumTextureUnitStates();
            auto params     = pass->getFragmentProgramParameters();

            if(algoName == "HybridTransparency")
            {
                ::Ogre::TextureUnitState* texState = pass->createTextureUnitState();
                texState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
                texState->setTextureFiltering(::Ogre::TFO_NONE);
                texState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                texState->setCompositorReference(algoName, "pingBuffer", 1);

                params->setNamedConstant("u_frontDepthBuffer", numTexUnit++);
            }
            ::Ogre::TextureUnitState* texState = pass->createTextureUnitState();
            texState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
            texState->setTextureFiltering(::Ogre::TFO_NONE);
            texState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
            texState->setCompositorReference(algoName, "occlusion", 0);

            params->setNamedConstant("u_occlusionDepthBuffer", numTexUnit);
            params->setNamedAutoConstant("u_vpWidth", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_WIDTH);
            params->setNamedAutoConstant("u_vpHeight", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_HEIGHT);
            params->setNamedAutoConstant("u_near", ::Ogre::GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE);
            params->setNamedAutoConstant("u_far", ::Ogre::GpuProgramParameters::ACT_FAR_CLIP_DISTANCE);
        }
    }
    else if(_schemeName == "WeightedBlended/transmittanceBlend"||
            _schemeName == "HybridTransparency/transmittanceBlend")
    {
        newTech = this->copyTechnique(depthTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            pass->setFragmentProgram(algoName + "/transmittanceBlend_FP");
            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBF_ZERO, ::Ogre::SBF_ONE_MINUS_SOURCE_COLOUR);

            if(algoName == "HybridTransparency")
            {
                ::Ogre::TextureUnitState* texState = pass->createTextureUnitState();
                texState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
                texState->setTextureFiltering(::Ogre::TFO_NONE);
                texState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                texState->setCompositorReference(algoName, "pingBuffer", 1);
            }
            ::Ogre::TextureUnitState* texState = pass->createTextureUnitState();
            texState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
            texState->setTextureFiltering(::Ogre::TFO_NONE);
            texState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
            texState->setCompositorReference(algoName, "occlusion", 0);
        }
    }
    else if( ::Ogre::StringUtil::startsWith(_schemeName, "DualDepthPeeling/peelInit", false) )
    {
        newTech = this->copyTechnique(depthTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_ADD);
            pass->setSceneBlendingOperation(::Ogre::SBO_MAX);

            pass->setFragmentProgram("DualDepthPeeling/peelInit_FP");
        }

    }
    else if( ::Ogre::StringUtil::startsWith(_schemeName, "DualDepthPeeling/peel", false) )
    {
        newTech = this->copyTechnique(defaultTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_ADD);
            pass->setSceneBlendingOperation(::Ogre::SBO_MAX);

            // replace fragment program and build it if needed
            auto fpName  = pass->getFragmentProgramName();
            auto newName = ::fwRenderOgre::helper::Shading::setTechniqueInProgramName(fpName, algoName + "/peel");
            this->ensureFPCreated(newName, algoName, algoPassName, fpName);
            pass->setFragmentProgram(newName);

            std::string inputBuffer;
            if(algoPassName == "peelPing")
            {
                inputBuffer = "pongBuffer";
            }
            else if(algoPassName == "peelPong")
            {
                inputBuffer = "pingBuffer";
            }
            else
            {
                OSLM_FATAL("DualDepthPeeling logical error");
            }

            auto numTexUnit = pass->getNumTextureUnitStates();

            // Modify texture input according to the requested pass
            for(size_t i = 0; i < 4; ++i)
            {
                ::Ogre::TextureUnitState* texState = pass->createTextureUnitState();
                texState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
                texState->setTextureFiltering(::Ogre::TFO_NONE);
                texState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                texState->setCompositorReference(algoName, inputBuffer, i == 3 ? 4 : i);
            }

            auto params = pass->getFragmentProgramParameters();
            params->setNamedConstant("u_nearestDepthBuffer", numTexUnit);
            params->setNamedConstant("u_farthestDepthBuffer", numTexUnit + 1);
            params->setNamedConstant("u_forwardColorBuffer", numTexUnit + 2);
            params->setNamedConstant("u_forwardAlphasBuffer", numTexUnit + 3);
            params->setNamedAutoConstant("u_vpWidth", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_WIDTH);
            params->setNamedAutoConstant("u_vpHeight", ::Ogre::GpuProgramParameters::ACT_VIEWPORT_HEIGHT);
            params->setNamedAutoConstant("u_diffuse", ::Ogre::GpuProgramParameters::ACT_SURFACE_DIFFUSE_COLOUR);
        }
    }
    else
    {
        OSLM_ERROR("not found : " << _schemeName );
    }

    return newTech;
}

// ----------------------------------------------------------------------------

Ogre::Technique* MaterialMgrListener::copyTechnique(::Ogre::Technique* _tech,
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

// ----------------------------------------------------------------------------

::Ogre::GpuProgramPtr MaterialMgrListener::ensureFPCreated(const std::string& _name,
                                                           const std::string& _algoName,
                                                           const std::string& _algoPassName,
                                                           const std::string& _baseName)
{
    // Determine shader source file and parameters
    std::string sourceFileName;
    ::fwRenderOgre::helper::Shading::GpuProgramParametersType parameters;

    // Set specific shader according to the algo and the pass
    if(_algoName == "DepthPeeling")
    {
        sourceFileName = "DepthPeelingPeel_FP.glsl";
        parameters.push_back(std::make_pair<std::string, std::string>("attach", "DepthPeelingCommon_FP"));
    }
    else if(_algoName == "DualDepthPeeling")
    {
        sourceFileName = "DualDepthPeelingPeel_FP.glsl";
        parameters.push_back(std::make_pair<std::string, std::string>("attach", "DepthPeelingCommon_FP"));
    }
    else if(_algoName == "HybridTransparency")
    {
        if(_algoPassName == "peel" || _algoPassName == "peelInit")
        {
            sourceFileName = "DepthPeelingPeel_FP.glsl";
            parameters.push_back(std::make_pair<std::string, std::string>("attach", "DepthPeelingCommon_FP"));
        }
        else
        {
            sourceFileName = "WeightedBlended_Weight_Blend_FP.glsl";
            parameters.push_back(std::make_pair<std::string, std::string>("attach", "DepthPeelingCommon_FP"));
            parameters.push_back(std::make_pair<std::string, std::string>("preprocessor_defines", "HYBRID=1"));
        }
    }
    else if(_algoName == "WeightedBlended")
    {
        sourceFileName = "WeightedBlended_Weight_Blend_FP.glsl";
    }
    else if(_algoName == "CelShadingDepthPeeling")
    {
        sourceFileName = "CelShadingDepthPeelingPeel_FP.glsl";
        parameters.push_back(std::make_pair<std::string, std::string>("attach", "DepthPeelingCommon_FP"));
    }
    else
    {
        OSLM_FATAL("Unreachable code");
    }

    return ::fwRenderOgre::helper::Shading::createProgramFrom(_name, sourceFileName, parameters,
                                                              ::Ogre::GPT_FRAGMENT_PROGRAM, _baseName);
}

// ----------------------------------------------------------------------------

} // namespace compositor

} // namespace fwRenderOgre
