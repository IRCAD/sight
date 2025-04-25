/************************************************************************
 *
 * Copyright (C) 2016-2025 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "pdf_writer.hpp"

#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>
#include <core/thread/worker.hpp>

#include <data/iterator/types.hpp>

#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <QPainter>
#include <QPixmap>

namespace sight::module::io::document
{

pdf_writer::pdf_writer() noexcept :
    writer("Choose an external data file")
{
}

//-----------------------------------------------------------------------------

void pdf_writer::info(std::ostream& _sstream)
{
    this->writer::info(_sstream);
    _sstream << std::endl << " External data file reader";
}

//------------------------------------------------------------------------------

void pdf_writer::configuring()
{
    this->writer::configuring();

    const auto& config = this->get_config();

    for(const auto& container : boost::make_iterator_range(config.equal_range("container")))
    {
        const auto uid = container.second.get<std::string>("<xmlattr>.uid");
        m_containers_i_ds.push_back(uid);
    }
}

//------------------------------------------------------------------------------

void pdf_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(*m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("pdf", "*.pdf");

    dialog_file.set_option(ui::dialog::location::write);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void pdf_writer::updating()
{
    if(!this->has_location_defined())
    {
        open_location_dialog();
    }

    if(this->has_location_defined())
    {
        QPdfWriter pdf_writer(this->get_locations().front().string().c_str());
        QPainter painter(&pdf_writer);
        pdf_writer.setPageSize(QPageSize(QPageSize::A4));

        // Scale value to fit the images to a PDF page
        const int scale = pdf_writer.logicalDpiX() * 8;

        // Adding fwImage from generic scene to the list of images to scale
        images_scaled_list_t images_to_scale;
        std::vector<std::shared_future<QImage> > futures_q_image;

        {
            // Keep locked_ptr until tasks are done
            std::vector<data::mt::locked_ptr<const data::image> > locked_images;

            for(const auto& image_ptr : m_images)
            {
                std::shared_future<QImage> future;
                auto locked_image = image_ptr.second->lock();
                auto fn           = [&]{return pdf_writer::convert_fw_image_to_q_image(*locked_image);};
                locked_images.push_back(std::move(locked_image));
                futures_q_image.emplace_back(std::async(std::launch::async, fn));
            }

            std::ranges::for_each(futures_q_image, std::mem_fn(&std::shared_future<QImage>::wait));
        }
        for(auto& future : futures_q_image)
        {
            QImage image_to_draw = future.get();
            images_to_scale.push_back(image_to_draw);
        }

        // Adding QImage from Qt containers to the list of images to scale
        for(QWidget*& qt_container : m_containers_to_export)
        {
            QImage image_to_draw = qt_container->grab().toImage();
            images_to_scale.push_back(image_to_draw);
        }

        // Scales images to fit the A4 format
        std::vector<std::shared_future<void> > futures;
        const std::size_t size_images_to_scale = images_to_scale.size();
        for(std::size_t idx = 0 ; idx < size_images_to_scale ; ++idx)
        {
            auto fn = [&]{images_to_scale[idx] = images_to_scale[idx].scaledToWidth(scale, Qt::FastTransformation);};
            futures.emplace_back(std::async(std::launch::async, fn));
        }

        std::ranges::for_each(futures, std::mem_fn(&std::shared_future<void>::wait));

        // Draws images onto the PDF.
        for(QImage& q_image : images_to_scale)
        {
            if(pdf_writer.newPage())
            {
                pdf_writer.setPageSize(QPageSize(QPageSize::A4));
                if(!q_image.isNull() && q_image.bits() != nullptr)
                {
                    painter.drawImage(0, 0, q_image);
                }
            }
        }

        painter.end();
    }
}

//------------------------------------------------------------------------------

void pdf_writer::starting()
{
    for(const auto& id : m_containers_i_ds)
    {
        ui::qt::container::widget::sptr container_elt;
        sight::ui::container::widget::sptr fw_container_from_config;
        if(sight::ui::registry::has_sid_container(id))
        {
            fw_container_from_config = sight::ui::registry::get_sid_container(id);
        }
        else
        {
            fw_container_from_config = sight::ui::registry::get_wid_container(id);
        }

        if(fw_container_from_config)
        {
            container_elt = std::dynamic_pointer_cast<ui::qt::container::widget>(fw_container_from_config);
            m_containers_to_export.push_back(container_elt->get_qt_container());
        }
    }
}

//------------------------------------------------------------------------------

void pdf_writer::stopping()
{
    for(QWidget*& qt_container : m_containers_to_export)
    {
        qt_container = nullptr;
    }

    m_containers_to_export.clear();
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t pdf_writer::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

QImage pdf_writer::convert_fw_image_to_q_image(const data::image& _fw_image)
{
    if(_fw_image.num_components() == 3
       && _fw_image.type().name() == "uint8"
       && _fw_image.size()[2] == 1)
    {
        // Initialize QImage parameters
        const data::image::size_t dimension = _fw_image.size();
        const int width                     = static_cast<int>(dimension[0]);
        const int height                    = static_cast<int>(dimension[1]);

        QImage q_image(width, height, QImage::Format_ARGB32);
        std::uint8_t* q_image_buffer = q_image.bits();

        const auto dump_lock = _fw_image.dump_lock();

        auto image_itr = _fw_image.begin<data::iterator::rgb>();

        const unsigned int size = static_cast<unsigned int>(width * height) * 4;
        for(unsigned int idx = 0 ; idx < size ; idx += 4, ++image_itr)
        {
            q_image_buffer[idx + 3] = (255 & 0xFF);
            q_image_buffer[idx + 2] = (image_itr->r & 0xFF);
            q_image_buffer[idx + 1] = (image_itr->g & 0xFF);
            q_image_buffer[idx + 0] = (image_itr->b & 0xFF);
        }

        return q_image.mirrored(false, true);
    }

    return {};
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::document
