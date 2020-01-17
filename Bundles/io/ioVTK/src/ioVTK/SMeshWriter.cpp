/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ioVTK/SMeshWriter.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Mesh.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwJobs/IJob.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/MeshWriter.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioVTK::SMeshWriter, ::fwData::Mesh );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SMeshWriter::SMeshWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SMeshWriter::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SMeshWriter::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static std::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a vtk file to save Mesh" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());

    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SMeshWriter::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SMeshWriter::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SMeshWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SMeshWriter::info(std::ostream& _sstream )
{
    _sstream << "SMeshWriter::info";
}

//------------------------------------------------------------------------------

void SMeshWriter::updating()
{
    if(  this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Mesh::csptr pMesh = this->getInput< ::fwData::Mesh >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", pMesh);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        ::fwVtkIO::MeshWriter::sptr writer = ::fwVtkIO::MeshWriter::New();

        m_sigJobCreated->emit(writer->getJob());

        writer->setObject(pMesh);
        writer->setFile(this->getFile());

        try
        {
            writer->write();
        }
        catch (const std::exception& e)
        {
            m_writeFailed = true;
            std::stringstream ss;
            ss << "Warning during saving : " << e.what();

            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning",
                ss.str(),
                ::fwGui::dialog::IMessageDialog::WARNING);
        }
        catch( ... )
        {
            m_writeFailed = true;
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Warning",
                "Warning during saving",
                ::fwGui::dialog::IMessageDialog::WARNING);
        }

        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
