/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "ioPacs/SQueryEditor.hpp"

#include <core/runtime/operations.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwPacsIO/exceptions/Base.hpp>
#include <fwPacsIO/helper/Series.hpp>

#include <fwServices/macros.hpp>

#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/ofstd/ofstring.h>

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <algorithm>
#include <vector>

namespace ioPacs
{

static const std::string s_ADVANCED_CONFIG    = "advanced";
static const std::string s_ICON_PATH_CONFIG   = "icon";
static const std::string s_ICON_WIDTH_CONFIG  = "width";
static const std::string s_ICON_HEIGHT_CONFIG = "height";

static const ::fwServices::IService::KeyType s_PACS_INPUT = "pacsConfig";

static const ::fwServices::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

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
    ::fwGui::IGuiContainerSrv::initialize();

    const auto configTree = this->getConfigTree();
    const auto config     = configTree.get_child_optional("config.<xmlattr>");
    if(config)
    {
        const auto iconPath = config->get_optional<std::string>(s_ICON_PATH_CONFIG);
        if(iconPath)
        {
            m_iconPath = core::runtime::getModuleResourceFilePath(iconPath.value());
        }

        m_advanced   = config->get<bool>(s_ADVANCED_CONFIG, m_advanced);
        m_iconWidth  = config->get<unsigned int>(s_ICON_WIDTH_CONFIG, m_iconWidth);
        m_iconHeight = config->get<unsigned int>(s_ICON_HEIGHT_CONFIG, m_iconHeight);
    }
}

//------------------------------------------------------------------------------

