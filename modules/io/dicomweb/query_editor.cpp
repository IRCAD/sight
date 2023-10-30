/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "query_editor.hpp"

#include <data/dicom_series.hpp>

#include <io/http/helper/Series.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>
#include <ui/qt/container/widget.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <filesystem>

namespace sight::module::io::dicomweb
{

//------------------------------------------------------------------------------

query_editor::query_editor() noexcept =
    default;

//------------------------------------------------------------------------------

query_editor::~query_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void query_editor::configuring()
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

    sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void query_editor::starting()
{
    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    // Main Widget
    auto* layout = new QGridLayout();

    m_patient_name_line_edit    = new QLineEdit();
    m_patient_name_query_button = new QPushButton("Send");
    layout->addWidget(new QLabel("Patient name:"), 0, 0);
    layout->addWidget(m_patient_name_line_edit, 0, 1);
    layout->addWidget(m_patient_name_query_button, 0, 2);

    m_begin_study_date_edit = new QDateEdit();
    m_begin_study_date_edit->setDate(QDate::currentDate());
    m_begin_study_date_edit->setDisplayFormat("dd.MM.yyyy");
    m_end_study_date_edit = new QDateEdit();
    m_end_study_date_edit->setDate(QDate::currentDate());
    m_end_study_date_edit->setDisplayFormat("dd.MM.yyyy");
    m_study_date_query_button = new QPushButton("Send");
    auto* date_layout = new QHBoxLayout();
    layout->addWidget(new QLabel("Study date:"), 1, 0);
    layout->addLayout(date_layout, 1, 1);
    layout->addWidget(m_study_date_query_button, 1, 2);
    date_layout->addWidget(m_begin_study_date_edit);
    date_layout->addWidget(m_end_study_date_edit);

    //Set layout
    qt_container->set_layout(layout);

    // Connect the signals
    QObject::connect(m_patient_name_line_edit, SIGNAL(returnPressed()), this, SLOT(query_patient_name()));
    QObject::connect(m_patient_name_query_button, SIGNAL(clicked()), this, SLOT(query_patient_name()));
    QObject::connect(m_study_date_query_button, SIGNAL(clicked()), this, SLOT(query_study_date()));
    QObject::connect(m_begin_study_date_edit, SIGNAL(dateChanged(QDate)), this, SLOT(query_study_date()));
    QObject::connect(m_end_study_date_edit, SIGNAL(dateChanged(QDate)), this, SLOT(query_study_date()));
}

//------------------------------------------------------------------------------

void query_editor::stopping()
{
    // Disconnect the signals
    QObject::disconnect(m_patient_name_line_edit, SIGNAL(returnPressed()), this, SLOT(query_patient_name()));
    QObject::disconnect(m_patient_name_query_button, SIGNAL(clicked()), this, SLOT(query_patient_name()));
    QObject::disconnect(m_study_date_query_button, SIGNAL(clicked()), this, SLOT(query_study_date()));
    QObject::disconnect(m_begin_study_date_edit, SIGNAL(dateChanged(QDate)), this, SLOT(query_study_date()));
    QObject::disconnect(m_end_study_date_edit, SIGNAL(dateChanged(QDate)), this, SLOT(query_study_date()));

    this->destroy();
}

//------------------------------------------------------------------------------

void query_editor::updating()
{
}

//------------------------------------------------------------------------------

void query_editor::query_patient_name()
{
    try
    {
        ui::preferences preferences;
        m_server_port     = preferences.delimited_get(m_server_port_key, m_server_port);
        m_server_hostname = preferences.delimited_get(m_server_hostname_key, m_server_hostname);
    }
    catch(...)
    {
        // Do nothing
    }

    try
    {
        // Vector of all Series that will be retrieved.
        data::series_set::container_t all_series;

        // Find series according to patient's name
        QJsonObject query;
        query.insert("PatientName", m_patient_name_line_edit->text().toStdString().c_str());

        QJsonObject body;
        body.insert("Level", "Series");
        body.insert("Query", query);
        body.insert("Limit", 0);

        /// Url PACS
        const std::string pacs_server("http://" + m_server_hostname + ":" + std::to_string(m_server_port));

        /// Orthanc "/tools/find" route. POST a JSON to get all Series corresponding to the SeriesInstanceUID.
        auto request                    = sight::io::http::request::New(pacs_server + "/tools/find");
        const QByteArray& series_answer = m_client_qt.post(request, QJsonDocument(body).toJson());
        QJsonDocument json_response     = QJsonDocument::fromJson(series_answer);
        QJsonArray series_array         = json_response.array();
        const int series_array_size     = series_array.count();

        for(int i = 0 ; i < series_array_size ; ++i)
        {
            const std::string& series_uid = series_array.at(i).toString().toStdString();
            const std::string instances_list_url(pacs_server + "/series/" + series_uid);
            const QByteArray& instances_answer = m_client_qt.get(sight::io::http::request::New(instances_list_url));
            json_response = QJsonDocument::fromJson(instances_answer);
            const QJsonObject& json_obj      = json_response.object();
            const QJsonArray& instance_array = json_obj["Instances"].toArray();

            // Retrieve the first instance for the needed information
            const std::string& instance_uid = instance_array.at(0).toString().toStdString();
            const std::string instance_url(pacs_server + "/instances/" + instance_uid + "/simplified-tags");
            const QByteArray& instance = m_client_qt.get(sight::io::http::request::New(instance_url));

            QJsonObject series_json = QJsonDocument::fromJson(instance).object();
            series_json.insert("NumberOfSeriesRelatedInstances", instance_array.count());

            // Convert response to DicomSeries
            data::series_set::container_t series = sight::io::http::helper::series::to_fw_med_data(series_json);

            all_series.insert(std::end(all_series), std::begin(series), std::end(series));
            this->update_series_set(all_series);
        }
    }
    catch(sight::io::http::exceptions::base& exception)
    {
        sight::module::io::dicomweb::query_editor::display_error_message(exception.what());
    }
}

//------------------------------------------------------------------------------

void query_editor::query_study_date()
{
    try
    {
        ui::preferences preferences;
        m_server_port     = preferences.delimited_get(m_server_port_key, m_server_port);
        m_server_hostname = preferences.delimited_get(m_server_hostname_key, m_server_hostname);
    }
    catch(...)
    {
        // Do nothing
    }

    try
    {
        // Vector of all Series that will be retrieved.
        data::series_set::container_t all_series;

        // Find Studies according to their StudyDate
        QJsonObject query;
        const std::string& begin_date = m_begin_study_date_edit->date().toString("yyyyMMdd").toStdString();
        const std::string& end_date   = m_end_study_date_edit->date().toString("yyyyMMdd").toStdString();
        const std::string& date_range = begin_date + "-" + end_date;
        query.insert("StudyDate", date_range.c_str());

        QJsonObject body;
        body.insert("Level", "Studies");
        body.insert("Query", query);
        body.insert("Limit", 0);

        /// Url PACS
        const std::string pacs_server("http://" + m_server_hostname + ":" + std::to_string(m_server_port));

        /// Orthanc "/tools/find" route. POST a JSON to get all Studies corresponding to StudyDate range.
        sight::io::http::request::sptr request = sight::io::http::request::New(pacs_server + "/tools/find");
        QByteArray studies_list_answer;
        try
        {
            studies_list_answer = m_client_qt.post(request, QJsonDocument(body).toJson());
        }
        catch(sight::io::http::exceptions::host_not_found& exception)
        {
            std::stringstream ss;
            ss << "Host not found:\n"
            << " Please check your configuration: \n"
            << "Pacs host name: " << m_server_hostname << "\n"
            << "Pacs port: " << m_server_port << "\n";

            sight::module::io::dicomweb::query_editor::display_error_message(ss.str());
            SIGHT_WARN(exception.what());
        }
        QJsonDocument json_response          = QJsonDocument::fromJson(studies_list_answer);
        const QJsonArray& studies_list_array = json_response.array();
        const int studies_list_array_size    = studies_list_array.count();

        for(int i = 0 ; i < studies_list_array_size ; ++i)
        {
            const std::string& studies_uid = studies_list_array.at(i).toString().toStdString();
            const std::string studies_url(pacs_server + "/studies/" + studies_uid);
            const QByteArray& studies_answer = m_client_qt.get(sight::io::http::request::New(studies_url));

            json_response = QJsonDocument::fromJson(studies_answer);
            const QJsonObject& json_obj    = json_response.object();
            const QJsonArray& series_array = json_obj["Series"].toArray();
            const int series_array_size    = series_array.count();

            for(int j = 0 ; j < series_array_size ; ++j)
            {
                const std::string& series_uid = series_array.at(j).toString().toStdString();
                const std::string instances_url(pacs_server + "/series/" + series_uid);
                const QByteArray& instances_answer = m_client_qt.get(sight::io::http::request::New(instances_url));
                json_response = QJsonDocument::fromJson(instances_answer);
                const QJsonObject& another_json_obj = json_response.object();
                const QJsonArray& instance_array    = another_json_obj["Instances"].toArray();

                // Retrieve the first instance for the needed information
                const std::string& instance_uid = instance_array.at(0).toString().toStdString();
                const std::string instance_url(pacs_server + "/instances/" + instance_uid + "/simplified-tags");
                const QByteArray& instance = m_client_qt.get(sight::io::http::request::New(instance_url));

                QJsonObject series_json = QJsonDocument::fromJson(instance).object();
                series_json.insert("NumberOfSeriesRelatedInstances", instance_array.count());

                // Convert response to DicomSeries
                data::series_set::container_t series = sight::io::http::helper::series::to_fw_med_data(series_json);

                all_series.insert(std::end(all_series), std::begin(series), std::end(series));
                this->update_series_set(all_series);
            }
        }
    }
    catch(sight::io::http::exceptions::base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        sight::module::io::dicomweb::query_editor::display_error_message(ss.str());
        SIGHT_WARN(exception.what());
    }
}

//------------------------------------------------------------------------------

void query_editor::update_series_set(const data::series_set::container_t& _series)
{
    const auto series_set     = m_series_set.lock();
    const auto scoped_emitter = series_set->scoped_emit();

    // Delete old series from the series_set
    series_set->clear();

    // Push new series in the series_set
    for(const auto& s : _series)
    {
        const auto& dicom_series = std::dynamic_pointer_cast<data::dicom_series>(s);
        series_set->push_back(dicom_series);
    }
}

//------------------------------------------------------------------------------

void query_editor::display_error_message(const std::string& _message)
{
    sight::ui::dialog::message message_box;
    message_box.set_title("Error");
    message_box.set_message(_message);
    message_box.set_icon(ui::dialog::message::critical);
    message_box.add_button(ui::dialog::message::ok);
    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicomweb
