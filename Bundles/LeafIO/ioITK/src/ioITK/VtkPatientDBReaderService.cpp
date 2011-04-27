/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/PatientDBMsg.hpp>

#include <io/IReader.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwTools/Factory.hpp>

#include <itkIO/ImageReader.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include "ioITK/VtkPatientDBReaderService.hpp"


namespace ioITK
{

REGISTER_SERVICE( ::io::IReader , ::ioITK::VtkPatientDBReaderService , ::fwData::PatientDB ) ;

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
        m_fsImagePath = boost::filesystem::path( filename ) ;
        m_bServiceIsConfigured = true ;
        OSLM_TRACE("Filename found" << filename ) ;
    }
}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();

    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a VtkImage file");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("VtkImage","*.vtk");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_fsImagePath = result->getPath();
        m_bServiceIsConfigured = true;
        _sDefaultPath = m_fsImagePath.branch_path();
    }
}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::info(std::ostream &_sstream )
{
    _sstream << "VtkPatientDBReaderService::info";
}

//------------------------------------------------------------------------------

::fwData::Image::sptr VtkPatientDBReaderService::createImage( const ::boost::filesystem::path inrFileDir )
{
    SLM_TRACE_FUNC();
    ::itkIO::ImageReader::NewSptr myLoader;

    ::fwData::Image::NewSptr img;
    myLoader->setObject(img);
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

    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                "Warning during loading",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }

    return myLoader->getConcreteObject();
}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( m_bServiceIsConfigured )
    {
        ::fwData::Image::sptr image = createImage( m_fsImagePath );

        if ( image != NULL )
        {
            // Build patient
            ::fwData::PatientDB::NewSptr pNewPatientDB;
            ::fwData::Patient::NewSptr pNewPatient;
            ::fwData::Study::NewSptr pNewStudy;
            ::fwData::Acquisition::NewSptr pNewAcquisition;

            pNewAcquisition->setImage(image);
            pNewStudy->addAcquisition(pNewAcquisition);
            pNewPatient->addStudy(pNewStudy);
            pNewPatient->setCRefName("anonymous");
            pNewPatient->setCRefFirstname("anonymous");
            pNewPatientDB->addPatient(pNewPatient);

            // Retrieve dataStruct associated with this service
            ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();

            pPatientDB->shallowCopy( pNewPatientDB );

            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);

            notificationOfDBUpdate();

            cursor.setDefaultCursor();
        }
    }
}

//------------------------------------------------------------------------------

void VtkPatientDBReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();

    ::fwData::PatientDB::sptr pDPDB = this->getObject< ::fwData::PatientDB >();

    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT );

    ::fwServices::IEditionService::notify(this->getSptr(), pDPDB, msg);
}

//------------------------------------------------------------------------------

} // namespace ioITK
