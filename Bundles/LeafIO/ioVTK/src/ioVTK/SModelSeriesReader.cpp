/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwCore/base.hpp>

#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/MultiFiles.hpp>
#include <fwData/location/ILocation.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwComEd/ModelSeriesMsg.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ILocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwTools/UUID.hpp>

#include <vtkIO/MeshReader.hpp>

#include "ioVTK/MeshWriterService.hpp"
#include "ioVTK/SModelSeriesReader.hpp"


namespace ioVTK
{

fwServicesRegisterMacro( ::io::IReader , ::ioVTK::SModelSeriesReader , ::fwMedData::ModelSeries ) ;

//------------------------------------------------------------------------------

::io::IOPathType SModelSeriesReader::getIOPathType() const
{
    return ::io::FILES;
}

//------------------------------------------------------------------------------

void SModelSeriesReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(::fwGui::dialog::ILocationDialog::MULTI_FILES);
    dialogFile.setTitle("Choose vtk files to load Series");
    dialogFile.addFilter("Vtk files","*.vtk");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::MultiFiles::sptr  result;
    result = ::fwData::location::MultiFiles::dynamicCast( dialogFile.show() );
    if (result)
    {
        const ::fwData::location::ILocation::VectPathType paths = result->getPaths();
        if(!paths.empty())
        {
            _sDefaultPath = paths[0].parent_path();
            dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        }
        this->setFiles(paths);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SModelSeriesReader::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SModelSeriesReader::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SModelSeriesReader::info(std::ostream &_sstream )
{
    _sstream << "SModelSeriesReader::info";
}

//------------------------------------------------------------------------------

void SModelSeriesReader::updating() throw(::fwTools::Failed)
{
    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >() ;

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        ::fwMedData::ModelSeries::ReconstructionVectorType recDB = modelSeries->getReconstructionDB();
        BOOST_FOREACH(const ::fwData::location::ILocation::PathType& file, this->getFiles())
        {
            ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();
            this->loadMesh(file, mesh);

            ::fwData::Reconstruction::sptr rec = ::fwData::Reconstruction::New();
            rec->setMesh(mesh);
            rec->setIsVisible(true);
            rec->setOrganName(file.stem().string());
            recDB.push_back(rec);
        }
        cursor.setDefaultCursor();
        modelSeries->setReconstructionDB(recDB);

        ::fwComEd::ModelSeriesMsg::sptr msg = ::fwComEd::ModelSeriesMsg::New();
        msg->addEvent( ::fwComEd::ModelSeriesMsg::ADD_RECONSTRUCTION ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), modelSeries, msg);
    }
}

//------------------------------------------------------------------------------

void SModelSeriesReader::loadMesh( const ::boost::filesystem::path file, ::fwData::Mesh::sptr mesh )
{
    ::vtkIO::MeshReader::sptr reader = ::vtkIO::MeshReader::New();

    reader->setObject(mesh);
    reader->setFile(file);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Loading Mesh");
        reader->addHandler( progressMeterGUI );
        ::fwData::mt::ObjectWriteLock lock(mesh);
        reader->read();
    }
    catch (const std::exception & e)
    {
        std::stringstream stream;
        stream << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning",
                stream.str(),
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning",
                "Warning during loading.",
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk

