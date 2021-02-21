/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "SQueryEditor.hpp"

#include <data/DicomSeries.hpp>
#include <data/tools/helper/SeriesDB.hpp>

#include <service/macros.hpp>

#include <boost/foreach.hpp>

#include <io/http/helper/Series.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/preferences/helper.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <filesystem>

namespace sight::module::io::dicomweb
{

//------------------------------------------------------------------------------

SQueryEditor::SQueryEditor() noexcept
{
}
//------------------------------------------------------------------------------

SQueryEditor::~SQueryEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SQueryEditor::configuring()
{
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

    sight::ui::base::IGuiContainer::initialize();
}

//------------------------------------------------------------------------------

void SQueryEditor::starting()
{
    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    // Main Widget
    QGridLayout* layout = new QGridLayout();

    m_patientNameLineEdit    = new QLineEdit();
    m_patientNameQueryButton = new QPushButton("Send");
    layout->addWidget(new QLabel("Patient name:"), 0, 0);
    layout->addWidget(m_patientNameLineEdit, 0, 1);
    layout->addWidget(m_patientNameQueryButton, 0, 2);

    m_beginStudyDateEdit = new QDateEdit();
    m_beginStudyDateEdit->setDate(QDate::currentDate());
    m_beginStudyDateEdit->setDisplayFormat("dd.MM.yyyy");
    m_endStudyDateEdit = new QDateEdit();
    m_endStudyDateEdit->setDate(QDate::currentDate());
    m_endStudyDateEdit->setDisplayFormat("dd.MM.yyyy");
    m_studyDateQueryButton = new QPushButton("Send");
    QHBoxLayout* dateLayout = new QHBoxLayout();
    layout->addWidget(new QLabel("Study date:"), 1, 0);
    layout->addLayout(dateLayout, 1, 1);
    layout->addWidget(m_studyDateQueryButton, 1, 2);
    dateLayout->addWidget(m_beginStudyDateEdit);
    dateLayout->addWidget(m_endStudyDateEdit);

    //Set layout
    qtContainer->setLayout(layout);

    // Connect the signals
    QObject::connect(m_patientNameLineEdit, SIGNAL(returnPressed()), this, SLOT(queryPatientName()));
    QObject::connect(m_patientNameQueryButton, SIGNAL(clicked()), this, SLOT(queryPatientName()));
    QObject::connect(m_studyDateQueryButton, SIGNAL(clicked()), this, SLOT(queryStudyDate()));
    QObject::connect(m_beginStudyDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(queryStudyDate()));
    QObject::connect(m_endStudyDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(queryStudyDate()));
}

//------------------------------------------------------------------------------

void SQueryEditor::stopping()
{

    // Disconnect the signals
    QObject::disconnect(m_patientNameLineEdit, SIGNAL(returnPressed()), this, SLOT(queryPatientName()));
    QObject::disconnect(m_patientNameQueryButton, SIGNAL(clicked()), this, SLOT(queryPatientName()));
    QObject::disconnect(m_studyDateQueryButton, SIGNAL(clicked()), this, SLOT(queryStudyDate()));
    QObject::disconnect(m_beginStudyDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(queryStudyDate()));
    QObject::disconnect(m_endStudyDateEdit, SIGNAL(dateChanged(QDate)), this, SLOT(queryStudyDate()));

    this->destroy();
}

//------------------------------------------------------------------------------

void SQueryEditor::updating()
{
}

//------------------------------------------------------------------------------

void SQueryEditor::queryPatientName()
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

    try
    {
        // Vector of all Series that will be retrieved.
        data::SeriesDB::ContainerType allSeries;

        // Find series according to patient's name
        QJsonObject query;
        query.insert("PatientName", m_patientNameLineEdit->text().toStdString().c_str());

        QJsonObject body;
        body.insert("Level", "Series");
        body.insert("Query", query);
        body.insert("Limit", 0);

        /// Url PACS
        const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));

        /// Orthanc "/tools/find" route. POST a JSON to get all Series corresponding to the SeriesInstanceUID.
        auto request                   = sight::io::http::Request::New(pacsServer + "/tools/find");
        const QByteArray& seriesAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
        QJsonDocument jsonResponse     = QJsonDocument::fromJson(seriesAnswer);
        QJsonArray seriesArray         = jsonResponse.array();
        const int seriesArraySize      = seriesArray.count();

        for(int i = 0; i < seriesArraySize; ++i)
        {

            const std::string& seriesUID = seriesArray.at(i).toString().toStdString();
            const std::string instancesListUrl(pacsServer + "/series/" + seriesUID);
            const QByteArray& instancesAnswer = m_clientQt.get( sight::io::http::Request::New(instancesListUrl));
            jsonResponse = QJsonDocument::fromJson(instancesAnswer);
            const QJsonObject& jsonObj      = jsonResponse.object();
            const QJsonArray& instanceArray = jsonObj["Instances"].toArray();

            // Retrieve the first instance for the needed information
            const std::string& instanceUID = instanceArray.at(0).toString().toStdString();
            const std::string instanceUrl(pacsServer + "/instances/" + instanceUID + "/simplified-tags");
            const QByteArray& instance = m_clientQt.get( sight::io::http::Request::New(instanceUrl));

            QJsonObject seriesJson = QJsonDocument::fromJson(instance).object();
            seriesJson.insert( "NumberOfSeriesRelatedInstances", instanceArray.count() );

            // Convert response to DicomSeries
            data::SeriesDB::ContainerType series = sight::io::http::helper::Series::toFwMedData(seriesJson);

            allSeries.insert(std::end(allSeries), std::begin(series), std::end(series));
            this->updateSeriesDB(allSeries);
        }
    }
    catch (sight::io::http::exceptions::Base& exception)
    {
        this->displayErrorMessage(exception.what());
    }

}

