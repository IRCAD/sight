/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>

#include <fwTools/ProgressToLogger.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include <fwData/PatientDB.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/location/MultiFiles.hpp>

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwComEd/PatientDBMsg.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <io/IReader.hpp>

// Defined QT_NO_KEYWORDS because of conflict with boost::signals namespace.
#ifndef QT_NO_KEYWORDS
#define QT_NO_KEYWORDS
#define QT_NO_KEYWORDS_FWDEF
#endif
#include <QApplication>
#include "ioGdcmQt/ui/DicomdirEditor.hpp"
#include "ioGdcmQt/DicomdirPatientDBReaderService.hpp"
#include "vtkGdcmIO/DicomPatientDBReader.hpp"
#ifdef QT_NO_KEYWORDS_FWDEF
#undef QT_NO_KEYWORDS
#undef QT_NO_KEYWORDS_FWDEF
#endif
namespace ioGdcm
{

REGISTER_SERVICE( ::io::IReader , ::ioGdcm::DicomdirPatientDBReaderService , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

DicomdirPatientDBReaderService::DicomdirPatientDBReaderService() throw() :
    m_bServiceIsConfigured(false),
    m_dicomdirPath("")
{
//    readerList.insetr(std::make_pair(::ioGdcm::DCMTK, "Dicom Reader (DCMTK)"));
//    readerList.insetr(std::make_pair(::ioGdcm::GDCM, "Dicom Reader (GDCM)")); // Comming soon
//    readerList.insert(std::make_pair(::ioGdcm::ITK_GDCM, "Dicom Reader (ITK/gdcm)"));
//    readerList.insert(std::make_pair(::ioGdcm::IRCAD, "Dicom Reader (Ircad)"));
    readerList.insert(std::make_pair(::ioGdcm::VTK_GDCM, "Dicom Reader (VTK/gdcm)"));

}
//------------------------------------------------------------------------------

DicomdirPatientDBReaderService::~DicomdirPatientDBReaderService() throw()
{}

//------------------------------------------------------------------------------

void DicomdirPatientDBReaderService::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    // Select the DICOMDIR
    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose a DICOMDIR file ");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("DICOMDIR", "DICOMDIR DICOMDIR.* dicomdir dicomdir.*");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);
    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {

        _sDefaultPath = result->getPath();
        m_dicomdirPath = result->getPath();
        // Open the Dicom dir editor
        ::ioGdcm::ui::DicomdirEditor editor(qApp->activeWindow(), m_dicomdirPath);
        editor.setReader(readerList);
        m_readerAndFilenames = editor.show();
        if(!m_readerAndFilenames.second.empty())
        {
            m_bServiceIsConfigured = true;
        }
    }
 }

//------------------------------------------------------------------------------

void DicomdirPatientDBReaderService::info(std::ostream &_sstream )
{
    _sstream << "DicomdirPatientDBReaderService::info" ;
}

//------------------------------------------------------------------------------

std::vector< std::string > DicomdirPatientDBReaderService::getSupportedExtensions()
{
    std::vector< std::string > extensions ;
    extensions.push_back("DICOMDIR");
    extensions.push_back("DICOMDIR.");
    return extensions ;
}

//------------------------------------------------------------------------------

std::string DicomdirPatientDBReaderService::getSelectorDialogTitle()
{
    return "Choose a DICOMDIR";
}

//------------------------------------------------------------------------------

void DicomdirPatientDBReaderService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomdirPatientDBReaderService::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomdirPatientDBReaderService::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomdirPatientDBReaderService::updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomdirPatientDBReaderService::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::PatientDB::sptr patientDB;
    if( m_bServiceIsConfigured )
    {
        switch(m_readerAndFilenames.first)
        {
//        case ::ioGdcm::DCMTK:
//            ::dcmtkIO::dcmtkPatientDBReader readerDicomPatientDb;
//            readerDicomPatientDb.readFiles(m_readerAndFilenames.second);
//            break;

//        case ::ioGdcm::GDCM :
//            // (GDCM) to come
//            ::gdcmIO::DicomPatientDBReader readerDicomPatientDb;
//            readerDicomPatientDb.readFiles(m_readerAndFilenames.second);
//            break;
//        case ::ioGdcm::ITK_GDCM :
//            // ITK/GDCM
//            // Some work is necessary for using itkIO::DicomPatientDBReader with list of files.
//            patientDB = createPatientDB <::itkIO::DicomPatientDBReader>( m_readerAndFilenames.second);
//            break;
//        case ::ioGdcm::IRCAD :
//            ::dicomIO::DicomPatientDBReader readerDicomPatientDb;
//            readerDicomPatientDb.readFiles(m_readerAndFilenames.second);
            break;
        case ::ioGdcm::VTK_GDCM :
            //(VTK/GDCM)
            patientDB = createPatientDB < ::vtkGdcmIO::DicomPatientDBReader > ( m_readerAndFilenames.second);
            break;
        }

        if( patientDB->getNumberOfPatients() > 0 )
        {
            // Retrieve dataStruct associated with this service
            ::fwData::PatientDB::sptr associatedPatientDB = this->getObject< ::fwData::PatientDB >();
            SLM_ASSERT("associatedPatientDB not instanced", associatedPatientDB);
            associatedPatientDB->shallowCopy( patientDB ) ;

            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);
            this->notificationOfDBUpdate();
            cursor.setDefaultCursor();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Image Reader","This file can not be read. Retry with another file reader.", ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------
// Note : Using the template is for adding other readers like itkIO::DicomPatientDBReader
// so READER could be for example ::vtkGdcmIO::DicomPatientDBReader or ::itkIO::DicomPatientDBReader
template < typename READER> ::fwData::PatientDB::sptr DicomdirPatientDBReaderService::createPatientDB(const std::vector< ::boost::filesystem::path>& filenames)
{
    SLM_TRACE_FUNC();
    typename READER::NewSptr reader;

    ::fwData::PatientDB::NewSptr dummy;
    reader->setObject( dummy );
    reader->setFiles(filenames);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Dicom Image");
        reader->addHandler( progressMeterGUI );
        reader->read();
    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
    }

    return reader->getConcreteObject();

}
//------------------------------------------------------------------------------
void DicomdirPatientDBReaderService::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();

    ::fwData::PatientDB::sptr pDPDB = this->getObject< ::fwData::PatientDB >();
    SLM_ASSERT("pDPDB not instanced", pDPDB);

    ::fwComEd::PatientDBMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT );

    ::fwServices::IEditionService::notify(this->getSptr(),  pDPDB, msg);
}

//------------------------------------------------------------------------------
} // namespace ioGdcm
