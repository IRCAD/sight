/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_DCMTK_CONTROLLER_SSERIESPULLER_HPP__
#define __IODICOMEXT_DCMTK_CONTROLLER_SSERIESPULLER_HPP__

#include "ioDicomExt/config.hpp"

#include <fwServices/IController.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwDicomIOExt/dcmtk/SeriesEnquirer.hpp>
#include <fwDicomIOExt/dcmtk/SeriesRetriever.hpp>
#include <fwDicomIOExt/data/PacsConfiguration.hpp>
#include <fwDicomData/DicomSeries.hpp>

#include <fwCom/Slot.hpp>
#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>
#include <vector>

namespace ioDicomExt
{

namespace dcmtk
{

namespace controller
{

/**
 * @brief   This service is used to pull series from a pacs.
 * @class   SSeriesPuller
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SSeriesPuller : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SSeriesPuller)( ::fwServices::IController ) );

    typedef ::fwMedData::SeriesDB::ContainerType DicomSeriesContainerType;
    typedef std::vector< std::string > InstanceUIDContainerType;
    typedef std::map < std::string, unsigned int > InstanceCountMapType;
    typedef std::map < std::string, WPTR(::fwDicomData::DicomSeries) > DicomSeriesMapType;

    IODICOMEXT_API static const ::fwCom::Slots::SlotKeyType s_READ_SLOT;
    typedef ::fwCom::Slot<void (DicomSeriesContainerType)> ReadDicomSlotType;

    IODICOMEXT_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

    /// Type of signal m_sigProgressBar
    typedef ::fwCom::Signal< void ( ::ioDicomExt::common::data::ProgressMsg::sptr ) > ProgressBarSignalType;

    /// Key in m_signals map of signal m_sigProgressBar
    static const ::fwCom::Signals::SignalKeyType s_PROGRESSED_SIG;

    /**
     * @brief Constructor
     */
    IODICOMEXT_API SSeriesPuller() throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~SSeriesPuller() throw();

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @verbatim
       <service uid="pullSeriesController" impl="::ioDicomExt::dcmtk::controller::SSeriesPuller">
         <config pacsConfigurationUID="pacsConfiguration"
         dicomReader="::ioVtkGdcm::SSeriesDBReader"
         destinationSeriesDBUID="localSeriesDB" />
       </service>
       @endverbatim
     */
    IODICOMEXT_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream );

protected:

    /// Pull Series
    IODICOMEXT_API void pullSeries();

    /**
     * @brief Read local series
     * @param[in] selectedSeries Dicom Series that must be read
     */
    IODICOMEXT_API void readLocalSeries(DicomSeriesContainerType selectedSeries);

    /**
     * @brief Display an error message
     * @param[in] message Error message to display
     */
    IODICOMEXT_API void displayErrorMessage(const std::string& message) const;

    /**
     * @brief Store instance callback
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @param[in] filePath File path
     */
    IODICOMEXT_API void storeInstanceCallback(const std::string& seriesInstanceUID, unsigned int instanceNumber,
                                              const std::string& filePath);

    /**
     * @brief Process callback
     * @param[in] percentage Percentage
     * @param[in] error True if there is an error
     */
    IODICOMEXT_API void progressCallback(float percentage, bool error);

    /**
     * @brief Notify progress bar
     * @param[in] action Progress action
     * @param[in] msg Progress bar message
     * @param[in] percentage Percentage of the progress bar
     */
    IODICOMEXT_API void notifyProgressBar(const std::string &actionId, const std::string &msg = "",
                                          float percentage = 0) const;

    /// Slot to call readLocalSeries method
    ReadDicomSlotType::sptr m_slotReadLocalSeries;

    /// Slot to call displayErrorMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slot to call storeInstanceCallback method using C-MOVE Requests
    ::fwDicomIOExt::dcmtk::SeriesRetriever::StoreInstanceCallbackSlotType::sptr
    m_slotStoreInstanceCallbackUsingMoveRequests;

    /// Slot to call storeInstanceCallback method using C-GET Requests
    ::fwDicomIOExt::dcmtk::SeriesEnquirer::StoreInstanceCallbackSlotType::sptr
    m_slotStoreInstanceCallbackUsingGetRequests;

    /// Slot to call progressCallback method
    ::fwDicomIOExt::dcmtk::SeriesEnquirer::ProgressCallbackSlotType::sptr m_slotProgressCallback;

    /// Signal that emits CompositeMsg when the bar is progressing
    ProgressBarSignalType::sptr m_sigProgressBar;

    /// Series enquirer
    ::fwDicomIOExt::dcmtk::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Pacs Configuration UID
    std::string m_pacsConfigurationUID;

    /// Pacs Configuration object
    ::fwDicomIOExt::data::PacsConfiguration::sptr m_pacsConfiguration;

    /// Reader
    ::io::IReader::sptr m_dicomReader;

    /// IODICOMEXT Reader
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

    /// Map used to store the number of instances per series
    InstanceCountMapType m_instanceCountMap;

    /// Is pulling is set to true when we are pulling series
    bool m_isPulling;

    /// Progress Bar ID
    std::string m_progressbarId;

    /// Total number of downloaded series
    unsigned int m_seriesCount;

    /// Index of the series being downloaded
    unsigned int m_seriesIndex;

    /// Map of Dicom series being pulled
    DicomSeriesMapType m_pullingDicomSeriesMap;
};

} // namespace controller
} // namespace dcmtk
} // namespace ioDicomExt

#endif // __IODICOMEXT_DCMTK_CONTROLLER_SSERIESPULLER_HPP__
