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

#include <core/memory/buffer_object.hpp>

#include <data/vector.hpp>

#include <io/http/client_qt.hpp>

#include <service/controller.hpp>

#include <vector>

namespace sight::data
{

class series;

} // namespace sight::data

namespace sight::module::io::dicomweb
{

/**
 * @brief   This service is used to push a DICOM series to a PACS.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dicomweb::series_pusher">
            <in key="selectedSeries" uid="..." />
            <server>%PACS_SERVER_HOSTNAME%:%PACS_SERVER_PORT%</server>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [sight::data::vector]: List of DICOM series to push to the PACS.
 * @subsection Configuration Configuration:
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service are from the preference settings.
 */
class series_pusher : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(series_pusher, sight::service::controller);

    using dicom_series_container_t = std::vector<std::shared_ptr<const data::series> >;

    static const core::com::slots::key_t DISPLAY_SLOT;
    using display_message_slot_t = core::com::slot<void (const std::string&, bool)>;

    /**
     * @brief Constructor
     */
    series_pusher() noexcept;

    /**
     * @brief Destructor
     */
    ~series_pusher() noexcept override;

protected:

    /// Gets the configuration.
    void configuring() override;

    /// Does nothing.
    void starting() override;

    /// Does nothing.
    void stopping() override;

    /// Checks the configuration and push the series.
    void updating() override;

private:

    /// Push Series
    void push_series();

    /**
     * @brief Display a message
     * @param[in] _message Message to display
     * @param[in] _error True if the message is an error messsage
     */
    static void display_message(const std::string& _message, bool _error);

    /// Http Qt Client
    sight::io::http::client_qt m_client_qt;

    /// Set to true when pushing series
    bool m_is_pushing {false};

    /// Server hostname preference key
    std::string m_server_hostname_key;

    /// Server port preference key
    std::string m_server_port_key;

    /// Server hostname
    std::string m_server_hostname {"localhost"};

    /// Server port
    int m_server_port {4242};

    sight::data::ptr<sight::data::vector, sight::data::access::in> m_selected_series {this, "selectedSeries"};
};

} // namespace sight::module::io::dicomweb
