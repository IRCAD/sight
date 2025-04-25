/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "module/viz/scene3d/adaptor/compositor_parameter.hpp"

#include <core/com/slots.hxx>

#include <viz/scene3d/compositor/chain_manager.hpp>

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

    CompositorListener(Ogre::Viewport* /*unused*/, compositor_parameter::sptr _adaptor) :
        m_adaptor(_adaptor)
    {
    }

    ~CompositorListener() override
    = default;

    //------------------------------------------------------------------------------

    void notifyMaterialSetup(Ogre::uint32 /*pass_id*/, Ogre::MaterialPtr& _mat) override
    {
        auto adaptor = m_adaptor.lock();
        SIGHT_ASSERT("Adaptor has expired.", adaptor);
        adaptor->update_value(_mat);
    }

    //------------------------------------------------------------------------------

    void notifyResourcesCreated(bool /*forResizeOnly*/) override
    {
        auto adaptor = m_adaptor.lock();
        SIGHT_ASSERT("Adaptor has expired.", adaptor);
        adaptor->lazy_update();
    }

    //------------------------------------------------------------------------------

    void notifyResourcesReleased(bool /*forResizeOnly*/) override
    {
    }

private:

    /// Associated Sight adaptor
    module::viz::scene3d::adaptor::compositor_parameter::wptr m_adaptor;
};

static const core::com::slots::key_t ADD_LISTENER_SLOT = "addListener";

//-----------------------------------------------------------------------------

compositor_parameter::compositor_parameter() noexcept
{
    new_slot(ADD_LISTENER_SLOT, &compositor_parameter::add_listener, this);
}

//------------------------------------------------------------------------------

void compositor_parameter::configuring()
{
    this->parameter_adaptor::configuring();

    const config_t config = this->get_config();

    static const std::string s_COMPOSITOR_NAME_CONFIG = CONFIG + "compositorName";
    m_compositor_name = config.get<std::string>(s_COMPOSITOR_NAME_CONFIG);
    SIGHT_ERROR_IF("'" + s_COMPOSITOR_NAME_CONFIG + "' attribute not set", m_compositor_name.empty());
}

//------------------------------------------------------------------------------

void compositor_parameter::starting()
{
    adaptor::init();

    sight::viz::scene3d::layer::sptr layer = this->layer();

    if(!visible())
    {
        this->slot(slots::UPDATE_VISIBILITY)->async_run(visible());
    }

    this->add_listener();

    m_resize_connection.connect(
        layer,
        sight::viz::scene3d::layer::RESIZE_LAYER_SIG,
        this->get_sptr(),
        ADD_LISTENER_SLOT
    );
}

//------------------------------------------------------------------------------

void compositor_parameter::stopping()
{
    m_resize_connection.disconnect();

    this->render_service()->make_current();

    this->parameter_adaptor::stopping();

    sight::viz::scene3d::layer::sptr layer = this->layer();

    Ogre::CompositorChain* comp_chain =
        Ogre::CompositorManager::getSingleton().getCompositorChain(layer->get_viewport());

    auto* compositor = comp_chain->getCompositor(m_compositor_name);

    // Association of a listener attached to this adaptor to the configured compositor
    compositor->removeListener(m_listener);
    delete m_listener;
    m_listener = nullptr;

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void compositor_parameter::set_visible(bool _enable)
{
    this->render_service()->make_current();

    const auto layer                  = this->layer();
    Ogre::CompositorChain* comp_chain =
        Ogre::CompositorManager::getSingleton().getCompositorChain(layer->get_viewport());

    auto* compositor = comp_chain->getCompositor(m_compositor_name);
    SIGHT_ASSERT("The given compositor '" + m_compositor_name + "' doesn't exist in the compositor chain", compositor);

    if(!_enable && m_listener != nullptr)
    {
        compositor->removeListener(m_listener);
        delete m_listener;
        m_listener = nullptr;
    }

    layer->update_compositor_state(m_compositor_name, _enable);

    if(_enable)
    {
        // Association of a listener attached to this adaptor to the configured compositor
        m_listener =
            new CompositorListener(
                layer->get_viewport(),
                std::dynamic_pointer_cast<compositor_parameter>(this->get_sptr())
            );
        compositor->addListener(m_listener);
    }
}

//------------------------------------------------------------------------------

void compositor_parameter::update_value(Ogre::MaterialPtr& _mat)
{
    this->set_material(_mat);
    this->parameter_adaptor::updating();
}

//------------------------------------------------------------------------------

void compositor_parameter::add_listener()
{
    this->render_service()->make_current();

    sight::viz::scene3d::layer::sptr layer = this->layer();

    Ogre::CompositorChain* comp_chain =
        Ogre::CompositorManager::getSingleton().getCompositorChain(layer->get_viewport());

    auto* compositor = comp_chain->getCompositor(m_compositor_name);
    SIGHT_ASSERT("The given compositor '" + m_compositor_name + "' doesn't exist in the compositor chain", compositor);

    if(m_listener != nullptr)
    {
        compositor->removeListener(m_listener);
        delete m_listener;
        m_listener = nullptr;
    }

    // Association of a listener attached to this adaptor to the configured compositor
    m_listener =
        new CompositorListener(
            layer->get_viewport(),
            std::dynamic_pointer_cast<compositor_parameter>(this->get_sptr())
        );
    compositor->addListener(m_listener);
}

//------------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
