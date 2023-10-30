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

#include "texture_selector.hpp"

#include <core/com/signal.hxx>

#include <data/array.hpp>
#include <data/image.hpp>
#include <data/material.hpp>

#include <io/__/service/ioTypes.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

#include <ui/__/dialog_editor.hpp>
#include <ui/qt/container/widget.hpp>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

namespace sight::module::ui::viz
{

//------------------------------------------------------------------------------

texture_selector::texture_selector() noexcept =
    default;

//------------------------------------------------------------------------------

texture_selector::~texture_selector() noexcept =
    default;

//------------------------------------------------------------------------------

void texture_selector::starting()
{
    this->create();

    const QString service_id = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->get_container()
    );
    qt_container->get_qt_container()->setObjectName(service_id);

    m_load_button = new QPushButton(QString("Load texture"));
    m_load_button->setObjectName(service_id + "/" + m_load_button->text());
    m_load_button->setToolTip(QString("Selected organ's texture"));
    m_load_button->setMinimumSize(m_load_button->sizeHint());

    m_delete_button = new QPushButton(QString("Remove texture"));
    m_delete_button->setObjectName(service_id + "/" + m_delete_button->text());
    m_delete_button->setToolTip(QString("Remove organ's texture"));
    m_delete_button->setMinimumSize(m_delete_button->sizeHint());

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_load_button, 0);
    layout->addWidget(m_delete_button, 0);

    qt_container->set_layout(layout);
    qt_container->set_enabled(true);

    QObject::connect(m_load_button, &QPushButton::clicked, this, &self_t::on_load_button);
    QObject::connect(m_delete_button, &QPushButton::clicked, this, &self_t::on_delete_button);

    this->updating();
}

//------------------------------------------------------------------------------

void texture_selector::stopping()
{
    QObject::disconnect(m_load_button, &QPushButton::clicked, this, &self_t::on_load_button);
    QObject::disconnect(m_delete_button, &QPushButton::clicked, this, &self_t::on_delete_button);

    this->destroy();
}

//------------------------------------------------------------------------------

void texture_selector::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void texture_selector::updating()
{
}

//------------------------------------------------------------------------------

void texture_selector::on_load_button()
{
    const auto reconstruction = m_reconstruction.lock();
    SIGHT_ASSERT("No associated Reconstruction", reconstruction);

    data::material::sptr material = reconstruction->get_material();
    data::image::sptr image       = material->get_diffuse_texture();

    bool existing_texture = (image != nullptr);

    // We have to instantiate a new image if necessary
    if(!existing_texture)
    {
        image = std::make_shared<data::image>();
        material->set_diffuse_texture(image);
    }

    auto srv = sight::service::add<sight::ui::dialog_editor>("sight::module::ui::io::selector");
    srv->set_inout(image, io::service::DATA_KEY);

    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    sight::service::remove(srv);

    // If we didn't have to create a new texture, we can notify the associated image
    if(existing_texture)
    {
        auto sig = image->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig->emit();
    }
    else
    {
        auto sig = material->signal<data::material::added_texture_signal_t>(
            data::material::ADDED_TEXTURE_SIG
        );
        sig->emit(image);
    }
}

//------------------------------------------------------------------------------

void texture_selector::on_delete_button()
{
    const auto reconstruction = m_reconstruction.lock();
    SIGHT_ASSERT("No associated Reconstruction", reconstruction);

    data::material::sptr material = reconstruction->get_material();
    data::image::sptr image       = material->get_diffuse_texture();

    if(image)
    {
        material->set_diffuse_texture(nullptr);
        auto sig = material->signal<data::material::removed_texture_signal_t>(
            data::material::REMOVED_TEXTURE_SIG
        );
        sig->emit(image);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
