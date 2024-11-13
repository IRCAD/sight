/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include "modules/viz/scene2d/adaptor/transfer_function.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/profiling.hpp>

#include <data/container.hpp>

#include <viz/scene2d/data/init_qt_pen.hpp>
#include <viz/scene2d/graphics_view.hpp>

#include <glm/common.hpp>

#include <QAction>
#include <QApplication>
#include <QColorDialog>
#include <QDesktopWidget>
#include <QGraphicsRectItem>
#include <QMenu>
#include <QTimer>

namespace sight::module::viz::scene2d::adaptor
{

static const core::com::slots::key_t UPDATE_TF_SLOT = "update_tf";

//-----------------------------------------------------------------------------

transfer_function::transfer_function() noexcept :
    m_event_filter(new QTimer())
{
    new_slot(UPDATE_TF_SLOT, &transfer_function::update_tf, this);
}

//-----------------------------------------------------------------------------

transfer_function::~transfer_function() noexcept
{
    delete m_event_filter;
}

//-----------------------------------------------------------------------------

void transfer_function::configuring()
{
    this->configure_params();

    const config_t tree = this->get_config();
    const auto config   = tree.get_child("config.<xmlattr>");

    const std::string polygon_color = config.get("lineColor", "#FFFFFF");
    sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_polygons_pen, polygon_color);

    const std::string point_color = config.get("pointColor", "#FFFFFF");
    sight::viz::scene2d::data::init_qt_pen::set_pen_color(m_points_pen, point_color);

    m_second_opacity = config.get<float>("secondOpacity", m_second_opacity);
    m_point_size     = config.get<float>("pointSize", m_point_size);
    m_interactive    = config.get<bool>("interactive", m_interactive);
}

//------------------------------------------------------------------------------

void transfer_function::starting()
{
    // Sets the current TF.
    {
        // Sets the current TF used to highlight it in the editor.
        const auto tf = m_tf.const_lock();
        if(tf && !tf->pieces().empty())
        {
            m_current_tf = tf->pieces().front();
        }
        else
        {
            SIGHT_FATAL("The current TF mustn't be null");
        }
    }

    // Adds the layer item to the scene.
    m_layer = new QGraphicsItemGroup();
    this->get_scene_2d_render()->get_scene()->addItem(m_layer);

    m_points_pen.setCosmetic(true);
    m_points_pen.setWidthF(0);

    m_polygons_pen.setCosmetic(true);
    m_polygons_pen.setWidthF(0);

    // Creates all entities.
    this->updating();
}

//------------------------------------------------------------------------------

