/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVTK/SModelSeriesWriter.hpp"

#include "ioVTK/SMeshWriter.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ILocationDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwJobs/IJob.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/UUID.hpp>

#include <fwVtkIO/MeshWriter.hpp>

#include <boost/filesystem/operations.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioVTK::SModelSeriesWriter, ::fwMedData::ModelSeries );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SModelSeriesWriter::SModelSeriesWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SModelSeriesWriter::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialog;
    dialog.setTitle(m_windowTitle.empty() ? "Choose a directory to save meshes" : m_windowTitle);
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

void SModelSeriesWriter::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::info(std::ostream& _sstream )
{
    _sstream << "SModelSeriesWriter::info";
}

//------------------------------------------------------------------------------

void SModelSeriesWriter::updating()
{
    SLM_TRACE_FUNC();

    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::ModelSeries::csptr modelSeries = this->getInput< ::fwMedData::ModelSeries >(::fwIO::s_DATA_KEY);
        if (!modelSeries)
        {
            FW_DEPRECATED_KEY(::fwIO::s_DATA_KEY, "inout", "18.0");
            modelSeries = this->getObject< ::fwMedData::ModelSeries >();
        }
        SLM_ASSERT("ModelSeries is not instanced", modelSeries);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        const ::fwMedData::ModelSeries::ReconstructionVectorType& recs = modelSeries->getReconstructionDB();
        for(const ::fwData::Reconstruction::csptr& rec :  recs)
        {
            SLM_ASSERT("Reconstruction from model series is not instanced", rec);
            ::fwData::Mesh::sptr mesh = rec->getMesh();
            SLM_ASSERT("Mesh from reconstruction is not instanced", mesh);

            ::fwVtkIO::MeshWriter::sptr writer = ::fwVtkIO::MeshWriter::New();
            m_sigJobCreated->emit(writer->getJob());

            writer->setObject(mesh);
            writer->setFile(this->getFolder() / (rec->getOrganName() + "_" + ::fwTools::UUID::get(mesh) + ".vtk"));

            try
            {
                writer->write();
            }
            catch (const std::exception& e)
            {
                std::stringstream ss;
                ss << "Warning during saving : " << e.what();

                ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Warning",
                    ss.str(),
                    ::fwGui::dialog::IMessageDialog::WARNING);
            }
            catch( ... )
            {
                ::fwGui::dialog::MessageDialog::showMessageDialog(
                    "Warning",
                    "Warning during saving",
                    ::fwGui::dialog::IMessageDialog::WARNING);
            }
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
