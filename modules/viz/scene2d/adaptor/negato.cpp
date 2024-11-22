/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "modules/viz/scene2d/adaptor/negato.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <service/macros.hpp>

#include <viz/scene2d/graphics_view.hpp>

#include <QBitmap>
#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QPoint>

namespace sight::module::viz::scene2d::adaptor
{

static const core::com::slots::key_t UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::slots::key_t UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";
static const core::com::slots::key_t UPDATE_BUFFER_SLOT      = "updateBuffer";
static const core::com::slots::key_t UPDATE_VISIBILITY_SLOT  = "update_visibility";
static const core::com::slots::key_t UPDATE_TF_SLOT          = "update_tf";

namespace medHelper = data::helper::medical_image;

//-----------------------------------------------------------------------------

negato::negato() noexcept
{
    new_slot(UPDATE_SLICE_INDEX_SLOT, &negato::update_slice_index, this);
    new_slot(UPDATE_SLICE_TYPE_SLOT, &negato::update_slice_type, this);
    new_slot(UPDATE_BUFFER_SLOT, &negato::update_buffer, this);
    new_slot(UPDATE_VISIBILITY_SLOT, &negato::update_visibility, this);
    new_slot(UPDATE_TF_SLOT, &negato::update_tf, this);
}

//-----------------------------------------------------------------------------

negato::~negato() noexcept =
    default;

//-----------------------------------------------------------------------------

void negato::configuring()
{
    this->configure_params();

    const config_t config = this->get_config().get_child("config.<xmlattr>");

    if(config.count("orientation") != 0U)
    {
        const auto orientation_value = config.get<std::string>("orientation");

        if(orientation_value == "axial")
        {
            m_orientation = orientation_t::axial;
        }
        else if(orientation_value == "sagittal")
        {
            m_orientation = orientation_t::sagittal;
        }
        else if(orientation_value == "frontal")
        {
            m_orientation = orientation_t::frontal;
        }
    }

    if(config.count("changeSliceType") != 0U)
    {
        const auto change_value = config.get<std::string>("changeSliceType");

        if(change_value == "true")
        {
            m_change_slice_type_allowed = true;
        }
        else if(change_value == "false")
        {
            m_change_slice_type_allowed = false;
        }
    }
}

//-----------------------------------------------------------------------------

void negato::update_buffer_from_image(QImage* _img)
{
    if(_img == nullptr)
    {
        return;
    }

    // Window min/max
    const auto tf = m_tf.const_lock();

    // Window max
    auto image                       = m_image.lock();
    const data::image::size_t size   = image->size();
    const auto* img_buff             = static_cast<const std::int16_t*>(image->buffer());
    const std::size_t image_z_offset = size[0] * size[1];

    std::uint8_t* p_dest = _img->bits();

    // Fill image according to current slice type:
    if(m_orientation == orientation_t::sagittal) // sagittal
    {
        const auto sagital_index = static_cast<std::size_t>(m_sagittal_index);

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            const std::size_t z_offset  = (size[2] - 1 - z) * image_z_offset;
            const std::size_t zx_offset = z_offset + sagital_index;

            for(std::size_t y = 0 ; y < size[1] ; ++y)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::negato::get_q_image_val(
                        img_buff[zx_offset + y * size[0]],
                        *tf
                    );

                *p_dest++ = static_cast<std::uint8_t>(qRed(val));
                *p_dest++ = static_cast<std::uint8_t>(qGreen(val));
                *p_dest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if(m_orientation == orientation_t::frontal) // frontal
    {
        const auto frontal_index   = static_cast<std::size_t>(m_frontal_index);
        const std::size_t y_offset = frontal_index * size[0];

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            const std::size_t z_offset  = (size[2] - 1 - z) * image_z_offset;
            const std::size_t zy_offset = z_offset + y_offset;

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::negato::get_q_image_val(img_buff[zy_offset + x], *tf);

                *p_dest++ = static_cast<std::uint8_t>(qRed(val));
                *p_dest++ = static_cast<std::uint8_t>(qGreen(val));
                *p_dest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if(m_orientation == orientation_t::axial) // axial
    {
        const auto axial_index     = static_cast<std::size_t>(m_axial_index);
        const std::size_t z_offset = axial_index * image_z_offset;

        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            const std::size_t y_offset  = y * size[0];
            const std::size_t zy_offset = z_offset + y_offset;

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::negato::get_q_image_val(img_buff[zy_offset + x], *tf);

                *p_dest++ = static_cast<std::uint8_t>(qRed(val));
                *p_dest++ = static_cast<std::uint8_t>(qGreen(val));
                *p_dest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }

    QPixmap m_pixmap = QPixmap::fromImage(*m_q_img);
    m_pixmap_item->setPixmap(m_pixmap);
}

//-----------------------------------------------------------------------------

QRgb negato::get_q_image_val(const std::int16_t _value, const data::transfer_function& _tf)
{
    const data::transfer_function::color_t color = _tf.sample(_value);

    // use QImage::Format_RGBA8888 in QImage if you need alpha value
    return qRgb(static_cast<int>(color.r * 255), static_cast<int>(color.g * 255), static_cast<int>(color.b * 255));
}

//---------------------------------------------------------------------------

QImage* negato::create_q_image()
{
    data::image::size_t size;
    data::image::spacing_t spacing;
    data::image::origin_t origin;

    {
        auto img = m_image.lock();

        if(!data::helper::medical_image::check_image_validity(img.get_shared()))
        {
            return nullptr;
        }

        size    = img->size();
        spacing = img->spacing();
        origin  = img->origin();
    }

    std::array<double, 2> q_image_spacing {};
    std::array<double, 2> q_image_origin {};
    std::array<int, 2> q_image_size {};

    switch(m_orientation)
    {
        case orientation_t::x_axis: // sagittal
            this->m_y_axis->set_scale(-1);
            q_image_size[0]    = static_cast<int>(size[1]);
            q_image_size[1]    = static_cast<int>(size[2]);
            q_image_spacing[0] = spacing[1];
            q_image_spacing[1] = spacing[2];
            q_image_origin[0]  = origin[1] - 0.5F * spacing[1];
            q_image_origin[1]  = -(origin[2] + static_cast<double>(size[2]) * spacing[2] - 0.5 * spacing[2]);
            break;

        case orientation_t::y_axis: // frontal
            q_image_size[0]    = static_cast<int>(size[0]);
            q_image_size[1]    = static_cast<int>(size[2]);
            q_image_spacing[0] = spacing[0];
            q_image_spacing[1] = spacing[2];
            q_image_origin[0]  = origin[0] - 0.5F * spacing[0];
            q_image_origin[1]  = -(origin[2] + static_cast<double>(size[2]) * spacing[2] - 0.5 * spacing[2]);
            break;

        case orientation_t::z_axis: // axial
            q_image_size[0]    = static_cast<int>(size[0]);
            q_image_size[1]    = static_cast<int>(size[1]);
            q_image_spacing[0] = spacing[0];
            q_image_spacing[1] = spacing[1];
            q_image_origin[0]  = origin[0] - 0.5 * spacing[0];
            q_image_origin[1]  = origin[1] - 0.5 * spacing[1];
            break;

        default:
            SIGHT_FATAL("Unsupported value for orientation");
            break;
    }

    // Create empty QImage
    auto* image = new QImage(q_image_size[0], q_image_size[1], QImage::Format_RGB888);

    // Place m_pixmapItem
    m_pixmap_item->resetTransform();
    m_pixmap_item->setTransform(QTransform::fromScale(q_image_spacing[0], q_image_spacing[1]), true);
    m_pixmap_item->setPos(q_image_origin[0], q_image_origin[1]);

    // Force bounding box recomputing ( Qt bug )
    m_layer->removeFromGroup(m_pixmap_item);
    m_layer->addToGroup(m_pixmap_item);

    // Update image scene
    this->get_scene_2d_render()->update_scene_size(0.20F);

    return image;
}

//-----------------------------------------------------------------------------

void negato::starting()
{
    auto tf = m_tf.lock();

    auto image = m_image.lock();

    m_axial_index   = std::max(0, int(medHelper::get_slice_index(*image, medHelper::orientation_t::axial).value_or(0)));
    m_frontal_index =
        std::max(0, int(medHelper::get_slice_index(*image, medHelper::orientation_t::frontal).value_or(0)));
    m_sagittal_index =
        std::max(0, int(medHelper::get_slice_index(*image, medHelper::orientation_t::sagittal).value_or(0)));

    m_pixmap_item = new QGraphicsPixmapItem();
    m_pixmap_item->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    m_pixmap_item->setTransformationMode(Qt::SmoothTransformation);
    m_layer = new QGraphicsItemGroup();
    m_layer->resetTransform();
    m_layer->addToGroup(m_pixmap_item);
    m_layer->setPos(m_x_axis->origin(), m_y_axis->origin());
    m_layer->setZValue(m_z_value);
    this->get_scene_2d_render()->get_scene()->addItem(m_layer);

    m_q_img = this->create_q_image();
    this->update_buffer_from_image(m_q_img);

    this->get_scene_2d_render()->update_scene_size(1.F);
}

//-----------------------------------------------------------------------------

void negato::updating()
{
    m_q_img = this->create_q_image();
    this->update_buffer_from_image(m_q_img);
}

//-----------------------------------------------------------------------------

void negato::update_slice_index(int _axial, int _frontal, int _sagittal)
{
    if(_sagittal != m_sagittal_index
       || _frontal != m_frontal_index
       || _axial != m_axial_index)
    {
        m_sagittal_index = _sagittal;
        m_frontal_index  = _frontal;
        m_axial_index    = _axial;
    }

    this->update_buffer_from_image(m_q_img);
}

//-----------------------------------------------------------------------------

void negato::update_slice_type(int _from, int _to)
{
    if(m_change_slice_type_allowed)
    {
        if(_to == static_cast<int>(m_orientation))
        {
            m_orientation = static_cast<orientation_t>(_from);
        }
        else if(_from == static_cast<int>(m_orientation))
        {
            m_orientation = static_cast<orientation_t>(_to);
        }

        // manages the modification of axes
        if(m_orientation == orientation_t::z_axis)
        {
            this->m_y_axis->set_scale(1);
        }
        else
        {
            this->m_y_axis->set_scale(-1);
        }

        this->updating();
    }
}

//-----------------------------------------------------------------------------

void negato::update_visibility(bool _is_visible)
{
    if(_is_visible) // display the scene
    {
        m_layer->setVisible(true);
    }
    else // remove the layer from the scene
    {
        m_layer->setVisible(false);
    }
}

//-----------------------------------------------------------------------------

void negato::update_buffer()
{
    this->update_buffer_from_image(m_q_img);
}

//------------------------------------------------------------------------------

void negato::update_tf()
{
    this->update_buffer_from_image(m_q_img);
}

//-----------------------------------------------------------------------------

void negato::stopping()
{
    this->get_scene_2d_render()->get_scene()->removeItem(m_layer);

    delete m_q_img;
    delete m_pixmap_item;
    delete m_layer;
}

//-----------------------------------------------------------------------------

void negato::process_interaction(sight::viz::scene2d::data::event& _event)
{
    // if a key is pressed
    if(_event.type() == sight::viz::scene2d::data::event::key_release)
    {
        // if pressed key is 'R'
        if(_event.get_key() == Qt::Key_R)
        {
            // get image origin
            QRectF rec_image = m_pixmap_item->sceneBoundingRect();

            const auto scene_viewport = m_viewport.lock();

            double scene_width  = static_cast<double>(this->get_scene_2d_render()->get_view()->width());
            double scene_height = static_cast<double>(this->get_scene_2d_render()->get_view()->height());

            double ratio_y_on_x_image = rec_image.height() / rec_image.width();
            double scene_ratio        = scene_height / scene_width;

            if(scene_ratio > ratio_y_on_x_image) // used scene ratio
            {
                double width_view_port_new  = rec_image.width();
                double height_view_port_new = width_view_port_new * scene_ratio;

                // computes new y origin
                double new_origine_y = rec_image.y() - (height_view_port_new - rec_image.height()) / 2.F;

                scene_viewport->set_x(rec_image.x());
                scene_viewport->set_y(new_origine_y);
                scene_viewport->set_width(width_view_port_new);
                scene_viewport->set_height(height_view_port_new);
            }
            else
            {
                double height_view_port_new = rec_image.height();
                double width_view_port_new  = height_view_port_new / scene_ratio;

                // computes new x origin
                double new_origine_x = rec_image.x() - (width_view_port_new - rec_image.width()) / 2.F;

                scene_viewport->set_x(new_origine_x);
                scene_viewport->set_y(rec_image.y());
                scene_viewport->set_width(width_view_port_new);
                scene_viewport->set_height(height_view_port_new);
            }

            auto viewport_object = m_viewport.lock();
            this->get_scene_2d_render()->get_view()->update_from_viewport(*viewport_object);
        }

        //image pixel
        if(_event.get_key() == Qt::Key_F)
        {
            m_pixmap_item->setTransformationMode(Qt::FastTransformation);
            this->updating();
        }

        //image smooth
        if(_event.get_key() == Qt::Key_S)
        {
            m_pixmap_item->setTransformationMode(Qt::SmoothTransformation);
            this->updating();
        }
    }

    sight::viz::scene2d::vec2d_t coord = this->get_scene_2d_render()->map_to_scene(_event.get_coord());
    coord.x = coord.x / m_layer->scale();
    coord.y = coord.y / m_layer->scale();

    if(_event.type() == sight::viz::scene2d::data::event::mouse_button_press
       && _event.get_button() == sight::viz::scene2d::data::event::right_button
       && _event.get_modifier() == sight::viz::scene2d::data::event::no_modifier)
    {
        m_point_is_captured = true;
        m_old_coord         = _event.get_coord();
        _event.set_accepted(true);
    }
    else if(m_point_is_captured)
    {
        if(_event.type() == sight::viz::scene2d::data::event::mouse_move)
        {
            sight::viz::scene2d::vec2d_t new_coord = _event.get_coord();
            this->change_image_min_max_from_coord(m_old_coord, new_coord);
            m_old_coord = new_coord;
            _event.set_accepted(true);
        }
        else if(_event.get_button() == sight::viz::scene2d::data::event::right_button
                && _event.type() == sight::viz::scene2d::data::event::mouse_button_release)
        {
            m_point_is_captured = false;
            _event.set_accepted(true);
        }
    }
}

//-----------------------------------------------------------------------------

void negato::change_image_min_max_from_coord(
    sight::viz::scene2d::vec2d_t& /*unused*/,
    sight::viz::scene2d::vec2d_t& _new_coord
)
{
    const auto tf = m_tf.lock();

    const double min = tf->window_min_max().first;
    const double max = tf->window_min_max().second;

    const double window = _new_coord.x - m_old_coord.x;
    const double level  = _new_coord.y - m_old_coord.y;

    const double img_window = max - min;
    const double img_level  = min + img_window / 2.0;

    const double new_img_level  = img_level + level;
    const double new_img_window = img_window + img_window * window / 100.0;

    this->updating();

    // Send signal
    tf->set_window(new_img_window);
    tf->set_level(new_img_level);
    tf->async_emit(this, data::transfer_function::WINDOWING_MODIFIED_SIG, new_img_window, new_img_level);
}

//------------------------------------------------------------------------------

service::connections_t negato::auto_connections() const
{
    return {
        {IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_INDEX_SLOT},
        {IMAGE_IN, data::image::SLICE_INDEX_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT},
        {IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, UPDATE_BUFFER_SLOT},
        {TF_INOUT, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT},
        {TF_INOUT, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {TF_INOUT, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT}
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
