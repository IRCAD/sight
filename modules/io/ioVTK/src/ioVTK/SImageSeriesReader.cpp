/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "ioVTK/SImageSeriesReader.hpp"

#include "ioVTK/SImageReader.hpp"

#include <core/com/Signal.hxx>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/Failed.hpp>
#include <core/tools/Os.hpp>
#include <core/tools/UUID.hpp>

#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>
#include <data/Patient.hpp>
#include <data/Study.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/ioTypes.hpp>
#include <fwIO/IReader.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/BitmapImageReader.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <filesystem>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioVTK::SImageSeriesReader, data::ImageSeries )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SImageSeriesReader::SImageSeriesReader() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SImageSeriesReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SImageSeriesReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SImageSeriesReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    // Initialize the available extensions for BitmapImageReader
    std::vector<std::string> ext;
    ::fwVtkIO::BitmapImageReader::getAvailableExtensions(ext);
    std::string availableExtensions = "";

    if(ext.size() > 0)
    {
        availableExtensions = "*" + ext.at(0);
        for(size_t i = 1; i < ext.size(); i++)
        {
            availableExtensions = availableExtensions + " *" + ext.at(i);
        }
    }

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an ImageSeries" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("Bitmap image", availableExtensions);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
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
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void SImageSeriesReader::info(std::ostream& _sstream )
{
    _sstream << "SImageSeriesReader::info";
}

//------------------------------------------------------------------------------

void initSeries(data::Series::sptr series)
{
    const std::string instanceUID        = core::tools::UUID::generateUUID();
    const ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    const std::string date               = core::tools::getDate(now);
    const std::string time               = core::tools::getTime(now);

    series->setModality("OT");
    series->setDate(date);
    series->setTime(time);
    series->setDescription("Image imported with VTK");
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
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::ImageSeries::sptr imageSeries = this->getInOut< data::ImageSeries >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("ImageSeries is not instanced", imageSeries);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        try
        {
            data::Image::sptr image = data::Image::New();

            if ( SImageReader::loadImage( this->getFile(), image, m_sigJobCreated ) )
            {
                imageSeries->setImage(image);
                initSeries(imageSeries);
                this->notificationOfDBUpdate();
            }
        }
        catch(core::tools::Failed& e)
        {
            FW_RAISE_EXCEPTION(e);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SImageSeriesReader::notificationOfDBUpdate()
{
    data::ImageSeries::sptr imageSeries = this->getInOut< data::ImageSeries >(::fwIO::s_DATA_KEY);
    SLM_ASSERT("imageSeries not instanced", imageSeries);

    auto sig = imageSeries->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    {
        core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
