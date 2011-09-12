/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/Bookmarks.hpp>
#include <fwTools/Object.hpp>

#include <fwData/String.hpp>

#include <fwServices/macros.hpp>

#include "ctrlSelection/BookmarkSrv.hpp"


namespace ctrlSelection
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::fwServices::IController, ::ctrlSelection::BookmarkSrv, ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

BookmarkSrv::BookmarkSrv() throw()
{
    handlingEventOff();
}

//-----------------------------------------------------------------------------

BookmarkSrv::~BookmarkSrv() throw()
{}

//-----------------------------------------------------------------------------

void BookmarkSrv::configuring()  throw ( ::fwTools::Failed )
{
    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("bookmark");
    SLM_ASSERT("Problem with configuration for BookmarkSrv type, one element \"bookmark\" must be present", m_configuration->findAllConfigurationElement("bookmark").size() == 1 );

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

void BookmarkSrv::starting()  throw ( ::fwTools::Failed )
{
    ::fwTools::Bookmarks::add( m_bookmarkName , this->getObject());
}

//-----------------------------------------------------------------------------

void BookmarkSrv::stopping()  throw ( ::fwTools::Failed )
{
    OSLM_WARN_IF("Bookmark \""<< m_bookmarkName << "\" already removed", ::fwTools::Bookmarks::exist( m_bookmarkName ));
    if(::fwTools::Bookmarks::exist( m_bookmarkName ))
    {
        ::fwTools::Bookmarks::remove( m_bookmarkName);
    }
}

//-----------------------------------------------------------------------------

void BookmarkSrv::swapping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void BookmarkSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void BookmarkSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void BookmarkSrv::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void BookmarkSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // ctrlSelection
