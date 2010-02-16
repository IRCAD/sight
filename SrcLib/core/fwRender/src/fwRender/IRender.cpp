/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/helper.hpp>


#include "fwRender/IRender.hpp"

namespace fwRender
{

const std::string IRender::ClockRateId  = "ClockRate";

IRender::IRender() throw() :
    m_clockRate( std::pair< bool , int >(false,0)),
    m_timer()
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

void IRender::initRender()
{
    this->initGuiParentContainer();

    // Start clock
    if( this->getClockRate().first )
    {
        m_timer = ::fwWX::Timer<>::ptr ( new ::fwWX::Timer<>() ) ;
        m_timer->setFunction( ::boost::bind( &IRender::update, this ) );
        m_timer->setRate( this->getClockRate().second ) ;
        m_timer->start() ;
    }

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
    this->resetGuiParentContainer();
}

}
