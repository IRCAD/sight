/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
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

#include "modules/io/dicomweb/config.hpp"

#include <data/dicom_series.hpp>

#include <io/http/client_qt.hpp>

#include <ui/__/editor.hpp>

#include <QLineEdit>
#include <QSlider>

#include <filesystem>

namespace sight
{

namespace core::thread
{

class Timer;

} // namespace core::thread

namespace io::service
{

class reader;

} // namespace io::service

namespace data
{

class integer;
class series_set;

} // namespace data

} // namespace sight

namespace sight::module::io::dicomweb
{

class MODULE_IO_DICOMWEB_CLASS_API slice_index_dicom_puller_editor : public QObject,
                                                                     public sight::ui::editor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(slice_index_dicom_puller_editor, sight::ui::editor);

    /**
     * @brief Constructor
     */
    MODULE_IO_DICOMWEB_API slice_index_dicom_puller_editor() noexcept;

    /**
     * @brief Destructor
     */
    MODULE_IO_DICOMWEB_API ~slice_index_dicom_puller_editor() noexcept override;

private Q_SLOTS:

    /**
     * @brief Slot called when the slider is moved
     * @param[in] _value Slider value
     */
    void change_slice_index(int _value);

protected:

    /// Gets the configurations and creates a timer on a worker.
    MODULE_IO_DICOMWEB_API void configuring() override;

    /// Creates the widgets, connect the signal, register the DICOM reader and starts the callback timer.
    MODULE_IO_DICOMWEB_API void starting() override;

    /// Stops the timer, unregister the DICOM reader and destroy the created widgets.
    MODULE_IO_DICOMWEB_API void stopping() override;

    /// Does nothing.
    MODULE_IO_DICOMWEB_API void updating() override;

private:

    /// Function called when a new slice must be displayed.
    void trigger_new_slice();

    /**
     * @brief Read the selected image
     * @param[in] _selected_slice_index Selected slice of the image that must be read
     */
    void read_image(sight::data::dicom_series& _dicom_series, std::size_t _selected_slice_index);

    /// Pull the selected slice from the Pacs
    void pull_instance(sight::data::dicom_series& _dicom_series);

    /**
     * @brief Displays a dialog box with the error message
     */
    static void display_error_message(const std::string& _message);

    /// Slice index slider
    QPointer<QSlider> m_slice_index_slider;

    /// Slice index line edit
    QPointer<QLineEdit> m_slice_index_line_edit;

    /// Number of instances
    std::size_t m_number_of_slices {};

    /// IODICOMWEB Reader
    std::string m_dicom_reader_type;

    /// Reader
    WPTR(sight::io::service::reader) m_dicom_reader;

    /// image Key
    std::string m_image_key;

    /// Temporary series_set
    SPTR(data::series_set) m_tmp_series_set;

    /// Axial slice index
    SPTR(data::integer) m_axial_index;
    /// Frontal slice index
    SPTR(data::integer) m_frontal_index;
    /// Sagittal slice index
    SPTR(data::integer) m_sagittal_index;

    /// Series enquirer
    sight::io::http::client_qt m_client_qt;

    /// Timer used to generate the new slice selection delay
    SPTR(core::thread::timer) m_delay_timer;

    /// Delay
    unsigned int m_delay {500};

    /// Optional configuration to set to reader implementation
    service::config_t m_reader_config;

    /// Server hostname preference key
    std::string m_server_hostname_key;

    /// Server port preference key
    std::string m_server_port_key;

    /// Server hostname
    std::string m_server_hostname {"localhost"};

    /// Server port
    int m_server_port {4242};

    sight::data::ptr<sight::data::dicom_series, sight::data::access::inout> m_series {this, "series"};
};

} // namespace sight::module::io::dicomweb
