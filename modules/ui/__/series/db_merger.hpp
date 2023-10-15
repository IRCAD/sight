/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/ui/__/config.hpp"

#include <core/com/signal.hpp>
#include <core/com/slot.hpp>

#include <data/series_set.hpp>

#include <ui/__/action.hpp>

namespace sight::core::jobs
{

class base;

} // namespace sight::core::jobs

namespace sight::module::ui::series
{

/**
 * @brief This action allows to load a new series_set and merge it with the current series_set
 *
 * @section Slots Slots
 * - \b forwardJob(core::jobs::base::sptr) : Called to forward a job.
 *
 * @section Signals Signals
 * - \b jobCreated(core::jobs::base::sptr) : This signal is emitted when a job is created
 *
 * The available reader can be configured
 * @section XML XML Configuration
 * @code{.xml}
   <service uid="action" type="sight::module::ui::series::db_merger">
        <inout key="seriesSet" uid="..." />
        <IOSelectorSrvConfig name="seriesSetImporterConfig" />
   </service>
   @endcode
 * With :
 * - \b seriesSetImporterConfig : the id of a configuration for selector
 * for example
 * @code{.xml}
    <extension implements="sight::service::extension::config">
        <id>seriesSetImporterConfig</id>
        <service>module::ui::editor::selector</service>
        <desc>IOSelector config to import series_set</desc>
        <config>
            <type mode="reader" />
            <selection mode="exclude" />
            <addSelection service="sight::module::io::session::reader" />
        </config>
    </extension>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b seriesSet [sight::data::series_set]: the series_set to merge.
 */
class MODULE_UI_CLASS_API db_merger : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(db_merger, sight::ui::action);

    typedef core::com::signal<void (SPTR(core::jobs::base))> JobCreatedSignalType;
    typedef core::com::slot<void (SPTR(core::jobs::base))> ForwardJobSlotType;

    MODULE_UI_API db_merger() noexcept;

    MODULE_UI_API ~db_merger() noexcept override;

protected:

    /**
     * @brief Configure action. Retrieves the IOSelector config.
     *
     * @code{.xml}
        <service impl="sight::module::ui::series::db_merger">
            <IOSelectorSrvConfig name="myConfig" />
        </service>
       @endcode
     * - \b IOSelectorSrvConfig: identifier of the config used for selector
     */
    void configuring() override;

    /// Start action.
    void starting() override;

    /// Show the series_set reader selector, load the new series_set and merge it the the current series_set
    void updating() override;

    /// Stop action.
    void stopping() override;
    void info(std::ostream& _sstream) override;

private:

    void forwardJob(SPTR(core::jobs::base) job);

    std::string m_ioSelectorSrvConfig;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
    SPTR(ForwardJobSlotType) m_slotForwardJob;

    static constexpr std::string_view s_SERIES_SET = "seriesSet";

    data::ptr<data::series_set, data::Access::inout> m_series_set {this, s_SERIES_SET};
};

} // namespace sight::module::ui::series
