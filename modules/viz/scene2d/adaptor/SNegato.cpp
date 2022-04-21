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

#include "modules/viz/scene2d/adaptor/SNegato.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/TransferFunction.hpp>

#include <service/macros.hpp>

#include <viz/scene2d/Scene2DGraphicsView.hpp>

#include <QBitmap>
#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QPoint>

namespace sight::module::viz::scene2d
{

namespace adaptor
{

static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";
static const core::com::Slots::SlotKeyType s_UPDATE_BUFFER_SLOT      = "updateBuffer";
static const core::com::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT  = "updateVisibility";

namespace medHelper = data::helper::MedicalImage;

//-----------------------------------------------------------------------------

SNegato::SNegato() noexcept :
    m_qImg(nullptr),
    m_pixmapItem(nullptr),
    m_layer(nullptr),
    m_pointIsCaptured(false),
    m_changeSliceTypeAllowed(true),
    m_helperTF(std::bind(&SNegato::updateTF, this))
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SNegato::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SNegato::updateSliceType, this);
    newSlot(s_UPDATE_BUFFER_SLOT, &SNegato::updateBuffer, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SNegato::updateVisibility, this);

    m_orientation = orientation_t::Z_AXIS;
}

//-----------------------------------------------------------------------------

SNegato::~SNegato() noexcept
{
}

//-----------------------------------------------------------------------------

