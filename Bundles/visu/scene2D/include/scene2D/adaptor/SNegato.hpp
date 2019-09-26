/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "scene2D/config.hpp"

#include <fwDataTools/helper/MedicalImage.hpp>
#include <fwDataTools/helper/TransferFunction.hpp>

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
 * @brief IAdaptor implementation to display one slice of an image.
 *
 * @section Slots Slots
 * - \b updateSliceIndex() : update image slice index
 * - \b updateSliceType() : update image slice type
 * - \b updateBuffer() : update image buffer
 * - \b updateVisibility() : update image visibility
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="negato" type="::scene2D::adaptor::SNegato" autoConnect="yes">
       <inout key="image" uid="..." />
       <inout key="tf" uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" color="gray" opacity="0.25" zValue="5" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to display.
 * - \b tf [::fwData::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration:
 * - \b config (mandatory): contains the adaptor configuration
 *    - \b xAxis (optional): x axis associated to the adaptor
 *    - \b yAxis (optional): y axis associated to the adaptor
 *    - \b zValue (optional, default=0): z value of the layer
 *    - \b opacity (optional, default=1.0): adaptor opacity (float)
 *    - \b orientation (optional, default axial): image orientation, axial, sagittal or frontal
 *    - \b changeSliceType (optional, default true): specify if the negato allow slice type events
 */
class SCENE2D_CLASS_API SNegato : public ::fwRenderQt::IAdaptor
{

public:

    fwCoreServiceMacro(SNegato, ::fwRenderQt::IAdaptor);

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
    SCENE2D_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    SCENE2D_API void configuring() override;

    SCENE2D_API void starting() override;

    SCENE2D_API void updating() override;

    SCENE2D_API void stopping() override;

    /// Retrives the current transfer function
    SCENE2D_API void swapping(const KeyType& key) override;

    SCENE2D_API void processInteraction( ::fwRenderQt::data::Event& _event ) override;

    /// Slot: updates the TF
    SCENE2D_API void updateTF();

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
                             double tfWin, const ::fwData::TransferFunction::csptr& tf);

    QImage* m_qimg;

    QGraphicsPixmapItem* m_pixmapItem;

    QGraphicsItemGroup* m_layer;

    /// The current orientation of the negato
    ::fwDataTools::helper::MedicalImage::Orientation m_orientation;

    /// Used during negato interaction to manage window/level
    bool m_pointIsCaptured;

    /// Ref. position when changing image window/level
    ::fwRenderQt::data::Coord m_oldCoord;

    /// Specify if the negato allow slice type events
    bool m_changeSliceTypeAllowed;

    ::fwDataTools::helper::TransferFunction m_helperTF;

    ::fwDataTools::helper::MedicalImage m_helperImg;
};

} // namespace adaptor
} // namespace scene2D
