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

#include "SImageReader.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/location/SingleFile.hpp>
#include <core/location/SingleFolder.hpp>

#include <data/Image.hpp>

#include <io/base/service/IReader.hpp>
#include <io/itk/InrImageReader.hpp>
#include <io/itk/NiftiImageReader.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

SImageReader::SImageReader() noexcept =
    default;

//------------------------------------------------------------------------------

SImageReader::~SImageReader() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SImageReader::getIOPathType() const
{
    return sight::io::base::service::FILE;
}

//------------------------------------------------------------------------------

void SImageReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an image" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialogFile.addFilter("Inr (.inr.gz)", "*.inr.gz");
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

//
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
        const auto data  = m_data.lock();
        const auto image = std::dynamic_pointer_cast<data::Image>(data.get_shared());
        SIGHT_ASSERT(
            "The object is not a '"
            + data::Image::classname()
            + "' or '"
            + sight::io::base::service::s_DATA_KEY
            + "' is not correctly set.",
            image
        );

        sight::ui::base::Cursor cursor;
        cursor.setCursor(ui::base::ICursor::BUSY);

        try
        {
            if(sight::module::io::itk::SImageReader::loadImage(this->getFile(), image))
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
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

bool SImageReader::loadImage(
    const std::filesystem::path& imgFile,
    const data::Image::sptr& img
)
{
    bool ok = true;

    std::string ext = imgFile.extension().string();
    boost::algorithm::to_lower(ext);

    sight::ui::base::dialog::ProgressDialog progressMeterGUI("Loading Image ");

    sight::io::base::reader::IObjectReader::sptr imageReader;
    if(boost::algorithm::ends_with(imgFile.string(), ".inr.gz"))
    {
        auto inrReader = sight::io::itk::InrImageReader::New();
        inrReader->setFile(imgFile);
        inrReader->addHandler(progressMeterGUI);
        imageReader = inrReader;
    }
    else if(ext == ".nii" || boost::algorithm::ends_with(imgFile.string(), ".nii.gz"))
    {
        auto niftiReader = sight::io::itk::NiftiImageReader::New();
        niftiReader->setFile(imgFile);
        niftiReader->addHandler(progressMeterGUI);
        imageReader = niftiReader;
    }
    else
    {
        std::stringstream ss;
        ss << "The file extension " << ext
        << " is not supported by the image reader. Please choose either *.inr.gz, *.nii or *.nii.gz files";
        sight::ui::base::dialog::MessageDialog::show(
            "Error",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::CRITICAL
        );
        return false;
    }

    imageReader->setObject(img);

    try
    {
        imageReader->read();
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
            "Warning during loading",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        ok = false;
    }
    return ok;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
