/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuOgre/SCoreCompositorEditor.hpp"

#include <fwData/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/compositor/Core.hpp>

#include <material/Plugin.hpp>

#include <QWidget>
#include <QVBoxLayout>

#include <OGRE/OgreCompositorManager.h>
#include <OGRE/OgreResource.h>
#include <OGRE/OgreResourceManager.h>

#include <QAbstractButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QComboBox>
#include <QLineEdit>
#include <QSlider>
#include <QCheckBox>
#include <QDoubleSpinBox>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisuOgre::SCoreCompositorEditor, ::fwData::Composite);

//------------------------------------------------------------------------------

SCoreCompositorEditor::SCoreCompositorEditor() throw() :
    m_currentCoreCompositor(nullptr),
    m_isLayerSelected(false)
{
}

//------------------------------------------------------------------------------

SCoreCompositorEditor::~SCoreCompositorEditor() throw()
{
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::starting() throw(::fwTools::Failed)
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );
    m_container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instantiated", m_container);

    QVBoxLayout* layout = new QVBoxLayout();

    QLabel* labelLayerSelector = new QLabel(tr("3D layer selected"),m_container);
    layout->addWidget(labelLayerSelector);
    m_layersBox = new QComboBox(m_container);
    layout->addWidget(m_layersBox);

    // --------------------------------------
    // Farid : try to add some SAO cell here
    // --------------------------------------
    {
        // add label
        QLabel* labelSAO = new QLabel(tr("SAO"),m_container);
        layout->addWidget(labelSAO);
        // add Check box
        m_SAOCheckBox = new QCheckBox(m_container);
        m_SAOCheckBox->setCheckable(false);
        m_SAOCheckBox->setCheckState(Qt::CheckState(false));
        layout->addWidget(m_SAOCheckBox);

        // ajout d'un bouton pour activer/desactiver le blend
        // add Label
        QLabel* labelBlend = new QLabel(tr("Blend"),m_container);
        layout->addWidget(labelBlend);
        // add CheckBox
        m_SAOBlend = new QCheckBox(m_container);
        m_SAOBlend->setCheckState(Qt::CheckState(false)); // the blend is enable by default
        m_SAOBlend->setCheckable(false);
        layout->addWidget(m_SAOBlend);

        // ajout d'un doubleSpinBox pour pouvoir modifier la valeur du rayon directement depuis l'interface

        // radius label
        QLabel* labelRadius = new QLabel(tr("Radius"),m_container);
        layout->addWidget(labelRadius);
        // add Double Spin Box
        m_SAORadius = new QDoubleSpinBox(m_container);
        m_SAORadius->setRange(0.01,3.00);
//        m_SAORadius->setValue(m_currentLayer.lock()->getSaoManager().getSaoRadius());
        m_SAORadius->setValue(0.85);
        m_SAORadius->setSingleStep(0.05);
        // by defaut this spin box in disable
        m_SAORadius->setEnabled(false);
        layout->addWidget(m_SAORadius);

        // add a SpinBox to change the number of Samples
        // label
        QLabel* labelSamples = new QLabel(tr("Samples"),m_container);
        layout->addWidget(labelSamples);
        // add Spin Box
        m_SAOSamples = new QSpinBox(m_container);
        m_SAOSamples->setRange(1,30);
        m_SAOSamples->setSingleStep(1);
//        m_SAOSamples->setValue(m_currentLayer.lock()->getSaoManager().getSaoSamples());
        m_SAOSamples->setValue(11);
        // disable by default
        m_SAOSamples->setEnabled(false);
        layout->addWidget(m_SAOSamples);

        // AO Intensity
        QLabel* labelAoIntensity = new QLabel(tr("Intensity(AO)"),m_container);
        layout->addWidget(labelAoIntensity);
        // Double Spin Box
        m_AoIntensity = new QDoubleSpinBox(m_container);
        m_AoIntensity->setValue(1.0);
        m_AoIntensity->setRange(0.1,5.0);
        m_AoIntensity->setSingleStep(0.1);
        m_AoIntensity->setEnabled(false);
        layout->addWidget(m_AoIntensity);

    }




    // Transparency depth managment
    {
        QLabel* labelTransparency = new QLabel(tr("Transparency depth"),m_container);
        layout->addWidget(labelTransparency);
        m_transparencyDepthSlider = new QSlider(Qt::Horizontal, m_container);
        layout->addWidget(m_transparencyDepthSlider);
        m_transparencyDepthSlider->setEnabled(false);
        m_transparencyDepthSlider->setTickInterval(1);
        m_transparencyDepthSlider->setTickPosition(QSlider::TicksBelow);
        m_transparencyDepthSlider->setRange(0, 12);
        m_transparencyDepthSlider->setValue(8);
    }

    // Transparency selector
    {
        QGroupBox* groupBox         = new QGroupBox(tr("Transparency technique"), m_container);
        QVBoxLayout* layoutGroupBox = new QVBoxLayout();
        groupBox->setLayout(layoutGroupBox);
        layout->addWidget(groupBox);

        m_transparencyButtonGroup = new QButtonGroup(groupBox);

        m_buttonDefault = new QRadioButton ( tr("Default"), groupBox );
        m_buttonDefault->setMinimumSize(m_buttonDefault->sizeHint());
        m_buttonDefault->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDefault, 0);
        layoutGroupBox->addWidget(m_buttonDefault);
        m_labelDefault = new QLabel(tr("<i>No Order Independent Transparency</i>"),m_container);
        m_labelDefault->setEnabled(false);
        layoutGroupBox->addWidget(m_labelDefault);

        m_buttonDepthPeeling = new QRadioButton ( tr("Depth Peeling"), groupBox );
        m_buttonDepthPeeling->setMinimumSize(m_buttonDepthPeeling->sizeHint());
        m_buttonDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDepthPeeling, 1);
        layoutGroupBox->addWidget(m_buttonDepthPeeling);
        m_labelDepthPeeling = new QLabel(tr("<i>Exact color blending but slowest technique</i>"),m_container);
        m_labelDepthPeeling->setEnabled(false);
        layoutGroupBox->addWidget(m_labelDepthPeeling);

        m_buttonDualDepthPeeling =
            new QRadioButton ( tr("Dual Depth Peeling"), groupBox );
        m_buttonDualDepthPeeling->setMinimumSize(m_buttonDualDepthPeeling->sizeHint());
        m_buttonDualDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonDualDepthPeeling, 2);
        layoutGroupBox->addWidget(m_buttonDualDepthPeeling);
        m_labelDualDepthPeeling = new QLabel(tr("<i>Exact color blending but slow technique</i>"),m_container);
        m_labelDualDepthPeeling->setEnabled(false);
        layoutGroupBox->addWidget(m_labelDualDepthPeeling);

        m_buttonWeightedBlendedOIT =
            new QRadioButton ( tr("Weighted Blended OIT"), groupBox );
        m_buttonWeightedBlendedOIT->setMinimumSize(m_buttonWeightedBlendedOIT->sizeHint());
        m_buttonWeightedBlendedOIT->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonWeightedBlendedOIT, 3);
        layoutGroupBox->addWidget(m_buttonWeightedBlendedOIT);
        m_labelWeightedBlendedOIT = new QLabel(tr("<i>Approximative color blending but fastest</i>"),m_container);
        m_labelWeightedBlendedOIT->setEnabled(false);
        layoutGroupBox->addWidget(m_labelWeightedBlendedOIT);

        m_buttonHybridTransparency =
            new QRadioButton ( tr("Hybrid transparency"), groupBox );
        m_buttonHybridTransparency->setMinimumSize(m_buttonHybridTransparency->sizeHint());
        m_buttonHybridTransparency->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonHybridTransparency, 4);
        layoutGroupBox->addWidget(m_buttonHybridTransparency);
        m_labelHybridTransparency =
            new QLabel(tr("<i>Depth Peeling + Weighted Blended OIT = half exact half fast</i>"),m_container);
        m_labelHybridTransparency->setEnabled(false);
        layoutGroupBox->addWidget(m_labelHybridTransparency);

        m_buttonCelShadingDepthPeeling = new QRadioButton ( tr("CelShading + Depth Peeling"), groupBox );
        m_buttonCelShadingDepthPeeling->setMinimumSize(m_buttonDepthPeeling->sizeHint());
        m_buttonCelShadingDepthPeeling->setEnabled(false);
        m_transparencyButtonGroup->addButton(m_buttonCelShadingDepthPeeling, 5);
        layoutGroupBox->addWidget(m_buttonCelShadingDepthPeeling);
        m_labelCelShadingDepthPeeling = new QLabel(tr(
                                                       "<i>Depth peeling with an edge detection per layer.</i>"),
                                                   m_container);
        m_labelCelShadingDepthPeeling->setEnabled(false);
        layoutGroupBox->addWidget(m_labelCelShadingDepthPeeling);
    }

    m_container->setLayout( layout );

    this->refreshRenderers();

    QObject::connect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));
    QObject::connect( m_transparencyDepthSlider, SIGNAL(valueChanged(int)), this, SLOT(onEditTransparencyDepth(int)) );
    QObject::connect(m_transparencyButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onEditTransparency(int)));

    // here we connect the sao check box with the correct method
    QObject::connect(m_SAOCheckBox,SIGNAL(stateChanged(int)),this,SLOT(onSaoCheck(int)));

    // connection between the radius spin box and the SAO Compositor Manager Class
    QObject::connect(m_SAORadius,SIGNAL(valueChanged(double)),this,SLOT(onSaoRadiusChange(double)));

    // same for the samples number
    QObject::connect(m_SAOSamples,SIGNAL(valueChanged(int)),this,SLOT(onSaoSampleChange(int)));

    // connection blend checkbox - slot
    QObject::connect(m_SAOBlend,SIGNAL(stateChanged(int)),this,SLOT(onSaoBlendChange(int)));

    // connection AoIntensity DoubleSpinBox - SaoCompositor Class
    QObject::connect(m_AoIntensity,SIGNAL(valueChanged(double)),this,SLOT(onAoIntensityChange(double)));


}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::stopping() throw(::fwTools::Failed)
{
    QObject::disconnect(m_layersBox, SIGNAL(activated(int)), this, SLOT(onSelectedLayerItem(int)));

    this->getContainer()->clean();
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

void SCoreCompositorEditor::configuring() throw(::fwTools::Failed)
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::updating() throw(::fwTools::Failed)
{
    m_currentCoreCompositor->update();
    m_currentLayer.lock()->requestRender();
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
        // here we enable the Sao button
        m_SAOCheckBox->setCheckable(true);
    }

    // Reloads buttons to match layer's parameters
    m_currentLayer          = m_layers[static_cast<size_t>(index)];
    m_currentCoreCompositor = m_currentLayer.lock()->getCoreCompositor();

    // If the layer is not yet started, we can't use its default compositor
    if(m_currentCoreCompositor)
    {
        switch (m_currentCoreCompositor->getTransparencyTechnique())
        {
            case DEFAULT:
                m_transparencyButtonGroup->button(0)->setChecked(true);
                break;
            case DEPTHPEELING:
                m_transparencyButtonGroup->button(1)->setChecked(true);
                break;
            case DUALDEPTHPEELING:
                m_transparencyButtonGroup->button(2)->setChecked(true);
                break;
            case WEIGHTEDBLENDEDOIT:
                m_transparencyButtonGroup->button(3)->setChecked(true);
                break;
            case HYBRIDTRANSPARENCY:
                m_transparencyButtonGroup->button(4)->setChecked(true);
                break;
            case CELSHADING_DEPTHPEELING:
                m_transparencyButtonGroup->button(5)->setChecked(true);
                break;
        }
        m_transparencyDepthSlider->setValue(m_currentCoreCompositor->getTransparencyDepth());
        this->update();
    }
//    */
//    this->update();
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onEditTransparencyDepth(int depth)
{
    if(m_currentCoreCompositor)
    {
        m_currentCoreCompositor->setTransparencyDepth(depth);
        this->update();
    }
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onEditTransparency(int index)
{
    if(m_currentCoreCompositor)
    {
        bool transparencyUpdated = false;
        switch (index)
        {
            case 0:
                transparencyUpdated = m_currentCoreCompositor->setTransparencyTechnique(DEFAULT);
                break;
            case 1:
                transparencyUpdated = m_currentCoreCompositor->setTransparencyTechnique(DEPTHPEELING);
                break;
            case 2:
                transparencyUpdated = m_currentCoreCompositor->setTransparencyTechnique(DUALDEPTHPEELING);
                break;
            case 3:
                transparencyUpdated = m_currentCoreCompositor->setTransparencyTechnique(WEIGHTEDBLENDEDOIT);
                break;
            case 4:
                transparencyUpdated = m_currentCoreCompositor->setTransparencyTechnique(HYBRIDTRANSPARENCY);
                break;
            case 5:
                transparencyUpdated = m_currentCoreCompositor->setTransparencyTechnique(CELSHADING_DEPTHPEELING);
                break;
        }

        if(!transparencyUpdated)
        {
            m_transparencyButtonGroup->button(0)->setChecked(true);
        }

        this->update();
    }
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onSaoCheck(int state)
{
    // need to change the behaviour of the 3D layer selector -> when selected a good layer, set enable the sao Button
    m_currentLayer.lock()->getSaoManager().setSaoState(state == Qt::Checked);
    // here we can enable/disable the parameters
    m_SAORadius->setEnabled(state);
    m_SAOSamples->setEnabled(state);
    m_SAOBlend->setCheckable(state);
    m_AoIntensity->setEnabled(state);

    this->update();
}


//------------------------------------------------------------------------------


void SCoreCompositorEditor::onSaoRadiusChange(double value)
{
    // change the value of the radius in the SAO Chain Manager class
    m_currentLayer.lock()->getSaoManager().setSaoRadius(value);
    this->update();
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onSaoSampleChange(int value)
{
    // change the value in the Sao Chain Manager
    m_currentLayer.lock()->getSaoManager().setSaoSamples(value);
    this->update();
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onSaoBlendChange(int state)
{

    m_currentLayer.lock()->getSaoManager().enableBlend(state == Qt::Checked);
    this->update();
}

//------------------------------------------------------------------------------

void SCoreCompositorEditor::onAoIntensityChange(double value)
{
    // change the value of the radius in the SAO Chain Manager class
    m_currentLayer.lock()->getSaoManager().setAoIntensity(value);
    this->update();
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
