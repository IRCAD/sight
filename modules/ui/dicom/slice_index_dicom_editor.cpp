/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "slice_index_dicom_editor.hpp"

#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/os/temp_path.hpp>
#include <core/thread/timer.hpp>

#include <data/array.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/series_set.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/lexical_cast.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace sight::module::ui::dicom
{

const core::com::slots::key_t slice_index_dicom_editor::READ_IMAGE_SLOT      = "readImage";
const core::com::slots::key_t slice_index_dicom_editor::DISPLAY_MESSAGE_SLOT = "displayErrorMessage";

//------------------------------------------------------------------------------

slice_index_dicom_editor::slice_index_dicom_editor() noexcept
{
    m_slot_read_image = new_slot(READ_IMAGE_SLOT, &slice_index_dicom_editor::read_image, this);
    new_slot(DISPLAY_MESSAGE_SLOT, &slice_index_dicom_editor::display_error_message);
}

//------------------------------------------------------------------------------

slice_index_dicom_editor::~slice_index_dicom_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void slice_index_dicom_editor::configuring()
{
    sight::ui::service::initialize();

    const auto& config = this->get_config();

    m_dicom_reader_type = config.get<std::string>("config.<xmlattr>.dicomReader", m_dicom_reader_type);
    m_delay             = config.get<std::size_t>("config.<xmlattr>.delay", m_delay);

    if(const auto reader_config = config.get_child_optional("readerConfig"); reader_config.has_value())
    {
        m_reader_config = reader_config.value();
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::starting()
{
    m_delay_timer2 = this->worker()->create_timer();

    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* layout = new QHBoxLayout();

    const auto dicom_series = m_dicom_series.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicom_series);
    m_number_of_slices = dicom_series->num_instances();

    // Slider
    m_slice_index_slider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_slice_index_slider, 1);
    m_slice_index_slider->setRange(0, static_cast<int>(m_number_of_slices - 1));
    m_slice_index_slider->setValue(static_cast<int>(m_number_of_slices / 2));

    // Line Edit
    m_slice_index_line_edit = new QLineEdit();
    layout->addWidget(m_slice_index_line_edit, 0);
    m_slice_index_line_edit->setReadOnly(true);
    m_slice_index_line_edit->setMaximumWidth(80);

    std::stringstream ss;
    ss << m_slice_index_slider->value() << " / " << (m_number_of_slices - 1);
    m_slice_index_line_edit->setText(std::string(ss.str()).c_str());

    qt_container->set_layout(layout);

    // Connect the signals
    QObject::connect(m_slice_index_slider, &QSlider::valueChanged, this, &self_t::change_slice_index);

    // Create temporary series_set
    m_tmp_series_set = std::make_shared<data::series_set>();

    // Create reader
    auto dicom_reader = sight::service::add<sight::io::service::reader>(m_dicom_reader_type);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicom_reader_type + "\" in "
                                                                        "sight::module::ui::dicom::slice_index_dicom_editor.",
        dicom_reader
    );
    dicom_reader->set_inout(m_tmp_series_set, sight::io::service::DATA_KEY);
    dicom_reader->set_config(m_reader_config);
    dicom_reader->configure();
    dicom_reader->start();

    m_dicom_reader = dicom_reader;

    // image Indexes
    m_axial_index    = std::make_shared<data::integer>(0);
    m_frontal_index  = std::make_shared<data::integer>(0);
    m_sagittal_index = std::make_shared<data::integer>(0);

    // Load a slice
    auto duration = std::chrono::milliseconds(m_delay);
    m_delay_timer2->set_function(
        [this]()
        {
            this->trigger_new_slice();
        });
    m_delay_timer2->set_duration(duration);
    m_delay_timer2->set_one_shot(true);

    this->trigger_new_slice();
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::stopping()
{
    // Stop dicom reader
    if(!m_dicom_reader.expired())
    {
        m_dicom_reader.lock()->stop();
        sight::service::remove(m_dicom_reader.lock());
    }

    // Disconnect the signals
    QObject::disconnect(m_slice_index_slider, &QSlider::valueChanged, this, &self_t::change_slice_index);

    this->destroy();
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::updating()
{
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::info(std::ostream& _sstream)
{
    _sstream << "slice_index_dicom_editor::info";
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::change_slice_index(int /*unused*/)
{
    // Update text
    std::stringstream ss;
    ss << m_slice_index_slider->value() << " / " << (m_number_of_slices - 1);
    m_slice_index_line_edit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    m_delay_timer2->start();
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::trigger_new_slice()
{
    // DicomSeries
    const auto dicom_series = m_dicom_series.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicom_series);

    // Compute slice index
    std::size_t selected_slice_index = static_cast<std::size_t>(m_slice_index_slider->value())
                                       + dicom_series->get_first_instance_number();

    SIGHT_ERROR_IF(
        "There is no instance available for selected slice index.",
        !dicom_series->is_instance_available(selected_slice_index)
    );

    if(dicom_series->is_instance_available(selected_slice_index))
    {
        m_slot_read_image->async_run(selected_slice_index);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::read_image(std::size_t _selected_slice_index)
{
    // DicomSeries
    const auto dicom_series = m_dicom_series.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicom_series);

    auto is_modality_supported = [](const data::series& _series)
                                 {
                                     return _series.get_dicom_type() == data::series::dicom_t::image;
                                 };

    if(!is_modality_supported(*dicom_series))
    {
        return;
    }

    // Clear temporary series_set
    const auto scoped_emitter = m_tmp_series_set->scoped_emit();
    m_tmp_series_set->clear();

    // Creates unique temporary folder
    core::os::temp_dir tmp_dir;
    std::filesystem::path tmp_path = tmp_dir / "tmp";

    SIGHT_INFO("Create " + tmp_path.string());
    std::filesystem::create_directories(tmp_path);

    const auto& binaries = dicom_series->get_dicom_container();
    auto iter            = binaries.find(_selected_slice_index);
    SIGHT_ASSERT("Index '" << _selected_slice_index << "' is not found in DicomSeries", iter != binaries.end());

    const core::memory::buffer_object::sptr buffer_obj = iter->second;
    const core::memory::buffer_object::lock_t locker_dest(buffer_obj);
    const char* buffer     = static_cast<char*>(locker_dest.buffer());
    const std::size_t size = buffer_obj->size();

    const std::filesystem::path dest = tmp_path / std::to_string(_selected_slice_index);
    std::ofstream fs(dest, std::ios::binary | std::ios::trunc);
    SIGHT_THROW_IF("Can't open '" << tmp_path << "' for write.", !fs.good());

    fs.write(buffer, static_cast<std::int64_t>(size));
    fs.close();

    // Read image
    m_dicom_reader.lock()->set_folder(tmp_path);
    if(!m_dicom_reader.expired())
    {
        m_dicom_reader.lock()->update();

        if(m_dicom_reader.expired() || m_dicom_reader.lock()->stopped())
        {
            return;
        }
    }
    else
    {
        return;
    }

    //Copy image
    data::image_series::sptr image_series;

    if(!m_tmp_series_set->empty())
    {
        auto series = m_tmp_series_set->front();
        if(is_modality_supported(*series))
        {
            image_series = std::dynamic_pointer_cast<data::image_series>(series);
        }
    }

    if(image_series)
    {
        const data::image::size_t new_size = image_series->size();

        m_frontal_index->set_value(static_cast<int>(new_size[0] / 2));
        m_sagittal_index->set_value(static_cast<int>(new_size[1] / 2));

        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::axis_t::axial,
            m_axial_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::axis_t::frontal,
            m_frontal_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::axis_t::sagittal,
            m_sagittal_index->value()
        );

        this->set_output(image_series, IMAGE);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::display_error_message(const std::string& _message)
{
    SIGHT_WARN("Error: " + _message);
    sight::ui::dialog::message message_box;
    message_box.set_title("Error");
    message_box.set_message(_message);
    message_box.set_icon(sight::ui::dialog::message::critical);
    message_box.add_button(sight::ui::dialog::message::ok);
    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::dicom
