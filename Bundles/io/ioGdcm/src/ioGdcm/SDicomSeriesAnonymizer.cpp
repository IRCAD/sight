/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioGdcm/SDicomSeriesAnonymizer.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Vector.hpp>

#include <fwGdcmIO/helper/DicomSeriesAnonymizer.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/IJob.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <vector>

namespace ioGdcm
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioGdcm::SDicomSeriesAnonymizer);

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

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
    ::fwData::Vector::sptr vector = this->getInOut< ::fwData::Vector >("selectedSeries");

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
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB>("seriesDB");
    ::fwData::Vector::sptr vector        = this->getInOut< ::fwData::Vector >("selectedSeries");

    ::fwMedDataTools::helper::SeriesDB sDBhelper(seriesDB);

    ::fwGdcmIO::helper::DicomSeriesAnonymizer::sptr anonymizer =
        ::fwGdcmIO::helper::DicomSeriesAnonymizer::New();
    m_sigJobCreated->emit(anonymizer->getJob());

    std::vector< ::fwMedData::DicomSeries::sptr > anonymizedDicomSeriesVector;

    for(const auto& value : vector->getContainer())
    {
        ::fwMedData::DicomSeries::sptr dicomSeries           = ::fwMedData::DicomSeries::dynamicCast(value);
        ::fwMedData::DicomSeries::sptr anonymizedDicomSeries = ::fwMedData::DicomSeries::New();
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
            ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(value);
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
