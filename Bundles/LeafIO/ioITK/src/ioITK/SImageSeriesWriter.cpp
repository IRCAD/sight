/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>


#include <io/IWriter.hpp>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>

#include <fwItkIO/ImageWriter.hpp>

#include "ioITK/InrImageWriterService.hpp"
#include "ioITK/SImageSeriesWriter.hpp"


namespace ioITK
{

fwServicesRegisterMacro( ::io::IWriter, ::ioITK::SImageSeriesWriter, ::fwMedData::ImageSeries );

//------------------------------------------------------------------------------

SImageSeriesWriter::SImageSeriesWriter() throw()
{
}

//------------------------------------------------------------------------------

SImageSeriesWriter::~SImageSeriesWriter() throw()
{
}

//------------------------------------------------------------------------------

::io::IOPathType SImageSeriesWriter::getIOPathType() const
{
    return ::io::FILE;
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::configureWithIHM()
{
    SLM_TRACE_FUNC();
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle("Choose an inrimage file to save image");
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Inrimage","*.inr.gz");
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

void SImageSeriesWriter::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::info(std::ostream &_sstream )
{
    _sstream << "SImageSeriesWriter::info";
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service

        ::fwMedData::ImageSeries::sptr iseries = this->getObject< ::fwMedData::ImageSeries >();
        const ::fwData::Image::sptr &associatedImage = iseries->getImage();
        SLM_ASSERT("associatedImage not instanced", associatedImage);

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);
        InrImageWriterService::saveImage(this->getFile(), associatedImage);
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioITK
