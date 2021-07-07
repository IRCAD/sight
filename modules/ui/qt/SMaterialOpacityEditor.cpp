/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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
#include "modules/ui/qt/SMaterialOpacityEditor.hpp"

#include <data/Material.hpp>

#include <ui/qt/container/QtContainer.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

namespace sight::module::ui::qt
{

static const service::IService::KeyType s_MATERIAL_INOUT = "material";

SMaterialOpacityEditor::SMaterialOpacityEditor() noexcept
{
    this->registerObject(s_MATERIAL_INOUT, service::IService::AccessType::INOUT, true);
}

SMaterialOpacityEditor::~SMaterialOpacityEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SMaterialOpacityEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SMaterialOpacityEditor::starting()
{
    this->create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    auto* const opacityLabel = new QLabel(tr("Material's opacity : "));
    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setToolTip(tr("Selected meshe's opacity"));
    m_opacitySlider->setRange(0, 100);
    m_opacitySlider->setTickInterval(20);
    m_opacitySlider->setTickPosition(QSlider::TicksBelow);
    m_opacitySlider->setMinimumSize(m_opacitySlider->sizeHint());

    m_opacityValue = new QLabel("");
    m_opacityValue->setMinimumSize(m_opacityValue->sizeHint());

    auto* const mainLayout    = new QVBoxLayout();
    auto* const opacityLayout = new QHBoxLayout();

    opacityLayout->addWidget(opacityLabel, 0);
    opacityLayout->addWidget(m_opacitySlider, 1);
    opacityLayout->addWidget(m_opacityValue, 0);

    mainLayout->addLayout(opacityLayout, 0);
    qtContainer->setLayout(mainLayout);

    QObject::connect(m_opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySlider(int)));
}

//------------------------------------------------------------------------------

void SMaterialOpacityEditor::updating()
{
}

//------------------------------------------------------------------------------

void SMaterialOpacityEditor::onOpacitySlider(int _value)
{
    auto material = this->getLockedInOut<data::Material>(s_MATERIAL_INOUT);

    material->diffuse()->alpha() = static_cast<float>(_value) / 100.f;
    std::stringstream ss;
    ss << _value << "%";
    m_opacityValue->setText(QString::fromStdString(ss.str()));
    auto sig = material->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

void SMaterialOpacityEditor::stopping()
{
    QObject::disconnect(m_opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacitySlider(int)));
    this->destroy();
}

//------------------------------------------------------------------------------

} // namespace sight::module
