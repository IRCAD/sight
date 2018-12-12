/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ioVTK/SImageReader.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataIO/reader/IObjectReader.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwVtkIO/BitmapImageReader.hpp>
#include <fwVtkIO/ImageReader.hpp>
#include <fwVtkIO/MetaImageReader.hpp>
#include <fwVtkIO/VtiImageReader.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <chrono>
#include <cstdint>
#include <thread>

namespace ioVTK
{

//------------------------------------------------------------------------------

// Register a new reader of ::fwData::Image
fwServicesRegisterMacro( ::fwIO::IReader, ::ioVTK::SImageReader, ::fwData::Image );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

::fwIO::IOPathType SImageReader::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SImageReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    /* Initialize the available extensions for BitmapImageReader */
    std::vector<std::string> ext;
    ::fwVtkIO::BitmapImageReader::getAvailableExtensions(ext);
    std::string availableExtensions = "";

    if(ext.size() > 0)
    {
        availableExtensions = "*" + ext.at(0);
        for(int i = 1; i < ext.size(); i++)
        {
            availableExtensions = availableExtensions + " *" + ext.at(i);
        }
    }

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an image" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("Bitmap image", availableExtensions);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
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
    ::fwIO::IReader::configuring();
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
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", image);

        // Read new image path and update image. If the reading process is a success, we notify all listeners that image
        // has been modified.

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        try
        {
            // Notify other image services that a new image has been loaded.
            if ( SImageReader::loadImage( this->getFile(), image, m_sigJobCreated ) )
            {
                auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
                {
                    ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }
        }
        catch(::fwTools::Failed& e)
        {
            OSLM_TRACE("Error : " << e.what());
            FW_RAISE_EXCEPTION(e);
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

template< typename READER > typename READER::sptr configureReader(const ::boost::filesystem::path& imgFile )
{
    typename READER::sptr reader = READER::New();
    reader->setFile(imgFile);
    return reader;
}

//------------------------------------------------------------------------------

bool SImageReader::loadImage( const ::boost::filesystem::path& imgFile,
                              const ::fwData::Image::sptr& img,
                              const SPTR(JobCreatedSignalType)& sigJobCreated)
{
    bool ok = true;

    std::string ext = ::boost::filesystem::extension(imgFile);
    ::boost::algorithm::to_lower(ext);

    ::fwDataIO::reader::IObjectReader::sptr imageReader;
    if(ext == ".vtk")
    {
        imageReader = configureReader< ::fwVtkIO::ImageReader >( imgFile );
    }
    else if(ext == ".vti")
    {
        imageReader = configureReader< ::fwVtkIO::VtiImageReader >( imgFile );
    }
    else if(ext == ".mhd")
    {
        imageReader = configureReader< ::fwVtkIO::MetaImageReader >( imgFile );
    }
    else
    {
        /* Handle BitmapImageReader extensions */
        std::vector<std::string> availableExtensions;
        ::fwVtkIO::BitmapImageReader::getAvailableExtensions(availableExtensions);

        /* If we find the current extensions in the available readers, we use it */
        size_t i = 0;
        for(; i < availableExtensions.size(); i++)
        {
            if(availableExtensions.at(i) == ext)
            {
                imageReader = configureReader< ::fwVtkIO::BitmapImageReader >( imgFile );
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
            FW_RAISE_EXCEPTION(::fwTools::Failed("Only " + bitmapExtensions + ".vtk, .vti and .mhd are supported."));
        }
    }

    // Set the image (already created, but empty) that will be modified
    ::fwData::mt::ObjectWriteLock lock(img);
    imageReader->setObject(img);

    sigJobCreated->emit(imageReader->getJob());

    try
    {
        imageReader->read();
    }
    catch(::fwTools::Failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
        // Raise exception  for superior level
        FW_RAISE_EXCEPTION(e);
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            "Warning during loading.",
            ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }

    return ok;
}

} // namespace ioVtk
