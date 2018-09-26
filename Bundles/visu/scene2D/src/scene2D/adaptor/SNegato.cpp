/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/SNegato.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwRenderQt/Scene2DGraphicsView.hpp>

#include <fwServices/macros.hpp>

#include <QBitmap>
#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QPoint>

fwServicesRegisterMacro( ::fwRenderQt::IAdaptor, ::scene2D::adaptor::SNegato );

namespace scene2D
{
namespace adaptor
{
static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";
static const ::fwServices::IService::KeyType s_TF_INOUT    = "tf";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_BUFFER_SLOT      = "updateBuffer";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT  = "updateVisibility";

typedef ::fwDataTools::helper::MedicalImage MedicalImage;

//-----------------------------------------------------------------------------

SNegato::SNegato() noexcept :
    m_helperTF(std::bind(&SNegato::updateTF, this)),
    m_qimg(nullptr),
    m_pixmapItem(nullptr),
    m_layer(nullptr),
    m_orientation(MedicalImage::Z_AXIS),
    m_pointIsCaptured(false),
    m_changeSliceTypeAllowed(true)
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SNegato::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SNegato::updateSliceType, this);
    newSlot(s_UPDATE_BUFFER_SLOT, &SNegato::updateBuffer, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SNegato::updateVisibility, this);
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

    if( config.count("orientation") )
    {
        const std::string orientationValue = config.get<std::string>("orientation");

        if ( orientationValue == "axial" )
        {
            m_orientation = MedicalImage::Z_AXIS;
        }
        else if ( orientationValue == "sagittal" )
        {
            m_orientation = MedicalImage::X_AXIS;
        }
        else if ( orientationValue == "frontal" )
        {
            m_orientation = MedicalImage::Y_AXIS;
        }
    }

    if(config.count("changeSliceType"))
    {
        const std::string changeValue = config.get<std::string>("changeSliceType");

        if(changeValue == "true" || changeValue == "yes")
        {
            m_changeSliceTypeAllowed = true;
        }
        else if(changeValue == "no" || changeValue == "false")
        {
            m_changeSliceTypeAllowed = false;
        }
    }
}

//-----------------------------------------------------------------------------

