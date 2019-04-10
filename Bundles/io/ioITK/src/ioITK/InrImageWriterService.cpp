/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioITK/InrImageWriterService.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/IWriter.hpp>

#include <fwItkIO/ImageWriter.hpp>

#include <fwServices/macros.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioITK::InrImageWriterService, ::fwData::Image );

//------------------------------------------------------------------------------

InrImageWriterService::InrImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

InrImageWriterService::~InrImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType InrImageWriterService::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void InrImageWriterService::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void InrImageWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an inrimage file to save image" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage", "*.inr.gz");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        this->setFile( result->getPath() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        m_writeFailed = true;
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void InrImageWriterService::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void InrImageWriterService::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void InrImageWriterService::info(std::ostream& _sstream )
{
    _sstream << "InrImageWriterService::info";
}

//------------------------------------------------------------------------------

void InrImageWriterService::saveImage( const ::boost::filesystem::path& inrFile, const ::fwData::Image::csptr& image )
{
    SLM_TRACE_FUNC();
    ::fwItkIO::ImageWriter::sptr myWriter = ::fwItkIO::ImageWriter::New();

    myWriter->setObject(image);
    myWriter->setFile(inrFile);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Image ");
        myWriter->addHandler( progressMeterGUI );
        myWriter->write();

    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                          ss.str(),
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                          "Warning during saving",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void InrImageWriterService::updating()
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", image);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        saveImage(this->getFile(), image);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
