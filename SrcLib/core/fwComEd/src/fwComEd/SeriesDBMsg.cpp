/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/message/macros.hpp>

#include "fwComEd/SeriesDBMsg.hpp"

fwServicesMessageRegisterMacro( ::fwComEd::SeriesDBMsg );

namespace fwComEd
{

std::string SeriesDBMsg::ADDED_OBJECTS   = "ADDED_OBJECTS";
std::string SeriesDBMsg::REMOVED_OBJECTS = "REMOVED_OBJECTS";

//-------------------------------------------------------------------------

SeriesDBMsg::SeriesDBMsg(::fwServices::ObjectMsg::Key key)
{
    m_removedSeries = ::fwData::Vector::New();
    m_addedSeries   = ::fwData::Vector::New();
}

//-------------------------------------------------------------------------

SeriesDBMsg::~SeriesDBMsg() throw()
{
}

//-------------------------------------------------------------------------

void SeriesDBMsg::appendAddedSeries( ::fwMedData::Series::sptr newSeries )
{
    ::fwData::Vector::ContainerType& addedSeries = m_addedSeries->getContainer();
    if( !this->hasEvent( ADDED_OBJECTS ) )
    {
        this->addEvent( ADDED_OBJECTS, m_addedSeries );
    }

    SLM_ASSERT("This Series is already registered",
               std::find(addedSeries.begin(), addedSeries.end(), newSeries) == addedSeries.end() );

    addedSeries.push_back(newSeries);
}

//-----------------------------------------------------------------------------

::fwData::Vector::sptr SeriesDBMsg::getAddedSeries() const
{
    return m_addedSeries;
}

//-----------------------------------------------------------------------------

void SeriesDBMsg::appendRemovedSeries( ::fwMedData::Series::sptr oldSeries )
{
    ::fwData::Vector::ContainerType& removedSeries = m_removedSeries->getContainer();
    if( !this->hasEvent( REMOVED_OBJECTS ) )
    {
        this->addEvent( REMOVED_OBJECTS, m_removedSeries );
    }

    SLM_ASSERT("This Series is already register",
               std::find(removedSeries.begin(), removedSeries.end(), oldSeries) == removedSeries.end() );

    removedSeries.push_back( oldSeries );
}

//-----------------------------------------------------------------------------

::fwData::Vector::sptr SeriesDBMsg::getRemovedSeries() const
{
    return m_removedSeries;
}

//-----------------------------------------------------------------------------

} // namespace op

