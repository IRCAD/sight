/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SFragmentsInfo.hpp"

#include <core/com/Signals.hpp>
#include <core/com/Slots.hxx>

#include <service/macros.hpp>

#include <viz/scene3d/helper/Technique.hpp>
#include <viz/scene3d/ogre.hpp>

#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreMaterialManager.h>

#include <memory>

namespace sight::module::viz::scene3d::adaptor
{

struct FragmentsInfoMaterialListener final : public Ogre::MaterialManager::Listener
{
    virtual ~FragmentsInfoMaterialListener()
    {
    }

    //------------------------------------------------------------------------------

    virtual Ogre::Technique* handleSchemeNotFound(
        unsigned short,
        const Ogre::String& _schemeName,
        Ogre::Material* _originalMaterial,
        unsigned short,
        const Ogre::Renderable*
)
    {
        Ogre::Technique* newTech = nullptr;

        if(_schemeName == "PrimitiveID_MS")
        {
            Ogre::Technique* defaultTech = _originalMaterial->getTechnique(0);
            newTech = sight::viz::scene3d::helper::Technique::copyToMaterial(
                defaultTech,
                _schemeName,
                _originalMaterial
            );

            const Ogre::Technique::Passes& passes = newTech->getPasses();
            for(const auto pass : passes)
            {
                pass->setCullingMode(Ogre::CULL_NONE);
                pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
                pass->setFragmentProgram("PrimitiveID_FP");
            }
        }

        return newTech;
    }
};

static std::unique_ptr<FragmentsInfoMaterialListener> s_MATERIAL_LISTENER = nullptr;

//-----------------------------------------------------------------------------

SFragmentsInfo::SFragmentsInfo() noexcept
{
}

//-----------------------------------------------------------------------------

SFragmentsInfo::~SFragmentsInfo() noexcept
{
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::configuring()
{
    // IAdaptor handles the layerID.
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");
    m_width     = config.get<int>("width", m_width);
    m_height    = config.get<int>("height", m_height);
    m_flipImage = config.get<bool>("flip", m_flipImage);

    // If Both width & height are found we fix the size.
    if(m_width > 0 && m_height > 0)
    {
        m_fixedSize = true;
    }
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::starting()
{
    this->initialize();

    m_compositorName        = this->getID() + "_Snapshot_C";
    m_targetName            = this->getID() + "_global_RTT";
    m_targetPrimitiveIDName = this->getID() + "_primitiveID_RTT";

    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();
    layer->getRenderService()->getInteractorManager()->getRenderTarget()->addListener(this);

    if(!s_MATERIAL_LISTENER)
    {
        s_MATERIAL_LISTENER = std::make_unique<FragmentsInfoMaterialListener>();
        Ogre::MaterialManager::getSingleton().addListener(s_MATERIAL_LISTENER.get());
    }

    // Fixed size.
    if(m_fixedSize)
    {
        this->createCompositor(m_width, m_height);
    }
    // If not listen to the resize event of the layer.
    else
    {
        Ogre::Viewport* const viewport = layer->getViewport();

        const auto h = viewport->getActualHeight();
        const auto w = viewport->getActualWidth();

        this->createCompositor(w, h);

        // Listen the viewport to catch the resize event.
        viewport->addListener(this);
    }
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::updating() noexcept
{
    {
        const auto image = m_image.lock();
        if(image)
        {
            const Ogre::TexturePtr text = m_compositor->getTextureInstance(m_targetName, 0);
            sight::viz::scene3d::Utils::convertFromOgreTexture(text, image.get_shared(), m_flipImage);

            const auto sig =
                image->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            sig->asyncEmit();
        }
    }

    {
        const auto depth = m_depth.lock();
        if(depth)
        {
            const Ogre::TexturePtr depthText = m_compositor->getTextureInstance(m_targetName, 1);
            sight::viz::scene3d::Utils::convertFromOgreTexture(depthText, depth.get_shared(), m_flipImage);

            const auto depthSig =
                depth->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            depthSig->asyncEmit();
        }
    }

    {
        const auto primitiveID = m_primitive.lock();
        if(primitiveID)
        {
            const Ogre::TexturePtr primitiveIDText = m_compositor->getTextureInstance(m_targetPrimitiveIDName, 0);
            sight::viz::scene3d::Utils::convertFromOgreTexture(primitiveIDText, primitiveID.get_shared(), m_flipImage);

            const auto primitiveIDSig =
                primitiveID->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
            primitiveIDSig->asyncEmit();
        }
    }
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::stopping()
{
    this->destroyCompositor();

    const sight::viz::scene3d::Layer::sptr layer = this->getLayer();
    layer->getRenderService()->getInteractorManager()->getRenderTarget()->removeListener(this);

    // Removes the listener from the viewport.
    if(!m_fixedSize)
    {
        Ogre::Viewport* const viewport = layer->getViewport();
        viewport->removeListener(this);
    }
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::createCompositor(int _width, int _height)
{
    /* Creates the following compositor:
       compositor 'm_compositorName'
       {
            technique
            {
                // The depth attachment only exist if the depth is needed.
                texture 'm_targetName' '_width' '_height' PF_R8G8B8 PF_FLOAT32_R global_scope

                // We use a local depth texture since Ogre doesn't allow global depth textures.
                // It will be copied by ForwardDepth_M.
                texture local_RTT target_width target_height PF_DEPTH32 local_scope

                // This target pass only exist if the depth is needed.
                target local_RTT
                {
                    input previous
                }

                target 'm_targetName'
                {
                    input previous
                    // This pass only exist if the depth is needed.
                    pass render_quad
                    {
                        material ForwardDepth_M
                        input 0 local_RTT
                    }
                }

                target_output
                {
                    input none

                    pass render_quad
                    {
                        material Forward
                        input 0 m_targetName 0
                    }
                }
            }
       }*/

    const bool retrieveDepth       = m_depth.lock().operator bool();
    const bool retrievePrimitiveID = m_primitive.lock().operator bool();

    Ogre::CompositorManager& cmpManager = Ogre::CompositorManager::getSingleton();

    m_compositor = cmpManager.create(
        m_compositorName,
        sight::viz::scene3d::RESOURCE_GROUP
    );

    Ogre::CompositionTechnique* const technique = m_compositor->createTechnique();

    Ogre::CompositionTechnique::TextureDefinition* globalTarget;
    globalTarget        = technique->createTextureDefinition(m_targetName);
    globalTarget->scope = Ogre::CompositionTechnique::TextureScope::TS_GLOBAL;
    globalTarget->formatList.push_back(Ogre::PixelFormat::PF_A8B8G8R8);
    globalTarget->height = static_cast<Ogre::uint32>(_height);
    globalTarget->width  = static_cast<Ogre::uint32>(_width);

    if(!retrieveDepth)
    {
        Ogre::CompositionTargetPass* const globalTargetPass = technique->createTargetPass();
        {
            globalTargetPass->setOutputName(m_targetName);
            globalTargetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
        }
    }
    else
    {
        globalTarget->formatList.push_back(Ogre::PixelFormat::PF_FLOAT32_R);

        const std::string localName("local_RTT");
        Ogre::CompositionTechnique::TextureDefinition* localTarget;
        localTarget        = technique->createTextureDefinition(localName);
        localTarget->scope = Ogre::CompositionTechnique::TextureScope::TS_LOCAL;
        localTarget->formatList.push_back(Ogre::PixelFormat::PF_DEPTH32);
        localTarget->height = static_cast<Ogre::uint32>(_height);
        localTarget->width  = static_cast<Ogre::uint32>(_width);

        Ogre::CompositionTargetPass* const localTargetPass = technique->createTargetPass();
        {
            localTargetPass->setOutputName(localName);
            localTargetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
        }

        Ogre::CompositionTargetPass* const globalTargetPass = technique->createTargetPass();
        {
            globalTargetPass->setOutputName(m_targetName);
            globalTargetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
            Ogre::CompositionPass* const targetOutputCompPass = globalTargetPass->createPass(
                Ogre::CompositionPass::PT_RENDERQUAD
            );
            targetOutputCompPass->setMaterialName("ForwardDepth_M");
            targetOutputCompPass->setInput(0, localName);
        }
    }

    if(retrievePrimitiveID)
    {
        Ogre::CompositionTechnique::TextureDefinition* globalTargetPrimitiveID;
        globalTargetPrimitiveID        = technique->createTextureDefinition(m_targetPrimitiveIDName);
        globalTargetPrimitiveID->scope = Ogre::CompositionTechnique::TextureScope::TS_GLOBAL;
        globalTargetPrimitiveID->formatList.push_back(Ogre::PixelFormat::PF_R32_SINT);
        globalTargetPrimitiveID->height = static_cast<Ogre::uint32>(_height);
        globalTargetPrimitiveID->width  = static_cast<Ogre::uint32>(_width);

        Ogre::CompositionTargetPass* const globalPrimitiveTargetPass = technique->createTargetPass();
        {
            globalPrimitiveTargetPass->setOutputName(m_targetPrimitiveIDName);
            globalPrimitiveTargetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_NONE);
            globalPrimitiveTargetPass->setMaterialScheme("PrimitiveID_MS");
            globalPrimitiveTargetPass->createPass(Ogre::CompositionPass::PT_CLEAR);
            globalPrimitiveTargetPass->createPass(Ogre::CompositionPass::PT_RENDERSCENE);
        }
    }

    Ogre::CompositionTargetPass* const targetOutputPass = technique->getOutputTargetPass();
    {
        Ogre::CompositionPass* const targetOutputCompPass = targetOutputPass->createPass(
            Ogre::CompositionPass::PT_RENDERQUAD
        );
        targetOutputCompPass->setMaterialName("Forward_M");
        targetOutputCompPass->setInput(0, m_targetName, 0);
    }

    const auto layer = this->getLayer();
    layer->addAvailableCompositor(m_compositorName);
    layer->updateCompositorState(m_compositorName, true);
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::destroyCompositor()
{
    Ogre::CompositorManager& cmpManager = Ogre::CompositorManager::getSingleton();

    const auto layer = this->getLayer();
    layer->updateCompositorState(m_compositorName, false);
    cmpManager.removeCompositor(layer->getViewport(), m_compositorName);
    cmpManager.remove(m_compositor);
    m_compositor.reset();
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::viewportDimensionsChanged(Ogre::Viewport* _viewport)
{
    int left, top, width, height;
    _viewport->getActualDimensions(left, top, width, height);

    // Sometimes, the size can be null, we need to avoid resizing since a global texture needs absolute values.
    if(width != 0 && height != 0)
    {
        this->destroyCompositor();
        this->createCompositor(width, height);
    }
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::postRenderTargetUpdate(const Ogre::RenderTargetEvent&)
{
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
