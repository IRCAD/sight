/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTools/editor/SStatus.hpp"

#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <QColor>
#include <QHBoxLayout>
#include <QPalette>
#include <QWidget>

namespace uiTools
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiTools::editor::SStatus, ::fwData::Object );

const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_GREEN_SLOT  = "changeToGreen";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_RED_SLOT    = "changeToRed";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_ORANGE_SLOT = "changeToOrange";
const ::fwCom::Slots::SlotKeyType SStatus::s_TOGGLE_GREEN_RED_SLOT = "toggleGreenRed";

//-----------------------------------------------------------------------------

SStatus::SStatus() noexcept :
    m_isCircular(false),
    m_width(20),
    m_height(20)
{
    m_slotChangeToGreen  = ::fwCom::newSlot( &SStatus::changeToGreen,  this );
    m_slotChangeToRed    = ::fwCom::newSlot( &SStatus::changeToRed,    this );
    m_slotChangeToOrange = ::fwCom::newSlot( &SStatus::changeToOrange, this );
    m_slotToggleGreenRed = ::fwCom::newSlot( &SStatus::toggleGreenRed, this );

    ::fwCom::HasSlots::m_slots
        ( s_CHANGE_TO_GREEN_SLOT, m_slotChangeToGreen   )
        ( s_CHANGE_TO_RED_SLOT, m_slotChangeToRed    )
        ( s_CHANGE_TO_ORANGE_SLOT, m_slotChangeToOrange    )
        ( s_TOGGLE_GREEN_RED_SLOT, m_slotToggleGreenRed )
    ;

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//------------------------------------------------------------------------------

SStatus::~SStatus() noexcept
{
}

//------------------------------------------------------------------------------

void SStatus::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QHBoxLayout* layout = new QHBoxLayout();

    m_indicator = new QLabel();
    m_indicator->setFixedWidth(m_width);
    m_indicator->setFixedHeight(m_height);

    layout->addWidget(m_indicator);
    layout->addWidget(m_labelStatus);

    qtContainer->setLayout(layout);

    this->changeToRed();
}

//------------------------------------------------------------------------------

void SStatus::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SStatus::configuring()
{
    this->initialize();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    ConfigType greenCfg       = m_configuration->findConfigurationElement("green");
    ConfigType redCfg         = m_configuration->findConfigurationElement("red");
    ConfigType orangeCfg      = m_configuration->findConfigurationElement("orange");
    ConfigType labelStatusCfg = m_configuration->findConfigurationElement("labelStatus");
    ConfigType formCfg        = m_configuration->findConfigurationElement("form");
    ConfigType sizeCfg        = m_configuration->findConfigurationElement("size");

    m_labelStatus = new QLabel();

    if (greenCfg)
    {
        m_greenTooltip = greenCfg->getValue();
    }
    if (redCfg)
    {
        m_redTooltip = redCfg->getValue();
    }
    if (orangeCfg)
    {
        m_orangeTooltip = orangeCfg->getValue();
    }
    if (labelStatusCfg)
    {
        m_labelStatus->setText(QString::fromStdString(labelStatusCfg->getValue()));
    }
    if (formCfg)
    {
        std::string form = formCfg->getValue();
        SLM_ASSERT("value for element 'form' should be 'circle' or 'square'.", form == "circle" || form == "square");

        m_isCircular = (form == "circle");
    }
    if(sizeCfg)
    {
        ConfigType widthCfg  = sizeCfg->findConfigurationElement("width");
        ConfigType heightCfg = sizeCfg->findConfigurationElement("height");

        SLM_ASSERT("element 'width' is missing.", widthCfg);
        SLM_ASSERT("element 'height' is missing.", heightCfg);

        m_width  = std::stoi(widthCfg->getValue());
        m_height = std::stoi(heightCfg->getValue());
    }
}

//------------------------------------------------------------------------------

void SStatus::updating()
{
}

//------------------------------------------------------------------------------

void SStatus::swapping()
{

}

//------------------------------------------------------------------------------

void SStatus::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void SStatus::changeToGreen()
{
    m_indicator->setStyleSheet("background-color: green; border-radius: "+QString(m_isCircular ? "10px;" : "0")+";");
    m_indicator->setToolTip(QString::fromStdString(m_greenTooltip));
}

//------------------------------------------------------------------------------

void SStatus::changeToRed()
{
    m_indicator->setStyleSheet("background-color: red; border-radius: "+QString(m_isCircular ? "10px;" : "0")+";");
    m_indicator->setToolTip(QString::fromStdString(m_redTooltip));
}

//------------------------------------------------------------------------------

void SStatus::changeToOrange()
{
    m_indicator->setStyleSheet("background-color: orange; border-radius: "+QString(m_isCircular ? "10px;" : "0")+";");
    m_indicator->setToolTip(QString::fromStdString(m_orangeTooltip));
}

//------------------------------------------------------------------------------

void SStatus::toggleGreenRed(bool green)
{
    m_indicator->setStyleSheet("background-color:"+ QString(green ? "green" : "red")+"; border-radius: "+
                               QString(m_isCircular ? "10px;" : "0")+";");
    m_indicator->setToolTip(green ? QString::fromStdString(m_greenTooltip) : QString::fromStdString(m_redTooltip));
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiTools
