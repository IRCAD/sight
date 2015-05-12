/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem/operations.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwCore/base.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/location/Folder.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ILocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwTools/UUID.hpp>

#include <fwVtkIO/MeshWriter.hpp>

#include "ioVTK/MeshWriterService.hpp"
#include "ioVTK/SModelSeriesWriter.hpp"


namespace ioVTK
{

fwServicesRegisterMacro( ::io::IWriter, ::ioVTK::SModelSeriesWriter, ::fwMedData::ModelSeries );

//------------------------------------------------------------------------------

::io::IOPathType SModelSeriesWriter::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialog;
    dialog.setTitle("Choose a directory to save meshes");
    dialog.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialog.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialog.setType(::fwGui::dialog::ILocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;

    while (result = ::fwData::location::Folder::dynamicCast( dialog.show() ))
    {
        if( ::boost::filesystem::is_empty(result->getFolder()) )
        {
            break;
        }
        // message box
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Overwrite confirmation");
        messageBox.setMessage("The selected directory is not empty. Write anyway ?");
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::YES);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::CANCEL);
        if( messageBox.show() == ::fwGui::dialog::IMessageDialog::YES)
        {
            break;
        }
    }

    if (result)
    {
        _sDefaultPath = result->getFolder().parent_path();
        dialog.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFolder(result->getFolder());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::info(std::ostream &_sstream )
{
    _sstream << "SModelSeriesWriter::info";
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::ModelSeries::sptr modelSeries = this->getObject< ::fwMedData::ModelSeries >();
        SLM_ASSERT("ModelSeries is not instanced", modelSeries);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        const ::fwMedData::ModelSeries::ReconstructionVectorType& recs = modelSeries->getReconstructionDB();
        BOOST_FOREACH(const SPTR(::fwData::Reconstruction)& rec, recs)
        {
            SLM_ASSERT("Reconstruction from model series is not instanced", rec);
            ::fwData::Mesh::sptr mesh = rec->getMesh();
            SLM_ASSERT("Mesh from reconstruction is not instanced", mesh);
            MeshWriterService::saveMesh(
                this->getFolder() / (rec->getOrganName() + "_" + ::fwTools::UUID::get(mesh) + ".vtk"),
                mesh);
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk

