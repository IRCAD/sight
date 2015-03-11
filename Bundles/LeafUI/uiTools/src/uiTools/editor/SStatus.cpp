/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTools/editor/SStatus.hpp"

#include <fwCom/Slots.hxx>
#include <fwCore/base.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <QWidget>
#include <QHBoxLayout>
#include <QPalette>
#include <QColor>


namespace uiTools
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiTools::editor::SStatus, ::fwData::Object );



const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_GREEN_SLOT  = "changeToGreen";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_RED_SLOT    = "changeToRed";
const ::fwCom::Slots::SlotKeyType SStatus::s_CHANGE_TO_ORANGE_SLOT = "changeToOrange";


//-----------------------------------------------------------------------------

SStatus::SStatus() throw()
{

    m_slotChangeToGreen  = ::fwCom::newSlot( &SStatus::changeToGreen,  this );
    m_slotChangeToRed    = ::fwCom::newSlot( &SStatus::changeToRed,  this );
    m_slotChangeToOrange = ::fwCom::newSlot( &SStatus::changeToOrange,  this );

    ::fwCom::HasSlots::m_slots
        ( s_CHANGE_TO_GREEN_SLOT, m_slotChangeToGreen   )
        ( s_CHANGE_TO_RED_SLOT, m_slotChangeToRed    )
        ( s_CHANGE_TO_ORANGE_SLOT, m_slotChangeToOrange    )
    ;


#ifdef COM_LOG
    ::fwCom::HasSlots::m_slots.setID();
#endif

    ::fwCom::HasSlots::m_slots.setWorker(m_associatedWorker);
}

//------------------------------------------------------------------------------

SStatus::~SStatus() throw()
{
}

//------------------------------------------------------------------------------

void SStatus::starting() throw(::fwTools::Failed)
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget *container = qtContainer->getQtContainer();

    QHBoxLayout * layout = new QHBoxLayout();

    m_label = new QLabel();
    m_label->setMinimumSize(16, 16);

    layout->addWidget(m_label);

    container->setLayout(layout);

    this->changeToRed();
}

//------------------------------------------------------------------------------

void SStatus::stopping() throw(::fwTools::Failed)
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    qtContainer->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SStatus::configuring() throw(fwTools::Failed)
{
    this->initialize();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigType;

    ConfigType greenCfg  = m_configuration->findConfigurationElement("green");
    ConfigType redCfg    = m_configuration->findConfigurationElement("red");
    ConfigType orangeCfg = m_configuration->findConfigurationElement("orange");

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
}

//------------------------------------------------------------------------------

void SStatus::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SStatus::swapping() throw(::fwTools::Failed)
{

}

//------------------------------------------------------------------------------

void SStatus::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void SStatus::changeToGreen()
{
    m_label->setStyleSheet("QLabel { background-color : green;}");
    m_label->setToolTip(QString::fromStdString(m_greenTooltip));
}

//------------------------------------------------------------------------------

void SStatus::changeToRed()
{
    m_label->setStyleSheet("QLabel { background-color : red;}");
    m_label->setToolTip(QString::fromStdString(m_redTooltip));
}

//------------------------------------------------------------------------------

void SStatus::changeToOrange()
{
    m_label->setStyleSheet("QLabel { background-color : orange;}");
    m_label->setToolTip(QString::fromStdString(m_orangeTooltip));
}

//------------------------------------------------------------------------------


} // namespace editor
} // namespace uiTools
