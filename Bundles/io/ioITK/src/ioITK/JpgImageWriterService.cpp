/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

void JpgImageWriterService::saveImage(const ::boost::filesystem::path& imgPath, const SPTR(::fwData::Image)& img)
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
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(::fwIO::s_DATA_KEY);
        if (!image)
        {
            FW_DEPRECATED_KEY(::fwIO::s_DATA_KEY, "inout", "fw4spl_18.0");
            image = this->getObject< ::fwData::Image >();
        }
        SLM_ASSERT("'" + ::fwIO::s_DATA_KEY + "' key is not defined", image);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        saveImage(this->getFolder(), image);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
