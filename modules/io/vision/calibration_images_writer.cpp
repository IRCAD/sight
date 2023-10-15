/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "calibration_images_writer.hpp"

#include <core/location/single_folder.hpp>

#include <data/calibration_info.hpp>
#include <data/image.hpp>

#include <io/opencv/image.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <opencv2/opencv.hpp>

#include <filesystem>
#include <sstream>

namespace sight::module::io::vision
{

//------------------------------------------------------------------------------

calibration_images_writer::calibration_images_writer() noexcept =
    default;

//------------------------------------------------------------------------------

calibration_images_writer::~calibration_images_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType calibration_images_writer::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void calibration_images_writer::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a folder to save the images" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::dialog::location::WRITE);
    dialogFile.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialogFile.show());

    if(result)
    {
        this->set_folder(result->get_folder());
        defaultDirectory->set_folder(result->get_folder().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void calibration_images_writer::configuring()
{
    sight::io::service::writer::configuring();

    const auto configTree = this->get_config();
    m_fileExtension = configTree.get("format", ".tiff");
}

//------------------------------------------------------------------------------

void calibration_images_writer::starting()
{
}

//------------------------------------------------------------------------------

void calibration_images_writer::updating()
{
    if(!m_fileExtension.empty() && this->hasLocationDefined())
    {
        const auto data      = m_data.lock();
        const auto calibInfo = std::dynamic_pointer_cast<const data::calibration_info>(data.get_shared());
        SIGHT_ASSERT("Missing calibration info input.", calibInfo);

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

        std::size_t count(0);
        for(const auto& calibImg : calibInfo->getImageContainer())
        {
            std::ostringstream imageNumber;
            imageNumber << std::setw(4) << std::setfill('0') << count++;

            const std::string filename       = "img_" + imageNumber.str() + m_fileExtension;
            const std::filesystem::path path = this->get_folder() / filename;

            cv::Mat cvImg = sight::io::opencv::image::copyToCv(calibImg);

            if(cvImg.dims == 3)
            {
                // Ensure that we have a true depth-less 2D image.
                cvImg = cvImg.reshape(0, 2, cvImg.size + 1);
            }

            try
            {
                if(cvImg.type() == CV_8UC3 || cvImg.type() == CV_8UC4)
                {
                    // convert the image from BGR to RGB
                    const auto colConvType = cvImg.type() == CV_8UC3 ? cv::COLOR_BGR2RGB : cv::COLOR_BGRA2RGBA;
                    cv::cvtColor(cvImg, cvImg, colConvType);
                }

                cv::imwrite(path.string(), cvImg);
            }
            catch(const cv::Exception& e)
            {
                m_writeFailed = true;
                sight::ui::dialog::message::show(
                    "Error writing calibration images.",
                    e.what(),
                    sight::ui::dialog::message::CRITICAL
                );
            }
        }

        cursor.setDefaultCursor();
    }
    else
    {
        m_writeFailed = true;
    }
}

//------------------------------------------------------------------------------

void calibration_images_writer::stopping()
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vision
