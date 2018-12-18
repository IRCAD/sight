/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ioDicom/config.hpp"

#include <fwCom/Slot.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwIO/IReader.hpp>

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

namespace ioDicom
{

/**
 * @brief   This editor service is used to select a slice index and read the corresponding image on the local computer.
 * *
 * @section Slots Slots
 * - \b readImage(size_t) : Read the given slice.
 * - \b displayErrorMessage(size_t) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::ioDicom::SSliceIndexDicomEditor">
           <in key="series" uid="..." />
           <out key="image" uid="..." />
           <config dicomReader="::ioGdcm::SSeriesDBReader" delay="500">
               <dicomReaderConfig> <!-- optional -->
                   <!-- here goes the configuration for the dicom reader implementation -->
               </dicomReaderConfig>
           </config>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b series [::fwMedData::DicomSeries]: Dicom Series where to extract the images.
 * @subsection Output Output:
 * - \b image [::fwData::Image]: Downloaded image.
 * @subsection Configuration Configuration:
 * - \b dicomReader Reader type to use.
 * - \b dicomReaderConfig Optional configuration for the DICOM Reader.
 */
class IODICOM_CLASS_API SSliceIndexDicomEditor : public QObject,
                                                 public ::fwGui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro( (SSliceIndexDicomEditor)( ::fwGui::editor::IEditor ) )

    IODICOM_API static const ::fwCom::Slots::SlotKeyType s_READ_IMAGE_SLOT;
    typedef ::fwCom::Slot<void (std::size_t)> ReadImageSlotType;

    IODICOM_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_MESSAGE_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    IODICOM_API SSliceIndexDicomEditor() noexcept;

    /**
     * @brief Destructor
     */
    IODICOM_API virtual ~SSliceIndexDicomEditor() noexcept;

protected:

    /// Configuring method. This method is used to configure the service.
    IODICOM_API virtual void configuring() override;

    /// Override
    IODICOM_API virtual void starting() override;

    /// Override
    IODICOM_API virtual void stopping() override;

    /// Override
    IODICOM_API void updating() override;

    /// Override
    IODICOM_API void info(std::ostream& _sstream ) override;

    /// Function called when a new slice must be displayed
    IODICOM_API void triggerNewSlice();

    /**
     * @brief Read the selected image
     * @param[in] selectedSliceIndex Selected slice of the image that must be read
     */
    IODICOM_API void readImage(std::size_t selectedSliceIndex);

    /**
     * @brief Display error message
     * @param[in] message Message that must be displayed
     */
    IODICOM_API void displayErrorMessage(const std::string& message) const;

private Q_SLOTS:
    /**
     * @brief Slot called when the slider is moved
     * @param[in] value Slider value
     */
    IODICOM_API void changeSliceIndex(int value);

private:

    /// Slot to call readLocalSeries method
    ReadImageSlotType::sptr m_slotReadImage;

    /// Slice index slider
    QPointer< QSlider > m_sliceIndexSlider;

    /// Slice index line edit
    QPointer< QLineEdit > m_sliceIndexLineEdit;

    /// Number of instances
    std::size_t m_numberOfSlices;

    /// IODICOM Reader
    std::string m_dicomReaderType;

    /// Reader
    ::fwIO::IReader::wptr m_dicomReader;

    /// Temporary SeriesDB
    SPTR(::fwMedData::SeriesDB) m_tempSeriesDB;

    /// Axial slice index
    SPTR(fwData::Integer) m_axialIndex;
    /// Frontal slice index
    SPTR(::fwData::Integer) m_frontalIndex;
    /// Sagittal slice index
    SPTR(::fwData::Integer) m_sagittalIndex;

    /// Timer used to generate the new slice selection delay
    SPTR(::fwThread::Timer) m_delayTimer2;

    /// Delay
    std::size_t m_delay;

    /// Optional configuration to set to reader implementation
    SPTR(::fwRuntime::ConfigurationElement) m_readerConfig;
};

} // namespace ioDicom
