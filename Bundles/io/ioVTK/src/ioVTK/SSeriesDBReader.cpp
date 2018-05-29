/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVTK/SSeriesDBReader.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/location/Folder.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/SeriesDBReader.hpp>

#include <boost/filesystem/operations.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioVTK::SSeriesDBReader, ::fwMedData::SeriesDB );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

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
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setType(::fwGui::dialog::ILocationDialog::MULTI_FILES);
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose vtk files to load Series" : m_windowTitle);
    dialogFile.addFilter("Vtk", "*.vtk *.vti *.mhd *.vtu");
    dialogFile.addFilter("Vtk files", "*.vtk");
    dialogFile.addFilter("Vti files", "*.vti");
    dialogFile.addFilter("Vtu files", "*.vtu");
    dialogFile.addFilter("MetaImage files", "*.mhd");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::FILE_MUST_EXIST);

    ::fwData::location::MultiFiles::sptr result;
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

void SSeriesDBReader::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::stopping()
{
    SLM_TRACE_FUNC();
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

void SSeriesDBReader::loadSeriesDB( const ::fwData::location::ILocation::VectPathType& vtkFiles,
                                    const ::fwMedData::SeriesDB::sptr& seriesDB )
{
    ::fwVtkIO::SeriesDBReader::sptr reader = ::fwVtkIO::SeriesDBReader::New();
    reader->setObject(seriesDB);
    reader->setFiles(vtkFiles);

    m_sigJobCreated->emit(reader->getJob());

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

void SSeriesDBReader::updating()
{
    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);
        if (!seriesDB)
        {
            FW_DEPRECATED_KEY(::fwIO::s_DATA_KEY, "inout", "fw4spl_18.0");
            seriesDB = this->getObject< ::fwMedData::SeriesDB >();
        }
        SLM_ASSERT("SeriesDB not instanced", seriesDB);

        ::fwMedData::SeriesDB::sptr localSeriesDB = ::fwMedData::SeriesDB::New();

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        this->loadSeriesDB(this->getFiles(), localSeriesDB);

        ::fwMedDataTools::helper::SeriesDB sdbHelper(seriesDB);
        sdbHelper.clear();
        // Notify removal.
        sdbHelper.notify();

        {
            ::fwData::mt::ObjectWriteLock lock(seriesDB);
            seriesDB->shallowCopy(localSeriesDB);
        }

        ::fwMedData::SeriesDB::ContainerType addedSeries = seriesDB->getContainer();

        auto sig = seriesDB->signal< ::fwMedData::SeriesDB::AddedSeriesSignalType >(
            ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG);
        sig->asyncEmit(addedSeries);

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
