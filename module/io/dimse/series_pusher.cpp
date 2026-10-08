/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "series_pusher.hpp"

#include <core/notification/observer.hpp>

#include <data/series.hpp>

#include <io/dicom/helper/series.hpp>
#include <io/dicom/writer/file.hpp>
#include <io/dimse/exceptions/base.hpp>
#include <io/dimse/helper/series.hpp>
#include <io/dimse/series_enquirer.hpp>

#include <ui/__/dialog/message.hpp>

namespace sight::module::io::dimse
{

//------------------------------------------------------------------------------

series_pusher::series_pusher() noexcept :
    has_monitors(has_signals::signals())
{
    // Internal slots
    m_slot_display_message = new_slot(slots::DISPLAY, &series_pusher::display_message);

    // Public signals
    new_signal<signals::started_progress_t>(signals::STARTED_PROGRESS);
    new_signal<signals::stopped_progress_t>(signals::STOPPED_PROGRESS);
}

//------------------------------------------------------------------------------

series_pusher::~series_pusher() noexcept =
    default;

//------------------------------------------------------------------------------

void series_pusher::info(std::ostream& _sstream)
{
    _sstream << "series_pusher::info";
}

//------------------------------------------------------------------------------

void series_pusher::configuring()
{
}

//------------------------------------------------------------------------------

void series_pusher::starting()
{
    // Worker
    m_push_series_worker = core::thread::worker::make();
}

//------------------------------------------------------------------------------

void series_pusher::stopping()
{
    m_push_series_worker->stop();
    m_push_series_worker.reset();
}

//------------------------------------------------------------------------------

void series_pusher::updating()
{
    const auto selected_series = m_selected_series.lock();

    if(m_is_pushing)
    {
        // Display a message to inform the user that the service is already pushing data.
        sight::ui::dialog::message message_box;
        message_box.set_title("Pushing Series");
        message_box.set_message(
            "The service is already pushing data. Please wait until the pushing is done "
            "before sending a new push request."
        );
        message_box.set_icon(ui::dialog::message::info);
        message_box.add_button(ui::dialog::message::ok);
        message_box.show();
    }
    else if(selected_series->empty())
    {
        // Display a message to inform the user that there is no series selected.
        sight::ui::dialog::message message_box;
        message_box.set_title("Pushing Series");
        message_box.set_message("Unable to push series, there is no series selected.");
        message_box.set_icon(ui::dialog::message::info);
        message_box.add_button(ui::dialog::message::ok);
        message_box.show();
    }
    else
    {
        const auto pacs_configuration = m_config.lock();

        // Set pushing boolean to true
        m_is_pushing = true;

        // Check whether some selected series are already on the PACS or not
        bool push_ok = this->check_series_on_pacs();
        if(push_ok)
        {
            // Push series to the PACS
            m_push_series_worker->post([this](auto&& ...){push_series();});
        }
    }
}

//------------------------------------------------------------------------------
bool series_pusher::check_series_on_pacs()
{
    // Return true if the push operation must be performed
    bool result = true;

    const auto series_vector = m_selected_series.lock();

    // Catch any errors
    try
    {
        // Find which selected series must be pushed
        dicom_series_container_t duplicate_series_vector;

        // Create enquirer
        auto series_enquirer = std::make_shared<sight::io::dimse::series_enquirer>();

        const auto pacs_configuration = m_config.lock();

        // Initialize enquirer
        series_enquirer->initialize(
            pacs_configuration->get_local_application_title(),
            pacs_configuration->get_pacs_host_name(),
            pacs_configuration->get_pacs_application_port(),
            pacs_configuration->get_pacs_application_title(),
            pacs_configuration->get_move_application_title()
        );
        // Connect to PACS
        series_enquirer->connect();

        for(const auto& object : *series_vector)
        {
            auto series = std::dynamic_pointer_cast<data::series>(object);
            SIGHT_ASSERT("The series_set should contain only Series.", series);

            // Try to find series on PACS
            OFList<QRResponse*> responses;
            responses = series_enquirer->find_series_by_uid(series->get_series_instance_uid());

            // If the series has been found on the PACS
            if(responses.size() > 1)
            {
                duplicate_series_vector.push_back(series);
            }

            sight::io::dimse::helper::series::release_responses(responses);
        }

        // Disconnect from PACS
        series_enquirer->disconnect();

        // Inform the user that some series are already on the PACS
        if(!duplicate_series_vector.empty())
        {
            std::stringstream ss;
            ss << "Those series are already on the PACS: \n";

            // Display duplicated Series
            for(const data::series::csptr& series : duplicate_series_vector)
            {
                std::string description = series->get_series_description();
                description = (description.empty()) ? "[No description]" : description;
                ss << "- " << description << std::endl;
            }

            ss << std::endl << "Would you like to perform the operation anyway ?" << std::endl
            << "(This will result in a merge operation)";

            sight::ui::dialog::message message_box;
            message_box.set_title("Duplicate series");
            message_box.set_message(ss.str());
            message_box.set_icon(ui::dialog::message::info);
            message_box.add_button(ui::dialog::message::ok);
            message_box.add_button(ui::dialog::message::cancel);
            sight::ui::dialog::message::buttons answer = message_box.show();

            result = (answer == sight::ui::dialog::message::ok);
        }
    }
    catch(sight::io::dimse::exceptions::base& exception)
    {
        const auto pacs_configuration = m_config.lock();

        std::stringstream ss;
        ss << "Unable to connect to the pacs. Please check your configuration: \n"
        << "Pacs host name: " << pacs_configuration->get_pacs_host_name() << "\n"
        << "Pacs application title: " << pacs_configuration->get_pacs_application_title() << "\n"
        << "Pacs port: " << pacs_configuration->get_pacs_application_port() << "\n";
        m_slot_display_message->async_run(ss.str(), true);
        SIGHT_WARN(exception.what());
        result = false;

        // Set pushing boolean to false
        m_is_pushing = false;
        this->async_emit(signals::STOPPED_PROGRESS);
    }

    return result;
}

//------------------------------------------------------------------------------

void series_pusher::push_series()
{
    const auto series_vector = m_selected_series.lock();

    // Catch any errors
    try
    {
        // List of dicom slice that must be pushed
        std::vector<sight::csptr<DcmDataset> > dicom_container;

        // Connect to PACS
        for(const auto& series : *series_vector)
        {
            auto dicom_series = std::dynamic_pointer_cast<data::series>(series);
            SIGHT_ASSERT("The series_set should contain only DicomSeries.", dicom_series);

            auto writing_series = std::make_shared<data::series_set>();
            auto path           = sight::io::dicom::helper::series::get_path(*dicom_series);
            auto writer         = std::make_shared<sight::io::dicom::writer::file>();
            writer->set_object(writing_series);
            writer->set_folder({path.string()});

            auto observer = this->make_notification<sight::core::notification::observer>("Write");
            writer->write(observer);
        }

        // Number of instances that must be uploaded
        m_instance_count = static_cast<std::uint64_t>(dicom_container.size());

        // Create enquirer
        auto series_enquirer = std::make_shared<sight::io::dimse::series_enquirer>();

        const auto pacs_configuration = m_config.lock();

        auto progress = this->observe("Push DICOM Series", false, nullptr, m_instance_count);

        // Initialize enquirer
        series_enquirer->initialize(
            pacs_configuration->get_local_application_title(),
            pacs_configuration->get_pacs_host_name(),
            pacs_configuration->get_pacs_application_port(),
            pacs_configuration->get_pacs_application_title(),
            pacs_configuration->get_move_application_title(),
            progress
        );

        // Connect from PACS
        series_enquirer->connect();
        this->async_emit(signals::STARTED_PROGRESS);

        // Push series
        series_enquirer->push_series(dicom_container);

        // Disconnect from PACS
        series_enquirer->disconnect();
    }
    catch(sight::io::dimse::exceptions::base& exception)
    {
        const auto pacs_configuration = m_config.lock();

        std::stringstream ss;
        ss << "Unable to connect to the pacs. Please check your configuration: \n"
        << "Pacs host name: " << pacs_configuration->get_pacs_host_name() << "\n"
        << "Pacs application title: " << pacs_configuration->get_pacs_application_title() << "\n"
        << "Pacs port: " << pacs_configuration->get_pacs_application_port() << "\n";
        m_slot_display_message->async_run(ss.str(), true);
        SIGHT_WARN(exception.what());
    }

    // Set pushing boolean to false
    m_is_pushing = false;
}

//------------------------------------------------------------------------------

void series_pusher::display_message(const std::string& _message, bool _error)
{
    SIGHT_WARN_IF("Error: " + _message, _error);
    sight::ui::dialog::message message_box;
    message_box.set_title((_error ? "Error" : "Information"));
    message_box.set_message(_message);
    message_box.set_icon(_error ? ui::dialog::message::critical : ui::dialog::message::info);
    message_box.add_button(ui::dialog::message::ok);
    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse
