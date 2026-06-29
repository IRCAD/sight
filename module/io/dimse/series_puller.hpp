/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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
#include <data/vector.hpp>

#include <io/__/service/reader.hpp>
#include <io/dimse/data/pacs_configuration.hpp>
#include <io/dimse/series_retriever.hpp>

#include <service/controller.hpp>
#include <service/has_services.hpp>
#include <service/notifier.hpp>

namespace sight::module::io::dimse
{

/**
 * @brief This service is used to pull series from a PACS.
 *
 * @section Signals Signals
 * - \b progress_started(std::string, float, std::string): sent when the process is updated (bar id,percentage,message).
 * - \b progress_stopped(std::string): sent when the process ended (bar id).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::dimse::series_puller">
        <in key="pacsConfig" uid="..." />
        <in key="selectedSeries" uid="..." />
        <inout key="seriesSet" uid="..." />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [sight::io::dimse::data::pacs_configuration]: PACS configuration data.
 * - \b selectedSeries [sight::data::vector]: list of DICOM series to pull from the PACS.
 *
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::series_set]: series set where to put the retrieved dicom series.
 */
class series_puller final : public service::controller,
                            public service::has_services,
                            private service::notifier,
                            public core::progress::has_monitors
{
public:

    struct signals
    {
        using progress_started_t = core::com::signal<void ()>;
        using progress_stopped_t = core::com::signal<void ()>;
        static inline const signal_key_t STARTED_PROGRESS = "progress_started";
        static inline const signal_key_t STOPPED_PROGRESS = "progress_stopped";
    };

    struct slots
    {
        static inline const slot_key_t REMOVE_SERIES = "removeSeries";
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(series_puller, sight::service::controller);

    /// Creates the service and slots.
    series_puller() noexcept;

    /// Destroys the service.
    ~series_puller() noexcept override = default;

protected:

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connects data::series_set::signals::REMOVED_OBJECTS of s_SERIES_SET_INOUT to REMOVE_SERIES (removeSeries)
     */
    connections_t auto_connections() const override;

    /// Configures the service.
    void configuring() override;

    /// Creates the DICOM reader.
    void starting() override;

    /// Stops the DICOM reader.
    void stopping() override;

    /// Pulls series.
    void updating() override;

private:

    using dicom_series_container_t = data::series_set::container_t;
    using read_dicom_slot_t        = core::com::slot<void (dicom_series_container_t)>;

    /// Pulls series from the PACS.
    void pull_series();

    /**
     * @brief Reads local series.
     * @param _selected_series DICOM series that must be read.
     */
    void read_local_series(dicom_series_container_t _selected_series);

    /**
     * @brief Stores instance callback.
     * @param _series_instance_uid series instance UID.
     * @param _instance_number instance number.
     * @param _file_path file path.
     */
    void store_instance_callback(
        const std::string& _series_instance_uid,
        unsigned _instance_number,
        const std::string& _file_path
    );

    ///SLOT: removes series from m_localSeries, when deleted in a gui selector for instance.
    void remove_series(data::series_set::container_t _removed_series);

    /// Contains the series_set where the DICOM reader sets its output.
    data::series_set::sptr m_series_set {nullptr};

    /// Stores local series.
    std::set<std::string> m_local_series;

    /// Defines the total number of instances that must be downloaded.
    std::size_t m_instance_count {0};

    /// Stores a map of DICOM series being pulled.
    std::map<std::string, data::series::wptr> m_pulling_dicom_series_map;

    data::ptr<sight::io::dimse::data::pacs_configuration, data::access::in> m_config {this, "pacsConfig"};
    data::ptr<sight::data::vector, data::access::in> m_selected_series {this, "selectedSeries"};

    static constexpr std::string_view SERIES_SET_INOUT = "series_set";
    data::ptr<sight::data::series_set, data::access::inout> m_dest_series_set {this, SERIES_SET_INOUT};
};

} // namespace sight::module::io::dimse.
