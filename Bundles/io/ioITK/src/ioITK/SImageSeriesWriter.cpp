/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioITK/SImageSeriesWriter.hpp"

#include "ioITK/InrImageWriterService.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwIO/IWriter.hpp>

#include <fwItkIO/ImageWriter.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwServices/macros.hpp>

namespace ioITK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioITK::SImageSeriesWriter, ::fwMedData::ImageSeries );

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
//
void SImageSeriesWriter::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an inrimage file to save image" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage", "*.inr.gz");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

    ::fwData::location::SingleFile::sptr result;
    result = ::fwData::location::SingleFile::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getPath().parent_path();
        this->setFile( result->getPath() );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::stopping()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::info(std::ostream& _sstream )
{
    _sstream << "SImageSeriesWriter::info";
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::updating()
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service

        ::fwMedData::ImageSeries::sptr iseries = this->getObject< ::fwMedData::ImageSeries >();
        const ::fwData::Image::sptr& associatedImage = iseries->getImage();
        SLM_ASSERT("associatedImage not instanced", associatedImage);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        InrImageWriterService::saveImage(this->getFile(), associatedImage);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
