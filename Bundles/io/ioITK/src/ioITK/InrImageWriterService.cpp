/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
        if (!image)
        {
            FW_DEPRECATED_KEY(::fwIO::s_DATA_KEY, "inout", "18.0");
            image = this->getObject< ::fwData::Image >();
        }
        SLM_ASSERT("'" + ::fwIO::s_DATA_KEY + "' key is not defined", image);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        saveImage(this->getFile(), image);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
