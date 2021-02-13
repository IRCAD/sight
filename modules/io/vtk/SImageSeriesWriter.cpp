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

#include "modules/io/vtk/SImageSeriesWriter.hpp"

#include "modules/io/vtk/SImageWriter.hpp"

#include <core/base.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/tools/Failed.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>

#include <service/macros.hpp>

#include <io/base/reader/IObjectReader.hpp>
#include <io/base/service/IWriter.hpp>
#include <io/vtk/ImageWriter.hpp>
#include <io/vtk/MetaImageWriter.hpp>
#include <io/vtk/VtiImageWriter.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace sight::modules::io::vtk
{

fwServicesRegisterMacro( ::sight::io::base::service::IWriter, ::sight::modules::io::vtk::SImageSeriesWriter,
                         ::sight::data::ImageSeries )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SImageSeriesWriter::SImageSeriesWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SImageSeriesWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an file to save an image" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
        this->setFile(result->getPath());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::starting()
{
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::configuring()
{
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::info(std::ostream& _sstream )
{
    _sstream << "SImageSeriesWriter::info";
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::updating()
{

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::ImageSeries::csptr imageSeries =
            this->getInput< data::ImageSeries >(sight::io::base::service::s_DATA_KEY);
        SLM_ASSERT("The input key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", imageSeries);

        SLM_ASSERT("Image from ImageSeries is not instanced", imageSeries->getImage());

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        try
        {
            SImageWriter::saveImage(this->getFile(), imageSeries->getImage(), m_sigJobCreated);
        }
        catch(core::tools::Failed& e)
        {
            m_writeFailed = true;
            FW_RAISE_EXCEPTION(e);
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
