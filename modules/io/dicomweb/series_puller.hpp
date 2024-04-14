/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <io/__/service/reader.hpp>
#include <io/http/client_qt.hpp>

#include <service/controller.hpp>

#include <filesystem>

namespace sight::data
{

class dicom_series;

} // namespace sight::data

namespace sight::module::io::dicomweb
{

/**
 * @brief   This service is used to pull series from a PACS (Orthanc).
 *

 * @section Slots Slots
 * - \b displayErrorMessage(const std::string&) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dicomweb::series_puller">
            <in key="selectedSeries" uid="..." />
            <inout key="seriesSet" uid="..." />
            <config dicomReader="sight::module::io::dicom::series_set_reader" readerConfig="config" />
            <server>%SERVER_HOSTNAME%:%SERVER_PORT%</server>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [sight::data::vector]: List of DICOM series to pull from the PACS..
 * @subsection In-Out In-Out:
 * - \b seriesSet [sight::data::series_set]: series_set where to put the retrieved dicom series.
 * @subsection Configuration Configuration:
 * - \b readerConfig Optional configuration for the DICOM Reader.
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service are from the preference settings.
 */

class series_puller : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(series_puller, sight::service::controller);

    using dicom_series_container_t = data::series_set::container_t;
    using instance_uid_container_t = std::vector<std::string>;
    using instance_count_map_t     = std::map<std::string, unsigned int>;
    using dicom_series_map_t       = std::map<std::string, std::weak_ptr<data::dicom_series> >;

    /**
     * @brief Constructor
     */
    series_puller() noexcept;

    /**
     * @brief Destructor
     */
    ~series_puller() noexcept override;

protected:

    /// Gets the configuration.
    void configuring() override;

    /// Registers the DICOM reader.
    void starting() override;

    /// Stops the DICOM reader.
    void stopping() override;

    /// Checks the configuration and pull the series.
    void updating() override;

private:

    /// Pull the Series from the Pacs.
    void pull_series();

    /**
     * @brief Read local series.
     * @param[in] _selected_series Series to read
     */
    void read_local_series(dicom_series_container_t _selected_series);

    /**
     * @brief Display an error message.
     * @param[in] _message Error message to display
     */
    static void display_error_message(const std::string& _message);

    /// Http Qt Client
    sight::io::http::client_qt m_client_qt;

    /// Reader
    sight::io::service::reader::sptr m_dicom_reader;

    /// Reader config
    std::string m_dicom_reader_srv_config;

    /// DicomWeb Reader
    std::string m_dicom_reader_type;

    /// Temporary series_set
    data::series_set::sptr m_tmp_series_set;

    /// Local Series
    instance_uid_container_t m_local_series;

    /// Is pulling is set to true when we are pulling series
    bool m_is_pulling {false};

    /// Index of the series being downloaded
    unsigned int m_series_index {0};

    /// Total number of instances that must be downloaded
    std::size_t m_instance_count {};

    /// Map of Dicom series being pulled
    dicom_series_map_t m_pulling_dicom_series_map;

    /// Server hostname preference key
    std::string m_server_hostname_key;

    /// Server port preference key
    std::string m_server_port_key;

    /// Server hostname
    std::string m_server_hostname {"localhost"};

    /// Server port
    int m_server_port {4242};

    /// DICOM Folder path
    std::filesystem::path m_path;

    sight::data::ptr<sight::data::vector, sight::data::access::in> m_selected_series {this, "selectedSeries"};
    sight::data::ptr<sight::data::series_set, sight::data::access::inout> m_series_set {this, "seriesSet"};
};

} // namespace sight::module::io::dicomweb
