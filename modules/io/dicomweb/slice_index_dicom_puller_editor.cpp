/************************************************************************
 *
 * Copyright (C) 2018-2024 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "slice_index_dicom_puller_editor.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/os/temp_path.hpp>
#include <core/thread/timer.hpp>

#include <data/array.hpp>
#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/series_set.hpp>

#include <io/__/service/reader.hpp>
#include <io/http/exceptions/base.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>
#include <ui/qt/container/widget.hpp>

#include <boost/lexical_cast.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>

#include <filesystem>
#include <fstream>
#include <iterator>

namespace sight::module::io::dicomweb
{

//------------------------------------------------------------------------------

slice_index_dicom_puller_editor::slice_index_dicom_puller_editor() noexcept =
    default;

//------------------------------------------------------------------------------

slice_index_dicom_puller_editor::~slice_index_dicom_puller_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::configuring()
{
    sight::ui::service::initialize();

    const auto& config = this->get_config();

    m_dicom_reader_type = config.get<std::string>("config.<xmlattr>.dicomReader", m_dicom_reader_type);
    m_delay             = config.get<unsigned int>("config.<xmlattr>.delay", m_delay);

    if(const auto reader_config = config.get_child_optional("readerConfig"); reader_config.has_value())
    {
        m_reader_config = reader_config.value();
    }

    if(m_delay_timer && m_delay_timer->is_running())
    {
        m_delay_timer->stop();
        m_delay_timer.reset();
    }

    m_delay_timer = this->worker()->create_timer();
    m_delay_timer->set_function(
        [this]()
        {
            this->trigger_new_slice();
        });

    m_delay_timer->set_one_shot(true);
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::starting()
{
    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* layout = new QHBoxLayout();

    const auto dicom_series = m_series.lock();
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
    QObject::connect(
        m_slice_index_slider,
        &QSlider::valueChanged,
        this,
        &slice_index_dicom_puller_editor::change_slice_index
    );

    // Create temporary series_set
    m_tmp_series_set = std::make_shared<data::series_set>();

    // Create reader
    auto dicom_reader = sight::service::add<sight::io::service::reader>(m_dicom_reader_type);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicom_reader_type + "\" in "
                                                                        "sight::module::io::dicomweb::slice_index_dicom_puller_editor.",
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
    if(m_delay_timer)
    {
        if(m_delay_timer->is_running())
        {
            m_delay_timer->stop();
        }

        m_delay_timer->set_duration(std::chrono::milliseconds(m_delay));
        m_delay_timer->start();
    }
    else
    {
        this->trigger_new_slice();
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::stopping()
{
    if(m_delay_timer && m_delay_timer->is_running())
    {
        m_delay_timer->stop();
    }

    // Stop dicom reader
    if(!m_dicom_reader.expired())
    {
        m_dicom_reader.lock()->stop();
        sight::service::remove(m_dicom_reader.lock());
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::updating()
{
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::change_slice_index(int /*unused*/)
{
    // Update text
    std::stringstream ss;
    ss << m_slice_index_slider->value() << " / " << (m_number_of_slices - 1);
    m_slice_index_line_edit->setText(std::string(ss.str()).c_str());

    // Get the new slice if there is no change for m_delay milliseconds
    if(m_delay_timer)
    {
        if(m_delay_timer->is_running())
        {
            m_delay_timer->stop();
        }

        m_delay_timer->start();
    }
    else
    {
        this->trigger_new_slice();
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::trigger_new_slice()
{
    // DicomSeries
    const auto dicom_series = m_series.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", dicom_series);

    // Compute slice index
    const std::size_t selected_slice_index = static_cast<std::size_t>(m_slice_index_slider->value())
                                             + dicom_series->get_first_instance_number();
    if(!dicom_series->is_instance_available(selected_slice_index))
    {
        this->pull_instance(*dicom_series);
    }
    else
    {
        this->read_image(*dicom_series, selected_slice_index);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::read_image(
    sight::data::dicom_series& _dicom_series,
    std::size_t _selected_slice_index
)
{
    // DicomSeries
    if(_dicom_series.get_dicom_type() != sight::data::series::dicom_t::image)
    {
        return;
    }

    // Creates unique temporary folder
    core::os::temp_dir tmp_dir;
    std::filesystem::path tmp_path = tmp_dir / "tmp";

    SIGHT_INFO("Create " + tmp_path.string());
    std::filesystem::create_directories(tmp_path);

    const auto& binaries = _dicom_series.get_dicom_container();
    auto iter            = binaries.find(_selected_slice_index);
    SIGHT_ASSERT("Index '" << _selected_slice_index << "' is not found in DicomSeries", iter != binaries.end());

    const core::memory::buffer_object::sptr buffer_obj = iter->second;
    const core::memory::buffer_object::lock_t locker_dest(buffer_obj);
    const char* buffer     = static_cast<char*>(locker_dest.buffer());
    const std::size_t size = buffer_obj->size();

    std::filesystem::path dest = tmp_path / std::to_string(_selected_slice_index);
    std::ofstream fs(dest, std::ios::binary | std::ios::trunc);
    SIGHT_THROW_IF("Can't open '" << tmp_path << "' for write.", !fs.good());

    fs.write(buffer, static_cast<std::streamsize>(size));
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
        image_series = std::dynamic_pointer_cast<data::image_series>(m_tmp_series_set->front());
    }

    if(image_series)
    {
        const data::image::size_t new_size = image_series->size();

        m_frontal_index->set_value(static_cast<int>(new_size[0] / 2));
        m_sagittal_index->set_value(static_cast<int>(new_size[1] / 2));

        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::orientation_t::axial,
            m_axial_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::orientation_t::axial,
            m_frontal_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image_series,
            data::helper::medical_image::orientation_t::axial,
            m_sagittal_index->value()
        );

        this->set_output(image_series, "image");
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::pull_instance(sight::data::dicom_series& _dicom_series)
{
    service::config_t configuration = this->get_config();
    //Parse server port and hostname
    if(configuration.count("server") != 0U)
    {
        const std::string server_info               = configuration.get("server", "");
        const std::string::size_type split_position = server_info.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", split_position != std::string::npos);

        m_server_hostname_key = server_info.substr(0, split_position);
        m_server_port_key     = server_info.substr(split_position + 1, server_info.size());
    }
    else
    {
        throw core::tools::failed("'server' element not found");
    }

    ui::preferences preferences;

    try
    {
        m_server_port = preferences.delimited_get(m_server_port_key, m_server_port);
    }
    catch(...)
    {
        // Do nothing
    }

    try
    {
        m_server_hostname = preferences.delimited_get(m_server_hostname_key, m_server_hostname);
    }
    catch(...)
    {
        // Do nothing
    }

    // Catch any errors
    try
    {
        // Get selected slice
        std::size_t selected_slice_index = static_cast<std::size_t>(m_slice_index_slider->value())
                                           + _dicom_series.get_first_instance_number();

        std::string series_instance_uid = _dicom_series.get_series_instance_uid();

        // Find Series according to SeriesInstanceUID
        QJsonObject query;
        query.insert("SeriesInstanceUID", series_instance_uid.c_str());

        QJsonObject body;
        body.insert("Level", "Series");
        body.insert("Query", query);
        body.insert("Limit", 0);

        /// Url PACS
        const std::string pacs_server("http://" + m_server_hostname + ":" + std::to_string(m_server_port));

        /// Orthanc "/tools/find" route. POST a JSON to get all Series corresponding to the SeriesInstanceUID.
        sight::io::http::request::sptr request = sight::io::http::request::New(
            pacs_server + "/tools/find"
        );
        QByteArray series_answer;
        try
        {
            series_answer = m_client_qt.post(request, QJsonDocument(body).toJson());
        }
        catch(sight::io::http::exceptions::host_not_found& exception)
        {
            std::stringstream ss;
            ss << "Host not found:\n"
            << " Please check your configuration: \n"
            << "Pacs host name: " << m_server_hostname << "\n"
            << "Pacs port: " << m_server_port << "\n";

            sight::module::io::dicomweb::slice_index_dicom_puller_editor::display_error_message(ss.str());
            SIGHT_WARN(exception.what());
        }
        QJsonDocument json_response    = QJsonDocument::fromJson(series_answer);
        const QJsonArray& series_array = json_response.array();

        // Should be one Series, so take the first of the array.
        const std::string series_uid(series_array.at(0).toString().toStdString());
        // GET all Instances by Series.
        const std::string instances_url(pacs_server + "/series/" + series_uid);

        const QByteArray& instances_answer = m_client_qt.get(sight::io::http::request::New(instances_url));
        json_response = QJsonDocument::fromJson(instances_answer);
        const QJsonObject& json_obj       = json_response.object();
        const QJsonArray& instances_array = json_obj["Instances"].toArray();
        const std::string& instance_uid   =
            instances_array.at(static_cast<int>(selected_slice_index)).toString().toStdString();

        // GET frame by Slice.
        std::string instance_path;
        const std::string instance_url(pacs_server + "/instances/" + instance_uid + "/file");
        try
        {
            instance_path = m_client_qt.get_file(sight::io::http::request::New(instance_url));
        }
        catch(sight::io::http::exceptions::content_not_found& exception)
        {
            std::stringstream ss;
            ss << "Content not found:  \n"
            << "Unable download the DICOM instance. \n";

            sight::module::io::dicomweb::slice_index_dicom_puller_editor::display_error_message(ss.str());
            SIGHT_WARN(exception.what());
        }

        // Add path and trigger reading
        _dicom_series.add_dicom_path(selected_slice_index, instance_path);
        this->read_image(_dicom_series, selected_slice_index);
    }
    catch(sight::io::http::exceptions::base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        sight::module::io::dicomweb::slice_index_dicom_puller_editor::display_error_message(ss.str());
        SIGHT_WARN(exception.what());
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::display_error_message(const std::string& _message)
{
    SIGHT_WARN("Error: " + _message);
    sight::ui::dialog::message message_box;
    message_box.set_title("Error");
    message_box.set_message(_message);
    message_box.set_icon(ui::dialog::message::critical);
    message_box.add_button(ui::dialog::message::ok);
    message_box.show();
}

} // namespace sight::module::io::dicomweb