void SNegato::updateBufferFromImage( QImage* qimg )
{
    if(!qimg)
    {
        return;
    }
    // Window min/max
    ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    ::fwData::mt::ObjectReadLock tfLock(tf);
    const double wlMin = tf->getWLMinMax().first;

    // Window max
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    ::fwDataTools::helper::Image imgHelper(image);
    const ::fwData::Image::SizeType size = image->getSize();
    const short* imgBuff                 = static_cast<const short*>(imgHelper.getBuffer());
    const size_t imageZOffset            = size[0] * size[1];

    const double tfMin = tf->getMinMaxTFValues().first;
    const double tfMax = tf->getMinMaxTFValues().second;
    const double tfWin = (1. / tf->getWindow()) * ((tfMax - tfMin) + tfMin);

    std::uint8_t* pDest = qimg->bits();

    // Fill image according to current slice type:
    if( m_orientation == MedicalImage::X_AXIS ) // sagittal
    {
        const size_t sagitalIndex = static_cast<size_t>(m_sagittalIndex->value());

        for( size_t z = 0; z < size[2]; ++z)
        {
            const size_t zOffset  = (size[2] - 1 - z) * imageZOffset;
            const size_t zxOffset = zOffset + sagitalIndex;

            for( size_t y = 0; y < size[1]; ++y )
            {
                const QRgb val = this->getQImageVal(zxOffset + y * size[0], imgBuff, wlMin, tfWin, tf);

                *pDest++ = static_cast<std::uint8_t>(qRed(val));
                *pDest++ = static_cast<std::uint8_t>(qGreen(val));
                *pDest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if( m_orientation == MedicalImage::Y_AXIS ) // frontal
    {
        const size_t frontalIndex = static_cast<size_t>(m_frontalIndex->value());
        const size_t yOffset      = frontalIndex * size[0];

        for( size_t z = 0; z < size[2]; ++z)
        {
            const size_t zOffset  = (size[2] - 1 - z) * imageZOffset;
            const size_t zyOffset = zOffset + yOffset;

            for( size_t x = 0; x < size[0]; ++x )
            {
                const QRgb val = this->getQImageVal(zyOffset + x, imgBuff, wlMin, tfWin, tf);

                *pDest++ = static_cast<std::uint8_t>(qRed(val));
                *pDest++ = static_cast<std::uint8_t>(qGreen(val));
                *pDest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }
    else if( m_orientation == MedicalImage::Z_AXIS ) // axial
    {
        const size_t axialIndex = static_cast<size_t>(m_axialIndex->value());
        const size_t zOffset    = axialIndex * imageZOffset;

        for( size_t y = 0; y < size[1]; ++y )
        {
            const size_t yOffset  = y * size[0];
            const size_t zyOffset = zOffset + yOffset;

            for( size_t x = 0; x < size[0]; ++x )
            {
                const QRgb val = this->getQImageVal(zyOffset + x, imgBuff, wlMin, tfWin, tf);

                *pDest++ = static_cast<std::uint8_t>(qRed(val));
                *pDest++ = static_cast<std::uint8_t>(qGreen(val));
                *pDest++ = static_cast<std::uint8_t>(qBlue(val));
            }
        }
    }

    QPixmap m_pixmap = QPixmap::fromImage( *m_qimg );
    m_pixmapItem->setPixmap(m_pixmap);
}

//-----------------------------------------------------------------------------

QRgb SNegato::getQImageVal(const size_t index, const short* buffer, double wlMin, double tfWin,
                           const fwData::TransferFunction::csptr& tf)
{
    const short val16 = buffer[index];

    double value = (val16 - wlMin) * tfWin;

    const ::fwData::TransferFunction::TFColor color = tf->getInterpolatedColor(value);

    // use QImage::Format_RGBA8888 in QImage if you need alpha value
    return qRgb(static_cast<int>(color.r*255), static_cast<int>(color.g*255), static_cast<int>(color.b*255));
}

//---------------------------------------------------------------------------

QImage* SNegato::createQImage()
{
    ::fwData::Image::sptr img = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    if (!::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( img ))
    {
        return nullptr;
    }

    const ::fwData::Image::SizeType size       = img->getSize();
    const ::fwData::Image::SpacingType spacing = img->getSpacing();
    const ::fwData::Image::OriginType origin   = img->getOrigin();

    double qImageSpacing[2];
    double qImageOrigin[2];
    int qImageSize[2];

    switch ( m_orientation )
    {
        case MedicalImage::X_AXIS:// sagittal
            this->m_yAxis->setScale(-1);
            qImageSize[0]    = static_cast<int>(size[1]);
            qImageSize[1]    = static_cast<int>(size[2]);
            qImageSpacing[0] = spacing[1];
            qImageSpacing[1] = spacing[2];
            qImageOrigin[0]  = origin[1] - 0.5f*spacing[1];
            qImageOrigin[1]  = -( origin[2] + size[2] * spacing[2]  - 0.5f*spacing[2]);
            break;

        case MedicalImage::Y_AXIS:// frontal
            qImageSize[0]    = static_cast<int>(size[0]);
            qImageSize[1]    = static_cast<int>(size[2]);
            qImageSpacing[0] = spacing[0];
            qImageSpacing[1] = spacing[2];
            qImageOrigin[0]  = origin[0] - 0.5f*spacing[0];
            qImageOrigin[1]  = -( origin[2] + size[2] * spacing[2]  - 0.5f*spacing[2]);
            break;

        case MedicalImage::Z_AXIS:// axial
            qImageSize[0]    = static_cast<int>(size[0]);
            qImageSize[1]    = static_cast<int>(size[1]);
            qImageSpacing[0] = spacing[0];
            qImageSpacing[1] = spacing[1];
            qImageOrigin[0]  = origin[0] - 0.5f*spacing[0];
            qImageOrigin[1]  = origin[1] - 0.5f*spacing[1];
            break;

        default:
            SLM_FATAL("Unsupported value for m_orientation");
            break;
    }

    // Create empty QImage
    QImage* qimage = new QImage(qImageSize[0], qImageSize[1], QImage::Format_RGB888);

    // Place m_pixmapItem
    m_pixmapItem->resetTransform();
    m_pixmapItem->setTransform(QTransform::fromScale(qImageSpacing[0], qImageSpacing[1]), true);
    m_pixmapItem->setPos(qImageOrigin[0], qImageOrigin[1]);

    // Force bounding box recomputing ( Qt bug )
    m_layer->removeFromGroup( m_pixmapItem );
    m_layer->addToGroup( m_pixmapItem );

    // Update image scene
    this->getScene2DRender()->updateSceneSize( 0.20f );

    return qimage;
}

//-----------------------------------------------------------------------------

void SNegato::starting()
{

    ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
    if(tf != nullptr)
    {
        ::fwData::mt::ObjectReadLock tfLock(tf);
        m_helperTF.setTransferFunction(tf);
    }
    else
    {
        m_helperTF.setTransferFunction(tf);
    }

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    this->updateImageInfos( image );
    m_helperTF.createTransferFunction( image );

    m_pixmapItem = new QGraphicsPixmapItem();
    m_pixmapItem->setShapeMode( QGraphicsPixmapItem::BoundingRectShape );
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    m_layer = new QGraphicsItemGroup();
    m_layer->resetTransform();
    m_layer->addToGroup( m_pixmapItem );
    m_layer->setPos( m_xAxis->getOrigin(), m_yAxis->getOrigin() );
    m_layer->setZValue( m_zValue );
    this->getScene2DRender()->getScene()->addItem( m_layer );

    m_qimg = this->createQImage();
    this->updateBufferFromImage( m_qimg );

    this->getScene2DRender()->updateSceneSize( 1.f );

    m_helperTF.installTFConnections();
}

//-----------------------------------------------------------------------------

void SNegato::updating()
{
    m_qimg = this->createQImage();
    this->updateBufferFromImage( m_qimg );
}

//------------------------------------------------------------------------------

void SNegato::swapping(const KeyType& key)
{
    if (key == s_TF_INOUT)
    {
        ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
        SLM_ASSERT("Missing image", image);

        ::fwData::TransferFunction::sptr tf = this->getInOut< ::fwData::TransferFunction>(s_TF_INOUT);
        if(tf != nullptr)
        {
            ::fwData::mt::ObjectReadLock tfLock(tf);
            m_helperTF.setOrCreateTF(tf, image);
        }
        else
        {
            m_helperTF.setOrCreateTF(tf, image);
        }
        this->updating();
    }
}

//-----------------------------------------------------------------------------

void SNegato::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    this->updateImageInfos( image );
    this->updateBufferFromImage( m_qimg );
}

//-----------------------------------------------------------------------------

void SNegato::updateSliceType(int from, int to)
{
    if (m_changeSliceTypeAllowed)
    {
        if( to == static_cast< int > ( m_orientation ) )
        {
            m_orientation = static_cast< MedicalImage::Orientation > ( from );
        }
        else if(from == static_cast<int>(m_orientation))
        {
            m_orientation = static_cast< MedicalImage::Orientation >( to );
        }

        // manages the modification of axes
        if ( m_orientation == MedicalImage::Z_AXIS )
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
    if( isVisible ) // display the scene
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
    this->updateBufferFromImage(m_qimg);
}

//------------------------------------------------------------------------------

void SNegato::updateTF()
{
    this->updateBufferFromImage( m_qimg );
}

//-----------------------------------------------------------------------------

void SNegato::stopping()
{
    m_helperTF.removeTFConnections();

    this->getScene2DRender()->getScene()->removeItem(m_layer);

    delete m_qimg;
    delete m_pixmapItem;
    delete m_layer;
}

//-----------------------------------------------------------------------------

void SNegato::processInteraction( ::fwRenderQt::data::Event& _event )
{
    // if a key is pressed
    if(_event.getType() == ::fwRenderQt::data::Event::KeyRelease)
    {
        // if pressed key is 'R'
        if ( _event.getKey() == Qt::Key_R )
        {
            // get image origin
            QRectF recImage = m_pixmapItem->sceneBoundingRect();

            ::fwRenderQt::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

            float sceneWidth  = static_cast<float>(this->getScene2DRender()->getView()->width());
            float sceneHeight = static_cast<float>(this->getScene2DRender()->getView()->height());

            float ratioYonXimage = recImage.height() / recImage.width();
            float sceneRatio     = sceneHeight / sceneWidth;

            if ( sceneRatio > ratioYonXimage ) // used scene ratio
            {
                float widthViewPortNew  = recImage.width();
                float heightViewPortNew = widthViewPortNew * sceneRatio;

                // computes new y origin
                float newOrigineY = recImage.y() - ( heightViewPortNew - recImage.height() ) / 2.f;

                sceneViewport->setX(  recImage.x() );
                sceneViewport->setY( newOrigineY );
                sceneViewport->setWidth(  widthViewPortNew );
                sceneViewport->setHeight( heightViewPortNew );
            }
            else
            {
                float heightViewPortNew = recImage.height();
                float widthViewPortNew  = heightViewPortNew / sceneRatio;

                // computes new x origin
                float newOrigineX = recImage.x() - (widthViewPortNew - recImage.width() )/ 2.f;

                sceneViewport->setX( newOrigineX );
                sceneViewport->setY( recImage.y() );
                sceneViewport->setWidth(  widthViewPortNew );
                sceneViewport->setHeight( heightViewPortNew );
            }

            this->getScene2DRender()->getView()->updateFromViewport();
        }

        //image pixel
        if ( _event.getKey() == Qt::Key_F )
        {
            m_pixmapItem->setTransformationMode(Qt::FastTransformation);
            this->updating();
        }

        //image smooth
        if ( _event.getKey() == Qt::Key_S )
        {
            m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
            this->updating();
        }
    }

    ::fwRenderQt::data::Coord coord = this->getScene2DRender()->mapToScene( _event.getCoord() );
    coord.setX( coord.getX() / m_layer->scale());
    coord.setY( coord.getY() / m_layer->scale());

    if ( _event.getType() == ::fwRenderQt::data::Event::MouseButtonPress
         && _event.getButton() == ::fwRenderQt::data::Event::RightButton
         && _event.getModifier() == ::fwRenderQt::data::Event::NoModifier )
    {
        m_pointIsCaptured = true;
        m_oldCoord        = _event.getCoord();
        _event.setAccepted(true);
    }
    else if ( m_pointIsCaptured )
    {
        if( _event.getType() == ::fwRenderQt::data::Event::MouseMove )
        {
            ::fwRenderQt::data::Coord newCoord = _event.getCoord();
            this->changeImageMinMaxFromCoord( m_oldCoord, newCoord );
            m_oldCoord = newCoord;
            _event.setAccepted(true);
        }
        else if( _event.getButton() == ::fwRenderQt::data::Event::RightButton
                 && _event.getType() == ::fwRenderQt::data::Event::MouseButtonRelease )
        {
            m_pointIsCaptured = false;
            _event.setAccepted(true);
        }
    }
}

//-----------------------------------------------------------------------------

void SNegato::changeImageMinMaxFromCoord( ::fwRenderQt::data::Coord& oldCoord, ::fwRenderQt::data::Coord& newCoord )
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    ::fwData::TransferFunction::sptr tf = m_helperTF.getTransferFunction();
    ::fwData::mt::ObjectWriteLock tfLock(tf);

    const double min = tf->getWLMinMax().first;
    const double max = tf->getWLMinMax().second;

    const double window = newCoord.getX() - m_oldCoord.getX();
    const double level  = newCoord.getY() - m_oldCoord.getY();

    const double imgWindow = max - min;
    const double imgLevel  = min + imgWindow/2.0;

    const double newImgLevel  = imgLevel + level;
    const double newImgWindow = imgWindow + imgWindow * window/100.0;

    this->updating();

    // Send signal
    tf->setWindow(newImgWindow);
    tf->setLevel(newImgLevel);
    auto sig = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
        ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(m_helperTF.getTFWindowingConnection());
        sig->asyncEmit( newImgWindow, newImgLevel);
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SNegato::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_BUFFER_SLOT );
    connections.push( s_IMAGE_INOUT, ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT );
    return connections;
}

//-----------------------------------------------------------------------------

} // namespace adaptor
} // namespace scene2D
