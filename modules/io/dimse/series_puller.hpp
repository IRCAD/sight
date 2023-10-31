/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/io/dimse/config.hpp"

#include <data/dicom_series.hpp>
#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <io/__/service/reader.hpp>
#include <io/dimse/data/pacs_configuration.hpp>
#include <io/dimse/series_retriever.hpp>

#include <service/controller.hpp>
#include <service/has_services.hpp>
#include <service/notifier.hpp>

#include <vector>

namespace sight::module::io::dimse
{

/**
 * @brief This service is used to pull series from a PACS.
 *
 * @section Signals Signals
 * - \b progressed(std::string): sent when the process start (bar id).
 * - \b progressStarted(std::string, float, std::string): sent when the process is updated (bar id,percentage,message).
 * - \b progressStopped(std::string): sent when the process ended (bar id).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service type="sight::module::io::dimse::series_puller">
        <in key="pacsConfig" uid="..." />
        <in key="selectedSeries" uid="..." />
        <inout key="seriesSet" uid="..." />
        <config dicomReader="sight::module::io::dicom::series_set_reader" readerConfig="config" />
    </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [sight::io::dimse::data::pacs_configuration]: PACS configuration data.
 * - \b selectedSeries [sight::data::vector]: list of DICOM series to pull from the PACS.
 *
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::series_set]: series set where to put the retrieved dicom series.
 *
 * @subsection Configuration Configuration:
 * - \b dicomReader (mandatory, string): reader type to use.
 * - \b readerConfig (optional, string, default=""): configuration for the DICOM Reader.
 */
class MODULE_IO_DIMSE_CLASS_API series_puller final : public service::controller,
                                                      public service::has_services,
                                                      private service::notifier
{
public:

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(series_puller, sight::service::controller);

    /// Creates the service and slots.
    MODULE_IO_DIMSE_API series_puller() noexcept;

    /// Destroys the service.
    MODULE_IO_DIMSE_API ~series_puller() noexcept override = default;

protected:

    /// Configures the service.
    MODULE_IO_DIMSE_API void configuring() override;

    /// Creates the DICOM reader.
    MODULE_IO_DIMSE_API void starting() override;

    /// Stops the DICOM reader.
    MODULE_IO_DIMSE_API void stopping() override;

    /// Pulls series.
    MODULE_IO_DIMSE_API void updating() override;

private:

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connects data::series_set::REMOVED_OBJECTS_SIG of s_SERIES_SET_INOUT to REMOVE_SERIES_SLOT (removeSeries)
     */
    MODULE_IO_DIMSE_API connections_t auto_connections() const override;

    using dicom_series_container_t  = data::series_set::container_t;
    using read_dicom_slot_t         = core::com::slot<void (dicom_series_container_t)>;
    using progress_started_signal_t = core::com::signal<void (std::string)>;
    using progressed_signal_t       = core::com::signal<void (std::string, float, std::string)>;
    using progress_stopped_signal_t = core::com::signal<void (std::string)>;

    /// Pulls series from the PACS.
    void pull_series();

    /**
     * @brief Reads local series.
     * @param _selectedSeries DICOM series that must be read.
     */
    void read_local_series(dicom_series_container_t _selected_series);

    /**
     * @brief Stores instance callback.
     * @param _seriesInstanceUID series instance UID.
     * @param _instanceNumber instance number.
     * @param _filePath file path.
     */
    void store_instance_callback(
        const std::string& _series_instance_uid,
        unsigned _instance_number,
        const std::string& _file_path
    );

    ///SLOT: removes series from m_localSeries, when deleted in a gui selector for instance.
    void remove_series(data::series_set::container_t _removed_series);

    /// Defines the worker of the series enquire thread.
    core::thread::worker::sptr m_request_worker;

    /// Defines the DICOM reader implementation.
    std::string m_dicom_reader_implementation;

    /// Contains the optional configuration to set to reader implementation.
    std::string m_reader_config;

    /// Contains the DICOM reader.
    sight::io::service::reader::sptr m_dicom_reader {nullptr};

    /// Contains the series_set where the DICOM reader sets its output.
    data::series_set::sptr m_series_set {nullptr};

    /// Contains the slot to call storeInstanceCallback method using C-MOVE requests.
    sight::io::dimse::series_retriever::progress_callback_slot_t::sptr m_slot_store_instance {nullptr};

    /// Contains the signal emitted when the progress bar is started.
    progress_started_signal_t::sptr m_sig_progress_started {nullptr};

    /// Contains the signal emitted when the progress bar is updated.
    progressed_signal_t::sptr m_sig_progressed {nullptr};

    /// Contains the signal emitted when the progress bar is stopped.
    progress_stopped_signal_t::sptr m_sig_progress_stopped {nullptr};

    /// Stores local series.
    std::set<std::string> m_local_series;

    /// Defines the progress bar ID.
    std::string m_progressbar_id {"pullDicomProgressBar"};

    /// Defines the total number of instances that must be downloaded.
    std::size_t m_instance_count {0};

    /// Stores a map of DICOM series being pulled.
    std::map<std::string, data::dicom_series::wptr> m_pulling_dicom_series_map;

    data::ptr<sight::io::dimse::data::pacs_configuration, data::access::in> m_config {this, "pacsConfig"};
    data::ptr<sight::data::vector, data::access::in> m_selected_series {this, "selectedSeries"};

    static constexpr std::string_view SERIES_SET_INOUT = "seriesSet";
    data::ptr<sight::data::series_set, data::access::inout> m_dest_series_set {this, SERIES_SET_INOUT};
};

} // namespace sight::module::io::dimse.
