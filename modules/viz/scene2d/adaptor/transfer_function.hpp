/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <data/transfer_function.hpp>

#include <viz/scene2d/adaptor.hpp>

#include <QGraphicsItemGroup>

namespace sight::module::viz::scene2d::adaptor
{

/**
 * @brief Defines an adaptor to display a composite of TF and interact with them.
 *
 * The following actions are available:
 * - Left mouse click: selects a new current TF or move the current clicked TF point.
 * - Left mouse double click: adds a new TF point to the current TF or open a color dialog
 *                            to change the current clicked TF point.
 * - Middle mouse click: adjusts the transfer function level and window by moving
 *                       the mouse up/down and left/right respectively.
 * - Right mouse click: remove the current clicked TF point or open a context menu
 *                      to manage multiple actions which are 'delete', 'add ramp', 'clamp' or 'linear'.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service uid="..." type="sight::module::viz::scene2d::adaptor::transfer_function" >
       <in key="viewport" uid="..." />
       <inout key="tf" uid="..." />
       <config lineColor="lightGray" pointColor="lightGray" xAxis="xAxis" yAxis="yAxis" zValue="0" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b viewport [sight::viz::scene2d::data::viewport]: object listened to update the adaptor.
 * - \b tf [sight::data::transfer_function](optional): current transfer function used to change editor selection.
 * It should be the same TF as the output.
 *
 * @subsection Configuration Configuration
 * - \b config (mandatory): contains the adaptor configuration.
 *    - \b xAxis (optional): x axis associated to the adaptor.
 *    - \b yAxis (optional): y axis associated to the adaptor.
 *    - \b zValue (optional, default="0"): z value of the layer.
 *    - \b lineColor (optional, default="#FFFFFF"): color of the lines between the points.
 *    - \b pointColor (optional, default="#FFFFFF"): outline color of circles representing the TF points.
 *    - \b secondOpacity (optional, default="0.0"): opacity of TF that not the current one.
 *    - \b pointSize (optional, default="0.03"): size of TF points in a ratio relative to the window.
 *    - \b opacity (optional, default="1.0"): opacity of the gradient.
 *    - \b interactive (optional, true/false, default="false"): enables interactions.
 */
class MODULE_VIZ_SCENE2D_CLASS_API transfer_function : public QObject,
                                                       public sight::viz::scene2d::adaptor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(transfer_function, sight::viz::scene2d::adaptor);

    /// Creates the adaptor.
    MODULE_VIZ_SCENE2D_API transfer_function() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE2D_API ~transfer_function() noexcept override;

protected:

    /// Configures the adaptor.
    void configuring() override;

    /**
     * @brief Initializes the current TF, the layer and draw all TF.
     *
     * @see updating()
     */
    void starting() override;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect sight::viz::scene2d::data::viewport::MODIFIED_SIG of s_VIEWPORT_INPUT to
     * module::viz::scene2d::adaptor::transfer_function::service::slots::UPDATE.
     * Connect data::object::MODIFIED_SIG of s_TF_POOL_INOUT to
     * module::viz::scene2d::adaptor::transfer_function::service::slots::UPDATE.
     * Connect data::composite::ADDED_OBJECTS_SIGof s_TF_POOL_INOUT to
     * module::viz::scene2d::adaptor::transfer_function::service::slots::UPDATE.
     * Connect data::composite::REMOVED_OBJECTS_SIG of s_TF_POOL_INOUT to
     * module::viz::scene2d::adaptor::transfer_function::service::slots::UPDATE.
     */
    connections_t auto_connections() const override;

    /// Release all graphics items and draw all TF, all TF connections a established here.
    void updating() override;

    /// Release all graphic items and disconect all TF in the composite.
    void stopping() override;

private:

    using vec2d_t = sight::viz::scene2d::vec2d_t;

    /// Represents a sub-TF which is a TF of the input composite.
    struct piece_view
    {
        /// Contains the TF data.
        data::transfer_function_piece::sptr m_tf;

        /// Sets the z value in the local layer.
        int m_z_index {0};

        /// Contains a set of graphic point and its coordinates in the window/level space.
        std::vector<std::pair<vec2d_t, QGraphicsEllipseItem*> > m_tf_points;

        /// Contains the graphic gradient.
        std::vector<QAbstractGraphicsShapeItem*> m_tf_polygons;
    };

    /// Deletes pieceView in @ref m_pieceView and clears them.
    void release_tf_data();

    /**
     * @brief Creates pieceView for each TF in the composite, fills basic data and create graphic points.
     *
     * @see createPieceView(const data::transfer_function::sptr _tf, int _zIndex)
     */
    void create_tf_points();

    /// Removes all graphic points in @ref m_pieceView from the layer and deletes them.
    void destroy_tf_points();

    /// Creates a pieceView from a TF, fills basic data and creates graphic points.
    piece_view* create_piece_view(const data::transfer_function_piece::sptr _tf, int _z_index);

    /// Creates the gradient of each pieceView and stores it in each element of @ref m_pieceView.
    void create_tf_polygons();

