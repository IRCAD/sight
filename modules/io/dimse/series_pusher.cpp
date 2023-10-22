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

#include <io/dimse/exceptions/Base.hpp>
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
    m_progressbarId("pushDicomProgressBar")
{
    // Internal slots
    m_slotDisplayMessage   = new_slot(DISPLAY_SLOT, &series_pusher::displayMessage);
    m_slotProgressCallback = new_slot(
        sight::io::dimse::SeriesEnquirer::PROGRESS_CALLBACK_SLOT,
        &series_pusher::progress_callback,
        this
    );

    // Public signals
    m_sigProgressed      = new_signal<progressed_signal_t>(PROGRESSED_SIG);
    m_sigStartedProgress = new_signal<started_progress_signal_t>(STARTED_PROGRESS_SIG);
    m_sigStoppedProgress = new_signal<stopped_progress_signal_t>(STOPPED_PROGRESS_SIG);
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
    m_seriesEnquirer = std::make_shared<sight::io::dimse::SeriesEnquirer>();

    // Worker
    m_pushSeriesWorker = core::thread::worker::make();
}

//------------------------------------------------------------------------------

void series_pusher::stopping()
{
    m_pushSeriesWorker->stop();
    m_pushSeriesWorker.reset();
}

//------------------------------------------------------------------------------

void series_pusher::updating()
{
    const auto selected_series = m_selectedSeries.lock();

    if(m_isPushing)
    {
        // Display a message to inform the user that the service is already pushing data.
        sight::ui::dialog::message message_box;
        message_box.setTitle("Pushing Series");
        message_box.setMessage(
            "The service is already pushing data. Please wait until the pushing is done "
            "before sending a new push request."
        );
        message_box.setIcon(ui::dialog::message::INFO);
        message_box.addButton(ui::dialog::message::OK);
        message_box.show();
    }
    else if(selected_series->empty())
    {
        // Display a message to inform the user that there is no series selected.
        sight::ui::dialog::message message_box;
        message_box.setTitle("Pushing Series");
        message_box.setMessage("Unable to push series, there is no series selected.");
        message_box.setIcon(ui::dialog::message::INFO);
        message_box.addButton(ui::dialog::message::OK);
        message_box.show();
    }
    else
    {
        const auto pacs_configuration = m_config.lock();

        // Initialize enquirer
        m_seriesEnquirer->initialize(
            pacs_configuration->getLocalApplicationTitle(),
            pacs_configuration->getPacsHostName(),
            pacs_configuration->getPacsApplicationPort(),
            pacs_configuration->getPacsApplicationTitle(),
            pacs_configuration->getMoveApplicationTitle(),
            m_slotProgressCallback
        );

        // Set pushing boolean to true
        m_isPushing = true;

        // Check whether some selected series are already on the PACS or not
        bool push_ok = this->checkSeriesOnPACS();
        if(push_ok)
        {
            // Push series to the PACS
            m_pushSeriesWorker->post([this](auto&& ...){pushSeries();});
        }
    }
}

