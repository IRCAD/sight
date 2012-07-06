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

#include <fwData/PatientDB.hpp>
#include <fwData/location/Folder.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <itkIO/JpgPatientDBReader.hpp>

#include "ioITK/JpgPatientDBReaderService.hpp"

namespace ioITK
{

REGISTER_SERVICE( ::io::IReader , ::ioITK::JpgPatientDBReaderService , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

JpgPatientDBReaderService::JpgPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

JpgPatientDBReaderService::~JpgPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

::io::IOPathType JpgPatientDBReaderService::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

void JpgPatientDBReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(this->getSelectorDialogTitle());
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr  result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void JpgPatientDBReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void JpgPatientDBReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void JpgPatientDBReaderService::info(std::ostream &_sstream )
{
    _sstream << "JpgPatientDBReaderService::info";
}


//------------------------------------------------------------------------------

std::vector< std::string > JpgPatientDBReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    return extensions ;
}

//------------------------------------------------------------------------------

std::string JpgPatientDBReaderService::getSelectorDialogTitle()
{
    return "Choose a directory with JPG images";
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr JpgPatientDBReaderService::createPatientDB( const ::boost::filesystem::path inrFileDir )
{
    SLM_TRACE_FUNC();
    ::itkIO::JpgPatientDBReader::NewSptr myLoader;

    ::fwData::PatientDB::NewSptr dummy;
    myLoader->setObject( dummy );
    myLoader->setFolder(inrFileDir);

    try
    {
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

void JpgPatientDBReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        ::fwData::PatientDB::sptr patientDB = createPatientDB( this->getFolder() );

        if( patientDB->getNumberOfPatients() > 0 )
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

void JpgPatientDBReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr pDPDB =  this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pDPDB not instanced", pDPDB);

    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT );

    ::fwServices::IEditionService::notify(this->getSptr(), pDPDB, msg);
}

//------------------------------------------------------------------------------

} // namespace ioITK
