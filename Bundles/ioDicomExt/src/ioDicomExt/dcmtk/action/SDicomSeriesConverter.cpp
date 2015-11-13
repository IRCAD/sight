/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicomExt/dcmtk/action/SDicomSeriesConverter.hpp"

#include <fwDicomIOExt/dcmtk/SeriesDBReader.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwServices/macros.hpp>

namespace ioDicomExt
{

namespace dcmtk
{

namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::ioDicomExt::dcmtk::action::SDicomSeriesConverter,
                         ::fwMedData::SeriesDB );

//------------------------------------------------------------------------------

SDicomSeriesConverter::SDicomSeriesConverter() throw()
{
}
//------------------------------------------------------------------------------

SDicomSeriesConverter::~SDicomSeriesConverter() throw()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::info(std::ostream &_sstream )
{
    _sstream << "SDicomSeriesConverter::info";
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get Destination SeriesDB
    m_destinationSeriesDB = ::fwMedData::SeriesDB::dynamicCast(::fwTools::fwID::getObject(m_destinationSeriesDBID));
    SLM_ASSERT("The SeriesDB \"" + m_destinationSeriesDBID + "\" doesn't exist.", m_destinationSeriesDB);
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("The service ::ioDicomExt::dcmtk::action::SDicomSeriesConverter must have a \"config\" element.",config);

    bool success;

    // Destination Series DB ID
    ::boost::tie(success, m_destinationSeriesDBID) = config->getSafeAttributeValue("destinationSeriesDBID");
    SLM_ASSERT("It should be a \"destinationSeriesDBID\" attribute in the "
               "::ioDicomExt::dcmtk::action::SDicomSeriesConverter config element.", success);


}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwMedData::SeriesDB::sptr dicomSeriesDB = this->getObject< ::fwMedData::SeriesDB >();

    // Display the informations
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.setTitle("Read DICOM series");

    if(dicomSeriesDB->empty())
    {
        messageBox.setMessage( "There is no DICOM series that can be read." );
    }
    else
    {
        ::fwDicomIOExt::dcmtk::SeriesDBReader::sptr myLoader = ::fwDicomIOExt::dcmtk::SeriesDBReader::New();
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Dicom Image");

        myLoader->setObject(m_destinationSeriesDB);
        myLoader->addHandler( progressMeterGUI );
        myLoader->readFromDicomSeriesDB(dicomSeriesDB);

        messageBox.setMessage( "DICOM series have been correctly read." );
    }

    ::fwGui::dialog::IMessageDialog::Buttons answer = messageBox.show();
}



} // namespace action
} // namespace dcmtk
} // namespace ioDicomExt
