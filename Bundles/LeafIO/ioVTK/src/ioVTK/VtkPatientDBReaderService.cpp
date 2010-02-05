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
#include <fwComEd/PatientDBMsg.hpp>

#include <io/IReader.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwTools/Factory.hpp>

#include <vtkIO/ImageReader.hpp>
#include <fwWX/ProgressTowx.hpp>

#include "ioVTK/VtkPatientDBReaderService.hpp"


namespace ioVTK
{

REGISTER_SERVICE( ::io::IReader , ::ioVTK::VtkPatientDBReaderService , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

VtkPatientDBReaderService::VtkPatientDBReaderService() throw() :
    m_bServiceIsConfigured(false),
    m_fsImagePath("")
{}

//------------------------------------------------------------------------------

VtkPatientDBReaderService::~VtkPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::configuring() throw(::fwTools::Failed)
{
    if( m_configuration->findConfigurationElement("filename") )
    {
        std::string filename = m_configuration->findConfigurationElement("filename")->getExistingAttributeValue("id") ;
        m_fsImagePath = ::boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = true ;
        OSLM_TRACE("Filename found" << filename ) ;
    }
}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::configureWithIHM()
{
    static wxString _sDefaultPath = _("");
    wxString title = _("Choose an Vtkimage file");
    wxString folder = wxFileSelector(
            title,
            _sDefaultPath,
            wxT(""),
            wxT(""),
            wxT("Vtkimage (*.vtk)|*.vtk"),
            wxFD_FILE_MUST_EXIST,
            wxTheApp->GetTopWindow() );

    if( folder.IsEmpty() == false)
    {
        m_fsImagePath = ::boost::filesystem::path( wxConvertWX2MB(folder), ::boost::filesystem::native );
        m_bServiceIsConfigured = true;
        _sDefaultPath = wxConvertMB2WX( m_fsImagePath.branch_path().string().c_str() );
    }
}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::info(std::ostream &_sstream )
{
    _sstream << "VtkPatientDBReaderService::info";
}

//------------------------------------------------------------------------------

bool VtkPatientDBReaderService::createImage( const ::boost::filesystem::path vtkFileDir, ::fwData::Image::sptr img )
{
    SLM_TRACE("DicomImageReaderService::createImage");
    bool res = false;
    ::vtkIO::ImageReader myLoader;
    myLoader.setObject(img);
    myLoader.setFile(vtkFileDir);

    try
    {
        ::fwWX::ProgressTowx progressMeterGUI("Loading Image ");
        myLoader.addHandler( progressMeterGUI );
        myLoader.read();
        res = true;
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

    return res;

}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE("VtkPatientDBReaderService::updating()");
    if( m_bServiceIsConfigured )
    {
        ::fwData::Image::NewSptr image;
        bool res = createImage( m_fsImagePath, image );

        if (res && image != NULL )
        {
            // Build patient
            ::fwData::PatientDB::NewSptr pNewPatientDB;
            ::fwData::Patient::NewSptr pNewPatient;
            ::fwData::Study::NewSptr pNewStudy;
            ::fwData::Acquisition::NewSptr pNewAcquisition;

            pNewAcquisition->setImage(image);
            pNewStudy->addAcquisition(pNewAcquisition);
            pNewPatient->addStudy(pNewStudy);

            pNewPatient->setCRefName(m_fsImagePath.filename());
            pNewPatientDB->addPatient(pNewPatient);

            // Retrieve dataStruct associated with this service
            ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();

            ( *( pPatientDB ) ) = ( *( pNewPatientDB.get() ) ) ;

            wxBeginBusyCursor();
            notificationOfDBUpdate();
            wxEndBusyCursor();
        }
    }
}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();

    ::fwData::PatientDB::sptr pDPDB = this->getObject< ::fwData::PatientDB >();

    //::boost::shared_ptr< ::fwComEd::PatientDBMsg > msg ( new ::fwComEd::PatientDBMsg( pDPDB ) ) ;
    //msg->addMessageInformation( ::fwComEd::PatientDBMsg::NEW_PATIENT );
    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT );

//  ::boost::shared_ptr< ::fwServices::IEditionService > basicEditor = ::fwServices::get< ::fwServices::IEditionService >( pDPDB ) ;
//  basicEditor->notify( msg );
    ::fwServices::IEditionService::notify(this->getSptr(), pDPDB, msg);
}

//------------------------------------------------------------------------------

} // namespace ioInr
