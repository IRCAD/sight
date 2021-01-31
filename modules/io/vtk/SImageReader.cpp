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

#include "modules/io/vtk/SImageReader.hpp"

#include <core/base.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>

#include <data/Image.hpp>
#include <data/location/Folder.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <services/macros.hpp>
#include <services/registry/ActiveWorkers.hpp>

#include <boost/algorithm/string.hpp>

#include <io/base/reader/IObjectReader.hpp>
#include <io/base/services/IReader.hpp>
#include <io/vtk/BitmapImageReader.hpp>
#include <io/vtk/ImageReader.hpp>
#include <io/vtk/MetaImageReader.hpp>
#include <io/vtk/VtiImageReader.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <thread>

namespace sight::modules::io::vtk
{

//------------------------------------------------------------------------------

// Register a new reader of data::Image
fwServicesRegisterMacro( ::sight::io::base::services::IReader, ::sight::modules::io::vtk::SImageReader,
                         ::sight::data::Image )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

sight::io::base::services::IOPathType SImageReader::getIOPathType() const
{
    return sight::io::base::services::FILE;
}

//------------------------------------------------------------------------------

void SImageReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SImageReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    /* Initialize the available extensions for BitmapImageReader */
    std::vector<std::string> ext;
    sight::io::vtk::BitmapImageReader::getAvailableExtensions(ext);
    std::string availableExtensions = "";

    if(ext.size() > 0)
    {
        availableExtensions = "*" + ext.at(0);
        for(size_t i = 1; i < ext.size(); i++)
        {
            availableExtensions = availableExtensions + " *" + ext.at(i);
        }
    }

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an image" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("Bitmap image", availableExtensions);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

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

SImageReader::SImageReader() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

void SImageReader::starting()
{
}

//------------------------------------------------------------------------------

void SImageReader::stopping()
{
}

//------------------------------------------------------------------------------

void SImageReader::configuring()
{
    sight::io::base::services::IReader::configuring();
}

//------------------------------------------------------------------------------

void SImageReader::info( std::ostream& _sstream )
{
    _sstream << "SImageReader::info";
}

//------------------------------------------------------------------------------

void SImageReader::updating()
{
    if( this->hasLocationDefined() )
    {
        data::Image::sptr image = this->getInOut< data::Image >(sight::io::base::services::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + sight::io::base::services::s_DATA_KEY + "' is not correctly set.", image);

        // Read new image path and update image. If the reading process is a success, we notify all listeners that image
        // has been modified.

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);
        try
        {
            // Notify other image services that a new image has been loaded.
            if ( SImageReader::loadImage( this->getFile(), image, m_sigJobCreated ) )
            {
                auto sig = image->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
                {
                    core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }
            else
            {
                m_readFailed = true;
            }
        }
        catch(core::tools::Failed& e)
        {
            m_readFailed = true;
            FW_RAISE_EXCEPTION(e);
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

template< typename READER > typename READER::sptr configureReader(const std::filesystem::path& imgFile )
{
    typename READER::sptr reader = READER::New();
    reader->setFile(imgFile);
    return reader;
}

//------------------------------------------------------------------------------

bool SImageReader::loadImage( const std::filesystem::path& imgFile,
                              const data::Image::sptr& img,
                              const SPTR(JobCreatedSignalType)& sigJobCreated)
{
    bool ok = true;

    std::string ext = imgFile.extension().string();
    ::boost::algorithm::to_lower(ext);

    sight::io::base::reader::IObjectReader::sptr imageReader;
    if(ext == ".vtk")
    {
        imageReader = configureReader< sight::io::vtk::ImageReader >( imgFile );
    }
    else if(ext == ".vti")
    {
        imageReader = configureReader< sight::io::vtk::VtiImageReader >( imgFile );
    }
    else if(ext == ".mhd")
    {
        imageReader = configureReader< sight::io::vtk::MetaImageReader >( imgFile );
    }
    else
    {
        /* Handle BitmapImageReader extensions */
        std::vector<std::string> availableExtensions;
        sight::io::vtk::BitmapImageReader::getAvailableExtensions(availableExtensions);

        /* If we find the current extensions in the available readers, we use it */
        size_t i = 0;
        for(; i < availableExtensions.size(); i++)
        {
            if(availableExtensions.at(i) == ext)
            {
                imageReader = configureReader< sight::io::vtk::BitmapImageReader >( imgFile );
                break;
            }
        }

        // If we didn't find any suitable extension with BitmapImageReader, we raise an exception */
        if(i == availableExtensions.size())
        {
            i = 0;
            std::string bitmapExtensions = "";
            for(; i < availableExtensions.size(); i++)
            {
                bitmapExtensions = bitmapExtensions + availableExtensions.at(i) + ", ";
            }
            FW_RAISE_EXCEPTION(core::tools::Failed("Only " + bitmapExtensions + ".vtk, .vti and .mhd are supported."));
        }
    }

    // Set the image (already created, but empty) that will be modified
    data::mt::ObjectWriteLock lock(img);
    imageReader->setObject(img);

    sigJobCreated->emit(imageReader->getJob());

    try
    {
        imageReader->read();
    }
    catch(core::tools::Failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING);
        ok = false;
        // Raise exception  for superior level
        FW_RAISE_EXCEPTION(e);
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    catch( ... )
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading.",
            sight::ui::base::dialog::IMessageDialog::WARNING);
        ok = false;
    }

    return ok;
}

} // namespace ioVtk
