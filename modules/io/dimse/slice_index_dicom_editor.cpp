/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include "slice_index_dicom_editor.hpp"

#include <core/com/slots.hxx>
#include <core/os/temp_path.hpp>

#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>

#include <io/dimse/exceptions/base.hpp>
#include <io/dimse/series_enquirer.hpp>

#include <service/extension/config.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>

namespace sight::module::io::dimse
{

static const std::string DELAY_CONFIG        = "delay";
static const std::string DICOM_READER_CONFIG = "dicomReader";
static const std::string READER_CONFIG       = "readerConfig";

//------------------------------------------------------------------------------

slice_index_dicom_editor::slice_index_dicom_editor() noexcept :
    sight::service::notifier(m_signals)
{
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::configuring(const config_t& _config)
{
    sight::ui::service::initialize();

    const config_t config = _config.get_child("config.<xmlattr>");

    m_delay                       = config.get<unsigned>(DELAY_CONFIG, m_delay);
    m_dicom_reader_implementation = config.get(DICOM_READER_CONFIG, m_dicom_reader_implementation);
    SIGHT_ERROR_IF("'" + DICOM_READER_CONFIG + "' attribute not set", m_dicom_reader_implementation.empty())

    m_reader_config = _config.get(READER_CONFIG, m_reader_config);
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::starting()
{
    // Create the worker.
    m_request_worker = core::thread::worker::make();

    // Create the DICOM reader.
    m_series_set = std::make_shared<data::series_set>();

    m_dicom_reader = this->register_service<sight::io::service::reader>(m_dicom_reader_implementation);
    SIGHT_ASSERT("Unable to create a reader of type '" + m_dicom_reader_implementation + "'", m_dicom_reader);
    m_dicom_reader->set_worker(m_request_worker);
    m_dicom_reader->set_inout(m_series_set, "data");

    if(!m_reader_config.empty())
    {
        const auto reader_config =
            sight::service::extension::config::get_default()->get_service_config(
                m_reader_config,
                "sight::io::service::reader"
            );

        SIGHT_ASSERT(
            "No service configuration " << m_reader_config << " for sight::io::service::reader",
            !reader_config.empty()
        );

        m_dicom_reader->set_config(reader_config);
    }

    m_dicom_reader->configure();
    m_dicom_reader->start().wait();
    SIGHT_ASSERT("'" + m_dicom_reader_implementation + "' is not started", m_dicom_reader->started());

    // Create the timer used to retrieve a slice.
    m_slice_trigger = this->worker()->create_timer();
    m_slice_trigger->set_function(
        [&]()
        {
            this->retrieve_slice();
        });
    m_slice_trigger->set_duration(std::chrono::milliseconds(m_delay));
    m_slice_trigger->set_one_shot(true);

    // Create the slider.
    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* layout = new QHBoxLayout();

    m_slider = new QSlider(Qt::Horizontal);
    layout->addWidget(m_slider, 1);

    m_line_edit = new QLineEdit();
    layout->addWidget(m_line_edit, 0);
    m_line_edit->setReadOnly(true);
    m_line_edit->setMaximumWidth(80);

    qt_container->set_layout(layout);

    // Connect the slider to the slot.
    QObject::connect(
        m_slider,
        &QSlider::valueChanged,
        this,
        &module::io::dimse::slice_index_dicom_editor::change_slice_index
    );

    // Update informations.
    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t slice_index_dicom_editor::auto_connections() const
{
    service::connections_t connections;
    connections.push(DICOMSERIES_INOUT, data::dicom_series::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::updating()
{
    // Retrieve the DICOM series and its informations.
    const auto dicom_series        = m_series.lock();
    const std::size_t slice_number = dicom_series->num_instances();

    if(slice_number > 0)
    {
        // If the current slice index is the initial value of the slider, we just send a signal to trigger other
        // services.
        const int current_slice = m_slider->value();
        if(current_slice == static_cast<int>(slice_number / 2))
        {
            this->change_slice_index(current_slice);
        }
        else
        {
            // Fill slider informations.
            m_slider->setRange(0, static_cast<int>(slice_number - 1));
            m_slider->setValue(static_cast<int>(slice_number / 2));

            this->set_slider_information(static_cast<unsigned int>(slice_number / 2));
        }
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::stopping()
{
    this->unregister_services();

    // Stop the worker.
    m_request_worker->stop();
    m_request_worker.reset();

    this->destroy();
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::change_slice_index(int _value)
{
    this->set_slider_information(static_cast<unsigned int>(_value));

    // Get the new slice if there is no change for m_delay milliseconds.
    m_slice_trigger->start();
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::set_slider_information(unsigned _value)
{
    std::stringstream text;
    text << _value << " / " << m_slider->maximum();
    m_line_edit->setText(std::string(text.str()).c_str());
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::retrieve_slice()
{
    // Check if the slice already exists.
    const auto dicom_series                = m_series.lock();
    const std::size_t selected_slice_index = std::size_t(m_slider->value()) + dicom_series->get_first_instance_number();
    const bool is_instance_available       = dicom_series->is_instance_available(selected_slice_index);

    // If the slice is not pulled, pull it.
    if(!is_instance_available)
    {
        m_request_worker->post([this, selected_slice_index](auto&& ...){pull_slice(selected_slice_index);});
    }
    else
    {
        this->read_slice(dicom_series, selected_slice_index);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::pull_slice(std::size_t _selected_slice_index) const
{
    bool success = false;

    // Retrieve informations.
    const auto pacs_config = m_config.lock();

    auto series_enquirer = std::make_shared<sight::io::dimse::series_enquirer>();

    // Initialize connection.
    try
    {
        series_enquirer->initialize(
            pacs_config->get_local_application_title(),
            pacs_config->get_pacs_host_name(),
            pacs_config->get_pacs_application_port(),
            pacs_config->get_pacs_application_title(),
            pacs_config->get_move_application_title()
        );
        series_enquirer->connect();
    }
    catch(const sight::io::dimse::exceptions::base& e)
    {
        SIGHT_ERROR("Unable to establish a connection with the PACS: " + std::string(e.what()));
        this->notifier::failure("Unable to connect to PACS");
    }

    const auto dicom_series = m_series.lock();

    // Get selected slice.
    try
    {
        const std::string series_instance_uid = dicom_series->get_series_instance_uid();
        const std::string sop_instance_uid    =
            series_enquirer->find_sop_instance_uid(
                series_instance_uid,
                static_cast<unsigned int>(_selected_slice_index)
            );

        // Check if an instance with the selected Instance Number is found on the PACS.
        if(!sop_instance_uid.empty())
        {
            // Pull selected series and save it to the temporary folder.
            switch(pacs_config->get_retrieve_method())
            {
                case sight::io::dimse::data::pacs_configuration::retrieve_method::get:
                    series_enquirer->pull_instance_using_get_retrieve_method(series_instance_uid, sop_instance_uid);
                    break;

                case sight::io::dimse::data::pacs_configuration::retrieve_method::move:
                    series_enquirer->pull_instance_using_move_retrieve_method(series_instance_uid, sop_instance_uid);
                    break;

                default:
                    SIGHT_ERROR("Unknown retrieve method, 'get' will be used");
                    series_enquirer->pull_instance_using_get_retrieve_method(series_instance_uid, sop_instance_uid);
                    break;
            }

            // Compute the path and add it to the DICOM series.
            std::filesystem::path tmp_path      = core::os::temp_dir::shared_directory() / "dicom";
            std::filesystem::path download_path = tmp_path / series_instance_uid / sop_instance_uid;
            dicom_series->add_dicom_path(_selected_slice_index, download_path);

            success = true;
        }
        else
        {
            this->notifier::failure("No instance found");
        }
    }
    catch(const sight::io::dimse::exceptions::base& e)
    {
        SIGHT_ERROR("Unable to execute query to the PACS: " + std::string(e.what()));
        this->notifier::failure("Unable to execute query");
    }
    catch(const std::filesystem::filesystem_error& e)
    {
        SIGHT_ERROR(std::string(e.what()));
    }

    // Disconnect the series enquirer.
    if(series_enquirer->is_connected_to_pacs())
    {
        series_enquirer->disconnect();
    }

    if(success)
    {
        this->read_slice(dicom_series, _selected_slice_index);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::read_slice(
    const data::mt::locked_ptr<data::dicom_series>& _dicom_series,
    std::size_t _selected_slice_index
) const
{
    // Retrieve informations.
    const auto type = _dicom_series->get_dicom_type();
    if(type == data::series::dicom_t::image)
    {
        this->notifier::info("Unable to read the modality '" + _dicom_series->get_modality_string() + "'");
        return;
    }

    // Get the DICOM buffer to write in a temporary folder.
    const auto& binaries = _dicom_series->get_dicom_container();
    auto iter            = binaries.find(_selected_slice_index);
    SIGHT_ASSERT("Index '" << _selected_slice_index << "' is not found in DicomSeries", iter != binaries.end());
    const core::memory::buffer_object::sptr buffer_obj = iter->second;
    const core::memory::buffer_object::lock_t locker_dest(buffer_obj);
    const char* buffer            = static_cast<char*>(locker_dest.buffer());
    const std::size_t buffer_size = buffer_obj->size();

    // Creates unique temporary folder to save the DICOM instance.
    // Do not delete the folder, as we may use the file in the DicomReference.
    const auto tmp_dir = core::os::temp_dir::shared_directory("do_not_delete_");

    // Open the temporary folder and write the buffer.
    std::filesystem::path path = tmp_dir / std::to_string(_selected_slice_index);
    std::ofstream fs(path, std::ios::binary | std::ios::trunc);
    if(!fs.good())
    {
        SIGHT_ERROR("Unable to open '" << path << "' for write.");
        return;
    }

    fs.write(buffer, std::streamsize(buffer_size));
    fs.close();

    // Read the image.
    m_dicom_reader->set_folder(tmp_dir);
    m_dicom_reader->update().wait();

    if(!m_dicom_reader->has_failed() && !m_series_set->empty())
    {
        // Copy the read series to the image.
        const auto image_series = std::dynamic_pointer_cast<data::image_series>(m_series_set->front());

        const auto image = m_image.lock();
        image->deep_copy(image_series);

        data::integer::sptr axial_index   = std::make_shared<data::integer>(0);
        data::integer::sptr frontal_index =
            std::make_shared<data::integer>(static_cast<std::int64_t>(image->size()[0] / 2));
        data::integer::sptr sagittal_index =
            std::make_shared<data::integer>(static_cast<std::int64_t>(image->size()[1] / 2));

        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::orientation_t::axial,
            axial_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::orientation_t::frontal,
            frontal_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::orientation_t::sagittal,
            sagittal_index->value()
        );

        // Send the signal
        const auto sig = image->signal<data::image::modified_signal_t>(data::image::MODIFIED_SIG);
        sig->async_emit();
    }
    else
    {
        SIGHT_ERROR("Unable to read the image");
        this->notifier::failure("Unable to read the image");
    }
}

} // namespace sight::module::io::dimse.
