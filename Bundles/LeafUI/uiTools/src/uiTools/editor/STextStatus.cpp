/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTools/editor/STextStatus.hpp"

#include <fwCom/Slots.hxx>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QString>

namespace uiTools
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiTools::editor::STextStatus );

const ::fwCom::Slots::SlotKeyType STextStatus::s_SET_DOUBLE_PARAMETER_SLOT = "setDoubleParameter";
const ::fwCom::Slots::SlotKeyType STextStatus::s_SET_INT_PARAMETER_SLOT    = "setIntParameter";
const ::fwCom::Slots::SlotKeyType STextStatus::s_SET_BOOL_PARAMETER_SLOT   = "setBoolParameter";
const ::fwCom::Slots::SlotKeyType STextStatus::s_SET_STRING_PARAMETER_SLOT = "setStringParameter";

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

void STextStatus::starting()
{
    this->create();
    auto qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(m_labelStaticText);
    layout->addWidget(m_labelValue);

    layout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);

    qtContainer->setLayout(layout);

}

//------------------------------------------------------------------------------

void STextStatus::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void STextStatus::configuring()
{
    this->initialize();

    auto txtCfg = m_configuration->findConfigurationElement("label");
    SLM_ASSERT("'<label>' configuration element is missing.", txtCfg);
    QString txt = QString::fromStdString(txtCfg->getValue());

    m_labelStaticText->setText(QString(txt + ": "));

    QString color = "red";

    auto colorCfg = m_configuration->findConfigurationElement("color");
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
    str.setNum(_val);
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

void STextStatus::updating()
{
}

//------------------------------------------------------------------------------

void STextStatus::swapping()
{

}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiTools
