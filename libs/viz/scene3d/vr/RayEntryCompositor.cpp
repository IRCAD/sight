/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "viz/scene3d/vr/RayEntryCompositor.hpp"

#include "viz/scene3d/compositor/Core.hpp"
#include "viz/scene3d/ogre.hpp"

#include <OgreCompositionPass.h>
#include <OgreCompositionTargetPass.h>
#include <OgreCompositionTechnique.h>
#include <OgreCompositorManager.h>

#include <mutex>

namespace sight::viz::scene3d
{

namespace vr
{

// Mutex to avoid concurrent compositor manager calls.
static std::mutex s_compositorManagerLock;

//------------------------------------------------------------------------------

RayEntryCompositor::RayEntryCompositor(
    const std::string& _compositorName,
    std::uint8_t _rqGroup,
    compositor::Core::StereoModeType _stereoMode,
    bool _enableMixedRendering
) :
    m_compositorName(_compositorName)
{
    auto& cm = ::Ogre::CompositorManager::getSingleton();
    std::lock_guard<std::mutex> guard(s_compositorManagerLock);

    m_compositor = cm.getByName(m_compositorName, RESOURCE_GROUP);

    if(!m_compositor)
    {
        m_compositor = cm.create(m_compositorName, viz::scene3d::RESOURCE_GROUP);

        auto* compTech = m_compositor->createTechnique();

        std::uint8_t nbViewpoints = 1;
        float heightFactor        = 1.f;
        float widthFactor         = 1.F;

        switch(_stereoMode)
        {
            case compositor::Core::StereoModeType::NONE:
                break;

            case compositor::Core::StereoModeType::STEREO:
                nbViewpoints = 2;
                heightFactor = 0.5f;
                break;

            case compositor::Core::StereoModeType::AUTOSTEREO_5:
                nbViewpoints = 5;
                heightFactor = 0.5f;
                widthFactor  = 0.6f;
                break;

            case compositor::Core::StereoModeType::AUTOSTEREO_8:
                nbViewpoints = 8;
                heightFactor = 0.5f;
                widthFactor  = 0.375f;
                break;
        }

        const auto schemePrefix = std::string("VolumeEntries") + (nbViewpoints > 1 ? "AutoStereo" : "");

        for(std::uint8_t i = 0 ; i < nbViewpoints ; ++i)
        {
            const auto schemeSuffix  = nbViewpoints > 1 ? std::to_string(i) : "";
            const auto texTargetName = m_compositorName + "Texture" + schemeSuffix;

            auto* texDef = compTech->createTextureDefinition(texTargetName);
            texDef->scope = ::Ogre::CompositionTechnique::TextureScope::TS_CHAIN;
            texDef->formatList.push_back(::Ogre::PixelFormat::PF_FLOAT32_GR);
            texDef->heightFactor = heightFactor;
            texDef->widthFactor  = widthFactor;

            auto* backFacesTargetPass = compTech->createTargetPass();
            backFacesTargetPass->setInputMode(::Ogre::CompositionTargetPass::InputMode::IM_NONE);
            backFacesTargetPass->setOutputName(texTargetName);

            auto* clearPass = backFacesTargetPass->createPass();
            clearPass->setType(::Ogre::CompositionPass::PT_CLEAR);
            clearPass->setClearBuffers(::Ogre::FBT_COLOUR | ::Ogre::FBT_DEPTH);
            clearPass->setClearColour(::Ogre::ColourValue(0.f, 1.f, 1.f, 1.f));

            auto* backFacesPass = backFacesTargetPass->createPass();
            backFacesPass->setMaterialScheme(schemePrefix + "_BackFaces" + schemeSuffix);
            backFacesPass->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
            backFacesPass->setFirstRenderQueue(_rqGroup);
            backFacesPass->setLastRenderQueue(_rqGroup);

            auto* frontFacesTargetPass = compTech->createTargetPass();
            frontFacesTargetPass->setInputMode(::Ogre::CompositionTargetPass::InputMode::IM_NONE);
            frontFacesTargetPass->setOutputName(texTargetName);

            auto* frontFacesPass = frontFacesTargetPass->createPass();
            frontFacesPass->setMaterialScheme(schemePrefix + "_FrontFaces" + schemeSuffix);
            frontFacesPass->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
            frontFacesPass->setFirstRenderQueue(_rqGroup);
            frontFacesPass->setLastRenderQueue(_rqGroup);

            auto* backFacesMaxTargetPass = compTech->createTargetPass();
            backFacesMaxTargetPass->setInputMode(::Ogre::CompositionTargetPass::InputMode::IM_NONE);
            backFacesMaxTargetPass->setOutputName(texTargetName);

            auto* backFacesMaxPass = backFacesMaxTargetPass->createPass();
            backFacesMaxPass->setMaterialScheme(schemePrefix + "_BackFacesMax" + schemeSuffix);
            backFacesMaxPass->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
            backFacesMaxPass->setFirstRenderQueue(_rqGroup);
            backFacesMaxPass->setLastRenderQueue(_rqGroup);

            if(_enableMixedRendering)
            {
                auto* frontFacesMinTargetPass = compTech->createTargetPass();
                frontFacesMinTargetPass->setInputMode(::Ogre::CompositionTargetPass::InputMode::IM_NONE);
                frontFacesMinTargetPass->setOutputName(texTargetName);

                auto* frontFacesMinPass = frontFacesMinTargetPass->createPass();
                frontFacesMinPass->setMaterialScheme(schemePrefix + "_FrontFacesMin" + schemeSuffix);
                frontFacesMinPass->setType(::Ogre::CompositionPass::PT_RENDERSCENE);
                frontFacesMinPass->setLastRenderQueue(compositor::Core::s_SURFACE_RQ_GROUP_ID);
            }
        }

        auto* outputTargetPass = compTech->getOutputTargetPass();
        outputTargetPass->setInputMode(::Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
    }
}

//------------------------------------------------------------------------------

RayEntryCompositor::~RayEntryCompositor()
{
    auto& cm = ::Ogre::CompositorManager::getSingleton();

    std::lock_guard<std::mutex> guard(s_compositorManagerLock);
    // If this is the last reference. (Plus the one kept by the manager)
    if(m_compositor.use_count() == 2)
    {
        cm.remove(m_compositor);
    }
}

//------------------------------------------------------------------------------

const std::string& RayEntryCompositor::getName() const
{
    return m_compositorName;
}

//------------------------------------------------------------------------------

} // namespace vr

} // namespace sight::viz::scene3d
