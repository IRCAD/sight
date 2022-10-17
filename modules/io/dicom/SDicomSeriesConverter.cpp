/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SDicomSeriesConverter.hpp"

#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <data/SeriesSet.hpp>

#include <io/dicom/reader/SeriesSet.hpp>

#include <ui/base/dialog/LoggerDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::module::io::dicom
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesConverter::SDicomSeriesConverter() noexcept :
    m_sigJobCreated(newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL))
{
}

//------------------------------------------------------------------------------

SDicomSeriesConverter::~SDicomSeriesConverter() noexcept =
    default;

//------------------------------------------------------------------------------

void SDicomSeriesConverter::info(std::ostream& _sstream)
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
    // Get Destination SeriesSet
    const auto dest_series_set = m_target.lock();
    SIGHT_ASSERT("The 'target' key doesn't exist.", dest_series_set);

    const auto dicom_series_set = m_source.lock();
    data::SeriesSet::sptr dummy = data::SeriesSet::New();

    if(dicom_series_set->empty())
    {
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
        messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
        messageBox.setTitle("Read DICOM series");
        messageBox.setMessage("There is no DICOM series that can be read.");
        messageBox.show();
    }
    else
    {
        auto reader = sight::io::dicom::reader::SeriesSet::New();
        reader->setObject(dummy);
        auto job = reader->getJob();
        m_sigJobCreated->emit(job);

        try
        {
            reader->readFromDicomSeriesSet(dicom_series_set.get_shared(), this->getSptr());

            core::log::Logger::sptr logger = reader->getLogger();
            logger->sort();

            std::stringstream ss;
            ss << "The reading process is over : <b>" << dummy->size() << " series</b> "
            << ((dummy->size() > 1) ? "have" : "has") << " been found. "
            << "<br>Please verify the log report to be informed of the potential errors.";

            bool result = false;
            if(!job->cancelRequested())
            {
                result = sight::ui::base::dialog::LoggerDialog::showLoggerDialog(
                    "Reading process over",
                    ss.str(),
                    logger
                );
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || job->cancelRequested())
            {
                dummy->clear();
            }
            else
            {
                const auto scoped_emitter = dest_series_set->scoped_emit();
                std::copy(dummy->cbegin(), dummy->cend(), sight::data::inserter(*dest_series_set));
            }
        }
        catch(const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during loading : " << e.what();
            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                ss.str(),
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
        }
        catch(...)
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                "Warning during loading",
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
