/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/window.h>
#include <wx/frame.h>
#include <string>
#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwData/Image.hpp>
#include <fwData/ProcessObject.hpp>
#include <fwServices/helper.hpp>
#include <fwCore/base.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <boost/lexical_cast.hpp>

#include "fwRender/IRender.hpp"

namespace fwRender
{

const std::string IRender::ClockRateId  = "ClockRate";

IRender::IRender() throw() :
    m_container( 0 ),
    m_guiContainerId( std::pair< bool , int >(false,0)),
    m_clockRate( std::pair< bool , int >(false,0)),
    m_timer(),
    m_isContainerLocallyCreated( false )
{
}

//-----------------------------------------------------------------------------

IRender::~IRender() throw()
{
}

//-----------------------------------------------------------------------------

void IRender::info(std::ostream &_sstream )
{
        if( ::fwServices::OSR::hasObject( this ) )
        {
                if( this->getClockRate().first )
                {
                        _sstream << "Refresh rate is " << this->getClockRate().second << " (ms)" ;
                }
                else
                {
                        _sstream << "Clock OFF " ;
                        return ;
                }
        }
}

//-----------------------------------------------------------------------------

void IRender::initClockRate()
{
        // Configuring clock if specified in configuration
        if( m_configuration->hasAttribute("clockRateMSeconds") )
        {
                        std::string clockRate = m_configuration->getExistingAttributeValue("clockRateMSeconds") ;
                        this->setClockRate( ::boost::lexical_cast< int >(clockRate) ) ;
        }
}

//-----------------------------------------------------------------------------

void IRender::setClockRate(int _rate )
{
        m_clockRate = std::pair< bool , int >(true,_rate) ;
}

//-----------------------------------------------------------------------------

std::pair< bool, int > IRender::getClockRate()
{
        return this->m_clockRate;
}

//-----------------------------------------------------------------------------

void IRender::initGuiContainerId()
{
        if( m_configuration->findConfigurationElement("win") )
        {
                SLM_TRACE("IRender:: find win") ;
                std::string id = m_configuration->findConfigurationElement("win")->getExistingAttributeValue("guiContainerId") ;
                m_guiContainerId = std::pair< bool, int >(true, ::boost::lexical_cast< int >(id)) ;
        }
}

//-----------------------------------------------------------------------------

void IRender::initRender()
{
        // Precondition management
        ::fwTools::Object::sptr associatedObject = this->getObject();

        if( m_guiContainerId.first )
        {
                OSLM_TRACE( "Finding gui container from IDENTIFIER " <<  m_guiContainerId.second );
                m_container = wxWindow::FindWindowById( m_guiContainerId.second ) ;
                assert( m_container ) ;
        }

        if( m_container == 0 )
        {
                std::stringstream msg;
                msg << "Tracking service cannot be started for " << associatedObject->className() << " (adr: " << associatedObject.get() << ")" << std::endl;
                msg << "WxContainer not specified : using application top one" << std::endl;
                SLM_WARN( msg.str() );
                m_container = new wxFrame(wxTheApp->GetTopWindow(),-1, wxConvertMB2WX( "IRendereringService" ));
                m_container->Show();
                m_container->Refresh();
                m_isContainerLocallyCreated = true ;
        }

        // Start clock
        if( this->getClockRate().first )
    {
        m_timer = ::fwWX::Timer<>::ptr ( new ::fwWX::Timer<>() ) ;
        m_timer->setFunction( ::boost::bind( &IRender::update, this ) );
        m_timer->setRate( this->getClockRate().second ) ;
        m_timer->start() ;
    }
        //fwServices::registerCommunicationChannel(this->getObject() ,this->getSptr() )->start();

        // Postcondition assertions
        assert( m_container ) ;
}

//-----------------------------------------------------------------------------

void IRender::stopRender()
{
        // Stop clocking
        if( m_timer)
        {
                m_timer->stop();
                m_timer.reset();
        }
        // Do not destroy m_container children either refresh
        // This is delegated to delegate which manage uninstallation of installed subwindows, such as the vtk one.
        if( m_isContainerLocallyCreated && m_container != 0 )
        {
                SLM_DEBUG("Destroying container") ;
                m_container->Destroy();
                m_container = 0 ;
        }
}

//-----------------------------------------------------------------------------

void IRender::setWxContainer( wxWindow * container )
{
        m_container = container ;
}

//-----------------------------------------------------------------------------

wxWindow * IRender::getWxContainer()
{
        assert( m_container ) ;
        return m_container ;
}

}
