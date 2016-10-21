/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "scene2D/adaptor/Negato.hpp"

#include "scene2D/Scene2DGraphicsView.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>
#include <fwDataTools/helper/Image.hpp>

#include <fwServices/macros.hpp>

#include <QBitmap>
#include <QGraphicsItemGroup>
#include <QPixmap>
#include <QPoint>

fwServicesRegisterMacro( ::scene2D::adaptor::IAdaptor, ::scene2D::adaptor::Negato, ::fwData::Image );


namespace scene2D
{
namespace adaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_BUFFER_SLOT      = "updateBuffer";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT  = "updateVisibility";

typedef ::fwDataTools::helper::MedicalImageAdaptor MedicalImageAdaptor;

//-----------------------------------------------------------------------------

Negato::Negato() throw() :
    m_qimg(nullptr),
    m_pixmapItem(nullptr),
    m_layer(nullptr),
    m_orientation(MedicalImageAdaptor::Z_AXIS),
    m_pointIsCaptured (false),
    m_changeSliceTypeAllowed(true)
{
    this->installTFSlots(this);
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &Negato::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &Negato::updateSliceType, this);
    newSlot(s_UPDATE_BUFFER_SLOT, &Negato::updateBuffer, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &Negato::updateVisibility, this);
}

//-----------------------------------------------------------------------------

Negato::~Negato() throw()
{
}

//-----------------------------------------------------------------------------

void Negato::configuring() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    SLM_ASSERT("\"config\" tag missing", m_configuration->getName() == "config");

    this->IAdaptor::configuring();

    SLM_TRACE("IAdaptor configuring ok");

    if( !m_configuration->getAttributeValue("orientation").empty() )
    {
        const std::string orientationValue = m_configuration->getAttributeValue("orientation");

        if ( orientationValue == "axial" )
        {
            m_orientation = MedicalImageAdaptor::Z_AXIS;
        }
        else if ( orientationValue == "sagittal" )
        {
            m_orientation = MedicalImageAdaptor::X_AXIS;
        }
        else if ( orientationValue == "frontal" )
        {
            m_orientation = MedicalImageAdaptor::Y_AXIS;
        }
    }

    if(!m_configuration->getAttributeValue("changeSliceType").empty())
    {
        const std::string changeValue = m_configuration->getAttributeValue(("changeSliceType"));

        if(changeValue == "true" || changeValue == "yes")
        {
            m_changeSliceTypeAllowed = true;
        }
        else if(changeValue == "no" || changeValue == "false")
        {
            m_changeSliceTypeAllowed = false;
        }
    }
    this->parseTFConfig( m_configuration );
}

//-----------------------------------------------------------------------------

void Negato::updateBufferFromImage( QImage* qimg )
{
    if(!qimg)
    {
        return;
    }
    // Window min/max
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();
    const double wlMin = tf->getWLMinMax().first;

    // Window max
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    ::fwDataTools::helper::Image imgHelper(image);
    const ::fwData::Image::SizeType size = image->getSize();
    const short* imgBuff                 = static_cast<const short*>(imgHelper.getBuffer());
    const size_t imageZOffset            = size[0] * size[1];

    const double tfMin = tf->getMinMaxTFValues().first;
    const double tfMax = tf->getMinMaxTFValues().second;
    const double tfWin = (1. / tf->getWindow()) * ((tfMax - tfMin) + tfMin);

    ::std::uint8_t* pDest = qimg->bits();

    // Fill image according to current slice type:
    if( m_orientation == MedicalImageAdaptor::X_AXIS ) // sagittal
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
    else if( m_orientation == MedicalImageAdaptor::Y_AXIS ) // frontal
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
    else if( m_orientation == MedicalImageAdaptor::Z_AXIS ) // axial
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

QRgb Negato::getQImageVal(const size_t index, const short* buffer, double wlMin, double tfWin,
                          const fwData::TransferFunction::sptr& tf)
{
    const short val16 = buffer[index];

    double value = (val16 - wlMin) * tfWin;

    const ::fwData::TransferFunction::TFColor color = tf->getInterpolatedColor(value);

    // use QImage::Format_RGBA8888 in QImage if you need alpha value
    return qRgb(static_cast<int>(color.r*255), static_cast<int>(color.g*255), static_cast<int>(color.b*255));
}

//---------------------------------------------------------------------------

QImage* Negato::createQImage()
{
    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();

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
        case MedicalImageAdaptor::X_AXIS:// sagittal
            this->m_yAxis->setScale(-1);
            qImageSize[0]    = static_cast<int>(size[1]);
            qImageSize[1]    = static_cast<int>(size[2]);
            qImageSpacing[0] = spacing[1];
            qImageSpacing[1] = spacing[2];
            qImageOrigin[0]  = origin[1] - 0.5f*spacing[1];
            qImageOrigin[1]  = -( origin[2] + size[2] * spacing[2]  - 0.5f*spacing[2]);
            break;

        case MedicalImageAdaptor::Y_AXIS:// frontal
            qImageSize[0]    = static_cast<int>(size[0]);
            qImageSize[1]    = static_cast<int>(size[2]);
            qImageSpacing[0] = spacing[0];
            qImageSpacing[1] = spacing[2];
            qImageOrigin[0]  = origin[0] - 0.5f*spacing[0];
            qImageOrigin[1]  = -( origin[2] + size[2] * spacing[2]  - 0.5f*spacing[2]);
            break;

        case MedicalImageAdaptor::Z_AXIS:// axial
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

void Negato::doStart() throw ( ::fwTools::Failed )
{
    ::fwData::Composite::wptr tfSelection = this->getSafeInOut< ::fwData::Composite>(this->getTFSelectionFwID());
    this->setTransferFunctionSelection(tfSelection);

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos( image );
    this->updateTransferFunction( image );

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

    this->installTFConnections();
}

//-----------------------------------------------------------------------------

void Negato::doUpdate() throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    m_qimg = this->createQImage();
    this->updateBufferFromImage( m_qimg );
}

//-----------------------------------------------------------------------------

void Negato::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos( image );
    this->updateBufferFromImage( m_qimg );
}

