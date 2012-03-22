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
#include <fwData/location/MultiFiles.hpp>

#include <fwTools/Factory.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include "ioITK/InrPatientDBReaderService.hpp"


namespace ioITK
{

REGISTER_SERVICE( ::io::IReader , ::ioITK::InrPatientDBReaderService , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

InrPatientDBReaderService::InrPatientDBReaderService() throw() :
        m_isRecursive(false)
{}

//------------------------------------------------------------------------------

InrPatientDBReaderService::~InrPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

::io::IOPathType InrPatientDBReaderService::getIOPathType() const
{
    return (::io::IOPathType)( ::io::FOLDER | ::io::FILES );
}

//------------------------------------------------------------------------------

void InrPatientDBReaderService::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;
    std::vector< ::boost::filesystem::path > patientDBPaths;

    std::vector<std::string> choices;
    choices.push_back("Files selection");
    choices.push_back("Directory selection");
    choices.push_back("Directory selection (recursive)");

    ::fwGui::dialog::SelectorDialog::NewSptr selector;

    selector->setTitle("Select a reader configuration");

    selector->setSelections(choices);
    std::string selection = selector->show();
    if( !selection.empty() )
    {
        ::fwGui::dialog::LocationDialog dialogFile;
        dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );

        if ( selection == choices[0] )
        {
            dialogFile.setTitle("Choose Inrimage files");
            dialogFile.setType(::fwGui::dialog::LocationDialog::MULTI_FILES);
            dialogFile.setOption(::fwGui::dialog::LocationDialog::READ);
            dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);
            dialogFile.addFilter("Inrimage", "*.inr.gz");

            ::fwData::location::MultiFiles::sptr  result;
            result= ::fwData::location::MultiFiles::dynamicCast( dialogFile.show() );
            if (result)
            {
                _sDefaultPath = result->getPaths()[0].parent_path();
                this->setFiles(result->getPaths());
                dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
            }
            else
            {
                this->clearLocations();
            }
        }
        else
        {
            dialogFile.setTitle("Choose a directory with Inrimage files");
            dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);
            dialogFile.setOption(::fwGui::dialog::LocationDialog::READ);
            dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

            ::fwData::location::Folder::sptr  result;
            result= ::fwData::location::Folder::dynamicCast( dialogFile.show() );
            if (result)
            {
                _sDefaultPath = result->getFolder();
                this->setFile(result->getFolder());
                m_isRecursive = (selection == choices[2]);
                dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
            }
            else
            {
                this->clearLocations();
            }
        }
    }
}

//------------------------------------------------------------------------------

void InrPatientDBReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void InrPatientDBReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void InrPatientDBReaderService::info(std::ostream &_sstream )
{
    _sstream << "InrPatientDBReaderService::info";
}

//------------------------------------------------------------------------------

std::vector< std::string > InrPatientDBReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back(".inr.gz");
    extensions.push_back(".inr");
    return extensions ;
}

//------------------------------------------------------------------------------

std::string InrPatientDBReaderService::getSelectorDialogTitle()
{
    return "Choose a directory with Inr images";
}

//------------------------------------------------------------------------------

::fwData::PatientDB::sptr InrPatientDBReaderService::createPatientDB()
{
    SLM_TRACE_FUNC();

    ::fwData::PatientDB::NewSptr patientDDTMP;
    ::itkIO::InrPatientDBReader::NewSptr loader;
    loader->setObject(patientDDTMP);
    if( ::boost::filesystem::is_directory(this->getLocations().at(0)) )
    {
        loader->setFolder(this->getFolder());
        loader->setRecursive(m_isRecursive);
    }
    else
    {
        loader->setFiles(this->getFiles());
    }

    SLM_ASSERT("No location specified", loader->getLocation() );
    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Image ");
        loader->addHandler( progressMeterGUI );
        loader->read();
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

    return loader->getConcreteObject();
}

//------------------------------------------------------------------------------

void InrPatientDBReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        ::fwData::PatientDB::sptr patientDB = createPatientDB();

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

void InrPatientDBReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr pDPDB = this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pDPDB not instanced", pDPDB);

    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT );

    ::fwServices::IEditionService::notify(this->getSptr(), pDPDB, msg);
}

//------------------------------------------------------------------------------

} // namespace ioITK
