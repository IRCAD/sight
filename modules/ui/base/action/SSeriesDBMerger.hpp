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

#include <data/SeriesDB.hpp>

#include <ui/base/IActionSrv.hpp>

namespace sight::core::jobs
{
class IJob;
}

namespace sight::modules::ui::base
{
namespace action
{

/**
 * @brief This action allows to load a new SeriesDB and merge it with the current SeriesDB
 *
 * @section Slots Slots
 * - \b forwardJob(core::jobs::IJob::sptr) : Called to forward a job.
 *
 * @section Signals Signals
 * - \b jobCreated(core::jobs::IJob::sptr) : This signal is emitted when a job is created
 *
 * The available reader can be configured
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="action" type="::sight::modules::ui::base::action::SSeriesDBMerger">
        <inout key="seriesDB" uid="..." />
        <IOSelectorSrvConfig name="seriesDBImporterConfig" />
   </service>
   @endcode
 * With :
 * - \b seriesDBImporterConfig : the id of a configuration for SIOSelector
 * for example
 * @code{.xml}
    <extension implements="::sight::services::registry::ServiceConfig">
        <id>seriesDBImporterConfig</id>
        <service>::modules::ui::base::editor::SIOSelector</service>
        <desc>IOSelector config to import SeriesDB</desc>
        <config>
            <type mode="reader" />
            <selection mode="exclude" />
            <addSelection service="::modules::io::atoms::SReader" />
        </config>
    </extension>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b seriesDB [sight::data::SeriesDB]: the SeriesDB to merge.
 */
class MODULE_UI_BASE_CLASS_API SSeriesDBMerger : public ::sight::ui::base::IActionSrv
{
public:
    fwCoreServiceMacro(SSeriesDBMerger, sight::ui::base::IActionSrv)

    typedef core::com::Signal< void ( SPTR(core::jobs::IJob) ) > JobCreatedSignalType;
    typedef core::com::Slot< void ( SPTR(core::jobs::IJob) ) > ForwardJobSlotType;

    MODULE_UI_BASE_API SSeriesDBMerger() noexcept;

    MODULE_UI_BASE_API virtual ~SSeriesDBMerger() noexcept;

protected:

    /**
     * @brief Configure action. Retrieves the IOSelector config.
     *
     * @code{.xml}
        <service impl="::modules::ui::base::action::SSeriesDBMerger">
            <IOSelectorSrvConfig name="myConfig" />
        </service>
       @endcode
     * - \b IOSelectorSrvConfig: identifier of the ServiceConfig used for SIOSelector
     */
    void configuring() override;

    /// Start action.
    void starting() override;

    /// Show the SeriesDB reader selector, load the new SeriesDB and merge it the the current SeriesDB
    void updating() override;

    /// Stop action.
    void stopping() override;
    void info(std::ostream& _sstream ) override;

private:
    void forwardJob(SPTR(core::jobs::IJob) iJob);

    std::string m_ioSelectorSrvConfig;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
    SPTR(ForwardJobSlotType) m_slotForwardJob;

};

} // namespace action
} // namespace sight::modules::ui::base
