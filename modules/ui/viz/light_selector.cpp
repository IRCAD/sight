/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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
#include <data/map.hpp>
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

const core::com::signals::key_t LIGHT_SELECTED_SIG = "light_selected";
const core::com::slots::key_t INIT_LIGHT_LIST_SLOT = "initLightList";

//------------------------------------------------------------------------------

light_selector::light_selector() noexcept
{
    new_signal<light_selected_signal_t>(LIGHT_SELECTED_SIG);
    new_slot(INIT_LIGHT_LIST_SLOT, &light_selector::init_light_list, this);
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

    const QString service_id = QString::fromStdString(base_id());

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    qt_container->get_qt_container()->setObjectName(service_id);

    m_layers_box = new QComboBox();
    m_layers_box->setObjectName(service_id + "/layersBox");
    m_lights_list = new QListWidget();
    m_lights_list->setObjectName(service_id + "/lightsList");
    m_add_light_btn = new QPushButton("Add light");
    m_add_light_btn->setObjectName(service_id + "/" + m_add_light_btn->text());
    m_remove_light_btn = new QPushButton("Remove light");
    m_remove_light_btn->setObjectName(service_id + "/" + m_remove_light_btn->text());
    m_ambient_color_btn = new QPushButton("scene ambient color");
    m_ambient_color_btn->setObjectName(service_id + "/" + m_ambient_color_btn->text());

    auto* layout_button = new QHBoxLayout;
    m_check_all_button = new QPushButton(tr("Check all"));
    m_check_all_button->setObjectName(service_id + "/" + m_check_all_button->text());
    layout_button->addWidget(m_check_all_button, 0);

    m_un_check_all_button = new QPushButton(tr("UnCheck all"));
    m_un_check_all_button->setObjectName(service_id + "/" + m_un_check_all_button->text());
    layout_button->addWidget(m_un_check_all_button, 0);

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_layers_box);
    layout->addLayout(layout_button);
    layout->addWidget(m_lights_list);

    auto* add_remove_layout = new QHBoxLayout();
    add_remove_layout->addWidget(m_add_light_btn);
    add_remove_layout->addWidget(m_remove_light_btn);
    m_remove_light_btn->setEnabled(false);

    layout->addLayout(add_remove_layout);
    layout->addWidget(m_ambient_color_btn);

    qt_container->set_layout(layout);

    this->refresh_layers();

    this->update_lights_list();

    QObject::connect(
        m_layers_box,
        qOverload<int>(&QComboBox::activated),
        this,
        &light_selector::on_selected_layer_item
    );
    QObject::connect(m_lights_list, &QListWidget::currentItemChanged, this, &light_selector::on_selected_light_item);
    QObject::connect(m_lights_list, &QListWidget::itemChanged, this, &light_selector::on_checked_light_item);
    QObject::connect(m_add_light_btn, &QPushButton::clicked, this, &light_selector::on_add_light);
    QObject::connect(m_remove_light_btn, &QPushButton::clicked, this, &light_selector::on_remove_light);
    QObject::connect(m_ambient_color_btn, &QPushButton::clicked, this, &light_selector::on_edit_ambient_color);
    QObject::connect(m_check_all_button, &QPushButton::clicked, this, &light_selector::on_check_all_check_box);
    QObject::connect(m_un_check_all_button, &QPushButton::clicked, this, &light_selector::on_un_check_all_check_box);
}

//------------------------------------------------------------------------------

void light_selector::updating()
{
}

//------------------------------------------------------------------------------

void light_selector::stopping()
{
    m_connections.disconnect();

    for(auto& light : m_managed_light_adaptors)
    {
        light_adaptor::destroy_light_adaptor(light.m_light);
    }

    m_managed_light_adaptors.clear();

    this->destroy();
}

//------------------------------------------------------------------------------

void light_selector::on_selected_layer_item(int _index)
{
    m_current_layer  = m_layers[static_cast<std::size_t>(_index)];
    m_light_adaptors = m_current_layer.lock()->get_light_adaptors();

    this->update_lights_list();
}

//------------------------------------------------------------------------------