//------------------------------------------------------------------------------
bool series_pusher::checkSeriesOnPACS()
{
    // Return true if the push operation must be performed
    bool result = true;

    const auto series_vector = m_selectedSeries.lock();

    // Catch any errors
    try
    {
        // Find which selected series must be pushed
        dicom_series_container_t duplicate_series_vector;

        // Connect to PACS
        m_seriesEnquirer->connect();

        for(const auto& object : *series_vector)
        {
            auto series = std::dynamic_pointer_cast<data::series>(object);
            SIGHT_ASSERT("The series_set should contain only Series.", series);

            // Try to find series on PACS
            OFList<QRResponse*> responses;
            responses = m_seriesEnquirer->findSeriesByUID(series->getSeriesInstanceUID());

            // If the series has been found on the PACS
            if(responses.size() > 1)
            {
                duplicate_series_vector.push_back(series);
            }

            sight::io::dimse::helper::series::releaseResponses(responses);
        }

        // Disconnect from PACS
        m_seriesEnquirer->disconnect();

        // Inform the user that some series are already on the PACS
        if(!duplicate_series_vector.empty())
        {
            std::stringstream ss;
            ss << "Those series are already on the PACS: \n";

            // Display duplicated Series
            for(const data::series::csptr& series : duplicate_series_vector)
            {
                std::string description = series->getSeriesDescription();
                description = (description.empty()) ? "[No description]" : description;
                ss << "- " << description << std::endl;
            }

            ss << std::endl << "Would you like to perform the operation anyway ?" << std::endl
            << "(This will result in a merge operation)";

            sight::ui::dialog::message message_box;
            message_box.setTitle("Duplicate series");
            message_box.setMessage(ss.str());
            message_box.setIcon(ui::dialog::message::INFO);
            message_box.addButton(ui::dialog::message::OK);
            message_box.addButton(ui::dialog::message::CANCEL);
            sight::ui::dialog::message::Buttons answer = message_box.show();

            result = (answer == sight::ui::dialog::message::OK);
        }
    }
    catch(sight::io::dimse::exceptions::Base& exception)
    {
        const auto pacs_configuration = m_config.lock();

        std::stringstream ss;
        ss << "Unable to connect to the pacs. Please check your configuration: \n"
        << "Pacs host name: " << pacs_configuration->getPacsHostName() << "\n"
        << "Pacs application title: " << pacs_configuration->getPacsApplicationTitle() << "\n"
        << "Pacs port: " << pacs_configuration->getPacsApplicationPort() << "\n";
        m_slotDisplayMessage->async_run(ss.str(), true);
        SIGHT_WARN(exception.what());
        result = false;

        // Set pushing boolean to false
        m_isPushing = false;
    }

    return result;
}

//------------------------------------------------------------------------------

void series_pusher::pushSeries()
{
    const auto series_vector = m_selectedSeries.lock();

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

            for(const auto& item : dicom_series->getDicomContainer())
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
        m_instanceCount = static_cast<std::uint64_t>(dicom_container.size());

        // Connect from PACS
        m_seriesEnquirer->connect();
        m_sigStartedProgress->async_emit(m_progressbarId);

        // Push series
        m_seriesEnquirer->pushSeries(dicom_container);

        // Disconnect from PACS
        m_seriesEnquirer->disconnect();
    }
    catch(sight::io::dimse::exceptions::Base& exception)
    {
        const auto pacs_configuration = m_config.lock();

        std::stringstream ss;
        ss << "Unable to connect to the pacs. Please check your configuration: \n"
        << "Pacs host name: " << pacs_configuration->getPacsHostName() << "\n"
        << "Pacs application title: " << pacs_configuration->getPacsApplicationTitle() << "\n"
        << "Pacs port: " << pacs_configuration->getPacsApplicationPort() << "\n";
        m_slotDisplayMessage->async_run(ss.str(), true);
        SIGHT_WARN(exception.what());
    }

    // Set pushing boolean to false
    m_isPushing = false;
}

//------------------------------------------------------------------------------

void series_pusher::progress_callback(
    const std::string& /*seriesInstanceUID*/,
    unsigned int _instance_number,
    const std::string& /*filePath*/
)
{
    if(_instance_number < (m_instanceCount - 1))
    {
        float percentage = static_cast<float>(_instance_number) / static_cast<float>(m_instanceCount);
        m_sigProgressed->async_emit(m_progressbarId, percentage, "Pushing series...");
    }
    else
    {
        m_sigStoppedProgress->async_emit(m_progressbarId);
    }
}

//------------------------------------------------------------------------------

void series_pusher::displayMessage(const std::string& _message, bool _error)
{
    SIGHT_WARN_IF("Error: " + _message, _error);
    sight::ui::dialog::message message_box;
    message_box.setTitle((_error ? "Error" : "Information"));
    message_box.setMessage(_message);
    message_box.setIcon(_error ? (ui::dialog::message::CRITICAL) : (ui::dialog::message::INFO));
    message_box.addButton(ui::dialog::message::OK);
    message_box.show();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dimse
