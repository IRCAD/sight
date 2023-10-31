/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/dicom_series.hpp>
#include <data/series.hpp>

#include <io/dimse/exceptions/base.hpp>
#include <io/dimse/helper/series.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <dcmtk/dcmdata/dcistrmb.h>

namespace sight::module::io::dimse
{

//------------------------------------------------------------------------------

const core::com::slots::key_t series_pusher::DISPLAY_SLOT = "displayMessage";

const core::com::signals::key_t series_pusher::PROGRESSED_SIG       = "progressed";
const core::com::signals::key_t series_pusher::STARTED_PROGRESS_SIG = "startedProgress";
const core::com::signals::key_t series_pusher::STOPPED_PROGRESS_SIG = "stoppedProgress";

//------------------------------------------------------------------------------

series_pusher::series_pusher() noexcept :
    m_progressbar_id("pushDicomProgressBar")
{
    // Internal slots
    m_slot_display_message   = new_slot(DISPLAY_SLOT, &series_pusher::display_message);
    m_slot_progress_callback = new_slot(
        sight::io::dimse::series_enquirer::PROGRESS_CALLBACK_SLOT,
        &series_pusher::progress_callback,
        this
    );

    // Public signals
    m_sig_progressed       = new_signal<progressed_signal_t>(PROGRESSED_SIG);
    m_sig_started_progress = new_signal<started_progress_signal_t>(STARTED_PROGRESS_SIG);
    m_sig_stopped_progress = new_signal<stopped_progress_signal_t>(STOPPED_PROGRESS_SIG);
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
    // Create enquirer
    m_series_enquirer = std::make_shared<sight::io::dimse::series_enquirer>();

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

        // Initialize enquirer
        m_series_enquirer->initialize(
            pacs_configuration->get_local_application_title(),
            pacs_configuration->get_pacs_host_name(),
            pacs_configuration->get_pacs_application_port(),
            pacs_configuration->get_pacs_application_title(),
            pacs_configuration->get_move_application_title(),
            m_slot_progress_callback
        );

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

        // Connect to PACS
        m_series_enquirer->connect();

        for(const auto& object : *series_vector)
        {
            auto series = std::dynamic_pointer_cast<data::series>(object);
            SIGHT_ASSERT("The series_set should contain only Series.", series);

            // Try to find series on PACS
            OFList<QRResponse*> responses;
            responses = m_series_enquirer->find_series_by_uid(series->get_series_instance_uid());

            // If the series has been found on the PACS
            if(responses.size() > 1)
            {
                duplicate_series_vector.push_back(series);
            }

            sight::io::dimse::helper::series::release_responses(responses);
        }

        // Disconnect from PACS
        m_series_enquirer->disconnect();

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
        std::vector<CSPTR(DcmDataset)> dicom_container;

        // Connect to PACS
        for(const auto& series : *series_vector)
        {
            data::dicom_series::csptr dicom_series = std::dynamic_pointer_cast<data::dicom_series>(series);
            SIGHT_ASSERT("The series_set should contain only DicomSeries.", dicom_series);

            for(const auto& item : dicom_series->get_dicom_container())
            {
                DcmFileFormat file_format;
                core::memory::buffer_object::sptr buffer_obj = item.second;
                const std::size_t buff_size                  = buffer_obj->size();
                core::memory::buffer_object::lock_t lock(buffer_obj);
                char* buffer = static_cast<char*>(lock.buffer());

                DcmInputBufferStream is;
                is.setBuffer(buffer, offile_off_t(buff_size));
                is.setEos();

                file_format.transferInit();
                if(!file_format.read(is).good())
                {
                    SIGHT_THROW("Unable to read Dicom file '" << buffer_obj->get_stream_info().fs_file.string() << "'");
                }

                file_format.loadAllDataIntoMemory();
                file_format.transferEnd();

                const DcmDataset* dataset = file_format.getAndRemoveDataset();
                CSPTR(DcmDataset) dataset_ptr(dataset);
                dicom_container.push_back(dataset_ptr);
            }
        }

        // Number of instances that must be uploaded
        m_instance_count = static_cast<std::uint64_t>(dicom_container.size());

        // Connect from PACS
        m_series_enquirer->connect();
        m_sig_started_progress->async_emit(m_progressbar_id);

        // Push series
        m_series_enquirer->push_series(dicom_container);

        // Disconnect from PACS
        m_series_enquirer->disconnect();
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

void series_pusher::progress_callback(
    const std::string& /*seriesInstanceUID*/,
    unsigned int _instance_number,
    const std::string& /*filePath*/
)
{
    if(_instance_number < (m_instance_count - 1))
    {
        float percentage = static_cast<float>(_instance_number) / static_cast<float>(m_instance_count);
        m_sig_progressed->async_emit(m_progressbar_id, percentage, "Pushing series...");
    }
    else
    {
        m_sig_stopped_progress->async_emit(m_progressbar_id);
    }
}

//------------------------------------------------------------------------------

void series_pusher::display_message(const std::string& _message, bool _error)
{
    SIGHT_WARN_IF("Error: " + _message, _error);
    sight::ui::dialog::message message_box;
    message_box.set_title((_error ? "Error" : "Information"));
    message_box.set_message(_message);
    message_box.set_icon(_error ? (ui::dialog::message::critical) : (ui::dialog::message::info));
    message_box.add_button(ui::dialog::message::ok);
    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse
