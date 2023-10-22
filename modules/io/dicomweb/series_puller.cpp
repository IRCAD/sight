/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include <io/http/exceptions/Base.hpp>
#include <io/http/helper/Series.hpp>
#include <io/http/Request.hpp>

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
    const auto& config = this->get_config();

    m_dicomReaderType      = config.get<std::string>("config.<xmlattr>.dicomReader", m_dicomReaderType);
    m_dicomReaderSrvConfig = config.get<std::string>("config.<xmlattr>.readerConfig", m_dicomReaderSrvConfig);

    //Parse server port and hostname
    if(config.count("server") != 0U)
    {
        const std::string server_info               = config.get("server", "");
        const std::string::size_type split_position = server_info.find(':');
        SIGHT_ASSERT("Server info not formatted correctly", split_position != std::string::npos);

        m_serverHostnameKey = server_info.substr(0, split_position);
        m_serverPortKey     = server_info.substr(split_position + 1, server_info.size());
    }
    else
    {
        throw core::tools::failed("'server' element not found");
    }
}

//------------------------------------------------------------------------------

void series_puller::starting()
{
    // Create temporary series_set
    m_tmp_series_set = std::make_shared<data::series_set>();

    // Create reader
    m_dicomReader = service::add<sight::io::service::reader>(m_dicomReaderType);
    SIGHT_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in module::io::dicomweb::series_puller.",
        m_dicomReader
    );
    m_dicomReader->set_inout(m_tmp_series_set, sight::io::service::s_DATA_KEY);

    if(!m_dicomReaderSrvConfig.empty())
    {
        // Get the config
        const auto reader_config = service::extension::config::getDefault()->get_service_config(
            m_dicomReaderSrvConfig,
            "sight::io::service::reader"
        );

        SIGHT_ASSERT(
            "Sorry, there is no service configuration "
            << m_dicomReaderSrvConfig
            << " for sight::io::service::reader",
            !reader_config.empty()
        );

        m_dicomReader->set_config(reader_config);
    }

    m_dicomReader->configure();
    m_dicomReader->start();
}

//------------------------------------------------------------------------------

void series_puller::stopping()
{
    // Stop reader service
    m_dicomReader->stop();
    service::remove(m_dicomReader);
}

//------------------------------------------------------------------------------

void series_puller::updating()
{
    try
    {
        ui::preferences preferences;
        m_serverPort     = preferences.delimited_get(m_serverPortKey, m_serverPort);
        m_serverHostname = preferences.delimited_get(m_serverHostnameKey, m_serverHostname);
    }
    catch(...)
    {
        // Do nothing
    }

    if(m_isPulling)
    {
        // Display a message to inform the user that the service is already pulling data.
        sight::ui::dialog::message message_box;
        message_box.setTitle("Pulling Series");
        message_box.setMessage(
            "The service is already pulling data. Please wait until the pulling is done "
            "before sending a new pull request."
        );
        message_box.setIcon(ui::dialog::message::INFO);
        message_box.addButton(ui::dialog::message::OK);
        message_box.show();
    }
    else
    {
        const auto selected_series = m_selectedSeries.lock();
        if(selected_series->empty())
        {
            // Display a message to inform the user that there is no series selected.
            sight::ui::dialog::message message_box;
            message_box.setTitle("Pulling Series");
            message_box.setMessage("Unable to pull series, there is no series selected. ");
            message_box.setIcon(ui::dialog::message::INFO);
            message_box.addButton(ui::dialog::message::OK);
            message_box.show();
        }
        else
        {
            this->pullSeries();
        }
    }
}

//------------------------------------------------------------------------------

