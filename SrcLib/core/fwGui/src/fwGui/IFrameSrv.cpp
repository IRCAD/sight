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
    SLM_ASSERT("<frame> tag must have type attribute", frameConfig->hasAttribute("type"));
    std::string frameLayoutManagerClassName = frameConfig->getAttributeValue("type");
    ::fwTools::Object::sptr layout = ::fwTools::Factory::New(frameLayoutManagerClassName);
    OSLM_ASSERT("Unable to create "<< frameLayoutManagerClassName, layout);
    m_frameLayoutManager = ::fwGui::layoutManager::IFrameLayoutManager::dynamicCast(layout);
    OSLM_ASSERT("Unable to cast "<< frameLayoutManagerClassName << " in frame layout manager", m_frameLayoutManager);

    m_frameLayoutManager->initialize(frameConfig);
}

//-----------------------------------------------------------------------------

}
