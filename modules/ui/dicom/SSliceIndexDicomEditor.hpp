/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "modules/ui/dicom/config.hpp"

#include <core/com/Slot.hpp>

#include <io/base/service/IReader.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QSlider>
#include <QWidget>

#include <ui/base/editor/IEditor.hpp>

#include <filesystem>

namespace sight::data
{
class Composite;
class Integer;
}

namespace thread
{
class Timer;
}

namespace sight::data
{
class SeriesDB;
}

namespace fwRuntime
{
struct ConfigurationElement;
}

namespace sight::modules::ui::dicom
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
        <service type="::sight::modules::ui::dicom::SSliceIndexDicomEditor">
           <in key="series" uid="..." />
           <out key="image" uid="..." />
           <config dicomReader="::modules::io::dicom::SSeriesDBReader" delay="500">
               <dicomReaderConfig> <!-- optional -->
                   <!-- here goes the configuration for the dicom reader implementation -->
               </dicomReaderConfig>
           </config>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b series [sight::data::DicomSeries]: Dicom Series where to extract the images.
 * @subsection Output Output:
 * - \b image [sight::data::Image]: Downloaded image.
 * @subsection Configuration Configuration:
 * - \b dicomReader Reader type to use.
 * - \b dicomReaderConfig Optional configuration for the DICOM Reader.
 */
class MODULE_UI_DICOM_CLASS_API SSliceIndexDicomEditor : public QObject,
                                                         public sight::ui::base::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceMacro(SSliceIndexDicomEditor,  sight::ui::base::editor::IEditor )

    MODULE_UI_DICOM_API static const core::com::Slots::SlotKeyType s_READ_IMAGE_SLOT;
    typedef core::com::Slot<void (std::size_t)> ReadImageSlotType;

    MODULE_UI_DICOM_API static const core::com::Slots::SlotKeyType s_DISPLAY_MESSAGE_SLOT;
    typedef core::com::Slot<void (const std::string&)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    MODULE_UI_DICOM_API SSliceIndexDicomEditor() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_UI_DICOM_API virtual ~SSliceIndexDicomEditor() noexcept;

protected:

    /// Configuring method. This method is used to configure the service.
    MODULE_UI_DICOM_API virtual void configuring() override;

    /// Override
    MODULE_UI_DICOM_API virtual void starting() override;

    /// Override
    MODULE_UI_DICOM_API virtual void stopping() override;

    /// Override
    MODULE_UI_DICOM_API void updating() override;

    /// Override
    MODULE_UI_DICOM_API void info(std::ostream& _sstream ) override;

    /// Function called when a new slice must be displayed
    MODULE_UI_DICOM_API void triggerNewSlice();

    /**
     * @brief Read the selected image
     * @param[in] selectedSliceIndex Selected slice of the image that must be read
     */
    MODULE_UI_DICOM_API void readImage(std::size_t selectedSliceIndex);

    /**
     * @brief Display error message
     * @param[in] message Message that must be displayed
     */
    MODULE_UI_DICOM_API void displayErrorMessage(const std::string& message) const;

private Q_SLOTS:
    /**
     * @brief Slot called when the slider is moved
     * @param[in] value Slider value
     */
    MODULE_UI_DICOM_API void changeSliceIndex(int value);

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
    sight::io::base::service::IReader::wptr m_dicomReader;

    /// Temporary SeriesDB
    SPTR(data::SeriesDB) m_tempSeriesDB;

    /// Axial slice index
    SPTR(data::Integer) m_axialIndex;
    /// Frontal slice index
    SPTR(data::Integer) m_frontalIndex;
    /// Sagittal slice index
    SPTR(data::Integer) m_sagittalIndex;

    /// Timer used to generate the new slice selection delay
    SPTR(core::thread::Timer) m_delayTimer2;

    /// Delay
    std::size_t m_delay;

    /// Optional configuration to set to reader implementation
    SPTR(core::runtime::ConfigurationElement) m_readerConfig;
};

} // namespace sight::modules::ui::dicom
