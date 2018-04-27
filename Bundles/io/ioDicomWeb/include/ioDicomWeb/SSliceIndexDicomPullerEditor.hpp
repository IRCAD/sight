/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioDicomWeb/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwIO/IReader.hpp>

#include <fwNetworkIO/http/ClientQt.hpp>

#include <fwThread/Worker.hpp>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QSlider>
#include <QWidget>

namespace fwData
{
class Composite;
class Integer;
}

namespace fwThread
{
class Timer;
}

namespace fwMedData
{
class SeriesDB;
}

namespace fwRuntime
{
struct ConfigurationElement;
}

namespace ioDicomWeb
{

/**
 * @brief   This editor service is used to select a slice index and pull the image from the pacs if it is not
 *          available on the local computer.
 * *
 * @section Slots Slots
 * - \b readImage(size_t) : Read the given slice.
 * - \b displayErrorMessage(size_t) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicom::SSliceIndexDicomPullerEditor">
           <inout key="series" uid="..." />
           <out key="image" uid="..." />
           <server>%PACS_SERVER_HOSTNAME%:%PACS_SERVER_PORT%</server>

           <config dicomReader="::ioGdcm::SSeriesDBReader">
               <dicomReaderConfig>
                   <config>
                       <showLogDialog>no</showLogDialog>
                       <enableBufferRotation>no</enableBufferRotation>
                       <dicomdirSupport>never</dicomdirSupport>
                   </config>
               </dicomReaderConfig>
           </config>
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b series [::fwMedData::DicomSeries]: Dicom Series where to extract the images.
 * @subsection Output Output:
 * - \b image [::fwData::Image]: Downloaded image.
 * @subsection Configuration Configuration:
 * - \b dicomReader Reader type to use.
 * - \b server: server URL. Need hostname and port in this format addr:port (default value is 127.0.0.1:8042).
 * @note : hostname and port of this service can be a value or a nameKey from preference settings
 *  (for example <server>%HOSTNAME%:%PORT%</server>)
 */
class IODICOMWEB_CLASS_API SSliceIndexDicomPullerEditor : public QObject,
                                                          public ::fwGui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro( (SSliceIndexDicomPullerEditor)( ::fwGui::editor::IEditor ) );

    IODICOMWEB_API static const ::fwCom::Slots::SlotKeyType s_READ_IMAGE_SLOT;
    typedef ::fwCom::Slot<void (std::size_t)> ReadImageSlotType;

    IODICOMWEB_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_MESSAGE_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    IODICOMWEB_API SSliceIndexDicomPullerEditor() noexcept;

    /**
     * @brief Destructor
     */
    IODICOMWEB_API virtual ~SSliceIndexDicomPullerEditor() noexcept;

private Q_SLOTS:
    /**
     * @brief Slot called when the slider is moved
     * @param[in] value Slider value
     */
    IODICOMWEB_API void changeSliceIndex(int value);

protected:

    /// Configuring method. This method is used to configure the service.
    IODICOMWEB_API virtual void configuring() override;

    /// Override
    IODICOMWEB_API virtual void starting() override;

    /// Override
    IODICOMWEB_API virtual void stopping() override;

    /// Override
    IODICOMWEB_API void updating() override;

    /// Override
    IODICOMWEB_API void info(std::ostream& _sstream ) override;

    /// Function called when a new slice must be displayed
    IODICOMWEB_API void triggerNewSlice();

    /**
     * @brief Read the selected image
     * @param[in] selectedSliceIndex Selected slice of the image that must be read
     */
    IODICOMWEB_API void readImage(std::size_t selectedSliceIndex);

    /// Pull the selected slice from the pacs
    IODICOMWEB_API void pullInstance();

    /**
     * @brief Displays a dialog box with the error message
     */
    IODICOMWEB_API void displayErrorMessage(const std::string& message) const;

    /// Slot to call readLocalSeries method
    ReadImageSlotType::sptr m_slotReadImage;

    /// Slot to call displayErrorMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slice index slider
    QPointer< QSlider > m_sliceIndexSlider;

    /// Slice index line edit
    QPointer< QLineEdit > m_sliceIndexLineEdit;

    /// Number of instances
    std::size_t m_numberOfSlices;

    /// IODICOMWEB Reader
    std::string m_dicomReaderType;

    /// Reader
    ::fwIO::IReader::wptr m_dicomReader;

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
    ::fwNetworkIO::http::ClientQt m_clientQt;

    /// Timer used to generate the new slice selection delay
    SPTR(::fwThread::Timer) m_delayTimer2;

    /// Delay
    unsigned int m_delay;

    /// Optional configuration to set to reader implementation
    SPTR(::fwRuntime::ConfigurationElement) m_readerConfig;

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
