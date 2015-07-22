/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwComEd/helper/SeriesDB.hpp>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwDicomData/DicomSeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>

#include <fwDicomIOExt/gdcm/helper/DicomSeriesAnonymizer.hpp>

#include "ioDicomExt/gdcm/SDicomSeriesAnonymizer.hpp"


namespace ioDicomExt
{

namespace gdcm
{

fwServicesRegisterMacro( ::fwServices::IController, ::ioDicomExt::gdcm::SDicomSeriesAnonymizer, ::fwData::Composite );

//------------------------------------------------------------------------------

SDicomSeriesAnonymizer::SDicomSeriesAnonymizer() throw()
{
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
    SLM_ASSERT("The service ::ioDicomExt::SDicomSeriesAnonymizer must have a \"config\" element.",config);

    /// SeriesDB UID
    ::boost::tie(success, m_seriesDBUID) = config->getSafeAttributeValue("seriesDBUID");
    SLM_ASSERT("It should be a \"seriesDBUID\" tag in the "
               "::ioDicomExt::SDicomSeriesAnonymizer config element.", success);
}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{

}

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::updating() throw(::fwTools::Failed)
{
    ::fwGui::dialog::MessageDialog dialog;
    dialog.setTitle("Series anonymization");
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

//------------------------------------------------------------------------------

void SDicomSeriesAnonymizer::anonymize()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    ::fwComEd::helper::SeriesDB sDBhelper(m_seriesDB);

    ::fwDicomIOExt::gdcm::helper::DicomSeriesAnonymizer::sptr anonymizer =
        ::fwDicomIOExt::gdcm::helper::DicomSeriesAnonymizer::New();
    ::fwGui::dialog::ProgressDialog progressMeterGUI("Anonymization ");
    anonymizer->addHandler( progressMeterGUI );

    BOOST_FOREACH(::fwData::Composite::ContainerType::value_type value, composite->getContainer())
    {
        ::fwDicomData::DicomSeries::sptr dicomSeries           = ::fwDicomData::DicomSeries::dynamicCast(value.second);
        ::fwDicomData::DicomSeries::sptr anonymizedDicomSeries = ::fwDicomData::DicomSeries::New();
        anonymizer->anonymize(dicomSeries, anonymizedDicomSeries);
        sDBhelper.add(anonymizedDicomSeries);
        sDBhelper.remove(dicomSeries);
    }

    // Notify SeriesDB
    sDBhelper.notify(this->getSptr());
}

} // namespace gdcm
} // namespace ioDicomExt
