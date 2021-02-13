/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "SSeriesPuller.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/tools/System.hpp>

#include <data/DicomSeries.hpp>
#include <data/tools/helper/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <service/registry/ObjectService.hpp>
#include <service/registry/ServiceConfig.hpp>
#include <service/registry/ServiceFactory.hpp>

#include <io/http/exceptions/Base.hpp>
#include <io/http/helper/Series.hpp>
#include <io/http/Request.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>
#include <ui/base/preferences/helper.hpp>

#include <filesystem>

namespace sight::modules::io::dicomweb
{

//------------------------------------------------------------------------------

SSeriesPuller::SSeriesPuller() noexcept :
    m_isPulling(false),
    m_seriesIndex(0)
{
}

//------------------------------------------------------------------------------

SSeriesPuller::~SSeriesPuller() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesPuller::configuring()
{
    core::runtime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service modules::io::dicomweb::SSeriesPuller must have a \"config\" element.", config);

    bool success;

    // Dicom Reader
    std::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" in the modules::io::dicomweb::SSeriesPuller config element.", success);

    // Dicom Reader Config
    std::tie(success, m_dicomReaderSrvConfig) = config->getSafeAttributeValue("dicomReaderConfig");

    service::IService::ConfigType configuration = this->getConfigTree();
    //Parse server port and hostname
    if(configuration.count("server"))
    {
        const std::string serverInfo               = configuration.get("server", "");
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        m_serverHostnameKey = serverInfo.substr(0, splitPosition);
        m_serverPortKey     = serverInfo.substr(splitPosition + 1, serverInfo.size());
    }
    else
    {
        throw core::tools::Failed("'server' element not found");
    }

}

//------------------------------------------------------------------------------

void SSeriesPuller::starting()
{
    // Get Destination SeriesDB
    m_destinationSeriesDB = this->getInOut< data::SeriesDB>("seriesDB");
    SLM_ASSERT("The 'seriesDB' key doesn't exist.", m_destinationSeriesDB);

    // Create temporary SeriesDB
    m_tempSeriesDB = data::SeriesDB::New();

    // Create reader
    service::registry::ServiceFactory::sptr srvFactory = service::registry::ServiceFactory::getDefault();
    m_dicomReader =
        sight::io::base::service::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SLM_ASSERT(
        "Unable to create a reader of type: \"" + m_dicomReaderType + "\" in modules::io::dicomweb::SSeriesPuller.",
        m_dicomReader);
    service::OSR::registerService(m_tempSeriesDB, sight::io::base::service::s_DATA_KEY,
                                  service::IService::AccessType::INOUT, m_dicomReader);

    if(!m_dicomReaderSrvConfig.empty())
    {
        // Get the config
        core::runtime::ConfigurationElement::csptr readerConfig =
            service::registry::ServiceConfig::getDefault()->getServiceConfig(
                m_dicomReaderSrvConfig, "::io::base::service::IReader");

        SLM_ASSERT("Sorry, there is no service configuration "
                   << m_dicomReaderSrvConfig
                   << " for sight::io::base::service::IReader", readerConfig);

        m_dicomReader->setConfiguration( core::runtime::ConfigurationElement::constCast(readerConfig) );
    }

    m_dicomReader->configure();
    m_dicomReader->start();
}

//------------------------------------------------------------------------------

void SSeriesPuller::stopping()
{
    // Stop reader service
    m_dicomReader->stop();
    service::OSR::unregisterService(m_dicomReader);
}

//------------------------------------------------------------------------------

