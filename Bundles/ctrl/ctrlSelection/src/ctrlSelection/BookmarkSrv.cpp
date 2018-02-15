/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    //handlingEventOff();
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

void BookmarkSrv::info( std::ostream &_sstream )
{
}

//-----------------------------------------------------------------------------

} // ctrlSelection
