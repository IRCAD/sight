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

#include "ioITK/InrImageReaderService.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwItkIO/ImageReader.hpp>

#include <fwServices/macros.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioITK::InrImageReaderService, ::fwData::Image );

//------------------------------------------------------------------------------

InrImageReaderService::InrImageReaderService() noexcept
{
}

//------------------------------------------------------------------------------

InrImageReaderService::~InrImageReaderService() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType InrImageReaderService::getIOPathType() const
{
    return ::fwIO::FILE;
}
//
//------------------------------------------------------------------------------

void InrImageReaderService::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void InrImageReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static std::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an Inrimage file" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage", "*.inr.gz");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        this->setFile(result->getPath());
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void InrImageReaderService::info(std::ostream& _sstream )
{
    _sstream << "InrImageReaderService::info";
}

//------------------------------------------------------------------------------

bool InrImageReaderService::createImage( const std::filesystem::path& inrFileDir,
                                         const ::fwData::Image::sptr& _pImg )
{
    SLM_TRACE_FUNC();
    ::fwItkIO::ImageReader::sptr myLoader = ::fwItkIO::ImageReader::New();
    bool ok = true;

    myLoader->setObject(_pImg);
    myLoader->setFile(inrFileDir);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Image ");
        myLoader->addHandler( progressMeterGUI );
        myLoader->read();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                          ss.str(),
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                                                          "Warning during loading",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
        ok = false;
    }
    return ok;
}

//------------------------------------------------------------------------------

void InrImageReaderService::updating()
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", image);

        if ( this->createImage( this->getFile(), image) )
        {
            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);
            notificationOfDBUpdate();
            cursor.setDefaultCursor();
        }
        else
        {
            m_readFailed = true;
        }
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void InrImageReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(::fwIO::s_DATA_KEY);
    SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", image);

    auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
