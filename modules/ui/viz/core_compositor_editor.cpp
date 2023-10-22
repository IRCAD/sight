/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include <data/composite.hpp>

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
        this->getContainer()
    );

    auto* layout = new QVBoxLayout();

    auto* label_layer_selector = new QLabel(tr("3D layer selected"));
    layout->addWidget(label_layer_selector);
    m_layersBox = new QComboBox();
    layout->addWidget(m_layersBox);

    // Transparency depth management
    {
        auto* label_transparency = new QLabel(tr("Transparency depth"));
        layout->addWidget(label_transparency);
        m_transparencyDepthSlider = new QSlider(Qt::Horizontal);
        layout->addWidget(m_transparencyDepthSlider);
        m_transparencyDepthSlider->setEnabled(false);
        m_transparencyDepthSlider->setTickInterval(1);
        m_transparencyDepthSlider->setTickPosition(QSlider::TicksBelow);
        m_transparencyDepthSlider->setRange(0, 12);
        m_transparencyDepthSlider->setValue(8);
    }

    // Transparency selector
    {
        auto* group_box        = new QGroupBox(tr("Transparency technique"));
        auto* layout_group_box = new QVBoxLayout();
        group_box->setLayout(layout_group_box);
        layout->addWidget(group_box);

        m_transparencyButtonGroup = new QButtonGroup(group_box);

        m_buttonDefault = new QRadioButton(tr("Default"), group_box);
        m_buttonDefault->setMinimumSize(m_buttonDefault->sizeHint());
        m_buttonDefault->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDefault, 0);
        layout_group_box->addWidget(m_buttonDefault);
        m_labelDefault = new QLabel(tr("<i>No Order Independent Transparency</i>"));
        m_labelDefault->setEnabled(false);
        layout_group_box->addWidget(m_labelDefault);

        m_buttonDepthPeeling = new QRadioButton(tr("Depth Peeling"), group_box);
        m_buttonDepthPeeling->setMinimumSize(m_buttonDepthPeeling->sizeHint());
        m_buttonDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDepthPeeling, 1);
        layout_group_box->addWidget(m_buttonDepthPeeling);
        m_labelDepthPeeling = new QLabel(tr("<i>Exact color blending but slowest technique</i>"));
        m_labelDepthPeeling->setEnabled(false);
        layout_group_box->addWidget(m_labelDepthPeeling);

        m_buttonDualDepthPeeling =
            new QRadioButton(tr("Dual Depth Peeling"), group_box);
        m_buttonDualDepthPeeling->setMinimumSize(m_buttonDualDepthPeeling->sizeHint());
        m_buttonDualDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDualDepthPeeling, 2);
        layout_group_box->addWidget(m_buttonDualDepthPeeling);
        m_labelDualDepthPeeling = new QLabel(tr("<i>Exact color blending but slow technique</i>"));
        m_labelDualDepthPeeling->setEnabled(false);
        layout_group_box->addWidget(m_labelDualDepthPeeling);

        m_buttonWeightedBlendedOIT =
            new QRadioButton(tr("Weighted Blended OIT"), group_box);
        m_buttonWeightedBlendedOIT->setMinimumSize(m_buttonWeightedBlendedOIT->sizeHint());
        m_buttonWeightedBlendedOIT->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonWeightedBlendedOIT, 3);
        layout_group_box->addWidget(m_buttonWeightedBlendedOIT);
        m_labelWeightedBlendedOIT = new QLabel(tr("<i>Approximative color blending but fastest</i>"));
        m_labelWeightedBlendedOIT->setEnabled(false);
        layout_group_box->addWidget(m_labelWeightedBlendedOIT);

        m_buttonHybridTransparency =
            new QRadioButton(tr("Hybrid transparency"), group_box);
        m_buttonHybridTransparency->setMinimumSize(m_buttonHybridTransparency->sizeHint());
        m_buttonHybridTransparency->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonHybridTransparency, 4);
        layout_group_box->addWidget(m_buttonHybridTransparency);
        m_labelHybridTransparency =
            new QLabel(tr("<i>Depth Peeling + Weighted Blended OIT = half exact half fast</i>"));
        m_labelHybridTransparency->setEnabled(false);
        layout_group_box->addWidget(m_labelHybridTransparency);

        m_buttonCelShadingDepthPeeling = new QRadioButton(tr("CelShading + Depth Peeling"), group_box);
        m_buttonCelShadingDepthPeeling->setMinimumSize(m_buttonDepthPeeling->sizeHint());
        m_buttonCelShadingDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonCelShadingDepthPeeling, 5);
        layout_group_box->addWidget(m_buttonCelShadingDepthPeeling);
        m_labelCelShadingDepthPeeling = new QLabel(
            tr(
                "<i>Depth peeling with an edge detection per layer.</i>"
            )
        );
        m_labelCelShadingDepthPeeling->setEnabled(false);
        layout_group_box->addWidget(m_labelCelShadingDepthPeeling);
    }

    qt_container->setLayout(layout);

    this->refreshRenderers();

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));
    QObject::connect(m_transparencyDepthSlider, SIGNAL(valueChanged(int)), this, SLOT(onEditTransparencyDepth(int)));
    QObject::connect(m_transparencyButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onEditTransparency(int)));
}

