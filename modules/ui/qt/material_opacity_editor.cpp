/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
#include "modules/ui/qt/material_opacity_editor.hpp"

#include <ui/qt/container/widget.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QVBoxLayout>

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------

void material_opacity_editor::configuring()
{
    const config_t configuration = this->get_config();
    m_label = configuration.get<std::string>("label", m_label);

    this->initialize();
}

//------------------------------------------------------------------------------

void material_opacity_editor::starting()
{
    this->create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    auto* const opacityLabel = new QLabel(tr(m_label.c_str()));
    m_opacitySlider = new QSlider(Qt::Horizontal);
    m_opacitySlider->setToolTip(tr("Adjust opacity level"));
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
    this->updating();
    QObject::connect(m_opacitySlider, &QSlider::valueChanged, this, &material_opacity_editor::onOpacitySlider);
}

//------------------------------------------------------------------------------

void material_opacity_editor::updating()
{
    auto material = m_material.lock();
    const int a   = static_cast<int>(material->diffuse()->alpha() * 100.F);
    m_opacitySlider->setValue(a);
}

//------------------------------------------------------------------------------

void material_opacity_editor::onOpacitySlider(int _value)
{
    auto material = m_material.lock();
    material->diffuse()->alpha() = static_cast<float>(_value) / 100.F;
    std::stringstream ss;
    ss << _value << "%";
    m_opacityValue->setText(QString::fromStdString(ss.str()));
    auto sig = material->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
    sig->async_emit();
}

//------------------------------------------------------------------------------

void material_opacity_editor::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
