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

#include "SDicomSeriesAnonymizer.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/base.hpp>

#include <data/DicomSeries.hpp>
#include <data/SeriesSet.hpp>

#include <io/dicom/helper/DicomSeriesAnonymizer.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::SDicomSeriesAnonymizer() noexcept :
    m_sigJobCreated(new_signal<JobCreatedSignal>(JOB_CREATED_SIGNAL))
{
}

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::~SDicomSeriesAnonymizer() noexcept =
    default;

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::configuring()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::starting()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::stopping()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::updating()
{
    const auto vector = m_selectedSeries.lock();

    sight::ui::dialog::message dialog;
    dialog.setTitle("Series anonymization");

    // If the selection is not empty
    if(!vector->empty())
    {
        dialog.setMessage("Are you sure you want to anonymize the selected series ?");
        dialog.setIcon(ui::dialog::message::QUESTION);
        dialog.addButton(ui::dialog::message::YES);
        dialog.addButton(ui::dialog::message::CANCEL);
        sight::ui::dialog::message::Buttons answer = dialog.show();

        if(answer == sight::ui::dialog::message::YES)
        {
            sight::ui::cursor cursor;
            cursor.setCursor(ui::cursor_base::BUSY);
            this->anonymize(*vector);
            cursor.setDefaultCursor();
        }
    }
    // If the selection is empty
    else
    {
        dialog.setMessage("Please select which series you want to anonymize.");
        dialog.setIcon(ui::dialog::message::INFO);
        dialog.addButton(ui::dialog::message::OK);
        dialog.show();
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::info(std::ostream& _sstream)
{
    _sstream << "SDicomSeriesAnonymizer::info";
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::anonymize(sight::data::Vector& _vector)
{
    const auto series_set     = m_series_set.lock();
    const auto scoped_emitter = series_set->scoped_emit();

    auto anonymizer = std::make_shared<sight::io::dicom::helper::DicomSeriesAnonymizer>();
    m_sigJobCreated->emit(anonymizer->getJob());

    std::vector<data::DicomSeries::sptr> anonymizedDicomSeriesVector;

    for(const auto& value : _vector)
    {
        data::DicomSeries::sptr dicomSeries           = std::dynamic_pointer_cast<data::DicomSeries>(value);
        data::DicomSeries::sptr anonymizedDicomSeries = std::make_shared<data::DicomSeries>();
        anonymizer->anonymize(dicomSeries, anonymizedDicomSeries);
        anonymizedDicomSeriesVector.push_back(anonymizedDicomSeries);

        m_cancelled = anonymizer->getJob()->cancel_requested();
        if(m_cancelled)
        {
            break;
        }
    }

    if(!m_cancelled)
    {
        for(const auto& value : _vector)
        {
            auto dicomSeries = std::dynamic_pointer_cast<data::DicomSeries>(value);
            series_set->remove(dicomSeries);
        }

        for(const auto& anonymizedDicomSeries : anonymizedDicomSeriesVector)
        {
            series_set->push_back(anonymizedDicomSeries);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
