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

#include <data/series.hpp>

#include <ui/__/action.hpp>

namespace sight::core::jobs
{

class base;

}

namespace sight::module::ui
{

namespace series
{

/**
 * @brief   This action allows to export the current series using a temporary series_set.
 *
 * This service must work on a series.
 * It creates a new series_set containing this series, and then launches a module::ui::editor::selector service
 * on the created series_set with the given configuration.
 *
 * @section Signal Signal
 * - \b jobCreated(SPTR(core::jobs::base)) : This signal is emitted by the slot 'forwardJob' to forward job process
 *   between selector service (jobCreated signal) and other services.
 *
 * @section Slot Slot
 * - \b forwardJob(SPTR(core::jobs::base)) : This slot allows to forward job process between selector service
 *   and other services. It is connected to selector 'jobCreated' signal.
 *
 * @section XML XML Configuration
 *
 * The available reader can be configured
 * @code{.xml}
    <service uid="action_seriesExporter" impl="sight::module::ui::series::export_with_series_set">
        <inout key="series" uid="..." />
        <IOSelectorSrvConfig name="seriesSetImporterConfig" />
    </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b series [sight::data::series]: series to export using a series_set writer
 *
 * @subsection Configuration Configuration
 * With :
 *  - \b IOSelectorSrvConfig : the id of a configuration for selector
 *  for example
 *  @code{.xml}
    <extension implements="sight::service::extension::config">
        <id>seriesSetExporterConfig</id>
        <service>module::ui::editor::selector</service>
        <desc>IOSelector config to export series_set</desc>
        <config>
            <type mode="writer" />
            <selection mode="include" />
            <addSelection service="sight::module::io::session::writer" />
        </config>
    </extension>
    @endcode
 */
class MODULE_UI_CLASS_API export_with_series_set : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(export_with_series_set, sight::ui::action);

    /**
     * @name Signal/Slot typedefs
     * @{
     */
    typedef core::com::signal<void (SPTR(core::jobs::base))> JobCreatedSignalType;
    typedef core::com::slot<void (SPTR(core::jobs::base))> ForwardJobSlotType;
    /// @}

    MODULE_UI_API export_with_series_set() noexcept;

    MODULE_UI_API ~export_with_series_set() noexcept override = default;

protected:

    /**
     * @brief Configure action. Retrieves the IOSelector config.
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

    /// SLOT: Allows to forward job process between io selector service and other services.
    void forwardJob(SPTR(core::jobs::base) job);

    std::string m_ioSelectorSrvConfig; ///< Configuration used for launched selector service

    SPTR(JobCreatedSignalType) m_sigJobCreated; ///< signal emitted to forward selector job process
    SPTR(ForwardJobSlotType) m_slotForwardJob;  ///< slot used to forward selector job process

    data::ptr<data::series, data::Access::inout> m_series {this, "series"};
};

} // namespace series

} // namespace sight::module::ui
