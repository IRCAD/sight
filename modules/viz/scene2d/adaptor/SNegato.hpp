/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "modules/viz/scene2d/config.hpp"

#include <data/helper/MedicalImage.hpp>

#include <viz/scene2d/IAdaptor.hpp>

#include <QGraphicsItemGroup>
#include <QImage>
#include <QPointF>

namespace sight::module::viz::scene2d::adaptor
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
   <service uid="negato" type="sight::module::viz::scene2d::adaptor::SNegato" autoConnect="true">
       <inout key="image" uid="..." />
       <inout key="tf" uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" color="gray" opacity="0.25" zValue="5" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::Image]: image to display.
 * - \b tf [sight::data::TransferFunction] (optional): the current TransferFunction. If it is not defined, we use the
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
class MODULE_VIZ_SCENE2D_CLASS_API SNegato : public sight::viz::scene2d::IAdaptor
{
public:

    SIGHT_DECLARE_SERVICE(SNegato, sight::viz::scene2d::IAdaptor);

    MODULE_VIZ_SCENE2D_API SNegato() noexcept;

    MODULE_VIZ_SCENE2D_API ~SNegato() noexcept override;

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
    MODULE_VIZ_SCENE2D_API service::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    MODULE_VIZ_SCENE2D_API void configuring() override;

    MODULE_VIZ_SCENE2D_API void starting() override;

    MODULE_VIZ_SCENE2D_API void updating() override;

    MODULE_VIZ_SCENE2D_API void stopping() override;

    MODULE_VIZ_SCENE2D_API void processInteraction(sight::viz::scene2d::data::Event& _event) override;

    /// Slot: updates the TF
    MODULE_VIZ_SCENE2D_API void updateTF();

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

    void updateBufferFromImage(QImage* _img);

    void changeImageMinMaxFromCoord(
        sight::viz::scene2d::vec2d_t& oldCoord,
        sight::viz::scene2d::vec2d_t& newCoord
    );

    static QRgb getQImageVal(std::int16_t value, const data::TransferFunction& tf);

    QImage* m_qImg {nullptr};

    QGraphicsPixmapItem* m_pixmapItem {nullptr};

    QGraphicsItemGroup* m_layer {nullptr};

    using orientation_t = data::helper::MedicalImage::orientation_t;

    /// The current orientation of the negato
    orientation_t m_orientation {orientation_t::Z_AXIS};

    /// Used during negato interaction to manage window/level
    bool m_pointIsCaptured {false};

    /// Ref. position when changing image window/level
    sight::viz::scene2d::vec2d_t m_oldCoord {};

    /// Specify if the negato allow slice type events
    bool m_changeSliceTypeAllowed {true};

    static constexpr std::string_view s_IMAGE_IN = "image";
    static constexpr std::string_view s_TF_INOUT = "tf";

    sight::data::ptr<sight::data::Image, sight::data::Access::in> m_image {this, s_IMAGE_IN, true};
    sight::data::ptr<sight::data::TransferFunction, sight::data::Access::inout> m_tf {this, s_TF_INOUT, true};
    sight::data::ptr<sight::viz::scene2d::data::Viewport, sight::data::Access::inout> m_viewport {this, "viewport"};

    /// Stores current slice index on each orientation.
    std::int64_t m_axialIndex {-1};
    std::int64_t m_frontalIndex {-1};
    std::int64_t m_sagittalIndex {-1};
};

} // namespace sight::module::viz::scene2d::adaptor
