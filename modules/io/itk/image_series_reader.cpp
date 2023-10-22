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

#include "modules/io/itk/image_series_reader.hpp"

#include "modules/io/itk/image_reader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/date_and_time.hpp>
#include <core/tools/failed.hpp>
#include <core/tools/os.hpp>
#include <core/tools/uuid.hpp>

#include <data/image.hpp>

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

image_series_reader::image_series_reader() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType image_series_reader::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void image_series_reader::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose a file" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialog_file.addFilter("Inr (.inr.gz)", "*.inr.gz");
    dialog_file.setOption(ui::dialog::location::READ);
    dialog_file.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void image_series_reader::starting()
{
}

//------------------------------------------------------------------------------

void image_series_reader::stopping()
{
}

//------------------------------------------------------------------------------

void image_series_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void image_series_reader::info(std::ostream& _sstream)
{
    _sstream << "image_series_reader::info";
}

//------------------------------------------------------------------------------

void init_series(data::series::sptr _series)
{
    const std::string instance_uid     = core::tools::UUID::generate();
    const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date             = core::tools::get_date(now);
    const std::string time             = core::tools::get_time(now);

    _series->setModality("OT");
    _series->setSeriesDate(date);
    _series->setSeriesTime(time);
    _series->setSeriesDescription("image imported with ITK");
    std::string physicians = _series->getPerformingPhysicianName();
    if(physicians.empty())
    {
        physicians = core::tools::os::get_env("USERNAME", core::tools::os::get_env("LOGNAME", "Unknown"));
    }

    _series->setPerformingPhysicianName(physicians);
    _series->setStudyInstanceUID(instance_uid);
    _series->setStudyDate(date);
    _series->setStudyTime(time);
}

//------------------------------------------------------------------------------

void image_series_reader::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked       = m_data.lock();
        const auto image_series = std::dynamic_pointer_cast<data::image_series>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::image_series::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            image_series
        );

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        try
        {
            if(image_reader::loadImage(this->get_file(), image_series))
            {
                init_series(image_series);

                auto sig = image_series->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
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
