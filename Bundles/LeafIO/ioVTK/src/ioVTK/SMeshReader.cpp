/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVTK/SMeshReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwJobs/IJob.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/MeshReader.hpp>

#include <boost/filesystem/operations.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::io::IReader, ::ioVTK::SMeshReader, ::fwData::Mesh );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

::io::IOPathType SMeshReader::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

SMeshReader::SMeshReader() throw()
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void SMeshReader::configureWithIHM()
{
    SLM_TRACE_FUNC();

    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a vtk file to load Mesh" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

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

void SMeshReader::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SMeshReader::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SMeshReader::configuring() throw(::fwTools::Failed)
{
    ::io::IReader::configuring();
}

//------------------------------------------------------------------------------

void SMeshReader::info(std::ostream& _sstream )
{
    _sstream << "SMeshReader::info";
}

//------------------------------------------------------------------------------

void SMeshReader::loadMesh( const ::boost::filesystem::path vtkFile, ::fwData::Mesh::sptr _pMesh )
{
    SLM_TRACE_FUNC();

    ::fwVtkIO::MeshReader::sptr reader = ::fwVtkIO::MeshReader::New();

    m_sigJobCreated->emit(reader->getJob());

    reader->setObject(_pMesh);
    reader->setFile(vtkFile);

    ::fwData::mt::ObjectWriteLock lock(_pMesh);

    try
    {
        reader->read();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        std::stringstream ss;
        ss << "Warning during loading. ";
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning",
            "Warning during loading.",
            ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void SMeshReader::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwData::Mesh::sptr pMesh = this->getObject< ::fwData::Mesh >();
        SLM_ASSERT("pMesh not instanced", pMesh);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        this->loadMesh(this->getFile(), pMesh);
        this->notificationOfUpdate();

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

void SMeshReader::notificationOfUpdate()
{
    SLM_TRACE_FUNC();
    ::fwData::Mesh::sptr pMesh = this->getObject< ::fwData::Mesh >();
    SLM_ASSERT("pMesh not instanced", pMesh);

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = pMesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
