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

#include "ioITK/SImageSeriesWriter.hpp"

#include "ioITK/InrImageWriterService.hpp"

#include <core/base.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/IWriter.hpp>

#include <fwItkIO/ImageWriter.hpp>

#include <fwServices/macros.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioITK::SImageSeriesWriter, data::ImageSeries )

//------------------------------------------------------------------------------

SImageSeriesWriter::SImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

SImageSeriesWriter::~SImageSeriesWriter() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SImageSeriesWriter::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an inrimage file to save image" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage", "*.inr.gz");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    data::location::SingleFile::sptr result;
    result = data::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        this->setFile( result->getPath() );
        dialogFile.saveDefaultLocation( data::location::Folder::New(_sDefaultPath) );
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

        data::ImageSeries::csptr iseries = this->getInput< data::ImageSeries >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", iseries);

        const data::Image::csptr& associatedImage = iseries->getImage();
        SLM_ASSERT("associatedImage not instanced", associatedImage);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        InrImageWriterService::saveImage(this->getFile(), associatedImage);
        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
