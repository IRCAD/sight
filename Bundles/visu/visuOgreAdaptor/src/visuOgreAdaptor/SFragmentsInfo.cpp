/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
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

#include "visuOgreAdaptor/SFragmentsInfo.hpp"

#include "fwRenderOgre/helper/Technique.hpp"

#include <fwCom/Signals.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreMaterialManager.h>

#include <memory>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SFragmentsInfo)

struct FragmentsInfoMaterialListener : public ::Ogre::MaterialManager::Listener
{

    virtual ~FragmentsInfoMaterialListener()
    {
    }

    //------------------------------------------------------------------------------

    virtual ::Ogre::Technique* handleSchemeNotFound(unsigned short, const ::Ogre::String& _schemeName,
                                                    ::Ogre::Material* _originalMaterial, unsigned short,
                                                    const ::Ogre::Renderable*)
    {
        ::Ogre::Technique* newTech = nullptr;

        if(_schemeName == "PrimitiveID_MS")
        {
            ::Ogre::Technique* defaultTech = _originalMaterial->getTechnique(0);
            newTech                        = ::fwRenderOgre::helper::Technique::copyToMaterial(defaultTech, _schemeName,
                                                                                               _originalMaterial);

            const ::Ogre::Technique::Passes& passes = newTech->getPasses();
            for(const auto pass : passes)
            {
                pass->setCullingMode(::Ogre::CULL_NONE);
                pass->setManualCullingMode(::Ogre::MANUAL_CULL_NONE);
                pass->setFragmentProgram("PrimitiveID_FP");
            }
        }

        return newTech;
    }

};

static const ::fwServices::IService::KeyType s_IMAGE_INOUT        = "image";
static const ::fwServices::IService::KeyType s_DEPTH_INOUT        = "depth";
static const ::fwServices::IService::KeyType s_PRIMITIVE_ID_INOUT = "primitiveID";

static const ::fwCom::Signals::SignalKeyType s_RESIZE_RENDER_TARGET_SLOT = "resizeRenderTarget";

static std::unique_ptr< FragmentsInfoMaterialListener > s_MATERIAL_LISTENER = nullptr;

//-----------------------------------------------------------------------------

