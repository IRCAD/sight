/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/com/slot.hpp>

#include <data/DicomSeries.hpp>
#include <data/Image.hpp>

#include <io/__/service/reader.hpp>

#include <ui/__/editor.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QSlider>
#include <QWidget>

#include <filesystem>

namespace sight::data
{

class Composite;
class Integer;

} // namespace sight::data

namespace sight::core::thread
{

class Timer;

} // namespace sight::core::thread

namespace sight::data
{

class SeriesSet;

} // namespace sight::data

namespace sight::module::ui::dicom
{

/**
 * @brief   This editor service is used to select a slice index and read the corresponding image on the local computer.
 * *
 * @section Slots Slots
 * - \b readImage(std::size_t) : Read the given slice.
 * - \b displayErrorMessage(std::size_t) : display an error message.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::ui::dicom::SSliceIndexDicomEditor">
           <in key="series" uid="..." />
           <out key="image" uid="..." />
           <config dicomReader="sight::module::io::dicom::SSeriesSetReader" delay="500">
               <readerConfig> <!-- optional -->
                   <!-- here goes the configuration for the dicom reader implementation -->
               </readerConfig>
           </config>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b series [sight::data::DicomSeries]: Dicom Series where to extract the images.
 * @subsection Output Output:
 * - \b image [sight::data::Image]: Downloaded image.
 * @subsection Configuration Configuration:
 * - \b dicomReader Reader type to use.
 * - \b readerConfig Optional configuration for the DICOM Reader.
 */
class MODULE_UI_DICOM_CLASS_API SSliceIndexDicomEditor : public QObject,
                                                         public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(SSliceIndexDicomEditor, sight::ui::editor);

    MODULE_UI_DICOM_API static const core::com::slots::key_t READ_IMAGE_SLOT;
    typedef core::com::slot<void (std::size_t)> ReadImageSlotType;

    MODULE_UI_DICOM_API static const core::com::slots::key_t DISPLAY_MESSAGE_SLOT;
    typedef core::com::slot<void (const std::string&)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    MODULE_UI_DICOM_API SSliceIndexDicomEditor() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_UI_DICOM_API ~SSliceIndexDicomEditor() noexcept override;

protected:

    /// Configuring method. This method is used to configure the service.
    MODULE_UI_DICOM_API void configuring() override;

    /// Override
    MODULE_UI_DICOM_API void starting() override;

    /// Override
    MODULE_UI_DICOM_API void stopping() override;

    /// Override
    MODULE_UI_DICOM_API void updating() override;

    /// Override
    MODULE_UI_DICOM_API void info(std::ostream& _sstream) override;

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
    static MODULE_UI_DICOM_API void displayErrorMessage(const std::string& message);

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
    QPointer<QSlider> m_sliceIndexSlider;

    /// Slice index line edit
    QPointer<QLineEdit> m_sliceIndexLineEdit;

    /// Number of instances
    std::size_t m_numberOfSlices {};

    /// IODICOM Reader
    std::string m_dicomReaderType;

    /// Reader
    sight::io::service::reader::wptr m_dicomReader;

    /// Temporary SeriesSet
    SPTR(data::SeriesSet) m_tmp_series_set;

    /// Axial slice index
    SPTR(data::Integer) m_axialIndex;
    /// Frontal slice index
    SPTR(data::Integer) m_frontalIndex;
    /// Sagittal slice index
    SPTR(data::Integer) m_sagittalIndex;

    /// Timer used to generate the new slice selection delay
    SPTR(core::thread::timer) m_delayTimer2;

    /// Delay
    std::size_t m_delay {500};

    /// Optional configuration to set to reader implementation
    service::config_t m_readerConfig;

    static constexpr std::string_view s_IMAGE = "image";

    data::ptr<data::DicomSeries, data::Access::in> m_dicomSeries {this, "series", true};
    data::ptr<data::Image, data::Access::out> m_image {this, s_IMAGE, false};
};

} // namespace sight::module::ui::dicom
