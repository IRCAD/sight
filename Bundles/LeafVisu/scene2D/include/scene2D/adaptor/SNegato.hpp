/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SCENE2D_ADAPTOR_SNEGATO_HPP__
#define __SCENE2D_ADAPTOR_SNEGATO_HPP__

#include "scene2D/config.hpp"

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderQt/data/Coord.hpp>
#include <fwRenderQt/IAdaptor.hpp>

#include <QGraphicsItemGroup>
#include <QImage>
#include <QPointF>

namespace scene2D
{
namespace adaptor
{

/**
 * @brief   IAdaptor implementation to display one slice of an image.
 *
 * Configuration example:
 *
   @code{.xml}
   <service uid="negato" type="::scene2D::adaptor::SNegato">
       <inout key="viewport" uid="..." />
       <inout key="image" uid="..." />
       <inout key="tfSelection" uid="..." />
       <config xAxis="xAxis" yAxis="axeHistogramY" color="gray" opacity="0.25" zValue="5" selectedTFKey="tkKey"/>
   </service>
   @endcode
 */
class SCENE2D_CLASS_API SNegato : public ::fwDataTools::helper::MedicalImageAdaptor,
                                  public ::fwRenderQt::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SNegato)(::fwRenderQt::IAdaptor) );

    SCENE2D_API SNegato() noexcept;
    SCENE2D_API virtual ~SNegato() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_SLICE_INDEX_MODIFIED_SIG to this::s_UPDATE_SLICE_INDEX_SLOT
     * Connect Image::s_SLICE_TYPE_MODIFIED_SIG to this::s_UPDATE_SLICE_TYPE_SLOT
     * Connect Image::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_VISIBILITY_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_BUFFER_SLOT
     */
    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

protected:

    SCENE2D_API void configuring();
    SCENE2D_API void starting();
    SCENE2D_API void updating();
    SCENE2D_API void stopping();
    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event );

    /// Called when transfer function points are modified.
    SCENE2D_API virtual void updatingTFPoints();

    /// Called when transfer function windowing is modified.
    SCENE2D_API virtual void updatingTFWindowing(double window, double level);

private:

    /**
     * @name Slots
     * @{
     */
    /// Slot: update image slice index
    void updateSliceIndex(int axial, int frontal, int sagittal);

    /// Slot: update image slice type
    void updateSliceType(int from, int to);

    /// Slot: update image buffer
    void updateBuffer();

    /// Slot: update image visibility
    void updateVisibility(bool isVisible);
    /**
     * @}
     */

    QImage* createQImage();
    void updateBufferFromImage( QImage* qimg );
    void changeImageMinMaxFromCoord( ::fwRenderQt::data::Coord& oldCoord, ::fwRenderQt::data::Coord& newCoord );

    static QRgb getQImageVal(const size_t index, const short* buffer, double wlMin,
                             double tfWin, const ::fwData::TransferFunction::sptr& tf);

    QImage* m_qimg;
    QGraphicsPixmapItem* m_pixmapItem;
    QGraphicsItemGroup* m_layer;

    /// The current orientation of the negato
    ::fwDataTools::helper::MedicalImageAdaptor::Orientation m_orientation;

    /// Used during negato interaction to manage window/level
    bool m_pointIsCaptured;

    /// Ref. position when changing image window/level
    ::fwRenderQt::data::Coord m_oldCoord;

    /// Specify if the negato allow slice type events
    bool m_changeSliceTypeAllowed;
};

} // namespace adaptor
} // namespace scene2D

#endif // __SCENE2D_ADAPTOR_SNEGATO_HPP__

