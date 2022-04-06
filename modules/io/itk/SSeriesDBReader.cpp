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

#include "SSeriesDBReader.hpp"

#include "modules/io/itk/SImageReader.hpp"

#include <core/base.hpp>
#include <core/location/MultipleFiles.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Equipment.hpp>
#include <data/helper/SeriesDB.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/Patient.hpp>
#include <data/SeriesDB.hpp>
#include <data/Study.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

SSeriesDBReader::SSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

SSeriesDBReader::~SSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SSeriesDBReader::getIOPathType() const
{
    return sight::io::base::service::FILES;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configuring()
{
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an image file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Inr (.inr.gz)", "*.inr.gz");
    dialogFile.addFilter("Nifti (.nii)", "*.nii");
    dialogFile.setType(ui::base::dialog::ILocationDialog::MULTI_FILES);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    auto result = core::location::MultipleFiles::dynamicCast(dialogFile.show());
    if(result)
    {
        const std::vector<std::filesystem::path> paths = result->getFiles();
        if(!paths.empty())
        {
            defaultDirectory->setFolder(paths[0].parent_path());
            dialogFile.saveDefaultLocation(defaultDirectory);
        }

        this->setFiles(paths);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto data     = m_data.lock();
        const auto seriesDB = std::dynamic_pointer_cast<data::SeriesDB>(data.get_shared());
        SIGHT_ASSERT("The inout key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", seriesDB);

        data::SeriesDB::sptr localSeriesDB = data::SeriesDB::New();

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        const std::string instanceUID = core::tools::UUID::generateUUID();

        for(const std::filesystem::path& path : this->getFiles())
        {
            data::ImageSeries::sptr imgSeries = data::ImageSeries::New();
            this->initSeries(imgSeries, instanceUID);

            data::Image::sptr image = data::Image::New();
            if(!SImageReader::loadImage(path, image))
            {
                m_readFailed = true;
            }

            imgSeries->setImage(image);

            localSeriesDB->getContainer().push_back(imgSeries);
        }

        data::helper::SeriesDB sDBhelper(*seriesDB);
        sDBhelper.merge(localSeriesDB);
        sDBhelper.notify();

        cursor.setDefaultCursor();
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::initSeries(data::Series::sptr series, const std::string& instanceUID)
{
    series->setModality("OT");
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date       = core::tools::getDate(now);
    const std::string time       = core::tools::getTime(now);
    series->setDate(date);
    series->setTime(time);

    series->getStudy()->setInstanceUID(instanceUID);
    series->getStudy()->setDate(date);
    series->getStudy()->setTime(time);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
