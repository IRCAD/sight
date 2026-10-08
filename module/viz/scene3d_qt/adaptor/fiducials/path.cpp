/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "module/viz/scene3d_qt/adaptor/fiducials/path.hpp"

#include <data/fiducials_series.hpp>
#include <data/has_fiducials.hpp>
#include <data/material.hpp>
#include <data/mesh.hpp>

#include <viz/scene3d/ogre.hpp>

#include <optional>
#include <utility>

namespace sight::module::viz::scene3d_qt::adaptor::fiducials
{

//------------------------------------------------------------------------------

static bool fiducial_query_predicate(const data::fiducials_series::query_result& _result)
{
    return _result.m_contour_data && _result.m_contour_data->size() == 3;
}

//-----------------------------------------------------------------------------

path::path() noexcept
{
    new_slot(slots::CLEAR, &path::clear, this);
    new_slot(slots::SELECT_GROUP, &path::select_group, this);
    new_slot(slots::POINT_EDIT_MODE, &path::point_edit_mode_changed, this);
    new_slot(slots::SET_LANDMARK_GROUP, &path::set_landmark_group, this);

    new_signal<signals::group_selected_t>(signals::GROUP_SELECTED);
    new_signal<signals::path_edit_mode_changed_t>(signals::PATH_EDIT_MODE_CHANGED);
    new_signal<signals::points_available_t>(signals::POINTS_AVAILABLE);
}

//-----------------------------------------------------------------------------

void path::configuring()
{
    configure_params();

    const auto& config = get_config();
    set_transform_id(
        config.get<std::string>(sight::viz::scene3d::transformable::TRANSFORM_CONFIG, get_id() + "_transform")
    );
}

//-----------------------------------------------------------------------------

void path::starting()
{
    adaptor::init();
    render_service()->make_current();

    m_transform_node = get_or_create_transform_node(get_scene_manager()->getRootSceneNode());
    m_path_material  = std::make_unique<sight::viz::scene3d::material::standard>(gen_id("fiducial_path_material"));
    m_path_material->set_layout(data::mesh::attribute::point_colors);
    m_path_material->set_shading(sight::data::material::shading_t::ambient, layer()->num_lights(), false, false);

    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t path::auto_connections() const
{
    return service::connections_t {
        {m_image_series, data::signals::MODIFIED, adaptor::slots::LAZY_UPDATE},
        {m_image_series, data::has_fiducials::signals::POINT_ADDED, adaptor::slots::LAZY_UPDATE},
        {m_image_series, data::has_fiducials::signals::POINT_MODIFIED, adaptor::slots::LAZY_UPDATE},
        {m_image_series, data::has_fiducials::signals::POINT_REMOVED, adaptor::slots::LAZY_UPDATE},
        {m_image_series, data::has_fiducials::signals::POINT_INSERTED, adaptor::slots::LAZY_UPDATE},
        {m_image_series, data::has_fiducials::signals::GROUP_REMOVED, adaptor::slots::LAZY_UPDATE},
        {m_image_series, data::has_fiducials::signals::GROUP_RENAMED, adaptor::slots::LAZY_UPDATE}
    } + adaptor::auto_connections();
}

//-----------------------------------------------------------------------------

void path::updating()
{
    create_path();
    update_done();
    request_render();
}

//-----------------------------------------------------------------------------

void path::create_path()
{
    clear_path();

    const auto image = m_image_series.const_lock();
    if(!image || !image->get_fiducials())
    {
        async_emit(signals::POINTS_AVAILABLE, false);
        return;
    }

    const auto points = image->get_fiducials()->query_fiducials(
        fiducial_query_predicate,
        data::fiducials_series::shape::point,
        m_path_group.value()
    );

    const bool points_available = points.size() >= 2;
    if(!points_available)
    {
        async_emit(signals::POINTS_AVAILABLE, false);
        return;
    }

    m_path_object = get_scene_manager()->createManualObject(gen_id("fiducial_path"));
    m_path_object->setRenderQueueGroup(sight::viz::scene3d::rq::SURFACE);
    m_path_object->begin(
        m_path_material->name(),
        Ogre::RenderOperation::OT_LINE_STRIP,
        sight::viz::scene3d::RESOURCE_GROUP
    );

    const auto path_color = m_path_color.const_lock();
    m_path_object->colour(
        Ogre::ColourValue(
            path_color->red(),
            path_color->green(),
            path_color->blue(),
            path_color->alpha()
        )
    );
    const Ogre::Technique* const line_tech = m_path_material->material()->getTechnique(0);
    SIGHT_ASSERT("No technique found", line_tech);
    Ogre::Pass* const line_pass = line_tech->getPass(0);
    SIGHT_ASSERT("No pass found", line_pass);
    line_pass->setDepthCheckEnabled(false);
    line_pass->setLineWidth(2.F);
    for(const auto& point : points)
    {
        const auto& position = *point.m_contour_data;
        m_path_object->position(
            static_cast<float>(position[0]),
            static_cast<float>(position[1]),
            static_cast<float>(position[2])
        );
    }

    m_path_object->end();
    m_path_object->setQueryFlags(0x0);
    m_transform_node->attachObject(m_path_object);
    async_emit(signals::POINTS_AVAILABLE, true);
}

//-----------------------------------------------------------------------------

void path::stopping()
{
    clear_path();
    m_path_material.reset();
    adaptor::deinit();
}

//-----------------------------------------------------------------------------

void path::clear_path()
{
    if(m_path_object == nullptr)
    {
        return;
    }

    m_path_object->detachFromParent();
    get_scene_manager()->destroyManualObject(m_path_object);
    m_path_object = nullptr;
}

//-----------------------------------------------------------------------------

void path::clear()
{
    clear_path();
    const auto image = m_image_series.lock();
    if(!image || !image->get_fiducials())
    {
        async_emit(signals::POINTS_AVAILABLE, false);
        return;
    }

    const auto [removed_points, removed_groups] = image->get_fiducials()->remove_fiducials(
        std::nullopt,
        data::fiducials_series::shape::point,
        m_path_group.value()
    );

    for(const auto& it : std::views::reverse(removed_points))
    {
        image->async_emit(
            this,
            data::has_fiducials::signals::POINT_REMOVED,
            it.m_group_name.value_or(""),
            it.m_shape_index
        );
    }

    for(const auto& group : removed_groups)
    {
        image->async_emit(this, data::has_fiducials::signals::GROUP_REMOVED, group);
    }

    async_emit(signals::POINTS_AVAILABLE, false);
}

//-----------------------------------------------------------------------------

void path::select_group(bool _selected)
{
    if(_selected)
    {
        auto image       = m_image_series.lock();
        const auto group = m_path_group.value();
        auto fiducials   = image->get_fiducials();

        if(!fiducials->get_fiducial_set_and_index(group))
        {
            const auto color = m_path_color.const_lock();
            const auto size  = m_fiducial_size.const_lock();
            fiducials->add_group(
                group,
                {color->red(), color->green(), color->blue(), color->alpha()},
                static_cast<float>(size->value())
            );
            image->async_emit(data::has_fiducials::signals::GROUP_ADDED, group);
        }
    }

    async_emit(signals::GROUP_SELECTED, _selected ? m_path_group.value() : m_landmark_group);
}

//-----------------------------------------------------------------------------

void path::point_edit_mode_changed(bool _enabled)
{
    if(!_enabled)
    {
        select_group(false);
        async_emit(signals::PATH_EDIT_MODE_CHANGED, false);
    }
}

//-----------------------------------------------------------------------------

void path::set_landmark_group(std::string _group)
{
    if(_group != m_path_group.value())
    {
        m_landmark_group = std::move(_group);
    }
}

} // namespace sight::module::viz::scene3d_qt::adaptor::fiducials
