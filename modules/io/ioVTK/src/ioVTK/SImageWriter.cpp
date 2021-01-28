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

#include "ioVTK/SImageWriter.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/tools/Failed.hpp>

#include <data/Image.hpp>
#include <data/location/Folder.hpp>
#include <data/location/SingleFile.hpp>

#include <fwDataIO/reader/IObjectReader.hpp>

#include <fwIO/IWriter.hpp>

#include <fwVtkIO/BitmapImageWriter.hpp>
#include <fwVtkIO/ImageWriter.hpp>
#include <fwVtkIO/MetaImageWriter.hpp>
#include <fwVtkIO/VtiImageWriter.hpp>

#include <services/macros.hpp>

#include <boost/algorithm/string.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

namespace ioVTK
{

fwServicesRegisterMacro( ::fwIO::IWriter, ::ioVTK::SImageWriter, ::sight::data::Image )

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SImageWriter::SImageWriter() noexcept
{
    m_sigJobCreated = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SImageWriter::getIOPathType() const
{
    return ::fwIO::FILE;
}

//------------------------------------------------------------------------------

void SImageWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SImageWriter::openLocationDialog()
{
    static std::filesystem::path _sDefaultPath("");

    ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to save an image" : m_windowTitle);
    dialogFile.setDefaultLocation( data::location::Folder::New(_sDefaultPath) );
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("Bitmap images", "*.bmp *.jpeg *.jpg *.png *.pnm *.tiff");
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

void SImageWriter::starting()
{
}

//------------------------------------------------------------------------------

void SImageWriter::stopping()
{
}

//------------------------------------------------------------------------------

void SImageWriter::configuring()
{
    ::fwIO::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SImageWriter::info(std::ostream& _sstream )
{
    _sstream << "SImageWriter::info";
}

//------------------------------------------------------------------------------

bool SImageWriter::saveImage( const std::filesystem::path& imgFile,
                              const CSPTR(data::Image)& image,
                              const SPTR(JobCreatedSignalType)& sigJobCreated )
{
    bool bValue = true;

    ::fwDataIO::writer::IObjectWriter::sptr myWriter;
    ui::base::dialog::ProgressDialog progressMeterGUI("Saving images... ");
    std::string ext = imgFile.extension().string();
    ::boost::algorithm::to_lower(ext);

    if(ext == ".vtk")
    {
        ::fwVtkIO::ImageWriter::sptr vtkWriter = ::fwVtkIO::ImageWriter::New();
        // Set the file system path
        vtkWriter->setFile(imgFile);
        myWriter = vtkWriter;
    }
    else if(ext == ".vti")
    {
        ::fwVtkIO::VtiImageWriter::sptr vtiWriter = ::fwVtkIO::VtiImageWriter::New();
        vtiWriter->setFile(imgFile);
        myWriter = vtiWriter;
    }
    else if(ext == ".mhd")
    {
        ::fwVtkIO::MetaImageWriter::sptr mhdWriter = ::fwVtkIO::MetaImageWriter::New();
        mhdWriter->setFile(imgFile);
        myWriter = mhdWriter;
    }
    else if(ext == ".bmp" || ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".pnm" || ext == ".tiff")
    {
        // Get image information
        std::string type = image->getType().string();
        int noc          = image->getNumberOfComponents();

        // Ensure type compatibility with output format
        // To warn the user when he/she attempts to export data in the wrong format
        // Special case for png as it can handle uin16 and 4 component data
        if(ext == ".png")
        {
            // Check data type
            if(type != "uint8" && type != "uint16")
            {
                ui::base::dialog::MessageDialog::show(
                    "Warning",
                    "Unsupported " + type + " format for " + ext + " export.\n The image will not be exported.",
                    ui::base::dialog::IMessageDialog::WARNING);
                return false;
            }
            // Check number of components
            if(noc < 1 || noc > 4)
            {
                ui::base::dialog::MessageDialog::show(
                    "Warning",
                    "Unsupported number of components (" + std::to_string(noc) + ") for " +
                    ext + " export.\n The image will not be exported.",
                    ui::base::dialog::IMessageDialog::WARNING);
                return false;
            }
        }
        // Otherwise ensure that we have 1 to 3 components with uint8 type
        else
        {
            if(type != "uint8")
            {
                ui::base::dialog::MessageDialog::show(
                    "Warning",
                    "Unsupported " + type + " format for " + ext + " export.\n The image will not be exported.",
                    ui::base::dialog::IMessageDialog::WARNING);
                return false;
            }
            // Check number of components
            if(noc < 1 || noc > 3)
            {
                ui::base::dialog::MessageDialog::show(
                    "Warning",
                    "Unsupported number of components (" + std::to_string(noc) + ") for " +
                    ext + " export.\n The image will not be exported.",
                    ui::base::dialog::IMessageDialog::WARNING);
                return false;
            }
        }

        ::fwVtkIO::BitmapImageWriter::sptr bitmapImageWriter = ::fwVtkIO::BitmapImageWriter::New();
        bitmapImageWriter->setFile(imgFile);
        myWriter = bitmapImageWriter;
    }
    else
    {
        FW_RAISE_EXCEPTION(core::tools::Failed("Unsupported " + ext + " format (Available formats: " +
                                               ".vtk, .vti, .mhd, .bmp, .jpg, .jpeg, .png, .pnm, .tiff)"));
    }

    myWriter->setObject(image);

    sigJobCreated->emit(myWriter->getJob());

    try
    {
        // Launch writing process
        myWriter->write();
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();

        ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            ui::base::dialog::IMessageDialog::WARNING);
        bValue = false;
    }
    catch( ... )
    {
        ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during saving.",
            ui::base::dialog::IMessageDialog::WARNING);
        bValue = false;
    }
    return bValue;
}

//------------------------------------------------------------------------------

void SImageWriter::updating()
{

    if( this->hasLocationDefined() )
    {
        // Retrieve dataStruct associated with this service
        data::Image::csptr pImage = this->getInput< data::Image >(::fwIO::s_DATA_KEY);
        SLM_ASSERT("The input key '" + ::fwIO::s_DATA_KEY + "' is not correctly set.", pImage);

        ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        try
        {
            this->saveImage(this->getFile(), pImage, m_sigJobCreated);
        }
        catch(core::tools::Failed& e)
        {
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
