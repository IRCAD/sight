/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SCompositorParameter.hpp"

#include "visuOgreAdaptor/defines.hpp"
#include "visuOgreAdaptor/SMaterial.hpp"

#include <fwRenderOgre/compositor/ChainManager.hpp>

#include <fwServices/macros.hpp>

#include <OgreCompositor.h>
#include <OgreCompositorChain.h>
#include <OgreCompositorInstance.h>
#include <OgreCompositorManager.h>
#include <OgreGpuProgramParams.h>
#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgreTechnique.h>

#include <algorithm>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro( ::visuOgreAdaptor::IParameter, ::visuOgreAdaptor::SCompositorParameter, ::fwData::Object);

//------------------------------------------------------------------------------

class CompositorListener : public ::Ogre::CompositorInstance::Listener
{
public:
    CompositorListener(::Ogre::Viewport* _vp, SCompositorParameter::sptr _adaptor) :
        m_adaptor(_adaptor)
    {
    }
    ~CompositorListener()
    {
    }

    //------------------------------------------------------------------------------

    void notifyMaterialRender(::Ogre::uint32 pass_id, ::Ogre::MaterialPtr& mat)
    {
        auto adaptor = m_adaptor.lock();
        SLM_ASSERT("Adaptor has expired.", adaptor);
        adaptor->updateValue(mat);
    }

private:
    /// Associated f4s adaptor
    ::visuOgreAdaptor::SCompositorParameter::wptr m_adaptor;
};

//------------------------------------------------------------------------------

SCompositorParameter::SCompositorParameter() noexcept
{
}

//------------------------------------------------------------------------------

SCompositorParameter::~SCompositorParameter() noexcept
{
}

//------------------------------------------------------------------------------

void SCompositorParameter::updateValue(::Ogre::MaterialPtr& _mat)
{
    this->setMaterial(_mat);
    this->updating();
}

//------------------------------------------------------------------------------

const std::string& SCompositorParameter::getCompositorName() const
{
    return m_compositorName;
}

//------------------------------------------------------------------------------

void SCompositorParameter::configuring()
{
    this->IParameter::configuring();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    m_compositorName = config.get<std::string>("compositorName", "");
    OSLM_ERROR_IF("'compositorName' attribute not set", m_compositorName.empty());
}

//------------------------------------------------------------------------------

void SCompositorParameter::starting()
{
    this->initialize();

    ::fwRenderOgre::Layer::sptr layer = this->getRenderService()->getLayer(m_layerID);

    ::Ogre::CompositorChain* compChain =
        ::Ogre::CompositorManager::getSingleton().getCompositorChain(layer->getViewport());

    m_compositor = compChain->getCompositor(m_compositorName);
    SLM_ASSERT("The given compositor '" + m_compositorName + "' doesn't exist in the compositor chain", m_compositor);

    // Association of a listener attached to this adaptor to the configured compositor
    m_listener = new CompositorListener(layer->getViewport(), SCompositorParameter::dynamicCast(this->getSptr()));
    m_compositor->addListener(m_listener);
}

//------------------------------------------------------------------------------

void SCompositorParameter::stopping()
{
    this->IParameter::stopping();

    // Association of a listener attached to this adaptor to the configured compositor
    m_compositor->removeListener(m_listener);
    delete m_listener;
}

//------------------------------------------------------------------------------

} // namespace visuOgreAdaptor
