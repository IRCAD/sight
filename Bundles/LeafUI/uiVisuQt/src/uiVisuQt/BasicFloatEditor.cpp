/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QHBoxLayout>
#include <QDoubleValidator>
#include <QApplication>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwData/Float.hpp>
#include <fwData/String.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/FloatMsg.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiVisuQt/BasicFloatEditor.hpp"

namespace uiVisu
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiVisu::BasicFloatEditor , ::fwData::Float ) ;


BasicFloatEditor::BasicFloatEditor() throw()
{
    addNewHandledEvent(::fwComEd::FloatMsg::VALUE_IS_MODIFIED);
}

//------------------------------------------------------------------------------

BasicFloatEditor::~BasicFloatEditor() throw()
{}

//------------------------------------------------------------------------------

void BasicFloatEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    ::fwData::Float::sptr floatObj = this->getObject< ::fwData::Float >();

    QHBoxLayout* layout = new QHBoxLayout();
    QDoubleValidator* doubleValidator = new QDoubleValidator( m_valueCtrl );

    m_valueCtrl = new QLineEdit( container );
    m_valueCtrl->setValidator( doubleValidator );
    layout->addWidget( m_valueCtrl, 1);

    container->setLayout( layout );

    QObject::connect(m_valueCtrl, SIGNAL(textChanged(QString)), this, SLOT(onModifyValue(QString)));

    this->updating();
}

//------------------------------------------------------------------------------

void BasicFloatEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_valueCtrl, SIGNAL(textChanged(QString)), this, SLOT(onModifyValue(QString)));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void BasicFloatEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void BasicFloatEditor::updating() throw(::fwTools::Failed)
{
    ::fwData::Float::sptr floatObj = this->getObject< ::fwData::Float >();
    SLM_ASSERT("Sorry, the object is null", floatObj);

    std::stringstream ss;
    ss << floatObj->value();
    QString newText = QString::fromStdString(ss.str());
    if(newText != m_valueCtrl->text())
    {
        m_valueCtrl->setText(newText);
    }
    OSLM_TRACE(floatObj->getID() << " updated value : " << ss.str());
}

//------------------------------------------------------------------------------

void BasicFloatEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void BasicFloatEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::FloatMsg::csptr floatMsg = ::fwComEd::FloatMsg::dynamicConstCast(_msg);

    if (floatMsg)
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void BasicFloatEditor::info( std::ostream &_sstream )
{
    _sstream << "Float Editor";
}

//------------------------------------------------------------------------------

void BasicFloatEditor::onModifyValue(QString value)
{
    SLM_TRACE_FUNC();
    ::fwData::Float::sptr floatObj = this->getObject< ::fwData::Float >();
    ::fwData::Float::NewSptr oldValue;
    oldValue->deepCopy(floatObj);

    std::string strValue = value.toStdString();

    if (strValue.empty() || strValue == "-")
    {
        floatObj->value() = 0;
    }
    else if (value.endsWith( QString(",") ))
    {
        m_valueCtrl->setText( m_valueCtrl->text().replace(QChar(','), QChar('.')) );
    }
    else
    {
        int pos = 0;
        QString str = m_valueCtrl->text();
        bool isValid = (m_valueCtrl->validator()->validate(str, pos) == QValidator::Acceptable);

        if (isValid)
        {
            floatObj->value() = ::boost::lexical_cast< ::fwData::Float::ValueType >(strValue);
        }
        else
        {
            this->updating();
        }
    }

    if ( *oldValue != *floatObj )
    {
        OSLM_TRACE(floatObj->getID() << " new value : " << *floatObj);
        ::fwComEd::FloatMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::FloatMsg::VALUE_IS_MODIFIED );
        ::fwServices::IEditionService::notify(this->getSptr(), floatObj, msg);
    }
}

//------------------------------------------------------------------------------

}

