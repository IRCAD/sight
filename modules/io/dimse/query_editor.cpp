/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "query_editor.hpp"

#include <core/runtime/path.hpp>

#include <data/dicom_series.hpp>
#include <data/series_set.hpp>

#include <io/dimse/exceptions/base.hpp>
#include <io/dimse/helper/series.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <dcmtk/dcmnet/scu.h>
#include <dcmtk/ofstd/ofstring.h>

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include <algorithm>
#include <vector>

namespace sight::module::io::dimse
{

static const std::string ADVANCED_CONFIG    = "advanced";
static const std::string ICON_PATH_CONFIG   = "icon";
static const std::string ICON_WIDTH_CONFIG  = "width";
static const std::string ICON_HEIGHT_CONFIG = "height";

//------------------------------------------------------------------------------

query_editor::query_editor() noexcept :
    sight::service::notifier(m_signals)
{
}

//------------------------------------------------------------------------------

void query_editor::configuring()
{
    sight::ui::service::initialize();

    const auto config_tree = this->get_config();
    const auto config      = config_tree.get_child_optional("config.<xmlattr>");
    if(config)
    {
        const auto icon_path = config->get_optional<std::string>(ICON_PATH_CONFIG);
        if(icon_path)
        {
            m_icon_path = core::runtime::get_module_resource_file_path(icon_path.value());
        }

        m_advanced    = config->get<bool>(ADVANCED_CONFIG, m_advanced);
        m_icon_width  = config->get<unsigned int>(ICON_WIDTH_CONFIG, m_icon_width);
        m_icon_height = config->get<unsigned int>(ICON_HEIGHT_CONFIG, m_icon_height);
    }
}

//------------------------------------------------------------------------------

void query_editor::starting()
{
    // Create the worker.
    m_request_worker = core::thread::worker::make();

    // Create the GUI.
    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* const main_layout = new QVBoxLayout();

    m_search_edit = new QLineEdit();
    m_search_edit->setPlaceholderText("Find by name, birth date, ID, date, description or modality");
    m_search_button = new QPushButton("Search");

    if(!m_icon_path.empty())
    {
        m_search_button->setText("");
        m_search_button->setIcon(QIcon(QString::fromStdString(m_icon_path.string())));
        if(m_icon_width > 0 && m_icon_height > 0)
        {
            m_search_button->setIconSize(QSize(int(m_icon_width), int(m_icon_height)));
        }
    }

    auto* const search_layout = new QHBoxLayout();
    search_layout->setObjectName("query_editor_search");

    search_layout->addWidget(m_search_edit);
    search_layout->addWidget(m_search_button);
    main_layout->addLayout(search_layout);

    QObject::connect(m_search_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
    QObject::connect(m_search_button, &QPushButton::clicked, this, &query_editor::execute_query_async);

    if(m_advanced)
    {
        auto* const advanced_layout = new QHBoxLayout();
        advanced_layout->setObjectName("query_editor_advanced");

        auto* const label_layout = new QVBoxLayout();
        label_layout->setObjectName("query_editor_labels");

        auto* const date_label = new QLabel("Date of the study");
        date_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* const name_label = new QLabel("Patient's name");
        name_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* const birth_date_label = new QLabel("Patient's birth date");
        birth_date_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* const id_label = new QLabel("Patient's ID");
        id_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* const series_uid_label = new QLabel("Series' ID");
        series_uid_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* const description_label = new QLabel("Series' description");
        description_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* const modality_label = new QLabel("Modality");
        modality_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        label_layout->addWidget(date_label);
        label_layout->addWidget(new QLabel());
        label_layout->addWidget(name_label);
        label_layout->addWidget(birth_date_label);
        label_layout->addWidget(id_label);
        label_layout->addWidget(series_uid_label);
        label_layout->addWidget(description_label);
        label_layout->addWidget(modality_label);

        auto* const date_layout = new QHBoxLayout();

        auto* const date_label_layout = new QVBoxLayout();

        auto* const from_label = new QLabel("from");
        from_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        auto* const to_label = new QLabel("to");
        to_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        date_label_layout->addWidget(from_label);
        date_label_layout->addWidget(to_label);

        auto* const date_edit_layout = new QVBoxLayout();

        m_begin_study_date_edit = new QDateEdit();
        m_begin_study_date_edit->setDate(QDate());
        m_begin_study_date_edit->setDisplayFormat("MM.dd.yyyy");
        m_end_study_date_edit = new QDateEdit();
        m_end_study_date_edit->setDate(QDate::currentDate());
        m_end_study_date_edit->setDisplayFormat("MM.dd.yyyy");
        date_edit_layout->addWidget(m_begin_study_date_edit);
        date_edit_layout->addWidget(m_end_study_date_edit);

        date_layout->addLayout(date_label_layout, 0);
        date_layout->addLayout(date_edit_layout, 1);

        auto* const edit_layout = new QVBoxLayout();
        edit_layout->setObjectName("query_editor_editors");

        m_patient_name_edit = new QLineEdit();

        auto* const birth_date_edit_layout = new QHBoxLayout();

        m_birth_date_edit = new QDateEdit();
        m_birth_date_edit->setDate(QDate());
        m_birth_date_edit->setDisplayFormat("MM.dd.yyyy");
        m_birth_date_edit->setEnabled(false);

        auto* const birth_date_enabler = new QCheckBox();

        birth_date_edit_layout->addWidget(birth_date_enabler, 0);
        birth_date_edit_layout->addWidget(m_birth_date_edit, 1);

        m_patient_uid_edit        = new QLineEdit();
        m_series_uid_edit         = new QLineEdit();
        m_series_description_edit = new QLineEdit();
        m_series_modality_edit    = new QLineEdit();

        edit_layout->addLayout(date_layout);
        edit_layout->addWidget(m_patient_name_edit);
        edit_layout->addLayout(birth_date_edit_layout);
        edit_layout->addWidget(m_patient_uid_edit);
        edit_layout->addWidget(m_series_uid_edit);
        edit_layout->addWidget(m_series_description_edit);
        edit_layout->addWidget(m_series_modality_edit);

        advanced_layout->addLayout(label_layout);
        advanced_layout->addLayout(edit_layout);

        main_layout->addLayout(advanced_layout);

        // Connect signals.
        QObject::connect(m_patient_name_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
        QObject::connect(birth_date_enabler, &QCheckBox::stateChanged, this, &query_editor::enable_birth_date_edit);
        QObject::connect(m_patient_uid_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
        QObject::connect(m_series_uid_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
        QObject::connect(
            m_series_description_edit,
            &QLineEdit::returnPressed,
            this,
            &query_editor::execute_query_async
        );
        QObject::connect(m_series_modality_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
    }

    qt_container->set_layout(main_layout);
}

//------------------------------------------------------------------------------

void query_editor::updating()
{
    this->execute_query_async();
}

//------------------------------------------------------------------------------

void query_editor::stopping()
{
    QObject::disconnect(m_search_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
    QObject::disconnect(m_search_button, &QPushButton::clicked, this, &query_editor::execute_query_async);
    QObject::disconnect(m_patient_name_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
    QObject::disconnect(m_patient_uid_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
    QObject::disconnect(m_series_uid_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
    QObject::disconnect(m_series_description_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);
    QObject::disconnect(m_series_modality_edit, &QLineEdit::returnPressed, this, &query_editor::execute_query_async);

    // Stop worker.
    m_request_worker->stop();
    m_request_worker.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void query_editor::execute_query_async()
{
    if(!m_is_querying)
    {
        m_request_worker->post([this](auto&& ...){execute_query();});
    }
    else
    {
        this->notifier::info("Already querying");
        return;
    }
}

//------------------------------------------------------------------------------

void query_editor::execute_query()
{
    m_is_querying = true;

    auto series_enquirer = std::make_shared<sight::io::dimse::series_enquirer>();

    // Initialize connection.
    try
    {
        const auto pacs_configuration = m_config.lock();

        series_enquirer->initialize(
            pacs_configuration->get_local_application_title(),
            pacs_configuration->get_pacs_host_name(),
            pacs_configuration->get_pacs_application_port(),
            pacs_configuration->get_pacs_application_title()
        );
        series_enquirer->connect();
    }
    catch(const sight::io::dimse::exceptions::base& e)
    {
        SIGHT_ERROR("Can't establish a connection with the PACS: " + std::string(e.what()));
        this->notifier::failure("Can't connect to the PACS");
        m_is_querying = false;
        return;
    }

    const std::function<std::string(std::string)> standardise =
        [](std::string _s)
        {
            std::string res = _s;
            std::transform(
                _s.begin(),
                _s.end(),
                res.begin(),
                [](unsigned char _c)
            {
                return std::tolower(_c);
            });
            return res;
        };

    try
    {
        // Execute find requests.
        const std::string search_value = standardise(m_search_edit->text().toStdString());
        std::string begin_data_search_value;
        std::string end_date_search_value;
        std::string name_search_value;
        std::string birth_date_search_value;
        std::string patient_uid_search_value;
        std::string series_uid_search_value;
        std::string description_search_value;
        std::string modality_search_value;

        if(m_advanced)
        {
            begin_data_search_value = m_begin_study_date_edit->date().toString("yyyyMMdd").toStdString();
            end_date_search_value   = m_end_study_date_edit->date().toString("yyyyMMdd").toStdString();
            name_search_value       = standardise(m_patient_name_edit->text().toStdString());
            if(m_birth_date_edit->isEnabled())
            {
                birth_date_search_value = m_birth_date_edit->date().toString("yyyyMMdd").toStdString();
            }

            patient_uid_search_value = standardise(m_patient_uid_edit->text().toStdString());
            series_uid_search_value  = standardise(m_series_uid_edit->text().toStdString());
            description_search_value = standardise(m_series_description_edit->text().toStdString());
            modality_search_value    = standardise(m_series_modality_edit->text().toStdString());
        }

        OFList<QRResponse*> responses;

        // If one of the advanced editor is filled, with find series with it's field to improve the research.
        if(!name_search_value.empty())
        {
            responses = series_enquirer->find_series_by_patient_name(name_search_value);
        }
        else if(!birth_date_search_value.empty())
        {
            responses = series_enquirer->find_series_by_patient_birth_date(birth_date_search_value);
        }
        else if(!patient_uid_search_value.empty())
        {
            responses = series_enquirer->find_series_by_patient_uid(patient_uid_search_value);
        }
        else if(!series_uid_search_value.empty())
        {
            responses = series_enquirer->find_series_by_uid(series_uid_search_value);
        }
        else if(!description_search_value.empty())
        {
            responses = series_enquirer->find_series_by_description(description_search_value);
        }
        else if(!modality_search_value.empty())
        {
            responses = series_enquirer->find_series_by_modality(modality_search_value);
        }
        // By default, check by date if the advanced mode is enable.
        else if(m_advanced)
        {
            responses = series_enquirer->find_series_by_date(begin_data_search_value, end_date_search_value);
        }
        // Else, retrieve all patient.
        else
        {
            responses = series_enquirer->find_series_by_patient_name("");
        }

        // filter all results.
        std::vector<QRResponse*> series_response;
        for(QRResponse* response : responses)
        {
            // Be sure we are not in the last response which does not have a dataset.
            if(response->m_dataset != nullptr)
            {
                OFString series_uid;
                response->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, series_uid);

                const auto finded_it = std::find_if(
                    series_response.begin(),
                    series_response.end(),
                    [&](const QRResponse* _res) -> bool
                    {
                        OFString data;
                        _res->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, data);
                        return data == series_uid;
                    });
                // If it's not already added, we check if the series match advanced request.
                if(finded_it == series_response.end())
                {
                    // Check if each tag match the request one.
                    OFString of_value;

                    response->m_dataset->findAndGetOFStringArray(DCM_StudyDate, of_value);
                    QDate study_date = QDate::fromString(of_value.c_str(), "yyyyMMdd");
                    // Check date if the advanced mode is enabled.
                    if(m_advanced)
                    {
                        if(study_date <= m_begin_study_date_edit->date() || study_date >= m_end_study_date_edit->date())
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_PatientBirthDate, of_value);
                    const std::string patient_birth_date = of_value.c_str();
                    if(!birth_date_search_value.empty())
                    {
                        if(patient_birth_date.find(birth_date_search_value) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_PatientName, of_value);
                    const std::string patient_name = standardise(of_value.c_str());
                    if(!name_search_value.empty())
                    {
                        if(patient_name.find(name_search_value) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_PatientID, of_value);
                    const std::string patient_id = standardise(of_value.c_str());
                    if(!patient_uid_search_value.empty())
                    {
                        if(patient_id.find(patient_uid_search_value) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, of_value);
                    const std::string series_instance_uid = standardise(of_value.c_str());
                    if(!series_uid_search_value.empty())
                    {
                        if(series_instance_uid.find(series_uid_search_value) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_SeriesDescription, of_value);
                    const std::string series_description = standardise(of_value.c_str());
                    if(!description_search_value.empty())
                    {
                        if(series_description.find(description_search_value) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    response->m_dataset->findAndGetOFStringArray(DCM_Modality, of_value);
                    const std::string modality = standardise(of_value.c_str());
                    if(!modality_search_value.empty())
                    {
                        if(modality.find(modality_search_value) == std::string::npos)
                        {
                            continue;
                        }
                    }

                    // Check if each tag match at least the main research value.
                    if(study_date.toString("MM/dd/yyyy").toStdString().find(search_value) != std::string::npos
                       || patient_name.find(search_value) != std::string::npos
                       || patient_birth_date.find(search_value) != std::string::npos
                       || patient_id.find(search_value) != std::string::npos
                       || series_instance_uid.find(search_value) != std::string::npos
                       || series_description.find(search_value) != std::string::npos
                       || modality.find(search_value) != std::string::npos)
                    {
                        series_response.push_back(response);
                    }
                }
            }
        }

        // Convert response to DicomSeries.
        OFList<QRResponse*> of_series_response;
        for(QRResponse* res : series_response)
        {
            of_series_response.push_back(res);
        }

        data::series_set::container_t series = sight::io::dimse::helper::series::to_fw_med_data(of_series_response);

        // Clean memory.
        sight::io::dimse::helper::series::release_responses(responses);

        // Check whether the instance number start at 1 or 0.
        for(const data::series::sptr& s : series)
        {
            data::dicom_series::sptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>(s);
            SIGHT_ASSERT("The PACS response should contain only DicomSeries", dicom_series);
            const std::string instance_uid =
                series_enquirer->find_sop_instance_uid(dicom_series->get_series_instance_uid(), 0);
            dicom_series->set_first_instance_number((instance_uid.empty() ? 1 : 0));
        }

        this->update_series_set(series);
    }
    catch(const sight::io::dimse::exceptions::base& e)
    {
        SIGHT_ERROR("Can't execute query to the PACS: " + std::string(e.what()));
        this->notifier::failure("Can't execute query");
    }

    if(series_enquirer->is_connected_to_pacs())
    {
        series_enquirer->disconnect();
    }

    m_is_querying = false;
}

//------------------------------------------------------------------------------

void query_editor::update_series_set(const data::series_set::container_t& _series)
{
    const auto series_set     = m_series_set.lock();
    const auto scoped_emitter = series_set->scoped_emit();

    // Delete old series from the series_set.
    series_set->clear();

    // Push new series in the series_set.
    for(const auto& s : _series)
    {
        series_set->push_back(std::dynamic_pointer_cast<data::dicom_series>(s));
    }
}

//------------------------------------------------------------------------------

void query_editor::enable_birth_date_edit(int _enable)
{
    m_birth_date_edit->setEnabled(_enable != 0);
}

} // namespace sight::module::io::dimse
