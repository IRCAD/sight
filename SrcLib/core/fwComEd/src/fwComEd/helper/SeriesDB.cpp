/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwServices/IEditionService.hpp>

#include "fwComEd/helper/SeriesDB.hpp"

namespace fwComEd
{
namespace helper
{

//-----------------------------------------------------------------------------

SeriesDB::SeriesDB( ::fwMedData::SeriesDB::wptr seriesDB )
    :   m_seriesDBMsg ( ::fwComEd::SeriesDBMsg::New() ),
        m_seriesDB ( seriesDB )
{}

//-----------------------------------------------------------------------------

SeriesDB::~SeriesDB()
{}

//-----------------------------------------------------------------------------

void SeriesDB::add( ::fwMedData::Series::sptr newSeries )
{
    ::fwMedData::SeriesDB::sptr seriesDB = m_seriesDB.lock();
    OSLM_ASSERT( "The object " << newSeries->getID() << " must not exist in SeriesDB." ,
                 std::find(seriesDB->begin(), seriesDB->end(), newSeries) == seriesDB->end());

    // Modify SeriesDB
    seriesDB->getContainer().push_back( newSeries );

    // Modify message
    m_seriesDBMsg->appendAddedSeries( newSeries );

}

//-----------------------------------------------------------------------------

void SeriesDB::remove( ::fwMedData::Series::sptr oldSeries )
{
    ::fwMedData::SeriesDB::sptr seriesDB = m_seriesDB.lock();
    ::fwMedData::SeriesDB::iterator iter = std::find(seriesDB->begin(), seriesDB->end(), oldSeries);
    OSLM_ASSERT( "The object " << oldSeries->getID() << " must exist in SeriesDB." ,
                 iter != seriesDB->end());

    // Modify SeriesDB
    seriesDB->getContainer().erase( iter );

    // Modify message
    m_seriesDBMsg->appendRemovedSeries( oldSeries );

}

//-----------------------------------------------------------------------------

void SeriesDB::clear()
{
    ::fwMedData::SeriesDB::sptr seriesDB = m_seriesDB.lock();

    while (!seriesDB->empty())
    {
        this->remove(seriesDB->front());
    }
}

//-----------------------------------------------------------------------------

void SeriesDB::merge(::fwMedData::SeriesDB::sptr seriesDBIn)
{
    ::fwMedData::SeriesDB::ContainerType& vectIn = seriesDBIn->getContainer();
    BOOST_FOREACH(::fwMedData::Series::sptr series, vectIn)
    {
        this->add(series);
    }
}

//-----------------------------------------------------------------------------

void SeriesDB::notify( ::fwServices::IService::sptr serviceSource, bool notifySource )
{
    if ( !m_seriesDBMsg->getEventIds().empty() )
    {
        ::fwServices::IEditionService::notify( serviceSource, m_seriesDB.lock(), m_seriesDBMsg , notifySource );
    }
    SLM_INFO_IF("Sorry, this helper cannot notify his message because the message is empty.",
                m_seriesDBMsg->getEventIds().empty());
}

//-----------------------------------------------------------------------------

} // namespace helper
} // namespace fwComEd
