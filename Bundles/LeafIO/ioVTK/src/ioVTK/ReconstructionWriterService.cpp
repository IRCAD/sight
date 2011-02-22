/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Acquisition.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/Cursor.hpp>

#include <fwGui/dialog/ProgressDialog.hpp>
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
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an obj file to save an image");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Obj","*.obj");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr  result;
    result= ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        m_fsAcqPath = result->getPath();
        m_bServiceIsConfigured = true;
        _sDefaultPath = m_fsAcqPath.branch_path();
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
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving Reconstructions ");
        myWriter.addHandler( progressMeterGUI );
        myWriter.write();

    }
    catch (const std::exception & e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading. ";

        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Warning");
        messageBox.setMessage( ss.str() );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::WARNING);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
        messageBox.show();
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

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        saveReconstruction(m_fsAcqPath,pAcquisition);

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioInr
