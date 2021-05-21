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

#include "modules/io/vtk/SImageWriter.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/tools/Failed.hpp>

#include <data/Image.hpp>

#include <io/base/reader/IObjectReader.hpp>
#include <io/base/service/IWriter.hpp>
#include <io/vtk/BitmapImageWriter.hpp>
#include <io/vtk/ImageWriter.hpp>
#include <io/vtk/MetaImageWriter.hpp>
#include <io/vtk/VtiImageWriter.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::io::vtk
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SImageWriter::SImageWriter() noexcept
{
    m_sigJobCreated = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SImageWriter::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SImageWriter::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SImageWriter::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to save an image" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("BMP", "*.bmp");
    dialogFile.addFilter("JPEG", "*.jpg, *.jpeg");
    dialogFile.addFilter("PNG", "*.png");
    dialogFile.addFilter("TIFF", "*.tiff");
    dialogFile.setOption(ui::base::dialog::ILocationDialog::WRITE);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->setFile(result->getFile());
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
    sight::io::base::service::IWriter::configuring();
}

//------------------------------------------------------------------------------

void SImageWriter::info(std::ostream& _sstream)
{
    _sstream << "SImageWriter::info";
}

//------------------------------------------------------------------------------

bool SImageWriter::saveImage(
    const std::filesystem::path& imgFile,
    const CSPTR(data::Image)& image,
    const SPTR(JobCreatedSignalType)& sigJobCreated
)
{
    bool bValue = true;

    sight::io::base::writer::IObjectWriter::sptr myWriter;
    sight::ui::base::dialog::ProgressDialog progressMeterGUI("Saving images... ");
    std::string ext = imgFile.extension().string();
    ::boost::algorithm::to_lower(ext);

    if(ext == ".vtk")
    {
        auto vtkWriter = sight::io::vtk::ImageWriter::New();

        // Set the file system path
        vtkWriter->setFile(imgFile);
        myWriter = vtkWriter;
    }
    else if(ext == ".vti")
    {
        auto vtiWriter = sight::io::vtk::VtiImageWriter::New();
        vtiWriter->setFile(imgFile);
        myWriter = vtiWriter;
    }
    else if(ext == ".mhd")
    {
        auto mhdWriter = sight::io::vtk::MetaImageWriter::New();
        mhdWriter->setFile(imgFile);
        myWriter = mhdWriter;
    }
    else if(ext == ".bmp" || ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".tiff")
    {
        // Get image information
        std::string type = image->getType().string();
        int noc          = image->getNumberOfComponents();

        // Check type.
        // All extension handles uint8, and ".png" also handles uint16.
        if(!(type == "uint8" || (type == "uint16" && ext == ".png")))
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                "Unsupported " + type + " format for " + ext + " export.\n The image will not be exported.",
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
            return false;
        }

        // Check number of components
        if(noc < 1 || noc > 4)
        {
            sight::ui::base::dialog::MessageDialog::show(
                "Warning",
                "Unsupported number of components (" + std::to_string(noc) + ") for "
                + ext + " export.\n The image will not be exported.",
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
            return false;
        }

        sight::io::vtk::BitmapImageWriter::sptr bitmapImageWriter = sight::io::vtk::BitmapImageWriter::New();
        bitmapImageWriter->setFile(imgFile);
        myWriter = bitmapImageWriter;
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::Failed(
                "Unsupported " + ext + " format (Available formats: "
                + ".vtk, .vti, .mhd, .bmp, .jpg, .jpeg, .png, .tiff)"
            )
        );
    }

    myWriter->setObject(image);

    sigJobCreated->emit(myWriter->getJob());

    try
    {
        // Launch writing process
        myWriter->write();
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();

        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        bValue = false;
    }
    catch(...)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during saving.",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        bValue = false;
    }
    return bValue;
}

//------------------------------------------------------------------------------

void SImageWriter::updating()
{
    if(this->hasLocationDefined())
    {
        // Retrieve dataStruct associated with this service
        const auto pImage = this->getLockedInput<data::Image>(sight::io::base::service::s_DATA_KEY);
        SIGHT_ASSERT("The input key '" + sight::io::base::service::s_DATA_KEY + "' is not correctly set.", pImage);

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        try
        {
            this->saveImage(this->getFile(), pImage.get_shared(), m_sigJobCreated);
        }
        catch(core::tools::Failed& e)
        {
            SIGHT_THROW_EXCEPTION(e);
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
