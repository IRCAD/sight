/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOMED_SERIESDBMSG_HPP__
#define __FWCOMED_SERIESDBMSG_HPP__

#include <fwData/Vector.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/ObjectMsg.hpp>

#include "fwComEd/config.hpp"


namespace fwComEd
{

/**
 * @brief   Object event message specialized for SeriesDB : store modification information
 * @class   SeriesDBMsg
 * @date    2005-2013.
 * @see     ::fwServices::ObjectMsg
 */
class FWCOMED_CLASS_API SeriesDBMsg : public ::fwServices::ObjectMsg
{
public:

    fwCoreClassDefinitionsWithFactoryMacro((SeriesDBMsg)(::fwServices::ObjectMsg), ( () ),
                                           ::fwServices::factory::message::New< SeriesDBMsg > );


    /// Event identifier used to inform that Series were added
    FWCOMED_API static std::string ADDED_OBJECTS;

    /// Event identifier used to inform that Series were removed
    FWCOMED_API static std::string REMOVED_OBJECTS;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWCOMED_API SeriesDBMsg(::fwServices::ObjectMsg::Key key);

    /**
     * @brief   Destructor : does nothing.
     */
    FWCOMED_API virtual ~SeriesDBMsg() throw();


    /**
     * @brief       This method add a Series in the vector of added Series.
     * @param[in]   newSeries   : the added Series.
     */
    FWCOMED_API void appendAddedSeries( ::fwMedData::Series::sptr newSeries );

    /**
     * @brief   Return the vector of the added Series.
     */
    FWCOMED_API ::fwData::Vector::sptr getAddedSeries() const;


    /**
     * @brief       This method add a Series in SeriesDB of removed Series.
     * @param[in]   oldSeries   : the Series which will be removed.
     */
    FWCOMED_API void appendRemovedSeries( ::fwMedData::Series::sptr oldSeries );

    /**
     * @brief   Return the vector of the removed Series.
     */
    FWCOMED_API ::fwData::Vector::sptr getRemovedSeries() const;

private:

    /// Stores removed object from SeriesDB
    ::fwData::Vector::sptr m_removedSeries;

    /// Stores new added object in SeriesDB
    ::fwData::Vector::sptr m_addedSeries;
};

} // namespace fwComEd

#endif //__FWCOMED_SERIESDBMSG_HPP__