void series_puller::pullSeries()
{
    // Catch any errors
    try
    {
        // Clear map of Dicom series being pulled
        m_pullingDicomSeriesMap.clear();

        // Set pulling boolean to true
        m_isPulling = true;

        // Reset Counters
        m_seriesIndex   = 0;
        m_instanceCount = 0;

        const auto selected_series = m_selectedSeries.lock();

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
                   m_localSeries.begin(),
                   m_localSeries.end(),
                   series->getSeriesInstanceUID()
               ) == m_localSeries.end())
            {
                // Add series in the pulling series map
                m_pullingDicomSeriesMap[series->getSeriesInstanceUID()] = series;

                pull_series_vector.push_back(series);
                m_instanceCount += series->numInstances();
            }

            selected_series_vector.push_back(series);
        }

        // Pull series
        if(!pull_series_vector.empty())
        {
            /// GET
            const InstanceUIDContainerType& series_instances_ui_ds =
                sight::io::http::helper::Series::toSeriesInstanceUIDContainer(pull_series_vector);
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
                const std::string pacs_server("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));

                /// Orthanc "/tools/find" route. POST a JSON to get all Series corresponding to the SeriesInstanceUID.
                sight::io::http::Request::sptr request = sight::io::http::Request::New(
                    pacs_server + "/tools/find"
                );
                QByteArray series_answer;
                try
                {
                    series_answer = m_clientQt.post(request, QJsonDocument(body).toJson());
                }
                catch(sight::io::http::exceptions::HostNotFound& exception)
                {
                    std::stringstream ss;
                    ss << "Host not found:\n"
                    << " Please check your configuration: \n"
                    << "Pacs host name: " << m_serverHostname << "\n"
                    << "Pacs port: " << m_serverPort << "\n";

                    sight::module::io::dicomweb::series_puller::displayErrorMessage(ss.str());
                    SIGHT_WARN(exception.what());
                }

                QJsonDocument json_response    = QJsonDocument::fromJson(series_answer);
                const QJsonArray& series_array = json_response.array();

                const int series_array_size = series_array.count();
                for(int i = 0 ; i < series_array_size ; ++i)
                {
                    const std::string& series_uid = series_array.at(i).toString().toStdString();

                    /// GET all Instances by Series.
                    const std::string& instances_url(std::string(pacs_server) + "/series/" + series_uid);
                    const QByteArray& instances_answer =
                        m_clientQt.get(sight::io::http::Request::New(instances_url));
                    json_response = QJsonDocument::fromJson(instances_answer);
                    const QJsonObject& json_obj       = json_response.object();
                    const QJsonArray& instances_array = json_obj["Instances"].toArray();

                    const int instances_array_size = instances_array.count();
                    for(int j = 0 ; j < instances_array_size ; ++j)
                    {
                        const std::string& instance_uid = instances_array.at(j).toString().toStdString();

                        /// GET DICOM Instance file.
                        const std::string instance_url(pacs_server + "/instances/" + instance_uid + "/file");

                        try
                        {
                            m_path = m_clientQt.get_file(sight::io::http::Request::New(instance_url));
                        }
                        catch(sight::io::http::exceptions::ContentNotFound& exception)
                        {
                            std::stringstream ss;
                            ss << "Content not found:  \n"
                            << "Unable download the DICOM instance. \n";

                            sight::module::io::dicomweb::series_puller::displayErrorMessage(ss.str());
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
        if(m_isPulling)
        {
            this->readLocalSeries(selected_series_vector);
        }

        // Set pulling boolean to false
        m_isPulling = false;
    }
    catch(sight::io::http::exceptions::Base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        sight::module::io::dicomweb::series_puller::displayErrorMessage(ss.str());
        SIGHT_WARN(exception.what());
        m_isPulling = false;
    }
}

//------------------------------------------------------------------------------

void series_puller::readLocalSeries(dicom_series_container_t _selected_series)
{
    const auto dest_series_set = m_series_set.lock();

    const auto scoped_emitter = dest_series_set->scoped_emit();

    // Read only series that are not in the series_set
    const InstanceUIDContainerType& already_loaded_series =
        sight::io::http::helper::Series::toSeriesInstanceUIDContainer(dest_series_set->get_content());

    for(const auto& series : _selected_series)
    {
        const std::string& selected_series_uid = series->getSeriesInstanceUID();

        // Add the series to the local series vector
        if(std::find(m_localSeries.begin(), m_localSeries.end(), selected_series_uid) == m_localSeries.end())
        {
            m_localSeries.push_back(selected_series_uid);
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

            m_dicomReader->set_folder(m_path);
            m_dicomReader->update();

            // Merge series
            std::copy(m_tmp_series_set->cbegin(), m_tmp_series_set->cend(), sight::data::inserter(*dest_series_set));
        }
    }
}

//------------------------------------------------------------------------------

void series_puller::displayErrorMessage(const std::string& _message)
{
    SIGHT_WARN("Error: " + _message);
    sight::ui::dialog::message message_box;
    message_box.setTitle("Error");
    message_box.setMessage(_message);
    message_box.setIcon(ui::dialog::message::CRITICAL);
    message_box.addButton(ui::dialog::message::OK);
    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicomweb
