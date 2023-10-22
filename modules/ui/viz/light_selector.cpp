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

#include "modules/ui/viz/helper/utils.hpp"

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

using sight::viz::scene3d::light_adaptor;
using sight::viz::scene3d::layer;

//------------------------------------------------------------------------------

const core::com::signals::key_t LIGHT_SELECTED_SIG = "lightSelected";
const core::com::slots::key_t INIT_LIGHT_LIST_SLOT = "initLightList";

//------------------------------------------------------------------------------

light_selector::light_selector() noexcept
{
    new_signal<light_selected_signal_t>(LIGHT_SELECTED_SIG);
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

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    qt_container->getQtContainer()->setObjectName(service_id);

    m_layersBox = new QComboBox();
    m_layersBox->setObjectName(service_id + "/layersBox");
    m_lightsList = new QListWidget();
    m_lightsList->setObjectName(service_id + "/lightsList");
    m_addLightBtn = new QPushButton("Add light");
    m_addLightBtn->setObjectName(service_id + "/" + m_addLightBtn->text());
    m_removeLightBtn = new QPushButton("Remove light");
    m_removeLightBtn->setObjectName(service_id + "/" + m_removeLightBtn->text());
    m_ambientColorBtn = new QPushButton("scene ambient color");
    m_ambientColorBtn->setObjectName(service_id + "/" + m_ambientColorBtn->text());

    auto* layout_button = new QHBoxLayout;
    m_checkAllButton = new QPushButton(tr("Check all"));
    m_checkAllButton->setObjectName(service_id + "/" + m_checkAllButton->text());
    layout_button->addWidget(m_checkAllButton, 0);

    m_unCheckAllButton = new QPushButton(tr("UnCheck all"));
    m_unCheckAllButton->setObjectName(service_id + "/" + m_unCheckAllButton->text());
    layout_button->addWidget(m_unCheckAllButton, 0);

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_layersBox);
    layout->addLayout(layout_button);
    layout->addWidget(m_lightsList);

    auto* add_remove_layout = new QHBoxLayout();
    add_remove_layout->addWidget(m_addLightBtn);
    add_remove_layout->addWidget(m_removeLightBtn);
    m_removeLightBtn->setEnabled(false);

    layout->addLayout(add_remove_layout);
    layout->addWidget(m_ambientColorBtn);

    qt_container->setLayout(layout);

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
        light_adaptor::destroyLightAdaptor(light.m_light);
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

        auto sig = this->signal<light_selected_signal_t>(LIGHT_SELECTED_SIG);
        sig->async_emit(m_currentLight);

        m_removeLightBtn->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void light_selector::onCheckedLightItem(QListWidgetItem* _item)
{
    light_adaptor::sptr checked_light_adaptor =
        this->retrieveLightAdaptor(_item->text().toStdString());

    checked_light_adaptor->switchOn(_item->checkState() == Qt::Checked);
}

//------------------------------------------------------------------------------

void light_selector::onAddLight(bool /*unused*/)
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    QWidget* const container = qt_container->getQtContainer();

    auto* light_dialog = new module::ui::viz::NewLightDialog(container);

    if(light_dialog->exec() == QDialog::Accepted)
    {
        std::string light_name = light_dialog->property("lightName").toString().toStdString();

        auto existing_light =
            std::find_if(
                m_lightAdaptors.begin(),
                m_lightAdaptors.end(),
                [light_name](light_adaptor::sptr _light_adaptor)
            {
                return _light_adaptor->getName() == light_name;
            });

        if(existing_light == m_lightAdaptors.end())
        {
            this->createLightAdaptor(light_name);
        }
    }
}

//------------------------------------------------------------------------------

void light_selector::onRemoveLight(bool /*unused*/)
{
    if(m_currentLight)
    {
        layer::sptr current_layer = m_currentLayer.lock();

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

        if(current_layer->isDefaultLight(m_currentLight))
        {
            current_layer->removeDefaultLight();
        }
        else
        {
            light_adaptor::destroyLightAdaptor(m_currentLight);
        }

        m_currentLight.reset();

        m_lightAdaptors = current_layer->getLightAdaptors();
        this->updateLightsList();

        m_removeLightBtn->setEnabled(false);

        auto sig = this->signal<light_selected_signal_t>(LIGHT_SELECTED_SIG);
        sig->async_emit(nullptr);

        current_layer->requestRender();
    }
}

