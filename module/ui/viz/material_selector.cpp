/************************************************************************
 *
 * Copyright (C) 2014-2025 IRCAD France
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

#include "material_selector.hpp"

#include <core/com/signal.hxx>

#include <data/helper/field.hpp>
#include <data/material.hpp>
#include <data/string.hpp>

#include <ui/qt/container/widget.hpp>

#include <viz/scene3d/ogre.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreMaterialManager.h>
#include <OGRE/OgrePass.h>
#include <OGRE/OgreResourceManager.h>
#include <OGRE/OgreTechnique.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

namespace sight::module::ui::viz
{

const core::com::signals::key_t material_selector::SELECTED_SIG = "selected";

static const std::string MATERIAL_RESOURCEGROUP_NAME = "materialsTemplate";

//------------------------------------------------------------------------------
material_selector::material_selector() noexcept
{
    new_signal<selected_signal_t>(SELECTED_SIG);
}

//------------------------------------------------------------------------------

void material_selector::configuring(const config_t& _config)
{
    this->initialize();

    const auto supported_materials_str = _config.get("config.<xmlattr>.materials", "Default");
    std::vector<std::string> supported_materials;
    boost::split(m_supported_materials, supported_materials_str, boost::is_any_of(",;"));
}

//------------------------------------------------------------------------------

void material_selector::starting()
{
    this->create();

    const QString service_id = QString::fromStdString(base_id());

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());
    qt_container->get_qt_container()->setObjectName(service_id);

    // Selection
    auto* current_material = new QLabel();
    current_material->setText("Current material : ");

    m_material_box = new QComboBox();
    m_material_box->setObjectName(service_id + "/materialBox");

    std::pair<std::string, std::string> elt;
    Ogre::ResourceManager::ResourceMapIterator iter = Ogre::MaterialManager::getSingleton().getResourceIterator();
    while(iter.hasMoreElements())
    {
        Ogre::ResourcePtr resource = iter.getNext();
        const auto mat             = std::dynamic_pointer_cast<Ogre::Material>(resource);
        if(mat && m_supported_materials.contains(mat->getName()))
        {
            m_material_box->addItem(QString::fromStdString(mat->getName()));
        }
    }

    m_material_box->setCurrentIndex(0);

    auto* label_layout = new QHBoxLayout();
    label_layout->addWidget(current_material);
    label_layout->addWidget(m_material_box);

    // Reload
    m_reload_button = new QPushButton("Reload");
    m_reload_button->setObjectName(service_id + "/" + m_reload_button->text());

    auto* layout = new QVBoxLayout();
    layout->addLayout(label_layout);
    layout->addWidget(m_reload_button);

    qt_container->set_layout(layout);

    this->updating();

    QObject::connect(
        m_material_box,
        SIGNAL(currentTextChanged(const QString&)),
        this,
        SLOT(on_selected_mode_item(const QString&))
    );
    QObject::connect(m_reload_button, SIGNAL(clicked()), this, SLOT(on_reload_material()));
}

//------------------------------------------------------------------------------

void material_selector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void material_selector::updating()
{
    this->update_material();
}

//------------------------------------------------------------------------------

void material_selector::update_material()
{
    const auto reconstruction     = m_reconstruction.lock();
    data::material::sptr material = reconstruction->get_material();
    data::object::sptr field_obj  = material->get_field("material");
    if(field_obj != nullptr)
    {
        data::string::sptr field = std::dynamic_pointer_cast<data::string>(field_obj);
        m_material_box->setCurrentText(field->value().c_str());
    }
}

//------------------------------------------------------------------------------

void material_selector::on_selected_mode_item(const QString& _text)
{
    const auto reconstruction     = m_reconstruction.lock();
    data::material::sptr material = reconstruction->get_material();
    data::string::sptr string     = std::make_shared<data::string>();
    string->set_value(_text.toStdString());

    data::helper::field helper(material);
    helper.set_field("material", string);
    helper.notify();

    auto sig = this->signal<selected_signal_t>(SELECTED_SIG);
    sig->async_emit(_text.toStdString());
}

//------------------------------------------------------------------------------

void material_selector::on_reload_material()
{
    auto material_name         = m_material_box->currentText().toStdString();
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(
        material_name,
        sight::viz::scene3d::RESOURCE_GROUP
    );

    if(!material)
    {
        SIGHT_ERROR("Could not find material" << std::quoted(material_name));
        return;
    }

    material->reload();

    const Ogre::Material::Techniques& techniques = material->getTechniques();

    for(auto* const tech : techniques)
    {
        SIGHT_ASSERT("technique is not set", tech);

        const Ogre::Technique::Passes& passes = tech->getPasses();

        for(auto* const pass : passes)
        {
            SIGHT_ASSERT("No pass found", pass);

            if(!pass->getVertexProgramName().empty())
            {
                pass->getVertexProgram()->reload();
            }

            if(!pass->getGeometryProgramName().empty())
            {
                pass->getGeometryProgram()->reload();
            }

            if(!pass->getFragmentProgramName().empty())
            {
                pass->getFragmentProgram()->reload();
            }
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
