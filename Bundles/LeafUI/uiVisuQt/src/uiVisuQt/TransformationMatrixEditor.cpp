/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QHBoxLayout>

#include <cmath>
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiVisuQt/TransformationMatrixEditor.hpp"

namespace uiVisu
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiVisu::TransformationMatrixEditor , ::fwData::TransformationMatrix3D ) ;


TransformationMatrixEditor::TransformationMatrixEditor() throw()
{
    addNewHandledEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED);
}

//------------------------------------------------------------------------------

TransformationMatrixEditor::~TransformationMatrixEditor() throw()
{}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QHBoxLayout* layout = new QHBoxLayout();

    m_angleSlider = new QSlider( Qt::Horizontal, container );
    m_angleSlider->setRange(0, 360);

    layout->addWidget( m_angleSlider, 1);

    QObject::connect(m_angleSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChange(int)));

    container->setLayout( layout );
    this->updating();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    QObject::disconnect(m_angleSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChange(int)));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::updating() throw(::fwTools::Failed)
{
    ::fwData::TransformationMatrix3D::sptr tm3D = this->getObject< ::fwData::TransformationMatrix3D >();

    int angle = acos (tm3D->getCoefficient(0,0)) * 180.0 / M_PI;
    m_angleSlider->setValue(angle);
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::swapping() throw(::fwTools::Failed)
{}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::onSliderChange( int angle  )
{
    ::fwData::TransformationMatrix3D::sptr tm3D = this->getObject< ::fwData::TransformationMatrix3D >();

    double angleRad = angle*M_PI/180.0;

    double cosAngle = cos(angleRad);
    double sinAngle = sin(angleRad);

    tm3D->setCoefficient(0,0, cosAngle); tm3D->setCoefficient(0,1, -sinAngle); tm3D->setCoefficient(0,2, 0); tm3D->setCoefficient(0,3, 0);
    tm3D->setCoefficient(1,0, sinAngle); tm3D->setCoefficient(1,1, cosAngle);  tm3D->setCoefficient(1,2, 0); tm3D->setCoefficient(1,2, 0);
    tm3D->setCoefficient(2,0, 0);        tm3D->setCoefficient(2,1, 0);         tm3D->setCoefficient(2,2, 1); tm3D->setCoefficient(2,3, 0);
    tm3D->setCoefficient(3,0, 0);        tm3D->setCoefficient(3,1, 0);         tm3D->setCoefficient(3,2, 0); tm3D->setCoefficient(3,3, 1);

    ::fwComEd::TransformationMatrix3DMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), tm3D, msg);
}

//------------------------------------------------------------------------------

}

