/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioITK/InrImageReaderService.hpp"

#include <fwServices/macros.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <io/IReader.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwItkIO/ImageReader.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::io::IReader, ::ioITK::InrImageReaderService, ::fwData::Image );

//------------------------------------------------------------------------------

InrImageReaderService::InrImageReaderService() throw()
{
}

//------------------------------------------------------------------------------

InrImageReaderService::~InrImageReaderService() throw()
{
}


//------------------------------------------------------------------------------

::io::IOPathType InrImageReaderService::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void InrImageReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an Inrimage file");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage","*.inr.gz");
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

void InrImageReaderService::info(std::ostream &_sstream )
{
    _sstream << "InrImageReaderService::info";
}

//------------------------------------------------------------------------------

bool InrImageReaderService::createImage( const ::boost::filesystem::path &inrFileDir,
                                         const ::fwData::Image::sptr &_pImg )
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
    catch (const std::exception & e)
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

void InrImageReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        if ( this->createImage( this->getFile(), this->getObject< ::fwData::Image >() ) )
        {
            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);
            notificationOfDBUpdate();
            cursor.setDefaultCursor();
        }
    }
}

//------------------------------------------------------------------------------

void InrImageReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr pImage = this->getObject< ::fwData::Image >();
    SLM_ASSERT("pImage not instanced", pImage);

    auto sig = pImage->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
