/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
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

REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::ICompositeParser, ::fwData::Composite );
REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::IProcessObjectParser, ::fwData::ProcessObject );

namespace fwComEd
{

//------------------------------------------------------------------------------

void ICompositeParser::updating( ) throw(fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr _cfgElement = this->m_configuration;
    ::fwData::Composite::sptr dataComposite = this->getObject< ::fwData::Composite >() ;
    assert( dataComposite ) ;

    for( ::fwRuntime::ConfigurationElement::Iterator configEltIter = _cfgElement->begin() ; !(configEltIter == _cfgElement->end()) ; ++configEltIter)
    {
        if( (*configEltIter)->getName() == "object" )
        {
            // Configuring the input/output
            ::fwTools::Object::sptr localObj = ::fwServices::New( (*configEltIter) ) ;
            assert( localObj ) ;
            // Inserting initialized object in processObject
            SLM_ASSERT("Sorry, attribute id is missing", (*configEltIter)->hasAttribute("id") ) ;
            std::string _id = (*configEltIter)->getExistingAttributeValue("id") ;
            assert( ::fwData::Object::dynamicCast(localObj ) );
            assert( dataComposite->getRefMap().find(_id) == dataComposite->getRefMap().end() ) ;
            dataComposite->getRefMap()[_id] = ::fwData::Object::dynamicCast(localObj ) ;
        }
    }
}

//------------------------------------------------------------------------------

void IProcessObjectParser::updating( ) throw(fwTools::Failed)
{
    ::fwRuntime::ConfigurationElement::sptr _cfgElement = this->m_configuration ;
    ::fwData::ProcessObject::sptr dataProcessObject = this->getObject< ::fwData::ProcessObject >() ;
    assert( dataProcessObject ) ;

    ///// INPUTS
    ::fwRuntime::ConfigurationElement::sptr inputParameterList = _cfgElement->findConfigurationElement("inputs");
    if( inputParameterList )
    {
        assert( inputParameterList ) ;
        this->configureIO(dataProcessObject->getInputs(), inputParameterList ) ;
    }
    ///// OUTPUTS
    ::fwRuntime::ConfigurationElement::sptr outputParameterList = _cfgElement->findConfigurationElement("outputs");
    if( outputParameterList )
    {
        assert( outputParameterList ) ;
        this->configureIO(dataProcessObject->getOutputs(), outputParameterList ) ;
    }
}

//------------------------------------------------------------------------------

void IProcessObjectParser::configureIO( ::fwTools::Object::sptr io, const ::fwRuntime::ConfigurationElement::sptr list)
{
    for( ::fwRuntime::ConfigurationElement::Iterator configEltIter = list->begin() ; !(configEltIter == list->end()) ; ++configEltIter)
    {
        OSLM_TRACE("ENTERING NAME " << (*configEltIter)->getName());
        /// Configuring the input/output
        ::fwTools::Object::sptr localObj = ::fwServices::New( (*configEltIter) ) ;
        assert( localObj ) ;
        // Inserting initialized object in processObject
        SLM_ASSERT("Sorry, attribute id is missing", (*configEltIter)->hasAttribute("id") ) ;
        std::string _id = (*configEltIter)->getExistingAttributeValue("id") ;

        ::fwTools::Field::sptr ioField = ::fwTools::Field::dynamicCast(io);
        assert( ioField);
        if ( ioField->getField( _id ).get() == NULL )
        {
            ioField->addFieldElement( _id, localObj ) ;
        }
    }
}

//------------------------------------------------------------------------------

}
