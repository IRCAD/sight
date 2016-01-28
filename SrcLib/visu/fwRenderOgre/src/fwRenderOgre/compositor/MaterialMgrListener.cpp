/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwRenderOgre/compositor/MaterialMgrListener.hpp>

#include <fwCore/spyLog.hpp>

#include <fwRenderOgre/helper/Shading.hpp>

#include <OGRE/OgreTechnique.h>

namespace fwRenderOgre
{

namespace compositor
{

// ----------------------------------------------------------------------------

MaterialMgrListener::~MaterialMgrListener()
{
    OSLM_ERROR("bye bye" );

}

::Ogre::Technique *fwRenderOgre::compositor::MaterialMgrListener::handleSchemeNotFound(unsigned short _schemeIndex,
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
    if(_schemeName == "CelShadingDepthPeeling/depthMap" ||
       _schemeName == "DepthPeeling/depthMap" ||
       _schemeName == "HybridTransparency/backDepth")
    {
        newTech = copyTechnique(depthTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_REPLACE);

            if(algoName == "CelShadingDepthPeeling" )
            {
                auto fpName = pass->getFragmentProgramName();
                pass->setFragmentProgram("CelShadingDepthPeeling/depthMap_FP");
            }
            else
            {
                pass->setFragmentProgram("DepthPeeling/depthMap_FP");
            }

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
        newTech = copyTechnique(defaultTech, _schemeName, _originalMaterial);

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
                pass->setVertexProgram("CelShadingDepthPeeling/PixelLit_VP");
            }
            auto fpName = pass->getFragmentProgramName();
            fpName = ::fwRenderOgre::helper::Shading::replaceProgramPrefix(fpName, algoName + "/peel");
            pass->setFragmentProgram(fpName);

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
        }
    }
    else if(_schemeName == "WeightedBlended/occlusionMap" ||
            _schemeName == "HybridTransparency/occlusionMap")
    {
        newTech = copyTechnique(depthTech, _schemeName, _originalMaterial);

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
        newTech = copyTechnique(defaultTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();
            auto fpName = pass->getFragmentProgramName();
            fpName = ::fwRenderOgre::helper::Shading::replaceProgramPrefix(fpName, algoName + "/weightBlend");
            pass->setFragmentProgram(fpName);

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_ADD);

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
    else if(_schemeName == "WeightedBlended/transmittanceBlend"||
            _schemeName == "HybridTransparency/transmittanceBlend")
    {
        newTech = copyTechnique(depthTech, _schemeName, _originalMaterial);

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
        newTech = copyTechnique(depthTech, _schemeName, _originalMaterial);

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
        newTech = copyTechnique(defaultTech, _schemeName, _originalMaterial);

        ::Ogre::Technique::PassIterator passIt = newTech->getPassIterator();
        while ( passIt.hasMoreElements() )
        {
            ::Ogre::Pass* pass = passIt.getNext();

            pass->setDepthCheckEnabled(false);
            pass->setCullingMode(::Ogre::CULL_NONE);
            pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
            pass->setSceneBlending(::Ogre::SBT_ADD);
            pass->setSceneBlendingOperation(::Ogre::SBO_MAX);

            auto fpName = pass->getFragmentProgramName();
            fpName = ::fwRenderOgre::helper::Shading::replaceProgramPrefix(fpName, algoName + "/peel");
            pass->setFragmentProgram(fpName);

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

            // Modify texture input according to the requested pass
            for(size_t i = 0; i < 4; ++i)
            {
                ::Ogre::TextureUnitState* texState = pass->createTextureUnitState();
                texState->setTextureAddressingMode(::Ogre::TextureUnitState::TAM_CLAMP);
                texState->setTextureFiltering(::Ogre::TFO_NONE);
                texState->setContentType(::Ogre::TextureUnitState::CONTENT_COMPOSITOR);
                texState->setCompositorReference(algoName, inputBuffer, i == 3 ? 4 : i);
            }
        }
    }
    else
    {
        OSLM_ERROR("not found : " << _schemeName );
    }


    return newTech;
}

// ----------------------------------------------------------------------------

Ogre::Technique* MaterialMgrListener::copyTechnique(Ogre::Technique* _tech,
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

} // namespace compositor

} // namespace fwRenderOgre
