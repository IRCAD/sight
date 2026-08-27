/************************************************************************
 *
 * Copyright (C) 2020-2026 IRCAD France
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

#include "core/notification/observer.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>

#include <io/dicom/helper/series.hpp>
#include <io/dicom/reader/file.hpp>
#include <io/dimse/exceptions/base.hpp>
#include <io/dimse/helper/series.hpp>
#include <io/dimse/series_enquirer.hpp>

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>

#include <memory>
#include <utility>

namespace sight::module::io::dimse
{

static const std::string DELAY_CONFIG = "delay";

//------------------------------------------------------------------------------

slice_index_dicom_editor::slice_index_dicom_editor() noexcept :
    has_notifications(has_signals::signals())
{
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::configuring(const config_t& _config)
{
    sight::ui::service::initialize();

    m_delay = _config.get<unsigned>("config.<xmlattr>." + DELAY_CONFIG, m_delay);
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::starting()
{
    // Create the worker.
    m_request_worker = core::thread::worker::make();

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
    m_line_edit->setProperty("class", "lineEditDicomEditor");
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
    connections.push(m_series, data::signals::MODIFIED, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::updating()
{
    // Retrieve the DICOM series and its informations.
    const auto dicom_series        = m_series.lock();
    const std::size_t slice_number = static_cast<std::size_t>(dicom_series->get_instance_number().value());

    if(slice_number > 0)
    {
        // If the current slice index is the initial value of the slider, we just send a signal to trigger other
        // services.
        const int current_slice = m_slider->value();
        if(std::cmp_equal(current_slice, slice_number / 2))
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
    const std::size_t selected_slice_index = static_cast<std::size_t>(m_slider->value()) + 1;

    if(!sight::io::dicom::helper::series::is_instance_available(*dicom_series, selected_slice_index))
    {
        m_request_worker->post([this, selected_slice_index](auto&& ...){pull_slice(selected_slice_index);});
    }
    else
    {
        this->read_slice(*dicom_series, selected_slice_index);
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
        this->fail("Unable to connect to PACS");
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

            sight::io::dicom::helper::series::add_instance(*dicom_series, _selected_slice_index, sop_instance_uid);

            success = true;
        }
        else
        {
            this->fail("No instance found");
        }
    }
    catch(const sight::io::dimse::exceptions::base& e)
    {
        SIGHT_ERROR("Unable to execute query to the PACS: " + std::string(e.what()));
        this->fail("Unable to execute query");
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
        this->read_slice(*dicom_series, _selected_slice_index);
    }
}

//------------------------------------------------------------------------------

void slice_index_dicom_editor::read_slice(
    const data::series& _dicom_series,
    std::size_t _selected_slice_index
) const
{
    // Retrieve informations.
    const auto type = _dicom_series.get_dicom_type();
    if(type == data::series::dicom_t::image)
    {
        this->inform("Unable to read the modality '" + _dicom_series.get_modality_string() + "'");
        return;
    }

    auto reading_series = std::make_shared<data::series_set>();
    auto path           = sight::io::dicom::helper::series::get_path(_dicom_series, _selected_slice_index);
    auto reader         = std::make_shared<sight::io::dicom::reader::file>();
    reader->set_object(reading_series);
    reader->set_files({path.string()});

    auto observer = std::make_shared<sight::core::notification::observer>("Read slice");
    reader->read(observer);

    if(!reading_series->empty())
    {
        // Copy the read series to the image.
        const auto image_series = std::dynamic_pointer_cast<data::image_series>(reading_series->front());

        const auto image = m_image.lock();
        image->deep_copy(image_series);

        data::integer::sptr axial_index   = std::make_shared<data::integer>(0);
        data::integer::sptr frontal_index =
            std::make_shared<data::integer>(static_cast<std::int64_t>(image->size()[0] / 2));
        data::integer::sptr sagittal_index =
            std::make_shared<data::integer>(static_cast<std::int64_t>(image->size()[1] / 2));

        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::axis_t::axial,
            axial_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::axis_t::frontal,
            frontal_index->value()
        );
        data::helper::medical_image::set_slice_index(
            *image,
            data::helper::medical_image::axis_t::sagittal,
            sagittal_index->value()
        );

        // Send the signal
        image->async_emit(data::signals::MODIFIED);
    }
    else
    {
        SIGHT_ERROR("Unable to read the image");
        this->fail("Unable to read the image");
    }
}

} // namespace sight::module::io::dimse.