//------------------------------------------------------------------------------

void SQueryEditor::queryStudyDate()
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

    try
    {
        // Vector of all Series that will be retrieved.
        data::SeriesDB::ContainerType allSeries;

        // Find Studies according to their StudyDate
        QJsonObject query;
        const std::string& beginDate = m_beginStudyDateEdit->date().toString("yyyyMMdd").toStdString();
        const std::string& endDate   = m_endStudyDateEdit->date().toString("yyyyMMdd").toStdString();
        const std::string& dateRange = beginDate + "-" + endDate;
        query.insert("StudyDate", dateRange.c_str());

        QJsonObject body;
        body.insert("Level", "Studies");
        body.insert("Query", query);
        body.insert("Limit", 0);

        /// Url PACS
        const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));

        /// Orthanc "/tools/find" route. POST a JSON to get all Studies corresponding to StudyDate range.
        sight::io::http::Request::sptr request = sight::io::http::Request::New(pacsServer + "/tools/find");
        QByteArray studiesListAnswer;
        try
        {
            studiesListAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
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
        QJsonDocument jsonResponse         = QJsonDocument::fromJson(studiesListAnswer);
        const QJsonArray& studiesListArray = jsonResponse.array();
        const int studiesListArraySize     = studiesListArray.count();

        for(int i = 0; i < studiesListArraySize; ++i)
        {
            const std::string& studiesUID = studiesListArray.at(i).toString().toStdString();
            const std::string studiesUrl(pacsServer + "/studies/" + studiesUID);
            const QByteArray& studiesAnswer = m_clientQt.get( sight::io::http::Request::New(studiesUrl));

            jsonResponse = QJsonDocument::fromJson(studiesAnswer);
            const QJsonObject& jsonObj    = jsonResponse.object();
            const QJsonArray& seriesArray = jsonObj["Series"].toArray();
            const int seriesArraySize     = seriesArray.count();

            for(int i = 0; i < seriesArraySize; ++i)
            {
                const std::string& seriesUID = seriesArray.at(i).toString().toStdString();
                const std::string instancesUrl(pacsServer + "/series/" + seriesUID);
                const QByteArray& instancesAnswer = m_clientQt.get( sight::io::http::Request::New(instancesUrl));
                jsonResponse = QJsonDocument::fromJson(instancesAnswer);
                const QJsonObject& jsonObj      = jsonResponse.object();
                const QJsonArray& instanceArray = jsonObj["Instances"].toArray();

                // Retrieve the first instance for the needed information
                const std::string& instanceUID = instanceArray.at(0).toString().toStdString();
                const std::string instanceUrl(pacsServer + "/instances/" + instanceUID + "/simplified-tags");
                const QByteArray& instance = m_clientQt.get( sight::io::http::Request::New(instanceUrl));

                QJsonObject seriesJson = QJsonDocument::fromJson(instance).object();
                seriesJson.insert( "NumberOfSeriesRelatedInstances", instanceArray.count() );

                // Convert response to DicomSeries
                data::SeriesDB::ContainerType series = sight::io::http::helper::Series::toFwMedData(seriesJson);

                allSeries.insert(std::end(allSeries), std::begin(series), std::end(series));
                this->updateSeriesDB(allSeries);
            }
        }
    }
    catch (sight::io::http::exceptions::Base& exception)
    {
        std::stringstream ss;
        ss << "Unknown error.";
        this->displayErrorMessage(ss.str());
        SLM_WARN(exception.what());
    }
}

//------------------------------------------------------------------------------

void SQueryEditor::updateSeriesDB(data::SeriesDB::ContainerType series)
{
    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB >("seriesDB");
    data::tools::helper::SeriesDB seriesDBHelper(seriesDB);

    // Delete old series from the SeriesDB
    seriesDBHelper.clear();

    // Push new series in the SeriesDB
    for(const data::Series::sptr& s: series)
    {
        data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast(s);
        seriesDBHelper.add(dicomSeries);
    }

    // Notify th SeriesDB
    seriesDBHelper.notify();
}

//------------------------------------------------------------------------------

void SQueryEditor::displayErrorMessage(const std::string& message) const
{
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
    messageBox.setMessage( message );
    messageBox.setIcon(ui::base::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicomweb
