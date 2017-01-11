/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioGdcm/SDicomSeriesConverter.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwGui/dialog/LoggerDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

namespace ioGdcm
{

fwServicesRegisterMacro(::fwGui::IActionSrv, ::ioGdcm::SDicomSeriesConverter, ::fwMedData::SeriesDB);

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesConverter::SDicomSeriesConverter() throw()
{
    m_sigJobCreated = newSignal<JobCreatedSignal>( JOB_CREATED_SIGNAL );
}
//------------------------------------------------------------------------------

SDicomSeriesConverter::~SDicomSeriesConverter() throw()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::info(std::ostream& _sstream )
{
    _sstream << "SDicomSeriesConverter::info";
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::starting() throw(::fwTools::Failed)
{
    // Get Destination SeriesDB
    m_destinationSeriesDB = this->getInOut< ::fwMedData::SeriesDB>("target");
    SLM_ASSERT("The 'target' key doesn't exist.", m_destinationSeriesDB);
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::stopping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::configuring() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::updating() throw(::fwTools::Failed)
{
    ::fwMedData::SeriesDB::csptr dicomSeriesDB = this->getInput< ::fwMedData::SeriesDB >("source");
    ::fwMedData::SeriesDB::sptr dummy          = ::fwMedData::SeriesDB::New();

    if(dicomSeriesDB->empty())
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.setTitle("Read DICOM series");
        messageBox.setMessage( "There is no DICOM series that can be read." );
        messageBox.show();
    }
    else
    {
        ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
        reader->setObject(dummy);
        auto job = reader->getJob();
        m_sigJobCreated->emit(job);

        try
        {
            reader->readFromDicomSeriesDB(dicomSeriesDB, this->getSptr());

            ::fwLog::Logger::sptr logger = reader->getLogger();
            logger->sort();

            std::stringstream ss;
            ss << "The reading process is over : <b>" << dummy->size() << " series</b> "
               << ((dummy->size() > 1) ? "have" : "has") << " been found. "
               << "<br>Please verify the log report to be informed of the potential errors.";

            bool result = false;
            if(!job->cancelRequested())
            {
                result = ::fwGui::dialog::LoggerDialog::showLoggerDialog("Reading process over", ss.str(), logger);
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || job->cancelRequested())
            {
                ::fwMedDataTools::helper::SeriesDB sDBhelper(dummy);
                sDBhelper.clear();
            }
            else
            {
                ::fwMedDataTools::helper::SeriesDB sDBhelper(m_destinationSeriesDB);
                sDBhelper.merge(dummy);
                sDBhelper.notify();
            }
        }
        catch (const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
        }
        catch( ... )
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace ioGdcm