void light_selector::on_selected_light_item(QListWidgetItem* _item, QListWidgetItem* /*unused*/)
{
    if(_item != nullptr)
    {
        m_current_light = this->retrieve_light_adaptor(_item->text().toStdString());

        auto sig = this->signal<light_selected_signal_t>(LIGHT_SELECTED_SIG);
        sig->async_emit(m_current_light);

        m_remove_light_btn->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void light_selector::on_checked_light_item(QListWidgetItem* _item)
{
    light_adaptor::sptr checked_light_adaptor =
        this->retrieve_light_adaptor(_item->text().toStdString());

    checked_light_adaptor->switch_on(_item->checkState() == Qt::Checked);
}

//------------------------------------------------------------------------------

void light_selector::on_add_light(bool /*unused*/)
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    QWidget* const container = qt_container->get_qt_container();

    auto* light_dialog = new module::ui::viz::new_light_dialog(container);

    if(light_dialog->exec() == QDialog::Accepted)
    {
        std::string light_name = light_dialog->property("lightName").toString().toStdString();

        auto existing_light =
            std::find_if(
                m_light_adaptors.begin(),
                m_light_adaptors.end(),
                [light_name](light_adaptor::sptr _light_adaptor)
            {
                return _light_adaptor->get_name() == light_name;
            });

        if(existing_light == m_light_adaptors.end())
        {
            this->create_light_adaptor(light_name);
        }
    }
}

//------------------------------------------------------------------------------

void light_selector::on_remove_light(bool /*unused*/)
{
    if(m_current_light)
    {
        layer::sptr current_layer = m_current_layer.lock();

        const auto position =
            std::find_if(
                m_managed_light_adaptors.begin(),
                m_managed_light_adaptors.end(),
                [&](const light& _light)
            {
                return _light.m_light == m_current_light;
            });

        if(position != m_managed_light_adaptors.end())
        {
            m_managed_light_adaptors.erase(position);
        }

        if(current_layer->is_default_light(m_current_light))
        {
            current_layer->remove_default_light();
        }
        else
        {
            light_adaptor::destroy_light_adaptor(m_current_light);
        }

        m_current_light.reset();

        m_light_adaptors = current_layer->get_light_adaptors();
        this->update_lights_list();

        m_remove_light_btn->setEnabled(false);

        auto sig = this->signal<light_selected_signal_t>(LIGHT_SELECTED_SIG);
        sig->async_emit(nullptr);

        current_layer->request_render();
    }
}

//------------------------------------------------------------------------------

void light_selector::on_edit_ambient_color(bool /*unused*/)
{
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    QWidget* const container = qt_container->get_qt_container();

    layer::sptr layer            = m_current_layer.lock();
    Ogre::ColourValue ogre_color = layer->get_scene_manager()->getAmbientLight();

    QColor q_color = QColorDialog::getColor(
        module::ui::viz::helper::utils::convert_ogre_color_to_q_color(ogre_color),
        container,
        "scene ambient color"
    );

    ogre_color = module::ui::viz::helper::utils::convert_q_color_to_ogre_color(q_color);

    layer->get_scene_manager()->setAmbientLight(ogre_color);
    layer->request_render();
}

//------------------------------------------------------------------------------

void light_selector::init_light_list(layer::sptr _layer)
{
    m_current_layer = m_layers[0];

    if(_layer == m_current_layer.lock())
    {
        this->on_selected_layer_item(0);
    }
}

//------------------------------------------------------------------------------

void light_selector::refresh_layers()
{
    m_layers_box->clear();

    const auto renderers = sight::service::get_services("sight::viz::scene3d::render");

    // Fills layer combo box with all enabled layers of each render services
    for(const auto& srv : renderers)
    {
        auto render = std::dynamic_pointer_cast<sight::viz::scene3d::render>(srv);

        for(auto& layer_map : render->get_layers())
        {
            const std::string id  = layer_map.first;
            std::string render_id = render->get_id();
            m_layers_box->addItem(QString::fromStdString(render_id + " : " + id));
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
        m_current_layer  = m_layers[0];
        m_light_adaptors = m_current_layer.lock()->get_light_adaptors();
    }
}

//------------------------------------------------------------------------------

void light_selector::update_lights_list()
{
    m_lights_list->clear();

    for(const auto& light_adaptor : m_light_adaptors)
    {
        QString light_name         = light_adaptor->get_name().c_str();
        Qt::CheckState light_state = light_adaptor->is_switched_on() ? Qt::Checked
                                                                     : Qt::Unchecked;

        auto* next_light = new QListWidgetItem(light_name, m_lights_list);
        next_light->setFlags(next_light->flags() | Qt::ItemIsUserCheckable);
        next_light->setCheckState(light_state);
    }
}

//------------------------------------------------------------------------------

void light_selector::create_light_adaptor(const std::string& _name)
{
    layer::sptr current_layer = m_current_layer.lock();

    if(current_layer)
    {
        data::color::sptr light_diffuse_color  = std::make_shared<data::color>();
        data::color::sptr light_specular_color = std::make_shared<data::color>();

        light_adaptor::sptr light_adaptor = light_adaptor::create_light_adaptor(
            light_diffuse_color,
            light_specular_color
        );
        light_adaptor->set_type(Ogre::Light::LT_DIRECTIONAL);
        light_adaptor->set_layer_id(current_layer->layer_id());
        light_adaptor->set_render_service(current_layer->render_service());
        service::config_t config;
        config.add("config.<xmlattr>.name", this->get_id() + "_light");
        config.add("config.<xmlattr>.layer", current_layer->layer_id());
        light_adaptor->set_config(config);
        light_adaptor->configure();
        light_adaptor->set_name(_name);
        light_adaptor->start();

        m_managed_light_adaptors.push_back({light_adaptor, light_diffuse_color, light_specular_color});
        m_light_adaptors = current_layer->get_light_adaptors();
        this->update_lights_list();

        const auto material_services = sight::service::get_services("sight::module::viz::scene3d::adaptor::material");

        for(const auto& srv : material_services)
        {
            auto material_adaptor = std::dynamic_pointer_cast<sight::viz::scene3d::adaptor>(srv);

            if(material_adaptor->layer_id() == current_layer->layer_id())
            {
                // Update materials of the scene to take the new light into account
                material_adaptor->update();
            }
        }
    }
}

//------------------------------------------------------------------------------

light_adaptor::sptr light_selector::retrieve_light_adaptor(const std::string& _name) const
{
    auto it = std::find_if(
        m_light_adaptors.begin(),
        m_light_adaptors.end(),
        [_name](light_adaptor::sptr _light_adaptor)
        {
            return _light_adaptor->get_name() == _name;
        });

    return it != m_light_adaptors.end() ? *it : nullptr;
}

//------------------------------------------------------------------------------

void light_selector::on_check_all_check_box()
{
    this->on_check_all_boxes(true);
}

//------------------------------------------------------------------------------

void light_selector::on_un_check_all_check_box()
{
    this->on_check_all_boxes(false);
}

//------------------------------------------------------------------------------

void light_selector::on_check_all_boxes(bool _visible)
{
    for(int i = 0 ; i < m_lights_list->count() ; ++i)
    {
        auto* item = m_lights_list->item(i);
        item->setCheckState(_visible ? Qt::Checked : Qt::Unchecked);
    }
}

//------------------------------------------------------------------------------

new_light_dialog::new_light_dialog(QWidget* _parent) :
    QDialog(_parent),
    m_light_name_edit(new QLineEdit(this))
{
    m_light_name_lbl = new QLabel("Name :", this);

    m_ok_btn = new QPushButton("Ok", this);

    auto* light_name_layout = new QHBoxLayout();
    light_name_layout->addWidget(m_light_name_lbl);
    light_name_layout->addWidget(m_light_name_edit);

    auto* new_light_layout = new QVBoxLayout();
    new_light_layout->addLayout(light_name_layout);
    new_light_layout->addWidget(m_ok_btn);

    this->setWindowTitle("New light");
    this->setModal(true);
    this->setLayout(new_light_layout);

    QObject::connect(m_ok_btn, &QPushButton::clicked, this, &new_light_dialog::on_ok_btn);
}

//------------------------------------------------------------------------------

new_light_dialog::~new_light_dialog()
{
    QObject::disconnect(m_ok_btn, &QPushButton::clicked, this, &new_light_dialog::on_ok_btn);
}

//------------------------------------------------------------------------------

void new_light_dialog::on_ok_btn(bool /*unused*/)
{
    if(!m_light_name_edit->text().isEmpty())
    {
        this->setProperty("lightName", QVariant(m_light_name_edit->text()));
        this->accept();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
