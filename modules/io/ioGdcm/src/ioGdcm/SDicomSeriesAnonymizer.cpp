/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ioGdcm/SDicomSeriesAnonymizer.hpp"

#include <core/com/Signal.hxx>

#include <data/DicomSeries.hpp>
#include <data/SeriesDB.hpp>
#include <data/tools/helper/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <fwGdcmIO/helper/DicomSeriesAnonymizer.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/IJob.hpp>

#include <services/macros.hpp>

#include <vector>

namespace ioGdcm
{

fwServicesRegisterMacro( ::sight::services::IController, ::ioGdcm::SDicomSeriesAnonymizer)

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::SDicomSeriesAnonymizer() noexcept :
    m_cancelled(false)
{
    m_sigJobCreated = newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::~SDicomSeriesAnonymizer() noexcept
{
}

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
    data::Vector::sptr vector = this->getInOut< data::Vector >("selectedSeries");

    ::fwGui::dialog::MessageDialog dialog;
    dialog.setTitle("Series anonymization");

    // If the selection is not empty
    if(!vector->empty())
    {
        dialog.setMessage( "Are you sure you want to anonymize the selected series ?" );
        dialog.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        dialog.addButton(::fwGui::dialog::IMessageDialog::YES);
        dialog.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
        ::fwGui::dialog::IMessageDialog::Buttons answer = dialog.show();

        if ( answer == ::fwGui::dialog::IMessageDialog::YES )
        {
            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);
            this->anonymize();
            cursor.setDefaultCursor();
        }
    }
    // If the selection is empty
    else
    {
        dialog.setMessage( "Please select which series you want to anonymize." );
        dialog.setIcon(::fwGui::dialog::IMessageDialog::INFO);
        dialog.addButton(::fwGui::dialog::IMessageDialog::OK);
        dialog.show();
    }
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::info(std::ostream& _sstream )
{
    _sstream << "SDicomSeriesAnonymizer::info";
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::anonymize()
{
    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB>("seriesDB");
    data::Vector::sptr vector     = this->getInOut< data::Vector >("selectedSeries");

    data::tools::helper::SeriesDB sDBhelper(seriesDB);

    ::fwGdcmIO::helper::DicomSeriesAnonymizer::sptr anonymizer =
        ::fwGdcmIO::helper::DicomSeriesAnonymizer::New();
    m_sigJobCreated->emit(anonymizer->getJob());

    std::vector< data::DicomSeries::sptr > anonymizedDicomSeriesVector;

    for(const auto& value : vector->getContainer())
    {
        data::DicomSeries::sptr dicomSeries           = data::DicomSeries::dynamicCast(value);
        data::DicomSeries::sptr anonymizedDicomSeries = data::DicomSeries::New();
        anonymizer->anonymize(dicomSeries, anonymizedDicomSeries);
        anonymizedDicomSeriesVector.push_back(anonymizedDicomSeries);

        m_cancelled = anonymizer->getJob()->cancelRequested();
        if(m_cancelled)
        {
            break;
        }
    }

    if(!m_cancelled)
    {
        for(const auto& value : vector->getContainer())
        {
            data::DicomSeries::sptr dicomSeries = data::DicomSeries::dynamicCast(value);
            sDBhelper.remove(dicomSeries);
        }

        for(const auto& anonymizedDicomSeries : anonymizedDicomSeriesVector)
        {
            sDBhelper.add(anonymizedDicomSeries);
        }
    }

    // Notify SeriesDB
    sDBhelper.notify();
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
