/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicomWeb/SSeriesPuller.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Vector.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwNetworkIO/exceptions/Base.hpp>
#include <fwNetworkIO/helper/Series.hpp>
#include <fwNetworkIO/http/Request.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem/operations.hpp>

namespace ioDicomWeb
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioDicomWeb::SSeriesPuller, ::fwData::Vector );

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

void SSeriesPuller::info(std::ostream& _sstream )
{
    _sstream << "SSeriesPuller::info";
}

//------------------------------------------------------------------------------

void SSeriesPuller::configuring()
{
    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicomWeb::SSeriesPuller must have a \"config\" element.", config);

    bool success;

    // Dicom Reader
    ::boost::tie(success, m_dicomReaderType) = config->getSafeAttributeValue("dicomReader");
    SLM_ASSERT("It should be a \"dicomReader\" in the ::ioDicomWeb::SSeriesPuller config element.", success);

    // Dicom Reader Config
    ::boost::tie(success, m_dicomReaderSrvConfig) = config->getSafeAttributeValue("dicomReaderConfig");

    ::fwServices::IService::ConfigType configuration = this->getConfigTree();
    //Parse server port and hostname
    if(configuration.count("server"))
    {
        const std::string serverInfo               = configuration.get("server", "");
        const std::string::size_type splitPosition = serverInfo.find(':');
        SLM_ASSERT("Server info not formatted correctly", splitPosition != std::string::npos);

        const std::string hostnameStr = serverInfo.substr(0, splitPosition);
        const std::string portStr     = serverInfo.substr(splitPosition + 1, serverInfo.size());

        m_serverHostnameKey = this->getPreferenceKey(hostnameStr);
        m_serverPortKey     = this->getPreferenceKey(portStr);

        if(m_serverHostnameKey.empty())
        {
            m_serverHostname = hostnameStr;
        }
        if(m_serverPortKey.empty())
        {
            m_serverPort = std::stoi(portStr);
        }
    }
    else
    {
        throw ::fwTools::Failed("'server' element not found");
    }

}

// -----------------------------------------------------------------------------

std::string SSeriesPuller::getPreferenceKey(const std::string& key) const
{
    std::string keyResult;
    const size_t first = key.find('%');
    const size_t last  = key.rfind('%');
    if (first == 0 && last == key.size() - 1)
    {
        keyResult = key.substr(1, key.size() - 2);
    }
    return keyResult;
}

//------------------------------------------------------------------------------

void SSeriesPuller::starting()
{
    // Get Destination SeriesDB
    m_destinationSeriesDB = this->getInOut< ::fwMedData::SeriesDB>("seriesDB");
    SLM_ASSERT("The 'seriesDB' key doesn't exist.", m_destinationSeriesDB);

    // Create temporary SeriesDB
    m_tempSeriesDB = ::fwMedData::SeriesDB::New();

    // Create reader
    ::fwServices::registry::ServiceFactory::sptr srvFactory = ::fwServices::registry::ServiceFactory::getDefault();
    m_dicomReader                                           =
        ::fwIO::IReader::dynamicCast(srvFactory->create(m_dicomReaderType));
    SLM_ASSERT("Unable to create a reader of type: \"" + m_dicomReaderType + "\" in ::ioDicomWeb::SSeriesPuller.",
               m_dicomReader);
    ::fwServices::OSR::registerService(m_tempSeriesDB, m_dicomReader);

    if(!m_dicomReaderSrvConfig.empty())
    {
        // Get the config
        ::fwRuntime::ConfigurationElement::csptr readerConfig =
            ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
                m_dicomReaderSrvConfig, "::fwIO::IReader");

        SLM_ASSERT("Sorry, there is no service configuration "
                   << m_dicomReaderSrvConfig
                   << " for ::fwIO::IReader", readerConfig);

        m_dicomReader->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(readerConfig) );
    }

    m_dicomReader->configure();
    m_dicomReader->start();
}

//------------------------------------------------------------------------------

void SSeriesPuller::stopping()
{
    // Stop reader service
    m_dicomReader->stop();
    ::fwServices::OSR::unregisterService(m_dicomReader);
}

//------------------------------------------------------------------------------