//-----------------------------------------------------------------------------

void Negato::updateSliceType(int from, int to)
{
    if (m_changeSliceTypeAllowed)
    {
        if( to == static_cast< int > ( m_orientation ) )
        {
            m_orientation = static_cast< MedicalImageAdaptor::Orientation > ( from );
        }
        else if(from == static_cast<int>(m_orientation))
        {
            m_orientation = static_cast< MedicalImageAdaptor::Orientation >( to );
        }

        // manages the modification of axes
        if ( m_orientation == MedicalImageAdaptor::Z_AXIS )
        {
            this->m_yAxis->setScale(1);
        }
        else
        {
            this->m_yAxis->setScale(-1);
        }

        this->doUpdate();
    }
}

//-----------------------------------------------------------------------------

void Negato::updateVisibility(bool isVisible)
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

void Negato::updateBuffer()
{
    this->updateBufferFromImage(m_qimg);
}

//------------------------------------------------------------------------------

void Negato::updatingTFPoints()
{
    this->updateBufferFromImage( m_qimg );
}

//------------------------------------------------------------------------------

void Negato::updatingTFWindowing(double window, double level)
{
    this->updateBufferFromImage( m_qimg );
}

//-----------------------------------------------------------------------------

void Negato::doSwap() throw ( ::fwTools::Failed )
{
    this->doStop();
    this->doStart();
}

//-----------------------------------------------------------------------------

void Negato::doStop() throw ( ::fwTools::Failed )
{
    this->removeTFConnections();

    this->getScene2DRender()->getScene()->removeItem(m_layer);

    delete m_qimg;
    delete m_pixmapItem;
    delete m_layer;
}

//-----------------------------------------------------------------------------

void Negato::processInteraction( ::scene2D::data::Event::sptr _event )
{
    SLM_TRACE_FUNC();

    // if a key is pressed
    if(_event->getType() == ::scene2D::data::Event::KeyRelease)
    {
        // if pressed key is 'R'
        if ( _event->getKey() == Qt::Key_R )
        {
            // get image origin
            QRectF recImage = m_pixmapItem->sceneBoundingRect();

            ::scene2D::data::Viewport::sptr sceneViewport = this->getScene2DRender()->getViewport();

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
        if ( _event->getKey() == Qt::Key_F )
        {
            m_pixmapItem->setTransformationMode(Qt::FastTransformation);
            this->doUpdate();
        }

        //image smooth
        if ( _event->getKey() == Qt::Key_S )
        {
            m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
            this->doUpdate();
        }
    }

    ::scene2D::data::Coord coord = this->getScene2DRender()->mapToScene( _event->getCoord() );
    coord.setX( coord.getX() / m_layer->scale());
    coord.setY( coord.getY() / m_layer->scale());

    if ( _event->getType() == ::scene2D::data::Event::MouseButtonPress
         && _event->getButton() == ::scene2D::data::Event::RightButton
         && _event->getModifier() == ::scene2D::data::Event::NoModifier )
    {
        m_pointIsCaptured = true;
        m_oldCoord        = _event->getCoord();
        _event->setAccepted(true);
    }
    else if ( m_pointIsCaptured )
    {
        if( _event->getType() == ::scene2D::data::Event::MouseMove )
        {
            ::scene2D::data::Coord newCoord = _event->getCoord();
            this->changeImageMinMaxFromCoord( m_oldCoord, newCoord );
            m_oldCoord = newCoord;
            _event->setAccepted(true);
        }
        else if( _event->getButton() == ::scene2D::data::Event::RightButton
                 && _event->getType() == ::scene2D::data::Event::MouseButtonRelease )
        {
            m_pointIsCaptured = false;
            _event->setAccepted(true);
        }
    }
}

//-----------------------------------------------------------------------------

void Negato::changeImageMinMaxFromCoord( scene2D::data::Coord& oldCoord, scene2D::data::Coord& newCoord )
{
    ::fwData::Image::sptr image         = this->getObject< ::fwData::Image >();
    ::fwData::TransferFunction::sptr tf = this->getTransferFunction();

    double min = tf->getWLMinMax().first;
    double max = tf->getWLMinMax().second;

    double window = newCoord.getX() - m_oldCoord.getX();
    double level  = newCoord.getY() - m_oldCoord.getY();

    double imgWindow = max - min;
    double imgLevel  = min + imgWindow/2.0;


    double newImgLevel  = imgLevel + level;
    double newImgWindow = imgWindow + imgWindow * window/100.0;

    this->doUpdate();

    // Send signal
    tf->setWindow(newImgWindow);
    tf->setLevel(newImgLevel);
    auto sig = tf->signal< ::fwData::TransferFunction::WindowingModifiedSignalType >(
        ::fwData::TransferFunction::s_WINDOWING_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdateTFWindowing));
        sig->asyncEmit( newImgWindow, newImgLevel);
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Negato::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_BUFFER_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_VISIBILITY_MODIFIED_SIG, s_UPDATE_VISIBILITY_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace adaptor
} // namespace scene2D



