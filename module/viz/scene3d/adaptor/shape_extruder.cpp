/************************************************************************
 *
 * Copyright (C) 2020-2025 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include "module/viz/scene3d/adaptor/shape_extruder.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/mesh.hpp>
#include <data/reconstruction.hpp>
#include <data/tools/color.hpp>

#include <geometry/data/mesh.hpp>

#include <viz/scene3d/ogre.hpp>

#include <OgreCamera.h>

namespace sight::module::viz::scene3d::adaptor
{

shape_extruder::triangle2_d::triangle2_d(
    const Ogre::Vector2& _a,
    const Ogre::Vector2& _b,
    const Ogre::Vector2& _c
) :
    a(_a),
    b(_b),
    c(_c),
    id(s_id++)
{
    // Matrix to rotate of 90 degree over the Z axis.
    const Ogre::Matrix3 quarter_rotation {0, -1, 0, 1, 0, 0, 0, 0, 1};

    // Compute the circumscribed circle of the triangle.
    Ogre::Vector2 first_bisector_pos;
    Ogre::Vector2 first_bisector_dir;

    if(a.y != b.y)
    {
        first_bisector_pos = (a + b) / 2.F;
        const Ogre::Vector2 first_edge_dir = (a - b).normalisedCopy();
        first_bisector_dir = (quarter_rotation * Ogre::Vector3(first_edge_dir.x, first_edge_dir.y, 0)).xy();
    }
    else
    {
        first_bisector_pos = (c + b) / 2.F;
        const Ogre::Vector2 first_edge_dir = (c - b).normalisedCopy();
        first_bisector_dir = (quarter_rotation * Ogre::Vector3(first_edge_dir.x, first_edge_dir.y, 0)).xy();
    }

    Ogre::Vector2 second_bisector_pos;
    Ogre::Vector2 second_bisector_dir;

    if(a.y != c.y)
    {
        second_bisector_pos = (a + c) / 2.F;
        Ogre::Vector2 second_edge_dir = (a - c).normalisedCopy();
        second_bisector_dir =
            (quarter_rotation * Ogre::Vector3(second_edge_dir.x, second_edge_dir.y, 0)).xy();
    }
    else
    {
        second_bisector_pos = (b + c) / 2.F;
        Ogre::Vector2 second_edge_dir = (b - c).normalisedCopy();
        second_bisector_dir =
            (quarter_rotation * Ogre::Vector3(second_edge_dir.x, second_edge_dir.y, 0)).xy();
    }

    const float a_prim = first_bisector_dir.y / first_bisector_dir.x;
    const float a_sec  = second_bisector_dir.y / second_bisector_dir.x;

    const float b_prim = first_bisector_pos.y - a_prim * first_bisector_pos.x;
    const float b_sec  = second_bisector_pos.y - a_sec * second_bisector_pos.x;

    const float intersect_x = (b_sec - b_prim) / (a_prim - a_sec);
    const float intersect_y = a_prim * intersect_x + b_prim;

    center = Ogre::Vector2(intersect_x, intersect_y);
    radius = a.distance(center);

    const Ogre::Vector2 bary_dir(((a + b) / 2.F) - c);
    barycentre = c + 2.F / 3.F * bary_dir;
}

//------------------------------------------------------------------------------

bool shape_extruder::edge::intersect(edge _edge) const
{
    const Ogre::Vector2 p = a;
    const Ogre::Vector2 r = (b - a);

    const Ogre::Vector2 q = _edge.a;
    const Ogre::Vector2 s = (_edge.b - _edge.a);

    const Ogre::Vector2 qp = (q - p);
    const float qp_xs      = (qp.x * s.y - qp.y * s.x);
    const float qp_xr      = (qp.x * r.y - qp.y * r.x);
    const float r_xs       = (r.x * s.y - r.y * s.x);
    const float t          = qp_xs / r_xs;
    const float u          = qp_xr / r_xs;

    return r_xs != 0 && t >= 0 && t < 1 && u >= 0 && u < 1;
}

//------------------------------------------------------------------------------

Ogre::Vector3 shape_extruder::get_cam_direction(const Ogre::Camera* const _cam)
{
    const Ogre::Matrix4 view = _cam->getViewMatrix();
    Ogre::Vector3 direction(view[2][0], view[2][1], view[2][2]);
    direction.normalise();
    return -direction;
}

//-----------------------------------------------------------------------------

shape_extruder::shape_extruder() noexcept :
    service::notifier(m_signals)
{
    new_slot(slots::ENABLE_TOOL, &shape_extruder::enable_tool, this);
    new_slot(slots::DELETE_LAST_MESH, &shape_extruder::delete_last_mesh, this);
    new_slot(slots::CANCEL_LAST_CLICK, &shape_extruder::cancel_last_click, this);
    new_slot(slots::UNDO, &shape_extruder::undo, this);
    new_slot(slots::RESET, &shape_extruder::reset, this);
    new_slot(slots::VALIDATE, &shape_extruder::validate, this);

    new_signal<signals::tool_disabled_signal_t>(signals::TOOL_DISABLED);
}

//-----------------------------------------------------------------------------

void shape_extruder::configuring()
{
    this->configure_params();

    const config_t config = this->get_config();

    static const std::string s_PRIORITY_CONFIG         = CONFIG + "priority";
    static const std::string s_EXTRUDE_CONFIG          = CONFIG + "extrude";
    static const std::string s_LINE_COLOR_CONFIG       = CONFIG + "lineColor";
    static const std::string s_EDGE_COLOR_CONFIG       = CONFIG + "edgeColor";
    static const std::string s_DOUBLE_CLICK_VALIDATION = CONFIG + "validation_on_double_click";

    m_priority                   = config.get<int>(s_PRIORITY_CONFIG, m_priority);
    m_extrude                    = config.get<bool>(s_EXTRUDE_CONFIG, m_extrude);
    m_validation_by_double_click = config.get<bool>(s_DOUBLE_CLICK_VALIDATION, m_validation_by_double_click);

    const auto divide_by255 = [](auto&& _p_h1, auto&& ...)
                              {
                                  return std::divides<>()(std::forward<decltype(_p_h1)>(_p_h1), 255.F);
                              };

    const auto hexa_line_color = config.get<std::string>(s_LINE_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> line_color {};
    data::tools::color::hexa_string_to_rgba(hexa_line_color, line_color);
    std::transform(line_color.begin(), line_color.end(), m_line_color.ptr(), divide_by255);

    const auto hexa_edge_color = config.get<std::string>(s_EDGE_COLOR_CONFIG, "#FFFFFF");
    std::array<std::uint8_t, 4> edge_color {};
    data::tools::color::hexa_string_to_rgba(hexa_edge_color, edge_color);
    std::transform(edge_color.begin(), edge_color.end(), m_edge_color.ptr(), divide_by255);
}

//-----------------------------------------------------------------------------

void shape_extruder::starting()
{
    adaptor::init();

    this->render_service()->make_current();

    // Add the interactor to the layer.
    const sight::viz::scene3d::layer::sptr layer = this->layer();

    const sight::viz::scene3d::interactor::base::sptr interactor =
        std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->add_interactor(interactor, m_priority);

    // Create entities.
    Ogre::SceneManager* const scene_mng = this->get_scene_manager();

    m_lasso_node = scene_mng->getRootSceneNode()->createChildSceneNode(gen_id("lassoNode"));

    m_lasso           = scene_mng->createManualObject(gen_id("lasso"));
    m_last_lasso_line = scene_mng->createManualObject(gen_id("lastLassoLine"));

    m_lasso_node->attachObject(m_lasso);
    m_lasso_node->attachObject(m_last_lasso_line);

    // Create the material.
    m_material = std::make_unique<sight::viz::scene3d::material::standard>(gen_id("material"));
    m_material->set_layout(sight::data::mesh::attribute::point_colors);
    m_material->set_shading(sight::data::material::shading_t::ambient, this->layer()->num_lights());
}

//-----------------------------------------------------------------------------

void shape_extruder::updating()
{
}

//-----------------------------------------------------------------------------

void shape_extruder::stopping()
{
    this->render_service()->make_current();

    // Destroy the material.
    m_material.reset();

    // Destroy entities.
    Ogre::SceneManager* const scene_mng = this->get_scene_manager();

    scene_mng->destroyManualObject(m_last_lasso_line);
    scene_mng->destroyManualObject(m_lasso);

    scene_mng->destroySceneNode(m_lasso_node);

    // Remove the interactor from the layer.
    const sight::viz::scene3d::layer::sptr layer = this->layer();

    const sight::viz::scene3d::interactor::base::sptr interactor =
        std::dynamic_pointer_cast<sight::viz::scene3d::interactor::base>(this->get_sptr());
    layer->remove_interactor(interactor);

    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void shape_extruder::enable_tool(bool _enable)
{
    this->render_service()->make_current();

    m_tool_enable_state = _enable;

    // Stop the lasso interaction.
    m_interaction_enable_state = false;

    // Clear entities.
    m_lasso_tool_positions.clear();
    m_lasso_near_positions.clear();
    m_lasso_far_positions.clear();
    m_lasso_edge_positions.clear();
    m_lasso->clear();
    m_last_lasso_line->clear();

    // Send a render request.
    this->request_render();
}

//-----------------------------------------------------------------------------

void shape_extruder::delete_last_mesh()
{
    // Get the reconstruction list.
    const auto extruded_meshes = m_extruded_meshes.lock();

    data::model_series::reconstruction_vector_t reconstructions = extruded_meshes->get_reconstruction_db();

    if(!reconstructions.empty())
    {
        reconstructions.pop_back();
        extruded_meshes->set_reconstruction_db(reconstructions);

        // Send notification
        this->notifier::info("Last extrusion deleted.");

        // Send the signal.
        auto sig = extruded_meshes->signal<data::model_series::reconstructions_removed_signal_t>(
            data::model_series::RECONSTRUCTIONS_REMOVED_SIG
        );
        sig->async_emit(data::model_series::reconstruction_vector_t {reconstructions});
    }
    else
    {
        this->notifier::failure("No extrusion to delete.");
    }
}

//------------------------------------------------------------------------------

void shape_extruder::cancel_last_click()
{
    modify_lasso(action::remove);
}

//------------------------------------------------------------------------------

void shape_extruder::undo()
{
    if(m_interaction_enable_state)
    {
        cancel_last_click();
    }
    else
    {
        delete_last_mesh();
    }
}

//------------------------------------------------------------------------------

void shape_extruder::reset()
{
    // Get the reconstruction list.
    const auto extruded_meshes = m_extruded_meshes.lock();

    data::model_series::reconstruction_vector_t reconstructions = extruded_meshes->get_reconstruction_db();

    if(!reconstructions.empty())
    {
        reconstructions.clear();
        extruded_meshes->set_reconstruction_db(reconstructions);

        // Send the signal.
        auto sig = extruded_meshes->signal<data::model_series::modified_signal_t>(data::model_series::MODIFIED_SIG);
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

std::tuple<Ogre::Vector3, Ogre::Vector3, Ogre::Vector3> shape_extruder::get_near_far_ray_positions(
    int _x,
    int _y
) const
{
    // Compute the ray.
    sight::viz::scene3d::layer::sptr layer = this->layer();

    const Ogre::Camera* const camera = layer->get_default_camera();
    const Ogre::Viewport* const vp   = camera->getViewport();

    const float vp_x = static_cast<float>(_x - vp->getActualLeft()) / static_cast<float>(vp->getActualWidth());
    const float vp_y = static_cast<float>(_y - vp->getActualTop()) / static_cast<float>(vp->getActualHeight());

    Ogre::Ray ray = camera->getCameraToViewportRay(vp_x, vp_y);

    // Compute the intersection between the ray and the far working plane.
    std::pair<bool, Ogre::Real> far_hit = Ogre::Math::intersects(ray, m_lasso_far_plane);
    SIGHT_ASSERT("The ray must hit the plane", far_hit.first);
    const Ogre::Vector3 far_position = ray.getPoint(far_hit.second);

    // Launch the ray on the near plane after since the ray must be in front or behind it due to the frustum curve.
    std::pair<bool, Ogre::Real> near_hit = Ogre::Math::intersects(ray, m_lasso_near_plane);
    if(!near_hit.first)
    {
        ray.setDirection(-ray.getDirection());
        near_hit = Ogre::Math::intersects(ray, m_lasso_near_plane);
    }

    SIGHT_ASSERT("The ray must hit the plane", near_hit.first);
    const Ogre::Vector3 near_position = ray.getPoint(near_hit.second);

    // Launch the ray on the tool plane.
    std::pair<bool, Ogre::Real> tool_hit = Ogre::Math::intersects(ray, m_lasso_tool_plane);
    if(!tool_hit.first)
    {
        ray.setDirection(-ray.getDirection());
        tool_hit = Ogre::Math::intersects(ray, m_lasso_tool_plane);
    }

    SIGHT_ASSERT("The ray must hit the plane", tool_hit.first);
    const Ogre::Vector3 tool_position = ray.getPoint(tool_hit.second);

    return {tool_position, near_position, far_position};
}

//------------------------------------------------------------------------------

void shape_extruder::modify_lasso(action _action, int _x, int _y)
{
    if(m_tool_enable_state)
    {
        this->render_service()->make_current();

        const sight::viz::scene3d::layer::sptr layer = this->layer();

        // Start the lasso interaction.
        if(!m_interaction_enable_state)
        {
            m_interaction_enable_state = true;

            // Compute the plane where the tool will work.
            // This plane allows to generate all points of the lasso on the same plane to simplify further algorithms.
            const Ogre::Camera* const camera = layer->get_default_camera();
            const Ogre::Vector3 direction    = shape_extruder::get_cam_direction(camera);

            // Compute the near plane and the far plane.
            const Ogre::Vector3 cam_pos = camera->getDerivedPosition();
            m_lasso_tool_plane = Ogre::Plane(direction, cam_pos + direction);
            m_lasso_near_plane = Ogre::Plane(direction, cam_pos + direction * camera->getNearClipDistance());
            m_lasso_far_plane  = Ogre::Plane(direction, cam_pos + direction * camera->getFarClipDistance());
        }

        m_left_button_move_state = false;

        // Cancel others interactions.
        layer->cancel_further_interaction();

        // Get the clicked point in the world space.
        const auto tool_near_far_pos = this->get_near_far_ray_positions(_x, _y);

        // Check the interactions.
        if(_action == action::add)
        {
            // Check if the point can be added.
            bool near = false;
            for(const Ogre::Vector3 pos : m_lasso_edge_positions)
            {
                if((std::get<0>(tool_near_far_pos) - pos).length() < m_lasso_edge_size)
                {
                    near = true;
                    break;
                }
            }

            // Add the clicked point.
            if(!near)
            {
                m_lasso_tool_positions.push_back(std::get<0>(tool_near_far_pos));
                m_lasso_near_positions.push_back(std::get<1>(tool_near_far_pos));
                m_lasso_far_positions.push_back(std::get<2>(tool_near_far_pos));
                m_lasso_edge_positions.push_back(std::get<0>(tool_near_far_pos));
            }
            else
            {
                return;
            }
        }
        else if(_action == action::remove)
        {
            // Remove the last clicked point.
            if(!m_lasso_tool_positions.empty())
            {
                m_lasso_edge_positions.pop_back();
                do
                {
                    m_lasso_tool_positions.pop_back();
                    m_lasso_near_positions.pop_back();
                    m_lasso_far_positions.pop_back();
                }
                while(!m_lasso_tool_positions.empty()
                      && m_lasso_tool_positions.back() != m_lasso_edge_positions.back());
            }

            // Clear the last line if it's empty.
            if(m_lasso_tool_positions.empty())
            {
                m_interaction_enable_state = false;
                m_last_lasso_line->clear();
                m_lasso->clear();
                this->request_render();
                return;
            }
        }

        // Draw the lasso.
        this->draw_lasso();

        // Draw the last lasso line.
        m_last_lasso_line->clear();

        SIGHT_ASSERT("Lasso positions must have at east one point", !m_lasso_tool_positions.empty());

        m_last_lasso_line->begin(
            m_material->name(),
            Ogre::RenderOperation::OT_LINE_STRIP,
            sight::viz::scene3d::RESOURCE_GROUP
        );

        m_last_lasso_line->colour(m_line_color);
        m_last_lasso_line->position(m_lasso_tool_positions.back());
        if(_x != -1 && _y != -1)
        {
            m_last_lasso_line->position(std::get<0>(tool_near_far_pos));
        }

        m_last_lasso_line->end();

        // Send a render request.
        this->request_render();
    }
}

//------------------------------------------------------------------------------

void shape_extruder::validate()
{
    this->render_service()->make_current();

    // When coming from touch, mouseReleaseEvent is not always called.
    if(m_left_button_move_state)
    {
        // Add a new point to the lasso edge list.
        m_lasso_edge_positions.push_back(m_lasso_tool_positions.back());
        this->draw_lasso();

        // Cancel the left button move state.
        m_left_button_move_state = false;
    }

    this->triangulate_points();

    this->enable_tool(false);

    auto sig = this->signal<signals::tool_disabled_signal_t>(signals::TOOL_DISABLED);
    sig->async_emit();

    // Send a render request.
    this->request_render();
}

//-----------------------------------------------------------------------------

void shape_extruder::wheel_event(modifier /*_mods*/, double /*_angleDelta*/, int /*_x*/, int /*_y*/)
{
    if(m_interaction_enable_state)
    {
        // Don't change the zoom when the tool interaction is enabled.
        const sight::viz::scene3d::layer::sptr layer = this->layer();
        layer->cancel_further_interaction();
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::button_press_event(mouse_button _button, modifier /*_mods*/, int _x, int _y)
{
    if(_button == mouse_button::left)
    {
        modify_lasso(action::add, _x, _y);
    }
    else if(_button == mouse_button::right)
    {
        modify_lasso(action::remove, _x, _y);
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::button_double_press_event(mouse_button _button, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_validation_by_double_click && m_interaction_enable_state && _button == mouse_button::left)
    {
        // Cancel others interactions.
        const sight::viz::scene3d::layer::sptr layer = this->layer();
        layer->cancel_further_interaction();

        this->validate();
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::mouse_move_event(
    mouse_button _button,
    modifier /*_mods*/,
    int _x,
    int _y,
    int /*_dx*/,
    int /*_dy*/
)
{
    if(!m_interaction_enable_state)
    {
        return;
    }

    render_service()->make_current();

    // Cancel others interactions.
    layer()->cancel_further_interaction();

    // Get the clicked point in the world space.
    const auto& [toolPosition, nearPosition, farPosition] = get_near_far_ray_positions(_x, _y);

    // Check if the mouse is still on the last point.
    // This should not happen but it's better to check, since adding the same points twice will break everything.
    if(!m_lasso_tool_positions.empty() && m_lasso_tool_positions.back() == toolPosition
       && !m_lasso_near_positions.empty() && m_lasso_near_positions.back() == nearPosition
       && !m_lasso_far_positions.empty() && m_lasso_far_positions.back() == farPosition)
    {
        return;
    }

    if(_button == mouse_button::left)
    {
        // Add a new position and draws the lasso.
        m_lasso_tool_positions.push_back(toolPosition);
        m_lasso_near_positions.push_back(nearPosition);
        m_lasso_far_positions.push_back(farPosition);
        draw_lasso();

        // Enable the left button move state.
        m_left_button_move_state = true;
    }

    // Draw the last lasso line.
    SIGHT_ASSERT("Lasso positions must have at east one point", !m_lasso_tool_positions.empty());

    m_last_lasso_line->beginUpdate(0);

    m_last_lasso_line->colour(m_line_color);
    m_last_lasso_line->position(m_lasso_tool_positions.back());
    m_last_lasso_line->position(toolPosition);

    m_last_lasso_line->end();

    // Send a render request.
    request_render();
}

//-----------------------------------------------------------------------------

void shape_extruder::button_release_event(mouse_button /*_button*/, modifier /*_mods*/, int /*_x*/, int /*_y*/)
{
    if(m_interaction_enable_state && m_left_button_move_state)
    {
        this->render_service()->make_current();

        // Cancel others interactions.
        const sight::viz::scene3d::layer::sptr layer = this->layer();
        layer->cancel_further_interaction();

        // Add a new point to the lasso edge list.
        m_lasso_edge_positions.push_back(m_lasso_tool_positions.back());
        this->draw_lasso();

        // Cancel the left button move state.
        m_left_button_move_state = false;

        // Send a render request.
        this->request_render();
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::draw_lasso()
{
    // Clear the previous lasso.
    m_lasso->clear();

    // Draw the lasso line.
    m_lasso->begin(
        m_material->name(),
        Ogre::RenderOperation::OT_LINE_STRIP,
        sight::viz::scene3d::RESOURCE_GROUP
    );
    m_lasso->colour(m_line_color);
    for(const Ogre::Vector3 pos : m_lasso_tool_positions)
    {
        m_lasso->position(pos);
    }

    m_lasso->end();

    // Draw the spheres at the edge of each line.
    const unsigned int sample = 16;
    const auto delta_ring     = static_cast<float>(Ogre::Math::PI / sample);
    const float delta_seg     = 2 * static_cast<float>(Ogre::Math::PI / sample);

    for(const Ogre::Vector3 pos : m_lasso_edge_positions)
    {
        // Begin a new section.
        m_lasso->begin(
            m_material->name(),
            Ogre::RenderOperation::OT_TRIANGLE_LIST,
            sight::viz::scene3d::RESOURCE_GROUP
        );
        m_lasso->colour(m_edge_color);

        Ogre::uint32 index = 0;
        for(unsigned ring = 0 ; ring <= sample ; ++ring)
        {
            const float r0 = m_lasso_edge_size * std::sin(static_cast<float>(ring) * delta_ring);
            const float y0 = m_lasso_edge_size * std::cos(static_cast<float>(ring) * delta_ring);

            for(unsigned seg = 0 ; seg <= sample ; ++seg)
            {
                const float x0 = r0 * std::sin(static_cast<float>(seg) * delta_seg);
                const float z0 = r0 * std::cos(static_cast<float>(seg) * delta_seg);
                Ogre::Vector3 point(x0, y0, z0);

                m_lasso->position(pos + point);

                if(ring != sample)
                {
                    m_lasso->index(index + sample + 1);
                    m_lasso->index(index);
                    m_lasso->index(index + sample);
                    m_lasso->index(index + sample + 1);
                    m_lasso->index(index + 1);
                    m_lasso->index(index);
                    ++index;
                }
            }
        }

        m_lasso->end();
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::triangulate_points() const
{
    SIGHT_ASSERT(
        "Lasso near and far positions must have the same size",
        m_lasso_near_positions.size() == m_lasso_far_positions.size()
    );

    // Generate triangles from the point list with the constrained Browyer-Watson algorithm.
    std::vector<triangle3_d> triangulation;
    if(m_lasso_near_positions.size() > 3)
    {
        this->generate_delaunay_triangulation(m_lasso_near_positions, triangulation);
        if(m_extrude)
        {
            this->generate_delaunay_triangulation(m_lasso_far_positions, triangulation);
        }
    }
    else if(m_lasso_near_positions.size() == 3)
    {
        triangulation.emplace_back(m_lasso_near_positions[0], m_lasso_near_positions[1], m_lasso_near_positions[2]);
        if(m_extrude)
        {
            triangulation.emplace_back(m_lasso_far_positions[0], m_lasso_far_positions[1], m_lasso_far_positions[2]);
        }
    }

    // Create triangles that links the two points lists.
    if(m_extrude)
    {
        const std::size_t size = m_lasso_near_positions.size();
        for(std::size_t index = 0 ; index < size ; ++index)
        {
            const bool is_last   = index == size - 1;
            const std::size_t i0 = index;
            const std::size_t i1 = is_last ? 0 : index + 1;

            const triangle3_d tri_a(m_lasso_near_positions[i0], m_lasso_far_positions[i0], m_lasso_far_positions[i1]);
            const triangle3_d tri_b(m_lasso_near_positions[i0], m_lasso_far_positions[i1], m_lasso_near_positions[i1]);

            triangulation.push_back(tri_a);
            triangulation.push_back(tri_b);
        }
    }

    this->generate_extruded_mesh(triangulation);
}

//-----------------------------------------------------------------------------

void shape_extruder::generate_extruded_mesh(const std::vector<triangle3_d>& _triangulation) const
{
    // Creates the mesh from a list a 3D triangles.
    const data::mesh::sptr mesh = std::make_shared<data::mesh>();
    {
        const auto lock = mesh->dump_lock();

        // 3 points per triangle and one cell per triangle.
        mesh->resize(
            static_cast<data::mesh::size_t>(_triangulation.size() * 3),
            static_cast<data::mesh::size_t>(_triangulation.size()),
            data::mesh::cell_type_t::triangle,
            data::mesh::attribute::point_normals
        );

        // Fill points.
        {
            auto it = mesh->begin<data::iterator::point::xyz>();

            for(const triangle3_d& triangle : _triangulation)
            {
                it->x = triangle.a.x;
                it->y = triangle.a.y;
                it->z = triangle.a.z;
                ++it;
                it->x = triangle.b.x;
                it->y = triangle.b.y;
                it->z = triangle.b.z;
                ++it;
                it->x = triangle.c.x;
                it->y = triangle.c.y;
                it->z = triangle.c.z;
                ++it;
            }
        }

        // Fill cell coordinates.
        {
            auto it = mesh->begin<data::iterator::cell::triangle>();
            for(data::mesh::size_t index = 0 ; index < _triangulation.size() * 3 ; index += 3)
            {
                it->pt[0] = index;
                it->pt[1] = index + 1;
                it->pt[2] = index + 2;

                ++it;
            }
        }

        // Generate normals.
        geometry::data::mesh::generate_point_normals(mesh);
    }

    // Get the reconstruction list.
    const auto extruded_meshes = m_extruded_meshes.lock();

    data::model_series::reconstruction_vector_t reconstructions = extruded_meshes->get_reconstruction_db();

    // Creates the reconstruction.
    const data::reconstruction::sptr reconstruction = std::make_shared<data::reconstruction>();
    reconstruction->set_mesh(mesh);
    reconstruction->set_organ_name("ExtrudedMesh_" + std::to_string(reconstructions.size()));

    // Pushes the reconstruction.
    reconstructions.push_back(reconstruction);
    extruded_meshes->set_reconstruction_db(reconstructions);

    // Send the signal.
    auto sig = extruded_meshes->signal<data::model_series::reconstructions_added_signal_t>(
        data::model_series::RECONSTRUCTIONS_ADDED_SIG
    );
    sig->async_emit(data::model_series::reconstruction_vector_t {reconstruction});
}

//------------------------------------------------------------------------------

void shape_extruder::generate_delaunay_triangulation(
    const std::vector<Ogre::Vector3>& _points,
    std::vector<triangle3_d>& _word_triangulation
) const
{
    // Retrieve the camera.
    const sight::viz::scene3d::layer::sptr layer = this->layer();
    const Ogre::Camera* const camera             = layer->get_default_camera();
    const Ogre::Matrix4 view_matrix              = camera->getViewMatrix();

    // Transform all point from world space to view space to get them in a 2D plane with a constant Z value.
    std::vector<Ogre::Vector2> points;
    for(const Ogre::Vector3 point : _points)
    {
        const Ogre::Vector2 view_point = (view_matrix * Ogre::Vector4(point, 1.F)).xy();
        points.push_back(view_point);
    }

    // Get the depth of the 2D plane.
    const float depth = (view_matrix * Ogre::Vector4(_points[0], 1.F)).z;

    // Compute the bounding box of points.
    const float min = std::numeric_limits<float>::lowest();
    const float max = std::numeric_limits<float>::max();
    Ogre::Vector2 min_bound(max, max);
    Ogre::Vector2 max_bound(min, min);

    for(const Ogre::Vector2 point : points)
    {
        min_bound.x = std::min(point.x, min_bound.x);
        max_bound.x = std::max(point.x, max_bound.x);
        min_bound.y = std::min(point.y, min_bound.y);
        max_bound.y = std::max(point.y, max_bound.y);
    }

    // Increase the bounding box to avoid points to be shared with some of the triangle edges.
    min_bound -= 1;
    max_bound += 1;

    // Compute a triangle large enough to contains all points.
    const Ogre::Vector2 bottom_left = min_bound;
    const Ogre::Vector2 bottom_right(min_bound.x + (max_bound.x - min_bound.x) * 2.F, min_bound.y);
    const Ogre::Vector2 top_left(min_bound.x, min_bound.y + (max_bound.y - min_bound.y) * 2.F);
    const triangle2_d super_triangle(bottom_left, bottom_right, top_left);

    // Store triangles.
    std::vector<triangle2_d> triangulation {super_triangle};

    // Triangulate points with the Bowyer-Watson algorithm.
    for(const Ogre::Vector2 sommet : points)
    {
        sight::module::viz::scene3d::adaptor::shape_extruder::add_delaunay_point(triangulation, sommet);
    }

    // Some input segment are missing from the triangulation, we insert them.
    // Add missing segments while new constraints are added to the previous iteration.
    std::vector<Ogre::Vector2> old_points = points;
    std::vector<Ogre::Vector2> new_points = points;
    const int max_iteration               = 3;
    int count                             = 0;
    do
    {
        old_points = new_points;
        new_points.clear();
        for(std::int64_t index = 0 ; index < static_cast<std::int64_t>(old_points.size()) ; ++index)
        {
            const std::size_t previous_index = index - 1
                                               < 0 ? old_points.size() - 1 : static_cast<std::size_t>(index - 1);
            const edge edge(old_points[previous_index], old_points[static_cast<std::size_t>(index)]);
            std::list<Ogre::Vector2> constraints = this->add_constraints(triangulation, edge);

            new_points.push_back(old_points[previous_index]);
            new_points.insert(new_points.end(), constraints.begin(), constraints.end());
        }
    }
    while(count++ < max_iteration && old_points.size() != new_points.size());

    // If triangles contains a vertex from original super-triangle, remove them.
    auto end_triangulation = triangulation.end();
    for(auto it = triangulation.begin() ; it != end_triangulation ; )
    {
        const bool edge_a = it->a == super_triangle.a || it->a == super_triangle.b || it->a == super_triangle.c;
        const bool edge_b = it->b == super_triangle.a || it->b == super_triangle.b || it->b == super_triangle.c;
        const bool edge_c = it->c == super_triangle.a || it->c == super_triangle.b || it->c == super_triangle.c;

        if(edge_a || edge_b || edge_c)
        {
            it                = triangulation.erase(it);
            end_triangulation = triangulation.end();
        }
        else
        {
            ++it;
        }
    }

    // Remove each triangle that aren't in the shape.
    for(auto it = triangulation.begin() ; it != end_triangulation ; )
    {
        const edge ray(it->barycentre, super_triangle.a);

        bool inside = false;
        for(std::int64_t i = 0 ; i < static_cast<std::int64_t>(points.size()) ; ++i)
        {
            const std::size_t previous_i = i - 1 < 0 ? points.size() - 1 : static_cast<std::size_t>(i - 1);
            const edge edge(points[previous_i], points[static_cast<std::size_t>(i)]);

            if(ray.intersect(edge))
            {
                inside = !inside;
            }
        }

        if(!inside)
        {
            it                = triangulation.erase(it);
            end_triangulation = triangulation.end();
        }
        else
        {
            ++it;
        }
    }

    // Gets back triangle coordinates to the world space.
    for(const triangle2_d& triangle : triangulation)
    {
        Ogre::Vector3 a = (view_matrix.inverse() * Ogre::Vector4(triangle.a.x, triangle.a.y, depth, 1.F)).xyz();
        Ogre::Vector3 b = (view_matrix.inverse() * Ogre::Vector4(triangle.b.x, triangle.b.y, depth, 1.F)).xyz();
        Ogre::Vector3 c = (view_matrix.inverse() * Ogre::Vector4(triangle.c.x, triangle.c.y, depth, 1.F)).xyz();

        _word_triangulation.emplace_back(a, b, c);
    }
}

//-----------------------------------------------------------------------------

void shape_extruder::add_delaunay_point(std::vector<triangle2_d>& _triangulation, const Ogre::Vector2& _sommet)
{
    // first find all the triangles that are no longer valid due to the insertion.
    std::list<triangle2_d> bad_triangles;
    for(const triangle2_d& triangle : _triangulation)
    {
        // If the current sommet is in the circumscribed circle of the current triangle.
        if(triangle.center.distance(_sommet) <= triangle.radius)
        {
            bad_triangles.push_back(triangle);
        }
    }

    // find the boundary of the polygonal hole.
    std::list<edge> polygon;
    for(const triangle2_d& triangle : bad_triangles)
    {
        const edge edge_a(triangle.a, triangle.b);
        const edge edge_b(triangle.a, triangle.c);
        const edge edge_c(triangle.b, triangle.c);

        bool allow_a = true;
        bool allow_b = true;
        bool allow_c = true;

        // If the edge is not shared by any other triangles in badTriangles.
        for(const triangle2_d& shared : bad_triangles)
        {
            if(shared.id != triangle.id)
            {
                edge shared_a(shared.a, shared.b);
                edge shared_b(shared.a, shared.c);
                edge shared_c(shared.b, shared.c);
                if(edge_a == shared_a || edge_a == shared_b || edge_a == shared_c)
                {
                    allow_a = false;
                }

                if(edge_b == shared_a || edge_b == shared_b || edge_b == shared_c)
                {
                    allow_b = false;
                }

                if(edge_c == shared_a || edge_c == shared_b || edge_c == shared_c)
                {
                    allow_c = false;
                }
            }
        }

        // Add edges of the triangle to the candidate polygon.
        if(allow_a)
        {
            polygon.push_back(edge_a);
        }

        if(allow_b)
        {
            polygon.push_back(edge_b);
        }

        if(allow_c)
        {
            polygon.push_back(edge_c);
        }

        _triangulation.erase(
            std::find_if(
                _triangulation.begin(),
                _triangulation.end(),
                [&](const triangle2_d& _tri) -> bool
            {
                return _tri.id == triangle.id;
            })
        );
    }

    // Create news triangles by using the current sommet and the polygon.
    for(const edge& edge : polygon)
    {
        // Add a new triangle to the triangulation.
        const triangle2_d tri(edge.a, edge.b, _sommet);
        _triangulation.push_back(tri);
    }
}

//------------------------------------------------------------------------------

std::list<Ogre::Vector2> shape_extruder::add_constraints(
    std::vector<triangle2_d>& _triangulation,
    const edge& _edge,
    int _depth
) const
{
    if(_depth >= 5)
    {
        return {};
    }

    bool found = false;
    for(const triangle2_d& triangle : _triangulation)
    {
        const edge edge_a(triangle.a, triangle.b);
        const edge edge_b(triangle.a, triangle.c);
        const edge edge_c(triangle.b, triangle.c);

        if(_edge == edge_a || _edge == edge_b || _edge == edge_c)
        {
            found = true;
            break;
        }
    }

    std::list<Ogre::Vector2> added_points;
    if(!found)
    {
        const Ogre::Vector2 mid_point = (_edge.a + _edge.b) / 2.F;
        shape_extruder::add_delaunay_point(_triangulation, mid_point);

        const int depth                      = _depth + 1;
        std::list<Ogre::Vector2> front_added = this->add_constraints(_triangulation, edge(_edge.a, mid_point), depth);
        std::list<Ogre::Vector2> back_added  = this->add_constraints(_triangulation, edge(mid_point, _edge.b), depth);

        added_points = front_added;
        added_points.push_back(mid_point);
        added_points.insert(added_points.end(), back_added.begin(), back_added.end());
    }

    return added_points;
}

} // namespace sight::module::viz::scene3d::adaptor.
