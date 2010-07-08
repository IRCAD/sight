/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwCore/base.hpp>
#include <fwTools/UUID.hpp>
#include <fwServices/helper.hpp>

#include "fwGui/IFrameSrv.hpp"

namespace fwGui
{

IFrameSrv::IFrameSrv()
{}

//-----------------------------------------------------------------------------

IFrameSrv::~IFrameSrv()
{}

//-----------------------------------------------------------------------------

void IFrameSrv::initialize()
{
    // find LayoutManager configuration
    std::vector < ConfigurationType > vectLayoutMng = m_configuration->find("frame");
    SLM_ASSERT("<frame> xml element must exist", !vectLayoutMng.empty());
    m_frameConfig = vectLayoutMng.at(0);
    this->initializeLayoutManager(m_frameConfig);



    m_viewRegistrar = ::fwGui::registrar::ViewRegistrar::NewSptr(this->getUUID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("viewRegistrar");
    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
        m_viewRegistrar->initialize(m_registrarConfig);
    }
}

//-----------------------------------------------------------------------------

void IFrameSrv::create()
{
    SLM_ASSERT("FrameLayoutManager must be initialized.",m_frameLayoutManager);
    m_frameLayoutManager->createFrame();
    ::fwGui::fwContainer::sptr frame = m_frameLayoutManager->getFrame();
    std::vector< ::fwGui::fwContainer::sptr > subViews;
    subViews.push_back(frame);
    m_viewRegistrar->manage(subViews);
}

//-----------------------------------------------------------------------------

void IFrameSrv::destroy()
{
    SLM_ASSERT("ViewRegistrar must be initialized.",m_viewRegistrar);
    m_viewRegistrar->unmanage();

    SLM_ASSERT("FrameLayoutManager must be initialized.",m_frameLayoutManager);
    m_frameLayoutManager->destroyFrame();
}

//-----------------------------------------------------------------------------

void IFrameSrv::initializeLayoutManager(ConfigurationType frameConfig)
{
    OSLM_ASSERT("Bad configuration name "<<frameConfig->getName()<< ", must be frame",
            frameConfig->getName() == "frame");

    m_frameLayoutManager = ::fwTools::ClassFactoryRegistry::create< ::fwGui::layoutManager::IFrameLayoutManager >( ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY );
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::layoutManager::IFrameLayoutManager::REGISTRY_KEY, m_frameLayoutManager);

    m_frameLayoutManager->initialize(frameConfig);
}

//-----------------------------------------------------------------------------

}
