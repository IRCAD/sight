/************************************************************************
 *
 * Copyright (C) 2018-2026 IRCAD France
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

#include "detail/query.hpp"

#include <core/progress/observer.hpp>
#include <core/thread/timer.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>

#include <io/dicom/helper/series.hpp>
#include <io/dicom/reader/file.hpp>
#include <io/http/exceptions/base.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <qdebug.h>

#include <utility>

namespace sight::module::io::dicomweb
{

//-----------------------------------------------------------------------------

service::connections_t slice_index_dicom_puller_editor::auto_connections() const
{
    service::connections_t connections;
    connections.push(m_series, data::signals::MODIFIED, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::configuring()
{
    sight::ui::service::initialize();

    const auto& config = this->get_config();

    m_delay = config.get<unsigned int>("config.<xmlattr>.delay", m_delay);

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

    // Slider
    m_slice_index_slider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_slice_index_slider, 1);

    // Line Edit
    m_slice_index_line_edit = new QLineEdit();
    layout->addWidget(m_slice_index_line_edit, 0);
    m_slice_index_line_edit->setReadOnly(true);
    m_slice_index_line_edit->setMaximumWidth(80);
    m_slice_index_line_edit->setProperty("class", "lineEditDicomEditor");

    qt_container->set_layout(layout);

    // Connect the signals
    QObject::connect(
        m_slice_index_slider,
        &QSlider::valueChanged,
        this,
        &slice_index_dicom_puller_editor::change_slice_index
    );

    // image Indexes
    m_axial_index    = std::make_shared<data::integer>(0);
    m_frontal_index  = std::make_shared<data::integer>(0);
    m_sagittal_index = std::make_shared<data::integer>(0);

    this->updating();

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

    this->destroy();
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::updating()
{
    // Retrieve the DICOM series and its informations.
    const auto dicom_series        = m_series.lock();
    const std::size_t slice_number = static_cast<std::size_t>(dicom_series->get_instance_number().value());

    if(slice_number > 0)
    {
        // If the current slice index is the initial value of the slider, we just send a signal to trigger other
        // services.
        const int current_slice = m_slice_index_slider->value();
        if(std::cmp_equal(current_slice, slice_number / 2))
        {
            this->change_slice_index(current_slice);
        }
        else
        {
            // Fill slider informations.
            m_slice_index_slider->setRange(0, static_cast<int>(slice_number - 1));
            m_slice_index_slider->setValue(static_cast<int>(slice_number / 2));

            std::stringstream ss;
            ss << m_slice_index_slider->value() << " / " << (slice_number - 1);
            m_slice_index_line_edit->setText(std::string(ss.str()).c_str());
        }
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::change_slice_index(int /*unused*/)
{
    // Update text
    std::stringstream ss;
    ss << m_slice_index_slider->value() << " / " << (m_slice_index_slider->maximum());
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
    const auto series = m_series.lock();
    SIGHT_ASSERT("DicomSeries should not be null !", series);

    // Compute slice index
    const std::size_t selected_slice_index = static_cast<std::size_t>(m_slice_index_slider->value()) + 1;
    if(!sight::io::dicom::helper::series::is_instance_available(*series, selected_slice_index))
    {
        this->pull_instance(*series);
    }
    else
    {
        this->read_image(*series, selected_slice_index);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::read_image(sight::data::series& _series, std::size_t _selected_slice_index)
{
    // DicomSeries
    if(_series.get_dicom_type() != sight::data::series::dicom_t::image)
    {
        return;
    }

    auto reading_series = std::make_shared<data::series_set>();
    auto path           = sight::io::dicom::helper::series::get_path(_series, _selected_slice_index);
    auto reader         = std::make_shared<sight::io::dicom::reader::file>();
    reader->set_object(reading_series);
    reader->set_files({path.string()});

    auto observer = std::make_shared<sight::core::progress::observer>("Read slice");
    reader->read(observer);

    //Copy image
    if(!reading_series->empty())
    {
        const auto image_series = std::dynamic_pointer_cast<data::image_series>(reading_series->front());

        const auto image = m_image.lock();
        image->deep_copy(image_series);

        const data::image::size_t new_size = image_series->size();

        m_frontal_index->set_value(static_cast<int>(new_size[0] / 2));
        m_sagittal_index->set_value(static_cast<int>(new_size[1] / 2));

        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::axis_t::axial,
            m_axial_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::axis_t::axial,
            m_frontal_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::axis_t::axial,
            m_sagittal_index->value()
        );

        image->async_emit(data::signals::MODIFIED);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_puller_editor::pull_instance(sight::data::series& _series)
{
    try
    {
        std::size_t selected_slice_index = static_cast<std::size_t>(m_slice_index_slider->value()) + 1;
        std::string series_instance_uid  = _series.get_series_instance_uid();

        QJsonObject query;
        query.insert("SeriesInstanceUID", series_instance_uid.c_str());

        QJsonObject body;
        body.insert("Level", "Series");
        body.insert("Query", query);
        body.insert("Limit", 0);

        const std::string pacs_server("http://" + *m_server_hostname + ":" + std::to_string(*m_server_port));
        sight::io::http::request::sptr request = sight::io::http::request::make(pacs_server + "/tools/find");
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
            << "Pacs host name: " << *m_server_hostname << "\n"
            << "Pacs port: " << *m_server_port << "\n";
            slice_index_dicom_puller_editor::display_error_message(ss.str());
            SIGHT_WARN(exception.what());
        }
        QJsonDocument json_response    = QJsonDocument::fromJson(series_answer);
        const QJsonArray& series_array = json_response.array();

        // This the series ID on the server, which may be different from the SeriesInstanceUID
        const std::string series_server_id(series_array.at(0).toString().toStdString());
        const std::string instances_url(pacs_server + "/series/" + series_server_id);
        const QByteArray& instances_answer = m_client_qt.get(sight::io::http::request::make(instances_url));
        json_response = QJsonDocument::fromJson(instances_answer);
        const QJsonObject& json_obj       = json_response.object();
        const QJsonArray& instances_array = json_obj["Instances"].toArray();

        // This the instance ID on the server, which may be different from the InstanceUID
        const std::string& instance_server_id =
            instances_array.at(static_cast<int>(selected_slice_index)).toString().toStdString();

        // Retrieve the SOP Instance UID
        const auto instance_uid = detail::query_instance_uid(pacs_server, instance_server_id, m_client_qt);
        const auto path         = sight::io::dicom::helper::series::get_path(series_instance_uid, instance_uid);
        try
        {
            const std::string instance_file_url(pacs_server + "/instances/" + instance_server_id + "/file");
            m_client_qt.get_file(sight::io::http::request::make(instance_file_url), path);
        }
        catch(sight::io::http::exceptions::content_not_found& exception)
        {
            std::stringstream ss;
            ss << "Content not found:\n"
            << "Unable to download the DICOM instance.\n";
            slice_index_dicom_puller_editor::display_error_message(ss.str());
            SIGHT_WARN(exception.what());
        }

        sight::io::dicom::helper::series::add_instance(_series, selected_slice_index, instance_uid);
        this->read_image(_series, selected_slice_index);
    }
    catch(sight::io::http::exceptions::base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        slice_index_dicom_puller_editor::display_error_message(ss.str());
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
