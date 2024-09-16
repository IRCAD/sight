/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "core_compositor_editor.hpp"

#include <data/map.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/render.hpp>

#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreResource.h>
#include <OGRE/OgreResourceManager.h>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QRadioButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::viz
{

//------------------------------------------------------------------------------

core_compositor_editor::core_compositor_editor() noexcept =
    default;

//------------------------------------------------------------------------------

core_compositor_editor::~core_compositor_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void core_compositor_editor::starting()
{
    this->create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );

    auto* layout = new QVBoxLayout();

    auto* label_layer_selector = new QLabel(tr("3D layer selected"));
    layout->addWidget(label_layer_selector);
    m_layers_box = new QComboBox();
    layout->addWidget(m_layers_box);

    // Transparency depth management
    {
        auto* label_transparency = new QLabel(tr("Transparency depth"));
        layout->addWidget(label_transparency);
        m_transparency_depth_slider = new QSlider(Qt::Horizontal);
        layout->addWidget(m_transparency_depth_slider);
        m_transparency_depth_slider->setEnabled(false);
        m_transparency_depth_slider->setTickInterval(1);
        m_transparency_depth_slider->setTickPosition(QSlider::TicksBelow);
        m_transparency_depth_slider->setRange(0, 12);
        m_transparency_depth_slider->setValue(8);
    }

    // Transparency selector
    {
        auto* group_box        = new QGroupBox(tr("Transparency technique"));
        auto* layout_group_box = new QVBoxLayout();
        group_box->setLayout(layout_group_box);
        layout->addWidget(group_box);

        m_transparency_button_group = new QButtonGroup(group_box);

        m_button_default = new QRadioButton(tr("Default"), group_box);
        m_button_default->setMinimumSize(m_button_default->sizeHint());
        m_button_default->setEnabled(false);
        m_transparency_button_group->addButton(m_button_default, 0);
        layout_group_box->addWidget(m_button_default);
        m_label_default = new QLabel(tr("<i>No Order Independent Transparency</i>"));
        m_label_default->setEnabled(false);
        layout_group_box->addWidget(m_label_default);

        m_button_depth_peeling = new QRadioButton(tr("Depth Peeling"), group_box);
        m_button_depth_peeling->setMinimumSize(m_button_depth_peeling->sizeHint());
        m_button_depth_peeling->setEnabled(false);
        m_transparency_button_group->addButton(m_button_depth_peeling, 1);
        layout_group_box->addWidget(m_button_depth_peeling);
        m_label_depth_peeling = new QLabel(tr("<i>Exact color blending but slowest technique</i>"));
        m_label_depth_peeling->setEnabled(false);
        layout_group_box->addWidget(m_label_depth_peeling);

        m_button_dual_depth_peeling =
            new QRadioButton(tr("Dual Depth Peeling"), group_box);
        m_button_dual_depth_peeling->setMinimumSize(m_button_dual_depth_peeling->sizeHint());
        m_button_dual_depth_peeling->setEnabled(false);
        m_transparency_button_group->addButton(m_button_dual_depth_peeling, 2);
        layout_group_box->addWidget(m_button_dual_depth_peeling);
        m_label_dual_depth_peeling = new QLabel(tr("<i>Exact color blending but slow technique</i>"));
        m_label_dual_depth_peeling->setEnabled(false);
        layout_group_box->addWidget(m_label_dual_depth_peeling);

        m_button_weighted_blended_oit =
            new QRadioButton(tr("Weighted Blended OIT"), group_box);
        m_button_weighted_blended_oit->setMinimumSize(m_button_weighted_blended_oit->sizeHint());
        m_button_weighted_blended_oit->setEnabled(false);
        m_transparency_button_group->addButton(m_button_weighted_blended_oit, 3);
        layout_group_box->addWidget(m_button_weighted_blended_oit);
        m_label_weighted_blended_oit = new QLabel(tr("<i>Approximative color blending but fastest</i>"));
        m_label_weighted_blended_oit->setEnabled(false);
        layout_group_box->addWidget(m_label_weighted_blended_oit);

        m_button_hybrid_transparency =
            new QRadioButton(tr("Hybrid transparency"), group_box);
        m_button_hybrid_transparency->setMinimumSize(m_button_hybrid_transparency->sizeHint());
        m_button_hybrid_transparency->setEnabled(false);
        m_transparency_button_group->addButton(m_button_hybrid_transparency, 4);
        layout_group_box->addWidget(m_button_hybrid_transparency);
        m_label_hybrid_transparency =
            new QLabel(tr("<i>Depth Peeling + Weighted Blended OIT = half exact half fast</i>"));
        m_label_hybrid_transparency->setEnabled(false);
        layout_group_box->addWidget(m_label_hybrid_transparency);

        m_button_cel_shading_depth_peeling = new QRadioButton(tr("CelShading + Depth Peeling"), group_box);
        m_button_cel_shading_depth_peeling->setMinimumSize(m_button_depth_peeling->sizeHint());
        m_button_cel_shading_depth_peeling->setEnabled(false);
        m_transparency_button_group->addButton(m_button_cel_shading_depth_peeling, 5);
        layout_group_box->addWidget(m_button_cel_shading_depth_peeling);
        m_label_cel_shading_depth_peeling = new QLabel(
            tr(
                "<i>Depth peeling with an edge detection per layer.</i>"
            )
        );
        m_label_cel_shading_depth_peeling->setEnabled(false);
        layout_group_box->addWidget(m_label_cel_shading_depth_peeling);
    }

    qt_container->set_layout(layout);

    this->refresh_renderers();

    QObject::connect(m_layers_box, SIGNAL(activated(int)), this, SLOT(on_selected_layer_item(int)));
    QObject::connect(
        m_transparency_depth_slider,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(on_edit_transparency_depth(int))
    );
    QObject::connect(m_transparency_button_group, SIGNAL(buttonClicked(int)), this, SLOT(on_edit_transparency(int)));
}

