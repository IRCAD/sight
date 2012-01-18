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

#include <fwComEd/PatientDBMsg.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <fwGui/dialog/PulseProgressDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/location/Folder.hpp>
//#include <fwData/location/SingleFile.hpp>

#include <io/IWriter.hpp>
#include <gdcmIO/writer/DicomPatientDBWriterManager.hpp>

#include "ioGdcm/DicomPatientDBWriterService.hpp"

namespace ioGdcm
{

REGISTER_SERVICE( ::io::IWriter , ::ioGdcm::DicomPatientDBWriterService , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

DicomPatientDBWriterService::DicomPatientDBWriterService() throw() :
    m_bServiceIsConfigured(false),
    m_fsPatientDBPath("")
{}

//------------------------------------------------------------------------------

DicomPatientDBWriterService::~DicomPatientDBWriterService() throw()
{}

//------------------------------------------------------------------------------

void DicomPatientDBWriterService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void DicomPatientDBWriterService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(this->getSelectorDialogTitle());
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr  result;
    result= ::fwData::location::Folder::dynamicCast( dialogFile.show() );
//    ::fwData::location::SingleFile::sptr  result;
//    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
//        _sDefaultPath = result->getPath();
        m_fsPatientDBPath = _sDefaultPath;
        m_bServiceIsConfigured = true;
    }
}

//------------------------------------------------------------------------------

void DicomPatientDBWriterService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientDBWriterService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientDBWriterService::info(std::ostream &_sstream )
{
    _sstream << "DicomPatientDBWriterService::info" ;
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomPatientDBWriterService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    return extensions ;
}

//------------------------------------------------------------------------------

std::string DicomPatientDBWriterService::getSelectorDialogTitle()
{
    return "Choose a DICOM File";
}

//------------------------------------------------------------------------------

void DicomPatientDBWriterService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( m_bServiceIsConfigured )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::PatientDB::sptr associatedPatientDB = this->getObject< ::fwData::PatientDB >();
        SLM_ASSERT("associatedPatientDB not instanced", associatedPatientDB);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        savePatientDB(m_fsPatientDBPath, associatedPatientDB);
        cursor.setDefaultCursor();
    }
}


//------------------------------------------------------------------------------

void DicomPatientDBWriterService::savePatientDB( const ::boost::filesystem::path _patientDBPath,
        ::fwData::PatientDB::sptr _pPatientDB )
{
    SLM_TRACE_FUNC();

    ::gdcmIO::writer::DicomPatientDBWriterManager myWriter;

    myWriter.setObject(_pPatientDB);
    myWriter.setFolder(_patientDBPath);
//    myWriter.setFile(_patientDBPath);

    try
    {

#ifndef __MACOSX__
        // NOTE: No exception can be caught
        ::fwTools::ProgressToLogger progressMeterText("Loading DicomPatientDBWriterService : ");
        myWriter.addHandler( progressMeterText );
        ::fwGui::dialog::PulseProgressDialog::Stuff stuff = ::boost::bind( &::gdcmIO::writer::DicomPatientDBWriterManager::write, &myWriter);
        ::fwGui::dialog::PulseProgressDialog p2("Save Dicom Image",  stuff ,  "Operation in progress");
        p2.show();
#else
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving PatientDB ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();
#endif
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
