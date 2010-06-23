/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwTools/System.hpp>

#include <fwData/Acquisition.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>

#include <fwWX/ProgressTowx.hpp>
#include <fwWX/wxZipFolder.hpp>

#include <fwGui/MessageDialog.hpp>

#include "ioXML/FwXMLGenericWriterService.hpp"

namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLGenericWriterService , ::fwTools::Object ) ;

//------------------------------------------------------------------------------

FwXMLGenericWriterService::FwXMLGenericWriterService() throw()
{}

//------------------------------------------------------------------------------

FwXMLGenericWriterService::~FwXMLGenericWriterService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::configuring() throw(::fwTools::Failed)
{
    m_writer.setFile( ::boost::filesystem::path("SAVEDGRAPH.fxz") );
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::configureWithIHM()
{
    static wxString _sDefaultPath = _("");
    wxString title = _("Choose a fxz or a xml file");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("fwXML archive (*.fxz)|*.fxz|fwXML (*.xml)|*.xml"),
#if wxCHECK_VERSION(2, 8, 0)
            wxFD_SAVE,
#else
            wxSAVE,
#endif
            wxTheApp->GetTopWindow() );

    if( folder.IsEmpty() == false)
    {
        m_writer.setFile(  ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native ) );
        _sDefaultPath = wxConvertMB2WX( m_writer.getFile().branch_path().string().c_str() );
    }
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLGenericWriterService::info" ;
}

//------------------------------------------------------------------------------

std::string FwXMLGenericWriterService::getCfgExtensionPoint()
{
    return "" ;
}

//------------------------------------------------------------------------------

std::string FwXMLGenericWriterService::getPersistanceId()
{
    return "ioITK::FwXMLGenericWriterService" ;
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::saveData( const ::boost::filesystem::path path, ::fwTools::Object::sptr _obj )
{
    SLM_TRACE_FUNC();
    ::fwXML::writer::FwXMLObjectWriter myWriter;

    myWriter.setObject(_obj);
    myWriter.setFile(path);

    try
    {
        ::fwWX::ProgressTowx progressMeterGUI("Saving data ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::IMessageDialog::Icons icon = ::fwGui::IMessageDialog::WARNING;
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading : ";
        ::fwGui::IMessageDialog::Icons icon = ::fwGui::IMessageDialog::WARNING;
        ::fwGui::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::IMessageDialog::OK);
        messageBox.show();
    }
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLGenericWriterService::updating()");

    if( !m_writer.getFile().empty() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Object::sptr obj = this->getObject< ::fwData::Object >();
        SLM_ASSERT("data is null", obj);

        wxBeginBusyCursor();
        m_writer.setFile( correctFileFormat( m_writer.getFile() ));
        if ( isAnFwxmlArchive( m_writer.getFile() ) )
        {
            manageZipAndSaveData( m_writer.getFile(), obj);
        }
        else
        {
            saveData(m_writer.getFile(), obj);
        }
        wxEndBusyCursor();
    }
}

//------------------------------------------------------------------------------

::boost::filesystem::path FwXMLGenericWriterService::correctFileFormat( const ::boost::filesystem::path _filePath ) const
{
    ::boost::filesystem::path newPath = _filePath;
    if ( ::boost::filesystem::extension(_filePath) != ".fxz" && ::boost::filesystem::extension(_filePath) != ".xml" )
    {
        newPath = _filePath.string() + ".fxz";
    }

    return newPath;
}

//------------------------------------------------------------------------------

bool FwXMLGenericWriterService::isAnFwxmlArchive( const ::boost::filesystem::path filePath )
{
    return ( ::boost::filesystem::extension(filePath) == ".fxz" );
}

//------------------------------------------------------------------------------

void FwXMLGenericWriterService::manageZipAndSaveData( const ::boost::filesystem::path path, ::fwTools::Object::sptr _obj )
{
    // Save in tmp folder
    ::boost::filesystem::path srcFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";
    ::boost::filesystem::path xmlfile = srcFolder / "root.xml";
    ::boost::filesystem::create_directories( srcFolder );
    saveData(xmlfile,_obj);

    // Zip
    wxString destZipFileName ( wxConvertMB2WX( path.string().c_str() ) );
    wxString srcFolderName ( wxConvertMB2WX( srcFolder.string().c_str() ) );
    ::fwWX::wxZipFolder::packFolder( srcFolderName, destZipFileName );

    // Remove temp folder
    ::boost::filesystem::remove_all( srcFolder );
}

//------------------------------------------------------------------------------

} // namespace ioXML