SFragmentsInfo::SFragmentsInfo() noexcept
{
    newSlot(s_RESIZE_RENDER_TARGET_SLOT, &SFragmentsInfo::resizeRenderTarget, this);
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
    m_width  = config.get<int>("width", m_width);
    m_height = config.get<int>("height", m_height);

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

    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_UPDATED_SIG,
                              this->getSptr(), s_UPDATE_SLOT);
    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_RANGE_UPDATED_SIG,
                              this->getSptr(), s_UPDATE_SLOT);

    if(!s_MATERIAL_LISTENER)
    {
        s_MATERIAL_LISTENER = std::make_unique< FragmentsInfoMaterialListener >();
        ::Ogre::MaterialManager::getSingleton().addListener(s_MATERIAL_LISTENER.get());
    }

    // Fixed size.
    if(m_fixedSize)
    {

        this->createCompositor(m_width, m_height);
    }
    // If not listen to the resize event of the layer.
    else
    {

        const auto h = this->getLayer()->getViewport()->getActualHeight();
        const auto w = this->getLayer()->getViewport()->getActualWidth();

        this->createCompositor(w, h);

        m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                                  this->getSptr(), s_RESIZE_RENDER_TARGET_SLOT);
    }
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::updating() noexcept
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    if(image)
    {
        const ::Ogre::TexturePtr text = m_compositor->getTextureInstance(m_targetName, 0);
        {
            ::fwData::mt::ObjectWriteLock lock(image);
            ::fwRenderOgre::Utils::convertFromOgreTexture(text, image, false);
        }

        auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
    }

    const ::fwData::Image::sptr depth = this->getInOut< ::fwData::Image >(s_DEPTH_INOUT);

    if(depth)
    {
        const ::Ogre::TexturePtr depthText = m_compositor->getTextureInstance(m_targetName, 1);
        {
            ::fwData::mt::ObjectWriteLock lock(depth);
            ::fwRenderOgre::Utils::convertFromOgreTexture(depthText, depth, false);
        }

        auto depthSig = depth->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        depthSig->asyncEmit();
    }

    const ::fwData::Image::sptr primitiveID = this->getInOut< ::fwData::Image>(s_PRIMITIVE_ID_INOUT);

    if(primitiveID)
    {
        const ::Ogre::TexturePtr primitiveIDText = m_compositor->getTextureInstance(m_targetPrimitiveIDName, 0);
        {
            ::fwData::mt::ObjectWriteLock lock(primitiveID);
            ::fwRenderOgre::Utils::convertFromOgreTexture(primitiveIDText, primitiveID, false);
        }

        auto primitiveIDSig = primitiveID->signal< ::fwData::Object::ModifiedSignalType >(
            ::fwData::Object::s_MODIFIED_SIG);
        primitiveIDSig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::stopping()
{
    this->destroyCompositor();
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

    bool retrieveDepth = this->getInOut< ::fwData::Image>(s_DEPTH_INOUT) != nullptr;

    bool retrievePrimitiveID = this->getInOut< ::fwData::Image>(s_PRIMITIVE_ID_INOUT) != nullptr;

    ::Ogre::CompositorManager& cmpMngr = ::Ogre::CompositorManager::getSingleton();

    m_compositor = cmpMngr.create(m_compositorName,
                                  ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    ::Ogre::CompositionTechnique* const technique = m_compositor->createTechnique();

    ::Ogre::CompositionTechnique::TextureDefinition* globalTarget;
    globalTarget        = technique->createTextureDefinition(m_targetName);
    globalTarget->scope = ::Ogre::CompositionTechnique::TextureScope::TS_GLOBAL;
    globalTarget->formatList.push_back(::Ogre::PixelFormat::PF_A8B8G8R8);
    globalTarget->height = _height;
    globalTarget->width  = _width;

    if(!retrieveDepth)
    {
        ::Ogre::CompositionTargetPass* const globalTargetPass = technique->createTargetPass();
        {
            globalTargetPass->setOutputName(m_targetName);
            globalTargetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
        }
    }
    else
    {
        globalTarget->formatList.push_back(::Ogre::PixelFormat::PF_FLOAT32_R);

        const std::string localName("local_RTT");
        ::Ogre::CompositionTechnique::TextureDefinition* localTarget;
        localTarget        = technique->createTextureDefinition(localName);
        localTarget->scope = ::Ogre::CompositionTechnique::TextureScope::TS_LOCAL;
        localTarget->formatList.push_back(::Ogre::PixelFormat::PF_DEPTH32);
        localTarget->height = _height;
        localTarget->width  = _width;

        ::Ogre::CompositionTargetPass* const localTargetPass = technique->createTargetPass();
        {
            localTargetPass->setOutputName(localName);
            localTargetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
        }

        ::Ogre::CompositionTargetPass* const globalTargetPass = technique->createTargetPass();
        {
            globalTargetPass->setOutputName(m_targetName);
            globalTargetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
            ::Ogre::CompositionPass* const targetOutputCompPass = globalTargetPass->createPass(
                ::Ogre::CompositionPass::PT_RENDERQUAD);
            targetOutputCompPass->setMaterialName("ForwardDepth_M");
            targetOutputCompPass->setInput(0, localName);
        }
    }

    if(retrievePrimitiveID)
    {
        std::cout << "generate primitive ID" << std::endl;
        ::Ogre::CompositionTechnique::TextureDefinition* globalTargetPrimitiveID;
        globalTargetPrimitiveID        = technique->createTextureDefinition(m_targetPrimitiveIDName);
        globalTargetPrimitiveID->scope = ::Ogre::CompositionTechnique::TextureScope::TS_GLOBAL;
        globalTargetPrimitiveID->formatList.push_back(::Ogre::PixelFormat::PF_R32_SINT);
        globalTargetPrimitiveID->height = _height;
        globalTargetPrimitiveID->width  = _width;

        ::Ogre::CompositionTargetPass* const globalPrimitiveTargetPass = technique->createTargetPass();
        {
            globalPrimitiveTargetPass->setOutputName(m_targetPrimitiveIDName);
            globalPrimitiveTargetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_NONE);
            globalPrimitiveTargetPass->setMaterialScheme("PrimitiveID_MS");
            globalPrimitiveTargetPass->createPass(::Ogre::CompositionPass::PT_CLEAR);
            globalPrimitiveTargetPass->createPass(::Ogre::CompositionPass::PT_RENDERSCENE);
        }
    }

    ::Ogre::CompositionTargetPass* const targetOutputPass = technique->getOutputTargetPass();
    {
        ::Ogre::CompositionPass* const targetOutputCompPass = targetOutputPass->createPass(
            ::Ogre::CompositionPass::PT_RENDERQUAD);
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
    ::Ogre::CompositorManager& cmpMngr = ::Ogre::CompositorManager::getSingleton();

    const auto layer = this->getLayer();
    layer->updateCompositorState(m_compositorName, false);
    cmpMngr.removeCompositor(layer->getViewport(), m_compositorName);
    cmpMngr.remove(m_compositor);
    m_compositor.reset();
}

//-----------------------------------------------------------------------------

void SFragmentsInfo::resizeRenderTarget(int _width, int _height)
{
    this->destroyCompositor();
    this->createCompositor(_width, _height);
}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor
