/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioDicomWeb/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwNetwork/http/ClientQt.hpp>

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
 * @section Signals Signals
 * - \b progressed(std::string) : Signal to start the progress (bar id).
 * - \b startedProgress(std::string, float, std::string) :  Signal to update the progress (bar id, percentage, message).
 * - \b stoppedProgress(std::string) : Signal to stop the progress (bar id).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicomWeb::SSeriesPusher">
            <in key="pacsConfig" uid="..." />
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
    typedef std::vector< ::boost::filesystem::path > DicomFileContainer;

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

    /// Override
    IODICOMWEB_API void info(std::ostream& _sstream) override;

protected:

    /**
     * @brief Check whether some series are already on the PACS
     * @return True if the series must be pushed
     */
    IODICOMWEB_API bool checkSeriesOnPACS();

    /// Pull Series
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
    ::fwNetwork::http::ClientQt m_clientQt;

    /// Push Worker
    ::fwThread::Worker::sptr m_pushSeriesWorker;

    /// Is pushing is set to true when we are puishing series
    bool m_isPushing;

    /// Total number of instances that must be uploaded
    long unsigned int m_instanceCount;

};

} // namespace ioDicomWeb
