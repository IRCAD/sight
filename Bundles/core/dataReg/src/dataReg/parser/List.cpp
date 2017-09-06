/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "dataReg/parser/List.hpp"

#include <fwData/List.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

fwServicesRegisterMacro( ::fwServices::IXMLParser, ::dataReg::parser::List, ::fwData::List );

namespace dataReg
{
namespace parser
{

//------------------------------------------------------------------------------

bool List::refObjectValidator( ::fwRuntime::ConfigurationElement::sptr _cfgElement )
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
                "\" for element object is not supported for the moment when you use a reference on item List.");
            isOk = false;
        }
    }

    return isOk;
}

//------------------------------------------------------------------------------

void List::updating( )
{
    SLM_FATAL("This method is deprecated, and thus shouldn't be used.");
}

//------------------------------------------------------------------------------

void List::createConfig( ::fwTools::Object::sptr _obj )
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT      = "new";
    const std::string GET_OBJECT        = "ref";

    ::fwData::List::sptr dataList = ::fwData::List::dynamicCast(_obj);
    SLM_ASSERT("The passed object must be a fwData::List", dataList);

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

            if( buildMode == BUILD_OBJECT )
            {

                // Create and manage object config
                ::fwServices::IAppConfigManager::sptr ctm = ::fwServices::IAppConfigManager::New();
                ctm->::fwServices::IAppConfigManager::setConfig( elem );

                m_ctmContainer.push_back( ctm );
                ctm->create();
                ::fwData::Object::sptr localObj = ctm->getConfigRoot();

                // Add object
                SLM_ASSERT("A ::fwData::List can contain only ::fwData::Object", localObj );
                dataList->getContainer().push_back( localObj );

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
    for( ::fwServices::IAppConfigManager::sptr ctm :  m_ctmContainer )
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void List::updateConfig()
{
    for( ::fwServices::IAppConfigManager::sptr ctm :  m_ctmContainer )
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void List::stopConfig()
{
    BOOST_REVERSE_FOREACH( ::fwServices::IAppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void List::destroyConfig()
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

