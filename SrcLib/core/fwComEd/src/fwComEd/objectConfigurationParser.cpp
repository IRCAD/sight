/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwData/ProcessObject.hpp>
#include <fwData/Composite.hpp>

#include "fwComEd/objectConfigurationParser.hpp"

REGISTER_SERVICE( ::fwServices::IXMLParser , ::fwComEd::ICompositeParser , ::fwData::Composite );
REGISTER_SERVICE( ::fwServices::IXMLParser , ::fwComEd::IProcessObjectParser ,  ::fwData::ProcessObject );

namespace fwComEd
{
void ICompositeParser::updating( ) throw(fwTools::Failed)
{
	::boost::shared_ptr< ::fwRuntime::ConfigurationElement >_cfgElement = this->m_configuration;
	::boost::shared_ptr< ::fwData::Composite > dataComposite = this->getObject< ::fwData::Composite >() ;
	assert( dataComposite ) ;

	for( ::fwRuntime::ConfigurationElement::Iterator configEltIter = _cfgElement->begin() ; !(configEltIter == _cfgElement->end()) ; ++configEltIter)
	{
		if( (*configEltIter)->getName() == "object" )
		{
			// Configuring the input/output
			::boost::shared_ptr< fwTools::Object > localObj = fwServices::New( (*configEltIter) ) ;
			assert( localObj ) ;
			// Inserting initialized object in processObject
			assert( (*configEltIter)->hasAttribute("id") ) ;
			std::string _id = (*configEltIter)->getExistingAttributeValue("id") ;
			assert( ::boost::dynamic_pointer_cast< ::fwData::Object >(localObj ));
			assert( dataComposite->getRefMap().find(_id) == dataComposite->getRefMap().end() ) ;
			dataComposite->getRefMap()[_id] = ::boost::dynamic_pointer_cast< ::fwData::Object >(localObj ) ;
		}
	}
}


void IProcessObjectParser::updating( ) throw(fwTools::Failed)
{
	 ::boost::shared_ptr< ::fwRuntime::ConfigurationElement >_cfgElement = this->m_configuration ;
	 ::boost::shared_ptr< ::fwData::ProcessObject > dataProcessObject = this->getObject< ::fwData::ProcessObject >() ;
//	 ::boost::shared_ptr< ::fwData::ProcessObject > dataProcessObject = ::boost::dynamic_pointer_cast< ::fwData::ProcessObject>(_obj) ;
	assert( dataProcessObject ) ;

	///// INPUTS
	const ::boost::shared_ptr< ::fwRuntime::ConfigurationElement> inputParameterList = _cfgElement->findConfigurationElement("inputs");
	if( inputParameterList )
	{
		assert( inputParameterList ) ;
		this->configureIO(dataProcessObject->getInputs() , inputParameterList ) ;
	}
	///// OUTPUTS
	const ::boost::shared_ptr< ::fwRuntime::ConfigurationElement> outputParameterList = _cfgElement->findConfigurationElement("outputs");
	if( outputParameterList )
	{
	assert( outputParameterList ) ;
	this->configureIO(dataProcessObject->getOutputs() , outputParameterList ) ;
	}
}

void IProcessObjectParser::configureIO( ::boost::shared_ptr< fwTools::Object > io, const ::boost::shared_ptr< ::fwRuntime::ConfigurationElement> list)
{
	for( ::fwRuntime::ConfigurationElement::Iterator configEltIter = list->begin() ; !(configEltIter == list->end()) ; ++configEltIter)
	{
		std::cout << "ENTERING NAME " << (*configEltIter)->getName() << std::endl;
		/// Configuring the input/output
		 ::boost::shared_ptr< fwTools::Object > localObj = fwServices::New( (*configEltIter) ) ;
		assert( localObj ) ;
		// Inserting initialized object in processObject
		assert( (*configEltIter)->hasAttribute("id") ) ;
		std::string _id = (*configEltIter)->getExistingAttributeValue("id") ;
//		assert( ::boost::dynamic_pointer_cast< ::fwData::Object >(localObj ));
		assert( ::boost::dynamic_pointer_cast< ::fwTools::Field >(io ));
		if ( ::boost::dynamic_pointer_cast< ::fwTools::Field >(io )->getField( _id ).get() == NULL )
		{
			::boost::dynamic_pointer_cast< ::fwTools::Field >(io )->addFieldElement( _id, localObj ) ;
		}
	}
}


}
