/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiVisuQt/PointEditor.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <QDoubleValidator>
#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QSpacerItem>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

namespace uiVisu
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisu::PointEditor, ::fwData::Composite );

static const ::fwCom::Slots::SlotKeyType s_GET_INTERACTION_SLOT = "getInteraction";

PointEditor::PointEditor() noexcept
{
    newSlot(s_GET_INTERACTION_SLOT, &PointEditor::getInteraction, this);
}

//------------------------------------------------------------------------------

PointEditor::~PointEditor() noexcept
{
}

//------------------------------------------------------------------------------

void PointEditor::starting()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QHBoxLayout* hLayout = new QHBoxLayout();

    QLabel* staticText_x = new QLabel( tr("x:"));
    hLayout->addWidget( staticText_x, 0, Qt::AlignVCenter );

    m_textCtrl_x = new QLineEdit();
    m_textCtrl_x->setValidator( new QDoubleValidator(m_textCtrl_x) );
    hLayout->addWidget( m_textCtrl_x, 1, Qt::AlignVCenter );

    QLabel* staticText_y = new QLabel( tr("y:") );
    hLayout->addWidget( staticText_y, 0, Qt::AlignVCenter );

    m_textCtrl_y = new QLineEdit();
    m_textCtrl_y->setValidator( new QDoubleValidator(m_textCtrl_y) );
    hLayout->addWidget( m_textCtrl_y, 1, Qt::AlignVCenter );

    QLabel* staticText_z = new QLabel( tr("z:"));
    hLayout->addWidget( staticText_z, 0, Qt::AlignVCenter );

    m_textCtrl_z = new QLineEdit();
    m_textCtrl_z->setValidator( new QDoubleValidator(m_textCtrl_z) );
    hLayout->addWidget( m_textCtrl_z, 1, Qt::AlignVCenter );

    qtContainer->setLayout( hLayout );
    this->updating();
}

//------------------------------------------------------------------------------

void PointEditor::stopping()
{
    SLM_TRACE_FUNC();

    this->destroy();
}

//------------------------------------------------------------------------------

void PointEditor::configuring()
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void PointEditor::updating()
{
}

//------------------------------------------------------------------------------

void PointEditor::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void PointEditor::getInteraction(::fwDataTools::PickingInfo info)
{
    if ( info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_MOVE )
    {
        m_textCtrl_x->setText(QString("%1").arg(info.m_worldPos[0], 0, 'f', 0));
        m_textCtrl_y->setText(QString("%1").arg(info.m_worldPos[1], 0, 'f', 0));
        m_textCtrl_z->setText(QString("%1").arg(info.m_worldPos[2], 0, 'f', 0));
    }
}

//------------------------------------------------------------------------------

void PointEditor::info( std::ostream& _sstream )
{
    _sstream << "Point Editor";
}

//------------------------------------------------------------------------------
}

