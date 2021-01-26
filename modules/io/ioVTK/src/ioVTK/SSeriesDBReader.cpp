/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ioVTK/SSeriesDBReader.hpp"

#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>

#include <data/location/Folder.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/SeriesDB.hpp>
#include <data/tools/helper/SeriesDB.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwVtkIO/SeriesDBReader.hpp>

#include <services/macros.hpp>

#include <filesystem>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioVTK::SSeriesDBReader, ::sight::data::SeriesDB )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SSeriesDBReader::SSeriesDBReader() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SSeriesDBReader::getIOPathType() const
{
    return ::fwIO::FILES;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configureWithIHM()
{
    this->openLocationDialog();
}
//------------------------------------------------------------------------------

void SSeriesDBReader::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(::fwGui::dialog::ILocationDialog::MULTI_FILES);
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose vtk files to load Series" : m_windowTitle);
    dialogFile.addFilter("All supported files", "*.vtk *.vtp *.vti *.mhd *.vtu *.obj *.ply *.stl");
    dialogFile.addFilter("MetaImage files", "*.mhd");
    dialogFile.addFilter("OBJ Files(.obj)", "*.obj");
    dialogFile.addFilter("PLY Files(.ply)", "*.ply");
    dialogFile.addFilter("STL Files(.stl)", "*.stl");
    dialogFile.addFilter("VTI Image files", "*.vti");
    dialogFile.addFilter("VTK Legacy Files(.vtk)", "*.vtk");
    dialogFile.addFilter("VTK Polydata Files(.vtp)", "*.vtp");
    dialogFile.addFilter("VTU Image files", "*.vtu");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    data::location::MultiFiles::sptr result;
    result = data::location::MultiFiles::dynamicCast( dialogFile.show() );
    if (result)
    {
        const data::location::ILocation::VectPathType paths = result->getPaths();
        if(!paths.empty())
        {
            _sDefaultPath = paths[0].parent_path();
            dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        }
        this->setFiles(paths);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::info(std::ostream& _sstream )
{
    _sstream << "SSeriesDBReader::info";
}

//------------------------------------------------------------------------------

void SSeriesDBReader::loadSeriesDB( const data::location::ILocation::VectPathType& vtkFiles,
                                    const data::SeriesDB::sptr& seriesDB )
{
    ::fwVtkIO::SeriesDBReader::sptr reader = ::fwVtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFiles(vtkFiles);

    m_sigJobCreated->emit(reader->getJob());

    try
    {
        reader->read();
        m_readFailed = false;
    }
    catch (const std::exception& e)
    {
        m_readFailed = true;
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        ::fwGui::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        m_readFailed = true;
        std::stringstream ss;
        ss << "Warning during loading. ";
        ::fwGui::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading.",
            ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::updating()
{
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        auto lockedSeriesDB = this->getLockedInOut< data::SeriesDB >(::fwIO::s_DATA_KEY);

        data::SeriesDB::sptr localSeriesDB = data::SeriesDB::New();

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        this->loadSeriesDB(this->getFiles(), localSeriesDB);

        data::tools::helper::SeriesDB sdbHelper(lockedSeriesDB.get_shared());
        sdbHelper.clear();
        // Notify removal.
        sdbHelper.notify();

        {
            lockedSeriesDB->shallowCopy(localSeriesDB);
        }

        data::SeriesDB::ContainerType addedSeries = lockedSeriesDB->getContainer();

        auto sig = lockedSeriesDB->signal< data::SeriesDB::AddedSeriesSignalType >(
            data::SeriesDB::s_ADDED_SERIES_SIG);
        sig->asyncEmit(addedSeries);

        cursor.setDefaultCursor();
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
