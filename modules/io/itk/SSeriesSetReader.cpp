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

#include "SSeriesSetReader.hpp"

#include "modules/io/itk/SImageReader.hpp"

#include <core/base.hpp>
#include <core/location/multiple_files.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/date_and_time.hpp>
#include <core/tools/uuid.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/SeriesSet.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

sight::io::service::IOPathType SSeriesSetReader::getIOPathType() const
{
    return sight::io::service::FILES;
}

//------------------------------------------------------------------------------

void SSeriesSetReader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void SSeriesSetReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an image file" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialogFile.addFilter("Inr (.inr.gz)", "*.inr.gz");
    dialogFile.setType(ui::dialog::location::MULTI_FILES);
    dialogFile.setOption(ui::dialog::location::READ);
    dialogFile.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::multiple_files>(dialogFile.show());
    if(result)
    {
        const std::vector<std::filesystem::path> paths = result->get_files();
        if(!paths.empty())
        {
            defaultDirectory->set_folder(paths[0].parent_path());
            dialogFile.saveDefaultLocation(defaultDirectory);
        }

        this->set_files(paths);
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
        SIGHT_ASSERT("The inout key '" + sight::io::service::s_DATA_KEY + "' is not correctly set.", series_set);

        // Set cursor busy
        sight::ui::BusyCursor cursor;

        bool readFailed               = false;
        auto localSet                 = std::make_shared<data::SeriesSet>();
        const std::string instanceUID = core::tools::UUID::generate();

        for(const std::filesystem::path& path : this->get_files())
        {
            auto imgSeries = std::make_shared<data::ImageSeries>();
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
    const std::string date       = core::tools::get_date(now);
    const std::string time       = core::tools::get_time(now);
    series->setSeriesDate(date);
    series->setSeriesTime(time);

    series->setStudyInstanceUID(instanceUID);
    series->setStudyDate(date);
    series->setStudyTime(time);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
