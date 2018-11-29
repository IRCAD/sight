/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "ioDicomWeb/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwNetworkIO/http/ClientQt.hpp>

#include <fwServices/IController.hpp>

#include <vector>

namespace fwMedData
{
class Series;
}

namespace ioDicomWeb
{

/**
 * @brief   This service is used to push a DICOM series to a PACS.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicomWeb::SSeriesPusher">
            <in key="selectedSeries" uid="..." />
            <server>%PACS_SERVER_HOSTNAME%:%PACS_SERVER_PORT%</server>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [::fwData::Vector]: List of DICOM series to push to the PACS.
 * @subsection Configuration Configuration:
 * - \b server : server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:4242).
 * @note : hostname and port of this service are from the preference settings.
 */
class IODICOMWEB_CLASS_API SSeriesPusher : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SSeriesPusher)( ::fwServices::IController ) );

    typedef std::vector< CSPTR(::fwMedData::Series) > DicomSeriesContainerType;

    IODICOMWEB_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_SLOT;
    typedef ::fwCom::Slot<void (const std::string&, bool)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    IODICOMWEB_API SSeriesPusher() noexcept;

    /**
     * @brief Destructor
     */
    IODICOMWEB_API virtual ~SSeriesPusher() noexcept;

protected:

    /// Gets the configuration.
    IODICOMWEB_API virtual void configuring() override;

    /// Does nothing.
    IODICOMWEB_API virtual void starting() override;

    /// Does nothing.
    IODICOMWEB_API virtual void stopping() override;

    /// Checks the configuration and push the series.
    IODICOMWEB_API void updating() override;

private:

    /// Push Series
    void pushSeries();

    /**
     * @brief Display a message
     * @param[in] message Message to display
     * @param[in] error True if the message is an error messsage
     */
    void displayMessage(const std::string& message, bool error) const;

    /// Http Qt Client
    ::fwNetworkIO::http::ClientQt m_clientQt;

    /// Set to true when pushing series
    bool m_isPushing;

    /// Returns a well formatted preferences key
    std::string getPreferenceKey(const std::string& key) const;

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname;

    /// Server port
    int m_serverPort;

};

} // namespace ioDicomWeb
