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

#include "SInrSeriesDBReader.hpp"

#include <core/base.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Equipment.hpp>
#include <data/helper/SeriesDB.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/location/Folder.hpp>
#include <data/location/MultiFiles.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/Patient.hpp>
#include <data/SeriesDB.hpp>
#include <data/Study.hpp>

#include <io/base/service/IReader.hpp>
#include <io/itk/ImageReader.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

SInrSeriesDBReader::SInrSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

SInrSeriesDBReader::~SInrSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SInrSeriesDBReader::getIOPathType() const
{
    return sight::io::base::service::FILES;
}

//------------------------------------------------------------------------------

void SInrSeriesDBReader::configuring()
{
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void SInrSeriesDBReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SInrSeriesDBReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an Inrimage file" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage", "*.inr.gz");
    dialogFile.setType(ui::base::dialog::ILocationDialog::MULTI_FILES);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    data::location::MultiFiles::sptr result;
    result = data::location::MultiFiles::dynamicCast( dialogFile.show() );
    if (result)
    {
        const data::location::ILocation::VectPathType paths = result->getPaths();
        if(!paths.empty())
        {
            _sDefaultPath = paths[0].parent_path();
            dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        }
        this->setFiles(paths);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

bool SInrSeriesDBReader::createImage( const std::filesystem::path inrFile, data::Image::sptr image )
{
    auto myLoader = sight::io::itk::ImageReader::New();
    bool ok       = true;

    myLoader->setObject(image);
    myLoader->setFile(inrFile);

    try
    {
        sight::ui::base::dialog::ProgressDialog progressMeterGUI("Loading Image ");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        sight::ui::base::dialog::MessageDialog::show("Warning",
                                                     ss.str(),
                                                     sight::ui::base::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    catch( ... )
    {
        sight::ui::base::dialog::MessageDialog::show("Warning",
                                                     "Warning during loading",
                                                     sight::ui::base::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    return ok;
}

//------------------------------------------------------------------------------

void SInrSeriesDBReader::updating()
{

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB >(sight::io::base::service::s_DATA_KEY);
        SIGHT_ASSERT("The inout key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", seriesDB);

        data::SeriesDB::sptr localSeriesDB = data::SeriesDB::New();

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        const std::string instanceUID = core::tools::UUID::generateUUID();

        for(const std::filesystem::path& path :  this->getFiles())
        {
            data::ImageSeries::sptr imgSeries = data::ImageSeries::New();
            this->initSeries(imgSeries, instanceUID);

            data::Image::sptr image = data::Image::New();
            if(!this->createImage( path, image ))
            {
                m_readFailed = true;
            }
            imgSeries->setImage(image);

            localSeriesDB->getContainer().push_back(imgSeries);
        }

        data::helper::SeriesDB sDBhelper(seriesDB);

        data::mt::ObjectWriteLock lock(seriesDB);
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

void SInrSeriesDBReader::initSeries(data::Series::sptr series, const std::string& instanceUID)
{
    series->setModality("OT");
    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    const std::string date = core::tools::getDate(now);
    const std::string time = core::tools::getTime(now);
    series->setDate(date);
    series->setTime(time);

    series->getStudy()->setInstanceUID(instanceUID);
    series->getStudy()->setDate(date);
    series->getStudy()->setTime(time);
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
