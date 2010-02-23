/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/wx.h>

#include <fwTools/UUID.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwData/Object.hpp>

#include <fwWX/IGuiContainer.hpp>

#include "gui/aspect/DefaultAspect.hpp"

#include "gui/Manager.hpp"


namespace gui
{
namespace aspect
{

REGISTER_SERVICE( ::gui::aspect::IAspect , ::gui::aspect::DefaultAspect , ::fwTools::Object ) ;


DefaultAspect::DefaultAspect() throw(): m_uid(""), m_autostart(false)
{}

//-----------------------------------------------------------------------------

DefaultAspect::~DefaultAspect() throw()
{}

//-----------------------------------------------------------------------------

void DefaultAspect::configuring() throw( ::fwTools::Failed )
{
    this->::gui::aspect::IAspect::configuring();
    if(m_configuration->findConfigurationElement("view"))
    {
        ::fwRuntime::ConfigurationElement::sptr viewCfgElt = m_configuration->findConfigurationElement("view");
        SLM_ASSERT("Sorry, \"uid\" attribute is missing.", viewCfgElt->hasAttribute("uid") );
        m_uid = viewCfgElt->getExistingAttributeValue("uid") ;
        if(viewCfgElt->hasAttribute("autoStart"))
        {
            std::string autostart = viewCfgElt->getExistingAttributeValue("autoStart");
            OSLM_ASSERT("Sorry, value "<<autostart<<" is not correct for attribute autoStart.",
                    autostart == "yes" || autostart == "no");
            m_autostart = (autostart == "yes");
        }
    }
    else
    {
        SLM_WARN("No main view specified.");
    }
}

//-----------------------------------------------------------------------------

void DefaultAspect::starting() throw(::fwTools::Failed)
{
    this->registerAspect();
    if(!m_uid.empty())
    {
        ::fwWX::IGuiContainer::registerGlobalWxContainer(m_uid, wxTheApp->GetTopWindow());
        if(m_autostart)
        {
            wxTheApp->GetTopWindow()->Update();
            OSLM_ASSERT("Service "<<m_uid<<" doesn't exist.", ::fwTools::UUID::exist(m_uid, ::fwTools::UUID::SIMPLE ));
            ::fwServices::IService::sptr service = ::fwServices::get( m_uid ) ;
            service->start();
        }
    }
}

//-----------------------------------------------------------------------------

void DefaultAspect::stopping() throw(::fwTools::Failed)
{
    if(!m_uid.empty())
    {
        ::fwWX::IGuiContainer::unregisterGlobalWxContainer(m_uid);
        OSLM_INFO_IF("Service "<<m_uid<<" doesn't exist.", !::fwTools::UUID::exist(m_uid, ::fwTools::UUID::SIMPLE ));
        if (::fwTools::UUID::exist(m_uid, ::fwTools::UUID::SIMPLE ))
        {
            ::fwServices::IService::sptr service = ::fwServices::get( m_uid ) ;
            service->stop();
        }
        m_uid = "";
    }
    // To update name
    this->unregisterAspect();

}

//-----------------------------------------------------------------------------

void DefaultAspect::info(std::ostream &_sstream )
{
    _sstream << "Manage aspect in main GUI application" ;
}

//-----------------------------------------------------------------------------

void DefaultAspect::updating( ::fwServices::ObjectMsg::csptr _msg ) throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultAspect::updating() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

}
}

