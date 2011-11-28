/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#define QT_NO_KEYWORDS

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QStringList>
#include <QDoubleValidator>
#include <QLabel>
#include <QPalette>
#include <QSpacerItem>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include "fwData/Composite.hpp"
#include "fwData/String.hpp"

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>

#include <fwComEd/InteractionMsg.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiVisuQt/PointEditor.hpp"
#include "fwMath/IntrasecTypes.hpp"

namespace uiVisu
{

    REGISTER_SERVICE( ::gui::editor::IEditor , ::uiVisu::PointEditor , ::fwData::Composite ) ;


    PointEditor::PointEditor() throw()
    {
        addNewHandledEvent(::fwComEd::InteractionMsg::MOUSE_MOVE);
    }

    //------------------------------------------------------------------------------

    PointEditor::~PointEditor() throw()
    {}

    //------------------------------------------------------------------------------

    void PointEditor::starting() throw(::fwTools::Failed)
    {
        SLM_TRACE_FUNC();
        this->::fwGui::IGuiContainerSrv::create();

        ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
        QWidget* const container = qtContainer->getQtContainer();
        SLM_ASSERT("container not instanced", container);

        QHBoxLayout* hLayout = new QHBoxLayout();

        QLabel* staticText_x = new QLabel( tr("x:"), container);
        hLayout->addWidget( staticText_x, 0, Qt::AlignVCenter );

        m_textCtrl_x = new QLineEdit( container );
        m_textCtrl_x->setValidator( new QDoubleValidator(m_textCtrl_x) );
        hLayout->addWidget( m_textCtrl_x, 1, Qt::AlignVCenter );

        QLabel* staticText_y = new QLabel( tr("y:"), container );
        hLayout->addWidget( staticText_y, 0, Qt::AlignVCenter );

        m_textCtrl_y = new QLineEdit( container );
        m_textCtrl_y->setValidator( new QDoubleValidator(m_textCtrl_y) );
        hLayout->addWidget( m_textCtrl_y, 1, Qt::AlignVCenter );

        QLabel* staticText_z = new QLabel( tr("z:"), container );
        hLayout->addWidget( staticText_z, 0, Qt::AlignVCenter );

        m_textCtrl_z = new QLineEdit( container );
        m_textCtrl_z->setValidator( new QDoubleValidator(m_textCtrl_z) );
        hLayout->addWidget( m_textCtrl_z, 1, Qt::AlignVCenter );


        container->setLayout( hLayout );
        this->updating();
    }

    //------------------------------------------------------------------------------

    void PointEditor::stopping() throw(::fwTools::Failed)
    {
        SLM_TRACE_FUNC();

        this->getContainer()->clean();
        this->::fwGui::IGuiContainerSrv::destroy();
    }

    //------------------------------------------------------------------------------

    void PointEditor::configuring() throw(fwTools::Failed)
    {
        SLM_TRACE_FUNC();
        this->::fwGui::IGuiContainerSrv::initialize();
    }

    //------------------------------------------------------------------------------

    void PointEditor::updating() throw(::fwTools::Failed)
    {
    }

    //------------------------------------------------------------------------------

    void PointEditor::swapping() throw(::fwTools::Failed)
    {
        this->updating();
    }

    //------------------------------------------------------------------------------

    void PointEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
    {
        SLM_TRACE_FUNC();
        ::fwComEd::InteractionMsg::csptr interactionMsg = ::fwComEd::InteractionMsg::dynamicConstCast(_msg);

        if (interactionMsg)
        {
            ::fwData::Point::csptr point = interactionMsg->getEventPoint();
            SLM_ASSERT("Sorry, the object is null", point);
            if(point)
            {
                fwVec3d  pointCoord = point->getCoord();
                m_textCtrl_x->setText(QString("%1").arg(pointCoord[0], 0, 'f', 0));
                m_textCtrl_y->setText(QString("%1").arg(pointCoord[1], 0, 'f', 0));
                m_textCtrl_z->setText(QString("%1").arg(pointCoord[2], 0, 'f', 0));
            }
            //        this->updating();
        }
    }

    //------------------------------------------------------------------------------

    void PointEditor::info( std::ostream &_sstream )
    {
        _sstream << "Point Editor";
    }

    //------------------------------------------------------------------------------
}

