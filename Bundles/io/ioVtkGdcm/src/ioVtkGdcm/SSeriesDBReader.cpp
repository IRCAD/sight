/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVtkGdcm/SSeriesDBReader.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwGui/backend.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwIO/IReader.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <vtkGdcmIO/SeriesDBReader.hpp>

namespace ioVtkGdcm
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioVtkGdcm::SSeriesDBReader, ::fwMedData::SeriesDB );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SSeriesDBReader::SSeriesDBReader() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

SSeriesDBReader::~SSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
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

SSeriesDBReader::ExtensionsType SSeriesDBReader::getSupportedExtensions()
{
    ExtensionsType extensions;
    return extensions;
}

//------------------------------------------------------------------------------

std::string SSeriesDBReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SSeriesDBReader::createSeriesDB(const ::boost::filesystem::path& dicomDir)
{
    SLM_TRACE_FUNC();
    ::vtkGdcmIO::SeriesDBReader::sptr reader = ::vtkGdcmIO::SeriesDBReader::New();
    ::fwMedData::SeriesDB::sptr dummy        = ::fwMedData::SeriesDB::New();
    reader->setObject(dummy);
    reader->setFolder(dicomDir);

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

void SSeriesDBReader::updating()
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        ::fwMedData::SeriesDB::sptr seriesDB = createSeriesDB( this->getFolder() );
        if( seriesDB->size() > 0  )
        {
            if(this->isStopped()) // FIXME service might be stopped while updating in a worker
            {
                return;
            }

            // Retrieve dataStruct associated with this service
            ::fwMedData::SeriesDB::sptr associatedSeriesDB =
                this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);
            SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", associatedSeriesDB);

            associatedSeriesDB->shallowCopy( seriesDB );

            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);
            this->notificationOfDBUpdate();
            cursor.setDefaultCursor();
        }
        else
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Image Reader", "This file can not be read. Retry with another file reader.",
                ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::notificationOfDBUpdate()
{
    SLM_TRACE_FUNC();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);
    SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", seriesDB);

    ::fwMedData::SeriesDB::ContainerType addedSeries;
    for( ::fwMedData::Series::sptr s :  seriesDB->getContainer() )
    {
        addedSeries.push_back(s);
    }

    auto sig = seriesDB->signal< ::fwMedData::SeriesDB::AddedSeriesSignalType >(
        ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG);
    sig->asyncEmit(addedSeries);
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SSeriesDBReader::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioInr
