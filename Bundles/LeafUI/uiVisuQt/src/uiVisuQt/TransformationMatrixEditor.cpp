/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QHBoxLayout>

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#include <fwCore/base.hpp>

#include <fwData/TransformationMatrix3D.hpp>

#include <fwServices/macros.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiVisuQt/TransformationMatrixEditor.hpp"

namespace uiVisu
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiVisu::TransformationMatrixEditor,
                         ::fwData::TransformationMatrix3D );

TransformationMatrixEditor::TransformationMatrixEditor() noexcept
{
}

//------------------------------------------------------------------------------

TransformationMatrixEditor::~TransformationMatrixEditor() noexcept
{
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::starting()
{
    SLM_TRACE_FUNC();

    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer = ::fwGuiQt::container::QtContainer::dynamicCast(
        this->getContainer() );

    QHBoxLayout* layout = new QHBoxLayout();

    m_angleSlider = new QSlider( Qt::Horizontal );
    m_angleSlider->setRange(0, 360);

    layout->addWidget( m_angleSlider, 1);

    QObject::connect(m_angleSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChange(int)));

    qtContainer->setLayout( layout );
    this->updating();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::stopping()
{
    SLM_TRACE_FUNC();
    QObject::disconnect(m_angleSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderChange(int)));

    this->destroy();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::configuring()
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::updating()
{
    ::fwData::TransformationMatrix3D::sptr tm3D = this->getInOut< ::fwData::TransformationMatrix3D>("matrix");

    int angle = acos(tm3D->getCoefficient(0, 0)) * 180.0 / M_PI;
    m_angleSlider->setValue(angle);
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::swapping()
{
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void TransformationMatrixEditor::onSliderChange( int angle  )
{
    ::fwData::TransformationMatrix3D::sptr tm3D = this->getInOut< ::fwData::TransformationMatrix3D >("matrix");

    double angleRad = angle*M_PI/180.0;

    double cosAngle = cos(angleRad);
    double sinAngle = sin(angleRad);

    tm3D->setCoefficient(0, 0, cosAngle); tm3D->setCoefficient(0, 1, -sinAngle); tm3D->setCoefficient(0, 2, 0);
    tm3D->setCoefficient(0, 3, 0);
    tm3D->setCoefficient(1, 0, sinAngle); tm3D->setCoefficient(1, 1, cosAngle);  tm3D->setCoefficient(1, 2, 0);
    tm3D->setCoefficient(1, 2, 0);
    tm3D->setCoefficient(2, 0, 0);        tm3D->setCoefficient(2, 1, 0);         tm3D->setCoefficient(2, 2, 1);
    tm3D->setCoefficient(2, 3, 0);
    tm3D->setCoefficient(3, 0, 0);        tm3D->setCoefficient(3, 1, 0);         tm3D->setCoefficient(3, 2, 0);
    tm3D->setCoefficient(3, 3, 1);

    auto sig = tm3D->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

}

