/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <data/helper/medical_image.hpp>

#include <viz/scene2d/adaptor.hpp>

#include <QGraphicsItemGroup>
#include <QImage>
#include <QPointF>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief adaptor implementation to display one slice of an image.
 *
 * @section Slots Slots
 * - \b updateSliceIndex() : update image slice index
 * - \b updateSliceType() : update image slice type
 * - \b updateBuffer() : update image buffer
 * - \b update_visibility() : update image visibility
 *
 * @section XML XML Configuration
 *
   @code{.xml}
   <service uid="negato" type="sight::module::viz::scene2d::adaptor::negato" auto_connect="true">
       <inout key="image" uid="..." />
       <inout key="tf" uid="..." />
       <config xAxis="xAxis" yAxis="yAxis" color="gray" opacity="0.25" zValue="5" />
   </service>
   @endcode
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::image]: image to display.
 * - \b tf [sight::data::transfer_function] (optional): the current TransferFunction. If it is not defined, we use the
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
class negato : public sight::viz::scene2d::adaptor
{
public:

    SIGHT_DECLARE_SERVICE(negato, sight::viz::scene2d::adaptor);

    negato() noexcept;

    ~negato() noexcept override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect image::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect image::SLICE_INDEX_MODIFIED_SIG to this::UPDATE_SLICE_INDEX_SLOT
     * Connect image::SLICE_TYPE_MODIFIED_SIG to this::UPDATE_SLICE_TYPE_SLOT
     * Connect image::BUFFER_MODIFIED_SIG to this::UPDATE_BUFFER_SLOT
     */
    service::connections_t auto_connections() const override;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    void process_interaction(sight::viz::scene2d::data::event& _event) override;

    /// Slot: updates the TF
    void update_tf();

private:

    /**
     * @name Slots
     * @{
     */

    /// Slot: update image slice index
    void update_slice_index(int _axial, int _frontal, int _sagittal);

    /// Slot: update image slice type
    void update_slice_type(int _from, int _to);

    /// Slot: update image buffer
    void update_buffer();

    /// Slot: update image visibility
    void update_visibility(bool _is_visible);
    /**
     * @}
     */

    QImage* create_q_image();

    void update_buffer_from_image(QImage* _img);

    void change_image_min_max_from_coord(
        sight::viz::scene2d::vec2d_t& _old_coord,
        sight::viz::scene2d::vec2d_t& _new_coord
    );

    static QRgb get_q_image_val(std::int16_t _value, const data::transfer_function& _tf);

    QImage* m_q_img {nullptr};

    QGraphicsPixmapItem* m_pixmap_item {nullptr};

    QGraphicsItemGroup* m_layer {nullptr};

    using orientation_t = data::helper::medical_image::orientation_t;

    /// The current orientation of the negato
    orientation_t m_orientation {orientation_t::z_axis};

    /// Used during negato interaction to manage window/level
    bool m_point_is_captured {false};

    /// Ref. position when changing image window/level
    sight::viz::scene2d::vec2d_t m_old_coord {};

    /// Specify if the negato allow slice type events
    bool m_change_slice_type_allowed {true};

    static constexpr std::string_view IMAGE_IN = "image";
    static constexpr std::string_view TF_INOUT = "tf";

    sight::data::ptr<sight::data::image, sight::data::access::in> m_image {this, IMAGE_IN, true};
    sight::data::ptr<sight::data::transfer_function, sight::data::access::inout> m_tf {this, TF_INOUT, true};
    sight::data::ptr<sight::viz::scene2d::data::viewport, sight::data::access::inout> m_viewport {this, "viewport"};

    /// Stores current slice index on each orientation.
    std::int64_t m_axial_index {-1};
    std::int64_t m_frontal_index {-1};
    std::int64_t m_sagittal_index {-1};
};

} // namespace sight::module::viz::scene2d::adaptor
