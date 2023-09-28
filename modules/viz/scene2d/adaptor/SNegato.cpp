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

#include "modules/viz/scene2d/adaptor/SNegato.hpp"

#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/TransferFunction.hpp>

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

namespace medHelper = data::helper::MedicalImage;

//-----------------------------------------------------------------------------

SNegato::SNegato() noexcept
{
    new_slot(UPDATE_SLICE_INDEX_SLOT, &SNegato::updateSliceIndex, this);
    new_slot(UPDATE_SLICE_TYPE_SLOT, &SNegato::updateSliceType, this);
    new_slot(UPDATE_BUFFER_SLOT, &SNegato::updateBuffer, this);
    new_slot(UPDATE_VISIBILITY_SLOT, &SNegato::updateVisibility, this);
    new_slot(UPDATE_TF_SLOT, &SNegato::updateTF, this);
}

//-----------------------------------------------------------------------------

SNegato::~SNegato() noexcept =
    default;

//-----------------------------------------------------------------------------

void SNegato::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfiguration().get_child("config.<xmlattr>");

    if(config.count("orientation") != 0U)
    {
        const auto orientationValue = config.get<std::string>("orientation");

        if(orientationValue == "axial")
        {
            m_orientation = orientation_t::AXIAL;
        }
        else if(orientationValue == "sagittal")
        {
            m_orientation = orientation_t::SAGITTAL;
        }
        else if(orientationValue == "frontal")
        {
            m_orientation = orientation_t::FRONTAL;
        }
    }

    if(config.count("changeSliceType") != 0U)
    {
        const auto changeValue = config.get<std::string>("changeSliceType");

        if(changeValue == "true")
        {
            m_changeSliceTypeAllowed = true;
        }
        else if(changeValue == "false")
        {
            m_changeSliceTypeAllowed = false;
        }
    }
}

//-----------------------------------------------------------------------------

