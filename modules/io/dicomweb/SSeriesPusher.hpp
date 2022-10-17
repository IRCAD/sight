/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include "modules/io/dicomweb/config.hpp"

#include <core/memory/BufferObject.hpp>

#include <data/Vector.hpp>

#include <io/http/ClientQt.hpp>

#include <service/IController.hpp>

#include <vector>

namespace sight::data
{

class Series;

} // namespace sight::data

namespace sight::module::io::dicomweb
{

/**
 * @brief   This service is used to push a DICOM series to a PACS.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::io::dicomweb::SSeriesPusher">
            <in key="selectedSeries" uid="..." />
            <server>%PACS_SERVER_HOSTNAME%:%PACS_SERVER_PORT%</server>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [sight::data::Vector]: List of DICOM series to push to the PACS.
 * @subsection Configuration Configuration:
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service are from the preference settings.
 */
class MODULE_IO_DICOMWEB_CLASS_API SSeriesPusher : public service::IController
{
public:

    SIGHT_DECLARE_SERVICE(SSeriesPusher, sight::service::IController);

    typedef std::vector<CSPTR(data::Series)> DicomSeriesContainerType;

    MODULE_IO_DICOMWEB_API static const core::com::Slots::SlotKeyType s_DISPLAY_SLOT;
    typedef core::com::Slot<void (const std::string&, bool)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    MODULE_IO_DICOMWEB_API SSeriesPusher() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_IO_DICOMWEB_API ~SSeriesPusher() noexcept override;

protected:

    /// Gets the configuration.
    MODULE_IO_DICOMWEB_API void configuring() override;

    /// Does nothing.
    MODULE_IO_DICOMWEB_API void starting() override;

    /// Does nothing.
    MODULE_IO_DICOMWEB_API void stopping() override;

    /// Checks the configuration and push the series.
    MODULE_IO_DICOMWEB_API void updating() override;

private:

    /// Push Series
    void pushSeries();

    /**
     * @brief Display a message
     * @param[in] message Message to display
     * @param[in] error True if the message is an error messsage
     */
    static void displayMessage(const std::string& message, bool error);

    /// Http Qt Client
    sight::io::http::ClientQt m_clientQt;

    /// Set to true when pushing series
    bool m_isPushing {false};

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname {"localhost"};

    /// Server port
    int m_serverPort {4242};

    sight::data::ptr<sight::data::Vector, sight::data::Access::in> m_selectedSeries {this, "selectedSeries"};
};

} // namespace sight::module::io::dicomweb
