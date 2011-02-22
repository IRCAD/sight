/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/macros.hpp"
#include "fwServices/ObjectXMLParser.hpp"

REGISTER_SERVICE( ::fwServices::IXMLParser , ::fwServices::ObjectXMLParser , ::fwTools::Object ) ;

namespace fwServices
{

//------------------------------------------------------------------------------

ObjectXMLParser::ObjectXMLParser( )
{}

//------------------------------------------------------------------------------

ObjectXMLParser::~ObjectXMLParser()
{}

//------------------------------------------------------------------------------

bool ObjectXMLParser::refObjectValidator( ::fwRuntime::ConfigurationElement::csptr _cfgElement )
{
    bool isOk = true;

    BOOST_FOREACH( ::fwRuntime::ConfigurationElement::csptr elem, _cfgElement->getElements() )
    {
        std::string subElementName = elem->getName();
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

void ObjectXMLParser::updating( ) throw(fwTools::Failed)
{
    SLM_FATAL("Sorry, this method is depreciated");
}


//------------------------------------------------------------------------------

void ObjectXMLParser::createConfig( ::fwTools::Object::sptr _obj )
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT = "new";
    const std::string GET_OBJECT = "ref";

    ::fwTools::Object::sptr associatedObject = _obj;
    assert( associatedObject ) ;

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
            SLM_ASSERT( "Sorry, xml element item must have one (and only one) xml sub-element \"object\".", elem->size() == 1 && (*(elem->getElements().begin()))->getName() == "object" );

            if( buildMode == BUILD_OBJECT )
            {
                // Test if key already exist in object
                OSLM_ASSERT("Sorry the key "<< key <<" already exists in the object.", associatedObject->getFieldSize( key ) == 0 );

                // Create and manage object config
                ::fwServices::AppConfigManager::NewSptr ctm;
                ctm->setConfig( *(elem->getElements().begin()) );
                m_ctmContainer.push_back( ctm );
                ctm->create();
                ::fwTools::Object::sptr localObj = ctm->getConfigRoot();

                // Add object
                associatedObject->setFieldSingleElement( key, localObj);
            }
            else // if( buildMode == GET_OBJECT )
            {
                SLM_FATAL("ACH => Todo");
                // ToDo
            }
        }
    }
}

//------------------------------------------------------------------------------

void ObjectXMLParser::startConfig()
{
    BOOST_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void ObjectXMLParser::updateConfig()
{
    BOOST_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void ObjectXMLParser::stopConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void ObjectXMLParser::destroyConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->destroy();
    }
    m_ctmContainer.clear();
}

//------------------------------------------------------------------------------

}