void SNegato::updateBufferFromImage(QImage* _img)
{
    if(_img == nullptr)
    {
        return;
    }

    // Window min/max
    const auto tf = m_tf.const_lock();

    // Window max
    auto image                     = m_image.lock();
    const data::Image::Size size   = image->size();
    const auto* imgBuff            = static_cast<const std::int16_t*>(image->buffer());
    const std::size_t imageZOffset = size[0] * size[1];

    std::uint8_t* pDest = _img->bits();

    // Fill image according to current slice type:
    if(m_orientation == orientation_t::SAGITTAL) // sagittal
    {
        const auto sagitalIndex = static_cast<std::size_t>(m_sagittalIndex);

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            const std::size_t zOffset  = (size[2] - 1 - z) * imageZOffset;
            const std::size_t zxOffset = zOffset + sagitalIndex;

            for(std::size_t y = 0 ; y < size[1] ; ++y)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::SNegato::getQImageVal(imgBuff[zxOffset + y * size[0]], *tf);

                *pDest++ = static_cast<std::uint8_t>(qRed(val));
                *pDest++ = static_cast<std::uint8_t>(qGreen(val));
                *pDest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if(m_orientation == orientation_t::FRONTAL) // frontal
    {
        const auto frontalIndex   = static_cast<std::size_t>(m_frontalIndex);
        const std::size_t yOffset = frontalIndex * size[0];

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            const std::size_t zOffset  = (size[2] - 1 - z) * imageZOffset;
            const std::size_t zyOffset = zOffset + yOffset;

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::SNegato::getQImageVal(imgBuff[zyOffset + x], *tf);

                *pDest++ = static_cast<std::uint8_t>(qRed(val));
                *pDest++ = static_cast<std::uint8_t>(qGreen(val));
                *pDest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if(m_orientation == orientation_t::AXIAL) // axial
    {
        const auto axialIndex     = static_cast<std::size_t>(m_axialIndex);
        const std::size_t zOffset = axialIndex * imageZOffset;

        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            const std::size_t yOffset  = y * size[0];
            const std::size_t zyOffset = zOffset + yOffset;

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const QRgb val =
                    sight::module::viz::scene2d::adaptor::SNegato::getQImageVal(imgBuff[zyOffset + x], *tf);

                *pDest++ = static_cast<std::uint8_t>(qRed(val));
                *pDest++ = static_cast<std::uint8_t>(qGreen(val));
                *pDest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }

    QPixmap m_pixmap = QPixmap::fromImage(*m_qImg);
    m_pixmapItem->setPixmap(m_pixmap);
}

//-----------------------------------------------------------------------------

QRgb SNegato::getQImageVal(const std::int16_t value, const data::TransferFunction& tf)
{
    const data::TransferFunction::color_t color = tf.sample(value);

    // use QImage::Format_RGBA8888 in QImage if you need alpha value
    return qRgb(static_cast<int>(color.r * 255), static_cast<int>(color.g * 255), static_cast<int>(color.b * 255));
}

//---------------------------------------------------------------------------

QImage* SNegato::createQImage()
{
    data::Image::Size size;
    data::Image::Spacing spacing;
    data::Image::Origin origin;

    {
        auto img = m_image.lock();

        if(!data::helper::MedicalImage::checkImageValidity(img.get_shared()))
        {
            return nullptr;
        }

        size    = img->size();
        spacing = img->getSpacing();
        origin  = img->getOrigin();
    }

    std::array<double, 2> qImageSpacing {};
    std::array<double, 2> qImageOrigin {};
    std::array<int, 2> qImageSize {};

    switch(m_orientation)
    {
        case orientation_t::X_AXIS: // sagittal
            this->m_yAxis->setScale(-1);
            qImageSize[0]    = static_cast<int>(size[1]);
            qImageSize[1]    = static_cast<int>(size[2]);
            qImageSpacing[0] = spacing[1];
            qImageSpacing[1] = spacing[2];
            qImageOrigin[0]  = origin[1] - 0.5F * spacing[1];
            qImageOrigin[1]  = -(origin[2] + static_cast<double>(size[2]) * spacing[2] - 0.5 * spacing[2]);
            break;

        case orientation_t::Y_AXIS: // frontal
            qImageSize[0]    = static_cast<int>(size[0]);
            qImageSize[1]    = static_cast<int>(size[2]);
            qImageSpacing[0] = spacing[0];
            qImageSpacing[1] = spacing[2];
            qImageOrigin[0]  = origin[0] - 0.5F * spacing[0];
            qImageOrigin[1]  = -(origin[2] + static_cast<double>(size[2]) * spacing[2] - 0.5 * spacing[2]);
            break;

        case orientation_t::Z_AXIS: // axial
            qImageSize[0]    = static_cast<int>(size[0]);
            qImageSize[1]    = static_cast<int>(size[1]);
            qImageSpacing[0] = spacing[0];
            qImageSpacing[1] = spacing[1];
            qImageOrigin[0]  = origin[0] - 0.5 * spacing[0];
            qImageOrigin[1]  = origin[1] - 0.5 * spacing[1];
            break;

        default:
            SIGHT_FATAL("Unsupported value for orientation");
            break;
    }

    // Create empty QImage
    auto* image = new QImage(qImageSize[0], qImageSize[1], QImage::Format_RGB888);

    // Place m_pixmapItem
    m_pixmapItem->resetTransform();
    m_pixmapItem->setTransform(QTransform::fromScale(qImageSpacing[0], qImageSpacing[1]), true);
    m_pixmapItem->setPos(qImageOrigin[0], qImageOrigin[1]);

    // Force bounding box recomputing ( Qt bug )
    m_layer->removeFromGroup(m_pixmapItem);
    m_layer->addToGroup(m_pixmapItem);

    // Update image scene
    this->getScene2DRender()->updateSceneSize(0.20F);

    return image;
}

//-----------------------------------------------------------------------------

void SNegato::starting()
{
    auto tf = m_tf.lock();

    auto image = m_image.lock();

    m_axialIndex    = std::max(0, int(medHelper::getSliceIndex(*image, medHelper::orientation_t::AXIAL).value_or(0)));
    m_frontalIndex  = std::max(0, int(medHelper::getSliceIndex(*image, medHelper::orientation_t::FRONTAL).value_or(0)));
    m_sagittalIndex =
        std::max(0, int(medHelper::getSliceIndex(*image, medHelper::orientation_t::SAGITTAL).value_or(0)));

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

void SNegato::updating()
{
    m_qImg = this->createQImage();
    this->updateBufferFromImage(m_qImg);
}

//-----------------------------------------------------------------------------

void SNegato::updateSliceIndex(int axial, int frontal, int sagittal)
{
    if(sagittal != m_sagittalIndex
       || frontal != m_frontalIndex
       || axial != m_axialIndex)
    {
        m_sagittalIndex = sagittal;
        m_frontalIndex  = frontal;
        m_axialIndex    = axial;
    }

    this->updateBufferFromImage(m_qImg);
}

//-----------------------------------------------------------------------------

void SNegato::updateSliceType(int from, int to)
{
    if(m_changeSliceTypeAllowed)
    {
        if(to == static_cast<int>(m_orientation))
        {
            m_orientation = static_cast<orientation_t>(from);
        }
        else if(from == static_cast<int>(m_orientation))
        {
            m_orientation = static_cast<orientation_t>(to);
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

void SNegato::updateVisibility(bool isVisible)
{
    if(isVisible) // display the scene
    {
        m_layer->setVisible(true);
    }
    else // remove the layer from the scene
    {
        m_layer->setVisible(false);
    }
}

//-----------------------------------------------------------------------------

void SNegato::updateBuffer()
{
    this->updateBufferFromImage(m_qImg);
}

//------------------------------------------------------------------------------

void SNegato::updateTF()
{
    this->updateBufferFromImage(m_qImg);
}

//-----------------------------------------------------------------------------

void SNegato::stopping()
{
    this->getScene2DRender()->getScene()->removeItem(m_layer);

    delete m_qImg;
    delete m_pixmapItem;
    delete m_layer;
}

//-----------------------------------------------------------------------------

void SNegato::processInteraction(sight::viz::scene2d::data::Event& _event)
{
    // if a key is pressed
    if(_event.getType() == sight::viz::scene2d::data::Event::KeyRelease)
    {
        // if pressed key is 'R'
        if(_event.getKey() == Qt::Key_R)
        {
            // get image origin
            QRectF recImage = m_pixmapItem->sceneBoundingRect();

            const auto sceneViewport = m_viewport.lock();

            double sceneWidth  = static_cast<double>(this->getScene2DRender()->getView()->width());
            double sceneHeight = static_cast<double>(this->getScene2DRender()->getView()->height());

            double ratioYonXimage = recImage.height() / recImage.width();
            double sceneRatio     = sceneHeight / sceneWidth;

            if(sceneRatio > ratioYonXimage) // used scene ratio
            {
                double widthViewPortNew  = recImage.width();
                double heightViewPortNew = widthViewPortNew * sceneRatio;

                // computes new y origin
                double newOrigineY = recImage.y() - (heightViewPortNew - recImage.height()) / 2.F;

                sceneViewport->setX(recImage.x());
                sceneViewport->setY(newOrigineY);
                sceneViewport->setWidth(widthViewPortNew);
                sceneViewport->setHeight(heightViewPortNew);
            }
            else
            {
                double heightViewPortNew = recImage.height();
                double widthViewPortNew  = heightViewPortNew / sceneRatio;

                // computes new x origin
                double newOrigineX = recImage.x() - (widthViewPortNew - recImage.width()) / 2.F;

                sceneViewport->setX(newOrigineX);
                sceneViewport->setY(recImage.y());
                sceneViewport->setWidth(widthViewPortNew);
                sceneViewport->setHeight(heightViewPortNew);
            }

            auto viewportObject = m_viewport.lock();
            this->getScene2DRender()->getView()->updateFromViewport(*viewportObject);
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
            sight::viz::scene2d::vec2d_t newCoord = _event.getCoord();
            this->changeImageMinMaxFromCoord(m_oldCoord, newCoord);
            m_oldCoord = newCoord;
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

void SNegato::changeImageMinMaxFromCoord(
    sight::viz::scene2d::vec2d_t& /*unused*/,
    sight::viz::scene2d::vec2d_t& newCoord
)
{
    const auto tf = m_tf.lock();

    const double min = tf->windowMinMax().first;
    const double max = tf->windowMinMax().second;

    const double window = newCoord.x - m_oldCoord.x;
    const double level  = newCoord.y - m_oldCoord.y;

    const double imgWindow = max - min;
    const double imgLevel  = min + imgWindow / 2.0;

    const double newImgLevel  = imgLevel + level;
    const double newImgWindow = imgWindow + imgWindow * window / 100.0;

    this->updating();

    // Send signal
    tf->setWindow(newImgWindow);
    tf->setLevel(newImgLevel);
    auto sig = tf->signal<data::TransferFunction::WindowingModifiedSignalType>(
        data::TransferFunction::WINDOWING_MODIFIED_SIG
    );
    {
        const core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_TF_SLOT)));
        sig->async_emit(newImgWindow, newImgLevel);
    }
}

//------------------------------------------------------------------------------

service::connections_t SNegato::getAutoConnections() const
{
    return {
        {s_IMAGE_IN, data::Image::MODIFIED_SIG, service::slots::UPDATE},
        {s_IMAGE_IN, data::Image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_INDEX_SLOT},
        {s_IMAGE_IN, data::Image::SLICE_INDEX_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT},
        {s_IMAGE_IN, data::Image::BUFFER_MODIFIED_SIG, UPDATE_BUFFER_SLOT},
        {s_TF_INOUT, data::TransferFunction::MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF_INOUT, data::TransferFunction::POINTS_MODIFIED_SIG, UPDATE_TF_SLOT},
        {s_TF_INOUT, data::TransferFunction::WINDOWING_MODIFIED_SIG, UPDATE_TF_SLOT}
    };
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
