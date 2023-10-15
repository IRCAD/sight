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

    const QString serviceID = QString::fromStdString(get_id().substr(get_id().find_last_of('_') + 1));

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(
        this->getContainer()
    );
    qtContainer->getQtContainer()->setObjectName(serviceID);

    m_loadButton = new QPushButton(QString("Load texture"));
    m_loadButton->setObjectName(serviceID + "/" + m_loadButton->text());
    m_loadButton->setToolTip(QString("Selected organ's texture"));
    m_loadButton->setMinimumSize(m_loadButton->sizeHint());

    m_deleteButton = new QPushButton(QString("Remove texture"));
    m_deleteButton->setObjectName(serviceID + "/" + m_deleteButton->text());
    m_deleteButton->setToolTip(QString("Remove organ's texture"));
    m_deleteButton->setMinimumSize(m_deleteButton->sizeHint());

    auto* layout = new QVBoxLayout();
    layout->addWidget(m_loadButton, 0);
    layout->addWidget(m_deleteButton, 0);

    qtContainer->setLayout(layout);
    qtContainer->setEnabled(true);

    QObject::connect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadButton()));
    QObject::connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButton()));

    this->updating();
}

//------------------------------------------------------------------------------

void texture_selector::stopping()
{
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

void texture_selector::onLoadButton()
{
    const auto reconstruction = m_reconstruction.lock();
    SIGHT_ASSERT("No associated Reconstruction", reconstruction);

    data::material::sptr material = reconstruction->getMaterial();
    data::image::sptr image       = material->getDiffuseTexture();

    bool existingTexture = (image != nullptr);

    // We have to instantiate a new image if necessary
    if(!existingTexture)
    {
        image = std::make_shared<data::image>();
        material->setDiffuseTexture(image);
    }

    auto srv = sight::service::add<sight::ui::dialog_editor>("sight::module::ui::io::selector");
    srv->set_inout(image, io::service::s_DATA_KEY);

    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    sight::service::remove(srv);

    // If we didn't have to create a new texture, we can notify the associated image
    if(existingTexture)
    {
        auto sig = image->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
        sig->emit();
    }
    else
    {
        auto sig = material->signal<data::material::AddedTextureSignalType>(
            data::material::ADDED_TEXTURE_SIG
        );
        sig->emit(image);
    }
}

//------------------------------------------------------------------------------

void texture_selector::onDeleteButton()
{
    const auto reconstruction = m_reconstruction.lock();
    SIGHT_ASSERT("No associated Reconstruction", reconstruction);

    data::material::sptr material = reconstruction->getMaterial();
    data::image::sptr image       = material->getDiffuseTexture();

    if(image)
    {
        material->setDiffuseTexture(nullptr);
        auto sig = material->signal<data::material::RemovedTextureSignalType>(
            data::material::REMOVED_TEXTURE_SIG
        );
        sig->emit(image);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
