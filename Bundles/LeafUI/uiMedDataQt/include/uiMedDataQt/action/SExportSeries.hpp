/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
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

namespace uiMedDataQt
{
namespace action
{

/**
 * @brief   This action allows to export the series specified in config to the SeriesDB.
 * The export is performed when updating the service.
 *
 * @note This action is not executable if the series already exists in the SeriesDB.
 *
 * @section Slots Slots
 * - \b checkAddedSeries(::fwMedData::SeriesDB::ContainerType): make the action executable if the added series matches
 * the series we want to export.
 * - \b checkAddedSeries(::fwMedData::SeriesDB::ContainerType): make the action inexecutable if the added series matches
 * the series we want to export.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::uiMedDataQt::action::SExportSeries" autoConnect="yes">
            <inout key="series" uid="..." />
            <inout key="seriesDB" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b series [::fwMedData::Series]: Source series to export.
 * - \b seriesDB [::fwMedData::SeriesDB]: Target series database where the series should be exported.
 */
class UIMEDDATAQT_CLASS_API SExportSeries : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (SExportSeries)(::fwGui::IActionSrv) );

    /// Constructor
    UIMEDDATAQT_API SExportSeries();

    /// Destructor
    UIMEDDATAQT_API virtual ~SExportSeries() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection.
     *
     * Connect ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG to this::s_CHECK_ADDED_SERIES_SLOT.
     * Connect ::fwMedData::SeriesDB::s_REMOVED_SERIES_SIG to this::s_CHECK_REMOVED_SERIES_SLOT.
     */
    UIMEDDATAQT_API virtual KeyConnectionsMap getAutoConnections() const;

protected:

    /// This method is used to configure the service parameters
    virtual void configuring();

    /// Starts service. If series associated with m_seriesId exists in SeriesDB, this action is not executable.
    virtual void starting();

    /// Stops service. Does nothing.
    virtual void stopping();

    /// Adds the series specified by m_seriesId in the SeriesDB.
    virtual void updating();

    virtual void info( std::ostream& _sstream );

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
    /// @deprecated appXml
    std::string m_seriesId;
};
} // namespace action
} // namespace uiMedDataQt

#endif // __UIMEDDATAQT_ACTION_SEXPORTSERIES_HPP__

