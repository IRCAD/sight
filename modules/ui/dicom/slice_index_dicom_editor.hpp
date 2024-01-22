/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <data/dicom_series.hpp>
#include <data/image.hpp>

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

class composite;
class integer;

} // namespace sight::data

namespace sight::core::thread
{

class Timer;

} // namespace sight::core::thread

namespace sight::data
{

class series_set;

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
        <service type="sight::module::ui::dicom::slice_index_dicom_editor">
           <in key="series" uid="..." />
           <out key="image" uid="..." />
           <config dicomReader="sight::module::io::dicom::series_set_reader" delay="500">
               <readerConfig> <!-- optional -->
                   <!-- here goes the configuration for the dicom reader implementation -->
               </readerConfig>
           </config>
       </service>
   @endcode
 * @subsection Input Input:
 * - \b series [sight::data::dicom_series]: Dicom Series where to extract the images.
 * @subsection Output Output:
 * - \b image [sight::data::image]: Downloaded image.
 * @subsection Configuration Configuration:
 * - \b dicomReader Reader type to use.
 * - \b readerConfig Optional configuration for the DICOM Reader.
 */
class MODULE_UI_DICOM_CLASS_API slice_index_dicom_editor : public QObject,
                                                           public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(slice_index_dicom_editor, sight::ui::editor);

    MODULE_UI_DICOM_API static const core::com::slots::key_t READ_IMAGE_SLOT;
    using read_image_slot_t = core::com::slot<void (std::size_t)>;

    MODULE_UI_DICOM_API static const core::com::slots::key_t DISPLAY_MESSAGE_SLOT;
    using display_message_slot_t = core::com::slot<void (const std::string&)>;

    /**
     * @brief Constructor
     */
    MODULE_UI_DICOM_API slice_index_dicom_editor() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_UI_DICOM_API ~slice_index_dicom_editor() noexcept override;

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
    MODULE_UI_DICOM_API void trigger_new_slice();

    /**
     * @brief Read the selected image
     * @param[in] _selected_slice_index Selected slice of the image that must be read
     */
    MODULE_UI_DICOM_API void read_image(std::size_t _selected_slice_index);

    /**
     * @brief Display error message
     * @param[in] _message Message that must be displayed
     */
    static MODULE_UI_DICOM_API void display_error_message(const std::string& _message);

private Q_SLOTS:

    /**
     * @brief Slot called when the slider is moved
     * @param[in] _value Slider value
     */
    MODULE_UI_DICOM_API void change_slice_index(int _value);

private:

    /// Slot to call readLocalSeries method
    read_image_slot_t::sptr m_slot_read_image;

    /// Slice index slider
    QPointer<QSlider> m_slice_index_slider;

    /// Slice index line edit
    QPointer<QLineEdit> m_slice_index_line_edit;

    /// Number of instances
    std::size_t m_number_of_slices {};

    /// IODICOM Reader
    std::string m_dicom_reader_type;

    /// Reader
    sight::io::service::reader::wptr m_dicom_reader;

    /// Temporary series_set
    SPTR(data::series_set) m_tmp_series_set;

    /// Axial slice index
    SPTR(data::integer) m_axial_index;
    /// Frontal slice index
    SPTR(data::integer) m_frontal_index;
    /// Sagittal slice index
    SPTR(data::integer) m_sagittal_index;

    /// Timer used to generate the new slice selection delay
    SPTR(core::thread::timer) m_delay_timer2;

    /// Delay
    std::size_t m_delay {500};

    /// Optional configuration to set to reader implementation
    service::config_t m_reader_config;

    static constexpr std::string_view IMAGE = "image";

    data::ptr<data::dicom_series, data::access::in> m_dicom_series {this, "series", true};
    data::ptr<data::image, data::access::out> m_image {this, IMAGE, false};
};

} // namespace sight::module::ui::dicom
