/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "modules/viz/scene3d/adaptor/SCompositorParameter.hpp"

#include "modules/viz/scene3d/adaptor/SMaterial.hpp"

#include <core/com/slots.hxx>

#include <viz/scene3d/compositor/ChainManager.hpp>

#include <OgreCompositor.h>
#include <OgreCompositorChain.h>
#include <OgreCompositorInstance.h>
#include <OgreCompositorManager.h>
#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

#include <algorithm>

namespace sight::module::viz::scene3d::adaptor
{

class CompositorListener : public Ogre::CompositorInstance::Listener
{
public:

    CompositorListener(Ogre::Viewport* /*unused*/, SCompositorParameter::sptr _adaptor) :
        m_adaptor(_adaptor)
    {
    }

    ~CompositorListener() override
    = default;

    //------------------------------------------------------------------------------

    void notifyMaterialSetup(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& mat) override
    {
        auto adaptor = m_adaptor.lock();
        SIGHT_ASSERT("Adaptor has expired.", adaptor);
        adaptor->updateValue(mat);
    }

    //------------------------------------------------------------------------------

    void notifyResourcesCreated(bool /*forResizeOnly*/) override
    {
        auto adaptor = m_adaptor.lock();
        SIGHT_ASSERT("Adaptor has expired.", adaptor);
        adaptor->setDirty();
    }

    //------------------------------------------------------------------------------

    void notifyResourcesReleased(bool /*forResizeOnly*/) override
    {
    }

private:

    /// Associated Sight adaptor
    module::viz::scene3d::adaptor::SCompositorParameter::wptr m_adaptor;
};

static const core::com::slots::key_t ADD_LISTENER_SLOT = "addListener";

//-----------------------------------------------------------------------------

SCompositorParameter::SCompositorParameter() noexcept
{
    new_slot(ADD_LISTENER_SLOT, &SCompositorParameter::addListener, this);
}

//------------------------------------------------------------------------------

void SCompositorParameter::configuring()
{
    this->IParameter::configuring();

    const ConfigType config = this->getConfiguration();

    static const std::string s_COMPOSITOR_NAME_CONFIG = s_CONFIG + "compositorName";
    m_compositorName = config.get<std::string>(s_COMPOSITOR_NAME_CONFIG);
    SIGHT_ERROR_IF("'" + s_COMPOSITOR_NAME_CONFIG + "' attribute not set", m_compositorName.empty());
}

//------------------------------------------------------------------------------

void SCompositorParameter::starting()
{
    this->initialize();

    sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    if(!m_isVisible)
    {
        this->slot(UPDATE_VISIBILITY_SLOT)->async_run(m_isVisible);
    }

    this->addListener();

    m_resizeConnection.connect(
        layer,
        sight::viz::scene3d::Layer::RESIZE_LAYER_SIG,
        this->get_sptr(),
        ADD_LISTENER_SLOT
    );
}

//------------------------------------------------------------------------------

void SCompositorParameter::updating()
{
    // This is typically called when the data has changed through autoconnect
    // So set the parameter as dirty and perform the update
    this->setDirty();
    this->IParameter::updating();
}

//------------------------------------------------------------------------------

void SCompositorParameter::stopping()
{
    m_resizeConnection.disconnect();

    this->getRenderService()->makeCurrent();

    this->IParameter::stopping();

    sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    Ogre::CompositorChain* compChain =
        Ogre::CompositorManager::getSingleton().getCompositorChain(layer->getViewport());

    auto* compositor = compChain->getCompositor(m_compositorName);

    // Association of a listener attached to this adaptor to the configured compositor
    compositor->removeListener(m_listener);
    delete m_listener;
    m_listener = nullptr;
}

//-----------------------------------------------------------------------------

void SCompositorParameter::setVisible(bool _enable)
{
    this->getRenderService()->makeCurrent();

    const auto layer                 = this->getLayer();
    Ogre::CompositorChain* compChain =
        Ogre::CompositorManager::getSingleton().getCompositorChain(layer->getViewport());

    auto* compositor = compChain->getCompositor(m_compositorName);
    SIGHT_ASSERT("The given compositor '" + m_compositorName + "' doesn't exist in the compositor chain", compositor);

    if(!_enable && m_listener != nullptr)
    {
        compositor->removeListener(m_listener);
        delete m_listener;
        m_listener = nullptr;
    }

    layer->updateCompositorState(m_compositorName, _enable);

    if(_enable)
    {
        // Association of a listener attached to this adaptor to the configured compositor
        m_listener =
            new CompositorListener(
                layer->getViewport(),
                std::dynamic_pointer_cast<SCompositorParameter>(this->get_sptr())
            );
        compositor->addListener(m_listener);
    }
}

//------------------------------------------------------------------------------

void SCompositorParameter::updateValue(Ogre::MaterialPtr& _mat)
{
    this->setMaterial(_mat);
    this->IParameter::updating();
}

//------------------------------------------------------------------------------

void SCompositorParameter::addListener()
{
    this->getRenderService()->makeCurrent();

    sight::viz::scene3d::Layer::sptr layer = this->getLayer();

    Ogre::CompositorChain* compChain =
        Ogre::CompositorManager::getSingleton().getCompositorChain(layer->getViewport());

    auto* compositor = compChain->getCompositor(m_compositorName);
    SIGHT_ASSERT("The given compositor '" + m_compositorName + "' doesn't exist in the compositor chain", compositor);

    if(m_listener != nullptr)
    {
        compositor->removeListener(m_listener);
        delete m_listener;
        m_listener = nullptr;
    }

    // Association of a listener attached to this adaptor to the configured compositor
    m_listener =
        new CompositorListener(layer->getViewport(), std::dynamic_pointer_cast<SCompositorParameter>(this->get_sptr()));
    compositor->addListener(m_listener);
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
