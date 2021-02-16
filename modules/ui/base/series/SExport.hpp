/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#pragma once

#include "modules/ui/base/config.hpp"

#include <core/com/Slots.hpp>

#include <data/Series.hpp>
#include <data/SeriesDB.hpp>

#include <ui/base/IAction.hpp>

namespace sight::modules::ui::base
{
namespace series
{

/**
 * @brief   This action allows to export the series specified in config to the SeriesDB.
 * The export is performed when updating the service.
 *
 * @note This action is not executable if the series already exists in the SeriesDB.
 *
 * @section Slots Slots
 * - \b checkAddedSeries(data::SeriesDB::ContainerType): make the action executable if the added series matches
 * the series we want to export.
 * - \b checkAddedSeries(data::SeriesDB::ContainerType): make the action inexecutable if the added series matches
 * the series we want to export.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::sight::modules::ui::base::series::SExport" autoConnect="yes">
            <inout key="series" uid="..." />
            <inout key="seriesDB" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b series [sight::data::Series]: Source series to export.
 * - \b seriesDB [sight::data::SeriesDB]: Target series database where the series should be exported.
 */
class MODULE_UI_BASE_CLASS_API SExport : public ::sight::ui::base::IAction
{
public:
    fwCoreServiceMacro(SExport, ::sight::ui::base::IAction)

    /// Constructor
    MODULE_UI_BASE_API SExport();

    /// Destructor
    MODULE_UI_BASE_API virtual ~SExport() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection.
     *
     * Connect data::SeriesDB::s_ADDED_SERIES_SIG to this::s_CHECK_ADDED_SERIES_SLOT.
     * Connect data::SeriesDB::s_REMOVED_SERIES_SIG to this::s_CHECK_REMOVED_SERIES_SLOT.
     */
    MODULE_UI_BASE_API virtual KeyConnectionsMap getAutoConnections() const override;

protected:

    /// This method is used to configure the service parameters
    virtual void configuring() override;

    /// Starts service. If series associated with m_seriesId exists in SeriesDB, this action is not executable.
    virtual void starting() override;

    /// Stops service. Does nothing.
    virtual void stopping() override;

    /// Adds the series specified by m_seriesId in the SeriesDB.
    virtual void updating() override;

    virtual void info( std::ostream& _sstream ) override;

private:

    /**
     * @name Slots
     * @{
     */
    static const core::com::Slots::SlotKeyType s_CHECK_ADDED_SERIES_SLOT;
    static const core::com::Slots::SlotKeyType s_CHECK_REMOVED_SERIES_SLOT;

    /// Slot: check if specified series is added and set action not executable
    void checkAddedSeries(data::SeriesDB::ContainerType addedSeries);

    /// Slot: check if specified series is removed and set action executable
    void checkRemovedSeries(data::SeriesDB::ContainerType removedSeries);

    /**
     * @}
     */

    /// Returns current series given by its fwID m_seriesId.
    data::Series::sptr getSeries();

    /// fwID of the series to add in SeriesDB
    /// @deprecated appXml
    std::string m_seriesId;
};
} // namespace series
} // namespace sight::modules::ui::base
