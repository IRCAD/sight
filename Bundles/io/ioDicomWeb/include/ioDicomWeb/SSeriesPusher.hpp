/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioDicomWeb/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwNetworkIO/http/ClientQt.hpp>

#include <fwServices/IController.hpp>

#include <fwThread/Worker.hpp>

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
       </service>
   @endcode
 * @subsection Input Input:
 * - \b selectedSeries [::fwData::Vector]: List of DICOM series to push to the PACS.
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

    /// Does nothing.
    IODICOMWEB_API virtual void configuring() override;

    /// Override
    IODICOMWEB_API virtual void starting() override;

    /// Override
    IODICOMWEB_API virtual void stopping() override;

    /// Override
    IODICOMWEB_API void updating() override;

private:

    /// Push Series
    IODICOMWEB_API void pushSeries();

    /**
     * @brief Display a message
     * @param[in] message Message to display
     * @param[in] error True if the message is an error messsage
     */
    IODICOMWEB_API void displayMessage(const std::string& message, bool error) const;

    /// Slot to call displayMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Http Qt Client
    ::fwNetworkIO::http::ClientQt m_clientQt;

    /// Push Worker
    ::fwThread::Worker::sptr m_pushSeriesWorker;

    /// Is pushing is set to true when we are puishing series
    bool m_isPushing;

    /// Total number of instances that must be uploaded
    long unsigned int m_instanceCount;

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
