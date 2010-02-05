/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <wx/wx.h>
#include <wx/version.h>

#include <fwServices/macros.hpp>
#include <fwServices/helper.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Acquisition.hpp>

#include <fwWX/ProgressTowx.hpp>
#include <vtkIO/ReconstructionWriter.hpp>

#include "ioVTK/ReconstructionWriterService.hpp"


namespace ioVTK
{

REGISTER_SERVICE( ::io::IWriter , ::ioVTK::ReconstructionWriterService , ::fwData::Acquisition ) ;

//------------------------------------------------------------------------------

ReconstructionWriterService::ReconstructionWriterService() throw() :
        m_bServiceIsConfigured(false),
        m_fsAcqPath("")
{}

//------------------------------------------------------------------------------

ReconstructionWriterService::~ReconstructionWriterService() throw()
{}

//------------------------------------------------------------------------------

void ReconstructionWriterService::configuring() throw(::fwTools::Failed)
{
        if( m_configuration->findConfigurationElement("filename") )
        {
                std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
                m_fsAcqPath = ::boost::filesystem::path( filename ) ;
                m_bServiceIsConfigured = ::boost::filesystem::exists(m_fsAcqPath);
                OSLM_TRACE("Filename found" << filename ) ;
        }
}

//------------------------------------------------------------------------------

void ReconstructionWriterService::configureWithIHM()
{
        static wxString _sDefaultPath = _("");
        wxString title = _("Choose an obj file to save image");
        wxString file = wxFileSelector(
                        title,
                        _sDefaultPath,
                        wxT(""),
                        wxT(""),
                        wxT("Obj (*.obj)|*.obj"),
#if wxCHECK_VERSION(2, 8, 0)
                        wxFD_SAVE,
#else
                        wxSAVE,
#endif
                        wxTheApp->GetTopWindow() );

        if( file.IsEmpty() == false)
        {
                m_fsAcqPath = ::boost::filesystem::path( wxConvertWX2MB(file), ::boost::filesystem::native );
                m_bServiceIsConfigured = true;
                _sDefaultPath = wxConvertMB2WX( m_fsAcqPath.branch_path().string().c_str() );
        }
}

//------------------------------------------------------------------------------

void ReconstructionWriterService::starting() throw(::fwTools::Failed)
{
        SLM_TRACE("ReconstructionWriterService::starting()");
}

//------------------------------------------------------------------------------

void ReconstructionWriterService::stopping() throw(::fwTools::Failed)
{
        SLM_TRACE("ReconstructionWriterService::stopping()");
}

//------------------------------------------------------------------------------

void ReconstructionWriterService::info(std::ostream &_sstream )
{
        _sstream << "ReconstructionWriterService::info";
}

//------------------------------------------------------------------------------

void ReconstructionWriterService::saveReconstruction( const ::boost::filesystem::path objFile, ::boost::shared_ptr< ::fwData::Acquisition > _pAcq )
{
        SLM_TRACE("ReconstructionWriterService::saveImage");
        ::vtkIO::ReconstructionWriter myWriter;

        myWriter.setObject(_pAcq);
        myWriter.setFile(objFile);

        try
        {
                ::fwWX::ProgressTowx progressMeterGUI("Saving Reconstructions ");
                myWriter.addHandler( progressMeterGUI );
                myWriter.write();

        }
        catch (const std::exception & e)
        {
                std::stringstream ss;
                ss << "Warning during loading : " << e.what();
                wxString wxStmp( ss.str().c_str(), wxConvLocal );
                wxMessageBox( wxStmp, _("Warning"), wxOK|wxICON_WARNING );
        }
        catch( ... )
        {
                std::stringstream ss;
                ss << "Warning during loading : ";
                wxString wxStmp( ss.str().c_str(), wxConvLocal );
                wxMessageBox( wxStmp, _("Warning"), wxOK|wxICON_WARNING );
        }
}

//------------------------------------------------------------------------------

void ReconstructionWriterService::updating() throw(::fwTools::Failed)
{
        SLM_TRACE("ReconstructionWriterService::updating()");

        if( m_bServiceIsConfigured )
        {
                // Retrieve dataStruct associated with this service
                ::fwData::Acquisition::sptr pAcquisition = this->getObject< ::fwData::Acquisition >() ;
                assert(pAcquisition);

                wxBeginBusyCursor();
                saveReconstruction(m_fsAcqPath,pAcquisition);
                wxEndBusyCursor();
        }
}

//------------------------------------------------------------------------------

} // namespace ioInr
