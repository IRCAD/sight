/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "uiVisuOgre/SLightSelector.hpp"

#include "uiVisuOgre/helper/Utils.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/Composite.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRenderOgre/SRender.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <OGRE/OgreColourValue.h>

#include <QColor>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

namespace uiVisuOgre
{

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiVisuOgre::SLightSelector, ::fwData::Composite )

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType s_LIGHT_SELECTED_SIG = "lightSelected";
const core::com::Slots::SlotKeyType s_INIT_LIGHT_LIST_SLOT = "initLightList";

//------------------------------------------------------------------------------

SLightSelector::SLightSelector() noexcept
{
    newSignal<LightSelectedSignalType>(s_LIGHT_SELECTED_SIG);
    newSlot(s_INIT_LIGHT_LIST_SLOT, &SLightSelector::initLightList, this);
}

//------------------------------------------------------------------------------

SLightSelector::~SLightSelector() noexcept
{
}

//------------------------------------------------------------------------------

void SLightSelector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SLightSelector::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());

    m_layersBox       = new QComboBox();
    m_lightsList      = new QListWidget();
    m_addLightBtn     = new QPushButton("Add light");
    m_removeLightBtn  = new QPushButton("Remove light");
    m_ambientColorBtn = new QPushButton("Scene ambient color");

    QHBoxLayout* layoutButton = new QHBoxLayout;
    m_checkAllButton = new QPushButton(tr("Check all"));
    layoutButton->addWidget(m_checkAllButton, 0);

    m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
    layoutButton->addWidget(m_unCheckAllButton, 0);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addLayout(layoutButton);
    layout->addWidget(m_lightsList);

    QHBoxLayout* addRemoveLayout = new QHBoxLayout();
    addRemoveLayout->addWidget(m_addLightBtn);
    addRemoveLayout->addWidget(m_removeLightBtn);
    m_removeLightBtn->setEnabled(false);

    layout->addLayout(addRemoveLayout);
    layout->addWidget(m_ambientColorBtn);

    qtContainer->setLayout(layout);

    this->refreshLayers();

    this->updateLightsList();

    QObject::connect(m_layersBox,  qOverload<int>(&QComboBox::activated), this, &SLightSelector::onSelectedLayerItem);
    QObject::connect(m_lightsList, &QListWidget::currentItemChanged, this, &SLightSelector::onSelectedLightItem);
    QObject::connect(m_lightsList, &QListWidget::itemChanged, this, &SLightSelector::onCheckedLightItem);
    QObject::connect(m_addLightBtn, &QPushButton::clicked, this, &SLightSelector::onAddLight);
    QObject::connect(m_removeLightBtn, &QPushButton::clicked, this, &SLightSelector::onRemoveLight);
    QObject::connect(m_ambientColorBtn, &QPushButton::clicked, this, &SLightSelector::onEditAmbientColor);
    QObject::connect(m_checkAllButton, &QPushButton::clicked, this, &SLightSelector::onCheckAllCheckBox);
    QObject::connect(m_unCheckAllButton, &QPushButton::clicked, this, &SLightSelector::onUnCheckAllCheckBox);
}

//------------------------------------------------------------------------------

void SLightSelector::updating()
{
}

//------------------------------------------------------------------------------

void SLightSelector::stopping()
{
    m_connections.disconnect();

    for(auto& light : m_managedLightAdaptors)
    {
        ::fwRenderOgre::ILight::destroyLightAdaptor(light.m_light);
    }
    m_managedLightAdaptors.clear();

    this->destroy();
}

//------------------------------------------------------------------------------

void SLightSelector::onSelectedLayerItem(int _index)
{
    m_currentLayer  = m_layers[static_cast<size_t>(_index)];
    m_lightAdaptors = m_currentLayer.lock()->getLightAdaptors();

    this->updateLightsList();
}

//------------------------------------------------------------------------------

