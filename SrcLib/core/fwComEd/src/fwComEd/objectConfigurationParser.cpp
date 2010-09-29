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

bool ICompositeParser::refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement )
{
    bool isOk = true;

    for(    ::fwRuntime::ConfigurationElement::Iterator configEltIter = _cfgElement->begin() ;
            configEltIter != _cfgElement->end();
            ++configEltIter)
    {
        std::string subElementName = (*configEltIter)->getName();
        if(     subElementName != "service" &&
                subElementName != "serviceList"    )
        {
            OSLM_ERROR("xml subelement \""<< subElementName <<"\" for element object is not supported for the moment when you use a reference on item composite.");
            isOk = false;
        }
    }

    return isOk;
}

//------------------------------------------------------------------------------

void ICompositeParser::updating( ) throw(fwTools::Failed)
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT = "new";
    const std::string GET_OBJECT = "ref";

    ::fwRuntime::ConfigurationElement::sptr _cfgElement = this->m_configuration;
    ::fwData::Composite::sptr dataComposite = this->getObject< ::fwData::Composite >() ;
    assert( dataComposite ) ;

    for( ::fwRuntime::ConfigurationElement::Iterator configEltIter = _cfgElement->begin() ; !(configEltIter == _cfgElement->end()) ; ++configEltIter)
    {

        if( (*configEltIter)->getName() == "item" )
        {
            ::fwRuntime::ConfigurationElement::sptr itemConfigElem = (*configEltIter);

            // Test build mode
            std::string buildMode = BUILD_OBJECT;

            if ( (*configEltIter)->hasAttribute( OBJECT_BUILD_MODE ) )
            {
                buildMode = itemConfigElem->getExistingAttributeValue( OBJECT_BUILD_MODE );
                OSLM_ASSERT( "Sorry, buildMode \""<< buildMode <<"\" is not supported by the application.", buildMode == BUILD_OBJECT || buildMode == GET_OBJECT );
            }


            SLM_ASSERT( "Sorry, the xml element \"item\" must have an attribute named \"key\" .", itemConfigElem->hasAttribute("key") );
            std::string key = itemConfigElem->getExistingAttributeValue("key");
            SLM_ASSERT( "Sorry, the xml element \"item\" must have an attribute named \"key\" not empty.", ! key.empty() );
            SLM_ASSERT( "Sorry, xml element item must have one (and only one) xml sub-element \"object\".", itemConfigElem->size() == 1 && (*itemConfigElem->begin())->getName() == "object" );

            if( buildMode == BUILD_OBJECT )
            {
                // Test if key already exist in composite
                OSLM_ASSERT("Sorry the key "<< key <<" already exists in the composite.", dataComposite->find( key ) == dataComposite->end() );

                // Create and manage object config
                ::fwTools::Object::sptr localObj = ::fwServices::New( * ( itemConfigElem->begin() ) ) ;

                // Add object
                SLM_ASSERT("Sorry an ::fwData::Composite can contain only ::fwData::Object", ::fwData::Object::dynamicCast( localObj ) );
                (*dataComposite)[ key ] = ::fwData::Object::dynamicCast( localObj );

            }
            else // if( buildMode == GET_OBJECT )
            {
                // Test if key already exist in composite
                OSLM_ASSERT("Sorry the key "<< key <<" not exists in the composite.", dataComposite->find( key ) != dataComposite->end() );

                ::fwData::Object::sptr objRef = (*dataComposite)[ key ];

                ::fwRuntime::ConfigurationElement::sptr objectConfigElem = ( * ( itemConfigElem->begin() ) );

                OSLM_ASSERT("Sorry, for an item reference, the xml subelement \"object\" does not have attributes.", objectConfigElem->getAttributes().size() == 0 );
                OSLM_ASSERT("Sorry, some subelement of element \"object\" are not supported.", refObjectValidator( objectConfigElem ) );
                ::fwServices::addServicesToObjectFromCfgElem( objRef, objectConfigElem );
            }
        }
    }

//    for( ::fwRuntime::ConfigurationElement::Iterator configEltIter = _cfgElement->begin() ; !(configEltIter == _cfgElement->end()) ; ++configEltIter)
//    {
//        if( (*configEltIter)->getName() == "object" )
//        {
//            // Configuring the input/output
//            ::fwTools::Object::sptr localObj = ::fwServices::New( (*configEltIter) ) ;
//            assert( localObj ) ;
//            // Inserting initialized object in processObject
//            SLM_ASSERT("Sorry, attribute id is missing", (*configEltIter)->hasAttribute("id") ) ;
//            std::string _id = (*configEltIter)->getExistingAttributeValue("id") ;
//            assert( ::fwData::Object::dynamicCast(localObj ) );
//            assert( dataComposite->getRefMap().find(_id) == dataComposite->getRefMap().end() ) ;
//            dataComposite->getRefMap()[_id] = ::fwData::Object::dynamicCast(localObj ) ;
//        }
//    }
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
