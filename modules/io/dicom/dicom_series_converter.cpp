/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "dicom_series_converter.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <data/series_set.hpp>

#include <io/dicom/reader/series_set.hpp>

#include <ui/__/dialog/logger.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

dicom_series_converter::dicom_series_converter() noexcept :
    m_sig_job_created(new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL))
{
}

//------------------------------------------------------------------------------

dicom_series_converter::~dicom_series_converter() noexcept =
    default;

//------------------------------------------------------------------------------

void dicom_series_converter::info(std::ostream& _sstream)
{
    _sstream << "dicom_series_converter::info";
}

//------------------------------------------------------------------------------

void dicom_series_converter::starting()
{
}

//------------------------------------------------------------------------------

void dicom_series_converter::stopping()
{
}

//------------------------------------------------------------------------------

void dicom_series_converter::configuring()
{
}

//------------------------------------------------------------------------------

void dicom_series_converter::updating()
{
    // Get Destination series_set
    const auto dest_series_set = m_target.lock();
    SIGHT_ASSERT("The 'target' key doesn't exist.", dest_series_set);

    const auto dicom_series_set  = m_source.lock();
    data::series_set::sptr dummy = std::make_shared<data::series_set>();

    if(dicom_series_set->empty())
    {
        sight::ui::dialog::message message_box;
        message_box.set_icon(ui::dialog::message::info);
        message_box.add_button(ui::dialog::message::ok);
        message_box.set_title("Read DICOM series");
        message_box.set_message("There is no DICOM series that can be read.");
        message_box.show();
    }
    else
    {
        auto reader = std::make_shared<sight::io::dicom::reader::series_set>();
        reader->set_object(dummy);
        auto job = reader->get_job();
        m_sig_job_created->emit(job);

        try
        {
            reader->read_from_dicom_series_set(dicom_series_set.get_shared(), this->get_sptr());

            core::log::logger::sptr logger = reader->get_logger();
            logger->sort();

            std::stringstream ss;
            ss << "The reading process is over : <b>" << dummy->size() << " series</b> "
            << ((dummy->size() > 1) ? "have" : "has") << " been found. "
            << "<br>Please verify the log report to be informed of the potential errors.";

            bool result = false;
            if(!job->cancel_requested())
            {
                result = sight::ui::dialog::logger::show_logger_dialog(
                    "Reading process over",
                    ss.str(),
                    logger
                );
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || job->cancel_requested())
            {
                dummy->clear();
            }
            else
            {
                const auto scoped_emitter = dest_series_set->scoped_emit();
                std::copy(dummy->cbegin(), dummy->cend(), sight::data::inserter(*dest_series_set));
            }
        }
        catch(const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();
            sight::ui::dialog::message::show(
                "Warning",
                ss.str(),
                sight::ui::dialog::message::warning
            );
        }
        catch(...)
        {
            sight::ui::dialog::message::show(
                "Warning",
                "Warning during loading",
                sight::ui::dialog::message::warning
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
