/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <data/SeriesSet.hpp>

#include <io/dicom/reader/SeriesSet.hpp>

#include <ui/__/dialog/logger.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SDicomSeriesConverter::SDicomSeriesConverter() noexcept :
    m_sigJobCreated(new_signal<JobCreatedSignal>(JOB_CREATED_SIGNAL))
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
    data::SeriesSet::sptr dummy = std::make_shared<data::SeriesSet>();

    if(dicom_series_set->empty())
    {
        sight::ui::dialog::message messageBox;
        messageBox.setIcon(ui::dialog::message::INFO);
        messageBox.addButton(ui::dialog::message::OK);
        messageBox.setTitle("Read DICOM series");
        messageBox.setMessage("There is no DICOM series that can be read.");
        messageBox.show();
    }
    else
    {
        auto reader = std::make_shared<sight::io::dicom::reader::SeriesSet>();
        reader->setObject(dummy);
        auto job = reader->getJob();
        m_sigJobCreated->emit(job);

        try
        {
            reader->readFromDicomSeriesSet(dicom_series_set.get_shared(), this->get_sptr());

            core::log::logger::sptr logger = reader->getLogger();
            logger->sort();

            std::stringstream ss;
            ss << "The reading process is over : <b>" << dummy->size() << " series</b> "
            << ((dummy->size() > 1) ? "have" : "has") << " been found. "
            << "<br>Please verify the log report to be informed of the potential errors.";

            bool result = false;
            if(!job->cancel_requested())
            {
                result = sight::ui::dialog::logger::showLoggerDialog(
                    "Reading process over",
                    ss.str(),
                    logger
                );
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || job->cancel_requested())
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
            sight::ui::dialog::message::show(
                "Warning",
                ss.str(),
                sight::ui::dialog::message::WARNING
            );
        }
        catch(...)
        {
            sight::ui::dialog::message::show(
                "Warning",
                "Warning during loading",
                sight::ui::dialog::message::WARNING
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
