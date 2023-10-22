/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "modules/data/config.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>
#include <core/com/slot.hpp>
#include <core/com/slots.hpp>

#include <data/series.hpp>
#include <data/series_set.hpp>

#include <service/controller.hpp>

namespace sight::module::data
{

/**
 * @brief This service emits a signal for each added series (the signal contains the added series)
 *
 * This action works on a data::series_set. It listens all the added series in the series_set and emits the signal.
 *
 * It is possible to configure the service to report only one type of series.
 *
 * @section Slots Slots
 * - \b reportSeries(data::series_set::container_type) : This slot is called when a series is added in the series_set.
 *   It emits a signal for each added series corresponding to the configured type.
 *
 * @section Signal Signal
 * - \b seriesAdded(data::series::sptr) : This signal is emitted when the added series correspond to the
 *   configured type.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="action_newActivity" type="sight::module::activity::action::series_signal" auto_connect="true" >
        <in key="seriesSet" uid="..." />
        <!-- Filter mode 'include' allows all given types.
             Filter mode 'exclude' allows all series excepted given ones. -->
        <filter>
            <mode>include</mode>
            <type>data::image_series</type>
            <type>data::model_series</type>
        </filter>
    </service>
        @endcode
 * @subsection Input Input
 * - \b seriesSet [sight::data::series_set]: series_set to listen to forward the added series signal.
 *
 * @subsection Configuration Configuration
 * - \b filter (optional): it allows to filter the series that can be notified.
 *    - \b mode: 'include' or 'exclude'. Defines if the series is notified (include) or not (exclude).
 *    - \b id: id of the activity
 */
class MODULE_DATA_CLASS_API series_signal : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(series_signal, service::controller);

    /// Constructor. Do nothing.
    MODULE_DATA_API series_signal() noexcept;

    /// Destructor. Do nothing.
    MODULE_DATA_API ~series_signal() noexcept override;

    MODULE_DATA_API static const core::com::slots::key_t REPORT_SERIES_SLOT;

    typedef core::com::signal<void (sight::data::series::sptr)> series_added_signal_t;
    MODULE_DATA_API static const core::com::signals::key_t SERIES_ADDED_SIG;

protected:

    /// Do nothing
    void starting() override;

    /// Do nothing
    void stopping() override;

    /// Do nothing
    void updating() override;

    /// Parse XML configuration
    void configuring() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::ADDED_OBJECTS_SIG to this::UPDATE_STATE_SLOT
     * Connect Vector::REMOVED_OBJECTS_SIG to this::UPDATE_STATE_SLOT
     */
    MODULE_DATA_API service::connections_t auto_connections() const override;

private:

    typedef std::vector<std::string> types_t;

    /**
     * @brief This slot is called when a series is added in the series_set.
     * It emits a signal for each added series corresponding to the configured type.
     */
    void reportSeriesSlot(sight::data::series_set::container_type _added_series);

    template<typename T>
    void reportSeries(const T& _added_series);

    /**
     * @brief Filter mode : include or exclude activity configurations.
     * @note Allowed values : 'include' or 'exclude'
     */
    std::string m_filterMode;

    /// Types of series to be enabled or disabled, according to filter mode.
    types_t m_types;

    /// Signal emitted when the added series correspond to the configured type.
    series_added_signal_t::sptr m_sigSeriesAdded;

    static constexpr std::string_view s_SERIES_SET_INPUT = "seriesSet";
    sight::data::ptr<sight::data::series_set, sight::data::Access::in> m_series_set {this, s_SERIES_SET_INPUT};
};

} // namespace sight::module::data
