/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/bind.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/PatientDBMsg.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <fwGui/dialog/PulseProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <io/IReader.hpp>

#include <fwData/PatientDB.hpp>

#include <itkIO/DicomPatientDBReader.hpp>

#include "ioITK/DicomPatientDBReaderService.hpp"

namespace ioITK
{

REGISTER_SERVICE( ::io::IReader , ::ioITK::DicomPatientDBReaderService , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

DicomPatientDBReaderService::DicomPatientDBReaderService() throw() :
    m_bServiceIsConfigured(false),
    m_fsPatientDBPath("")
{}

//------------------------------------------------------------------------------

DicomPatientDBReaderService::~DicomPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

void DicomPatientDBReaderService::configuring() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void DicomPatientDBReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(this->getSelectorDialogTitle());
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr  result;
    result= ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        m_fsPatientDBPath = result->getFolder();
        m_bServiceIsConfigured = true;
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
}

//------------------------------------------------------------------------------

void DicomPatientDBReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientDBReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomPatientDBReaderService::info(std::ostream &_sstream )
{
    _sstream << "DicomPatientDBReaderService::info" ;
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomPatientDBReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    return extensions ;
}

//------------------------------------------------------------------------------

std::string DicomPatientDBReaderService::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}


//------------------------------------------------------------------------------

::fwData::PatientDB::sptr DicomPatientDBReaderService::createPatientDB( const ::boost::filesystem::path dicomDir )
{
    SLM_TRACE_FUNC();
    ::itkIO::DicomPatientDBReader::NewSptr myLoader;

    ::fwData::PatientDB::NewSptr dummy;
    myLoader->setObject( dummy );
    myLoader->setFolder(dicomDir);

    try
    {
        ::fwTools::ProgressToLogger progressMeterText("Loading DicomPatientDBReaderService : ");
        myLoader->addHandler( progressMeterText );

        ::fwGui::dialog::PulseProgressDialog::Stuff stuff = ::boost::bind( &::itkIO::DicomPatientDBReader::read, myLoader);
        ::fwGui::dialog::PulseProgressDialog p2("Load Dicom Image",  stuff ,  "Operation in progress");
        p2.show();
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

void DicomPatientDBReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( m_bServiceIsConfigured )
    {
        ::fwData::PatientDB::sptr patientDB = createPatientDB( m_fsPatientDBPath );

        if( patientDB->getPatientSize() > 0 )
        {
            // Retrieve dataStruct associated with this service
            ::fwData::PatientDB::sptr associatedPatientDB = this->getObject< ::fwData::PatientDB >();
            SLM_ASSERT("associatedPatientDB not instanced", associatedPatientDB);

            associatedPatientDB->shallowCopy( patientDB );

            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);
            notificationOfDBUpdate();
            cursor.setDefaultCursor();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Image Reader",
                                        "File format unknown. Retry with another file reader.",
                                        ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void DicomPatientDBReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr pDPDB = this->getObject< ::fwData::PatientDB >() ;

    SLM_ASSERT("pDPDB not instanced", pDPDB);

    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT );

    ::fwServices::IEditionService::notify(this->getSptr(), pDPDB, msg);
}

//------------------------------------------------------------------------------

} // namespace ioITK