void SSeriesPuller::updating()
{
    if(!m_serverHostnameKey.empty())
    {
        const std::string hostname = ::fwPreferences::getPreference(m_serverHostnameKey);
        if(!hostname.empty())
        {
            m_serverHostname = hostname;
        }
    }
    if(!m_serverPortKey.empty())
    {
        const std::string port = ::fwPreferences::getPreference(m_serverPortKey);
        if(!port.empty())
        {
            m_serverPort = std::stoi(port);
        }
    }

    ::fwData::Vector::csptr selectedSeries = this->getInput< ::fwData::Vector >("selectedSeries");

    if(m_isPulling)
    {
        // Display a message to inform the user that the service is already pulling data.
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pulling Series");
        messageBox.setMessage( "The service is already pulling data. Please wait until the pulling is done "
                               "before sending a new pull request." );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    else if(selectedSeries->empty())
    {
        // Display a message to inform the user that there is no series selected.
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Pulling Series");
        messageBox.setMessage( "Unable to pull series, there is no series selected. " );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
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

        ::fwData::Vector::csptr selectedSeries = this->getInput< ::fwData::Vector >("selectedSeries");

        // Find which selected series must be pulled
        DicomSeriesContainerType pullSeriesVector;
        DicomSeriesContainerType selectedSeriesVector;

        ::fwData::Vector::ConstIteratorType it = selectedSeries->begin();
        for(; it != selectedSeries->end(); ++it)
        {
            ::fwMedData::DicomSeries::sptr series = ::fwMedData::DicomSeries::dynamicCast(*it);

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
                ::fwNetworkIO::helper::Series::toSeriesInstanceUIDContainer(pullSeriesVector);
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
                ::fwNetworkIO::http::Request::sptr request = ::fwNetworkIO::http::Request::New(
                    pacsServer + "/tools/find");
                const QByteArray& seriesAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
                QJsonDocument jsonResponse     = QJsonDocument::fromJson(seriesAnswer);
                const QJsonArray& seriesArray  = jsonResponse.array();

                const size_t seriesArraySize = seriesArray.count();
                for(size_t i = 0; i < seriesArraySize; ++i)
                {
                    const std::string& seriesUID = seriesArray.at(i).toString().toStdString();

                    /// GET all Instances by Series.
                    const std::string& instancesUrl(pacsServer + "/series/" + seriesUID);
                    const QByteArray& instancesAnswer =
                        m_clientQt.get( ::fwNetworkIO::http::Request::New(instancesUrl));
                    jsonResponse = QJsonDocument::fromJson(instancesAnswer);
                    const QJsonObject& jsonObj       = jsonResponse.object();
                    const QJsonArray& instancesArray = jsonObj["Instances"].toArray();

                    const size_t instancesArraySize = instancesArray.count();
                    for(size_t j = 0; j < instancesArraySize; ++j)
                    {
                        const std::string& instanceUID = instancesArray.at(j).toString().toStdString();

                        /// GET DICOM Instance file.
                        const std::string instanceUrl(pacsServer +"/instances/" + instanceUID + "/file");
                        m_path = m_clientQt.getFile(::fwNetworkIO::http::Request::New(instanceUrl));
                        // Create dicom folder
                        ::boost::filesystem::path instancePath = m_path.parent_path() / seriesInstancesUID;
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
    catch (::fwNetworkIO::exceptions::Base& exception)
    {
        ::std::stringstream ss;
        ss << "Error: Retrieving from the PACS failed.";
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
        ::fwNetworkIO::helper::Series::toSeriesInstanceUIDContainer(m_destinationSeriesDB->getContainer());

    // Create temporary series helper
    ::fwMedDataTools::helper::SeriesDB tempSDBhelper(m_tempSeriesDB);

    for(const ::fwMedData::Series::sptr& series: selectedSeries)
    {
        ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(series);
        dicomSeries->setDicomAvailability(::fwMedData::DicomSeries::PATHS);

        const std::string& selectedSeriesUID = series->getInstanceUID();
        // Add the series to the local series vector
        if(::std::find(m_localSeries.begin(), m_localSeries.end(), selectedSeriesUID) == m_localSeries.end())
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
            ::fwMedDataTools::helper::SeriesDB sDBhelper(m_destinationSeriesDB);
            sDBhelper.merge(m_tempSeriesDB);
            sDBhelper.notify();
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesPuller::displayErrorMessage(const ::std::string& message) const
{
    SLM_WARN("Error: " + message);
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace ioDicomWeb
