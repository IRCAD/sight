/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "modules/io/vtk/SImageSeriesReader.hpp"

#include "modules/io/vtk/SImageReader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/date_and_time.hpp>
#include <core/tools/failed.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <data/Image.hpp>

#include <io/__/service/ioTypes.hpp>
#include <io/__/service/reader.hpp>
#include <io/vtk/BitmapImageReader.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SImageSeriesReader::SImageSeriesReader() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType SImageSeriesReader::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void SImageSeriesReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    // Initialize the available extensions for BitmapImageReader
    std::vector<std::string> ext;
    sight::io::vtk::BitmapImageReader::getAvailableExtensions(ext);
    std::string availableExtensions;

    if(!ext.empty())
    {
        availableExtensions = "*" + ext.at(0);
        for(std::size_t i = 1 ; i < ext.size() ; i++)
        {
            availableExtensions.append(" *").append(ext.at(i));
        }
    }

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an ImageSeries" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("Bitmap image", availableExtensions);
    dialogFile.setOption(ui::dialog::location::READ);
    dialogFile.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SImageSeriesReader::starting()
{
}

//------------------------------------------------------------------------------

void SImageSeriesReader::stopping()
{
}

//------------------------------------------------------------------------------

void SImageSeriesReader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void SImageSeriesReader::info(std::ostream& _sstream)
{
    _sstream << "SImageSeriesReader::info";
}

//------------------------------------------------------------------------------

void initSeries(data::Series::sptr series)
{
    const std::string instanceUID      = core::tools::UUID::generate();
    const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date             = core::tools::get_date(now);
    const std::string time             = core::tools::get_time(now);

    series->setModality("OT");
    series->setSeriesDate(date);
    series->setSeriesTime(time);
    series->setSeriesDescription("Image imported with VTK");
    std::string physicians = series->getPerformingPhysicianName();
    if(physicians.empty())
    {
        physicians = core::tools::os::get_env("USERNAME", core::tools::os::get_env("LOGNAME", "Unknown"));
    }

    series->setPerformingPhysicianName(physicians);
    series->setStudyInstanceUID(instanceUID);
    series->setStudyDate(date);
    series->setStudyTime(time);
}

//------------------------------------------------------------------------------

void SImageSeriesReader::updating()
{
    m_readFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked      = m_data.lock();
        const auto imageSeries = std::dynamic_pointer_cast<data::ImageSeries>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::ImageSeries::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            imageSeries
        );

        sight::ui::BusyCursor cursor;

        if(SImageReader::loadImage(this->get_file(), imageSeries, m_sigJobCreated))
        {
            initSeries(imageSeries);

            auto sig = imageSeries->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
            {
                core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                sig->async_emit();
            }

            m_readFailed = false;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
