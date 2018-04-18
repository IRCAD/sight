/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicomWeb/SQueryEditor.hpp"

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwNetwork/helper/Series.hpp>

#include <fwPreferences/helper.hpp>

#include <fwServices/macros.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include <dcmtk/dcmnet/scu.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>

namespace ioDicomWeb
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::ioDicomWeb::SQueryEditor, ::fwMedData::SeriesDB );

//------------------------------------------------------------------------------

SQueryEditor::SQueryEditor() noexcept
{
}
//------------------------------------------------------------------------------

SQueryEditor::~SQueryEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SQueryEditor::info(std::ostream& _sstream )
{
    _sstream << "SQueryEditor::info";
}

//------------------------------------------------------------------------------

void SQueryEditor::configuring()
{
    ::fwServices::IService::ConfigType configuration = this->getConfigTree();
    //Parse server port and hostname
    if(configuration.count("server"))
    {
        const std::string serverInfo = configuration.get("server", "");
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

    ::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

std::string SQueryEditor::getPreferenceKey(const std::string& key) const
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

void SQueryEditor::starting()
{
    SLM_TRACE_FUNC();
    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

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
    SLM_TRACE_FUNC();

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
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SQueryEditor::queryPatientName()
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

    try
    {
        // Vector of all Series that will be retrieved.
        ::fwMedData::SeriesDB::ContainerType allSeries;

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
        ::fwNetwork::http::Request::sptr request = ::fwNetwork::http::Request::New(pacsServer + "/tools/find");
        const QByteArray& seriesAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
        QJsonDocument jsonResponse     = QJsonDocument::fromJson(seriesAnswer);
        const QJsonArray& seriesArray  = jsonResponse.array();
        const size_t seriesArraySize   = seriesArray.count();

        for(size_t i = 0; i < seriesArraySize; ++i)
        {

            const std::string& seriesUID = seriesArray.at(i).toString().toStdString();
            const std::string instancesUrl(pacsServer + "/series/" + seriesUID);
            const QByteArray& instancesAnswer = m_clientQt.get( ::fwNetwork::http::Request::New(instancesUrl));
            jsonResponse = QJsonDocument::fromJson(instancesAnswer);
            const QJsonObject& jsonObj       = jsonResponse.object();
            const QJsonArray& instancesArray = jsonObj["Instances"].toArray();

            const size_t instancesArraySize = instancesArray.count();
            for(size_t j = 0; j < instancesArraySize; ++j)
            {
                const std::string& instanceUID = instancesArray.at(j).toString().toStdString();
                const std::string instancesUrl(pacsServer + "/instances/" + instanceUID + "/simplified-tags");
                const QByteArray& instance = m_clientQt.get( ::fwNetwork::http::Request::New(instancesUrl));

                // Convert response to DicomSeries
                ::fwMedData::SeriesDB::ContainerType series = ::fwNetwork::helper::Series::toFwMedData(instance);

                allSeries.insert(std::end(allSeries), std::begin(series), std::end(series));
                this->updateSeriesDB(allSeries);
            }
        }
    }
    catch (::fwNetwork::exceptions::Base& exception)
    {
        this->displayErrorMessage(exception.what());
    }

}

//------------------------------------------------------------------------------

void SQueryEditor::queryStudyDate()
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

    try
    {
        // Vector of all Series that will be retrieved.
        ::fwMedData::SeriesDB::ContainerType allSeries;

        // Find Studies according to their StudyDate
        QJsonObject query;
        const std::string& beginDate = m_beginStudyDateEdit->date().toString("yyyyMMdd").toStdString();
        const std::string& endDate = m_endStudyDateEdit->date().toString("yyyyMMdd").toStdString();
        const std::string& dateRange = beginDate + "-" + endDate;
        query.insert("StudyDate", dateRange.c_str());

        QJsonObject body;
        body.insert("Level", "Studies");
        body.insert("Query", query);
        body.insert("Limit", 0);

        /// Url PACS
        const std::string pacsServer("http://" + m_serverHostname + ":" + std::to_string(m_serverPort));

        /// Orthanc "/tools/find" route. POST a JSON to get all Studies corresponding to StudyDate range.
        ::fwNetwork::http::Request::sptr request = ::fwNetwork::http::Request::New(pacsServer + "/tools/find");
        const QByteArray& studiesListAnswer = m_clientQt.post(request, QJsonDocument(body).toJson());
        QJsonDocument jsonResponse     = QJsonDocument::fromJson(studiesListAnswer);
        const QJsonArray& studiesListArray  = jsonResponse.array();
        const size_t studiesListArraySize   = studiesListArray.count();

        for(size_t i = 0; i < studiesListArraySize; ++i)
        {
            const std::string& studiesUID = studiesListArray.at(i).toString().toStdString();
            const std::string studiesUrl(pacsServer + "/studies/" + studiesUID);
            const QByteArray& studiesAnswer = m_clientQt.get( ::fwNetwork::http::Request::New(studiesUrl));

            jsonResponse = QJsonDocument::fromJson(studiesAnswer);
            const QJsonObject& jsonObj       = jsonResponse.object();
            const QJsonArray& seriesArray = jsonObj["Series"].toArray();
            const size_t seriesArraySize   = seriesArray.count();

            for(size_t i = 0; i < seriesArraySize; ++i)
            {
                const std::string& seriesUID = seriesArray.at(i).toString().toStdString();
                const std::string instancesUrl(pacsServer + "/series/" + seriesUID);
                const QByteArray& instancesAnswer = m_clientQt.get( ::fwNetwork::http::Request::New(instancesUrl));
                jsonResponse = QJsonDocument::fromJson(instancesAnswer);
                const QJsonObject& jsonObj       = jsonResponse.object();
                const QJsonArray& instancesArray = jsonObj["Instances"].toArray();

                const size_t instancesArraySize = instancesArray.count();
                for(size_t j = 0; j < instancesArraySize; ++j)
                {
                    const std::string& instanceUID = instancesArray.at(j).toString().toStdString();
                    const std::string instancesUrl(pacsServer + "/instances/" + instanceUID + "/simplified-tags");
                    const QByteArray& instance = m_clientQt.get( ::fwNetwork::http::Request::New(instancesUrl));

                    // Convert response to DicomSeries
                    ::fwMedData::SeriesDB::ContainerType series = ::fwNetwork::helper::Series::toFwMedData(instance);

                    allSeries.insert(std::end(allSeries), std::begin(series), std::end(series));
                    this->updateSeriesDB(allSeries);
                }
            }
        }
    }
    catch (::fwNetwork::exceptions::Base& exception)
    {
        this->displayErrorMessage(exception.what());
    }
}

//------------------------------------------------------------------------------

void SQueryEditor::updateSeriesDB(::fwMedData::SeriesDB::ContainerType series)
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >("seriesDB");
    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);

    // Delete old series from the SeriesDB
    seriesDBHelper.clear();

    // Push new series in the SeriesDB
    for(const ::fwMedData::Series::sptr& s: series)
    {
        ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(s);
        seriesDBHelper.add(dicomSeries);
    }

    // Notify th SeriesDB
    seriesDBHelper.notify();

}

//------------------------------------------------------------------------------

void SQueryEditor::displayErrorMessage(const std::string& message) const
{
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("Error");
//     messageBox.setMessage( ss.str() );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::CRITICAL);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

} // namespace ioDicomWeb
