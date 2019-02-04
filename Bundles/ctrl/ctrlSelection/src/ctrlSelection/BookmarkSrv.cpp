/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ctrlSelection/BookmarkSrv.hpp"

#include <fwData/String.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Bookmarks.hpp>

namespace ctrlSelection
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::ctrlSelection::BookmarkSrv, ::fwData::Object );

//-----------------------------------------------------------------------------

BookmarkSrv::BookmarkSrv() noexcept
{
    FW_DEPRECATED_MSG("This service is no longer supported", "20.0");
}

//-----------------------------------------------------------------------------

BookmarkSrv::~BookmarkSrv() noexcept
{
}

//-----------------------------------------------------------------------------

void BookmarkSrv::configuring()
{
    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("bookmark");
    SLM_ASSERT("Problem with configuration for BookmarkSrv type, one element \"bookmark\" must be present", m_configuration->findAllConfigurationElement(
                   "bookmark").size() == 1 );

    m_bookmarkName = "";

    if (config->hasAttribute("fromString"))
    {
        std::string uid = config->getExistingAttributeValue("fromString");
        OSLM_ASSERT("Object '" << uid << "' does not exist", ::fwTools::fwID::exist(uid));
        ::fwData::String::sptr str = ::fwData::String::dynamicCast(::fwTools::fwID::getObject(uid));
        OSLM_ASSERT("Object '" << uid << "' is not a '::fwData::String'", str);
        m_bookmarkName = str->value();
    }
    if (config->hasAttribute("fromString") && config->hasAttribute("name"))
    {
        m_bookmarkName += "_";
    }
    if (config->hasAttribute("name"))
    {
        m_bookmarkName += config->getExistingAttributeValue("name");
    }
}

//-----------------------------------------------------------------------------

void BookmarkSrv::starting()
{
    ::fwTools::Bookmarks::add( m_bookmarkName, this->getObject());
}

//-----------------------------------------------------------------------------

void BookmarkSrv::stopping()
{
    OSLM_WARN_IF("Bookmark \""<< m_bookmarkName << "\" already removed", ::fwTools::Bookmarks::exist( m_bookmarkName ));
    if(::fwTools::Bookmarks::exist( m_bookmarkName ))
    {
        ::fwTools::Bookmarks::remove( m_bookmarkName);
    }
}

//-----------------------------------------------------------------------------

void BookmarkSrv::swapping()
{
}

//-----------------------------------------------------------------------------

void BookmarkSrv::reconfiguring()
{
}

//-----------------------------------------------------------------------------

void BookmarkSrv::updating()
{
}

//-----------------------------------------------------------------------------

void BookmarkSrv::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

} // ctrlSelection
