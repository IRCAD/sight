/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_ACTION_SEXPORTSERIES_HPP__
#define __UIMEDDATAQT_ACTION_SEXPORTSERIES_HPP__

#include "uiMedDataQt/config.hpp"

#include <fwCom/Slots.hpp>

#include <fwGui/IActionSrv.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>


namespace uiMedData
{
namespace action
{
/**
 * @brief   This action allows to export the series specified in config to the SeriesDB.
 * @class   SExportSeries
 *
 * @date    2013.
 * @note This action is not executable if the series already exists in the SeriesDB.
 */
class UIMEDDATAQT_CLASS_API SExportSeries : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (SExportSeries)(::fwGui::IActionSrv) );

    /// Constructor
    UIMEDDATAQT_API SExportSeries();

    /// Destructor
    UIMEDDATAQT_API virtual ~SExportSeries() throw();

protected:

    /// Starts service. If series associated with m_seriesId exists in SeriesDB, this action is not executable.
    virtual void starting() throw(::fwTools::Failed);

    /// Stops service. Does nothing.
    virtual void stopping() throw(::fwTools::Failed);

    /**
     *
     * @verbatim
       <service uid="selector" impl="::uiMedData::action::SExportSeries" type="::fwGui::IActionSrv" autoConnect="yes">
        <seriesId>activitySeries</seriesId>
       </service>
       @endverbatim
     * - \b seriesId : defines the fwId of the series to export in the current SeriesDB.
     */
    virtual void configuring() throw (::fwTools::Failed);

    /// Adds the series specified by m_seriesId in the SeriesDB.
    virtual void updating() throw (::fwTools::Failed);

    virtual void info( std::ostream &_sstream );

private:

    /**
     * @name Slots
     * @{
     */
    static const ::fwCom::Slots::SlotKeyType s_CHECK_ADDED_SERIES_SLOT;
    static const ::fwCom::Slots::SlotKeyType s_CHECK_REMOVED_SERIES_SLOT;

    /// Slot: check if specified series is added and set action not executable
    void checkAddedSeries(::fwMedData::SeriesDB::ContainerType addedSeries);

    /// Slot: check if specified series is removed and set action executable
    void checkRemovedSeries(::fwMedData::SeriesDB::ContainerType removedSeries);

    /**
     * @}
     */

    /// Returns current series given by its fwID m_seriesId.
    ::fwMedData::Series::sptr getSeries();

    /// fwID of the series to add in SeriesDB
    std::string m_seriesId;
};
} // namespace action
} // namespace uiMedData

#endif // __UIMEDDATAQT_ACTION_SEXPORTSERIES_HPP__

