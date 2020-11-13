/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ioPacs/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwIO/IReader.hpp>

#include <fwPacsIO/data/PacsConfiguration.hpp>
#include <fwPacsIO/SeriesEnquirer.hpp>

#include <fwThread/Worker.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QSlider>
#include <QWidget>

#include <filesystem>

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

namespace ioPacs
{

/**
 * @brief This editor service is used to select a slice index and pull the image from the pacs if it is not
 *        available on the local computer.
 * @deprecated it will be removed in sight 21.0, use ::ioPacs::SSliceIndexDicomEditor instead.
 *
 * @section Slots Slots
 * - \b readImage(size_t): read the given slice.
 * - \b displayErrorMessage(size_t): display an error message.
 *
 * @section XML XML Configuration
 * @code{.xml}
        <service type="::ioPacs::SSliceIndexDicomPullerEditor">
           <in key="pacsConfig" uid="..." />
           <inout key="series" uid="..." />
           <out key="image" uid="..." />
           <config dicomReader="::ioGdcm::SSeriesDBReader" delay="500">
               <dicomReaderConfig>
                   <!-- here goes the configuration for the dicom reader implementation -->
               </dicomReaderConfig>
           </config>
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b pacsConfig [::fwPacsIO::data::PacsConfiguration]: PACS configuration data.
 *
 * @subsection In-Out In-Out:
 * - \b series [::fwMedData::DicomSeries]: DICOM Series where to extract the images.
 *
 * @subsection Output Output:
 * - \b image [::fwData::Image]: downloaded image.
 *
 * @subsection Configuration Configuration:
 * - \b dicomReader (mandatory, string): reader type to use.
 * - \b dicomReaderConfig (optional, string): configuration for the DICOM Reader.
 * - \b delay (optional, unsigned int): delay to wait between each slice move.
 */
class IOPACS_CLASS_API SSliceIndexDicomPullerEditor final :
    public QObject,
    public ::fwGui::editor::IEditor
{

Q_OBJECT;

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SSliceIndexDicomPullerEditor,  ::fwGui::editor::IEditor)

    /// Creates slots.
    [[deprecated("Will be removed in sight 21.0, use ::ioPacs::SSliceIndexDicomEditor instead")]]
    IOPACS_API SSliceIndexDicomPullerEditor() noexcept;

    /// Destroys the service.
    IOPACS_API virtual ~SSliceIndexDicomPullerEditor() noexcept;

protected:

    /// Configures the service.
    IOPACS_API void configuring() override;

    /// Creates the DICOM reader and the slider.
    IOPACS_API void starting() override;

    /// Does nothing
    IOPACS_API void updating() override;

    /// Unregisters the DICOM reader and destroys the slider.
    IOPACS_API void stopping() override;

private Q_SLOTS:

    /// SLOT: Changes the slice index, called when the slider is moved.
    void changeSliceIndex(int);

private:

    /// Retrieves the local slice or pull it, then, read it.
    void triggerNewSlice();

    /**
     * @brief Reads a local slice.
     * @param _selectedSliceIndex index of the slice to read.
     */
    void readImage(std::size_t _selectedSliceIndex);

    /// Pulls the slice from the PACS.
    void pullInstance();

    /// Displays a dialog box with the error message.
    void displayErrorMessage(const std::string& _message) const;

    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

    /// Contains the slot to call displayErrorMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Contains the slider.
    QPointer< QSlider > m_sliceIndexSlider;

    /// Contains the slider informations.
    QPointer< QLineEdit > m_sliceIndexLineEdit;

    /// Defines the number of slices.
    std::size_t m_numberOfSlices;

    /// Defines the UID of the PACS configuration data.
    std::string m_pacsConfigurationUID;

    /// Contains the PACS configuration.
    ::fwPacsIO::data::PacsConfiguration::csptr m_pacsConfiguration;

    /// Defines the DICOM reader implementation.
    std::string m_dicomReaderType;

    /// Contains the DICOM reader.
    ::fwIO::IReader::wptr m_dicomReader;

    /// Defines the image key.
    std::string m_imageKey;

    /// Contains the seriesDB where the DICOM reader sets its output.
    SPTR(::fwMedData::SeriesDB) m_tempSeriesDB;

    /// Contains the worker that pulls series from the PACS.
    ::fwThread::Worker::sptr m_pullSeriesWorker;

    /// Contains the timer used to generate the new slice selection delay.
    SPTR(::fwThread::Timer) m_delayTimer2;

    /// Defines the delay to wait between each slice move.
    unsigned int m_delay { 500 };

    /// Contains the optional configuration to set to reader implementation.
    SPTR(::fwRuntime::ConfigurationElement) m_readerConfig;
};

} // namespace ioPacs
