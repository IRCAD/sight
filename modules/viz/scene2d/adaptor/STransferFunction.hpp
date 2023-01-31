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

#include <data/TransferFunction.hpp>

#include <viz/scene2d/IAdaptor.hpp>

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
   <service uid="..." type="sight::module::viz::scene2d::adaptor::STransferFunction" >
       <in key="viewport" uid="..." />
       <inout key="tf" uid="..." />
       <config lineColor="lightGray" pointColor="lightGray" xAxis="xAxis" yAxis="yAxis" zValue="0" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b viewport [sight::viz::scene2d::data::Viewport]: object listened to update the adaptor.
 * - \b tf [sight::data::TransferFunction](optional): current transfer function used to change editor selection.
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
class MODULE_VIZ_SCENE2D_CLASS_API STransferFunction : public QObject,
                                                       public sight::viz::scene2d::IAdaptor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(STransferFunction, sight::viz::scene2d::IAdaptor);

    /// Creates the adaptor.
    MODULE_VIZ_SCENE2D_API STransferFunction() noexcept;

    /// Destroys the adaptor.
    MODULE_VIZ_SCENE2D_API ~STransferFunction() noexcept override;

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
     * Connect sight::viz::scene2d::data::Viewport::s_MODIFIED_SIG of s_VIEWPORT_INPUT to
     * module::viz::scene2d::adaptor::STransferFunction::IService::slots::s_UPDATE.
     * Connect data::Object::s_MODIFIED_SIG of s_TF_POOL_INOUT to
     * module::viz::scene2d::adaptor::STransferFunction::IService::slots::s_UPDATE.
     * Connect data::Composite::s_ADDED_OBJECTS_SIGof s_TF_POOL_INOUT to
     * module::viz::scene2d::adaptor::STransferFunction::IService::slots::s_UPDATE.
     * Connect data::Composite::s_REMOVED_OBJECTS_SIG of s_TF_POOL_INOUT to
     * module::viz::scene2d::adaptor::STransferFunction::IService::slots::s_UPDATE.
     */
    KeyConnectionsMap getAutoConnections() const override;

    /// Release all graphics items and draw all TF, all TF connections a established here.
    void updating() override;

    /// Release all graphic items and disconect all TF in the composite.
    void stopping() override;

