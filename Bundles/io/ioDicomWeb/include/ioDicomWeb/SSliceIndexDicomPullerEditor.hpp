/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
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

#include "ioDicomWeb/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <fwIO/IReader.hpp>

#include <fwNetworkIO/http/ClientQt.hpp>

#include <filesystem>
#include <QLineEdit>
#include <QSlider>

namespace fwData
{
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
 * - \b displayErrorMessage(const std::string&) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicomWeb::SSliceIndexDicomPullerEditor">
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

    fwCoreServiceMacro(SSliceIndexDicomPullerEditor,  ::fwGui::editor::IEditor );

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
    void changeSliceIndex(int value);

protected:

    /// Gets the configurations and creates a timer on a worker.
    IODICOMWEB_API virtual void configuring() override;

    /// Creates the wigdets, connect the signal, register the DICOM reader and starts the callback timer.
    IODICOMWEB_API virtual void starting() override;

    /// Stops the timer, unregister the DICOM reader and destroy the created widgets.
    IODICOMWEB_API virtual void stopping() override;

    /// Does nothing.
    IODICOMWEB_API void updating() override;

private:
    /// Function called when a new slice must be displayed.
    void triggerNewSlice();

    /**
     * @brief Read the selected image
     * @param[in] selectedSliceIndex Selected slice of the image that must be read
     */
    void readImage(size_t selectedSliceIndex);

    /// Pull the selected slice from the Pacs
    void pullInstance();

    /**
     * @brief Displays a dialog box with the error message
     */
    void displayErrorMessage(const std::string& message) const;

    /// Slice index slider
    QPointer< QSlider > m_sliceIndexSlider;

    /// Slice index line edit
    QPointer< QLineEdit > m_sliceIndexLineEdit;

    /// Number of instances
    size_t m_numberOfSlices;

    /// IODICOMWEB Reader
    std::string m_dicomReaderType;

    /// Reader
    ::fwIO::IReader::wptr m_dicomReader;

    /// Image Key
    std::string m_imageKey;

    /// Temporary SeriesDB
    SPTR(::fwMedData::SeriesDB) m_tempSeriesDB;

    /// Axial slice index
    SPTR(fwData::Integer) m_axialIndex;
    /// Frontal slice index
    SPTR(::fwData::Integer) m_frontalIndex;
    /// Sagittal slice index
    SPTR(::fwData::Integer) m_sagittalIndex;

    /// Series enquirer
    ::fwNetworkIO::http::ClientQt m_clientQt;

    /// Timer used to generate the new slice selection delay
    SPTR(::fwThread::Timer) m_delayTimer;

    /// Delay
    unsigned int m_delay;

    /// Optional configuration to set to reader implementation
    SPTR(::fwRuntime::ConfigurationElement) m_readerConfig;

    /// Server hostname preference key
    std::string m_serverHostnameKey;

    /// Server port preference key
    std::string m_serverPortKey;

    /// Server hostname
    std::string m_serverHostname{"localhost"};

    /// Server port
    int m_serverPort{4242};

};

} // namespace ioDicomWeb