void SQueryEditor::starting()
{
    // Create the worker.
    m_requestWorker = core::thread::Worker::New();

    // Create the GUI.
    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QVBoxLayout* const mainLayout = new QVBoxLayout();

    m_searchEdit = new QLineEdit();
    m_searchEdit->setPlaceholderText("Find by name, birth date, ID, date, description or modality");
    m_searchButton = new QPushButton("Search");

    if(!m_iconPath.empty())
    {
        m_searchButton->setText("");
        m_searchButton->setIcon(QIcon(QString::fromStdString(m_iconPath.string())));
        if(m_iconWidth > 0 && m_iconHeight > 0)
        {
            m_searchButton->setIconSize(QSize(m_iconWidth, m_iconHeight));
        }
    }

    QHBoxLayout* const searchLayout = new QHBoxLayout();
    searchLayout->setObjectName("SQueryEditor_search");

    searchLayout->addWidget(m_searchEdit);
    searchLayout->addWidget(m_searchButton);
    mainLayout->addLayout(searchLayout);

    QObject::connect(m_searchEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
    QObject::connect(m_searchButton, &QPushButton::clicked, this, &SQueryEditor::executeQueryAsync);

    if(m_advanced)
    {
        QHBoxLayout* const advancedLayout = new QHBoxLayout();
        advancedLayout->setObjectName("SQueryEditor_advanced");

        QVBoxLayout* const labelLayout = new QVBoxLayout();
        labelLayout->setObjectName("SQueryEditor_labels");

        QLabel* const dateLabel = new QLabel("Date of the study");
        dateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel* const nameLabel = new QLabel("Patient's name");
        nameLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel* const birthDateLabel = new QLabel("Patient's birth date");
        birthDateLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel* const idLabel = new QLabel("Patient's ID");
        idLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel* const seriesUIDLabel = new QLabel("Series's ID");
        seriesUIDLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel* const descriptionLabel = new QLabel("Series's description");
        descriptionLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel* const modalityLabel = new QLabel("Modality");
        modalityLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        labelLayout->addWidget(dateLabel);
        labelLayout->addWidget(new QLabel());
        labelLayout->addWidget(nameLabel);
        labelLayout->addWidget(birthDateLabel);
        labelLayout->addWidget(idLabel);
        labelLayout->addWidget(seriesUIDLabel);
        labelLayout->addWidget(descriptionLabel);
        labelLayout->addWidget(modalityLabel);

        QHBoxLayout* const dateLayout = new QHBoxLayout();

        QVBoxLayout* const dateLabelLayout = new QVBoxLayout();

        QLabel* const fromLabel = new QLabel("from");
        fromLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QLabel* const toLabel = new QLabel("to");
        toLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        dateLabelLayout->addWidget(fromLabel);
        dateLabelLayout->addWidget(toLabel);

        QVBoxLayout* const dateEditLayout = new QVBoxLayout();

        m_beginStudyDateEdit = new QDateEdit();
        m_beginStudyDateEdit->setDate(QDate());
        m_beginStudyDateEdit->setDisplayFormat("MM.dd.yyyy");
        m_endStudyDateEdit = new QDateEdit();
        m_endStudyDateEdit->setDate(QDate::currentDate());
        m_endStudyDateEdit->setDisplayFormat("MM.dd.yyyy");
        dateEditLayout->addWidget(m_beginStudyDateEdit);
        dateEditLayout->addWidget(m_endStudyDateEdit);

        dateLayout->addLayout(dateLabelLayout, 0);
        dateLayout->addLayout(dateEditLayout, 1);

        QVBoxLayout* const editLayout = new QVBoxLayout();
        editLayout->setObjectName("SQueryEditor_editors");

        m_patientNameEdit = new QLineEdit();

        QHBoxLayout* const birthDateEditLayout = new QHBoxLayout();

        m_birthDateEdit = new QDateEdit();
        m_birthDateEdit->setDate(QDate());
        m_birthDateEdit->setDisplayFormat("MM.dd.yyyy");
        m_birthDateEdit->setEnabled(false);

        QCheckBox* const birthDateEnabler = new QCheckBox();

        birthDateEditLayout->addWidget(birthDateEnabler, 0);
        birthDateEditLayout->addWidget(m_birthDateEdit, 1);

        m_patientUIDEdit        = new QLineEdit();
        m_seriesUIDEdit         = new QLineEdit();
        m_seriesDescriptionEdit = new QLineEdit();
        m_seriesModalityEdit    = new QLineEdit();

        editLayout->addLayout(dateLayout);
        editLayout->addWidget(m_patientNameEdit);
        editLayout->addLayout(birthDateEditLayout);
        editLayout->addWidget(m_patientUIDEdit);
        editLayout->addWidget(m_seriesUIDEdit);
        editLayout->addWidget(m_seriesDescriptionEdit);
        editLayout->addWidget(m_seriesModalityEdit);

        advancedLayout->addLayout(labelLayout);
        advancedLayout->addLayout(editLayout);

        mainLayout->addLayout(advancedLayout);

        // Connect signals.
        QObject::connect(m_patientNameEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
        QObject::connect(birthDateEnabler, &QCheckBox::stateChanged, this, &SQueryEditor::enableBirthDateEdit);
        QObject::connect(m_patientUIDEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
        QObject::connect(m_seriesUIDEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
        QObject::connect(m_seriesDescriptionEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
        QObject::connect(m_seriesModalityEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
    }

    qtContainer->setLayout(mainLayout);
}

//------------------------------------------------------------------------------

void SQueryEditor::updating()
{
    this->executeQueryAsync();
}

//------------------------------------------------------------------------------

void SQueryEditor::stopping()
{
    QObject::disconnect(m_searchEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
    QObject::disconnect(m_searchButton, &QPushButton::clicked, this, &SQueryEditor::executeQueryAsync);
    QObject::disconnect(m_patientNameEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
    QObject::disconnect(m_patientUIDEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
    QObject::disconnect(m_seriesUIDEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
    QObject::disconnect(m_seriesDescriptionEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);
    QObject::disconnect(m_seriesModalityEdit, &QLineEdit::returnPressed, this, &SQueryEditor::executeQueryAsync);

    // Stop worker.
    m_requestWorker->stop();
    m_requestWorker.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void SQueryEditor::executeQueryAsync()
{
    if(!m_isQuerying)
    {
        m_requestWorker->post(std::bind(&::ioPacs::SQueryEditor::executeQuery, this));
    }
    else
    {
        const auto notif = this->signal< ::fwServices::IService::InfoNotifiedSignalType >(
            ::fwServices::IService::s_INFO_NOTIFIED_SIG);
        notif->asyncEmit("Already querying");
        return;
    }
}

//------------------------------------------------------------------------------

void SQueryEditor::executeQuery()
{
    m_isQuerying = true;

    ::fwPacsIO::SeriesEnquirer::sptr seriesEnquirer = ::fwPacsIO::SeriesEnquirer::New();

    // Initialize connection.
    try
    {
        const auto pacsConfiguration = this->getLockedInput< const ::fwPacsIOdata::PacsConfiguration >(s_PACS_INPUT);

        seriesEnquirer->initialize(
            pacsConfiguration->getLocalApplicationTitle(),
            pacsConfiguration->getPacsHostName(),
            pacsConfiguration->getPacsApplicationPort(),
            pacsConfiguration->getPacsApplicationTitle());
        seriesEnquirer->connect();
    }
    catch(const ::fwPacsIO::exceptions::Base& _e)
    {
        SLM_ERROR("Can't establish a connection with the PACS: " + std::string(_e.what()));
        const auto notif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
            ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
        notif->asyncEmit("Can't connect to the PACS");
        m_isQuerying = false;
        return;
    }

    const std::function< std::string(std::string) > standardise
        = [](std::string _s)
          {
              std::string res = _s;
              std::transform(_s.begin(), _s.end(), res.begin(), [](unsigned char _c)
            {
                return std::tolower(_c);
            });
              return res;
          };

    try
    {
        // Execute find requests.
        const std::string searchValue      = standardise(m_searchEdit->text().toStdString());
        std::string beginDataSearchValue   = "";
        std::string endDateSearchValue     = "";
        std::string nameSearchValue        = "";
        std::string birthDateSearchValue   = "";
        std::string patientUIDSearchValue  = "";
        std::string seriesUIDSearchValue   = "";
        std::string descriptionSearchValue = "";
        std::string modalitySearchValue    = "";

        if(m_advanced)
        {
            beginDataSearchValue = m_beginStudyDateEdit->date().toString("yyyyMMdd").toStdString();
            endDateSearchValue   = m_endStudyDateEdit->date().toString("yyyyMMdd").toStdString();
            nameSearchValue      = standardise(m_patientNameEdit->text().toStdString());
            if(m_birthDateEdit->isEnabled())
            {
                birthDateSearchValue = m_birthDateEdit->date().toString("yyyyMMdd").toStdString();
            }
            patientUIDSearchValue  = standardise(m_patientUIDEdit->text().toStdString());
            seriesUIDSearchValue   = standardise(m_seriesUIDEdit->text().toStdString());
            descriptionSearchValue = standardise(m_seriesDescriptionEdit->text().toStdString());
            modalitySearchValue    = standardise(m_seriesModalityEdit->text().toStdString());
        }

        OFList< QRResponse* > responses;

        // If one of the advanced editor is filled, with find series with it's field to improve the research.
        if(!nameSearchValue.empty())
        {
            responses = seriesEnquirer->findSeriesByPatientName(nameSearchValue);
        }
        else if(!birthDateSearchValue.empty())
        {
            responses = seriesEnquirer->findSeriesByPatientBirthDate(birthDateSearchValue);
        }
        else if(!patientUIDSearchValue.empty())
        {
            responses = seriesEnquirer->findSeriesByPatientUID(patientUIDSearchValue);
        }
        else if(!seriesUIDSearchValue.empty())
        {
            responses = seriesEnquirer->findSeriesByUID(seriesUIDSearchValue);
        }
        else if(!descriptionSearchValue.empty())
        {
            responses = seriesEnquirer->findSeriesByDescription(descriptionSearchValue);
        }
        else if(!modalitySearchValue.empty())
        {
            responses = seriesEnquirer->findSeriesByModality(modalitySearchValue);
        }
        // By default, check by date if the advanced mode is enable.
        else if(m_advanced)
        {
            responses = seriesEnquirer->findSeriesByDate(beginDataSearchValue, endDateSearchValue);
        }
        // Else, retrieve all patient.
        else
        {
            responses = seriesEnquirer->findSeriesByPatientName("");
        }

        // Filter all results.
        std::vector< QRResponse* > seriesResponse;
        for(QRResponse* response : responses)
        {
            // Be sure we are not in the last response which does not have a dataset.
            if(response->m_dataset != nullptr)
            {
                OFString seriesUID;
                response->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, seriesUID);

                const auto findedIt = std::find_if(seriesResponse.begin(), seriesResponse.end(),
                                                   [&](const QRResponse* _res) -> bool
                    {

                        OFString data;
                        _res->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, data);
                        if(data.compare(seriesUID) == 0)
                        {
                            return true;
                        }
                        return false;
                    });
                // If it's not already added, we check if the series match advanced request.
                if(findedIt == seriesResponse.end())
                {
                    // Check if each tag match the request one.
                    OFString ofValue;

                    response->m_dataset->findAndGetOFStringArray(DCM_StudyDate, ofValue);
                    QDate studyDate = QDate::fromString(ofValue.c_str(), "yyyyMMdd");
                    // Check date if the advanced mode is enabled.
                    if(m_advanced)
                    {
                        if(studyDate <= m_beginStudyDateEdit->date() || studyDate >= m_endStudyDateEdit->date())
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_PatientBirthDate, ofValue);
                    const std::string patientBirthDate = ofValue.c_str();
                    if(!birthDateSearchValue.empty())
                    {
                        if(patientBirthDate.find(birthDateSearchValue) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_PatientName, ofValue);
                    const std::string patientName = standardise(ofValue.c_str());
                    if(!nameSearchValue.empty())
                    {
                        if(patientName.find(nameSearchValue) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_PatientID, ofValue);
                    const std::string patientID = standardise(ofValue.c_str());
                    if(!patientUIDSearchValue.empty())
                    {
                        if(patientID.find(patientUIDSearchValue) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, ofValue);
                    const std::string seriesInstanceUID = standardise(ofValue.c_str());
                    if(!seriesUIDSearchValue.empty())
                    {
                        if(seriesInstanceUID.find(seriesUIDSearchValue) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_SeriesDescription, ofValue);
                    const std::string seriesDescription = standardise(ofValue.c_str());
                    if(!descriptionSearchValue.empty())
                    {
                        if(seriesDescription.find(descriptionSearchValue) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_Modality, ofValue);
                    const std::string modality = standardise(ofValue.c_str());
                    if(!modalitySearchValue.empty())
                    {
                        if(modality.find(modalitySearchValue) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    // Check if each tag match at least the main research value.
                    if(studyDate.toString("MM/dd/yyyy").toStdString().find(searchValue) != std::string::npos ||
                       patientName.find(searchValue) != std::string::npos ||
                       patientBirthDate.find(searchValue) != std::string::npos ||
                       patientID.find(searchValue) != std::string::npos ||
                       seriesInstanceUID.find(searchValue) != std::string::npos ||
                       seriesDescription.find(searchValue) != std::string::npos ||
                       modality.find(searchValue) != std::string::npos)
                    {
                        seriesResponse.push_back(response);
                    }
                }
            }
        }

        // Convert response to DicomSeries.
        OFList< QRResponse* > ofSeriesResponse;
        for(QRResponse* res : seriesResponse)
        {
            ofSeriesResponse.push_back(res);
        }
        ::fwMedData::SeriesDB::ContainerType series = ::fwPacsIO::helper::Series::toFwMedData(ofSeriesResponse);

        // Clean memory.
        ::fwPacsIO::helper::Series::releaseResponses(responses);

        // Check whether the instance number start at 1 or 0.
        for(::fwMedData::Series::sptr s: series)
        {
            ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(s);
            SLM_ASSERT("The PACS response should contain only DicomSeries", dicomSeries);
            const std::string instanceUID = seriesEnquirer->findSOPInstanceUID(dicomSeries->getInstanceUID(), 0);
            dicomSeries->setFirstInstanceNumber((instanceUID.empty() ? 1 : 0));
        }

        this->updateSeriesDB(series);
    }
    catch(const ::fwPacsIO::exceptions::Base& _e)
    {
        SLM_ERROR("Can't execute query to the PACS: " + std::string(_e.what()));
        const auto notif = this->signal< ::fwServices::IService::FailureNotifiedSignalType >(
            ::fwServices::IService::s_FAILURE_NOTIFIED_SIG);
        notif->asyncEmit("Can't execture query");
    }

    if(seriesEnquirer->isConnectedToPacs())
    {
        seriesEnquirer->disconnect();
    }

    m_isQuerying = false;
}

//------------------------------------------------------------------------------

void SQueryEditor::updateSeriesDB(const ::fwMedData::SeriesDB::ContainerType& _series)
{
    const auto seriesDB = this->getLockedInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);

    ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB.get_shared());

    // Delete old series from the SeriesDB.
    seriesDBHelper.clear();

    // Push new series in the SeriesDB.
    for(const ::fwMedData::Series::sptr& s: _series)
    {
        ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(s);
        seriesDBHelper.add(dicomSeries);
    }

    // Notify the SeriesDB.
    seriesDBHelper.notify();
}

//------------------------------------------------------------------------------

void SQueryEditor::enableBirthDateEdit(int _enable)
{
    m_birthDateEdit->setEnabled(_enable);
}

} // namespace ioPacs
