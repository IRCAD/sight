/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "dataReg/parser/Object.hpp"

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::Object, ::fwData::Object );

namespace dataReg
{
namespace parser
{

//------------------------------------------------------------------------------

Object::Object( )
{
}

//------------------------------------------------------------------------------

Object::~Object()
{
}

//------------------------------------------------------------------------------

bool Object::refObjectValidator( ::fwRuntime::ConfigurationElement::csptr _cfgElement )
{
    bool isOk = true;

    for( ::fwRuntime::ConfigurationElement::csptr elem :  _cfgElement->getElements() )
    {
        std::string subElementName = elem->getName();
        if(     subElementName != "service" &&
                subElementName != "serviceList"    )
        {
            OSLM_ERROR(
                "xml subelement \""<< subElementName <<
                "\" for element object is not supported for the moment when you use a reference on item composite.");
            isOk = false;
        }
    }

    return isOk;
}

//------------------------------------------------------------------------------

void Object::updating( ) throw(fwTools::Failed)
{
    SLM_FATAL("This method is deprecated, and thus, shouldn't be used.");
}

//------------------------------------------------------------------------------

void Object::createConfig( ::fwTools::Object::sptr _obj )
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT      = "new";
    const std::string GET_OBJECT        = "ref";

    ::fwData::Object::sptr associatedObject = ::fwData::Object::dynamicCast(_obj);
    SLM_ASSERT("associatedObject not instanced", associatedObject);

    for( ::fwRuntime::ConfigurationElement::csptr elem :  m_cfg->getElements() )
    {

        if( elem->getName() == "item" )
        {
            // Test build mode
            std::string buildMode = BUILD_OBJECT;

            if ( elem->hasAttribute( OBJECT_BUILD_MODE ) )
            {
                buildMode = elem->getExistingAttributeValue( OBJECT_BUILD_MODE );
                OSLM_ASSERT( "The buildMode \""<< buildMode <<"\" is not supported, it should either be BUILD_OBJECT "
                             "or GET_OBJECT.",
                             buildMode == BUILD_OBJECT || buildMode == GET_OBJECT );
            }

            SLM_ASSERT( "The xml element \"item\" must have an attribute named \"key\" .",
                        elem->hasAttribute("key") );
            std::string key = elem->getExistingAttributeValue("key");
            SLM_ASSERT( "The xml element \"item\" must have an attribute named \"key\" whick is not empty.",
                        !key.empty() );
            SLM_ASSERT( "The xml element \"item\" must have one (and only one) xml sub-element \"object\".",
                        elem->size() == 1 && (*(elem->getElements().begin()))->getName() == "object" );

            if( buildMode == BUILD_OBJECT )
            {
                // Test if key already exist in object
                OSLM_ASSERT("The key "<< key <<" already exists in the object.", !associatedObject->getField(
                                key ) );

                // Create and manage object config
                ::fwServices::IAppConfigManager::sptr ctm = ::fwServices::IAppConfigManager::New();
                if( ::fwServices::IService::isVersion2())
                {
                    ctm->::fwServices::IAppConfigManager::setConfig( elem );
                }
                else
                {
                    ctm->::fwServices::IAppConfigManager::setConfig( *( elem->getElements().begin() ) );
                }
                m_ctmContainer.push_back( ctm );
                ctm->create();
                ::fwData::Object::sptr localObj = ctm->getConfigRoot();

                // Add object
                associatedObject->setField(key, localObj);
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

void Object::startConfig()
{
    for( ::fwServices::IAppConfigManager::sptr ctm :  m_ctmContainer )
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void Object::updateConfig()
{
    for( ::fwServices::IAppConfigManager::sptr ctm :  m_ctmContainer )
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void Object::stopConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::IAppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void Object::destroyConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::IAppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->destroy();
    }
    m_ctmContainer.clear();
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace dataReg

