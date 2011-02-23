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
    SLM_FATAL("Sorry, this method is depreciated.");
}

//------------------------------------------------------------------------------

void ICompositeParser::createConfig( ::fwTools::Object::sptr _obj )
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT = "new";
    const std::string GET_OBJECT = "ref";

    ::fwData::Composite::sptr dataComposite = ::fwData::Composite::dynamicCast(_obj);
    SLM_ASSERT("Sorry, object given in parameter is not a fwData::Composite",dataComposite);

    BOOST_FOREACH( ::fwRuntime::ConfigurationElement::csptr elem, m_cfg->getElements() )
    {
        if( elem->getName() == "item" )
        {

            // Test build mode
            std::string buildMode = BUILD_OBJECT;

            if ( elem->hasAttribute( OBJECT_BUILD_MODE ) )
            {
                buildMode = elem->getExistingAttributeValue( OBJECT_BUILD_MODE );
                OSLM_ASSERT( "Sorry, buildMode \""<< buildMode <<"\" is not supported by the application.", buildMode == BUILD_OBJECT || buildMode == GET_OBJECT );
            }


            SLM_ASSERT( "Sorry, the xml element \"item\" must have an attribute named \"key\" .", elem->hasAttribute("key") );
            std::string key = elem->getExistingAttributeValue("key");
            SLM_ASSERT( "Sorry, the xml element \"item\" must have an attribute named \"key\" not empty.", ! key.empty() );
            SLM_ASSERT( "Sorry, xml element item must have one (and only one) xml sub-element \"object\".", elem->size() == 1 && (*elem->getElements().begin())->getName() == "object" );

            if( buildMode == BUILD_OBJECT )
            {
                // Test if key already exist in composite
                OSLM_ASSERT("Sorry the key "<< key <<" already exists in the composite.", dataComposite->find( key ) == dataComposite->end() );

                // Create and manage object config
                ::fwServices::AppConfigManager::NewSptr ctm;
                ctm->setConfig( * ( elem->getElements().begin() ) );
                m_ctmContainer.push_back( ctm );
                ctm->create();
                ::fwTools::Object::sptr localObj = ctm->getConfigRoot();

                // Add object
                SLM_ASSERT("Sorry an ::fwData::Composite can contain only ::fwData::Object", ::fwData::Object::dynamicCast( localObj ) );
                (*dataComposite)[ key ] = ::fwData::Object::dynamicCast( localObj );

            }
            else // if( buildMode == GET_OBJECT )
            {
                SLM_FATAL("ACH => Todo");
            }
        }
    }
}

//------------------------------------------------------------------------------

void ICompositeParser::startConfig()
{
    BOOST_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void ICompositeParser::updateConfig()
{
    BOOST_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void ICompositeParser::stopConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void ICompositeParser::destroyConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->destroy();
    }
    m_ctmContainer.clear();
}

//------------------------------------------------------------------------------

void IProcessObjectParser::updating( ) throw(fwTools::Failed)
{
    SLM_FATAL("Sorry, this method is depreciated.");
}

//------------------------------------------------------------------------------

void IProcessObjectParser::configureIO( ::fwTools::Object::sptr io, const ::fwRuntime::ConfigurationElement::sptr list)
{
    SLM_FATAL("Sorry, this method is depreciated.");
}

//------------------------------------------------------------------------------

}