void SSeriesPuller::updating()
{
    const std::string hostname = ui::base::preferences::getValue(m_serverHostnameKey);
    if(!hostname.empty())
    {
        m_serverHostname = hostname;
    }
    const std::string port = ui::base::preferences::getValue(m_serverPortKey);
    if(!port.empty())
    {
        m_serverPort = std::stoi(port);
    }

    data::Vector::csptr selectedSeries = this->getInput< data::Vector >("selectedSeries");

    if(m_isPulling)
    {
        // Display a message to inform the user that the service is already pulling data.
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pulling Series");
        messageBox.setMessage( "The service is already pulling data. Please wait until the pulling is done "
                               "before sending a new pull request." );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
        messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    else if(selectedSeries->empty())
    {
        // Display a message to inform the user that there is no series selected.
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pulling Series");
        messageBox.setMessage( "Unable to pull series, there is no series selected. " );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
        messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    else
    {
        this->pullSeries();
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::pullSeries()
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

        data::Vector::csptr selectedSeries = this->getInput< data::Vector >("selectedSeries");

        // Find which selected series must be pulled
        DicomSeriesContainerType pullSeriesVector;
        DicomSeriesContainerType selectedSeriesVector;

        data::Vector::ConstIteratorType it = selectedSeries->begin();
        for(; it != selectedSeries->end(); ++it)
        {
            data::DicomSeries::sptr series = data::DicomSeries::dynamicCast(*it);

            // Check if the series must be pulled
            if(series &&
               std::find(m_localSeries.begin(), m_localSeries.end(), series->getInstanceUID()) == m_localSeries.end())
            {
                // Add series in the pulling series map
                m_pullingDicomSeriesMap[series->getInstanceUID()] = series;

                pullSeriesVector.push_back(series);
                m_instanceCount += series->getNumberOfInstances();
            }
            selectedSeriesVector.push_back(series);
        }

        // Pull series
        if(!pullSeriesVector.empty())
        {
            /// GET
            const InstanceUIDContainerType& seriesInstancesUIDs =
                sight::io::http::helper::Series::toSeriesInstanceUIDContainer(pullSeriesVector);
            for( const std::string& seriesInstancesUID : seriesInstancesUIDs )
            {
                // Find Series according to SeriesInstanceUID
                QJsonObject query;
                query.insert("SeriesInstanceUID", seriesInstancesUID.c_str());

                QJsonObject body;
                body.insert("Level", "Series");
                body.insert("Query", query);
                body.insert("Limit", 0);

                /// Url PACS
                const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));

                /// Orthanc "/tools/find" route. POST a JSON to get all Series corresponding to the SeriesInstanceUID.
                sight::io::http::Request::sptr request = sight::io::http::Request::New(
                    pacsServer + "/tools/find");
                QByteArray seriesAnswer;
                try
                {
                    seriesAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
                }
                catch  (sight::io::http::exceptions::HostNotFound& exception)
                {
                    std::stringstream ss;
                    ss << "Host not found:\n"
                       << " Please check your configuration: \n"
                       << "Pacs host name: " << m_serverHostname << "\n"
                       << "Pacs port: " << m_serverPort << "\n";

                    this->displayErrorMessage(ss.str());
                    SLM_WARN(exception.what());
                }

                QJsonDocument jsonResponse    = QJsonDocument::fromJson(seriesAnswer);
                const QJsonArray& seriesArray = jsonResponse.array();

                const size_t seriesArraySize = seriesArray.count();
                for(size_t i = 0; i < seriesArraySize; ++i)
                {
                    const std::string& seriesUID = seriesArray.at(i).toString().toStdString();

                    /// GET all Instances by Series.
                    const std::string& instancesUrl(pacsServer + "/series/" + seriesUID);
                    const QByteArray& instancesAnswer =
                        m_clientQt.get( sight::io::http::Request::New(instancesUrl));
                    jsonResponse = QJsonDocument::fromJson(instancesAnswer);
                    const QJsonObject& jsonObj       = jsonResponse.object();
                    const QJsonArray& instancesArray = jsonObj["Instances"].toArray();

                    const size_t instancesArraySize = instancesArray.count();
                    for(size_t j = 0; j < instancesArraySize; ++j)
                    {
                        const std::string& instanceUID = instancesArray.at(j).toString().toStdString();

                        /// GET DICOM Instance file.
                        const std::string instanceUrl(pacsServer +"/instances/" + instanceUID + "/file");

                        try
                        {
                            m_path = m_clientQt.getFile(sight::io::http::Request::New(instanceUrl));
                        }
                        catch  (sight::io::http::exceptions::ContentNotFound& exception)
                        {
                            std::stringstream ss;
                            ss << "Content not found:  \n"
                               << "Unable download the DICOM instance. \n";

                            this->displayErrorMessage(ss.str());
                            SLM_WARN(exception.what());
                        }

                        // Create dicom folder
                        std::filesystem::path instancePath = m_path.parent_path() / seriesInstancesUID;
                        QDir().mkpath(instancePath.string().c_str());
                        // Move dicom file to the created dicom folder
                        instancePath /= m_path.filename();
                        QFile().rename(m_path.string().c_str(), instancePath.string().c_str());
                        m_path = m_path.parent_path() / seriesInstancesUID;
                    }
                }
            }
        }

        // Read series if there is no error
        if(m_isPulling)
        {
            this->readLocalSeries(selectedSeriesVector);
        }

        // Set pulling boolean to false
        m_isPulling = false;

    }
    catch (sight::io::http::exceptions::Base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        this->displayErrorMessage(ss.str());
        SLM_WARN(exception.what());
        m_isPulling = false;
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::readLocalSeries(DicomSeriesContainerType selectedSeries)
{
    // Read only series that are not in the SeriesDB
    const InstanceUIDContainerType& alreadyLoadedSeries =
        sight::io::http::helper::Series::toSeriesInstanceUIDContainer(m_destinationSeriesDB->getContainer());

    // Create temporary series helper
    data::tools::helper::SeriesDB tempSDBhelper(m_tempSeriesDB);

    for(const data::Series::sptr& series: selectedSeries)
    {
        const std::string& selectedSeriesUID = series->getInstanceUID();

        // Add the series to the local series vector
        if(std::find(m_localSeries.begin(), m_localSeries.end(), selectedSeriesUID) == m_localSeries.end())
        {
            m_localSeries.push_back(selectedSeriesUID);
        }

        // Check if the series is loaded
        if(std::find(alreadyLoadedSeries.begin(), alreadyLoadedSeries.end(),
                     selectedSeriesUID) == alreadyLoadedSeries.end())
        {
            // Clear temporary series
            tempSDBhelper.clear();

            m_dicomReader->setFolder(m_path);
            m_dicomReader->update();

            // Merge series
            data::tools::helper::SeriesDB sDBhelper(m_destinationSeriesDB);
            sDBhelper.merge(m_tempSeriesDB);
            sDBhelper.notify();
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::displayErrorMessage(const std::string& message) const
{
    SLM_WARN("Error: " + message);
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace sight::modules::io::dicomweb
