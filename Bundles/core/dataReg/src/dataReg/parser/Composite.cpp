/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "dataReg/parser/Composite.hpp"

#include <fwData/Composite.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::Composite, ::fwData::Composite );

namespace dataReg
{
namespace parser
{

//------------------------------------------------------------------------------

bool Composite::refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement )
{
    bool isOk = true;

    for(    ::fwRuntime::ConfigurationElement::Iterator configEltIter = _cfgElement->begin();
            configEltIter != _cfgElement->end();
            ++configEltIter)
    {
        std::string subElementName = (*configEltIter)->getName();
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

void Composite::updating()
{
    SLM_FATAL("This method is deprecated, and this, shouldn't be used.");
}

//------------------------------------------------------------------------------

void Composite::createConfig( ::fwTools::Object::sptr _obj )
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT      = "new";
    const std::string GET_OBJECT        = "ref";

    ::fwData::Composite::sptr dataComposite = ::fwData::Composite::dynamicCast(_obj);
    SLM_ASSERT("The passed object must be a fwData::Composite",dataComposite);

    for( ::fwRuntime::ConfigurationElement::csptr elem :  m_cfg->getElements() )
    {
        if( elem->getName() == "item" )
        {

            // Test build mode
            std::string buildMode = BUILD_OBJECT;

            if ( elem->hasAttribute( OBJECT_BUILD_MODE ) )
            {
                buildMode = elem->getExistingAttributeValue( OBJECT_BUILD_MODE );
                OSLM_ASSERT( "The buildMode \""<< buildMode <<"\" is not supported, it should be either BUILD_OBJECT"
                             "or GET_OBJECT.",
                             buildMode == BUILD_OBJECT || buildMode == GET_OBJECT );
            }

            SLM_ASSERT( "The xml element \"item\" must have an attribute named \"key\" .",
                        elem->hasAttribute("key") );
            std::string key = elem->getExistingAttributeValue("key");
            SLM_ASSERT( "The xml element \"item\" must have an attribute named \"key\" which is not empty.",
                        !key.empty() );
            SLM_ASSERT( "The xml element \"item\" must have one (and only one) xml sub-element \"object\".",
                        elem->size() == 1 && (*elem->getElements().begin())->getName() == "object" );

            if( buildMode == BUILD_OBJECT )
            {
                // Test if key already exist in composite
                OSLM_ASSERT("The key "<< key <<" already exists in the composite.", dataComposite->find(
                                key ) == dataComposite->end() );

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
                SLM_ASSERT("A ::fwData::Composite can contain only ::fwData::Object", localObj );
                (*dataComposite)[ key ] = localObj;

            }
            else // if( buildMode == GET_OBJECT )
            {
                SLM_FATAL("ACH => Todo");
            }
        }
    }
}

//------------------------------------------------------------------------------

void Composite::startConfig()
{
    for( ::fwServices::IAppConfigManager::sptr ctm :  m_ctmContainer )
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void Composite::updateConfig()
{
    for( ::fwServices::IAppConfigManager::sptr ctm :  m_ctmContainer )
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void Composite::stopConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::IAppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void Composite::destroyConfig()
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

