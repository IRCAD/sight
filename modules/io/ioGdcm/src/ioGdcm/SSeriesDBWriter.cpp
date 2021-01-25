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

#include "ioGdcm/SSeriesDBWriter.hpp"

#include <core/base.hpp>
#include <core/tools/ProgressToLogger.hpp>

#include <data/location/Folder.hpp>
#include <data/Series.hpp>
#include <data/SeriesDB.hpp>
#include <data/tools/helper/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <fwGdcmIO/helper/Fiducial.hpp>
#include <fwGdcmIO/writer/SeriesDB.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwIO/IWriter.hpp>

#include <fwServices/macros.hpp>

namespace ioGdcm
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioGdcm::SSeriesDBWriter, data::Vector )

//------------------------------------------------------------------------------

SSeriesDBWriter::SSeriesDBWriter() noexcept :
    m_fiducialsExportMode(::fwGdcmIO::writer::Series::COMPREHENSIVE_3D_SR)
{
}

//------------------------------------------------------------------------------

SSeriesDBWriter::~SSeriesDBWriter() noexcept
{
}

//------------------------------------------------------------------------------

void SSeriesDBWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SSeriesDBWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a directory for DICOM images" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    data::location::Folder::sptr result;
    result = data::location::Folder::dynamicCast( dialogFile.show() );
    if (result && this->selectFiducialsExportMode())
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( result->getFolder() );
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SSeriesDBWriter::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesDBWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesDBWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SSeriesDBWriter::updating()
{
    if( this->hasLocationDefined() )
    {
        const std::filesystem::path& folder = this->getFolder();
        if(!std::filesystem::is_empty(folder))
        {
            ::fwGui::dialog::MessageDialog dialog;
            dialog.setMessage("Folder '"+folder.string()+"' isn't empty, files can be overwritten."
                              "\nDo you want to continue ?");
            dialog.setTitle("Folder not empty.");
            dialog.setIcon(::fwGui::dialog::MessageDialog::QUESTION);
            dialog.addButton( ::fwGui::dialog::MessageDialog::YES_NO );
            ::fwGui::dialog::MessageDialog::Buttons button = dialog.show();

            if(button == ::fwGui::dialog::MessageDialog::NO)
            {
                m_writeFailed = true;
                return;
            }
        }
        else
        {
            m_writeFailed = true;
        }

        // Retrieve dataStruct associated with this service
        data::Vector::csptr vector = this->getInput< data::Vector >(::fwIO::s_DATA_KEY);

        // Create SeriesDB
        data::SeriesDB::sptr seriesDB = data::SeriesDB::New();
        data::tools::helper::SeriesDB seriesDBHelper(seriesDB);

        for(const data::Object::sptr& object : vector->getContainer())
        {
            data::Series::sptr series = data::Series::dynamicCast(object);
            SLM_ASSERT("The container should only contain series.", series);
            seriesDBHelper.add(series);
        }

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        this->saveSeriesDB(folder, seriesDB);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

void SSeriesDBWriter::saveSeriesDB( const std::filesystem::path folder, data::SeriesDB::sptr seriesDB )
{
    ::fwGdcmIO::writer::SeriesDB::sptr writer = ::fwGdcmIO::writer::SeriesDB::New();
    writer->setObject(seriesDB);
    writer->setFiducialsExportMode(m_fiducialsExportMode);
    data::location::Folder::sptr loc = data::location::Folder::New();
    loc->setFolder(folder);
    writer->setLocation(loc);

    try
    {
        ::fwGui::dialog::ProgressDialog progressMeterGUI("Saving series ");
        writer->addHandler( progressMeterGUI );
        writer->write();
    }
    catch (const std::exception& e)
    {
        m_writeFailed = true;
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        ::fwGui::dialog::MessageDialog::show(
            "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        m_writeFailed = true;
        ::fwGui::dialog::MessageDialog::show(
            "Warning", "Warning during saving", ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//-----------------------------------------------------------------------------

::fwIO::IOPathType SSeriesDBWriter::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

bool SSeriesDBWriter::selectFiducialsExportMode()
{
    // Retrieve dataStruct associated with this service
    data::Vector::csptr vector = this->getInput< data::Vector >(::fwIO::s_DATA_KEY);

    // Create SeriesDB
    data::SeriesDB::sptr seriesDB = data::SeriesDB::New();
    data::tools::helper::SeriesDB seriesDBHelper(seriesDB);

    for(const data::Object::sptr& object : vector->getContainer())
    {
        data::Series::sptr series = data::Series::dynamicCast(object);
        SLM_ASSERT("The container should only contain series.", series);
        seriesDBHelper.add(series);
    }

    const bool containsLandmarks   = ::fwGdcmIO::helper::Fiducial::containsLandmarks(seriesDB);
    const bool containsDistances   = ::fwGdcmIO::helper::Fiducial::containsDistances(seriesDB);
    const bool contains3DDistances = ::fwGdcmIO::helper::Fiducial::contains3DDistances(seriesDB);

    if(containsLandmarks || containsDistances)
    {
        static const std::string fiducialIOD          = "Spatial Fiducials";
        static const std::string comprehensiveSRIOD   = "Comprehensive SR";
        static const std::string comprehensive3DSRIOD = "Comprehensive 3D SR";

        std::vector< std::string > exportModes;
        if(!containsDistances)
        {
            exportModes.push_back(fiducialIOD);
        }
        if(!contains3DDistances)
        {
            exportModes.push_back(comprehensiveSRIOD);
        }
        exportModes.push_back(comprehensive3DSRIOD);

        // Create selector
        ::fwGui::dialog::SelectorDialog::sptr selector = ::fwGui::dialog::SelectorDialog::New();

        selector->setTitle("Fiducials export mode");
        selector->setSelections(exportModes);
        const std::string mode             = selector->show();
        const bool modeSelectionIsCanceled = mode.empty();

        if(mode == fiducialIOD)
        {
            m_fiducialsExportMode = ::fwGdcmIO::writer::Series::SPATIAL_FIDUCIALS;
        }
        else if(mode == comprehensiveSRIOD)
        {
            m_fiducialsExportMode = ::fwGdcmIO::writer::Series::COMPREHENSIVE_SR;
        }
        else
        {
            m_fiducialsExportMode = ::fwGdcmIO::writer::Series::COMPREHENSIVE_3D_SR;
        }

        if(mode.empty())
        {
            m_writeFailed = true;
        }

        return !modeSelectionIsCanceled;
    }

    return true;
}

} // namespace ioGdcm
