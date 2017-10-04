/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOPACS_SSERIESPUSHER_HPP__
#define __IOPACS_SSERIESPUSHER_HPP__

#include "ioPacs/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>

#include <fwServices/IController.hpp>

#include <fwThread/Worker.hpp>

#include <vector>

namespace fwMedData
{
class Series;
}

namespace ioPacs
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
        <service type="::ioPacs::SSeriesPusher">
            <in key="pacsConfig" uid="..." />
            <in key="selectedSeries" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b pacsConfig [::fwPacsIO::data::PacsConfiguration]: PACS configuration data.
 * - \b selectedSeries [::fwData::Vector]: List of DICOM series to push to the PACS.
 */
class IOPACS_CLASS_API SSeriesPusher : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SSeriesPusher)( ::fwServices::IController ) );

    typedef std::vector< CSPTR(::fwMedData::Series) > DicomSeriesContainerType;
    typedef std::vector< ::boost::filesystem::path > DicomFileContainer;

    IOPACS_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_SLOT;
    typedef ::fwCom::Slot<void (const std::string&, bool)> DisplayMessageSlotType;

    /// Signal to start the progress (bar id)
    typedef ::fwCom::Signal< void ( std::string ) > StartedProgressSignalType;
    /// Signal to update the progress (bar id, percentage, message)
    typedef ::fwCom::Signal< void ( std::string, float, std::string ) > ProgressedSignalType;
    /// Signal to stop the progress (bar id)
    typedef ::fwCom::Signal< void ( std::string ) > StoppedProgressSignalType;

    /// Key in m_signals map of signal m_sigProgressed
    static const ::fwCom::Signals::SignalKeyType s_PROGRESSED_SIG;
    static const ::fwCom::Signals::SignalKeyType s_STARTED_PROGRESS_SIG;
    static const ::fwCom::Signals::SignalKeyType s_STOPPED_PROGRESS_SIG;

    /**
     * @brief Constructor
     */
    IOPACS_API SSeriesPusher() noexcept;

    /**
     * @brief Destructor
     */
    IOPACS_API virtual ~SSeriesPusher() noexcept;

protected:

    /// Does nothing.
    IOPACS_API virtual void configuring() override;

    /// Override
    IOPACS_API virtual void starting() override;

    /// Override
    IOPACS_API virtual void stopping() override;

    /// Override
    IOPACS_API void updating() override;

    /// Override
    IOPACS_API void info(std::ostream& _sstream) override;

protected:

    /**
     * @brief Check whether some series are already on the PACS
     * @return True if the series must be pushed
     */
    IOPACS_API bool checkSeriesOnPACS();

    /// Pull Series
    IOPACS_API void pushSeries();

    /**
     * @brief Display a message
     * @param[in] message Message to display
     * @param[in] error True if the message is an error messsage
     */
    IOPACS_API void displayMessage(const std::string& message, bool error) const;

    /**
     * @brief Progress callback
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @param[in] filePath File path
     */
    IOPACS_API void progressCallback(const std::string& seriesInstanceUID, unsigned int instanceNumber,
                                     const std::string& filePath);

    /// Slot to call displayMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slot to call progressCallback method
    ::fwPacsIO::SeriesEnquirer::ProgressCallbackSlotType::sptr m_slotProgressCallback;

    /// Signal emitted when the bar is progressing
    ProgressedSignalType::sptr m_sigProgressed;

    /// Signal emitted when the bar is starting
    StartedProgressSignalType::sptr m_sigStartedProgress;

    /// Signal emitted when the bar is stopping
    StoppedProgressSignalType::sptr m_sigStoppedProgress;

    /// Progress Bar ID
    std::string m_progressbarId;

    /// Series enquirer
    ::fwPacsIO::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Pacs Configuration object
    ::fwPacsIO::data::PacsConfiguration::csptr m_pacsConfiguration;

    /// Push Worker
    ::fwThread::Worker::sptr m_pushSeriesWorker;

    /// Is pushing is set to true when we are puishing series
    bool m_isPushing;

    /// Total number of instances that must be uploaded
    long unsigned int m_instanceCount;

};

} // namespace ioPacs

#endif // __IOPACS_SSERIESPUSHER_HPP__
