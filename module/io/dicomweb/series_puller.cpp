/************************************************************************
 *
 * Copyright (C) 2018-2025 IRCAD France
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

#include "series_puller.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/tools/system.hpp>

#include <data/dicom_series.hpp>

#include <io/http/exceptions/base.hpp>
#include <io/http/helper/series.hpp>
#include <io/http/request.hpp>

#include <service/extension/config.hpp>
#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>
#include <ui/__/preferences.hpp>

#include <filesystem>

namespace sight::module::io::dicomweb
{

//------------------------------------------------------------------------------

series_puller::series_puller() noexcept =
    default;

//------------------------------------------------------------------------------

series_puller::~series_puller() noexcept =
    default;

//------------------------------------------------------------------------------

void series_puller::configuring()
{
    const config_t config_tree = this->get_config();
    const auto config          = config_tree.get_child("config.<xmlattr>");

    m_dicom_reader_type       = config.get<std::string>("dicom_reader", m_dicom_reader_type);
    m_dicom_reader_srv_config = config.get<std::string>("reader_config", m_dicom_reader_srv_config);
}

//------------------------------------------------------------------------------

void series_puller::starting()
{
    // Create temporary series_set
    m_tmp_series_set = std::make_shared<data::series_set>();

    // Create reader
    m_dicom_reader = service::add<sight::io::service::reader>(m_dicom_reader_type);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicom_reader_type + "\" in module::io::dicomweb::series_puller.",
        m_dicom_reader
    );
    m_dicom_reader->set_inout(m_tmp_series_set, sight::io::service::DATA_KEY);

    if(!m_dicom_reader_srv_config.empty())
    {
        // Get the config
        const auto reader_config = service::extension::config::get_default()->get_service_config(
            m_dicom_reader_srv_config,
            "sight::io::service::reader"
        );

        SIGHT_ASSERT(
            "Sorry, there is no service configuration "
            << m_dicom_reader_srv_config
            << " for sight::io::service::reader",
            !reader_config.empty()
        );

        m_dicom_reader->set_config(reader_config);
    }

    m_dicom_reader->configure();
    m_dicom_reader->start();
}

//------------------------------------------------------------------------------

void series_puller::stopping()
{
    {
        const auto series_set     = m_series_set.lock();
        const auto scoped_emitter = series_set->scoped_emit();

        // Delete old series from the series_set.
        series_set->clear();
    }
    // Stop reader service
    m_dicom_reader->stop();
    service::remove(m_dicom_reader);
}

//------------------------------------------------------------------------------

void series_puller::updating()
{
    if(m_is_pulling)
    {
        // Display a message to inform the user that the service is already pulling data.
        sight::ui::dialog::message message_box;
        message_box.set_title("Pulling Series");
        message_box.set_message(
            "The service is already pulling data. Please wait until the pulling is done "
            "before sending a new pull request."
        );
        message_box.set_icon(ui::dialog::message::info);
        message_box.add_button(ui::dialog::message::ok);
        message_box.show();
    }
    else
    {
        const auto selected_series = m_selected_series.lock();
        if(selected_series->empty())
        {
            // Display a message to inform the user that there is no series selected.
            sight::ui::dialog::message message_box;
            message_box.set_title("Pulling Series");
            message_box.set_message("Unable to pull series, there is no series selected. ");
            message_box.set_icon(ui::dialog::message::info);
            message_box.add_button(ui::dialog::message::ok);
            message_box.show();
        }
        else
        {
            this->pull_series();
        }
    }
}

//------------------------------------------------------------------------------

void series_puller::pull_series()
{
    // Catch any errors
    try
    {
        // Clear map of Dicom series being pulled
        m_pulling_dicom_series_map.clear();

        // Set pulling boolean to true
        m_is_pulling = true;

        // Reset Counters
        m_series_index   = 0;
        m_instance_count = 0;

        const auto selected_series = m_selected_series.lock();

        // Find which selected series must be pulled
        dicom_series_container_t pull_series_vector;
        dicom_series_container_t selected_series_vector;

        auto it = selected_series->cbegin();
        for( ; it != selected_series->cend() ; ++it)
        {
            data::dicom_series::sptr series = std::dynamic_pointer_cast<data::dicom_series>(*it);

            // Check if the series must be pulled
            if(series
               && std::find(
                   m_local_series.begin(),
                   m_local_series.end(),
                   series->get_series_instance_uid()
               ) == m_local_series.end())
            {
                // Add series in the pulling series map
                m_pulling_dicom_series_map[series->get_series_instance_uid()] = series;

                pull_series_vector.push_back(series);
                m_instance_count += series->num_instances();
            }

            selected_series_vector.push_back(series);
        }

        // Pull series
        if(!pull_series_vector.empty())
        {
            /// GET
            const instance_uid_container_t& series_instances_ui_ds =
                sight::io::http::helper::series::to_series_instance_uid_container(pull_series_vector);
            for(const std::string& series_instances_uid : series_instances_ui_ds)
            {
                // Find Series according to SeriesInstanceUID
                QJsonObject query;
                query.insert("SeriesInstanceUID", series_instances_uid.c_str());

                QJsonObject body;
                body.insert("Level", "Series");
                body.insert("Query", query);
                body.insert("Limit", 0);

                /// Url PACS
                const std::string pacs_server("http://" + *m_server_hostname + ":" + std::to_string(*m_server_port));

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
                    << "Pacs host name: " << *m_server_hostname << "\n"
                    << "Pacs port: " << *m_server_port << "\n";

                    sight::module::io::dicomweb::series_puller::display_error_message(ss.str());
                    SIGHT_WARN(exception.what());
                }

                QJsonDocument json_response    = QJsonDocument::fromJson(series_answer);
                const QJsonArray& series_array = json_response.array();

                const auto series_array_size = series_array.count();
                for(auto i = 0 ; i < series_array_size ; ++i)
                {
                    const std::string& series_uid = series_array.at(i).toString().toStdString();

                    /// GET all Instances by Series.
                    const std::string& instances_url(std::string(pacs_server) + "/series/" + series_uid);
                    const QByteArray& instances_answer =
                        m_client_qt.get(sight::io::http::request::New(instances_url));
                    json_response = QJsonDocument::fromJson(instances_answer);
                    const QJsonObject& json_obj       = json_response.object();
                    const QJsonArray& instances_array = json_obj["Instances"].toArray();

                    const auto instances_array_size = instances_array.count();
                    for(auto j = 0 ; j < instances_array_size ; ++j)
                    {
                        const std::string& instance_uid = instances_array.at(j).toString().toStdString();

                        /// GET DICOM Instance file.
                        const std::string instance_url(pacs_server + "/instances/" + instance_uid + "/file");

                        try
                        {
                            m_path = m_client_qt.get_file(sight::io::http::request::New(instance_url));
                        }
                        catch(sight::io::http::exceptions::content_not_found& exception)
                        {
                            std::stringstream ss;
                            ss << "Content not found:  \n"
                            << "Unable download the DICOM instance. \n";

                            sight::module::io::dicomweb::series_puller::display_error_message(ss.str());
                            SIGHT_WARN(exception.what());
                        }

                        // Create dicom folder
                        std::filesystem::path instance_path = m_path.parent_path() / series_instances_uid;
                        QDir().mkpath(instance_path.string().c_str());
                        // Move dicom file to the created dicom folder
                        instance_path /= m_path.filename();
                        QFile::rename(m_path.string().c_str(), instance_path.string().c_str());
                        m_path = m_path.parent_path() / series_instances_uid;
                    }
                }
            }
        }

        // Read series if there is no error
        if(m_is_pulling)
        {
            this->read_local_series(selected_series_vector);
        }

        // Set pulling boolean to false
        m_is_pulling = false;
    }
    catch(sight::io::http::exceptions::base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        sight::module::io::dicomweb::series_puller::display_error_message(ss.str());
        SIGHT_WARN(exception.what());
        m_is_pulling = false;
    }
}

//------------------------------------------------------------------------------

void series_puller::read_local_series(dicom_series_container_t _selected_series)
{
    const auto dest_series_set = m_series_set.lock();

    const auto scoped_emitter = dest_series_set->scoped_emit();

    // Read only series that are not in the series_set
    const instance_uid_container_t& already_loaded_series =
        sight::io::http::helper::series::to_series_instance_uid_container(dest_series_set->get_content());

    for(const auto& series : _selected_series)
    {
        const std::string& selected_series_uid = series->get_series_instance_uid();

        // Add the series to the local series vector
        if(std::find(m_local_series.begin(), m_local_series.end(), selected_series_uid) == m_local_series.end())
        {
            m_local_series.push_back(selected_series_uid);
        }

        // Check if the series is loaded
        if(std::find(
               already_loaded_series.cbegin(),
               already_loaded_series.cend(),
               selected_series_uid
           ) == already_loaded_series.cend())
        {
            // Clear temporary series
            m_tmp_series_set->clear();

            m_dicom_reader->set_folder(m_path);
            m_dicom_reader->update();

            // Merge series
            std::copy(m_tmp_series_set->cbegin(), m_tmp_series_set->cend(), sight::data::inserter(*dest_series_set));
        }
    }
}

//------------------------------------------------------------------------------

void series_puller::display_error_message(const std::string& _message)
{
    SIGHT_WARN("Error: " + _message);
    sight::ui::dialog::message message_box;
    message_box.set_title("Error");
    message_box.set_message(_message);
    message_box.set_icon(ui::dialog::message::critical);
    message_box.add_button(ui::dialog::message::ok);
    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicomweb
