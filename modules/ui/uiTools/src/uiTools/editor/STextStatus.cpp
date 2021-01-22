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

#include "uiTools/editor/STextStatus.hpp"

#include <core/com/Slots.hxx>

#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QString>

namespace uiTools
{

namespace editor
{

static const ::fwServices::IService::KeyType s_STRING_INPUT = "string";

static const core::com::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
static const core::com::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT    = "setIntParameter";
static const core::com::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
static const core::com::Slots::SlotKeyType s_SET_STRING_PARAMETER_SLOT = "setStringParameter";

//-----------------------------------------------------------------------------

STextStatus::STextStatus()
{
    m_labelStaticText = new QLabel();
    m_labelStaticText->setStyleSheet("font-weight: bold;");
    m_labelValue = new QLabel();
    m_labelValue->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    newSlot(s_SET_DOUBLE_PARAMETER_SLOT, &STextStatus::setDoubleParameter, this);
    newSlot(s_SET_INT_PARAMETER_SLOT, &STextStatus::setIntParameter, this);
    newSlot(s_SET_BOOL_PARAMETER_SLOT, &STextStatus::setBoolParameter, this);
    newSlot(s_SET_STRING_PARAMETER_SLOT, &STextStatus::setStringParameter, this);
}

//------------------------------------------------------------------------------

STextStatus::~STextStatus()
{

}

//------------------------------------------------------------------------------

void STextStatus::configuring()
{
    this->initialize();

    const auto txtCfg = m_configuration->findConfigurationElement("label");
    if(txtCfg)
    {
        const QString txt = QString::fromStdString(txtCfg->getValue());
        m_labelStaticText->setText(QString(txt + ": "));
    }

    QString color = "red";

    const auto colorCfg = m_configuration->findConfigurationElement("color");
    if(colorCfg)
    {
        const QString txtColor = QString::fromStdString(colorCfg->getValue());
        if(!txtColor.isEmpty())
        {
            color = txtColor;
        }
    }

    m_labelStaticText->setStyleSheet(m_labelStaticText->styleSheet() + " color: " + color + ";");
}

//------------------------------------------------------------------------------

void STextStatus::starting()
{
    this->create();
    auto qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QHBoxLayout* const layout = new QHBoxLayout();
    layout->addWidget(m_labelStaticText);
    layout->addWidget(m_labelValue);

    layout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    qtContainer->setLayout(layout);

    // Get input data.
    const auto stringInput     = this->getWeakInput< const ::fwData::String >(s_STRING_INPUT);
    const auto stringInputLock = stringInput.lock();

    if(stringInputLock)
    {
        m_labelValue->setText(QString::fromStdString(stringInputLock->value()));
    }

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap STextStatus::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_STRING_INPUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void STextStatus::updating()
{
    // Get input data.
    const auto stringInput     = this->getWeakInput< const ::fwData::String >(s_STRING_INPUT);
    const auto stringInputLock = stringInput.lock();

    if(stringInputLock)
    {
        m_labelValue->setText(QString::fromStdString(stringInputLock->value()));
    }
}

//------------------------------------------------------------------------------

void STextStatus::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void STextStatus::setIntParameter(int _val)
{
    QString str;
    str.setNum(_val);
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void STextStatus::setDoubleParameter(double _val)
{
    QString str;
    str.setNum(_val, 'g', 8); // 8 decimals precision
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void STextStatus::setBoolParameter(bool _val)
{
    QString str( _val ? "ON" : "OFF");
    m_labelValue->setText(str);
}

//------------------------------------------------------------------------------

void STextStatus::setStringParameter(std::string _val)
{
    m_labelValue->setText(QString::fromStdString(_val));
}

//------------------------------------------------------------------------------

} // namespace editor.
} // namespace uiTools.
