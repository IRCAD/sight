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

#include "material_selector.hpp"

#include <core/com/signal.hxx>

#include <data/helper/field.hpp>
#include <data/material.hpp>
#include <data/string.hpp>

#include <service/macros.hpp>

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

static const std::string s_MATERIAL_RESOURCEGROUP_NAME = "materialsTemplate";

//------------------------------------------------------------------------------
material_selector::material_selector() noexcept
{
    new_signal<selected_signal_t>(SELECTED_SIG);
}

//------------------------------------------------------------------------------

material_selector::~material_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void material_selector::starting()
{
    this->create();

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());
    qt_container->getQtContainer()->setObjectName(service_id);

    // Selection
    auto* current_material = new QLabel();
    current_material->setText("Current material : ");

    m_materialBox = new QComboBox();
    m_materialBox->setObjectName(service_id + "/materialBox");

    std::pair<std::string, std::string> elt;
    Ogre::ResourceManager::ResourceMapIterator iter = Ogre::MaterialManager::getSingleton().getResourceIterator();
    while(iter.hasMoreElements())
    {
        Ogre::ResourcePtr mat = iter.getNext();
        if(mat->getGroup() == s_MATERIAL_RESOURCEGROUP_NAME)
        {
            m_materialBox->addItem(QString::fromStdString(mat->getName()));
        }
    }

    m_materialBox->setCurrentIndex(0);

    auto* label_layout = new QHBoxLayout();
    label_layout->addWidget(current_material);
    label_layout->addWidget(m_materialBox);

    // Reload
    m_reloadButton = new QPushButton("Reload");
    m_reloadButton->setObjectName(service_id + "/" + m_reloadButton->text());

    auto* layout = new QVBoxLayout();
    layout->addLayout(label_layout);
    layout->addWidget(m_reloadButton);

    qt_container->setLayout(layout);

    this->updating();

    QObject::connect(
        m_materialBox,
        SIGNAL(activated(const QString&)),
        this,
        SLOT(onSelectedModeItem(const QString&))
    );
    QObject::connect(m_reloadButton, SIGNAL(clicked()), this, SLOT(onReloadMaterial()));
}

//------------------------------------------------------------------------------

void material_selector::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void material_selector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void material_selector::updating()
{
    this->updateMaterial();
}

//------------------------------------------------------------------------------

void material_selector::updateMaterial()
{
    const auto reconstruction     = m_reconstruction.lock();
    data::material::sptr material = reconstruction->getMaterial();
    data::object::sptr field_obj  = material->get_field("ogreMaterial");
    if(field_obj != nullptr)
    {
        data::string::sptr field = std::dynamic_pointer_cast<data::string>(field_obj);
        m_materialBox->setCurrentText(field->value().c_str());
    }
}

//------------------------------------------------------------------------------

void material_selector::onSelectedModeItem(const QString& _text)
{
    const auto reconstruction     = m_reconstruction.lock();
    data::material::sptr material = reconstruction->getMaterial();
    data::string::sptr string     = std::make_shared<data::string>();
    string->setValue(_text.toStdString());

    data::helper::field helper(material);
    helper.set_field("ogreMaterial", string);
    helper.notify();

    auto sig = this->signal<selected_signal_t>(SELECTED_SIG);
    sig->async_emit(_text.toStdString());
}

//------------------------------------------------------------------------------

void material_selector::onReloadMaterial()
{
    auto material_name         = m_materialBox->currentText().toStdString();
    Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(
        material_name,
        sight::viz::scene3d::RESOURCE_GROUP
    );

    if(!material)
    {
        SIGHT_ERROR("Could not find material" << material_name);
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
