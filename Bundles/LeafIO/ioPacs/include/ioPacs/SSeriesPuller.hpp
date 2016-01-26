/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOPACS_SSERIESPULLER_HPP__
#define __IOPACS_SSERIESPULLER_HPP__

#include "ioPacs/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>
#include <fwPacsIO/SeriesRetriever.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwServices/IController.hpp>
#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <vector>

namespace fwDicomData
{
class DicomSeries;
}

namespace ioPacs
{

/**
 * @brief   This service is used to pull series from a pacs.
 * @class   SSeriesPuller
 * @date    2013.
 */
class IOPACS_CLASS_API SSeriesPuller : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SSeriesPuller)( ::fwServices::IController ) );

    typedef ::fwMedData::SeriesDB::ContainerType DicomSeriesContainerType;
    typedef std::vector< std::string > InstanceUIDContainerType;
    typedef std::map < std::string, unsigned int > InstanceCountMapType;
    typedef std::map < std::string, WPTR(::fwDicomData::DicomSeries) > DicomSeriesMapType;

    IOPACS_API static const ::fwCom::Slots::SlotKeyType s_READ_SLOT;
    typedef ::fwCom::Slot<void (DicomSeriesContainerType)> ReadDicomSlotType;

    IOPACS_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

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
    IOPACS_API SSeriesPuller() throw();

    /**
     * @brief Destructor
     */
    IOPACS_API virtual ~SSeriesPuller() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @verbatim
       <service uid="pullSeriesController" impl="::ioPacs::SSeriesPuller">
         <config pacsConfigurationUID="pacsConfiguration"
         dicomReader="::ioGdcm::SSeriesDBReader"
         dicomReaderSrvConfig="config"
         destinationSeriesDBUID="localSeriesDB" />
       </service>
       @endverbatim
     */
    IOPACS_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IOPACS_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOPACS_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOPACS_API void updating() throw(::fwTools::Failed);

    /// Override
    IOPACS_API void info(std::ostream &_sstream );

protected:

    /// Pull Series
    IOPACS_API void pullSeries();

    /**
     * @brief Read local series
     * @param[in] selectedSeries Dicom Series that must be read
     */
    IOPACS_API void readLocalSeries(DicomSeriesContainerType selectedSeries);

    /**
     * @brief Display an error message
     * @param[in] message Error message to display
     */
    IOPACS_API void displayErrorMessage(const std::string& message) const;

    /**
     * @brief Store instance callback
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @param[in] filePath File path
     */
    IOPACS_API void storeInstanceCallback(const std::string& seriesInstanceUID, unsigned int instanceNumber,
                                          const std::string& filePath);



    /// Slot to call readLocalSeries method
    ReadDicomSlotType::sptr m_slotReadLocalSeries;

    /// Slot to call displayErrorMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slot to call storeInstanceCallback method using C-MOVE Requests
    ::fwPacsIO::SeriesRetriever::ProgressCallbackSlotType::sptr m_slotStoreInstanceCallbackUsingMoveRequests;

    /// Slot to call storeInstanceCallback method using C-GET Requests
    ::fwPacsIO::SeriesEnquirer::ProgressCallbackSlotType::sptr m_slotStoreInstanceCallbackUsingGetRequests;

    /// Signal emitted when the bar is progressing
    ProgressedSignalType::sptr m_sigProgressed;

    /// Signal emitted when the bar is starting
    StartedProgressSignalType::sptr m_sigStartedProgress;

    /// Signal emitted when the bar is stopping
    StoppedProgressSignalType::sptr m_sigStoppedProgress;

    /// Series enquirer
    ::fwPacsIO::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Pacs Configuration UID
    std::string m_pacsConfigurationUID;

    /// Pacs Configuration object
    ::fwPacsIO::data::PacsConfiguration::sptr m_pacsConfiguration;

    /// Reader
    ::io::IReader::sptr m_dicomReader;

    /// Reader Config
    std::string m_dicomReaderSrvConfig;

    /// IOPACS Reader
    std::string m_dicomReaderType;

    /// Temporary SeriesDB
    ::fwMedData::SeriesDB::sptr m_tempSeriesDB;

    /// Destination SeriesDB UID
    std::string m_destinationSeriesDBUID;

    /// Destination SeriesDB
    ::fwMedData::SeriesDB::sptr m_destinationSeriesDB;

    /// Pull Worker
    ::fwThread::Worker::sptr m_pullSeriesWorker;

    /// Local Series
    InstanceUIDContainerType m_localSeries;

    /// Is pulling is set to true when we are pulling series
    bool m_isPulling;

    /// Progress Bar ID
    std::string m_progressbarId;

    /// Total number of downloaded series
    long unsigned int m_seriesCount;

    /// Index of the series being downloaded
    unsigned int m_seriesIndex;

    /// Total number of instances that must be downloaded
    std::size_t m_instanceCount;

    /// Map of Dicom series being pulled
    DicomSeriesMapType m_pullingDicomSeriesMap;
};

} // namespace ioPacs

#endif // __IOPACS_SSERIESPULLER_HPP__
