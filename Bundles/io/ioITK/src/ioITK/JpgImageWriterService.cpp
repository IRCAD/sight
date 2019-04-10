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

#include "ioITK/JpgImageWriterService.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/IWriter.hpp>

#include <fwItkIO/JpgImageWriter.hpp>

#include <fwServices/macros.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioITK::JpgImageWriterService, ::fwData::Image );

//------------------------------------------------------------------------------

JpgImageWriterService::JpgImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

JpgImageWriterService::~JpgImageWriterService() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType JpgImageWriterService::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

void JpgImageWriterService::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void JpgImageWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory to save image" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder(result->getFolder());
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        m_writeFailed = true;
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void JpgImageWriterService::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void JpgImageWriterService::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void JpgImageWriterService::info(std::ostream& _sstream )
{
    _sstream << "JpgImageWriterService::info";
}

//------------------------------------------------------------------------------

void JpgImageWriterService::saveImage(const ::boost::filesystem::path& imgPath, const CSPTR(::fwData::Image)& img)
{
    SLM_TRACE_FUNC();
    ::fwItkIO::JpgImageWriter::sptr writer = ::fwItkIO::JpgImageWriter::New();
    ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving image... ");

    ::fwData::location::Folder::sptr loc = ::fwData::location::Folder::New();
    loc->setFolder(imgPath);
    writer->setLocation(loc);
    writer->setObject(img);

    try
    {
        writer->addHandler( progressMeterGUI );
        writer->write();

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

void JpgImageWriterService::updating()
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", image);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        saveImage(this->getFolder(), image);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
