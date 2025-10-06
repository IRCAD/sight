/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/progress/has_monitors.hpp>

#include <data/series_set.hpp>

#include <ui/__/action.hpp>

namespace sight::core::progress
{

class base;

} // namespace sight::core::progress

namespace sight::module::ui::series
{

/**
 * @brief This action allows to load a new series_set and merge it with the current series_set
 *
 * @section Slots Slots
 * - \b forward_monitor(core::progress::monitor::sptr) : Called to forward a monbitor.
 *
 * @section Signals Signals
 * - \b monitor_created(core::progress::monitor::sptr) : This signal is emitted when a monbitor is created
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
class db_merger : public sight::ui::action,
                  public sight::core::progress::has_monitors
{
public:

    SIGHT_DECLARE_SERVICE(db_merger, sight::ui::action);

    using forward_monitor_slot_t = core::com::slot<void (std::shared_ptr<core::progress::monitor>)>;

    db_merger() noexcept;
    ~db_merger() noexcept override = default;

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

    void forward_monitor(SPTR(core::progress::monitor) _monitor);

    std::string m_io_selector_srv_config;

    SPTR(forward_monitor_slot_t) m_slot_forward_monitor;
    static constexpr std::string_view SERIES_SET = "seriesSet";

    data::ptr<data::series_set, data::access::inout> m_series_set {this, SERIES_SET};
};

} // namespace sight::module::ui::series