    /**
     * @brief Creates lines and gradient polygons of a pieceView.
     * @param _pieceView the pieceView used to create the gradient and store the generated graphic item.
     */
    void create_tf_polygon(piece_view* _piece_view);

    /**
     * @brief Removes all graphic gradient in @ref m_pieceView from the layer and deletes them.
     *
     * @see destroyTFPolygons()
     */
    void destroy_tf_polygons();

    /**
     * @brief Removes graphic gradient of the pieceView from the layer and deletes them.
     * @param _pieceView to where destory the polygon.
     */
    void destroy_tf_polygon(piece_view* _piece_view);

    /**
     * @brief Creates lines and linear gradient polygons of a pieceView.
     * @param _pieceView the pieceView used to create the gradient.
     * @param _position the position vector to fill.
     * @param _grad the gradient to create.
     * @param _distanceMax the maximum distance used by the gradient.
     */
    static void build_linear_polygons(
        piece_view* _piece_view,
        QVector<QPointF>& _position,
        QLinearGradient& _grad,
        double _distance_max
    );

    /**
     * @brief Creates lines and nearest gradient polygons of a pieceView.
     * @param _pieceView the pieceView used to create the gradient.
     * @param _position the position vector to fill.
     * @param _grad the gradient to create.
     * @param _distanceMax the maximum distance used by the gradient.
     */
    static void build_nearest_polygons(
        piece_view* _piece_view,
        QVector<QPointF>& _position,
        QLinearGradient& _grad,
        double _distance_max
    );

    /// Adds graphic items of @ref m_pieceView to @ref m_layer at the right z-index.
    void build_layer();

    /**
     * @brief Changes @ref m_currentTF with the new one.
     *
     * Sets the new current TF as output of this adaptor and updates z-index of each pieceView in @ref m_pieceView.
     *
     * @param _pieceView the new current pieceView.
     */
    void set_current_tf(piece_view* _piece_view);

    /**
     * @brief Get pieceView that match the clicked coord of the event.
     * @param _event the 2D scene event.
     * @return A list of pieceView.
     */
    std::vector<piece_view*> get_matching_piece_view(const sight::viz::scene2d::data::event& _event) const;

    /**
     * @brief Filters the event to call the right methods from mouse informations.
     * @param _event the 2D scene event.
     *
     * The following actions are available:
     * - Left mouse click: selects a new current TF or move the current clicked TF point.
     * - Left mouse double click: adds a new TF point to the current TF or open a color dialog
     *                            to change the current clicked TF point.
     * - Middle mouse click: adjusts the transfer function level and window by moving
     *                       the mouse up/down and left/right respectively.
     * - Right mouse click: remove the current clicked TF point or open a context menu
     *                      to manage multiple actions which are 'delete', 'add ramp', 'clamp' or 'linear'.
     * - Wheel move: updates the whole current TF opacity.
     */
    void process_interaction(sight::viz::scene2d::data::event& _event) override;

    /**
     * @brief Finds the nearest pieceView and set it a the current one.
     * @param _event the 2D scene event.
     *
     * @see setCurrentTF(PieceView* const)
     */
    void left_button_click_event(const sight::viz::scene2d::data::event& _event);

    /**
     * @brief Sets @ref m_capturedTFPoint and highlight the captured clicked point.
     * @param _pieceView the selected pieceView.
     * @param _TFPoint the selected TF point.
     */
    void left_button_click_on_point_event(
        piece_view* _piece_view,
        std::pair<vec2d_t,
                  QGraphicsEllipseItem*>& _tf_point
    );

    /**
     * @brief Move @ref m_capturedTFPoint to the new mouse position and update the related TF.
     * @param _event the 2D scene event.
     *
     * @pre m_capturedTFPoint must be previously sets.
     * @see leftButtonClickOnPointEvent(PieceView* const, std::pair< vec2d_t, QGraphicsEllipseItem* >&)
     */
    void mouse_move_on_point_event(piece_view* _piece_view, const sight::viz::scene2d::data::event& _event);

    /**
     * @brief Resets the captured TF point highlighting and sets @ref m_capturedTFPoint to null.
     *
     * @pre m_capturedTFPoint must be previously sets.
     * @see leftButtonClickOnPointEvent(PieceView* const, std::pair< vec2d_t, QGraphicsEllipseItem* >&)
     */
    void left_button_release_event();

    /**
     * @brief Removes a TF point from the current pieceView and update the related TF.
     * @param _pieceView the selected pieceView.
     * @param _TFPoint the selected TF point.
     */
    void right_button_click_on_point_event(
        piece_view* _piece_view,
        std::pair<vec2d_t, QGraphicsEllipseItem*>& _tf_point
    );

    /**
     * @brief Changes the TF point color by opening a color dialog and update the related TF.
     * @param _pieceView the selected pieceView.
     * @param _TFPoint the selected TF point.
     */
    void left_button_double_click_on_point_event(
        piece_view* _piece_view,
        std::pair<vec2d_t, QGraphicsEllipseItem*>& _tf_point
    );

