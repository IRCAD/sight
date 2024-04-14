/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "render.hpp"

#include "graphics_view.hpp"

#include "viz/scene2d/adaptor.hpp"
#include "viz/scene2d/registry/adaptor.hpp"

#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/tools/color.hpp>

#include <service/macros.hpp>

#include <ui/qt/container/widget.hpp>

#include <QGraphicsRectItem>
#include <QVBoxLayout>

#include <cmath>

namespace sight::viz::scene2d
{

render::render() noexcept :
    m_scene_start(-100., -100.),
    m_scene_width(200., 200.),
    m_background("#000000")
{
}

//-----------------------------------------------------------------------------

render::~render() noexcept =
    default;

//-----------------------------------------------------------------------------

QGraphicsScene* render::get_scene() const
{
    return m_scene;
}

//-----------------------------------------------------------------------------

graphics_view* render::get_view() const
{
    return m_view;
}

//-----------------------------------------------------------------------------

scene2d::data::axis::sptr render::get_axis(const std::string& _id) const
{
    scene2d::data::axis::sptr axis;
    const auto iter = m_axis_map.find(_id);
    if(iter != m_axis_map.end())
    {
        axis = iter->second;
    }

    return axis;
}

//-----------------------------------------------------------------------------

void render::dispatch_interaction(scene2d::data::event& _event)
{
    if(!_event.is_accepted())
    {
        // Get all started adaptors.
        std::vector<viz::scene2d::adaptor::sptr> ordered_adaptors;

        const auto& registry = viz::scene2d::registry::get_adaptor_registry();
        for(const auto& elt : registry)
        {
            if(elt.second == this->get_id())
            {
                viz::scene2d::adaptor::sptr adaptor =
                    std::dynamic_pointer_cast<viz::scene2d::adaptor>(core::tools::id::get_object(elt.first));
                if(adaptor != nullptr && adaptor->started())
                {
                    ordered_adaptors.push_back(adaptor);
                }
            }
        }

        // Sort adaptors by z value.
        std::sort(
            ordered_adaptors.begin(),
            ordered_adaptors.end(),
            [&](viz::scene2d::adaptor::sptr _a1, viz::scene2d::adaptor::sptr _a2)
            {
                return _a1->get_z_value() > _a2->get_z_value();
            });

        // Process interaction on all adaptors until one has accepted the event.
        for(const viz::scene2d::adaptor::sptr& adaptor : ordered_adaptors)
        {
            adaptor->process_interaction(_event);
            if(_event.is_accepted())
            {
                return;
            }
        }
    }
}

//-----------------------------------------------------------------------------

bool render::contains(const scene2d::vec2d_t& _coord) const
{
    /// Returns the viewport coordinate point mapped to scene coordinates.
    const QPoint qp(static_cast<int>(_coord.x), static_cast<int>(_coord.y));
    QPointF qps = m_view->mapToScene(qp);

    QRectF rect = m_view->sceneRect();

    return rect.contains(qps);
}

//-----------------------------------------------------------------------------

scene2d::vec2d_t render::map_to_scene(const scene2d::vec2d_t& _coord, bool _clip) const
{
    /// Returns the viewport coordinate point mapped to scene coordinates.
    const QPoint qp(static_cast<int>(_coord.x), static_cast<int>(_coord.y));
    QPointF qps = m_view->mapToScene(qp);
    if(_clip)
    {
        QRectF rect = m_view->sceneRect();
        if(!rect.contains(qps))
        {
            // Keep the item inside the scene rect.
            qps.setX(qMin(rect.right(), qMax(qps.x(), rect.left())));
            qps.setY(qMin(rect.bottom(), qMax(qps.y(), rect.top())));
        }
    }

    return {qps.x(), qps.y()};
}

//-----------------------------------------------------------------------------

void render::configuring()
{
    this->initialize();

    const auto& config = this->get_config();

    const auto scene_cfg = config.get_child_optional("scene");
    SIGHT_ASSERT("There is no implementation between \"scene\" tags", scene_cfg.has_value());

    for(const auto& iter : scene_cfg.value())
    {
        if(iter.first == "axis")
        {
            this->configure_axis(iter.second.get_child("<xmlattr>"));
        }
        else if(iter.first == "scene")
        {
            this->configure_scene(iter.second.get_child("<xmlattr>"));
        }
        else if(iter.first == "adaptor")
        {
            this->configure_adaptor(iter.second.get_child("<xmlattr>"));
        }
        else
        {
            SIGHT_ASSERT("Bad scene const base&, unknown xml node : " + iter.first, false);
        }
    }
}

//-----------------------------------------------------------------------------

void render::starting()
{
    this->create();

    this->start_context();
}

//-----------------------------------------------------------------------------

void render::updating()
{
}

//-----------------------------------------------------------------------------

void render::stopping()
{
    m_axis_map.clear();

    this->stop_context();
    this->destroy();
}

//-----------------------------------------------------------------------------

void render::start_context()
{
    auto qt_container = std::dynamic_pointer_cast<ui::qt::container::widget>(this->get_container());

    // Convert the background color
    std::array<std::uint8_t, 4> color {};
    sight::data::tools::color::hexa_string_to_rgba(m_background, color);
    m_scene = new QGraphicsScene(m_scene_start.x, m_scene_start.y, m_scene_width.x, m_scene_width.y);

    // First use the qss class if available
    if(!m_qss_class.empty())
    {
        m_scene->setProperty("class", QString::fromStdString(m_qss_class));
    }
    // Use the background color if no qss
    else
    {
        m_scene->setBackgroundBrush(QBrush(QColor(color[0], color[1], color[2], color[3])));
    }

    m_scene->setFocus(Qt::MouseFocusReason);

    m_view = new graphics_view(m_scene, qt_container->get_qt_container());
    m_view->set_scene_render(std::dynamic_pointer_cast<viz::scene2d::render>(this->get_sptr()));
    m_view->setRenderHint(QPainter::Antialiasing, m_antialiasing);

    auto* layout = new QVBoxLayout;
    layout->addWidget(m_view);
    qt_container->set_layout(layout);

    viz::scene2d::data::viewport init_viewport;
    init_viewport.set_x(m_scene_start.x);
    init_viewport.set_y(m_scene_start.y);
    init_viewport.set_width(m_scene_width.x);
    init_viewport.set_height(m_scene_width.y);
    m_view->update_from_viewport(init_viewport);
}

//-----------------------------------------------------------------------------

void render::stop_context()
{
    delete m_scene;
    delete m_view;
}

//-----------------------------------------------------------------------------

Qt::AspectRatioMode render::get_aspect_ratio_mode() const
{
    return m_aspect_ratio_mode;
}

//-----------------------------------------------------------------------------

void render::configure_axis(const config_t& _conf)
{
    const auto id         = _conf.get<std::string>("id");
    const auto scale_type = _conf.get<std::string>("scaleType");
    const auto origin     = _conf.get<float>("origin");
    const auto scale      = _conf.get<float>("scale");

    scene2d::data::axis::sptr axis = std::make_shared<scene2d::data::axis>();
    axis->set_origin(origin);
    axis->set_scale(scale);
    axis->set_scale_type(scale_type == "LINEAR" ? scene2d::data::axis::linear : scene2d::data::axis::log);
    m_axis_map[id] = axis;
}

//-----------------------------------------------------------------------------

void render::configure_scene(const config_t& _conf)
{
    m_scene_start.x = _conf.get<float>("x");
    m_scene_start.y = _conf.get<float>("y");
    m_scene_width.x = _conf.get<float>("width");
    m_scene_width.y = _conf.get<float>("height");

    m_antialiasing = _conf.get<bool>("antialiasing", m_antialiasing);

    if(const auto aspect_ratio = _conf.get_optional<std::string>("aspectRatioMode"); aspect_ratio.has_value())
    {
        if(*aspect_ratio == "KeepAspectRatioByExpanding")
        {
            m_aspect_ratio_mode = Qt::KeepAspectRatioByExpanding;
        }
        else if(*aspect_ratio == "KeepAspectRatio")
        {
            m_aspect_ratio_mode = Qt::KeepAspectRatio;
        }
        else
        {
            SIGHT_ERROR_IF(
                "Unknown aspect ratio ("
                << *aspect_ratio
                << "). Possible values are: KeepAspectRatio, KeepAspectRatioByExpanding or IgnoreAspectRatio.",
                *aspect_ratio != "IgnoreAspectRatio"
            );
            m_aspect_ratio_mode = Qt::IgnoreAspectRatio;
        }
    }

    if(const auto hexa_color = _conf.get<std::string>("background", ""); !hexa_color.empty())
    {
        SIGHT_ASSERT(
            "Color string should start with '#' and followed by 6 or 8 "
            "hexadecimal digits. Given color: " << hexa_color,
            hexa_color[0] == '#'
            && (hexa_color.length() == 7 || hexa_color.length() == 9)
        );
        m_background = hexa_color;
    }

    if(const auto qss_class = _conf.get<std::string>("QSSClass", ""); !qss_class.empty())
    {
        m_qss_class = qss_class;
    }
}

//-----------------------------------------------------------------------------

void render::configure_adaptor(const config_t& _conf)
{
    const auto adaptor_id = _conf.get<std::string>("uid");

    auto& registry = viz::scene2d::registry::get_adaptor_registry();
    registry[adaptor_id] = this->get_id();
}

//-----------------------------------------------------------------------------

void render::update_scene_size(float _ratio_percent)
{
    QRectF rec = m_scene->itemsBoundingRect();
    qreal x    = NAN;
    qreal y    = NAN;
    qreal w    = NAN;
    qreal h    = NAN;
    rec.getRect(&x, &y, &w, &h);

    if(_ratio_percent != 0)
    {
        qreal center_x = x + w / 2.0;
        qreal center_y = y + h / 2.0;
        w = w + w * _ratio_percent;
        h = h + h * _ratio_percent;
        x = center_x - w / 2.0;
        y = center_y - h / 2.0;
        rec.setRect(x, y, w, h);
    }

    m_scene_start.x = x;
    m_scene_start.y = y;
    m_scene_width.x = w;
    m_scene_width.y = h;

    m_scene->setSceneRect(rec);
}

//-----------------------------------------------------------------------------

} // namespace sight::viz::scene2d
