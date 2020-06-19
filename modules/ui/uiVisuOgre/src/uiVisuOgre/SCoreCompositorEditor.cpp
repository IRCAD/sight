/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
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

#include "uiVisuOgre/SCoreCompositorEditor.hpp"

#include <fwData/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

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

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiVisuOgre::SCoreCompositorEditor, ::fwData::Composite)

//------------------------------------------------------------------------------

SCoreCompositorEditor::SCoreCompositorEditor() noexcept :
    m_isLayerSelected(false)
{
}

//------------------------------------------------------------------------------

SCoreCompositorEditor::~SCoreCompositorEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();

    QLabel* labelLayerSelector = new QLabel(tr("3D layer selected"));
    layout->addWidget(labelLayerSelector);
    m_layersBox = new QComboBox();
    layout->addWidget(m_layersBox);

    // Transparency depth managment
    {
        QLabel* labelTransparency = new QLabel(tr("Transparency depth"));
        layout->addWidget(labelTransparency);
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
        QGroupBox* groupBox         = new QGroupBox(tr("Transparency technique"));
        QVBoxLayout* layoutGroupBox = new QVBoxLayout();
        groupBox->setLayout(layoutGroupBox);
        layout->addWidget(groupBox);

        m_transparencyButtonGroup = new QButtonGroup(groupBox);

        m_buttonDefault = new QRadioButton( tr("Default"), groupBox );
        m_buttonDefault->setMinimumSize(m_buttonDefault->sizeHint());
        m_buttonDefault->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDefault, 0);
        layoutGroupBox->addWidget(m_buttonDefault);
        m_labelDefault = new QLabel(tr("<i>No Order Independent Transparency</i>"));
        m_labelDefault->setEnabled(false);
        layoutGroupBox->addWidget(m_labelDefault);

        m_buttonDepthPeeling = new QRadioButton( tr("Depth Peeling"), groupBox );
        m_buttonDepthPeeling->setMinimumSize(m_buttonDepthPeeling->sizeHint());
        m_buttonDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDepthPeeling, 1);
        layoutGroupBox->addWidget(m_buttonDepthPeeling);
        m_labelDepthPeeling = new QLabel(tr("<i>Exact color blending but slowest technique</i>"));
        m_labelDepthPeeling->setEnabled(false);
        layoutGroupBox->addWidget(m_labelDepthPeeling);

        m_buttonDualDepthPeeling =
            new QRadioButton( tr("Dual Depth Peeling"), groupBox );
        m_buttonDualDepthPeeling->setMinimumSize(m_buttonDualDepthPeeling->sizeHint());
        m_buttonDualDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDualDepthPeeling, 2);
        layoutGroupBox->addWidget(m_buttonDualDepthPeeling);
        m_labelDualDepthPeeling = new QLabel(tr("<i>Exact color blending but slow technique</i>"));
        m_labelDualDepthPeeling->setEnabled(false);
        layoutGroupBox->addWidget(m_labelDualDepthPeeling);

        m_buttonWeightedBlendedOIT =
            new QRadioButton( tr("Weighted Blended OIT"), groupBox );
        m_buttonWeightedBlendedOIT->setMinimumSize(m_buttonWeightedBlendedOIT->sizeHint());
        m_buttonWeightedBlendedOIT->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonWeightedBlendedOIT, 3);
        layoutGroupBox->addWidget(m_buttonWeightedBlendedOIT);
        m_labelWeightedBlendedOIT = new QLabel(tr("<i>Approximative color blending but fastest</i>"));
        m_labelWeightedBlendedOIT->setEnabled(false);
        layoutGroupBox->addWidget(m_labelWeightedBlendedOIT);

        m_buttonHybridTransparency =
            new QRadioButton( tr("Hybrid transparency"), groupBox );
        m_buttonHybridTransparency->setMinimumSize(m_buttonHybridTransparency->sizeHint());
        m_buttonHybridTransparency->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonHybridTransparency, 4);
        layoutGroupBox->addWidget(m_buttonHybridTransparency);
        m_labelHybridTransparency =
            new QLabel(tr("<i>Depth Peeling + Weighted Blended OIT = half exact half fast</i>"));
        m_labelHybridTransparency->setEnabled(false);
        layoutGroupBox->addWidget(m_labelHybridTransparency);

        m_buttonCelShadingDepthPeeling = new QRadioButton( tr("CelShading + Depth Peeling"), groupBox );
        m_buttonCelShadingDepthPeeling->setMinimumSize(m_buttonDepthPeeling->sizeHint());
        m_buttonCelShadingDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonCelShadingDepthPeeling, 5);
        layoutGroupBox->addWidget(m_buttonCelShadingDepthPeeling);
        m_labelCelShadingDepthPeeling = new QLabel(tr(
                                                       "<i>Depth peeling with an edge detection per layer.</i>"));
        m_labelCelShadingDepthPeeling->setEnabled(false);
        layoutGroupBox->addWidget(m_labelCelShadingDepthPeeling);
    }

    qtContainer->setLayout( layout );

    this->refreshRenderers();

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));
    QObject::connect( m_transparencyDepthSlider, SIGNAL(valueChanged(int)), this, SLOT(onEditTransparencyDepth(int)) );
    QObject::connect(m_transparencyButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onEditTransparency(int)));
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::refreshRenderers()
{
    m_layersBox->clear();

    // Fill layer box with all enabled layers
    ::fwServices::registry::ObjectService::ServiceVectorType renderers =
        ::fwServices::OSR::getServices("::fwRenderOgre::SRender");

    bool is3DLayer = false;
    for(auto srv : renderers)
    {
        ::fwRenderOgre::SRender::sptr render = ::fwRenderOgre::SRender::dynamicCast(srv);

        for(auto& layerMap : render->getLayers())
        {
            // Adds default layers (3D scene)
            if(layerMap.second->isCoreCompositorEnabled())
            {
                const std::string id       = layerMap.first;
                const std::string renderID = render->getID();
                m_layersBox->addItem(QString::fromStdString(renderID + " : " + id));
                m_layers.push_back(layerMap.second);
                is3DLayer = true;
            }
        }
    }

    m_layersBox->setCurrentIndex(-1);
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::updating()
{
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onSelectedLayerItem(int index)
{
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
    m_currentLayer = m_layers[static_cast<size_t>(index)];

    // If the layer is not yet started, we can't use its default compositor
    auto layer = m_currentLayer.lock();
    if(layer)
    {
        switch (layer->getTransparencyTechnique())
        {
            case ::fwRenderOgre::compositor::DEFAULT:
                m_transparencyButtonGroup->button(0)->setChecked(true);
                break;
            case ::fwRenderOgre::compositor::DEPTHPEELING:
                m_transparencyButtonGroup->button(1)->setChecked(true);
                break;
            case ::fwRenderOgre::compositor::DUALDEPTHPEELING:
                m_transparencyButtonGroup->button(2)->setChecked(true);
                break;
            case ::fwRenderOgre::compositor::WEIGHTEDBLENDEDOIT:
                m_transparencyButtonGroup->button(3)->setChecked(true);
                break;
            case ::fwRenderOgre::compositor::HYBRIDTRANSPARENCY:
                m_transparencyButtonGroup->button(4)->setChecked(true);
                break;
            case ::fwRenderOgre::compositor::CELSHADING_DEPTHPEELING:
                m_transparencyButtonGroup->button(5)->setChecked(true);
                break;
        }
        m_transparencyDepthSlider->setValue(layer->getTransparencyDepth());
    }
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onEditTransparencyDepth(int depth)
{
    auto layer = m_currentLayer.lock();
    if(layer)
    {
        layer->setTransparencyDepth(depth);
    }
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onEditTransparency(int index)
{
    auto layer = m_currentLayer.lock();
    if(layer)
    {
        bool transparencyUpdated = false;
        switch (index)
        {
            case 0:
                transparencyUpdated = layer->setTransparencyTechnique(
                    ::fwRenderOgre::compositor::DEFAULT);
                break;
            case 1:
                transparencyUpdated = layer->setTransparencyTechnique(
                    ::fwRenderOgre::compositor::DEPTHPEELING);
                break;
            case 2:
                transparencyUpdated = layer->setTransparencyTechnique(
                    ::fwRenderOgre::compositor::DUALDEPTHPEELING);
                break;
            case 3:
                transparencyUpdated = layer->setTransparencyTechnique(
                    ::fwRenderOgre::compositor::WEIGHTEDBLENDEDOIT);
                break;
            case 4:
                transparencyUpdated = layer->setTransparencyTechnique(
                    ::fwRenderOgre::compositor::HYBRIDTRANSPARENCY);
                break;
            case 5:
                transparencyUpdated = layer->setTransparencyTechnique(
                    ::fwRenderOgre::compositor::CELSHADING_DEPTHPEELING);
                break;
        }

        if(!transparencyUpdated)
        {
            m_transparencyButtonGroup->button(0)->setChecked(true);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