//------------------------------------------------------------------------------

void core_compositor_editor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void core_compositor_editor::refresh_renderers()
{
    m_layers_box->clear();

    // Fill layer box with all enabled layers
    const auto renderers = sight::service::get_services("sight::viz::scene3d::render");

    for(const auto& srv : renderers)
    {
        sight::viz::scene3d::render::sptr render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layer_map : render->get_layers())
        {
            // Adds default layers (3D scene)
            if(layer_map.second->is_core_compositor_enabled())
            {
                const std::string id  = layer_map.first;
                std::string render_id = render->get_id();
                m_layers_box->addItem(QString::fromStdString(render_id + " : " + id));
                m_layers.push_back(layer_map.second);
            }
        }
    }

    m_layers_box->setCurrentIndex(-1);
}

//------------------------------------------------------------------------------

void core_compositor_editor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void core_compositor_editor::updating()
{
}

//------------------------------------------------------------------------------

void core_compositor_editor::on_selected_layer_item(int _index)
{
    namespace compositor = sight::viz::scene3d::compositor;

    if(!m_is_layer_selected)
    {
        m_transparency_depth_slider->setEnabled(true);
        m_button_default->setEnabled(true);
        m_button_depth_peeling->setEnabled(true);
        m_button_dual_depth_peeling->setEnabled(true);
        m_button_weighted_blended_oit->setEnabled(true);
        m_button_hybrid_transparency->setEnabled(true);
        m_button_cel_shading_depth_peeling->setEnabled(true);
        m_label_default->setEnabled(true);
        m_label_depth_peeling->setEnabled(true);
        m_label_dual_depth_peeling->setEnabled(true);
        m_label_weighted_blended_oit->setEnabled(true);
        m_label_hybrid_transparency->setEnabled(true);
        m_label_cel_shading_depth_peeling->setEnabled(true);
    }

    // Reloads buttons to match layer's parameters
    m_current_layer = m_layers[static_cast<std::size_t>(_index)];

    // If the layer is not yet started, we can't use its default compositor
    auto layer = m_current_layer.lock();
    if(layer)
    {
        switch(layer->get_transparency_technique())
        {
            case compositor::DEFAULT:
                m_transparency_button_group->button(0)->setChecked(true);
                break;

            case compositor::depthpeeling:
                m_transparency_button_group->button(1)->setChecked(true);
                break;

            case compositor::dualdepthpeeling:
                m_transparency_button_group->button(2)->setChecked(true);
                break;

            case compositor::weightedblendedoit:
                m_transparency_button_group->button(3)->setChecked(true);
                break;

            case compositor::hybridtransparency:
                m_transparency_button_group->button(4)->setChecked(true);
                break;

            case compositor::cellshading_depthpeeling:
                m_transparency_button_group->button(5)->setChecked(true);
                break;
        }

        m_transparency_depth_slider->setValue(layer->get_transparency_depth());
    }
}

//------------------------------------------------------------------------------

void core_compositor_editor::on_edit_transparency_depth(int _depth)
{
    auto layer = m_current_layer.lock();
    if(layer)
    {
        layer->set_transparency_depth(_depth);
    }
}

//------------------------------------------------------------------------------

void core_compositor_editor::on_edit_transparency(int _index)
{
    namespace compositor = sight::viz::scene3d::compositor;

    auto layer = m_current_layer.lock();
    if(layer)
    {
        bool transparency_updated = false;
        switch(_index)
        {
            case 0:
                transparency_updated = layer->set_transparency_technique(compositor::DEFAULT);
                break;

            case 1:
                transparency_updated = layer->set_transparency_technique(compositor::depthpeeling);
                break;

            case 2:
                transparency_updated = layer->set_transparency_technique(compositor::dualdepthpeeling);
                break;

            case 3:
                transparency_updated = layer->set_transparency_technique(compositor::weightedblendedoit);
                break;

            case 4:
                transparency_updated = layer->set_transparency_technique(compositor::hybridtransparency);
                break;

            case 5:
                transparency_updated = layer->set_transparency_technique(compositor::cellshading_depthpeeling);
                break;

            default:
                transparency_updated = false;
        }

        if(!transparency_updated)
        {
            m_transparency_button_group->button(0)->setChecked(true);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