//------------------------------------------------------------------------------

void core_compositor_editor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void core_compositor_editor::refreshRenderers()
{
    m_layersBox->clear();

    // Fill layer box with all enabled layers
    const auto renderers = sight::service::get_services("sight::viz::scene3d::render");

    for(const auto& srv : renderers)
    {
        sight::viz::scene3d::render::sptr render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layer_map : render->getLayers())
        {
            // Adds default layers (3D scene)
            if(layer_map.second->isCoreCompositorEnabled())
            {
                const std::string id  = layer_map.first;
                std::string render_id = render->get_id();
                m_layersBox->addItem(QString::fromStdString(render_id + " : " + id));
                m_layers.push_back(layer_map.second);
            }
        }
    }

    m_layersBox->setCurrentIndex(-1);
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

void core_compositor_editor::onSelectedLayerItem(int _index)
{
    namespace compositor = sight::viz::scene3d::compositor;

    if(!m_isLayerSelected)
    {
        m_transparencyDepthSlider->setEnabled(true);
        m_buttonDefault->setEnabled(true);
        m_buttonDepthPeeling->setEnabled(true);
        m_buttonDualDepthPeeling->setEnabled(true);
        m_buttonWeightedBlendedOIT->setEnabled(true);
        m_buttonHybridTransparency->setEnabled(true);
        m_buttonCelShadingDepthPeeling->setEnabled(true);
        m_labelDefault->setEnabled(true);
        m_labelDepthPeeling->setEnabled(true);
        m_labelDualDepthPeeling->setEnabled(true);
        m_labelWeightedBlendedOIT->setEnabled(true);
        m_labelHybridTransparency->setEnabled(true);
        m_labelCelShadingDepthPeeling->setEnabled(true);
    }

    // Reloads buttons to match layer's parameters
    m_currentLayer = m_layers[static_cast<std::size_t>(_index)];

    // If the layer is not yet started, we can't use its default compositor
    auto layer = m_currentLayer.lock();
    if(layer)
    {
        switch(layer->getTransparencyTechnique())
        {
            case compositor::DEFAULT:
                m_transparencyButtonGroup->button(0)->setChecked(true);
                break;

            case compositor::DEPTHPEELING:
                m_transparencyButtonGroup->button(1)->setChecked(true);
                break;

            case compositor::DUALDEPTHPEELING:
                m_transparencyButtonGroup->button(2)->setChecked(true);
                break;

            case compositor::WEIGHTEDBLENDEDOIT:
                m_transparencyButtonGroup->button(3)->setChecked(true);
                break;

            case compositor::HYBRIDTRANSPARENCY:
                m_transparencyButtonGroup->button(4)->setChecked(true);
                break;

            case compositor::CELLSHADING_DEPTHPEELING:
                m_transparencyButtonGroup->button(5)->setChecked(true);
                break;
        }

        m_transparencyDepthSlider->setValue(layer->getTransparencyDepth());
    }
}

//------------------------------------------------------------------------------

void core_compositor_editor::onEditTransparencyDepth(int _depth)
{
    auto layer = m_currentLayer.lock();
    if(layer)
    {
        layer->setTransparencyDepth(_depth);
    }
}

//------------------------------------------------------------------------------

void core_compositor_editor::onEditTransparency(int _index)
{
    namespace compositor = sight::viz::scene3d::compositor;

    auto layer = m_currentLayer.lock();
    if(layer)
    {
        bool transparency_updated = false;
        switch(_index)
        {
            case 0:
                transparency_updated = layer->setTransparencyTechnique(compositor::DEFAULT);
                break;

            case 1:
                transparency_updated = layer->setTransparencyTechnique(compositor::DEPTHPEELING);
                break;

            case 2:
                transparency_updated = layer->setTransparencyTechnique(compositor::DUALDEPTHPEELING);
                break;

            case 3:
                transparency_updated = layer->setTransparencyTechnique(compositor::WEIGHTEDBLENDEDOIT);
                break;

            case 4:
                transparency_updated = layer->setTransparencyTechnique(compositor::HYBRIDTRANSPARENCY);
                break;

            case 5:
                transparency_updated = layer->setTransparencyTechnique(compositor::CELLSHADING_DEPTHPEELING);
                break;

            default:
                transparency_updated = false;
        }

        if(!transparency_updated)
        {
            m_transparencyButtonGroup->button(0)->setChecked(true);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
