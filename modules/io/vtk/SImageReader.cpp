/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/io/vtk/SImageReader.hpp"

#include <core/base.hpp>
#include <core/com/HasSignals.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/jobs/Aggregator.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/Image.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <io/base/reader/IObjectReader.hpp>
#include <io/base/service/IReader.hpp>
#include <io/vtk/BitmapImageReader.hpp>
#include <io/vtk/ImageReader.hpp>
#include <io/vtk/MetaImageReader.hpp>
#include <io/vtk/VtiImageReader.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>

#include <boost/algorithm/string.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <thread>

namespace sight::module::io::vtk
{

//------------------------------------------------------------------------------

// Register a new reader of data::Image

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SImageReader::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SImageReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    /* Initialize the available extensions for BitmapImageReader */
    std::vector<std::string> ext;
    sight::io::vtk::BitmapImageReader::getAvailableExtensions(ext);
    std::string availableExtensions = "";

    if(ext.size() > 0)
    {
        availableExtensions = "*" + ext.at(0);
        for(size_t i = 1 ; i < ext.size() ; i++)
        {
            availableExtensions = availableExtensions + " *" + ext.at(i);
        }
    }

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an image" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("Bitmap image", availableExtensions);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::FILE_MUST_EXIST);

    auto result = core::location::SingleFile::dynamicCast(dialogFile.show());
    if(result)
    {
        this->setFile(result->getFile());
        defaultDirectory->setFolder(result->getFile().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

SImageReader::SImageReader() noexcept
{
    m_sigJobCreated = newSignal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void SImageReader::starting()
{
}

//------------------------------------------------------------------------------

void SImageReader::stopping()
{
}

//------------------------------------------------------------------------------

void SImageReader::configuring()
{
    sight::io::base::service::IReader::configuring();
}

//------------------------------------------------------------------------------

void SImageReader::info(std::ostream& _sstream)
{
    _sstream << "SImageReader::info";
}

//------------------------------------------------------------------------------

void SImageReader::updating()
{
    m_readFailed = true;

    if(this->hasLocationDefined())
    {
        const auto locked = m_data.lock();
        const auto image  = std::dynamic_pointer_cast<data::Image>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::Image::classname()
            + "' or '"
            + sight::io::base::service::s_DATA_KEY
            + "' is not correctly set.",
            image
        );

        // Read new image path and update image. If the reading process is a success, we notify all listeners that image
        // has been modified.

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);
        try
        {
            // Notify other image services that a new image has been loaded.
            if(SImageReader::loadImage(this->getFile(), image, m_sigJobCreated))
            {
                m_readFailed = false;

                auto sig = image->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
                {
                    core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                    sig->asyncEmit();
                }
            }
        }
        catch(core::tools::Failed& e)
        {
            cursor.setDefaultCursor();
            SIGHT_THROW_EXCEPTION(e);
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

template<typename READER>
typename READER::sptr configureReader(const std::filesystem::path& imgFile)
{
    typename READER::sptr reader = READER::New();
    reader->setFile(imgFile);
    return reader;
}

//------------------------------------------------------------------------------

bool SImageReader::loadImage(
    const std::filesystem::path& imgFile,
    std::shared_ptr<data::Image> img,
    const SPTR(JobCreatedSignalType)& sigJobCreated
)
{
    bool ok = true;

    std::string ext = imgFile.extension().string();
    boost::algorithm::to_lower(ext);

    sight::io::base::reader::IObjectReader::sptr imageReader;
    if(ext == ".vtk")
    {
        imageReader = configureReader<sight::io::vtk::ImageReader>(imgFile);
    }
    else if(ext == ".vti")
    {
        imageReader = configureReader<sight::io::vtk::VtiImageReader>(imgFile);
    }
    else if(ext == ".mhd")
    {
        imageReader = configureReader<sight::io::vtk::MetaImageReader>(imgFile);
    }
    else
    {
        /* Handle BitmapImageReader extensions */
        std::vector<std::string> availableExtensions;
        sight::io::vtk::BitmapImageReader::getAvailableExtensions(availableExtensions);

        /* If we find the current extensions in the available readers, we use it */
        size_t i = 0;
        for( ; i < availableExtensions.size() ; i++)
        {
            if(availableExtensions.at(i) == ext)
            {
                imageReader = configureReader<sight::io::vtk::BitmapImageReader>(imgFile);
                break;
            }
        }

        // If we didn't find any suitable extension with BitmapImageReader, we raise an exception */
        if(i == availableExtensions.size())
        {
            i = 0;
            std::string bitmapExtensions = "";
            for( ; i < availableExtensions.size() ; i++)
            {
                bitmapExtensions = bitmapExtensions + availableExtensions.at(i) + ", ";
            }

            SIGHT_THROW_EXCEPTION(
                core::tools::Failed(
                    "Only " + bitmapExtensions
                    + ".vtk, .vti and .mhd are supported."
                )
            );
        }
    }

    // Set the image (already created, but empty) that will be modified
    imageReader->setObject(img);

    sigJobCreated->emit(imageReader->getJob());

    try
    {
        imageReader->read();
    }
    catch(core::tools::Failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;

        // Raise exception  for superior level
        SIGHT_THROW_EXCEPTION(e);
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;
    }
    catch(...)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading.",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;
    }

    return ok;
}

} // namespace ioVtk
