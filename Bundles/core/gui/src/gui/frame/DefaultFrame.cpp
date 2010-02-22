/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/assign/list_of.hpp>

#include <wx/wx.h>

#include <fwTools/UUID.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>

#include <fwData/Object.hpp>

#include <fwWX/IGuiContainer.hpp>
#include <fwWX/convert.hpp>

#include "gui/Manager.hpp"
#include "gui/frame/DefaultFrame.hpp"

namespace gui
{
namespace frame
{

REGISTER_SERVICE( ::gui::frame::IFrame , ::gui::frame::DefaultFrame , ::fwTools::Object ) ;


const std::map<std::string, long> DefaultFrame::FWSTYLE_TO_WXSTYLE =
                                                ::boost::assign::map_list_of("DEFAULT",wxDEFAULT_FRAME_STYLE)
                                                                            ("STAY_ON_TOP",wxSTAY_ON_TOP)
                                                                            ("FRAME_FLOAT_ON_PARENT",wxFRAME_FLOAT_ON_PARENT);

DefaultFrame::DefaultFrame() throw() : m_container(0),
        m_name(""),
        m_minSizeHeight(-1),
        m_minSizeWidth(-1),
        m_modeStyle(wxDEFAULT_FRAME_STYLE)
{}

//-----------------------------------------------------------------------------

DefaultFrame::~DefaultFrame() throw()
{}

//-----------------------------------------------------------------------------

void DefaultFrame::configuring() throw( ::fwTools::Failed )
{
    assert( m_configuration->getName() == "service" );

    if(m_configuration->findConfigurationElement("name"))
    {
        ::fwRuntime::ConfigurationElement::sptr nameCfgElt = m_configuration->findConfigurationElement("name");
        m_name = nameCfgElt->getValue();
    }
    if(m_configuration->findConfigurationElement("icon"))
    {
        ::fwRuntime::ConfigurationElement::sptr iconCfgElt = m_configuration->findConfigurationElement("icon");
        m_iconPath = ::boost::filesystem::path( iconCfgElt->getValue() ) ;
        OSLM_ASSERT("Sorry, icon "<< m_iconPath << " doesn't exist", ::boost::filesystem::exists(m_iconPath));
    }
    if( m_configuration->findConfigurationElement("minSize") )
    {
        ::fwRuntime::ConfigurationElement::sptr minSizeCfgElt = m_configuration->findConfigurationElement("minSize");

        if(minSizeCfgElt->hasAttribute("width"))
        {
            m_minSizeWidth = ::boost::lexical_cast<int >(minSizeCfgElt->getExistingAttributeValue("width")) ;
        }
        if(minSizeCfgElt->hasAttribute("height"))
        {
            m_minSizeHeight = ::boost::lexical_cast<int >(minSizeCfgElt->getExistingAttributeValue("height")) ;
        }
    }

    if(m_configuration->findConfigurationElement("styles"))
    {
        ::fwRuntime::ConfigurationElement::sptr stylesCfgElt = m_configuration->findConfigurationElement("styles");
        std::vector < ::fwRuntime::ConfigurationElement::sptr > vectStylesConfig = stylesCfgElt->find("style");
        assert(!vectStylesConfig.empty());
        ::fwRuntime::ConfigurationElementContainer::Iterator iter;
        for (iter = vectStylesConfig.begin() ; iter != vectStylesConfig.end() ; ++iter)
        {
            SLM_FATAL_IF("<styles> node can only contain <style> node", (*iter)->getName()!="style" );
            SLM_FATAL_IF("<style> node must contain mode attribute", !(*iter)->hasAttribute("mode") );
            const std::string style = (*iter)->getExistingAttributeValue("mode");
            SLM_ASSERT("Sorry, style "<<style<< " is unknown.",
                    DefaultFrame::FWSTYLE_TO_WXSTYLE.find(style) != DefaultFrame::FWSTYLE_TO_WXSTYLE.end());
            m_modeStyle |= DefaultFrame::FWSTYLE_TO_WXSTYLE.find(style)->second;
        }
    }

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

    m_managedServices.clear();
    if (m_configuration->findConfigurationElement("services"))
    {
        ::fwRuntime::ConfigurationElement::sptr servicesCfgElt = m_configuration->findConfigurationElement("services");

        std::vector < ::fwRuntime::ConfigurationElement::sptr > vectConfig = servicesCfgElt->find("service");
        assert(!vectConfig.empty());

        ::fwRuntime::ConfigurationElementContainer::Iterator iter;
        for (iter = vectConfig.begin() ; iter != vectConfig.end() ; ++iter)
        {
            SLM_FATAL_IF("<services> node can only contain <service> node", (*iter)->getName()!="service" );
            SLM_FATAL_IF("<service> node must contain uid attribute", !(*iter)->hasAttribute("uid") );
            m_managedServices.push_back((*iter)->getExistingAttributeValue("uid"));
        }
    }
}

//-----------------------------------------------------------------------------

void DefaultFrame::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    wxFrame* frame = new wxFrame(wxTheApp->GetTopWindow(),
            wxNewId(),
            ::fwWX::std2wx(m_name),
            wxDefaultPosition,
            wxDefaultSize,
            m_modeStyle
            );
    frame->SetMinSize(wxSize(m_minSizeWidth, m_minSizeHeight));
    m_container = frame;
    if(!m_iconPath.empty())
    {
        wxIcon icon( ::fwWX::std2wx(m_iconPath.native_file_string()), wxBITMAP_TYPE_ICO );
        OSLM_ASSERT("Sorry, unable to create an icon instance from " << m_iconPath.native_file_string(), icon.Ok());
        frame->SetIcon( icon );
    }

