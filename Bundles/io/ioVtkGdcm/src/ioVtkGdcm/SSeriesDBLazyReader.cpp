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

#include "ioVtkGdcm/SSeriesDBLazyReader.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

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

#include <vtkGdcmIO/SeriesDBLazyReader.hpp>

namespace ioVtkGdcm
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioVtkGdcm::SSeriesDBLazyReader, ::fwMedData::SeriesDB );
static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SSeriesDBLazyReader::SSeriesDBLazyReader() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

SSeriesDBLazyReader::~SSeriesDBLazyReader() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesDBLazyReader::configureWithIHM()
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
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void SSeriesDBLazyReader::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBLazyReader::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBLazyReader::configuring()
{
    ::fwIO::IReader::configuring();
}

//------------------------------------------------------------------------------

void SSeriesDBLazyReader::info(std::ostream& _sstream )
{
    _sstream << "SSeriesDBLazyReader::info";
}

//------------------------------------------------------------------------------

SSeriesDBLazyReader::ExtensionsType SSeriesDBLazyReader::getSupportedExtensions()
{
    ExtensionsType extensions;
    return extensions;
}

//------------------------------------------------------------------------------

std::string SSeriesDBLazyReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SSeriesDBLazyReader::createSeriesDB(const ::boost::filesystem::path& dicomDir)
{
    SLM_TRACE_FUNC();
    ::vtkGdcmIO::SeriesDBLazyReader::sptr myLoader = ::vtkGdcmIO::SeriesDBLazyReader::New();
    ::fwMedData::SeriesDB::sptr dummy              = ::fwMedData::SeriesDB::New();
    myLoader->setObject(dummy);
    myLoader->setFolder(dicomDir);

    m_sigJobCreated->emit(myLoader->getJob());

    try
    {
        myLoader->read();
    }
    catch (const std::exception& e)
    {
        m_readFailed = true;
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        m_readFailed = true;
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
    }

    return myLoader->getConcreteObject();
}

//------------------------------------------------------------------------------

void SSeriesDBLazyReader::updating()
{
    if( this->hasLocationDefined() )
    {
        ::fwMedData::SeriesDB::sptr seriesDB = createSeriesDB( this->getFolder() );

        if( seriesDB->size() > 0 )
        {
            // Retrieve dataStruct associated with this service
            ::fwMedData::SeriesDB::sptr associatedSeriesDB =
                this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);

            if(!associatedSeriesDB)
            {
                m_readFailed = true;
            }

            SLM_ASSERT("The inout key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", associatedSeriesDB);

            associatedSeriesDB->shallowCopy( seriesDB );

            ::fwGui::Cursor cursor;
            cursor.setCursor(::fwGui::ICursor::BUSY);
            this->notificationOfDBUpdate();
            cursor.setDefaultCursor();
        }
        else
        {
            m_readFailed = true;
            ::fwGui::dialog::MessageDialog::showMessageDialog(
                "Image Reader", "This file can not be read. Retry with another file reader.",
                ::fwGui::dialog::IMessageDialog::WARNING);
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesDBLazyReader::notificationOfDBUpdate()
{
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

::fwIO::IOPathType SSeriesDBLazyReader::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

} // namespace ioInr
