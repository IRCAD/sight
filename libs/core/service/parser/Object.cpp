/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "service/parser/Object.hpp"

#include "service/macros.hpp"

#include <boost/foreach.hpp>

namespace sight::service
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

bool Object::refObjectValidator( core::runtime::ConfigurationElement::csptr _cfgElement )
{
    bool isOk = true;

    for( core::runtime::ConfigurationElement::csptr elem :  _cfgElement->getElements() )
    {
        std::string subElementName = elem->getName();
        if(     subElementName != "service" &&
                subElementName != "serviceList"    )
        {
            SLM_ERROR(
                "xml subelement \""<< subElementName <<
                    "\" for element object is not supported for the moment when you use a reference on item composite.");
            isOk = false;
        }
    }

    return isOk;
}

//------------------------------------------------------------------------------

void Object::updating( )
{
    SLM_FATAL("This method is deprecated, and thus, shouldn't be used.");
}

//------------------------------------------------------------------------------

void Object::createConfig( core::tools::Object::sptr _obj )
{
    // Declaration of attributes values
    const std::string OBJECT_BUILD_MODE = "src";
    const std::string BUILD_OBJECT      = "new";
    const std::string GET_OBJECT        = "ref";

    data::Object::sptr associatedObject = data::Object::dynamicCast(_obj);
    SLM_ASSERT("associatedObject not instanced", associatedObject);

    for( core::runtime::ConfigurationElement::csptr elem :  m_cfg->getElements() )
    {

        if( elem->getName() == "item" )
        {
            // Test build mode
            std::string buildMode = BUILD_OBJECT;

            if ( elem->hasAttribute( OBJECT_BUILD_MODE ) )
            {
                buildMode = elem->getExistingAttributeValue( OBJECT_BUILD_MODE );
                SLM_ASSERT( "The buildMode \""<< buildMode <<"\" is not supported, it should either be BUILD_OBJECT "
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
                SLM_ASSERT("The key "<< key <<" already exists in the object.", !associatedObject->getField(
                               key ) );

                // Create and manage object config
                service::IAppConfigManager::sptr ctm = service::IAppConfigManager::New();
                ctm->service::IAppConfigManager::setConfig( elem );

                m_ctmContainer.push_back( ctm );
                ctm->create();
                data::Object::sptr localObj = ctm->getConfigRoot();

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
    for( service::IAppConfigManager::sptr ctm :  m_ctmContainer )
    {
        ctm->start();
    }
}

//------------------------------------------------------------------------------

void Object::updateConfig()
{
    for( service::IAppConfigManager::sptr ctm :  m_ctmContainer )
    {
        ctm->update();
    }
}

//------------------------------------------------------------------------------

void Object::stopConfig()
{
    BOOST_REVERSE_FOREACH( service::IAppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->stop();
    }
}

//------------------------------------------------------------------------------

void Object::destroyConfig()
{
    BOOST_REVERSE_FOREACH( service::IAppConfigManager::sptr ctm, m_ctmContainer )
    {
        ctm->destroy();
    }
    m_ctmContainer.clear();
}

//------------------------------------------------------------------------------

} //namespace parser
} //namespace sight::service
