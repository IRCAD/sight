/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/app.h>
#include <wx/wx.h>
#include <wx/bitmap.h>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/bundle/runtime.hpp>
#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include "gui/aspect/IToolBar.hpp"
#include "gui/aspect/IAspect.hpp"
#include "gui/action/IAction.hpp"


namespace gui
{
namespace aspect
{

IToolBar::IToolBar() throw() : m_toolBar(0)
{
}

//-----------------------------------------------------------------------------

IToolBar::~IToolBar() throw()
{
}

//-----------------------------------------------------------------------------

void IToolBar::configuring() throw( ::fwTools::Failed )
{
    SLM_INFO("IToolBar::configuring") ;
}

//-----------------------------------------------------------------------------

void IToolBar::starting() throw( ::fwTools::Failed )
{
    SLM_INFO("IToolBar::starting") ;

    //get frame
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    SLM_ASSERT( "No wxFrame", frame ) ;

    // If no menu bar yet : create it
    if( !frame->GetToolBar() )
    {
        SLM_TRACE("No tool bar : creating it") ;

        // Create the toolbar
        frame->CreateToolBar(wxNO_BORDER|wxHORIZONTAL|wxTB_FLAT, -1);
        frame->GetToolBar()->SetMargins( 2, 2 );
        frame->GetToolBar()->SetToolBitmapSize( wxSize(32, 32) );
        m_toolBar = frame->GetToolBar();
    }

    // Reconfiguring and starting appropriate actions
    ::fwRuntime::ConfigurationElementContainer::Iterator iter ;
    std::string name;
    for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
    {
        if( (*iter)->getName() == "toolBitmapSize" )
        {
            int height = -1;
            int width = -1;
            if((*iter)->hasAttribute("height"))
            {
                height = boost::lexical_cast< int > ((*iter)->getExistingAttributeValue("height"));
            }
            if((*iter)->hasAttribute("width"))
            {
                width = boost::lexical_cast< int > ((*iter)->getExistingAttributeValue("width"));
            }
            m_toolBar->SetToolBitmapSize( wxSize(width, height) );
        }

        if( (*iter)->getName() == "action" )
        {
            SLM_ASSERT("id tag depreciated", !(*iter)->hasAttribute("id"));
            SLM_ASSERT("uid attribute missing", (*iter)->hasAttribute("uid"));

            std::string uid =  (*iter)->getExistingAttributeValue("uid") ;
            ::gui::action::IAction::sptr action = ::gui::action::IAction::dynamicCast( ::fwServices::get(uid) );
            SLM_ASSERT("action missing", action);
            int id = action->getId();

            name = "";
            if((*iter)->hasAttribute("name"))
            {
                name = (*iter)->getExistingAttributeValue("name");
            }

            wxImage image = wxNullImage ;
            if((*iter)->hasAttribute("icon"))
            {
                std::string icon = (*iter)->getExistingAttributeValue("icon");
                if ( !image.LoadFile(wxConvertMB2WX( icon.c_str())))
                {
                    wxLogError(_("Couldn't load image from '%s'."), wxConvertMB2WX( icon.c_str()));
                }
            }
            wxItemKind kind = wxITEM_NORMAL;
            if((*iter)->hasAttribute("style"))
            {
                if((*iter)->getExistingAttributeValue("style") == "check")
                {
                    kind = wxITEM_CHECK;
                }
                else if((*iter)->getExistingAttributeValue("style") == "radio")
                {
                    kind = wxITEM_RADIO;
                }
            }
            m_toolBar->AddTool(id, wxConvertMB2WX(name.c_str()), wxBitmap(image),  wxBitmap(image.ConvertToGreyscale()), kind, wxConvertMB2WX( name.c_str()));
            if((*iter)->hasAttribute("state"))
            {
                if((*iter)->getExistingAttributeValue("state") == "checked" && (kind == wxITEM_CHECK || kind == wxITEM_RADIO))
                {
                    m_toolBar->ToggleTool(id, true);
                }
            }
            m_toolBar->EnableTool(id, action->isEnable());
        }
        if( (*iter)->getName() == "separator" )
        {
            m_toolBar->AddSeparator();
        }
    }
    m_toolBar->Realize();
}

//-----------------------------------------------------------------------------

void IToolBar::stopping() throw( ::fwTools::Failed )
{
    SLM_INFO("IToolBar::stopping") ;
    if( m_toolBar != 0 )
    {
        m_toolBar->ClearTools();
        delete m_toolBar;
        m_toolBar = 0;
    }
}
//-----------------------------------------------------------------------------

void IToolBar::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("IToolBar::updating");
}

//-----------------------------------------------------------------------------

void IToolBar::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_FATAL("an IToolBar service does not received a message." );
}

//-----------------------------------------------------------------------------

}
}
