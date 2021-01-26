/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ioGdcm/SDicomSeriesConverter.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <data/tools/helper/SeriesDB.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwGui/dialog/LoggerDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <services/macros.hpp>

namespace ioGdcm
{

fwServicesRegisterMacro(::fwGui::IActionSrv, ::ioGdcm::SDicomSeriesConverter, data::SeriesDB)

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesConverter::SDicomSeriesConverter() noexcept
{
    m_sigJobCreated = newSignal<JobCreatedSignal>( JOB_CREATED_SIGNAL );
}
//------------------------------------------------------------------------------

SDicomSeriesConverter::~SDicomSeriesConverter() noexcept
{
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::info(std::ostream& _sstream )
{
    _sstream << "SDicomSeriesConverter::info";
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::starting()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::stopping()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::configuring()
{
}

//------------------------------------------------------------------------------

void SDicomSeriesConverter::updating()
{
    // Get Destination SeriesDB
    data::SeriesDB::sptr destinationSeriesDB = this->getInOut< data::SeriesDB>("target");
    SLM_ASSERT("The 'target' key doesn't exist.", destinationSeriesDB);

    data::SeriesDB::csptr dicomSeriesDB = this->getInput< data::SeriesDB >("source");
    data::SeriesDB::sptr dummy          = data::SeriesDB::New();

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

            core::log::Logger::sptr logger = reader->getLogger();
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
                data::tools::helper::SeriesDB sDBhelper(dummy);
                sDBhelper.clear();
            }
            else
            {
                data::tools::helper::SeriesDB sDBhelper(destinationSeriesDB);
                sDBhelper.merge(dummy);
                sDBhelper.notify();
            }
        }
        catch (const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();
            ::fwGui::dialog::MessageDialog::show(
                "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
        }
        catch( ... )
        {
            ::fwGui::dialog::MessageDialog::show(
                "Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
