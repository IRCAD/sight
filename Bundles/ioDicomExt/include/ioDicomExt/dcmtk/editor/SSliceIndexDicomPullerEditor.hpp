/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_DCMTK_EDITOR_SSLICEINDEXDICOMPULLEREDITOR_HPP__
#define __IODICOMEXT_DCMTK_EDITOR_SSLICEINDEXDICOMPULLEREDITOR_HPP__

#include <boost/filesystem/path.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <QObject>
#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QSlider>

#include <gui/editor/IEditor.hpp>
#include <fwDicomIOExt/dcmtk/SeriesEnquirer.hpp>
#include <fwDicomIOExt/dcmtk/SeriesRetriever.hpp>
#include <fwDicomIOExt/data/PacsConfiguration.hpp>
#include <io/IReader.hpp>
#include <fwCom/Slot.hpp>

#include "ioDicomExt/config.hpp"

namespace fwData
{
class Composite;
class Integer;
}

namespace fwMedData
{
class SeriesDB;
}

namespace ioDicomExt
{

namespace dcmtk
{

namespace editor
{

/**
 * @brief   This editor service is used to select a slice index and pull the image from the pacs if it is not
 *           available on the local computer.
 * @class   SSliceIndexDicomPullerEditor
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SSliceIndexDicomPullerEditor : public QObject,
                                                          public ::gui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro ( (SSliceIndexDicomPullerEditor)( ::gui::editor::IEditor ) );

    IODICOMEXT_API static const ::fwCom::Slots::SlotKeyType s_READ_IMAGE_SLOT;
    typedef ::fwCom::Slot<void (unsigned int)> ReadImageSlotType;

    IODICOMEXT_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_MESSAGE_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    IODICOMEXT_API SSliceIndexDicomPullerEditor() throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~SSliceIndexDicomPullerEditor() throw();

private Q_SLOTS:
    /**
     * @brief Slot called when the slider is moved
     * @param[in] value Slider value
     */
    IODICOMEXT_API void changeSliceIndex(int value);

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @verbatim
       <service uid="sliderIndexDicomPullerEditor" type="::gui::editor::IEditor"
         impl="::ioDicomExt::dcmtk::editor::SSliceIndexDicomPullerEditor" autoConnect="yes">
         <config compositeUID="previewComposite" imageKey="image" dicomReader="::ioVtkGdcm::SSeriesDBReader"
            pacsConfigurationUID="pacsConfiguration" delay="500" />
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

    /// Function called when a new slice must be displayed
    IODICOMEXT_API void triggerNewSlice(const boost::system::error_code& e);

    /// Run the service used to delay the selection of the slice
    IODICOMEXT_API void runDelay();

    /**
     * @brief Read the selected image
     * @param[in] selectedSliceIndex Selected slice of the image that must be read
     */
    IODICOMEXT_API void readImage(unsigned int selectedSliceIndex);

    /// Pull the selected slice from the pacs
    IODICOMEXT_API void pullInstance();

    /**
     * @brief Update the seriesDB with the series retrieved from the pacs
     * @param[in] series Series which must be added to the SeriesDB
     */
    IODICOMEXT_API void displayErrorMessage(const std::string& message) const;

    /**
     * @brief Store instance callback
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @param[in] filePath File path
     */
    IODICOMEXT_API void storeInstanceCallback(
        const std::string& seriesInstanceUID, unsigned int instanceNumber, const std::string& filePath);

    /// Slot to call readLocalSeries method
    ReadImageSlotType::sptr m_slotReadImage;

    /// Slot to call displayErrorMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slice index slider
    QPointer< QSlider > m_sliceIndexSlider;

    /// Slice index line edit
    QPointer< QLineEdit > m_sliceIndexLineEdit;

    /// Number of instances
    unsigned int m_numberOfSlices;

    /// Pacs Configuration UID
    std::string m_pacsConfigurationUID;

    /// Pacs Configuration object
    ::fwDicomIOExt::data::PacsConfiguration::sptr m_pacsConfiguration;

    /// IODICOMEXT Reader
    std::string m_dicomReaderType;

    /// Reader
    ::io::IReader::sptr m_dicomReader;

    /// Image Key
    std::string m_imageKey;

    /// Composite UID
    std::string m_compositeUID;

    /// Composite
    SPTR(::fwData::Composite) m_composite;

    /// Temporary SeriesDB
    SPTR(::fwMedData::SeriesDB) m_tempSeriesDB;

    /// Axial slice index
    SPTR(fwData::Integer) m_axialIndex;
    /// Frontal slice index
    SPTR(::fwData::Integer) m_frontalIndex;
    /// Sagittal slice index
    SPTR(::fwData::Integer) m_sagittalIndex;

    /// Pull Worker
    ::fwThread::Worker::sptr m_pullSeriesWorker;

    /// Series enquirer
    ::fwDicomIOExt::dcmtk::SeriesEnquirer::sptr m_seriesEnquirer;

    /// Service used to create a delay before loading a new slice
    ::boost::asio::io_service m_delayService;

    /// Work used for the new slice selection delay
    ::boost::asio::io_service::work m_delayWork;

    /// Thread used to create the slice selection delay
    ::boost::thread m_delayThread;

    /// Timer used to generate the new slice selection delay
    ::boost::asio::deadline_timer m_delayTimer;

    /// Delay
    unsigned int m_delay;

};

} // namespace editor
} // namespace dcmtk
} // namespace ioDicomExt

#endif // __IODICOMEXT_DCMTK_EDITOR_SSLICEINDEXDICOMPULLEREDITOR_HPP__