void SNegato::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    if(config.count("orientation"))
    {
        const std::string orientationValue = config.get<std::string>("orientation");

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

    if(config.count("changeSliceType"))
    {
        const std::string changeValue = config.get<std::string>("changeSliceType");

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
    if(!_img)
    {
        return;
    }

    // Window min/max
    const data::TransferFunction::csptr tf = m_helperTF.getTransferFunction();
    const data::mt::locked_ptr tfLock(tf);
    const double wlMin = tf->getWLMinMax().first;

    // Window max
    auto image                     = m_image.lock();
    const data::Image::Size size   = image->getSize();
    const short* imgBuff           = static_cast<const short*>(image->getBuffer());
    const std::size_t imageZOffset = size[0] * size[1];

    const double tfMin = tf->getMinMaxTFValues().first;
    const double tfMax = tf->getMinMaxTFValues().second;
    const double tfWin = (1. / tf->getWindow()) * ((tfMax - tfMin) + tfMin);

    std::uint8_t* pDest = _img->bits();

    // Fill image according to current slice type:
    if(m_orientation == orientation_t::SAGITTAL) // sagittal
    {
        const std::size_t sagitalIndex = static_cast<std::size_t>(m_sagittalIndex);

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            const std::size_t zOffset  = (size[2] - 1 - z) * imageZOffset;
            const std::size_t zxOffset = zOffset + sagitalIndex;

            for(std::size_t y = 0 ; y < size[1] ; ++y)
            {
                const QRgb val = this->getQImageVal(zxOffset + y * size[0], imgBuff, wlMin, tfWin, tf);

                *pDest++ = static_cast<std::uint8_t>(qRed(val));
                *pDest++ = static_cast<std::uint8_t>(qGreen(val));
                *pDest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if(m_orientation == orientation_t::FRONTAL) // frontal
    {
        const std::size_t frontalIndex = static_cast<std::size_t>(m_frontalIndex);
        const std::size_t yOffset      = frontalIndex * size[0];

        for(std::size_t z = 0 ; z < size[2] ; ++z)
        {
            const std::size_t zOffset  = (size[2] - 1 - z) * imageZOffset;
            const std::size_t zyOffset = zOffset + yOffset;

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const QRgb val = this->getQImageVal(zyOffset + x, imgBuff, wlMin, tfWin, tf);

                *pDest++ = static_cast<std::uint8_t>(qRed(val));
                *pDest++ = static_cast<std::uint8_t>(qGreen(val));
                *pDest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if(m_orientation == orientation_t::AXIAL) // axial
    {
        const std::size_t axialIndex = static_cast<std::size_t>(m_axialIndex);
        const std::size_t zOffset    = axialIndex * imageZOffset;

        for(std::size_t y = 0 ; y < size[1] ; ++y)
        {
            const std::size_t yOffset  = y * size[0];
            const std::size_t zyOffset = zOffset + yOffset;

            for(std::size_t x = 0 ; x < size[0] ; ++x)
            {
                const QRgb val = this->getQImageVal(zyOffset + x, imgBuff, wlMin, tfWin, tf);

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

QRgb SNegato::getQImageVal(
    const std::size_t index,
    const short* buffer,
    double wlMin,
    double tfWin,
    const data::TransferFunction::csptr& tf
)
{
    const short val16 = buffer[index];

    double value = (val16 - wlMin) * tfWin;

    const data::TransferFunction::TFColor color = tf->getInterpolatedColor(value);

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

        size    = img->getSize();
        spacing = img->getSpacing();
        origin  = img->getOrigin();
    }

    double qImageSpacing[2];
    double qImageOrigin[2];
    int qImageSize[2];

    switch(m_orientation)
    {
        case orientation_t::X_AXIS: // sagittal
            this->m_yAxis->setScale(-1);
            qImageSize[0]    = static_cast<int>(size[1]);
            qImageSize[1]    = static_cast<int>(size[2]);
            qImageSpacing[0] = spacing[1];
            qImageSpacing[1] = spacing[2];
            qImageOrigin[0]  = origin[1] - 0.5f * spacing[1];
            qImageOrigin[1]  = -(origin[2] + static_cast<double>(size[2]) * spacing[2] - 0.5 * spacing[2]);
            break;

        case orientation_t::Y_AXIS: // frontal
            qImageSize[0]    = static_cast<int>(size[0]);
            qImageSize[1]    = static_cast<int>(size[2]);
            qImageSpacing[0] = spacing[0];
            qImageSpacing[1] = spacing[2];
            qImageOrigin[0]  = origin[0] - 0.5f * spacing[0];
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
    QImage* image = new QImage(qImageSize[0], qImageSize[1], QImage::Format_RGB888);

    // Place m_pixmapItem
    m_pixmapItem->resetTransform();
    m_pixmapItem->setTransform(QTransform::fromScale(qImageSpacing[0], qImageSpacing[1]), true);
    m_pixmapItem->setPos(qImageOrigin[0], qImageOrigin[1]);

    // Force bounding box recomputing ( Qt bug )
    m_layer->removeFromGroup(m_pixmapItem);
    m_layer->addToGroup(m_pixmapItem);

    // Update image scene
    this->getScene2DRender()->updateSceneSize(0.20f);

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

    m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());

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

    this->getScene2DRender()->updateSceneSize(1.f);
}

//-----------------------------------------------------------------------------

void SNegato::updating()
{
    m_qImg = this->createQImage();
    this->updateBufferFromImage(m_qImg);
}

//------------------------------------------------------------------------------

void SNegato::swapping(std::string_view key)
{
    if(key == s_TF_INOUT)
    {
        {
            auto image = m_image.lock();
            SIGHT_ASSERT("Missing image", image);

            auto tf = m_tf.lock();
            m_helperTF.setOrCreateTF(tf.get_shared(), image.get_shared());
        }

        this->updating();
    }
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
    m_helperTF.removeTFConnections();

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
                double newOrigineY = recImage.y() - (heightViewPortNew - recImage.height()) / 2.f;

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
                double newOrigineX = recImage.x() - (widthViewPortNew - recImage.width()) / 2.f;

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

    sight::viz::scene2d::data::Coord coord = this->getScene2DRender()->mapToScene(_event.getCoord());
    coord.setX(coord.getX() / m_layer->scale());
    coord.setY(coord.getY() / m_layer->scale());

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
            sight::viz::scene2d::data::Coord newCoord = _event.getCoord();
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
    sight::viz::scene2d::data::Coord&,
    sight::viz::scene2d::data::Coord& newCoord
)
{
    data::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    data::mt::locked_ptr tfLock(tf);

    const double min = tf->getWLMinMax().first;
    const double max = tf->getWLMinMax().second;

    const double window = newCoord.getX() - m_oldCoord.getX();
    const double level  = newCoord.getY() - m_oldCoord.getY();

    const double imgWindow = max - min;
    const double imgLevel  = min + imgWindow / 2.0;

    const double newImgLevel  = imgLevel + level;
    const double newImgWindow = imgWindow + imgWindow * window / 100.0;

    this->updating();

    // Send signal
    tf->setWindow(newImgWindow);
    tf->setLevel(newImgLevel);
    auto sig = tf->signal<data::TransferFunction::WindowingModifiedSignalType>(
        data::TransferFunction::s_WINDOWING_MODIFIED_SIG
    );
    {
        core::com::Connection::Blocker block(m_helperTF.getTFWindowingConnection());
        sig->asyncEmit(newImgWindow, newImgLevel);
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SNegato::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, data::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_BUFFER_SLOT);
    connections.push(s_IMAGE_INOUT, data::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

} // namespace adaptor

} // namespace sight::module::viz::scene2d
