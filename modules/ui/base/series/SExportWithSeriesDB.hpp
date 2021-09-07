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

#include <core/com/Signal.hpp>
#include <core/com/Slot.hpp>

#include <data/Series.hpp>

#include <ui/base/IAction.hpp>

namespace sight::core::jobs
{

class IJob;

}

namespace sight::module::ui::base
{

namespace series
{

/**
 * @brief   This action allows to export the current series using a temporary seriesDB.
 *
 * This service must work on a series.
 * It creates a new SeriesDB containing this series, and then launchs an module::ui::base::editor::SSelector service
 * on the created SeriesDB with the given configuration.
 *
 * @section Signal Signal
 * - \b jobCreated(SPTR(core::jobs::IJob)) : This signal is emitted by the slot 'forwardJob' to forward job process
 *   between SSelector service (jobCreated signal) and other services.
 *
 * @section Slot Slot
 * - \b forwardJob(SPTR(core::jobs::IJob)) : This slot allows to forward job process between SSelector service
 *   and other services. It is connected to SSelector 'jobCreated' signal.
 *
 * @section XML XML Configuration
 *
 * The available reader can be configured
 * @code{.xml}
    <service uid="action_seriesExporter" impl="::sight::module::ui::base::series::SExportWithSeriesDB">
        <inout key="series" uid="..." />
        <IOSelectorSrvConfig name="seriesDBImporterConfig" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b series [sight::data::Series]: series to export using a SeriesDB writer
 *
 * @subsection Configuration Configuration
 * With :
 *  - \b IOSelectorSrvConfig : the id of a configuration for SSelector
 *  for example
 *  @code{.xml}
    <extension implements="::sight::service::extension::Config">
        <id>seriesDBExporterConfig</id>
        <service>module::ui::base::editor::SSelector</service>
        <desc>IOSelector config to export SeriesDB</desc>
        <config>
            <type mode="writer" />
            <selection mode="include" />
            <addSelection service="::sight::module::io::atoms::SWriter" />
        </config>
    </extension>
    @endcode
 */
class MODULE_UI_BASE_CLASS_API SExportWithSeriesDB : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SExportWithSeriesDB, sight::ui::base::IAction);

    /**
     * @name Signal/Slot typedefs
     * @{
     */
    typedef core::com::Signal<void (SPTR(core::jobs::IJob))> JobCreatedSignalType;
    typedef core::com::Slot<void (SPTR(core::jobs::IJob))> ForwardJobSlotType;
    /// @}

    MODULE_UI_BASE_API SExportWithSeriesDB() noexcept;

    MODULE_UI_BASE_API virtual ~SExportWithSeriesDB() noexcept;

protected:

    /**
     * @brief Configure action. Retrieves the IOSelector config.
     */
    void configuring() override;

    /// Start action.
    void starting() override;

    /// Show the SeriesDB reader selector, load the new SeriesDB and merge it the the current SeriesDB
    void updating() override;

    /// Stop action.
    void stopping() override;

    void info(std::ostream& _sstream) override;

private:

    /// SLOT: Allows to forward job process between io selector service and other services.
    void forwardJob(SPTR(core::jobs::IJob) iJob);

    std::string m_ioSelectorSrvConfig; ///< Configuration used for launched SSelector service

    SPTR(JobCreatedSignalType) m_sigJobCreated; ///< signal emitted to forward SSelector job process
    SPTR(ForwardJobSlotType) m_slotForwardJob;  ///< slot used to forward SSelector job process

    data::ptr<data::Series, data::Access::inout> m_series {this, "series"};
};

} // namespace series

} // namespace sight::module::ui::base
