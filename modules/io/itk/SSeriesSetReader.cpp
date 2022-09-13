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

#include "SSeriesSetReader.hpp"

#include "modules/io/itk/SImageReader.hpp"

#include <core/base.hpp>
#include <core/location/MultipleFiles.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/SeriesSet.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SSeriesSetReader::getIOPathType() const
{
    return sight::io::base::service::FILES;
}

//------------------------------------------------------------------------------

void SSeriesSetReader::configuring()
{
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void SSeriesSetReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an image file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialogFile.addFilter("Inr (.inr.gz)", "*.inr.gz");
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

void SSeriesSetReader::updating()
{
    // Do it right here in case of exception...
    m_readFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto data       = m_data.lock();
        const auto series_set = std::dynamic_pointer_cast<data::SeriesSet>(data.get_shared());
        SIGHT_ASSERT("The inout key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", series_set);

        // Set cursor busy
        sight::ui::base::BusyCursor cursor;

        bool readFailed               = false;
        auto localSet                 = data::SeriesSet::New();
        const std::string instanceUID = core::tools::UUID::generateUUID();

        for(const std::filesystem::path& path : this->getFiles())
        {
            auto imgSeries = data::ImageSeries::New();
            SSeriesSetReader::initSeries(imgSeries, instanceUID);

            if(!SImageReader::loadImage(path, imgSeries))
            {
                readFailed = true;
            }

            localSet->push_back(imgSeries);
        }

        if(!localSet->empty())
        {
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->clear();
            series_set->insert(series_set->begin(), localSet->cbegin(), localSet->cend());
        }

        m_readFailed = readFailed;
    }
}

//------------------------------------------------------------------------------

void SSeriesSetReader::initSeries(data::Series::sptr series, const std::string& instanceUID)
{
    series->setModality("OT");
    boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
    const std::string date       = core::tools::getDate(now);
    const std::string time       = core::tools::getTime(now);
    series->setSeriesDate(date);
    series->setSeriesTime(time);

    series->setStudyInstanceUID(instanceUID);
    series->setStudyDate(date);
    series->setStudyTime(time);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
