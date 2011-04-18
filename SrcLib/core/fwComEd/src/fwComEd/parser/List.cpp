/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwData/List.hpp>

#include "fwComEd/parser/List.hpp"

REGISTER_SERVICE( ::fwServices::IXMLParser, ::fwComEd::parser::List, ::fwData::List );

namespace fwComEd
{
namespace parser
{

//------------------------------------------------------------------------------

bool List::refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement )
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
            OSLM_ERROR("xml subelement \""<< subElementName <<"\" for element object is not supported for the moment when you use a reference on item List.");
            isOk = false;
        }
    }

    return isOk;
}

//------------------------------------------------------------------------------

void List::updating( ) throw(fwTools::Failed)
{
    SLM_FATAL("Sorry, this method is depreciated.");
}

//------------------------------------------------------------------------------

void List::createConfig( ::fwTools::Object::sptr _obj )
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT = "new";
    const std::string GET_OBJECT = "ref";

    ::fwData::List::sptr dataList = ::fwData::List::dynamicCast(_obj);
    SLM_ASSERT("Sorry, object given in parameter is not a fwData::List",dataList);

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
          
            if( buildMode == BUILD_OBJECT )
            {
                
                // Create and manage object config
                ::fwServices::AppConfigManager::NewSptr ctm;
                ctm->setConfig( * ( elem->getElements().begin() ) );
                m_ctmContainer.push_back( ctm );
                ctm->create();
                ::fwTools::Object::sptr localObj = ctm->getConfigRoot();

                // Add object
                SLM_ASSERT("Sorry an ::fwData::List can contain only ::fwData::Object", ::fwData::Object::dynamicCast( localObj ) );
                dataList->push_back( ::fwData::Object::dynamicCast( localObj ));
             
            }
            else // if( buildMode == GET_OBJECT )
            {
                SLM_FATAL("ACH => Todo");
            }
        }
    }
}

//------------------------------------------------------------------------------

void List::startConfig()
{
    BOOST_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void List::updateConfig()
{
    BOOST_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void List::stopConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void List::destroyConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::AppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->destroy();
    }
    m_ctmContainer.clear();
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace fwComEd


