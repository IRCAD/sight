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

#include "modules/viz/scene2d/adaptor/negato.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <service/macros.hpp>

#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QBitmap>
#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QPoint>

namespace sight::module::viz::scene2d::adaptor
{

static const core::com::slots::key_t UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::slots::key_t UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";
static const core::com::slots::key_t UPDATE_BUFFER_SLOT      = "updateBuffer";
static const core::com::slots::key_t UPDATE_VISIBILITY_SLOT  = "updateVisibility";
static const core::com::slots::key_t UPDATE_TF_SLOT          = "updateTF";

namespace medHelper = data::helper::medical_image;

//-----------------------------------------------------------------------------

negato::negato() noexcept
{
    new_slot(UPDATE_SLICE_INDEX_SLOT, &negato::updateSliceIndex, this);
    new_slot(UPDATE_SLICE_TYPE_SLOT, &negato::updateSliceType, this);
    new_slot(UPDATE_BUFFER_SLOT, &negato::updateBuffer, this);
    new_slot(UPDATE_VISIBILITY_SLOT, &negato::updateVisibility, this);
    new_slot(UPDATE_TF_SLOT, &negato::updateTF, this);
}

//-----------------------------------------------------------------------------

negato::~negato() noexcept =
    default;

//-----------------------------------------------------------------------------

void negato::configuring()
{
    this->configureParams();

    const config_t config = this->get_config().get_child("config.<xmlattr>");

    if(config.count("orientation") != 0U)
    {
        const auto orientation_value = config.get<std::string>("orientation");

        if(orientation_value == "axial")
        {
            m_orientation = orientation_t::AXIAL;
        }
        else if(orientation_value == "sagittal")
        {
            m_orientation = orientation_t::SAGITTAL;
        }
        else if(orientation_value == "frontal")
        {
            m_orientation = orientation_t::FRONTAL;
        }
    }

    if(config.count("changeSliceType") != 0U)
    {
        const auto change_value = config.get<std::string>("changeSliceType");

        if(change_value == "true")
        {
            m_changeSliceTypeAllowed = true;
        }
        else if(change_value == "false")
        {
            m_changeSliceTypeAllowed = false;
        }
    }
}

//-----------------------------------------------------------------------------

void negato::updateBufferFromImage(QImage* _img)
{
    if(_img == nullptr)
    {
        return;
    }

    // Window min/max
    const auto tf = m_tf.const_lock();

    // Window max
    auto image                       = m_image.lock();
    const data::image::Size size     = image->size();
    const auto* img_buff             = static_cast<const std::int16_t*>(image->buffer());
    const std::size_t image_z_offset = size[0] * size[1];

    std::uint8_t* p_dest = _img->bits();

    // Fill image according to current slice type:
    if(m_orientation == orientation_t::SAGITTAL) // sagittal
    {
        const auto sagital_index = static_cast<std::size_t>(m_sagittalIndex);

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            const std::size_t z_offset  = (size[2] - 1 - z) * image_z_offset;
            const std::size_t zx_offset = z_offset + sagital_index;

            for(std::size_t y = 0 ; y < size[1] ; ++y)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::negato::getQImageVal(img_buff[zx_offset + y * size[0]], *tf);

                *p_dest++ = static_cast<std::uint8_t>(qRed(val));
                *p_dest++ = static_cast<std::uint8_t>(qGreen(val));
                *p_dest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if(m_orientation == orientation_t::FRONTAL) // frontal
    {
        const auto frontal_index   = static_cast<std::size_t>(m_frontalIndex);
        const std::size_t y_offset = frontal_index * size[0];

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            const std::size_t z_offset  = (size[2] - 1 - z) * image_z_offset;
            const std::size_t zy_offset = z_offset + y_offset;

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::negato::getQImageVal(img_buff[zy_offset + x], *tf);

                *p_dest++ = static_cast<std::uint8_t>(qRed(val));
                *p_dest++ = static_cast<std::uint8_t>(qGreen(val));
                *p_dest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if(m_orientation == orientation_t::AXIAL) // axial
    {
        const auto axial_index     = static_cast<std::size_t>(m_axialIndex);
        const std::size_t z_offset = axial_index * image_z_offset;

        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            const std::size_t y_offset  = y * size[0];
            const std::size_t zy_offset = z_offset + y_offset;

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::negato::getQImageVal(img_buff[zy_offset + x], *tf);

                *p_dest++ = static_cast<std::uint8_t>(qRed(val));
                *p_dest++ = static_cast<std::uint8_t>(qGreen(val));
                *p_dest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }

    QPixmap m_pixmap = QPixmap::fromImage(*m_qImg);
    m_pixmapItem->setPixmap(m_pixmap);
}

//-----------------------------------------------------------------------------

QRgb negato::getQImageVal(const std::int16_t _value, const data::transfer_function& _tf)
{
    const data::transfer_function::color_t color = _tf.sample(_value);

    // use QImage::Format_RGBA8888 in QImage if you need alpha value
    return qRgb(static_cast<int>(color.r * 255), static_cast<int>(color.g * 255), static_cast<int>(color.b * 255));
}

//---------------------------------------------------------------------------

QImage* negato::createQImage()
{
    data::image::Size size;
    data::image::Spacing spacing;
    data::image::Origin origin;

    {
        auto img = m_image.lock();

        if(!data::helper::medical_image::check_image_validity(img.get_shared()))
        {
            return nullptr;
        }

        size    = img->size();
        spacing = img->getSpacing();
        origin  = img->getOrigin();
    }

    std::array<double, 2> q_image_spacing {};
    std::array<double, 2> q_image_origin {};
    std::array<int, 2> q_image_size {};

    switch(m_orientation)
    {
        case orientation_t::X_AXIS: // sagittal
            this->m_yAxis->setScale(-1);
            q_image_size[0]    = static_cast<int>(size[1]);
            q_image_size[1]    = static_cast<int>(size[2]);
            q_image_spacing[0] = spacing[1];
            q_image_spacing[1] = spacing[2];
            q_image_origin[0]  = origin[1] - 0.5F * spacing[1];
            q_image_origin[1]  = -(origin[2] + static_cast<double>(size[2]) * spacing[2] - 0.5 * spacing[2]);
            break;

        case orientation_t::Y_AXIS: // frontal
            q_image_size[0]    = static_cast<int>(size[0]);
            q_image_size[1]    = static_cast<int>(size[2]);
            q_image_spacing[0] = spacing[0];
            q_image_spacing[1] = spacing[2];
            q_image_origin[0]  = origin[0] - 0.5F * spacing[0];
            q_image_origin[1]  = -(origin[2] + static_cast<double>(size[2]) * spacing[2] - 0.5 * spacing[2]);
            break;

        case orientation_t::Z_AXIS: // axial
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
    m_pixmapItem->resetTransform();
    m_pixmapItem->setTransform(QTransform::fromScale(q_image_spacing[0], q_image_spacing[1]), true);
    m_pixmapItem->setPos(q_image_origin[0], q_image_origin[1]);

    // Force bounding box recomputing ( Qt bug )
    m_layer->removeFromGroup(m_pixmapItem);
    m_layer->addToGroup(m_pixmapItem);

    // Update image scene
    this->getScene2DRender()->updateSceneSize(0.20F);

    return image;
}

//-----------------------------------------------------------------------------

void negato::starting()
{
    auto tf = m_tf.lock();

    auto image = m_image.lock();

    m_axialIndex   = std::max(0, int(medHelper::get_slice_index(*image, medHelper::orientation_t::AXIAL).value_or(0)));
    m_frontalIndex =
        std::max(0, int(medHelper::get_slice_index(*image, medHelper::orientation_t::FRONTAL).value_or(0)));
    m_sagittalIndex =
        std::max(0, int(medHelper::get_slice_index(*image, medHelper::orientation_t::SAGITTAL).value_or(0)));

    m_pixmapItem = new QGraphicsPixmapItem();
    m_pixmapItem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    m_layer = new QGraphicsItemGroup();
    m_layer->resetTransform();
    m_layer->addToGroup(m_pixmapItem);
    m_layer->setPos(m_xAxis->getOrigin(), m_yAxis->getOrigin());
    m_layer->setZValue(m_zValue);
    this->getScene2DRender()->getScene()->addItem(m_layer);

    m_qImg = this->createQImage();
    this->updateBufferFromImage(m_qImg);

    this->getScene2DRender()->updateSceneSize(1.F);
}

//-----------------------------------------------------------------------------

void negato::updating()
{
    m_qImg = this->createQImage();
    this->updateBufferFromImage(m_qImg);
}

//-----------------------------------------------------------------------------

void negato::updateSliceIndex(int _axial, int _frontal, int _sagittal)
{
    if(_sagittal != m_sagittalIndex
       || _frontal != m_frontalIndex
       || _axial != m_axialIndex)
    {
        m_sagittalIndex = _sagittal;
        m_frontalIndex  = _frontal;
        m_axialIndex    = _axial;
    }

    this->updateBufferFromImage(m_qImg);
}

//-----------------------------------------------------------------------------

void negato::updateSliceType(int _from, int _to)
{
    if(m_changeSliceTypeAllowed)
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
        if(m_orientation == orientation_t::Z_AXIS)
        {
            this->m_yAxis->setScale(1);
        }
        else
        {
            this->m_yAxis->setScale(-1);
        }

        this->updating();
    }
}

//-----------------------------------------------------------------------------

void negato::updateVisibility(bool _is_visible)
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

void negato::updateBuffer()
{
    this->updateBufferFromImage(m_qImg);
}

//------------------------------------------------------------------------------

void negato::updateTF()
{
    this->updateBufferFromImage(m_qImg);
}

//-----------------------------------------------------------------------------

void negato::stopping()
{
    this->getScene2DRender()->getScene()->removeItem(m_layer);

    delete m_qImg;
    delete m_pixmapItem;
    delete m_layer;
}

//-----------------------------------------------------------------------------

void negato::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    // if a key is pressed
    if(_event.getType() == sight::viz::scene2d::data::Event::KeyRelease)
    {
        // if pressed key is 'R'
        if(_event.getKey() == Qt::Key_R)
        {
            // get image origin
            QRectF rec_image = m_pixmapItem->sceneBoundingRect();

            const auto scene_viewport = m_viewport.lock();

            double scene_width  = static_cast<double>(this->getScene2DRender()->getView()->width());
            double scene_height = static_cast<double>(this->getScene2DRender()->getView()->height());

            double ratio_yon_ximage = rec_image.height() / rec_image.width();
            double scene_ratio      = scene_height / scene_width;

            if(scene_ratio > ratio_yon_ximage) // used scene ratio
            {
                double width_view_port_new  = rec_image.width();
                double height_view_port_new = width_view_port_new * scene_ratio;

                // computes new y origin
                double new_origine_y = rec_image.y() - (height_view_port_new - rec_image.height()) / 2.F;

                scene_viewport->setX(rec_image.x());
                scene_viewport->setY(new_origine_y);
                scene_viewport->setWidth(width_view_port_new);
                scene_viewport->setHeight(height_view_port_new);
            }
            else
            {
                double height_view_port_new = rec_image.height();
                double width_view_port_new  = height_view_port_new / scene_ratio;

                // computes new x origin
                double new_origine_x = rec_image.x() - (width_view_port_new - rec_image.width()) / 2.F;

                scene_viewport->setX(new_origine_x);
                scene_viewport->setY(rec_image.y());
                scene_viewport->setWidth(width_view_port_new);
                scene_viewport->setHeight(height_view_port_new);
            }

            auto viewport_object = m_viewport.lock();
            this->getScene2DRender()->getView()->updateFromViewport(*viewport_object);
        }

        //image pixel
        if(_event.getKey() == Qt::Key_F)
        {
            m_pixmapItem->setTransformationMode(Qt::FastTransformation);
            this->updating();
        }

        //image smooth
        if(_event.getKey() == Qt::Key_S)
        {
            m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
            this->updating();
        }
    }

    sight::viz::scene2d::vec2d_t coord = this->getScene2DRender()->mapToScene(_event.getCoord());
    coord.x = coord.x / m_layer->scale();
    coord.y = coord.y / m_layer->scale();

    if(_event.getType() == sight::viz::scene2d::data::Event::MouseButtonPress
       && _event.getButton() == sight::viz::scene2d::data::Event::RightButton
       && _event.getModifier() == sight::viz::scene2d::data::Event::NoModifier)
    {
        m_pointIsCaptured = true;
        m_oldCoord        = _event.getCoord();
        _event.setAccepted(true);
    }
    else if(m_pointIsCaptured)
    {
        if(_event.getType() == sight::viz::scene2d::data::Event::MouseMove)
        {
            sight::viz::scene2d::vec2d_t new_coord = _event.getCoord();
            this->changeImageMinMaxFromCoord(m_oldCoord, new_coord);
            m_oldCoord = new_coord;
            _event.setAccepted(true);
        }
        else if(_event.getButton() == sight::viz::scene2d::data::Event::RightButton
                && _event.getType() == sight::viz::scene2d::data::Event::MouseButtonRelease)
        {
            m_pointIsCaptured = false;
            _event.setAccepted(true);
        }
    }
}

//-----------------------------------------------------------------------------

void negato::changeImageMinMaxFromCoord(
    sight::viz::scene2d::vec2d_t& /*unused*/,
    sight::viz::scene2d::vec2d_t& _new_coord
)
{
    const auto tf = m_tf.lock();

    const double min = tf->windowMinMax().first;
    const double max = tf->windowMinMax().second;

    const double window = _new_coord.x - m_oldCoord.x;
    const double level  = _new_coord.y - m_oldCoord.y;

    const double img_window = max - min;
    const double img_level  = min + img_window / 2.0;

    const double new_img_level  = img_level + level;
    const double new_img_window = img_window + img_window * window / 100.0;

    this->updating();

    // Send signal
    tf->setWindow(new_img_window);
    tf->setLevel(new_img_level);
    auto sig = tf->signal<data::transfer_function::windowing_modified_signal_t>(
        data::transfer_function::WINDOWING_MODIFIED_SIG
    );
    {
        const core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_TF_SLOT)));
        sig->async_emit(new_img_window, new_img_level);
    }
}

//------------------------------------------------------------------------------

service::connections_t negato::auto_connections() const
{
    return {
        {s_IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_IN, data::image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_INDEX_SLOT},
        {s_IMAGE_IN, data::image::SLICE_INDEX_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT},
        {s_IMAGE_IN, data::image::BUFFER_MODIFIED_SIG, UPDATE_BUFFER_SLOT},
        {s_TF_INOUT, data::transfer_function::MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF_INOUT, data::transfer_function::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF_INOUT, data::transfer_function::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT}
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
