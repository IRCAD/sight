/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/io/vtk/image_writer.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/failed.hpp>

#include <data/image.hpp>

#include <io/__/reader/object_reader.hpp>
#include <io/__/service/writer.hpp>
#include <io/vtk/ImageWriter.hpp>
#include <io/vtk/MetaImageWriter.hpp>
#include <io/vtk/VtiImageWriter.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

image_writer::image_writer() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType image_writer::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void image_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to save an image" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.setOption(ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void image_writer::starting()
{
}

//------------------------------------------------------------------------------

void image_writer::stopping()
{
}

//------------------------------------------------------------------------------

void image_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void image_writer::info(std::ostream& _sstream)
{
    _sstream << "image_writer::info";
}

//------------------------------------------------------------------------------

bool image_writer::saveImage(
    const std::filesystem::path& imgFile,
    const CSPTR(data::image)& image,
    const SPTR(JobCreatedSignalType)& sigJobCreated
)
{
    bool bValue = true;

    sight::io::writer::object_writer::sptr myWriter;
    sight::ui::dialog::progress progressMeterGUI("Saving images... ");
    std::string ext = imgFile.extension().string();
    boost::algorithm::to_lower(ext);

    if(ext == ".vtk")
    {
        auto vtkWriter = std::make_shared<sight::io::vtk::ImageWriter>();

        // Set the file system path
        vtkWriter->set_file(imgFile);
        myWriter = vtkWriter;
    }
    else if(ext == ".vti")
    {
        auto vtiWriter = std::make_shared<sight::io::vtk::VtiImageWriter>();
        vtiWriter->set_file(imgFile);
        myWriter = vtiWriter;
    }
    else if(ext == ".mhd")
    {
        auto mhdWriter = std::make_shared<sight::io::vtk::MetaImageWriter>();
        mhdWriter->set_file(imgFile);
        myWriter = mhdWriter;
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Unsupported " + ext + " format (Available formats: "
                + ".vtk, .vti, .mhd)"
            )
        );
    }

    data::mt::locked_ptr<const data::image> locked(image);
    myWriter->set_object(image);

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

        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
        bValue = false;
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving.",
            sight::ui::dialog::message::WARNING
        );
        bValue = false;
    }
    return bValue;
}

//------------------------------------------------------------------------------

void image_writer::updating()
{
    if(this->hasLocationDefined())
    {
        const auto data   = m_data.lock();
        const auto pImage = std::dynamic_pointer_cast<const data::image>(data.get_shared());
        SIGHT_ASSERT("The input key '" + sight::io::service::s_DATA_KEY + "' is not correctly set.", pImage);

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        try
        {
            this->saveImage(this->get_file(), pImage, m_sigJobCreated);
        }
        catch(core::tools::failed& e)
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

} // namespace sight::module::io::vtk
