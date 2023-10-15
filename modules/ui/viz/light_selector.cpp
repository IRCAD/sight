/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "light_selector.hpp"

#include "modules/ui/viz/helper/Utils.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/color.hpp>
#include <data/composite.hpp>
#include <data/matrix4.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/render.hpp>

#include <OGRE/OgreColourValue.h>

#include <QColor>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::viz
{

using sight::viz::scene3d::ILight;
using sight::viz::scene3d::Layer;

//------------------------------------------------------------------------------

const core::com::signals::key_t LIGHT_SELECTED_SIG = "lightSelected";
const core::com::slots::key_t INIT_LIGHT_LIST_SLOT = "initLightList";

//------------------------------------------------------------------------------

light_selector::light_selector() noexcept
{
    new_signal<LightSelectedSignalType>(LIGHT_SELECTED_SIG);
    new_slot(INIT_LIGHT_LIST_SLOT, &light_selector::initLightList, this);
}

//------------------------------------------------------------------------------

light_selector::~light_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void light_selector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void light_selector::starting()
{
    this->create();

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    qtContainer->getQtContainer()->setObjectName(serviceID);

    m_layersBox = new QComboBox();
    m_layersBox->setObjectName(serviceID + "/layersBox");
    m_lightsList = new QListWidget();
    m_lightsList->setObjectName(serviceID + "/lightsList");
    m_addLightBtn = new QPushButton("Add light");
    m_addLightBtn->setObjectName(serviceID + "/" + m_addLightBtn->text());
    m_removeLightBtn = new QPushButton("Remove light");
    m_removeLightBtn->setObjectName(serviceID + "/" + m_removeLightBtn->text());
    m_ambientColorBtn = new QPushButton("Scene ambient color");
    m_ambientColorBtn->setObjectName(serviceID + "/" + m_ambientColorBtn->text());

    auto* layoutButton = new QHBoxLayout;
    m_checkAllButton = new QPushButton(tr("Check all"));
    m_checkAllButton->setObjectName(serviceID + "/" + m_checkAllButton->text());
    layoutButton->addWidget(m_checkAllButton, 0);

    m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
    m_unCheckAllButton->setObjectName(serviceID + "/" + m_unCheckAllButton->text());
    layoutButton->addWidget(m_unCheckAllButton, 0);

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addLayout(layoutButton);
    layout->addWidget(m_lightsList);

    auto* addRemoveLayout = new QHBoxLayout();
    addRemoveLayout->addWidget(m_addLightBtn);
    addRemoveLayout->addWidget(m_removeLightBtn);
    m_removeLightBtn->setEnabled(false);

    layout->addLayout(addRemoveLayout);
    layout->addWidget(m_ambientColorBtn);

    qtContainer->setLayout(layout);

    this->refreshLayers();

    this->updateLightsList();

    QObject::connect(m_layersBox, qOverload<int>(&QComboBox::activated), this, &light_selector::onSelectedLayerItem);
    QObject::connect(m_lightsList, &QListWidget::currentItemChanged, this, &light_selector::onSelectedLightItem);
    QObject::connect(m_lightsList, &QListWidget::itemChanged, this, &light_selector::onCheckedLightItem);
    QObject::connect(m_addLightBtn, &QPushButton::clicked, this, &light_selector::onAddLight);
    QObject::connect(m_removeLightBtn, &QPushButton::clicked, this, &light_selector::onRemoveLight);
    QObject::connect(m_ambientColorBtn, &QPushButton::clicked, this, &light_selector::onEditAmbientColor);
    QObject::connect(m_checkAllButton, &QPushButton::clicked, this, &light_selector::onCheckAllCheckBox);
    QObject::connect(m_unCheckAllButton, &QPushButton::clicked, this, &light_selector::onUnCheckAllCheckBox);
}

//------------------------------------------------------------------------------

void light_selector::updating()
{
}

//------------------------------------------------------------------------------

void light_selector::stopping()
{
    m_connections.disconnect();

    for(auto& light : m_managedLightAdaptors)
    {
        ILight::destroyLightAdaptor(light.m_light);
    }

    m_managedLightAdaptors.clear();

    this->destroy();
}

//------------------------------------------------------------------------------

void light_selector::onSelectedLayerItem(int _index)
{
    m_currentLayer  = m_layers[static_cast<std::size_t>(_index)];
    m_lightAdaptors = m_currentLayer.lock()->getLightAdaptors();

    this->updateLightsList();
}

//------------------------------------------------------------------------------

void light_selector::onSelectedLightItem(QListWidgetItem* _item, QListWidgetItem* /*unused*/)
{
    if(_item != nullptr)
    {
        m_currentLight = this->retrieveLightAdaptor(_item->text().toStdString());

        auto sig = this->signal<LightSelectedSignalType>(LIGHT_SELECTED_SIG);
        sig->async_emit(m_currentLight);

        m_removeLightBtn->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void light_selector::onCheckedLightItem(QListWidgetItem* _item)
{
    ILight::sptr checkedLightAdaptor =
        this->retrieveLightAdaptor(_item->text().toStdString());

    checkedLightAdaptor->switchOn(_item->checkState() == Qt::Checked);
}

//------------------------------------------------------------------------------

void light_selector::onAddLight(bool /*unused*/)
{
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    QWidget* const container = qtContainer->getQtContainer();

    auto* lightDialog = new module::ui::viz::NewLightDialog(container);

    if(lightDialog->exec() == QDialog::Accepted)
    {
        std::string lightName = lightDialog->property("lightName").toString().toStdString();

        auto existingLight =
            std::find_if(
                m_lightAdaptors.begin(),
                m_lightAdaptors.end(),
                [lightName](ILight::sptr lightAdaptor)
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

void light_selector::onRemoveLight(bool /*unused*/)
{
    if(m_currentLight)
    {
        Layer::sptr currentLayer = m_currentLayer.lock();

        const auto position =
            std::find_if(
                m_managedLightAdaptors.begin(),
                m_managedLightAdaptors.end(),
                [&](const Light& _light)
            {
                return _light.m_light == m_currentLight;
            });

        if(position != m_managedLightAdaptors.end())
        {
            m_managedLightAdaptors.erase(position);
        }

        if(currentLayer->isDefaultLight(m_currentLight))
        {
            currentLayer->removeDefaultLight();
        }
        else
        {
            ILight::destroyLightAdaptor(m_currentLight);
        }

        m_currentLight.reset();

        m_lightAdaptors = currentLayer->getLightAdaptors();
        this->updateLightsList();

        m_removeLightBtn->setEnabled(false);

        auto sig = this->signal<LightSelectedSignalType>(LIGHT_SELECTED_SIG);
        sig->async_emit(nullptr);

        currentLayer->requestRender();
    }
}

//------------------------------------------------------------------------------

void light_selector::onEditAmbientColor(bool /*unused*/)
{
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    QWidget* const container = qtContainer->getQtContainer();

    Layer::sptr layer           = m_currentLayer.lock();
    Ogre::ColourValue ogreColor = layer->getSceneManager()->getAmbientLight();

    QColor qColor = QColorDialog::getColor(
        module::ui::viz::helper::Utils::convertOgreColorToQColor(ogreColor),
        container,
        "Scene ambient color"
    );

    ogreColor = module::ui::viz::helper::Utils::convertQColorToOgreColor(qColor);

    layer->getSceneManager()->setAmbientLight(ogreColor);
    layer->requestRender();
}

//------------------------------------------------------------------------------

void light_selector::initLightList(Layer::sptr _layer)
{
    m_currentLayer = m_layers[0];

    if(_layer == m_currentLayer.lock())
    {
        this->onSelectedLayerItem(0);
    }
}

//------------------------------------------------------------------------------

void light_selector::refreshLayers()
{
    m_layersBox->clear();

    const auto renderers = sight::service::getServices("sight::viz::scene3d::render");

    // Fills layer combo box with all enabled layers of each render services
    for(const auto& srv : renderers)
    {
        auto render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layerMap : render->getLayers())
        {
            const std::string id = layerMap.first;
            std::string renderID = render->get_id();
            m_layersBox->addItem(QString::fromStdString(renderID + " : " + id));
            m_layers.push_back(layerMap.second);

            m_connections.connect(
                layerMap.second,
                Layer::INIT_LAYER_SIG,
                this->get_sptr(),
                INIT_LIGHT_LIST_SLOT
            );
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

void light_selector::updateLightsList()
{
    m_lightsList->clear();

    for(const auto& lightAdaptor : m_lightAdaptors)
    {
        QString lightName         = lightAdaptor->getName().c_str();
        Qt::CheckState lightState = lightAdaptor->isSwitchedOn() ? Qt::Checked
                                                                 : Qt::Unchecked;

        auto* nextLight = new QListWidgetItem(lightName, m_lightsList);
        nextLight->setFlags(nextLight->flags() | Qt::ItemIsUserCheckable);
        nextLight->setCheckState(lightState);
    }
}

//------------------------------------------------------------------------------

void light_selector::createLightAdaptor(const std::string& _name)
{
    Layer::sptr currentLayer = m_currentLayer.lock();

    if(currentLayer)
    {
        data::color::sptr lightDiffuseColor  = std::make_shared<data::color>();
        data::color::sptr lightSpecularColor = std::make_shared<data::color>();

        ILight::sptr lightAdaptor = ILight::createLightAdaptor(
            lightDiffuseColor,
            lightSpecularColor
        );
        lightAdaptor->setType(Ogre::Light::LT_DIRECTIONAL);
        lightAdaptor->setLayerID(currentLayer->getLayerID());
        lightAdaptor->setRenderService(currentLayer->getRenderService());
        service::config_t config;
        config.add("config.<xmlattr>.name", this->get_id() + "_light");
        config.add("config.<xmlattr>.layer", currentLayer->getLayerID());
        lightAdaptor->set_config(config);
        lightAdaptor->configure();
        lightAdaptor->setName(_name);
        lightAdaptor->start();

        m_managedLightAdaptors.push_back({lightAdaptor, lightDiffuseColor, lightSpecularColor});
        m_lightAdaptors = currentLayer->getLightAdaptors();
        this->updateLightsList();

        const auto materialServices = sight::service::getServices("sight::module::viz::scene3d::adaptor::material");

        for(const auto& srv : materialServices)
        {
            auto materialAdaptor = std::dynamic_pointer_cast<sight::viz::scene3d::adaptor>(srv);

            if(materialAdaptor->getLayerID() == currentLayer->getLayerID())
            {
                // Update materials of the scene to take the new light into account
                materialAdaptor->update();
            }
        }
    }
}

//------------------------------------------------------------------------------

ILight::sptr light_selector::retrieveLightAdaptor(const std::string& _name) const
{
    auto it = std::find_if(
        m_lightAdaptors.begin(),
        m_lightAdaptors.end(),
        [_name](ILight::sptr lightAdaptor)
        {
            return lightAdaptor->getName() == _name;
        });

    return it != m_lightAdaptors.end() ? *it : nullptr;
}

//------------------------------------------------------------------------------

void light_selector::onCheckAllCheckBox()
{
    this->onCheckAllBoxes(true);
}

//------------------------------------------------------------------------------

void light_selector::onUnCheckAllCheckBox()
{
    this->onCheckAllBoxes(false);
}

//------------------------------------------------------------------------------

void light_selector::onCheckAllBoxes(bool _visible)
{
    for(int i = 0 ; i < m_lightsList->count() ; ++i)
    {
        auto* item = m_lightsList->item(i);
        item->setCheckState(_visible ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

NewLightDialog::NewLightDialog(QWidget* _parent) :
    QDialog(_parent),
    m_lightNameEdit(new QLineEdit(this))
{
    m_lightNameLbl = new QLabel("Name :", this);

    m_okBtn = new QPushButton("Ok", this);

    auto* lightNameLayout = new QHBoxLayout();
    lightNameLayout->addWidget(m_lightNameLbl);
    lightNameLayout->addWidget(m_lightNameEdit);

    auto* newLightLayout = new QVBoxLayout();
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

void NewLightDialog::onOkBtn(bool /*unused*/)
{
    if(!m_lightNameEdit->text().isEmpty())
    {
        this->setProperty("lightName", QVariant(m_lightNameEdit->text()));
        this->accept();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
