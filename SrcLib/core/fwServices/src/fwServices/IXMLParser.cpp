/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IXMLParser.hpp"
#include "fwServices/op/New.hpp"



namespace fwServices
{

IXMLParser::IXMLParser( )
{}

IXMLParser::~IXMLParser()
{}

void IXMLParser::starting() throw(::fwTools::Failed)
{}

void IXMLParser::stopping() throw(::fwTools::Failed)
{}

void IXMLParser::configuring() throw(::fwTools::Failed)
{}

void IXMLParser::updating( fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{}

void IXMLParser::updating() throw(::fwTools::Failed)
{
	for( ::fwRuntime::ConfigurationElement::Iterator configEltIter = this->m_configuration->begin() ; !(configEltIter == this->m_configuration->end()) ; ++configEltIter)
	{
		if( (*configEltIter)->getName() == "object" )
		{
			fwTools::Object::sptr localObj = fwServices::New( (*configEltIter) ) ;
			assert( localObj ) ;
			assert( (*configEltIter)->hasAttribute("id") ) ;
			std::string _id = (*configEltIter)->getExistingAttributeValue("id") ;
			assert( !this->getObject()->getField(_id) );
			this->getObject()->setFieldSingleElement(_id,localObj);
		}
	}
}

}