//------------------------------------------------------------------------------

void light_selector::onEditAmbientColor(bool /*unused*/)
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    QWidget* const container = qt_container->getQtContainer();

    layer::sptr layer            = m_currentLayer.lock();
    Ogre::ColourValue ogre_color = layer->getSceneManager()->getAmbientLight();

    QColor q_color = QColorDialog::getColor(
        module::ui::viz::helper::utils::convertOgreColorToQColor(ogre_color),
        container,
        "scene ambient color"
    );

    ogre_color = module::ui::viz::helper::utils::convertQColorToOgreColor(q_color);

    layer->getSceneManager()->setAmbientLight(ogre_color);
    layer->requestRender();
}

//------------------------------------------------------------------------------

void light_selector::initLightList(layer::sptr _layer)
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

    const auto renderers = sight::service::get_services("sight::viz::scene3d::render");

    // Fills layer combo box with all enabled layers of each render services
    for(const auto& srv : renderers)
    {
        auto render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layer_map : render->getLayers())
        {
            const std::string id  = layer_map.first;
            std::string render_id = render->get_id();
            m_layersBox->addItem(QString::fromStdString(render_id + " : " + id));
            m_layers.push_back(layer_map.second);

            m_connections.connect(
                layer_map.second,
                layer::INIT_LAYER_SIG,
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

    for(const auto& light_adaptor : m_lightAdaptors)
    {
        QString light_name         = light_adaptor->getName().c_str();
        Qt::CheckState light_state = light_adaptor->isSwitchedOn() ? Qt::Checked
                                                                   : Qt::Unchecked;

        auto* next_light = new QListWidgetItem(light_name, m_lightsList);
        next_light->setFlags(next_light->flags() | Qt::ItemIsUserCheckable);
        next_light->setCheckState(light_state);
    }
}

//------------------------------------------------------------------------------

void light_selector::createLightAdaptor(const std::string& _name)
{
    layer::sptr current_layer = m_currentLayer.lock();

    if(current_layer)
    {
        data::color::sptr light_diffuse_color  = std::make_shared<data::color>();
        data::color::sptr light_specular_color = std::make_shared<data::color>();

        light_adaptor::sptr light_adaptor = light_adaptor::createLightAdaptor(
            light_diffuse_color,
            light_specular_color
        );
        light_adaptor->setType(Ogre::Light::LT_DIRECTIONAL);
        light_adaptor->setLayerID(current_layer->getLayerID());
        light_adaptor->setRenderService(current_layer->getRenderService());
        service::config_t config;
        config.add("config.<xmlattr>.name", this->get_id() + "_light");
        config.add("config.<xmlattr>.layer", current_layer->getLayerID());
        light_adaptor->set_config(config);
        light_adaptor->configure();
        light_adaptor->setName(_name);
        light_adaptor->start();

        m_managedLightAdaptors.push_back({light_adaptor, light_diffuse_color, light_specular_color});
        m_lightAdaptors = current_layer->getLightAdaptors();
        this->updateLightsList();

        const auto material_services = sight::service::get_services("sight::module::viz::scene3d::adaptor::material");

        for(const auto& srv : material_services)
        {
            auto material_adaptor = std::dynamic_pointer_cast<sight::viz::scene3d::adaptor>(srv);

            if(material_adaptor->getLayerID() == current_layer->getLayerID())
            {
                // Update materials of the scene to take the new light into account
                material_adaptor->update();
            }
        }
    }
}

//------------------------------------------------------------------------------

light_adaptor::sptr light_selector::retrieveLightAdaptor(const std::string& _name) const
{
    auto it = std::find_if(
        m_lightAdaptors.begin(),
        m_lightAdaptors.end(),
        [_name](light_adaptor::sptr _light_adaptor)
        {
            return _light_adaptor->getName() == _name;
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

    auto* light_name_layout = new QHBoxLayout();
    light_name_layout->addWidget(m_lightNameLbl);
    light_name_layout->addWidget(m_lightNameEdit);

    auto* new_light_layout = new QVBoxLayout();
    new_light_layout->addLayout(light_name_layout);
    new_light_layout->addWidget(m_okBtn);

    this->setWindowTitle("New light");
    this->setModal(true);
    this->setLayout(new_light_layout);

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