    m_container->Bind( wxEVT_CLOSE_WINDOW, &DefaultFrame::onCloseFrame, this,  m_container->GetId());
    ::fwWX::IGuiContainer::registerGlobalWxContainer(m_uid, m_container);

    if(m_autostart)
    {
        wxTheApp->GetTopWindow()->Update();
        OSLM_ASSERT("Service "<<m_uid<<" doesn't exist.", ::fwTools::UUID::exist(m_uid, ::fwTools::UUID::SIMPLE ));
        ::fwServices::IService::sptr service = ::fwServices::get( m_uid ) ;
        service->start();
    }

    m_container->Fit();
    m_container->Show();
    m_container->Refresh();

    BOOST_FOREACH(std::string serviceUID, m_managedServices)
    {
        OSLM_FATAL_IF("Service " << serviceUID << " doesn't exist.", ! ::fwTools::UUID::exist(serviceUID, ::fwTools::UUID::SIMPLE ));
        ::fwServices::IService::sptr service = ::fwServices::get( serviceUID ) ;
        service->start();
    }
}

//-----------------------------------------------------------------------------

void DefaultFrame::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("Sorry, no container!",m_container);
    m_container->Show(false);

    BOOST_REVERSE_FOREACH(std::string serviceUID, m_managedServices)
    {
        OSLM_FATAL_IF("Service " << serviceUID << " doesn't exist.", ! ::fwTools::UUID::exist(serviceUID, ::fwTools::UUID::SIMPLE ));
        ::fwServices::IService::sptr service = ::fwServices::get( serviceUID ) ;
        service->stop();
    }

    if(!m_uid.empty())
    {
        ::fwWX::IGuiContainer::unregisterGlobalWxContainer(m_uid);
        OSLM_ASSERT("Service "<<m_uid<<" doesn't exist.", ::fwTools::UUID::exist(m_uid, ::fwTools::UUID::SIMPLE ));
        ::fwServices::IService::sptr service = ::fwServices::get( m_uid ) ;
        service->stop();
    }

    m_container->Unbind( wxEVT_CLOSE_WINDOW, &DefaultFrame::onCloseFrame, this,  m_container->GetId());

    m_container->Destroy();
}

//-----------------------------------------------------------------------------

void DefaultFrame::info(std::ostream &_sstream )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultFrame::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultFrame::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void DefaultFrame::onCloseFrame(wxCloseEvent& event)
{
    this->stop();
}

//-----------------------------------------------------------------------------

}
}

