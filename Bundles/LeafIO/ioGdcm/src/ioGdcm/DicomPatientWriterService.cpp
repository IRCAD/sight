/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/PatientMsg.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <fwData/Patient.hpp>
#include <fwData/location/Folder.hpp>

#include <io/IWriter.hpp>
#include <gdcmIO/writer/DicomGlobalWriterManager.hpp>

#include "ioGdcm/DicomPatientWriterService.hpp"




namespace ioGdcm
{

REGISTER_SERVICE( ::io::IWriter , ::ioGdcm::DicomPatientWriterService , ::fwData::Patient ) ;

//------------------------------------------------------------------------------

DicomPatientWriterService::DicomPatientWriterService() throw()
{}

//------------------------------------------------------------------------------

DicomPatientWriterService::~DicomPatientWriterService() throw()
{}

//------------------------------------------------------------------------------

::io::IOPathType DicomPatientWriterService::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

void DicomPatientWriterService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(this->getSelectorDialogTitle());
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr  result;
    result= ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( _sDefaultPath );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void DicomPatientWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientWriterService::info(std::ostream &_sstream )
{
    _sstream << "DicomPatientWriterService::info";
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomPatientWriterService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    return extensions ;
}

//------------------------------------------------------------------------------

std::string DicomPatientWriterService::getSelectorDialogTitle()
{
    return "Choose a directory for DICOM images";
}


//------------------------------------------------------------------------------

void DicomPatientWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Patient::sptr associatedPatient = this->getObject< ::fwData::Patient >();
        SLM_ASSERT("associatedPatient not instanced", associatedPatient);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        savePatient(this->getFolder(), associatedPatient);
        cursor.setDefaultCursor();
    }
}


//------------------------------------------------------------------------------

void DicomPatientWriterService::savePatient( const ::boost::filesystem::path _patientDBPath,
        ::fwData::Patient::sptr _pPatient )
{
    SLM_TRACE_FUNC();

    ::gdcmIO::writer::DicomGlobalWriterManager::NewSptr myWriter;

    myWriter->setObject(_pPatient);
    myWriter->setFolder(_patientDBPath);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Patient ");
        myWriter->addHandler( progressMeterGUI );
        myWriter->write();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning", "Warning during saving", ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
