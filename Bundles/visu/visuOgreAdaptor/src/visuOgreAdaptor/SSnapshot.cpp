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

#include "visuOgreAdaptor/SSnapshot.hpp"

#include <fwCom/HasSlots.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include <OGRE/OgreCompositionTargetPass.h>
#include <OGRE/OgreCompositor.h>
#include <OGRE/OgreCompositorManager.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SSnapshot)

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

static const ::fwCom::Signals::SignalKeyType s_RESIZE_RENDER_TARGET_SLOT = "resizeRenderTarget";

//-----------------------------------------------------------------------------

SSnapshot::SSnapshot() noexcept
{
    newSlot(s_RESIZE_RENDER_TARGET_SLOT, &SSnapshot::resizeRenderTarget, this);
}

//-----------------------------------------------------------------------------

SSnapshot::~SSnapshot() noexcept
{

}

//-----------------------------------------------------------------------------

void SSnapshot::configuring()
{
    // IAdaptor handles the layerID.
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");
    m_width  = config.get<int>("width");
    m_height = config.get<int>("height");

    // If Both width & height are found we fix the size.
    if(m_width != -1 && m_height != -1)
    {
        m_fixedSize = true;
    }
}

//-----------------------------------------------------------------------------

void SSnapshot::starting()
{
    this->initialize();

    m_compositorName = this->getID() + "_Snapshot";
    m_targetName     = this->getID() + "_target";

    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_UPDATED_SIG,
                              this->getSptr(), s_UPDATE_SLOT);
    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_RANGE_UPDATED_SIG,
                              this->getSptr(), s_UPDATE_SLOT);

    // Fixed size.
    if(m_fixedSize)
    {

        this->createCompositor(m_width, m_height);
    }
    // If not listen to the resize event of the layer.
    else
    {
        m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                                  this->getSptr(), s_RESIZE_RENDER_TARGET_SLOT);

        const auto h = this->getLayer()->getViewport()->getActualHeight();
        const auto w = this->getLayer()->getViewport()->getActualWidth();

        this->createCompositor(h, w);
    }

}

//-----------------------------------------------------------------------------

void SSnapshot::updating() noexcept
{
    const ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("inout '" + s_IMAGE_INOUT + "' does not exist.", image);

    const ::Ogre::TexturePtr text = m_compositor->getTextureInstance(m_targetName, 0);

    {
        ::fwData::mt::ObjectWriteLock lock(image);
        ::fwRenderOgre::Utils::convertFromOgreTexture(text, image, false);
    }

    auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SSnapshot::stopping()
{
    this->destroyCompositor();
}

//-----------------------------------------------------------------------------

void SSnapshot::createCompositor(int _width, int _height)
{
    ::Ogre::CompositorManager& cmpMngr = ::Ogre::CompositorManager::getSingleton();

    m_compositor = cmpMngr.create(m_compositorName,
                                  ::Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    ::Ogre::CompositionTechnique* technique = m_compositor->createTechnique();

    m_target = technique->createTextureDefinition(m_targetName);

    m_target->scope = ::Ogre::CompositionTechnique::TextureScope::TS_GLOBAL;
    m_target->formatList.push_back(::Ogre::PixelFormat::PF_A8R8G8B8);

    m_target->height = _height;
    m_target->width  = _width;

    {
        ::Ogre::CompositionTargetPass* targetPass = technique->createTargetPass();
        targetPass->setOutputName(m_targetName);
        targetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_NONE);
        targetPass->createPass(::Ogre::CompositionPass::PassType::PT_CLEAR);
    }

    {
        ::Ogre::CompositionTargetPass* targetPass = technique->createTargetPass();
        targetPass->setOutputName(m_targetName);
        targetPass->setInputMode(Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
    }

    {
        ::Ogre::CompositionTargetPass* targetOutputPass = technique->getOutputTargetPass();
        targetOutputPass->setInputMode(::Ogre::CompositionTargetPass::InputMode::IM_PREVIOUS);
    }

    const auto layer = this->getLayer();
    cmpMngr.addCompositor(layer->getViewport(), m_compositorName);
    cmpMngr.setCompositorEnabled(layer->getViewport(), m_compositorName, true);
}

//-----------------------------------------------------------------------------

void SSnapshot::destroyCompositor()
{
    ::Ogre::CompositorManager& cmpMngr = ::Ogre::CompositorManager::getSingleton();

    const auto layer = this->getLayer();
    cmpMngr.setCompositorEnabled(layer->getViewport(), m_compositorName, false);
    cmpMngr.removeCompositor(layer->getViewport(), m_compositorName);
    cmpMngr.remove(m_compositor);
    m_compositor.reset();
}

//-----------------------------------------------------------------------------

void SSnapshot::resizeRenderTarget(int _width, int _height)
{
    this->destroyCompositor();
    this->createCompositor(_width, _height);

}

//-----------------------------------------------------------------------------

} //namespace visuOgreAdaptor
