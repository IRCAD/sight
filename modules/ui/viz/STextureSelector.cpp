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

#include "STextureSelector.hpp"

#include <core/com/Signal.hxx>

#include <data/Array.hpp>
#include <data/Image.hpp>
#include <data/Material.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <io/base/service/ioTypes.hpp>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>

#include <ui/base/IDialogEditor.hpp>
#include <ui/qt/container/QtContainer.hpp>

namespace sight::module::ui::viz
{

fwServicesRegisterMacro( ::sight::ui::base::IEditor, ::sight::module::ui::viz::STextureSelector,
                         ::sight::data::Reconstruction)

static const std::string s_RECONSTRUCTION_INOUT = "reconstruction";

//------------------------------------------------------------------------------

STextureSelector::STextureSelector() noexcept
{

}

//------------------------------------------------------------------------------

STextureSelector::~STextureSelector() noexcept
{
}

//------------------------------------------------------------------------------

void STextureSelector::starting()
{

    this->create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(
        this->getContainer() );

    m_loadButton = new QPushButton(QString("Load texture"));
    m_loadButton->setToolTip(QString("Selected organ's texture"));
    m_loadButton->setMinimumSize(m_loadButton->sizeHint());

    m_deleteButton = new QPushButton(QString("Remove texture"));
    m_deleteButton->setToolTip(QString("Remove organ's texture"));
    m_deleteButton->setMinimumSize(m_deleteButton->sizeHint());

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( m_loadButton, 0 );
    layout->addWidget( m_deleteButton, 0 );

    qtContainer->setLayout( layout );
    qtContainer->setEnabled(true);

    QObject::connect(m_loadButton, SIGNAL(clicked()), this, SLOT(onLoadButton()));
    QObject::connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButton()));

    this->updating();
}

//------------------------------------------------------------------------------

void STextureSelector::stopping()
{

    this->destroy();
}

//------------------------------------------------------------------------------

void STextureSelector::configuring()
{

    this->initialize();
}

//------------------------------------------------------------------------------

void STextureSelector::updating()
{
}

//------------------------------------------------------------------------------

void STextureSelector::onLoadButton()
{
    auto reconstruction = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("No associated Reconstruction", reconstruction);

    data::Material::sptr material = reconstruction->getMaterial();
    data::Image::sptr image       = material->getDiffuseTexture();

    bool existingTexture = (image != nullptr);

    // We have to instantiate a new image if necessary
    if(!existingTexture)
    {
        image = data::Image::New();
        material->setDiffuseTexture(image);
    }

    auto srv = service::add< sight::ui::base::IDialogEditor >("::sight::module::ui::base::io::SSelector");
    srv->registerInOut(image, io::base::service::s_DATA_KEY);

    srv->configure();
    srv->start();
    srv->update();
    srv->stop();
    service::OSR::unregisterService( srv );

    // If we didn't have to create a new texture, we can notify the associated image
    if(existingTexture)
    {
        auto sig = image->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
        sig->emit();
    }
    else
    {
        auto sig = material->signal< data::Material::AddedTextureSignalType >(
            data::Material::s_ADDED_TEXTURE_SIG);
        sig->emit(image);
    }
}

//------------------------------------------------------------------------------

void STextureSelector::onDeleteButton()
{
    auto reconstruction = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("No associated Reconstruction", reconstruction);

    data::Material::sptr material = reconstruction->getMaterial();
    data::Image::sptr image       = material->getDiffuseTexture();

    if(image)
    {
        material->setDiffuseTexture(nullptr);
        auto sig = material->signal< data::Material::RemovedTextureSignalType >(
            data::Material::s_REMOVED_TEXTURE_SIG);
        sig->emit(image);
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
