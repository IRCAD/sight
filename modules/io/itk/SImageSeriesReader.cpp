/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
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

#include "modules/io/itk/SImageSeriesReader.hpp"

#include "modules/io/itk/SImageReader.hpp"

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

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <filesystem>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

SImageSeriesReader::SImageSeriesReader() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType SImageSeriesReader::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void SImageSeriesReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialogFile.addFilter("Inr (.inr.gz)", "*.inr.gz");
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
    series->setSeriesDescription("Image imported with ITK");
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

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        try
        {
            if(SImageReader::loadImage(this->get_file(), imageSeries))
            {
                initSeries(imageSeries);

                auto sig = imageSeries->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
                {
                    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                    sig->async_emit();
                }
            }
        }
        catch(core::tools::failed& e)
        {
            cursor.setDefaultCursor();
            SIGHT_THROW_EXCEPTION(e);
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