private:

    using vec2d_t = sight::viz::scene2d::vec2d_t;

    /// Represents a sub-TF which is a TF of the input composite.
    struct PieceView
    {
        /// Contains the TF data.
        data::TransferFunctionPiece::sptr m_tf;

        /// Sets the z value in the local layer.
        int m_zIndex {0};

        /// Contains a set of graphic point and its coordinates in the window/level space.
        std::vector<std::pair<vec2d_t, QGraphicsEllipseItem*> > m_TFPoints;

        /// Contains the graphic gradient.
        std::vector<QAbstractGraphicsShapeItem*> m_TFPolygons;
    };

    /// Deletes pieceView in @ref m_pieceView and clears them.
    void releaseTFData();

    /**
     * @brief Creates pieceView for each TF in the composite, fills basic data and create graphic points.
     *
     * @see createPieceView(const data::TransferFunction::sptr _tf, int _zIndex)
     */
    void createTFPoints();

    /// Removes all graphic points in @ref m_pieceView from the layer and deletes them.
    void destroyTFPoints();

    /// Creates a pieceView from a TF, fills basic data and creates graphic points.
    PieceView* createPieceView(const data::TransferFunctionPiece::sptr _tf, int _zIndex);

    /// Creates the gradient of each pieceView and stores it in each element of @ref m_pieceView.
    void createTFPolygons();

    /**
     * @brief Creates lines and gradient polygons of a pieceView.
     * @param _pieceView the pieceView used to create the gradient and store the generated graphic item.
     */
    void createTFPolygon(PieceView* _pieceView);

    /**
     * @brief Removes all graphic gradient in @ref m_pieceView from the layer and deletes them.
     *
     * @see destroyTFPolygons()
     */
    void destroyTFPolygons();

    /**
     * @brief Removes graphic gradient of the pieceView from the layer and deletes them.
     * @param _pieceView to where destory the polygon.
     */
    void destroyTFPolygon(PieceView* _pieceView);

    /**
     * @brief Creates lines and linear gradient polygons of a pieceView.
     * @param _pieceView the pieceView used to create the gradient.
     * @param _position the position vector to fill.
     * @param _grad the gradient to create.
     * @param _distanceMax the maximum distance used by the gradient.
     */
    static void buildLinearPolygons(
        PieceView* _pieceView,
        QVector<QPointF>& _position,
        QLinearGradient& _grad,
        double _distanceMax
    );

    /**
     * @brief Creates lines and nearest gradient polygons of a pieceView.
     * @param _pieceView the pieceView used to create the gradient.
     * @param _position the position vector to fill.
     * @param _grad the gradient to create.
     * @param _distanceMax the maximum distance used by the gradient.
     */
    static void buildNearestPolygons(
        PieceView* _pieceView,
        QVector<QPointF>& _position,
        QLinearGradient& _grad,
        double _distanceMax
    );

    /// Adds graphic items of @ref m_pieceView to @ref m_layer at the right z-index.
    void buildLayer();

    /**
     * @brief Changes @ref m_currentTF with the new one.
     *
     * Sets the new current TF as output of this adaptor and updates z-index of each pieceView in @ref m_pieceView.
     *
     * @param _pieceView the new current pieceView.
     */
    void setCurrentTF(PieceView* _pieceView);

    /**
     * @brief Get pieceView that match the clicked coord of the event.
     * @param _event the 2D scene event.
     * @return A list of pieceView.
     */
    std::vector<PieceView*> getMatchingPieceView(const sight::viz::scene2d::data::Event& _event) const;

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
    void processInteraction(sight::viz::scene2d::data::Event& _event) override;

    /**
     * @brief Finds the nearest pieceView and set it a the current one.
     * @param _event the 2D scene event.
     *
     * @see setCurrentTF(PieceView* const)
     */
    void leftButtonClickEvent(const sight::viz::scene2d::data::Event& _event);

    /**
     * @brief Sets @ref m_capturedTFPoint and highlight the captured clicked point.
     * @param _pieceView the selected pieceView.
     * @param _TFPoint the selected TF point.
     */
    void leftButtonClickOnPointEvent(PieceView* _pieceView, std::pair<vec2d_t, QGraphicsEllipseItem*>& _TFPoint);

    /**
     * @brief Move @ref m_capturedTFPoint to the new mouse position and update the related TF.
     * @param _event the 2D scene event.
     *
     * @pre m_capturedTFPoint must be previously sets.
     * @see leftButtonClickOnPointEvent(PieceView* const, std::pair< vec2d_t, QGraphicsEllipseItem* >&)
     */
    void mouseMoveOnPointEvent(PieceView* _pieceView, const sight::viz::scene2d::data::Event& _event);

    /**
     * @brief Resets the captured TF point highlighting and sets @ref m_capturedTFPoint to null.
     *
     * @pre m_capturedTFPoint must be previously sets.
     * @see leftButtonClickOnPointEvent(PieceView* const, std::pair< vec2d_t, QGraphicsEllipseItem* >&)
     */
    void leftButtonReleaseEvent();

    /**
     * @brief Removes a TF point from the current pieceView and update the related TF.
     * @param _pieceView the selected pieceView.
     * @param _TFPoint the selected TF point.
     */
    void rightButtonClickOnPointEvent(
        PieceView* _pieceView,
        std::pair<vec2d_t, QGraphicsEllipseItem*>& _TFPoint
    );

    /**
     * @brief Changes the TF point color by opening a color dialog and update the related TF.
     * @param _pieceView the selected pieceView.
     * @param _TFPoint the selected TF point.
     */
    void leftButtonDoubleClickOnPointEvent(
        PieceView* _pieceView,
        std::pair<vec2d_t, QGraphicsEllipseItem*>& _TFPoint
    );

    /**
     * @brief Adds a new TF point to the current pieceView and update the related TF.
     * @param _event the 2D scene event.
     */
    void leftButtonDoubleClickEvent(const sight::viz::scene2d::data::Event& _event);

    /**
     * @brief Sets @ref m_capturedTF if the clicked coord if over the current TF.
     * @param _event the 2D scene event.
     */
    void midButtonClickEvent(sight::viz::scene2d::data::Event& _event);

    /**
     * @brief Update the window/level of the current TF relatively to the mouse movement.
     * @param _event the 2D scene event.
     *
     * @pre m_capturedTF must be previously sets.
     * @see midButtonClickEvent(const sight::viz::scene2d::data::Event&)
     */
    void mouseMoveOnPieceViewEvent(const sight::viz::scene2d::data::Event& _event);

    /**
     * @brief Resets @ref m_capturedTF.
     *
     * @pre m_capturedTF must be previously sets.
     * @see midButtonClickEvent(const sight::viz::scene2d::data::Event&)
     */
    void midButtonReleaseEvent();

    /**
     * @brief Open a context menu to delete or create TF.
     * @param _event the 2D scene event.
     */
    void rightButtonCLickEvent(const sight::viz::scene2d::data::Event& _event);

    /**
     * @brief Updates the whole current TF opacity.
     * @param _event the 2D scene event.
     */
    void midButtonWheelMoveEvent(sight::viz::scene2d::data::Event& _event);

    /// Deletes the current TF and change the current TF.
    void removeCurrentTF();

    /**
     * @brief Sets if the current TF is clamped or not.
     * @param _clamp the clamp status.
     */
    void clampCurrentTF(bool _clamp);

    /**
     * @brief Sets if the current TF interpolation mode is linear or nearest.
     * @param _linear uses true is the interpolation mode must be linear.
     */
    void toggleLinearCurrentTF(bool _linear);

    /**
     * @brief Adds a new TF to the composite and re draw the scene.
     * @param _tf the new TF to add.
     */
    void addNewTF(const data::TransferFunctionPiece::sptr _tf);

    /**
     * @brief Adds a left ramp pieceView and update the composite.
     * @param _event the 2D scene event.
     *
     * @see addNewTF(const data::TransferFunction::sptr, const data::Composite::KeyType&)
     */
    void addLeftRamp(const sight::viz::scene2d::data::Event& _event);

    /**
     * @brief Adds a right ramp pieceView and update the composite.
     * @param _event the 2D scene event.
     *
     * @see addNewTF(const data::TransferFunction::sptr, const data::Composite::KeyType&)
     */
    void addRightRamp(const sight::viz::scene2d::data::Event& _event);

    /**
     * @brief Adds a trapeze pieceView and update the composite.
     * @param _event the 2D scene event.
     *
     * @see addNewTF(const data::TransferFunction::sptr, const data::Composite::KeyType&)
     */
    void addTrapeze(const sight::viz::scene2d::data::Event& _event);

    /// Updates the transfer function.
    void updateTF();

    /// Defines the size of TF points in a ratio relative to the window.
    float m_pointSize {0.03F};

    /// Defines the pen used by gradients.
    QPen m_polygonsPen;

    /// Defines the pen used by TF points.
    QPen m_pointsPen;

    /// Defines the opacity used for TF except for the current one.
    float m_secondOpacity {0.0F};

    /// Sets if interactions are enable or not.
    bool m_interactive {true};

    /// Stores all created pieceView.
    std::vector<PieceView*> m_pieceView;

    /// Stores the main layer.
    QGraphicsItemGroup* m_layer {};

    /**
     * We never know if a single click might be followed by another single click effectively resulting in a double
     * click. Qt does it and delivers events for double clicks (QEvent.MouseButtonDblClick). On the other hand Qt still
     * delivers events for single clicks (QEvent.MouseButtonPress) even in the case of a double click, but only one.
     * We must differentiate them correctly. We do it with an additional timer that needs to be a bit longer than the
     * inbuilt Qt timer for detecting double clicks.
     */
    QTimer* m_eventFilter {nullptr};

    /// Stores the current working TF.
    data::TransferFunctionPiece::sptr m_currentTF {nullptr};

    /// Stores the captured clicked point.
    std::pair<vec2d_t, QGraphicsEllipseItem*>* m_capturedTFPoint {nullptr};

    /// Stores the captured clicked TF and the current mouse position,
    /// the first coord is in the window/level space and the second in screen space,
    /// it allows to adjust the window/level of the current TF.
    std::pair<data::TransferFunctionPiece::sptr, sight::viz::scene2d::vec2d_t> m_capturedTF;

    static constexpr std::string_view s_VIEWPORT_INPUT   = "viewport";
    static constexpr std::string_view s_CURRENT_TF_INOUT = "tf";

    data::ptr<sight::viz::scene2d::data::Viewport, sight::data::Access::in> m_viewport {this, s_VIEWPORT_INPUT, true};
    data::ptr<sight::data::TransferFunction, sight::data::Access::inout> m_tf {this, s_CURRENT_TF_INOUT, true};
};

} // namespace sight::module::viz::scene2d::adaptor
