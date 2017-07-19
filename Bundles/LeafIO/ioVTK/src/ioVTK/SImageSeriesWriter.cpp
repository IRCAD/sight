/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioVTK/SImageSeriesWriter.hpp"

#include "ioVTK/SImageWriter.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/reader/IObjectReader.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>

#include <fwMedData/ImageSeries.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Failed.hpp>

#include <fwVtkIO/ImageWriter.hpp>
#include <fwVtkIO/MetaImageWriter.hpp>
#include <fwVtkIO/VtiImageWriter.hpp>

#include <io/IWriter.hpp>

#include <boost/algorithm/string.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::io::IWriter, ::ioVTK::SImageSeriesWriter, ::fwMedData::ImageSeries );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SImageSeriesWriter::SImageSeriesWriter() throw()
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
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
    static ::boost::filesystem::path _sDefaultPath("");

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose an file to save an image" : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::WRITE);

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

void SImageSeriesWriter::configuring() throw(::fwTools::Failed)
{
    ::io::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SImageSeriesWriter::info(std::ostream& _sstream )
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
        ::fwMedData::ImageSeries::sptr imageSeries = this->getObject< ::fwMedData::ImageSeries >();
        SLM_ASSERT("ImageSeries is not instanced", imageSeries);
        SLM_ASSERT("Image from ImageSeries is not instanced", imageSeries->getImage());

        ::fwGui::Cursor cursor;
        cursor.setCursor(::fwGui::ICursor::BUSY);

        try
        {
            SImageWriter::saveImage(this->getFile(), imageSeries->getImage(), m_sigJobCreated);
        }
        catch(::fwTools::Failed& e)
        {
            OSLM_TRACE("Error : " << e.what());
            FW_RAISE_EXCEPTION(e);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace ioVtk