void SLightSelector::onSelectedLightItem(QListWidgetItem* _item, QListWidgetItem*)
{
    if(_item)
    {
        m_currentLight = this->retrieveLightAdaptor(_item->text().toStdString());

        auto sig = this->signal<LightSelectedSignalType>(s_LIGHT_SELECTED_SIG);
        sig->asyncEmit(m_currentLight);

        m_removeLightBtn->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void SLightSelector::onCheckedLightItem(QListWidgetItem* _item)
{
    ::fwRenderOgre::ILight::sptr checkedLightAdaptor =
        this->retrieveLightAdaptor(_item->text().toStdString());

    checkedLightAdaptor->switchOn(_item->checkState() == ::Qt::Checked);
}

//------------------------------------------------------------------------------

void SLightSelector::onAddLight(bool)
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();

    ::uiVisuOgre::NewLightDialog* lightDialog = new ::uiVisuOgre::NewLightDialog(container);

    if(lightDialog->exec() == QDialog::Accepted)
    {
        std::string lightName = lightDialog->property("lightName").toString().toStdString();

        auto existingLight =
            std::find_if(m_lightAdaptors.begin(), m_lightAdaptors.end(),
                         [lightName](::fwRenderOgre::ILight::sptr lightAdaptor)
            {
                return lightAdaptor->getName() == lightName;
            });

        if(existingLight == m_lightAdaptors.end())
        {
            this->createLightAdaptor(lightName);
        }
    }
}

//------------------------------------------------------------------------------

void SLightSelector::onRemoveLight(bool)
{
    if(m_currentLight)
    {
        ::fwRenderOgre::Layer::sptr currentLayer = m_currentLayer.lock();

        const std::vector< Light >::iterator position
            = std::find_if(m_managedLightAdaptors.begin(), m_managedLightAdaptors.end(), [&](const Light& _light)
            {
                return _light.m_light == m_currentLight;
            });

        if (position != m_managedLightAdaptors.end())
        {
            m_managedLightAdaptors.erase(position);
        }

        if(currentLayer->isDefaultLight(m_currentLight))
        {
            currentLayer->removeDefaultLight();
        }
        else
        {
            ::fwRenderOgre::ILight::destroyLightAdaptor(m_currentLight);
        }
        m_currentLight.reset();

        m_lightAdaptors = currentLayer->getLightAdaptors();
        this->updateLightsList();

        m_removeLightBtn->setEnabled(false);

        auto sig = this->signal<LightSelectedSignalType>(s_LIGHT_SELECTED_SIG);
        sig->asyncEmit(nullptr);

        currentLayer->requestRender();
    }
}

//------------------------------------------------------------------------------

void SLightSelector::onEditAmbientColor(bool)
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer());
    QWidget* const container = qtContainer->getQtContainer();

    ::fwRenderOgre::Layer::sptr layer = m_currentLayer.lock();
    ::Ogre::ColourValue ogreColor     = layer->getSceneManager()->getAmbientLight();

    QColor qColor = QColorDialog::getColor(::uiVisuOgre::helper::Utils::converOgreColorToQColor(ogreColor),
                                           container,
                                           "Scene ambient color");

    ogreColor = ::uiVisuOgre::helper::Utils::convertQColorToOgreColor(qColor);

    layer->getSceneManager()->setAmbientLight(ogreColor);
    layer->requestRender();
}

//------------------------------------------------------------------------------

void SLightSelector::initLightList(::fwRenderOgre::Layer::sptr _layer)
{
    m_currentLayer = m_layers[0];

    if(_layer == m_currentLayer.lock())
    {
        this->onSelectedLayerItem(0);
    }
}

//------------------------------------------------------------------------------

void SLightSelector::refreshLayers()
{
    m_layersBox->clear();

    ::fwServices::registry::ObjectService::ServiceVectorType renderers =
        ::fwServices::OSR::getServices("::fwRenderOgre::SRender");

    // Fills layer combo box with all enabled layers of each render services
    for(auto srv : renderers)
    {
        ::fwRenderOgre::SRender::sptr render = ::fwRenderOgre::SRender::dynamicCast(srv);

        for(auto& layerMap : render->getLayers())
        {
            const std::string id       = layerMap.first;
            const std::string renderID = render->getID();
            m_layersBox->addItem(QString::fromStdString(renderID + " : " + id));
            m_layers.push_back(layerMap.second);

            m_connections.connect(layerMap.second, ::fwRenderOgre::Layer::s_INIT_LAYER_SIG,
                                  this->getSptr(), s_INIT_LIGHT_LIST_SLOT);
        }
    }

    // Default to the first layer
    if(!m_layers.empty())
    {
        m_currentLayer  = m_layers[0];
        m_lightAdaptors = m_currentLayer.lock()->getLightAdaptors();
    }
}

//------------------------------------------------------------------------------

void SLightSelector::updateLightsList()
{
    m_lightsList->clear();

    for(auto lightAdaptor : m_lightAdaptors)
    {
        QString lightName = lightAdaptor->getName().c_str();
        ::Qt::CheckState lightState = lightAdaptor->isSwitchedOn() ? ::Qt::Checked :
                                      ::Qt::Unchecked;

        QListWidgetItem* nextLight = new QListWidgetItem(lightName, m_lightsList);
        nextLight->setFlags(nextLight->flags() | ::Qt::ItemIsUserCheckable);
        nextLight->setCheckState(lightState);
    }
}

