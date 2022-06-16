/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
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

#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/Failed.hpp>
#include <core/tools/Os.hpp>
#include <core/tools/UUID.hpp>

#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/Patient.hpp>
#include <data/Study.hpp>

#include <io/base/service/ioTypes.hpp>
#include <io/base/service/IReader.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <filesystem>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

SImageSeriesReader::SImageSeriesReader() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SImageSeriesReader::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SImageSeriesReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialogFile.addFilter("Inr (.inr.gz)", "*.inr.gz");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->setFile(result->getFile());
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
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void SImageSeriesReader::info(std::ostream& _sstream)
{
    _sstream << "SImageSeriesReader::info";
}

//------------------------------------------------------------------------------

void initSeries(data::Series::sptr series)
{
    const std::string instanceUID      = core::tools::UUID::generateUUID();
    const boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date             = core::tools::getDate(now);
    const std::string time             = core::tools::getTime(now);

    series->setModality("OT");
    series->setDate(date);
    series->setTime(time);
    series->setDescription("Image imported with ITK");
    data::DicomValuesType physicians = series->getPerformingPhysiciansName();
    if(physicians.empty())
    {
        const std::string username = core::tools::os::getEnv("USERNAME", core::tools::os::getEnv("LOGNAME", "Unknown"));
        physicians.push_back(username);
    }

    series->setPerformingPhysiciansName(physicians);
    series->getStudy()->setInstanceUID(instanceUID);
    series->getStudy()->setDate(date);
    series->getStudy()->setTime(time);
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
            + sight::io::base::service::s_DATA_KEY
            + "' is not correctly set.",
            imageSeries
        );

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        try
        {
            auto image = data::Image::New();

            if(SImageReader::loadImage(this->getFile(), image))
            {
                imageSeries->setImage(image);
                initSeries(imageSeries);

                auto sig = imageSeries->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
                {
                    core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }
        }
        catch(core::tools::Failed& e)
        {
            cursor.setDefaultCursor();
            SIGHT_THROW_EXCEPTION(e);
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioItk
