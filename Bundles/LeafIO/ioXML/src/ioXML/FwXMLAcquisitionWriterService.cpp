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

#include "ioXML/FwXMLAcquisitionWriterService.hpp"

namespace ioXML
{

REGISTER_SERVICE( ::io::IWriter , ::ioXML::FwXMLAcquisitionWriterService , ::fwData::Acquisition ) ;

//------------------------------------------------------------------------------

FwXMLAcquisitionWriterService::FwXMLAcquisitionWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsAcquisitionPath("")
{}

//------------------------------------------------------------------------------

FwXMLAcquisitionWriterService::~FwXMLAcquisitionWriterService() throw()
{}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::configureWithIHM()
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
        m_fsAcquisitionPath = ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native );
        m_bServiceIsConfigured = true;
        _sDefaultPath = wxConvertMB2WX( m_fsAcquisitionPath.branch_path().string().c_str() );
    }
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::info(std::ostream &_sstream )
{
    _sstream << "FwXMLAcquisitionWriterService::info" ;
}

//------------------------------------------------------------------------------

std::string FwXMLAcquisitionWriterService::getCfgExtensionPoint()
{
    return "" ;
}

//------------------------------------------------------------------------------

std::string FwXMLAcquisitionWriterService::getPersistanceId()
{
    return "ioITK::FwXMLAcquisitionWriterService" ;
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::saveAcquisition( const ::boost::filesystem::path inrFileDir, ::fwData::Acquisition::sptr _pAcquisition )
{
    SLM_TRACE_FUNC();
    ::fwXML::writer::FwXMLObjectWriter myWriter;

    myWriter.setObject(_pAcquisition);
    myWriter.setFile(inrFileDir);

    try
    {
        ::fwWX::ProgressTowx progressMeterGUI("Saving Image ");
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

void FwXMLAcquisitionWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("FwXMLAcquisitionWriterService::updating()");

    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Acquisition::sptr acquisition = this->getObject< ::fwData::Acquisition >();
        SLM_ASSERT("Acquisition is null", acquisition);

        wxBeginBusyCursor();
        m_fsAcquisitionPath = correctFileFormat( m_fsAcquisitionPath );
        if ( isAnFwxmlArchive( m_fsAcquisitionPath ) )
        {
            manageZipAndSaveAcquisition(m_fsAcquisitionPath, acquisition);
        }
        else
        {
            saveAcquisition(m_fsAcquisitionPath, acquisition);
        }
        wxEndBusyCursor();
    }
}

//------------------------------------------------------------------------------

::boost::filesystem::path FwXMLAcquisitionWriterService::correctFileFormat( const ::boost::filesystem::path _filePath ) const
{
    ::boost::filesystem::path newPath = _filePath;
    if ( ::boost::filesystem::extension(_filePath) != ".fxz" && ::boost::filesystem::extension(_filePath) != ".xml" )
    {
        newPath = _filePath.string() + ".fxz";
    }

    return newPath;
}

//------------------------------------------------------------------------------

bool FwXMLAcquisitionWriterService::isAnFwxmlArchive( const ::boost::filesystem::path filePath )
{
    return ( ::boost::filesystem::extension(filePath) == ".fxz" );
}

//------------------------------------------------------------------------------

void FwXMLAcquisitionWriterService::manageZipAndSaveAcquisition( const ::boost::filesystem::path inrFileDir, ::fwData::Acquisition::sptr _pAcquisition )
{
    // Save in tmp folder
    ::boost::filesystem::path srcFolder = ::fwTools::System::getTemporaryFolder() / "fwxmlArchiveFolder";
    ::boost::filesystem::path xmlfile = srcFolder / "patient.xml";
    ::boost::filesystem::create_directories( srcFolder );
    saveAcquisition(xmlfile,_pAcquisition);

    // Zip
    wxString destZipFileName ( wxConvertMB2WX( inrFileDir.string().c_str() ) );
    wxString srcFolderName ( wxConvertMB2WX( srcFolder.string().c_str() ) );
    ::fwWX::wxZipFolder::packFolder( srcFolderName, destZipFileName );

    // Remove temp folder
    ::boost::filesystem::remove_all( srcFolder );
}

//------------------------------------------------------------------------------

} // namespace ioXML
