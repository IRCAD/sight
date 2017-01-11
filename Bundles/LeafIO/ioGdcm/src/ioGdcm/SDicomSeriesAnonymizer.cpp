/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "ioGdcm/SDicomSeriesAnonymizer.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

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

#include <boost/foreach.hpp>

#include <vector>

namespace ioGdcm
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioGdcm::SDicomSeriesAnonymizer, ::fwData::Object );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::SDicomSeriesAnonymizer() throw() :
    m_cancelled(false)
{
    m_sigJobCreated = newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::~SDicomSeriesAnonymizer() throw()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::configuring() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::starting() throw(::fwTools::Failed)
{
    m_seriesDB = this->getInOut< ::fwMedData::SeriesDB>("seriesDB");
    SLM_ASSERT("The SeriesDB key doesn't exist.", m_seriesDB);
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::stopping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::updating() throw(::fwTools::Failed)
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
    ::fwData::Vector::sptr vector = this->getInOut< ::fwData::Vector >("selectedSeries");
    ::fwMedDataTools::helper::SeriesDB sDBhelper(m_seriesDB);

    ::fwGdcmIO::helper::DicomSeriesAnonymizer::sptr anonymizer =
        ::fwGdcmIO::helper::DicomSeriesAnonymizer::New();
    m_sigJobCreated->emit(anonymizer->getJob());

    std::vector< ::fwMedData::DicomSeries::sptr > anonymizedDicomSeriesVector;

    for(::fwData::Vector::ContainerType::value_type value: vector->getContainer())
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
        for(::fwData::Vector::ContainerType::value_type value: vector->getContainer())
        {
            ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::dynamicCast(value);
            sDBhelper.remove(dicomSeries);
        }

        for(auto anonymizedDicomSeries: anonymizedDicomSeriesVector)
        {
            sDBhelper.add(anonymizedDicomSeries);
        }
    }

    // Notify SeriesDB
    sDBhelper.notify();
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
