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
    m_sigJobCreated = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType image_writer::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void image_writer::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose a file to save an image" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.addFilter("Vtk", "*.vtk");
    dialog_file.addFilter("Vti", "*.vti");
    dialog_file.addFilter("MetaImage", "*.mhd");
    dialog_file.setOption(ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
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
    const std::filesystem::path& _img_file,
    const CSPTR(data::image)& _image,
    const SPTR(job_created_signal_t)& _sig_job_created
)
{
    bool b_value = true;

    sight::io::writer::object_writer::sptr my_writer;
    sight::ui::dialog::progress progress_meter_gui("Saving images... ");
    std::string ext = _img_file.extension().string();
    boost::algorithm::to_lower(ext);

    if(ext == ".vtk")
    {
        auto vtk_writer = std::make_shared<sight::io::vtk::ImageWriter>();

        // Set the file system path
        vtk_writer->set_file(_img_file);
        my_writer = vtk_writer;
    }
    else if(ext == ".vti")
    {
        auto vti_writer = std::make_shared<sight::io::vtk::VtiImageWriter>();
        vti_writer->set_file(_img_file);
        my_writer = vti_writer;
    }
    else if(ext == ".mhd")
    {
        auto mhd_writer = std::make_shared<sight::io::vtk::MetaImageWriter>();
        mhd_writer->set_file(_img_file);
        my_writer = mhd_writer;
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

    data::mt::locked_ptr<const data::image> locked(_image);
    my_writer->set_object(_image);

    _sig_job_created->emit(my_writer->getJob());

    try
    {
        // Launch writing process
        my_writer->write();
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
        b_value = false;
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving.",
            sight::ui::dialog::message::WARNING
        );
        b_value = false;
    }
    return b_value;
}

//------------------------------------------------------------------------------

void image_writer::updating()
{
    if(this->hasLocationDefined())
    {
        const auto data    = m_data.lock();
        const auto p_image = std::dynamic_pointer_cast<const data::image>(data.get_shared());
        SIGHT_ASSERT("The input key '" + sight::io::service::s_DATA_KEY + "' is not correctly set.", p_image);

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        try
        {
            this->saveImage(this->get_file(), p_image, m_sigJobCreated);
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