    /**
     * @brief Adds a new TF point to the current pieceView and update the related TF.
     * @param _event the 2D scene event.
     */
    void left_button_double_click_event(const sight::viz::scene2d::data::event& _event);

    /**
     * @brief Sets @ref m_capturedTF if the clicked coord if over the current TF.
     * @param _event the 2D scene event.
     */
    void mid_button_click_event(sight::viz::scene2d::data::event& _event);

    /**
     * @brief Update the window/level of the current TF relatively to the mouse movement.
     * @param _event the 2D scene event.
     *
     * @pre m_capturedTF must be previously sets.
     * @see midButtonClickEvent(const sight::viz::scene2d::data::Event&)
     */
    void mouse_move_on_piece_view_event(const sight::viz::scene2d::data::event& _event);

    /**
     * @brief Resets @ref m_capturedTF.
     *
     * @pre m_capturedTF must be previously sets.
     * @see midButtonClickEvent(const sight::viz::scene2d::data::Event&)
     */
    void mid_button_release_event();

    /**
     * @brief Open a context menu to delete or create TF.
     * @param _event the 2D scene event.
     */
    void right_button_c_lick_event(const sight::viz::scene2d::data::event& _event);

    /**
     * @brief Updates the whole current TF opacity.
     * @param _event the 2D scene event.
     */
    void mid_button_wheel_move_event(sight::viz::scene2d::data::event& _event);

    /// Deletes the current TF and change the current TF.
    void remove_current_tf();

    /**
     * @brief Sets if the current TF is clamped or not.
     * @param _clamp the clamp status.
     */
    void clamp_current_tf(bool _clamp);

    /**
     * @brief Sets if the current TF interpolation mode is linear or nearest.
     * @param _linear uses true is the interpolation mode must be linear.
     */
    void toggle_linear_current_tf(bool _linear);

    /**
     * @brief Adds a new TF to the composite and re draw the scene.
     * @param _tf the new TF to add.
     */
    void add_new_tf(const data::transfer_function_piece::sptr _tf);

    /**
     * @brief Adds a left ramp pieceView and update the composite.
     * @param _event the 2D scene event.
     *
     * @see addNewTF(const data::transfer_function::sptr, const data::composite::key_t&)
     */
    void add_left_ramp(const sight::viz::scene2d::data::event& _event);

    /**
     * @brief Adds a right ramp pieceView and update the composite.
     * @param _event the 2D scene event.
     *
     * @see addNewTF(const data::transfer_function::sptr, const data::composite::key_t&)
     */
    void add_right_ramp(const sight::viz::scene2d::data::event& _event);

    /**
     * @brief Adds a trapeze pieceView and update the composite.
     * @param _event the 2D scene event.
     *
     * @see addNewTF(const data::transfer_function::sptr, const data::composite::key_t&)
     */
    void add_trapeze(const sight::viz::scene2d::data::event& _event);

    /// Updates the transfer function.
    void update_tf();
    /// Sends the point modified signal of input _tf.
    void points_modified(const sight::data::transfer_function& _tf) const;

    /// Defines the size of TF points in a ratio relative to the window.
    float m_point_size {0.03F};

    /// Defines the pen used by gradients.
    QPen m_polygons_pen;

    /// Defines the pen used by TF points.
    QPen m_points_pen;

    /// Defines the opacity used for TF except for the current one.
    float m_second_opacity {0.0F};

    /// Sets if interactions are enable or not.
    bool m_interactive {true};

    /// Stores all created pieceView.
    std::vector<piece_view*> m_piece_view;

    /// Stores the main layer.
    QGraphicsItemGroup* m_layer {};

    /**
     * We never know if a single click might be followed by another single click effectively resulting in a double
     * click. Qt does it and delivers events for double clicks (QEvent.MouseButtonDblClick). On the other hand Qt still
     * delivers events for single clicks (QEvent.MouseButtonPress) even in the case of a double click, but only one.
     * We must differentiate them correctly. We do it with an additional timer that needs to be a bit longer than the
     * inbuilt Qt timer for detecting double clicks.
     */
    QTimer* m_event_filter {nullptr};

    /// Stores the current working TF.
    data::transfer_function_piece::sptr m_current_tf {nullptr};

    /// Stores the captured clicked point.
    std::pair<vec2d_t, QGraphicsEllipseItem*>* m_captured_tf_point {nullptr};

    /// Stores the captured clicked TF and the current mouse position,
    /// the first coord is in the window/level space and the second in screen space,
    /// it allows to adjust the window/level of the current TF.
    std::pair<data::transfer_function_piece::sptr, sight::viz::scene2d::vec2d_t> m_captured_tf;

    static constexpr std::string_view VIEWPORT_INPUT   = "viewport";
    static constexpr std::string_view CURRENT_TF_INOUT = "tf";

    data::ptr<sight::viz::scene2d::data::viewport, sight::data::access::in> m_viewport {this, VIEWPORT_INPUT, true};
    data::ptr<sight::data::transfer_function, sight::data::access::inout> m_tf {this, CURRENT_TF_INOUT, true};
};

} // namespace sight::module::viz::scene2d::adaptor
