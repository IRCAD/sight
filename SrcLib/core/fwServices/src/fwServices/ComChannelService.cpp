/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <boost/regex.hpp>

#include <fwCore/base.hpp>

#include <fwTools/UUID.hpp>

#include "fwServices/ComChannelService.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/helper.hpp"
#include "fwServices/op/Get.hpp"
#include "fwServices/IService.hpp"
#include "fwServices/macros.hpp"
#include "fwServices/monitor/Monitor.hpp"
#include "fwServices/ObjectMsg.hpp"

REGISTER_SERVICE( ::fwServices::ICommunication , ::fwServices::ComChannelService , ::fwTools::Object ) ;

namespace fwServices
{

ComChannelService::ComChannelService() : m_destUUID( std::pair< bool , std::string >(false , "") )
{
}

//------------------------------------------------------------------------------

ComChannelService::~ComChannelService()
{
}

//------------------------------------------------------------------------------

bool ComChannelService::isValid()
{
	return (!m_destination.expired() && !m_source.expired()  ) ;
}

//------------------------------------------------------------------------------

void ComChannelService::setSrc(IEditionService::sptr  _src)
{
	m_source = _src;
}

//------------------------------------------------------------------------------

void ComChannelService::configuring() throw( ::fwTools::Failed )
{
	SLM_ASSERT("Deprecated tag source", !m_configuration->findConfigurationElement("source"));

	if( m_configuration->findConfigurationElement("target") )
	{
		m_destUUID.first = true ;
		m_destUUID.second = m_configuration->findConfigurationElement("target")->getValue() ;
		OSLM_INFO("Destination = " << m_destUUID.second ) ;
	}
}

//------------------------------------------------------------------------------

void ComChannelService::starting() throw(fwTools::Failed)
{
	if( m_destination.expired() )
	{
		assert( m_destUUID.first );
		assert( ::fwTools::UUID::exist( m_destUUID.second , ::fwTools::UUID::SIMPLE) );
		m_destination = ::fwTools::UUID::get< ::fwServices::IService >( m_destUUID.second ) ;
		OSLM_DEBUG("Destination = " << m_destUUID.second << " found") ;
	}

	SLM_ASSERT("intern data mismatch", m_destination.lock()->getUUID() == m_destUUID.second);

	if( m_source.expired() )
	{
		m_source = ::fwServices::get< ::fwServices::IEditionService >( this->getObject() ) ;
		assert( !m_source.expired() ) ;
		OSLM_DEBUG("Source (IEditionService) = " << this->getObject()->getUUID() << " found") ;
	}


	OSLM_ASSERT("there are similar observations, dest= " <<
			m_destination.lock()->getUUID() <<
			" src= " << this->getObject()->getUUID(),
			!this->hasSimilarObservation());

	// Assertion
	assert( !m_destination.expired() ) ;
	assert( !m_source.expired() ) ;


	if( !m_source.lock()->isAttached( this->getSptr() ) )
	{
		std::stringstream msg ;
		this->info( msg ) ;
		SLM_TRACE( "Starting ComChannelService : " + msg.str() );
		m_source.lock()->attach( this->getSptr() );
		m_source.lock()->start() ;
	}

	// Post condition
	assert( !m_source.expired() ) ;
	assert( !m_destination.expired() ) ;
}

//------------------------------------------------------------------------------

void ComChannelService::stopping() throw(fwTools::Failed)
{
	// Pre condition
	if( !m_source.expired() )
	{
		if( m_source.lock()->isAttached( this->getSptr() ) )
		{
			std::stringstream msg ;
			this->info( msg ) ;
			SLM_TRACE( "Stopping ComChannelService " + msg.str() ); // crash from spylog???
			m_source.lock()->detach( this->getSptr() );
		}
	}
}

//------------------------------------------------------------------------------

void ComChannelService::info(std::ostream &_sstream )
{
	// Status
	std::string status ;
	if( !m_source.expired() )
	{
		if( m_source.lock()->isAttached( this->getSptr() ) )
		{
			status = "ON" ;
		}
		else
		{
			status = "OFF" ;
		}
	}

	// Update _sstream
	if(!m_source.expired() )
	{
		if( ::fwServices::OSR::hasObject(m_source.lock().get()) )
		{
			::fwTools::Object::sptr observedObject = m_source.lock()->getObject() ;
			_sstream << "ComChannelService (" << status << ") "<< " : SRC = " << observedObject.get() << " (" << observedObject->className() << ")";
		}
		else
		{
			_sstream << "ComChannelService (" << status << ") "<< " SRC not specified" ;
		}
	}
	if( !m_destination.expired() )
	{
		_sstream << " - DEST = " << m_destination.lock().get() << " (" << (m_destination.lock())->getClassname() << ")";
	}

}

//------------------------------------------------------------------------------

void ComChannelService::setDest(IService::sptr _client)
{
	m_destination = _client ;
	m_destUUID.first = true ;
	m_destUUID.second = _client->getUUID() ;
}

//------------------------------------------------------------------------------

IService::sptr  ComChannelService::getDest()
{
	assert( !m_destination.expired() ) ;
	return m_destination.lock();
}

//------------------------------------------------------------------------------

IEditionService::sptr ComChannelService::getSrc()
{
	assert( !m_source.expired() ) ;
	return m_source.lock();
}

//------------------------------------------------------------------------------

void ComChannelService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(fwTools::Failed)
{
	SLM_FATAL("method depreciated");
}

//------------------------------------------------------------------------------

bool ComChannelService::hasSimilarObservation()
{
	bool res = false;
	typedef std::vector< ::fwServices::ComChannelService::sptr > OContainerType;
	OContainerType obs = ::fwServices::OSR::getServices< ::fwServices::ComChannelService >() ;

	for( OContainerType::iterator iter = obs.begin() ; !res && iter != obs.end() ; ++iter )
	{
		if( (*iter).get() != this && (*iter)->isValid() )
		{
			res = ( (*iter)->getDest() == m_destination.lock() &&  (*iter)->getSrc() == m_source.lock() );
		}
	}
	return res;
}

//------------------------------------------------------------------------------

std::string ComChannelService::convertToLightString( std::string _initialString )
{
	std::string lightString = _initialString;

	::boost::regex toolsRegex ("::fwTools::(.*)");
	::boost::regex dataRegex ("::fwData::(.*)");
	const std::string machine_format("\\1");

	if ( ::boost::regex_match( _initialString, toolsRegex ) )
	{
		lightString = ::boost::regex_replace( _initialString, toolsRegex, machine_format, boost::match_default | boost::format_sed );
	}
	else if ( ::boost::regex_match( _initialString, dataRegex ) )
	{
		lightString = ::boost::regex_replace( _initialString, dataRegex, machine_format, boost::match_default | boost::format_sed );
	}

	return lightString;
}

//------------------------------------------------------------------------------

std::string ComChannelService::getNotificationInformation( ::fwServices::ObjectMsg::csptr _msg )
{
	std::string sourceUUID = convertToLightString(_msg->getSubject().lock()->getUUID() );
	std::string destUUID = convertToLightString( m_destination.lock()->getUUID() );
	std::string objectUUID = convertToLightString( _msg->getUUID() );

	std::stringstream sstr;
	sstr << "MSG ( " << objectUUID << " ) : " << sourceUUID << " ===> " << destUUID;
	return sstr.str();
}

//------------------------------------------------------------------------------

void ComChannelService::sendMessage(::fwServices::ObjectMsg::csptr _msg)
{
	if( m_destination.lock()->isStarted())
	{
		if(  !m_destination.lock()->isSending() )
		{
			SLM_INFO( getNotificationInformation(_msg) );
			m_destination.lock()->update(_msg ) ;
		}
		else
		{
			//OSLM_ASSERT("LOOP DETECTION : "<< getNotificationInformation// _msg->getSource().lock()->getClassname(), m_destination.lock() ==  _msg->getSource().lock());
			OSLM_ASSERT("LOOP DETECTION : "<< getNotificationInformation(_msg), m_destination.lock() ==  _msg->getSource().lock());
		}
	}
}


}
