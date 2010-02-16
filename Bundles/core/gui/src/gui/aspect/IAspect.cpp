/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <wx/wx.h>

#include <fwRuntime/Runtime.hpp>
#include <fwRuntime/helper.hpp>
#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/Base.hpp>

#include <fwData/Object.hpp>

#include <fwWX/convert.hpp>

#include "gui/aspect/IAspect.hpp"
#include "gui/Manager.hpp"


namespace gui
{

namespace aspect
{

IAspect::IAspect() throw():
    m_name("NO NAME"),
    m_minSizeHeight(-1),
    m_minSizeWidth(-1)
{}

//---------------------------------------------------------------------------

IAspect::~IAspect() throw()
{}

//---------------------------------------------------------------------------

std::string IAspect::getName()
{
    return m_name ;
}

//---------------------------------------------------------------------------

void IAspect::configuring() throw( ::fwTools::Failed )
{
    SLM_TRACE("IAspect::configuring");
    SLM_FATAL_IF( "Depreciated tag \"views\" in configuration", m_configuration->findConfigurationElement("views") );
    SLM_FATAL_IF( "Depreciated tag \"menus\" in configuration", m_configuration->findConfigurationElement("menus") );
    SLM_FATAL_IF( "Depreciated tag \"toolbar\" in configuration", m_configuration->findConfigurationElement("toolbar") );

    assert( m_configuration->getName() == "aspect" || m_configuration->getName() == "service" );

    ::fwRuntime::ConfigurationElementContainer::Iterator iter ;
    for( iter = m_configuration->begin() ; iter != m_configuration->end() ; ++iter )
    {
        SLM_ASSERT( "actions tag is depreciated", (*iter)->getName() != "actions" );

        if( (*iter)->getName() == "name" )
        {
            m_name = (*iter)->getValue();
        }

        if( (*iter)->getName() == "icon" )
        {
            OSLM_TRACE("icon: "<< (*iter)->getValue());
            m_icon = ::boost::shared_ptr< wxIcon >(createIcon((*iter)->getValue()));
        }

        if( (*iter)->getName() == "minSize" )
        {
            //TODO: it should be useful to verify the config is well formed...
            //assert((*iter)->hasAttribute("height") && (*iter)->hasAttribute("width") );
            //m_minSizeHeight = ::boost::lexical_cast<int >((*iter)->getExistingAttributeValue("height").c_str());
            //m_minSizeWidth = ::boost::lexical_cast<int >((*iter)->getExistingAttributeValue("width").c_str());
            if( (*iter)->hasAttribute("height") )
            {
                m_minSizeHeight = ::boost::lexical_cast<int >((*iter)->getExistingAttributeValue("height").c_str());
            }
            if( (*iter)->hasAttribute("width") )
            {
                m_minSizeWidth = ::boost::lexical_cast<int >((*iter)->getExistingAttributeValue("width").c_str());
            }
        }
    }
}

//---------------------------------------------------------------------------

void IAspect::info(std::ostream &_sstream )
{
    _sstream << "Manage aspect in main GUI application" ;
}

//---------------------------------------------------------------------------

::boost::shared_ptr< wxIcon > IAspect::getIcon()
{
    return m_icon;
}

//---------------------------------------------------------------------------

wxSize IAspect::getMinSize()
{
    return wxSize(m_minSizeWidth, m_minSizeHeight);
}

//---------------------------------------------------------------------------

wxIcon* IAspect::createIcon(std::string _imagePath) const throw(::fwTools::Failed)
{
    wxIcon  * result = 0;
    ::boost::filesystem::path imagePath(_imagePath);
    std::string imageFileExtension( ::boost::filesystem::extension( imagePath ) );
    if( imageFileExtension == ".ico" )
    {
        // Creates the new icon directly from the image path.
        result = new wxIcon(
            wxConvertMB2WX( imagePath.native_file_string().c_str() ),
            wxBITMAP_TYPE_ICO );

        // Ensure that the icon is valid.
        if( result->Ok() == false )
        {
            std::string message;
            message += imagePath.string();
            message += ": unbale to create an icon instance from the file.";
            throw ::fwTools::Failed(message);
        }
    }
    return result;
}

//-----------------------------------------------------------------------------

void IAspect::registerAspect( )
{
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    SLM_ASSERT( "No wxFrame", frame ) ;
    frame->SetTitle( ::fwWX::std2wx(this->getName()) ) ;
    if(this->getIcon()!= 0)
    {
        frame->SetIcon( *this->getIcon().get() );
    }
    frame->SetMinSize(this->getMinSize());
    frame->Show(TRUE);
}

//-----------------------------------------------------------------------------

void IAspect::unregisterAspect( )
{
    wxFrame *frame = wxDynamicCast( wxTheApp->GetTopWindow() , wxFrame ) ;
    SLM_ASSERT( "No wxFrame", frame ) ;
    frame->SetTitle("") ;
    wxTheApp->GetTopWindow()->Refresh();
}

}
}

