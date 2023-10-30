/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "dicom_series_anonymizer.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/base.hpp>

#include <data/dicom_series.hpp>
#include <data/series_set.hpp>

#include <io/dicom/helper/DicomSeriesAnonymizer.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

dicom_series_anonymizer::dicom_series_anonymizer() noexcept :
    m_sig_job_created(new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL))
{
}

//------------------------------------------------------------------------------

dicom_series_anonymizer::~dicom_series_anonymizer() noexcept =
    default;

//------------------------------------------------------------------------------

void dicom_series_anonymizer::configuring()
{
}

//------------------------------------------------------------------------------

void dicom_series_anonymizer::starting()
{
}

//------------------------------------------------------------------------------

void dicom_series_anonymizer::stopping()
{
}

//------------------------------------------------------------------------------

void dicom_series_anonymizer::updating()
{
    const auto vector = m_selected_series.lock();

    sight::ui::dialog::message dialog;
    dialog.set_title("Series anonymization");

    // If the selection is not empty
    if(!vector->empty())
    {
        dialog.set_message("Are you sure you want to anonymize the selected series ?");
        dialog.set_icon(ui::dialog::message::question);
        dialog.add_button(ui::dialog::message::yes);
        dialog.add_button(ui::dialog::message::cancel);
        sight::ui::dialog::message::buttons answer = dialog.show();

        if(answer == sight::ui::dialog::message::yes)
        {
            sight::ui::cursor cursor;
            cursor.set_cursor(ui::cursor_base::busy);
            this->anonymize(*vector);
            cursor.set_default_cursor();
        }
    }
    // If the selection is empty
    else
    {
        dialog.set_message("Please select which series you want to anonymize.");
        dialog.set_icon(ui::dialog::message::info);
        dialog.add_button(ui::dialog::message::ok);
        dialog.show();
    }
}

//------------------------------------------------------------------------------

void dicom_series_anonymizer::info(std::ostream& _sstream)
{
    _sstream << "dicom_series_anonymizer::info";
}

//------------------------------------------------------------------------------

void dicom_series_anonymizer::anonymize(sight::data::vector& _vector)
{
    const auto series_set     = m_series_set.lock();
    const auto scoped_emitter = series_set->scoped_emit();

    auto anonymizer = std::make_shared<sight::io::dicom::helper::dicom_series_anonymizer>();
    m_sig_job_created->emit(anonymizer->get_job());

    std::vector<data::dicom_series::sptr> anonymized_dicom_series_vector;

    for(const auto& value : _vector)
    {
        data::dicom_series::sptr dicom_series            = std::dynamic_pointer_cast<data::dicom_series>(value);
        data::dicom_series::sptr anonymized_dicom_series = std::make_shared<data::dicom_series>();
        anonymizer->anonymize(dicom_series, anonymized_dicom_series);
        anonymized_dicom_series_vector.push_back(anonymized_dicom_series);

        m_cancelled = anonymizer->get_job()->cancel_requested();
        if(m_cancelled)
        {
            break;
        }
    }

    if(!m_cancelled)
    {
        for(const auto& value : _vector)
        {
            auto dicom_series = std::dynamic_pointer_cast<data::dicom_series>(value);
            series_set->remove(dicom_series);
        }

        for(const auto& anonymized_dicom_series : anonymized_dicom_series_vector)
        {
            series_set->push_back(anonymized_dicom_series);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
