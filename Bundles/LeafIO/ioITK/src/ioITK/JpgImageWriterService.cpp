/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/ImageMsg.hpp>

#include <fwTools/DynamicType.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <itkIO/JpgImageWriter.hpp>

#include "ioITK/JpgImageWriterService.hpp"


namespace ioITK
{

REGISTER_SERVICE( ::io::IWriter , ::ioITK::JpgImageWriterService , ::fwData::Image ) ;

//------------------------------------------------------------------------------

JpgImageWriterService::JpgImageWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsImagePath("")
{}

//------------------------------------------------------------------------------

JpgImageWriterService::~JpgImageWriterService() throw()
{}

//------------------------------------------------------------------------------

void JpgImageWriterService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void JpgImageWriterService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a directory to save image");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr  result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        m_fsImagePath = result->getFolder();
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void JpgImageWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void JpgImageWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void JpgImageWriterService::info(std::ostream &_sstream )
{
    _sstream << "JpgImageWriterService::info";
}

//------------------------------------------------------------------------------

void JpgImageWriterService::saveImage( const ::boost::filesystem::path imgPath, ::fwData::Image::sptr _pImage )
{
    SLM_TRACE_FUNC();
    ::itkIO::JpgImageWriter::NewSptr myWriter;
    ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Image ");

    ::fwData::location::Folder::NewSptr loc;
    loc->setFolder(imgPath);
    myWriter->setLocation(loc);
    myWriter->setObject(_pImage);

    try
    {
        myWriter->addHandler( progressMeterGUI );
        myWriter->write();

    }
    catch (const std::exception & e)
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

void JpgImageWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();
        SLM_ASSERT("associatedImage not instanced", associatedImage);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        saveImage(m_fsImagePath,associatedImage);
        cursor.setDefaultCursor();
    }
}


//------------------------------------------------------------------------------

} // namespace ioITK
