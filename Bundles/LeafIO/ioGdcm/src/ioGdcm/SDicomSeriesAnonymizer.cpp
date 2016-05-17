/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "ioGdcm/SDicomSeriesAnonymizer.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwComEd/helper/SeriesDB.hpp>
#include <fwCore/base.hpp>
#include <fwData/Vector.hpp>
#include <fwDicomData/DicomSeries.hpp>
#include <fwGdcmIO/helper/DicomSeriesAnonymizer.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwJobs/Aggregator.hpp>
#include <fwJobs/IJob.hpp>
#include <fwMedData/SeriesDB.hpp>
#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

#include <vector>

namespace ioGdcm
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioGdcm::SDicomSeriesAnonymizer, ::fwData::Vector );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::SDicomSeriesAnonymizer() throw() :
    m_sigJobCreated(JobCreatedSignal::New()),
    m_cancelled(false)
{
    ::fwCom::HasSignals::m_signals
        ( JOB_CREATED_SIGNAL, m_sigJobCreated );
}

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::~SDicomSeriesAnonymizer() throw()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::info(std::ostream &_sstream )
{
    _sstream << "SDicomSeriesAnonymizer::info";
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::starting() throw(::fwTools::Failed)
{
    // Get Destination SeriesDB
    m_seriesDB = ::fwMedData::SeriesDB::dynamicCast(::fwTools::fwID::getObject(m_seriesDBUID));
    SLM_ASSERT("The SeriesDB \"" + m_seriesDBUID + "\" doesn't exist.", m_seriesDB);
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::stopping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::configuring() throw(::fwTools::Failed)
{
    bool success;

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioGdcm::SDicomSeriesAnonymizer must have a \"config\" element.",config);

    /// SeriesDB UID
    ::boost::tie(success, m_seriesDBUID) = config->getSafeAttributeValue("seriesDBUID");
    SLM_ASSERT("It should be a \"seriesDBUID\" tag in the "
               "::ioGdcm::SDicomSeriesAnonymizer config element.", success);
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::updating() throw(::fwTools::Failed)
{
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();

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

void SDicomSeriesAnonymizer::anonymize()
{
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();
    ::fwComEd::helper::SeriesDB sDBhelper(m_seriesDB);

    ::fwGdcmIO::helper::DicomSeriesAnonymizer::sptr anonymizer =
        ::fwGdcmIO::helper::DicomSeriesAnonymizer::New();
    m_sigJobCreated->emit(anonymizer->getJob());

    std::vector< ::fwDicomData::DicomSeries::sptr > anonymizedDicomSeriesVector;

    for(::fwData::Vector::ContainerType::value_type value: vector->getContainer())
    {
        ::fwDicomData::DicomSeries::sptr dicomSeries           = ::fwDicomData::DicomSeries::dynamicCast(value);
        ::fwDicomData::DicomSeries::sptr anonymizedDicomSeries = ::fwDicomData::DicomSeries::New();
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
            ::fwDicomData::DicomSeries::sptr dicomSeries = ::fwDicomData::DicomSeries::dynamicCast(value);
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

} // namespace ioGdcm