service::connections_t transfer_function::auto_connections() const
{
    connections_t connections;
    connections.push(VIEWPORT_INPUT, sight::viz::scene2d::data::viewport::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(CURRENT_TF_INOUT, data::object::MODIFIED_SIG, UPDATE_TF_SLOT);
    connections.push(CURRENT_TF_INOUT, data::transfer_function::WINDOWING_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(CURRENT_TF_INOUT, data::transfer_function::POINTS_MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

void transfer_function::updating()
{
    // Clears old data.
    this->destroy_tf_polygons();
    this->destroy_tf_points();
    this->release_tf_data();

    // Creates all TF.
    this->create_tf_points();
    this->create_tf_polygons();

    // Builds the layer.
    this->build_layer();
}

//-----------------------------------------------------------------------------

void transfer_function::stopping()
{
    this->destroy_tf_polygons();
    this->destroy_tf_points();
    this->release_tf_data();
}

//-----------------------------------------------------------------------------

void transfer_function::release_tf_data()
{
    for(piece_view* const piece_view : m_piece_view)
    {
        delete piece_view;
    }

    m_piece_view.clear();
}

//-----------------------------------------------------------------------------

void transfer_function::create_tf_points()
{
    SIGHT_ASSERT("The current TF mustn't be null", m_current_tf);

    // Iterates over each TF to create a piece view.
    const auto tf = m_tf.const_lock();

    if(!tf->pieces().empty())
    {
        int z_index = 0;
        for(const auto& tf_data : tf->pieces())
        {
            // Sets the z-index of the current TF over all others.
            const bool is_current = m_current_tf == tf_data;
            int index             = is_current ? static_cast<int>(tf->pieces().size()) : z_index;

            // Pushes the piece view to the vector.
            m_piece_view.push_back(this->create_piece_view(tf_data, index));
            ++z_index;
        }
    }
}

//-----------------------------------------------------------------------------

void transfer_function::destroy_tf_points()
{
    // Removes TF point items from the scene and clear the TF point vector of each piece view.
    for(piece_view* const piece_view : m_piece_view)
    {
        for(std::pair<vec2d_t, QGraphicsEllipseItem*>& tf_point : piece_view->m_tf_points)
        {
            this->get_scene_2d_render()->get_scene()->removeItem(tf_point.second);
            delete tf_point.second;
        }

        piece_view->m_tf_points.clear();
    }
}

//-----------------------------------------------------------------------------

transfer_function::piece_view* transfer_function::create_piece_view(
    const data::transfer_function_piece::sptr _tf,
    int _z_index
)
{
    auto viewport = m_viewport.lock();

    const double scene_width  = this->get_scene_2d_render()->get_view()->width();
    const double scene_height = this->get_scene_2d_render()->get_view()->height();

    // Computes point size in screen space and keep the smallest size (relatively to width or height).
    double point_size = scene_width * m_point_size;
    if(point_size > scene_height * m_point_size)
    {
        point_size = scene_height * m_point_size;
    }

    const double viewport_width  = viewport->width();
    const double viewport_height = viewport->height();

    // Computes point size from screen space to viewport space.
    const double point_width  = (viewport_width * point_size) / scene_width;
    const double point_height = (viewport_height * point_size) / scene_height;

    // Creates the piece view and fill basic informations.
    auto* piece_view = new struct piece_view ();
    piece_view->m_tf      = _tf;
    piece_view->m_z_index = _z_index;

    // Fills piece view point with color points.
    for(const auto& elt : *_tf)
    {
        // Computes TF value from TF space to window/level space.
        const data::transfer_function::value_t value = _tf->map_value_to_window(elt.first);

        // Creates the color.
        const data::transfer_function::color_t color_t = elt.second;
        const vec2d_t val_color(value, std::min(color_t.a, 1.));
        vec2d_t coord = this->map_adaptor_to_scene(val_color);

        // Builds a point item, set its color, pen and zIndex.
        auto* point = new QGraphicsEllipseItem(
            coord.x - point_width / 2,
            coord.y - point_height / 2,
            point_width,
            point_height
        );
        QColor color(static_cast<int>(color_t.r * 255),
                     static_cast<int>(color_t.g * 255),
                     static_cast<int>(color_t.b * 255));
        point->setBrush(QBrush(color));
        point->setPen(m_points_pen);
        point->setZValue(piece_view->m_z_index * 2 + 1);

        // Pushes it back into the point vector
        if(_tf->window() > 0)
        {
            piece_view->m_tf_points.emplace_back(coord, point);
        }
        // If the window is negative, the TF is reversed and point must be sort in reverse.
        else
        {
            piece_view->m_tf_points.insert(piece_view->m_tf_points.begin(), std::make_pair(coord, point));
        }
    }

    return piece_view;
}

//-----------------------------------------------------------------------------

void transfer_function::create_tf_polygons()
{
    // Iterates over all piece views to create polygons.
    for(piece_view* const piece_view : m_piece_view)
    {
        this->create_tf_polygon(piece_view);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::create_tf_polygon(piece_view* const _piece_view)
{
    if(m_interactive)
    {
        const auto viewport = m_viewport.lock();

        QVector<QPointF> position;
        QLinearGradient grad;

        const std::pair<vec2d_t, QGraphicsEllipseItem*>& first_tf_point = _piece_view->m_tf_points.front();
        const std::pair<vec2d_t, QGraphicsEllipseItem*>& last_tf_point  = _piece_view->m_tf_points.back();

        const QGraphicsEllipseItem* const first_point = first_tf_point.second;

        double x_begin = first_tf_point.first.x;
        double x_end   = last_tf_point.first.x;

        if(_piece_view->m_tf->clamped())
        {
            position.append(QPointF(x_begin, 0));
        }
        else
        {
            if(x_begin > viewport->x())
            {
                x_begin = viewport->x() - 10;
                position.append(QPointF(x_begin, 0));
                position.append(QPointF(x_begin, first_tf_point.first.y));
            }
            else
            {
                position.append(QPointF(x_begin, 0));
            }

            if(x_end < viewport->x() + viewport->width())
            {
                x_end = viewport->x() + viewport->width() + 10;
            }
        }

        grad.setColorAt(0, first_point->brush().color());

        grad.setStart(x_begin, 0);
        grad.setFinalStop(x_end, 0);

        const double distance_max = x_end - x_begin;

        // Iterates on TF points vector to add line and polygon items to the polygons vector.
        if(_piece_view->m_tf->get_interpolation_mode() == data::transfer_function::interpolation_mode::linear)
        {
            sight::module::viz::scene2d::adaptor::transfer_function::build_linear_polygons(
                _piece_view,
                position,
                grad,
                distance_max
            );
        }
        else
        {
            sight::module::viz::scene2d::adaptor::transfer_function::build_nearest_polygons(
                _piece_view,
                position,
                grad,
                distance_max
            );
        }

        if(!_piece_view->m_tf->clamped())
        {
            if(x_end == viewport->x() + viewport->width() + 10)
            {
                position.append(QPointF(x_end, last_tf_point.first.y));
            }

            const double last_point_x = last_tf_point.first.x;
            grad.setColorAt((last_point_x - x_begin) / distance_max, last_tf_point.second->brush().color());
        }

        position.append(QPointF(x_end, 0));
        grad.setColorAt(1, last_tf_point.second->brush().color());

        auto* const poly = new QGraphicsPolygonItem(QPolygonF(position));
        // Sets gradient, opacity and pen to the polygon
        poly->setBrush(QBrush(grad));
        poly->setPen(m_polygons_pen);
        poly->setZValue(_piece_view->m_z_index * 2);

        // If the z-index is the highest, it's the current one.
        if(static_cast<std::size_t>(_piece_view->m_z_index) == m_piece_view.size())
        {
            poly->setOpacity(m_opacity);
        }
        else
        {
            poly->setOpacity(m_second_opacity);
        }

        // Pushes the polygon back into the polygons vector
        _piece_view->m_tf_polygons.push_back(poly);
    }
    else
    {
        const auto viewport = m_viewport.lock();

        auto* const view            = this->get_scene_2d_render()->get_view();
        const double viewport_width = viewport->width();
        const double step           = viewport_width / view->width();
        const uint size             = static_cast<uint>(viewport_width / step);
        double f                    = viewport->left();

        for(uint i = 0 ; i < size ; ++i)
        {
            const auto color = _piece_view->m_tf->sample(f);

            vec2d_t pt = this->map_adaptor_to_scene({f, color.a});

            QColor q_color;
            q_color.setRgbF(color.r, color.g, color.b, 1.0);
            QBrush brush = QBrush(q_color);

            auto* rect = new QGraphicsRectItem(pt.x, 0.0, step, pt.y);
            rect->setBrush(brush);
            rect->setPen(Qt::NoPen);

            _piece_view->m_tf_polygons.push_back(rect);

            f += step;
        }
    }
}

//-----------------------------------------------------------------------------

void transfer_function::destroy_tf_polygons()
{
    // Removes polygon items from the scene and clear the polygon vector.
    for(piece_view* const piece_view : m_piece_view)
    {
        this->destroy_tf_polygon(piece_view);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::destroy_tf_polygon(piece_view* _piece_view)
{
    for(auto& poly : _piece_view->m_tf_polygons)
    {
        this->get_scene_2d_render()->get_scene()->removeItem(poly);
        delete poly;
    }

    // Removes polygon items from the scene and clear the polygon vector.
    _piece_view->m_tf_polygons.clear();
}

//-----------------------------------------------------------------------------

void transfer_function::build_linear_polygons(
    piece_view* const _piece_view,
    QVector<QPointF>& _position,
    QLinearGradient& _grad,
    double _distance_max
)
{
    const std::vector<std::pair<vec2d_t, QGraphicsEllipseItem*> >& tf_points = _piece_view->m_tf_points;
    for(auto tf_point_it = tf_points.cbegin() ; tf_point_it != tf_points.cend() - 1 ; ++tf_point_it)
    {
        const QPointF p1(tf_point_it->first.x, tf_point_it->first.y);
        const QPointF p2((tf_point_it + 1)->first.x, (tf_point_it + 1)->first.y);

        _position.append(p1);
        _position.append(p2);

        // Builds the gradient
        _grad.setColorAt((p1.x() - _position[0].x()) / _distance_max, (tf_point_it->second)->brush().color());
    }
}

//-----------------------------------------------------------------------------

void transfer_function::build_nearest_polygons(
    piece_view* const _piece_view,
    QVector<QPointF>& _position,
    QLinearGradient& _grad,
    double _distance_max
)
{
    const std::vector<std::pair<vec2d_t, QGraphicsEllipseItem*> >& tf_points = _piece_view->m_tf_points;
    for(auto tf_point_it = tf_points.cbegin() ; tf_point_it != tf_points.cend() - 1 ; ++tf_point_it)
    {
        const QPointF p1(tf_point_it->first.x, tf_point_it->first.y);
        const QPointF p4((tf_point_it + 1)->first.x, (tf_point_it + 1)->first.y);

        const QPointF p2(p1.x() + (p4.x() - p1.x()) / 2., p1.y());
        const QPointF p3(p2.x(), p4.y());

        _position.append(p1);
        _position.append(p2);
        _position.append(p3);
        _position.append(p4);

        const double d1 = (p1.x() - _position[0].x()) / _distance_max;
        const double d2 = (p2.x() - _position[0].x()) / _distance_max;
        const double d3 = d2 + std::numeric_limits<double>::epsilon();
        const double d4 = (p4.x() - _position[0].x()) / _distance_max;

        const QColor c1 = (tf_point_it->second)->brush().color();
        const QColor c4 = ((tf_point_it + 1)->second)->brush().color();

        _grad.setColorAt(d1, c1);
        _grad.setColorAt(d2, c1);
        _grad.setColorAt(d3, c4);
        _grad.setColorAt(d4, c4);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::build_layer()
{
    // Adds graphics items vectors to the layer.
    for(piece_view* const piece_view : m_piece_view)
    {
        for(std::pair<vec2d_t, QGraphicsEllipseItem*>& tf_point : piece_view->m_tf_points)
        {
            m_layer->addToGroup(tf_point.second);
        }

        for(auto& poly : piece_view->m_tf_polygons)
        {
            m_layer->addToGroup(poly);
        }
    }

    // Adjusts the layer's position and zValue depending on the associated axis.
    m_layer->setPos(m_x_axis->origin(), m_y_axis->origin());
    m_layer->setZValue(m_z_value);
}

//-----------------------------------------------------------------------------

void transfer_function::set_current_tf(piece_view* const _piece_view)
{
    // Sets the new current TF.
    SIGHT_ASSERT("The current TF mustn't be null", m_current_tf);

    // Find the old piece view.
    piece_view* const current_piece_view = *(std::find_if(
                                                 m_piece_view.begin(),
                                                 m_piece_view.end(),
                                                 [&](const piece_view* _piece_view)
        {
            return _piece_view->m_tf == m_current_tf;
        }));

    // Changes the current piece view.
    m_current_tf = _piece_view->m_tf;

    // Recomputes z-index and set the z-index of the selected TF over all others.
    int z_index = 0;
    for(piece_view* piece_view : m_piece_view)
    {
        piece_view->m_z_index = piece_view->m_tf == m_current_tf ? static_cast<int>(m_piece_view.size()) : z_index;
        for(std::pair<vec2d_t, QGraphicsEllipseItem*>& point : piece_view->m_tf_points)
        {
            point.second->setZValue(piece_view->m_z_index * 2 + 1);
        }

        ++z_index;
    }

    // Re-draw polygons since the current TF as changed.
    this->destroy_tf_polygon(current_piece_view);
    this->create_tf_polygon(current_piece_view);
    this->destroy_tf_polygon(_piece_view);
    this->create_tf_polygon(_piece_view);
    this->build_layer();
}

//-----------------------------------------------------------------------------

std::vector<transfer_function::piece_view*> transfer_function::get_matching_piece_view(
    const sight::viz::scene2d::data::event& _event
) const
{
    // Finds all piece views that match the clicked coord.
    std::vector<piece_view*> matching_piece_view;
    const QPoint scene_pos = QPoint(
        static_cast<int>(_event.get_coord().x),
        static_cast<int>(_event.get_coord().y)
    );
    QList<QGraphicsItem*> items = this->get_scene_2d_render()->get_view()->items(scene_pos);

    // Fills the piece view vector with clicked ones.
    for(piece_view* const piece_view : m_piece_view)
    {
        // Checks if a polygon is clicked.
        if(items.indexOf(piece_view->m_tf_polygons.front()) >= 0)
        {
            matching_piece_view.push_back(piece_view);
        }
    }

    return matching_piece_view;
}

//-----------------------------------------------------------------------------

void transfer_function::process_interaction(sight::viz::scene2d::data::event& _event)
{
    if(!m_interactive)
    {
        return;
    }

    SIGHT_ASSERT("The current TF mustn't be null", m_current_tf);

    // If it's a resize event, all the scene must be recomputed.
    if(_event.type() == sight::viz::scene2d::data::event::resize)
    {
        this->updating();
        _event.set_accepted(true);
        return;
    }

    // If a point as already been captured.
    if(m_captured_tf_point != nullptr)
    {
        if(_event.get_button() == sight::viz::scene2d::data::event::left_button
           && _event.type() == sight::viz::scene2d::data::event::mouse_button_release)
        {
            // Releases capture point.
            this->left_button_release_event();
            _event.set_accepted(true);
            return;
        }
    }

    // If a piece view as already been captured.
    if(m_captured_tf.first)
    {
        if(_event.type() == sight::viz::scene2d::data::event::mouse_move)
        {
            // Changes the piece view level.
            this->mouse_move_on_piece_view_event(_event);
            _event.set_accepted(true);
            return;
        }

        if(_event.get_button() == sight::viz::scene2d::data::event::mid_button
           && _event.type() == sight::viz::scene2d::data::event::mouse_button_release)
        {
            // Releases capture piece view.
            this->mid_button_release_event();
            _event.set_accepted(true);
            return;
        }
    }

    const QPoint scene_pos = QPoint(
        static_cast<int>(_event.get_coord().x),
        static_cast<int>(_event.get_coord().y)
    );
    QList<QGraphicsItem*> items = this->get_scene_2d_render()->get_view()->items(scene_pos);

    // Checks if a point is clicked.
    for(piece_view* const piece_view : m_piece_view)
    {
        for(std::pair<vec2d_t, QGraphicsEllipseItem*>& tf_point : piece_view->m_tf_points)
        {
            // If a point has already been captured.
            if(m_captured_tf_point == &tf_point)
            {
                if(_event.type() == sight::viz::scene2d::data::event::mouse_move)
                {
                    // Moves the captured point.
                    this->mouse_move_on_point_event(piece_view, _event);
                    _event.set_accepted(true);
                    return;
                }
            }
            else if(items.indexOf(tf_point.second) >= 0)
            {
                // If there is a double click on a point, open a color dialog.
                if(_event.get_button() == sight::viz::scene2d::data::event::left_button
                   && _event.type() == sight::viz::scene2d::data::event::mouse_button_double_click)
                {
                    this->left_button_double_click_on_point_event(piece_view, tf_point);
                    _event.set_accepted(true);
                    return;
                }

                // If left button is pressed on a point, set the TF as current.
                if(_event.get_button() == sight::viz::scene2d::data::event::left_button
                   && _event.type() == sight::viz::scene2d::data::event::mouse_button_press)
                {
                    this->left_button_click_on_point_event(piece_view, tf_point);
                    _event.set_accepted(true);
                    return;
                }

                // If right button is pressed on a point, remove it.
                if(_event.get_button() == sight::viz::scene2d::data::event::right_button
                   && _event.type() == sight::viz::scene2d::data::event::mouse_button_press
                   && piece_view->m_tf == m_current_tf && piece_view->m_tf_points.size() > 2)
                {
                    this->right_button_click_on_point_event(piece_view, tf_point);
                    _event.set_accepted(true);
                    return;
                }
            }
        }
    }

    // Adds a new TF point.
    if(_event.get_button() == sight::viz::scene2d::data::event::left_button
       && _event.type() == sight::viz::scene2d::data::event::mouse_button_double_click)
    {
        // Cancel the previous single click interaction.
        m_event_filter->stop();
        this->left_button_double_click_event(_event);
        _event.set_accepted(true);
        return;
    }

    // If left button is pressed, set the nearest TF as current.
    if(_event.get_button() == sight::viz::scene2d::data::event::left_button
       && _event.type() == sight::viz::scene2d::data::event::mouse_button_press)
    {
        // Cancel the previous event if it's needed.
        m_event_filter->stop();
        delete m_event_filter;

        // Validates the event in 250ms, this allow to the double click event to cancel the interaction.
        m_event_filter = new QTimer();
        QTimer::connect(
            m_event_filter,
            &QTimer::timeout,
            this,
            [_event, this]()
            {
                this->left_button_click_event(_event);
            });
        m_event_filter->setSingleShot(true);
        m_event_filter->start(250);
        // _event.setAccepted(true);
        return;
    }

    // If middle button is pressed, select the current TF to adjust the window/level.
    if(_event.get_button() == sight::viz::scene2d::data::event::mid_button
       && _event.type() == sight::viz::scene2d::data::event::mouse_button_press)
    {
        this->mid_button_click_event(_event);
        return;
    }

    // If right button is pressed, open a context menu to manage multiple actions.
    if(_event.get_button() == sight::viz::scene2d::data::event::right_button
       && _event.type() == sight::viz::scene2d::data::event::mouse_button_press)
    {
        this->right_button_c_lick_event(_event);
        _event.set_accepted(true);
        return;
    }

    // If the middle button wheel moves, change the whole piece view opacity.
    if(_event.get_button() == sight::viz::scene2d::data::event::no_button
       && (_event.type() == sight::viz::scene2d::data::event::mouse_wheel_down
           || _event.type() == sight::viz::scene2d::data::event::mouse_wheel_up))
    {
        this->mid_button_wheel_move_event(_event);
        return;
    }
}

//-----------------------------------------------------------------------------

void transfer_function::left_button_click_event(const sight::viz::scene2d::data::event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    std::vector<piece_view*> matching_piece_view = this->get_matching_piece_view(_event);

    if(!matching_piece_view.empty())
    {
        if(matching_piece_view.size() == 1)
        {
            if(matching_piece_view[0]->m_tf != m_current_tf)
            {
                this->set_current_tf(matching_piece_view[0]);
            }
        }
        // Finds the closest one.
        else
        {
            const auto tf = m_tf.lock();

            sight::viz::scene2d::vec2d_t click_coord = this->get_scene_2d_render()->map_to_scene(_event.get_coord());

            float closest_distance             = std::numeric_limits<float>::max();
            piece_view* new_current_piece_view = nullptr;
            for(piece_view* piece_view : matching_piece_view)
            {
                // Finds nearest position of the iterate piece view.
                float local_closest_distance                    = std::numeric_limits<float>::max();
                struct piece_view* local_new_current_piece_view = nullptr;
                for(std::size_t i = 0 ; i <= piece_view->m_tf_points.size() ; ++i)
                {
                    vec2d_t tf_point1;
                    vec2d_t tf_point2;

                    // Creates the first fictional TF point.
                    if(i == 0)
                    {
                        tf_point2 = piece_view->m_tf_points[i].first;
                        if(piece_view->m_tf->clamped())
                        {
                            // The first point is the same a the real first but with a zero alpha channel.
                            tf_point1 = vec2d_t(tf_point2.x, 0);
                        }
                        else
                        {
                            // The first point is the same a the real but with an infinite lower value.

                            const auto viewport = m_viewport.lock();
                            tf_point1 = vec2d_t(viewport->x(), tf_point2.y);
                        }
                    }
                    // Creates the last fictional TF point.
                    else if(i == piece_view->m_tf_points.size())
                    {
                        tf_point1 = piece_view->m_tf_points[i - 1].first;
                        if(piece_view->m_tf->clamped())
                        {
                            // The last point is the same a the real last but with a zero alpha channel.
                            tf_point2 = vec2d_t(tf_point1.x, 0);
                        }
                        else
                        {
                            // The last point is the same a the real but with an infinite upper value.
                            const auto viewport = m_viewport.lock();
                            tf_point2 = vec2d_t(viewport->x() + viewport->width(), tf_point1.y);
                        }
                    }
                    // Retrieves two TF points.
                    else
                    {
                        tf_point1 = piece_view->m_tf_points[i - 1].first;
                        tf_point2 = piece_view->m_tf_points[i].first;
                    }

                    // Gets a line/point projection.
                    const QLineF line(tf_point1.x, tf_point1.y, tf_point2.x, tf_point2.y);

                    QLineF perpendicular_line(click_coord.x, click_coord.y, click_coord.x, 0);
                    perpendicular_line.setAngle(90.F + line.angle());

                    QPointF intersect_point;
                    line.intersects(perpendicular_line, &intersect_point);

                    const QVector2D origin(static_cast<float>(click_coord.x),
                                           static_cast<float>(click_coord.y));

                    float distance = std::numeric_limits<float>::max();

                    // Checks if the intersection belong the segment.
                    if(intersect_point.x() >= tf_point1.x && intersect_point.x() <= tf_point2.x)
                    {
                        const QVector2D intersect(intersect_point);
                        const QVector2D projection = origin - intersect;

                        distance = projection.length();
                    }
                    // Elses the lower distance is between the point and one of the segment edge.
                    else
                    {
                        const QVector2D first_line(static_cast<float>(click_coord.x - tf_point1.x),
                                                   static_cast<float>(click_coord.y - tf_point1.y));
                        const QVector2D second_line(static_cast<float>(click_coord.x - tf_point2.x),
                                                    static_cast<float>(click_coord.y - tf_point2.y));

                        distance = first_line.length();
                        if(second_line.length() < distance)
                        {
                            distance = second_line.length();
                        }
                    }

                    if(distance < local_closest_distance)
                    {
                        local_closest_distance       = distance;
                        local_new_current_piece_view = piece_view;
                    }
                }

                if(local_closest_distance < closest_distance)
                {
                    closest_distance       = local_closest_distance;
                    new_current_piece_view = local_new_current_piece_view;
                }
            }

            SIGHT_ASSERT("newCurrentPieceView is null", new_current_piece_view != nullptr);

            // Sets the new current TF.
            if(new_current_piece_view->m_tf != m_current_tf)
            {
                this->set_current_tf(new_current_piece_view);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void transfer_function::left_button_click_on_point_event(
    piece_view* const _piece_view,
    std::pair<vec2d_t,
              QGraphicsEllipseItem*>& _tf_point
)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Stores the captured TF point in case it's moved.
    m_captured_tf_point = &_tf_point;

    // Sets the selected point pen to lighter to get a visual feedback that the selected point is selected.
    const QColor color = _tf_point.second->brush().color();
    QPen tf_point_pen(color);
    tf_point_pen.setCosmetic(true);
    _tf_point.second->setPen(tf_point_pen);

    // Sets the new current TF.
    if(_piece_view->m_tf != m_current_tf)
    {
        this->set_current_tf(_piece_view);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::mouse_move_on_point_event(
    piece_view* const _piece_view,
    const sight::viz::scene2d::data::event& _event
)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();

    // m_capturedTFPoint must be previously sets by
    // leftButtonClickOnPointEvent(PieceView* const, std::pair< vec2d_t, QGraphicsEllipseItem* >&)
    SIGHT_ASSERT("The captured TF point must exist", m_captured_tf_point);

    const auto point_it = std::find(
        _piece_view->m_tf_points.begin(),
        _piece_view->m_tf_points.end(),
        *m_captured_tf_point
    );
    SIGHT_ASSERT("The captured point is not found", point_it != _piece_view->m_tf_points.end());

    // Gets the previous point of the TF.
    auto previous_point = point_it;
    if(*m_captured_tf_point != _piece_view->m_tf_points.front())
    {
        --previous_point;
    }

    // Gets the next point of the TF.
    auto next_point = point_it;
    if(*m_captured_tf_point != _piece_view->m_tf_points.back())
    {
        ++next_point;
    }

    // Gets position informations of the previous and the next point.
    const double previous_point_x_coord = previous_point->first.x;
    const double next_point_x_coord     = next_point->first.x;

    // Gets the actual mouse point coordinates.
    sight::viz::scene2d::vec2d_t new_coord = this->get_scene_2d_render()->map_to_scene(_event.get_coord(), true);

    // Clamps new y coord between -1 and 0.
    new_coord.y = std::clamp(new_coord.y, -1., 0.);

    // Clamps new coord in the viewport.
    {
        auto viewport = m_viewport.lock();
        new_coord = glm::clamp(
            new_coord,
            glm::dvec2(viewport->left(), viewport->top()),
            glm::dvec2(viewport->right(), viewport->bottom())
        );
    }

    // Clamps new x coord between the previous and the next one.
    const double delta = 1.;
    if(*m_captured_tf_point == _piece_view->m_tf_points.front())
    {
        if(new_coord.x >= next_point_x_coord)
        {
            new_coord.x = next_point_x_coord - delta;
        }
    }
    else if(*m_captured_tf_point == _piece_view->m_tf_points.back())
    {
        if(new_coord.x <= previous_point_x_coord)
        {
            new_coord.x = previous_point_x_coord + delta;
        }
    }
    else
    {
        if(new_coord.x <= previous_point_x_coord)
        {
            new_coord.x = previous_point_x_coord + delta;
        }
        else if(new_coord.x >= next_point_x_coord)
        {
            new_coord.x = next_point_x_coord - delta;
        }
    }

    // Moves the selected TF point by the difference between the old coordinates and the new ones.
    m_captured_tf_point->second->moveBy(
        new_coord.x - m_captured_tf_point->first.x,
        new_coord.y - m_captured_tf_point->first.y
    );

    // Stores new coordinates to the captured one.
    m_captured_tf_point->first.x = new_coord.x;
    m_captured_tf_point->first.y = new_coord.y;

    // Re-draw the current polygons.
    this->destroy_tf_polygon(_piece_view);
    this->create_tf_polygon(_piece_view);
    this->build_layer();

    // Updates the TF with the new point position.
    std::size_t point_index = std::size_t(point_it - _piece_view->m_tf_points.begin());

    // If the window is negative, the TF point list is reversed compared to the TF data.
    if(_piece_view->m_tf->window() < 0)
    {
        point_index = _piece_view->m_tf_points.size() - 1 - point_index;
    }

    const data::transfer_function_piece::sptr tf_piece = _piece_view->m_tf;

    // Retrieves the TF point.
    auto tf_data_it = tf_piece->cbegin();
    for(unsigned i = 0 ; i < point_index ; ++i)
    {
        tf_data_it++;
    }

    // Gets the TF point information
    data::transfer_function::value_t old_tf_value = tf_data_it->first;
    data::transfer_function::color_t color        = tf_data_it->second;

    // Gets new window/level min max value in the window/level space.
    const double min = _piece_view->m_tf_points.begin()->first.x;
    const double max = _piece_view->m_tf_points.rbegin()->first.x;

    // Computes TF value from window/level space to TF space.
    const data::transfer_function::value_t new_tf_value = tf_piece->map_value_from_window(new_coord.x);

    // Removes the old TF point.
    tf_piece->erase(old_tf_value);

    // Updates the color alpha channel.
    color.a = std::abs(new_coord.y);

    // Adds the new TF point.
    tf_piece->insert({new_tf_value, color});

    // Updates the window/level.
    if(tf_piece->window() > 0)
    {
        tf_piece->set_window_min_max(data::transfer_function::min_max_t(min, max));
    }
    else
    {
        tf_piece->set_window_min_max(data::transfer_function::min_max_t(max, min));
    }

    tf->fit_window();

    points_modified(*tf);
}

//-----------------------------------------------------------------------------

void transfer_function::left_button_release_event()
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Removes the highlighting of the captured point.
    m_captured_tf_point->second->setPen(m_points_pen);
    m_captured_tf_point = nullptr;
}

//-----------------------------------------------------------------------------

void transfer_function::right_button_click_on_point_event(
    piece_view* const _piece_view,
    std::pair<vec2d_t, QGraphicsEllipseItem*>& _tf_point
)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();

    // Updates the TF.
    auto point_it = std::find(_piece_view->m_tf_points.begin(), _piece_view->m_tf_points.end(), _tf_point);
    SIGHT_ASSERT("The captured point is not found", point_it != _piece_view->m_tf_points.end());
    std::size_t point_index = std::size_t(point_it - _piece_view->m_tf_points.begin());

    const data::transfer_function_piece::sptr tf_piece = _piece_view->m_tf;
    {
        // If the window is negative, the TF point list is reversed compared to the TF data.
        const double window = tf_piece->window();
        if(window <= 0)
        {
            point_index = _piece_view->m_tf_points.size() - 1 - point_index;
        }

        // Retrieves the TF point.
        auto tf_data_it = tf_piece->cbegin();
        for(unsigned i = 0 ; i < point_index ; ++i)
        {
            tf_data_it++;
        }

        // Removes the TF point.
        const data::transfer_function::value_t tf_value = tf_data_it->first;
        tf_piece->erase(tf_value);

        // Gets new window/level min max value in the window/level space.
        double min = _piece_view->m_tf_points.begin()->first.x;
        double max = _piece_view->m_tf_points.rbegin()->first.x;

        // If the removed point is the last or the first, the min max is wrong and need to be updated.
        if((point_index == 0 && window >= 0) || (point_index == _piece_view->m_tf_points.size() - 1 && window < 0))
        {
            min = (_piece_view->m_tf_points.begin() + 1)->first.x;
        }
        else if((point_index == _piece_view->m_tf_points.size() - 1 && window >= 0) || (point_index == 0 && window < 0))
        {
            max = (_piece_view->m_tf_points.rbegin() + 1)->first.x;
        }

        // Updates the window/level.
        if(window > 0)
        {
            tf_piece->set_window_min_max(data::transfer_function::min_max_t(min, max));
        }
        else
        {
            tf_piece->set_window_min_max(data::transfer_function::min_max_t(max, min));
        }
    }

    tf->fit_window();

    points_modified(*tf);

    this->get_scene_2d_render()->get_scene()->removeItem(point_it->second);
    delete point_it->second;
    _piece_view->m_tf_points.erase(point_it);

    // Re-draw the current polygons.
    this->destroy_tf_polygon(_piece_view);
    this->create_tf_polygon(_piece_view);
    this->build_layer();
}

//-----------------------------------------------------------------------------

void transfer_function::left_button_double_click_on_point_event(
    piece_view* const _piece_view,
    std::pair<vec2d_t, QGraphicsEllipseItem*>& _tf_point
)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    QColor old_color;
    {
        // Lock in a scope to avoid potential dead_locks because of the exec() implied by QColorDialog static function
        const auto tf = m_tf.lock();

        // Opens a QColorDialog with the selected circle color and the tf point alpha as default rgba color.
        old_color = _tf_point.second->brush().color();
        old_color.setAlphaF(-_tf_point.first.y);
    }

    QColor new_color = QColorDialog::getColor(
        old_color,
        this->get_scene_2d_render()->get_view(),
        QString("Choose the point color"),
        QColorDialog::ShowAlphaChannel
    );

    if(new_color.isValid())
    {
        // Updates the TF.
        auto point_it =
            std::find(_piece_view->m_tf_points.begin(), _piece_view->m_tf_points.end(), _tf_point);
        SIGHT_ASSERT("The captured point is not found", point_it != _piece_view->m_tf_points.end());
        std::size_t point_index = std::size_t(point_it - _piece_view->m_tf_points.begin());

        const auto tf_piece = _piece_view->m_tf;
        {
            // If the window is negative, the TF point list is reversed compared to the TF data.
            if(tf_piece->window() < 0)
            {
                point_index = _piece_view->m_tf_points.size() - 1 - point_index;
            }

            // Retrieves the TF point.
            auto tf_data_it = tf_piece->cbegin();
            for(unsigned i = 0 ; i < point_index ; ++i)
            {
                tf_data_it++;
            }

            // Removes the TF point.
            data::transfer_function::value_t tf_value = tf_data_it->first;
            tf_piece->erase(tf_value);

            // Adds the new one with the new color.
            data::transfer_function::color_t color_t(new_color.red() / 255.,
                                                     new_color.green() / 255.,
                                                     new_color.blue() / 255.,
                                                     old_color.alpha() / 255.);
            tf_piece->insert({tf_value, color_t});
        }

        const auto tf = m_tf.lock();
        tf->fit_window();

        points_modified(*tf);

        // Updates the displayed TF point.
        new_color.setAlpha(255);
        _tf_point.second->setBrush(QBrush(new_color));

        // Re-draw the current polygons.
        this->destroy_tf_polygon(_piece_view);
        this->create_tf_polygon(_piece_view);
        this->build_layer();
    }
}

//-----------------------------------------------------------------------------

void transfer_function::left_button_double_click_event(const sight::viz::scene2d::data::event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    sight::viz::scene2d::vec2d_t new_coord = this->get_scene_2d_render()->map_to_scene(_event.get_coord());

    new_coord.y = std::clamp(new_coord.y, -1., 0.);
    {
        const auto tf = m_tf.lock();

        // Finds the current piece view.
        SIGHT_ASSERT("The current TF mustn't be null", m_current_tf);
        piece_view* const current_piece_view = *(std::find_if(
                                                     m_piece_view.begin(),
                                                     m_piece_view.end(),
                                                     [&](const piece_view* _piece_view)
            {
                return _piece_view->m_tf == m_current_tf;
            }));
        const auto tf_piece = current_piece_view->m_tf;
        {
            data::transfer_function::color_t new_color;

            // The new coord becomes the new first TF point, get the current first color in the list.
            if(new_coord.x < current_piece_view->m_tf_points.front().first.x)
            {
                const QColor first_color = current_piece_view->m_tf_points.front().second->brush().color();
                new_color = data::transfer_function::color_t(
                    first_color.red() / 255.,
                    first_color.green() / 255.,
                    first_color.blue() / 255.,
                    -new_coord.y
                );
            }
            // The new coord becomes the new last TF point, get the current last color in the list.
            else if(new_coord.x > current_piece_view->m_tf_points.back().first.x)
            {
                const QColor first_color = current_piece_view->m_tf_points.back().second->brush().color();
                new_color = data::transfer_function::color_t(
                    first_color.red() / 255.,
                    first_color.green() / 255.,
                    first_color.blue() / 255.,
                    -new_coord.y
                );
            }
            // Gets an interpolate color since the new point is between two others.
            else
            {
                new_color   = tf_piece->sample(new_coord.x);
                new_color.a = -new_coord.y;
            }

            // Adds the new TF point.
            const data::transfer_function::value_t tf_value = tf_piece->map_value_from_window(new_coord.x);
            tf_piece->insert({tf_value, new_color});

            // Gets new window/level min max value in the window/level space.
            const double min = std::min(current_piece_view->m_tf_points.begin()->first.x, new_coord.x);
            const double max = std::max(current_piece_view->m_tf_points.rbegin()->first.x, new_coord.x);

            // Updates the window/level.
            if(tf_piece->window() > 0)
            {
                tf_piece->set_window_min_max(data::transfer_function::min_max_t(min, max));
            }
            else
            {
                tf_piece->set_window_min_max(data::transfer_function::min_max_t(max, min));
            }
        }

        tf->fit_window();

        points_modified(*tf);
    }

    // Re-draw all the scene.
    this->updating();
}

//-----------------------------------------------------------------------------

void transfer_function::mid_button_click_event(sight::viz::scene2d::data::event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();

    // Finds all piece views that match the clicked coord.
    std::vector<piece_view*> matching_piece_view = this->get_matching_piece_view(_event);

    // Checks if the current tf is in the matching list.
    const auto matching_it = std::find_if(
        matching_piece_view.begin(),
        matching_piece_view.end(),
        [&](const piece_view* _piece_view)
        {
            return _piece_view->m_tf == m_current_tf;
        });

    // Moves the window/level only if the mouse if over the current TF.
    if(matching_it != matching_piece_view.end())
    {
        this->get_scene_2d_render()->get_view()->setCursor(Qt::ClosedHandCursor);
        sight::viz::scene2d::vec2d_t window_level_coord = this->get_scene_2d_render()->map_to_scene(_event.get_coord());
        // Stores the level in window/level space and the window in screen space.
        m_captured_tf = std::make_pair(
            (*matching_it)->m_tf,
            sight::viz::scene2d::vec2d_t(window_level_coord.x, _event.get_coord().y)
        );
        _event.set_accepted(true);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::mouse_move_on_piece_view_event(const sight::viz::scene2d::data::event& _event)
{
    // m_capturedTF must be previously sets by midButtonClickEvent(const sight::viz::scene2d::data::Event& _event)
    SIGHT_ASSERT("The captured pieceView must exist", m_captured_tf.first);

    sight::viz::scene2d::vec2d_t window_level_coord = this->get_scene_2d_render()->map_to_scene(_event.get_coord());

    const auto min_max = m_captured_tf.first->min_max();
    const auto min     = m_captured_tf.first->map_value_to_window(min_max.first);
    const auto max     = m_captured_tf.first->map_value_to_window(min_max.second);

    // Prevent the whole curve to move outside the viewport.
    {
        auto viewport = m_viewport.lock();

        window_level_coord = glm::clamp(
            window_level_coord,
            glm::dvec2(viewport->left() + (m_captured_tf.second.x - min), viewport->top()),
            glm::dvec2(viewport->right() - (max - m_captured_tf.second.x), viewport->bottom())
        );
    }

    // The level delta is in window/level space.
    const double level_delta = window_level_coord.x - m_captured_tf.second.x;

    // The window delta is in screen space.
    const double window_delta = _event.get_coord().y - m_captured_tf.second.y;

    // Updates the TF.
    const data::transfer_function_piece::sptr tf_piece = m_captured_tf.first;
    {
        const auto tf = m_tf.lock();

        tf_piece->set_window(tf_piece->window() - window_delta);
        tf_piece->set_level(tf_piece->level() + level_delta);

        tf->fit_window();

        // Sends the signal.
        const auto sig = tf->signal<data::transfer_function::windowing_modified_signal_t>(
            data::transfer_function::WINDOWING_MODIFIED_SIG
        );
        {
            const core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit(tf->window(), tf->level());
        }
    }

    // Stores the level in window/level space and the window in screen space.
    m_captured_tf.second = sight::viz::scene2d::vec2d_t(window_level_coord.x, _event.get_coord().y);

    // Re-draw all the scene.
    this->updating();
}

//-----------------------------------------------------------------------------

void transfer_function::mid_button_release_event()
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    this->get_scene_2d_render()->get_view()->setCursor(Qt::ArrowCursor);
    m_captured_tf.first = nullptr;
}

//-----------------------------------------------------------------------------

void transfer_function::right_button_c_lick_event(const sight::viz::scene2d::data::event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Finds all piece views that match the clicked coord.
    std::vector<piece_view*> matching_piece_view = this->get_matching_piece_view(_event);

    // Creates the menu.
    auto* const trapeze_action = new QAction("Add trapeze");
    QObject::connect(
        trapeze_action,
        &QAction::triggered,
        this,
        [_event, this]()
        {
            this->add_trapeze(_event);
        });
    auto* const left_ramp_action = new QAction("Add left ramp");
    QObject::connect(
        left_ramp_action,
        &QAction::triggered,
        this,
        [_event, this]()
        {
            this->add_left_ramp(_event);
        });
    auto* const right_ramp_action = new QAction("Add right ramp");
    QObject::connect(
        right_ramp_action,
        &QAction::triggered,
        this,
        [_event, this]()
        {
            this->add_right_ramp(_event);
        });

    auto* const context_menu = new QMenu();
    {
        // Checks if the current tf is in the matching list.
        const auto matching_it = std::find_if(
            matching_piece_view.begin(),
            matching_piece_view.end(),
            [&](const piece_view* _piece_view)
            {
                return _piece_view->m_tf == m_current_tf;
            });

        // Adds the delete action if the current TF is clicked.
        if(matching_it != matching_piece_view.end())
        {
            {
                const auto tf = m_tf.const_lock();
                // Adds the delete action if there is more than one TF.
                if(tf->pieces().size() > 1)
                {
                    auto* delete_action = new QAction("Delete");
                    QObject::connect(
                        delete_action,
                        &QAction::triggered,
                        this,
                        &transfer_function::remove_current_tf
                    );
                    context_menu->addAction(delete_action);
                }
            }

            // Adds the clamp action.
            auto* const clamp_action = new QAction("Clamp");
            clamp_action->setCheckable(true);
            clamp_action->setChecked(m_current_tf->clamped());
            QObject::connect(clamp_action, &QAction::triggered, this, &transfer_function::clamp_current_tf);
            context_menu->addAction(clamp_action);

            // Adds the interpolation mode action.
            auto* const linear_action = new QAction("Linear");
            linear_action->setCheckable(true);
            linear_action->setChecked(
                m_current_tf->get_interpolation_mode() == data::transfer_function::interpolation_mode::linear
            );
            QObject::connect(linear_action, &QAction::triggered, this, &transfer_function::toggle_linear_current_tf);
            context_menu->addAction(linear_action);
        }
    }

    context_menu->addAction(trapeze_action);
    context_menu->addAction(left_ramp_action);
    context_menu->addAction(right_ramp_action);

    // Opens the menu.
    context_menu->exec(QCursor::pos());

    delete context_menu;
}

//-----------------------------------------------------------------------------

void transfer_function::mid_button_wheel_move_event(sight::viz::scene2d::data::event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Finds all piece views that match the current coord.
    std::vector<piece_view*> matching_piece_view = this->get_matching_piece_view(_event);

    // Checks if the current tf is in the matching list.
    const auto matching_it = std::find_if(
        matching_piece_view.begin(),
        matching_piece_view.end(),
        [&](const piece_view* _piece_view)
        {
            return _piece_view->m_tf == m_current_tf;
        });

    // Change the opacity only if the mouse if over the current TF.
    if(matching_it != matching_piece_view.end())
    {
        data::transfer_function_piece::sptr tf_piece;
        {
            const auto tf      = m_tf.lock();
            const auto& pieces = tf->pieces();

            SIGHT_ASSERT(
                "inout '" + std::string(CURRENT_TF_INOUT) + "' must have at least on TF inside.",
                !tf->pieces().empty()
            );

            // Finds the tf
            SIGHT_ASSERT("The current TF mustn't be null", m_current_tf);
            tf_piece = *std::find_if(pieces.begin(), pieces.end(), [&](const auto& _p){return _p == m_current_tf;});

            // Updates the current TF.
            const double scale = _event.type() == sight::viz::scene2d::data::event::mouse_wheel_down ? 0.9 : 1.1;

            // Scale data
            for(auto& data : *tf_piece)
            {
                data.second.a = data.second.a * scale;
            }

            tf->fit_window();

            points_modified(*tf);
        }

        // Re-draw all the scene.
        this->updating();

        _event.set_accepted(true);
    }
}

//-----------------------------------------------------------------------------

void transfer_function::remove_current_tf()
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);
    {
        SIGHT_ASSERT("The current TF mustn't be null", m_current_tf);

        const auto tf = m_tf.lock();
        SIGHT_ASSERT("Transfer function is not set.", tf);
        SIGHT_ASSERT("Transfer function must have more than one TF piece inside.", tf->pieces().size() > 1);

        auto& pieces = tf->pieces();
        std::erase_if(pieces, [&](const auto& _piece){return _piece == m_current_tf;});

        // Sets the new current TF.
        m_current_tf = pieces.front();
        tf->fit_window();
        // Block notifier
        auto sig = tf->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        const core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        {
            sig->async_emit();
        }
    }

    // Re-draw all the scene here since swapping method as not been called.
    this->updating();
}

//-----------------------------------------------------------------------------

void transfer_function::clamp_current_tf(bool _clamp)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();
    SIGHT_ASSERT("Transfer function is not set.", tf);

    const auto& pieces = tf->pieces();
    auto tf_piece      = *std::find_if(pieces.begin(), pieces.end(), [&](const auto& _p){return _p == m_current_tf;});

    tf_piece->set_clamped(_clamp);
    tf->fit_window();

    points_modified(*tf);

    piece_view* current_piece_view = *(std::find_if(
                                           m_piece_view.begin(),
                                           m_piece_view.end(),
                                           [&](const piece_view* _piece_view)
        {
            return _piece_view->m_tf == m_current_tf;
        }));

    // Re-draw the current polygons.
    this->destroy_tf_polygon(current_piece_view);
    this->create_tf_polygon(current_piece_view);
    this->build_layer();
}

//-----------------------------------------------------------------------------

void transfer_function::toggle_linear_current_tf(bool _linear)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto tf = m_tf.lock();
    SIGHT_ASSERT("Transfer function is not set.", tf);

    const auto& pieces = tf->pieces();
    auto tf_piece      = *std::find_if(pieces.begin(), pieces.end(), [&](const auto& _p){return _p == m_current_tf;});

    tf_piece->set_interpolation_mode(
        _linear ? data::transfer_function::interpolation_mode::linear
                : data::transfer_function::interpolation_mode::nearest
    );
    tf->fit_window();
    points_modified(*tf);

    piece_view* current_piece_view = *(std::find_if(
                                           m_piece_view.begin(),
                                           m_piece_view.end(),
                                           [&](const piece_view* _piece_view)
        {
            return _piece_view->m_tf == m_current_tf;
        }));

    // Re-draw the current polygons.
    this->destroy_tf_polygon(current_piece_view);
    this->create_tf_polygon(current_piece_view);
    this->build_layer();
}

//-----------------------------------------------------------------------------

void transfer_function::add_new_tf(const data::transfer_function_piece::sptr _tf)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    {
        const auto tf = m_tf.lock();
        SIGHT_ASSERT("inout '" + std::string(CURRENT_TF_INOUT) + "' does not exist.", tf);

        // Adds the new TF.
        tf->pieces().push_back(_tf);
        tf->fit_window();
        // Block notifier
        auto sig = tf->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        const core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        {
            sig->async_emit();
        }
    }

    // Creates the new piece view.
    piece_view* new_piece_view = this->create_piece_view(_tf, 0);

    // Pushes the piece view to the vector.
    m_piece_view.push_back(new_piece_view);

    this->create_tf_polygon(new_piece_view);
    this->build_layer();

    // Updates the current TF.
    this->set_current_tf(new_piece_view);
}

//-----------------------------------------------------------------------------

void transfer_function::add_left_ramp(const sight::viz::scene2d::data::event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    const auto left_ramp = std::make_shared<data::transfer_function_piece>();
    left_ramp->insert({0.0, data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0)});
    left_ramp->insert({1.0, data::transfer_function::color_t()});
    left_ramp->set_clamped(false);
    left_ramp->set_window(500.);
    left_ramp->set_level(50.);

    // Updates the window/level.
    sight::viz::scene2d::vec2d_t new_coord        = this->get_scene_2d_render()->map_to_scene(_event.get_coord());
    const data::transfer_function::value_t window = left_ramp->window();
    data::transfer_function::value_t min          = new_coord.x - window / 2.;
    data::transfer_function::value_t max          = min + window;
    left_ramp->set_window_min_max(data::transfer_function::min_max_t(min, max));

    this->add_new_tf(left_ramp);
}

//-----------------------------------------------------------------------------

void transfer_function::add_right_ramp(const sight::viz::scene2d::data::event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Creates the new TF.
    const auto right_ramp = std::make_shared<data::transfer_function_piece>();
    right_ramp->insert({0.0, data::transfer_function::color_t()});
    right_ramp->insert({1.0, data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0)});
    right_ramp->set_clamped(false);
    right_ramp->set_window(500.);
    right_ramp->set_level(50.);

    // Updates the window/level.
    sight::viz::scene2d::vec2d_t new_coord        = this->get_scene_2d_render()->map_to_scene(_event.get_coord());
    const data::transfer_function::value_t window = right_ramp->window();
    data::transfer_function::value_t min          = new_coord.x - window / 2.;
    data::transfer_function::value_t max          = min + window;
    right_ramp->set_window_min_max(data::transfer_function::min_max_t(min, max));

    this->add_new_tf(right_ramp);
}

//-----------------------------------------------------------------------------

void transfer_function::add_trapeze(const sight::viz::scene2d::data::event& _event)
{
    SIGHT_ASSERT("Interactions disabled, this code should not reached", m_interactive);

    // Creates the new TF.
    const auto trapeze = std::make_shared<data::transfer_function_piece>();
    trapeze->insert({0.0, data::transfer_function::color_t()});
    trapeze->insert({1. / 3., data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0)});
    trapeze->insert({2. / 3., data::transfer_function::color_t(1.0, 1.0, 1.0, 1.0)});
    trapeze->insert({1.0, data::transfer_function::color_t()});
    trapeze->set_clamped(true);
    trapeze->set_window(500.);
    trapeze->set_level(50.);

    // Updates the window/level.
    sight::viz::scene2d::vec2d_t new_coord        = this->get_scene_2d_render()->map_to_scene(_event.get_coord());
    const data::transfer_function::value_t window = trapeze->window();
    data::transfer_function::value_t min          = new_coord.x - window / 2.;
    data::transfer_function::value_t max          = min + window;
    trapeze->set_window_min_max(data::transfer_function::min_max_t(min, max));

    this->add_new_tf(trapeze);
}

//------------------------------------------------------------------------------

void transfer_function::update_tf()
{
    // Sets the current TF.
    {
        // Sets the current TF used to highlight it in the editor.
        const auto tf = m_tf.const_lock();
        if(tf && !tf->pieces().empty())
        {
            m_current_tf = tf->pieces().front();
        }
        else
        {
            SIGHT_FATAL("The current TF mustn't be null");
        }
    }

    updating();
}

//------------------------------------------------------------------------------

void transfer_function::points_modified(const sight::data::transfer_function& _tf) const
{
    // Sends the modification signal.
    const auto sig_tf = _tf.signal<data::transfer_function::points_modified_signal_t>(
        data::transfer_function::POINTS_MODIFIED_SIG
    );

    const core::com::connection::blocker block1(sig_tf->get_connection(slot(service::slots::UPDATE)));
    sig_tf->async_emit();
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene2d::adaptor