//------------------------------------------------------------------------------

void SLightSelector::createLightAdaptor(const std::string& _name)
{
    ::fwRenderOgre::Layer::sptr currentLayer = m_currentLayer.lock();

    if(currentLayer)
    {
        ::fwData::Color::sptr lightDiffuseColor  = ::fwData::Color::New();
        ::fwData::Color::sptr lightSpecularColor = ::fwData::Color::New();

        ::fwRenderOgre::ILight::sptr lightAdaptor = ::fwRenderOgre::ILight::createLightAdaptor(lightDiffuseColor,
                                                                                               lightSpecularColor);
        lightAdaptor->setType(::Ogre::Light::LT_DIRECTIONAL);
        lightAdaptor->setLayerID(currentLayer->getLayerID());
        lightAdaptor->setRenderService(currentLayer->getRenderService());
        ::fwServices::IService::ConfigType config;
        config.add("config.<xmlattr>.name", this->getID() + "_light");
        config.add("config.<xmlattr>.layer", currentLayer->getLayerID());
        lightAdaptor->setConfiguration(config);
        lightAdaptor->configure();
        lightAdaptor->setName(_name);
        lightAdaptor->start();

        m_managedLightAdaptors.push_back({lightAdaptor, lightDiffuseColor, lightSpecularColor});
        m_lightAdaptors = currentLayer->getLightAdaptors();
        this->updateLightsList();

        ::fwServices::registry::ObjectService::ServiceVectorType materialServices =
            ::fwServices::OSR::getServices("::visuOgreAdaptor::SMaterial");

        for(auto srv : materialServices)
        {
            ::fwRenderOgre::IAdaptor::sptr materialAdaptor = ::fwRenderOgre::IAdaptor::dynamicCast(srv);

            if(materialAdaptor->getLayerID() == currentLayer->getLayerID())
            {
                // Update materials of the scene to take the new light into account
                materialAdaptor->update();
            }
        }
    }
}

//------------------------------------------------------------------------------

::fwRenderOgre::ILight::sptr SLightSelector::retrieveLightAdaptor(const std::string& _name) const
{
    auto it = std::find_if(m_lightAdaptors.begin(), m_lightAdaptors.end(),
                           [_name](::fwRenderOgre::ILight::sptr lightAdaptor)
        {
            return lightAdaptor->getName() == _name;
        });

    return it != m_lightAdaptors.end() ? *it : nullptr;
}

//------------------------------------------------------------------------------

void SLightSelector::onCheckAllCheckBox()
{
    this->onCheckAllBoxes(true);
}

//------------------------------------------------------------------------------

void SLightSelector::onUnCheckAllCheckBox()
{
    this->onCheckAllBoxes(false);
}

//------------------------------------------------------------------------------

void SLightSelector::onCheckAllBoxes( bool _visible )
{
    for( int i = 0; i < m_lightsList->count(); ++i )
    {
        auto item = m_lightsList->item( i );
        item->setCheckState(_visible ? ::Qt::Checked : ::Qt::Unchecked );
    }
}

//------------------------------------------------------------------------------

NewLightDialog::NewLightDialog(QWidget* _parent) :
    QDialog(_parent)
{
    m_lightNameLbl  = new QLabel("Name :", this);
    m_lightNameEdit = new QLineEdit(this);

    m_okBtn = new QPushButton("Ok", this);

    QHBoxLayout* lightNameLayout = new QHBoxLayout();
    lightNameLayout->addWidget(m_lightNameLbl);
    lightNameLayout->addWidget(m_lightNameEdit);

    QVBoxLayout* newLightLayout = new QVBoxLayout();
    newLightLayout->addLayout(lightNameLayout);
    newLightLayout->addWidget(m_okBtn);

    this->setWindowTitle("New light");
    this->setModal(true);
    this->setLayout(newLightLayout);

    QObject::connect(m_okBtn, &QPushButton::clicked, this, &NewLightDialog::onOkBtn);
}

//------------------------------------------------------------------------------

NewLightDialog::~NewLightDialog()
{
    QObject::disconnect(m_okBtn, &QPushButton::clicked, this, &NewLightDialog::onOkBtn);
}

//------------------------------------------------------------------------------

void NewLightDialog::onOkBtn(bool)
{
    if(!m_lightNameEdit->text().isEmpty())
    {
        this->setProperty("lightName", QVariant(m_lightNameEdit->text()));
        this->accept();
    }
}

//------------------------------------------------------------------------------

} // namespace uiVisuOgre
