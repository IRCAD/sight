#include <QHBoxLayout>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwData/String.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/StringMsg.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "guiQt/editor/Code.hpp"


namespace guiQt
{

namespace editor
{


REGISTER_SERVICE( ::gui::editor::IEditor , ::guiQt::editor::Code , ::fwData::String ) ;


Code::Code() throw()
{
    addNewHandledEvent(::fwComEd::StringMsg::VALUE_IS_MODIFIED);
}

//------------------------------------------------------------------------------

Code::~Code() throw()
{}

//------------------------------------------------------------------------------

void Code::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    assert( container );

    ::fwData::String::sptr stringObj = this->getObject< ::fwData::String >();

    QHBoxLayout* layout = new QHBoxLayout();
    m_valueCtrl = new QTextEdit( container );
    layout->addWidget( m_valueCtrl, 1);

    container->setLayout( layout );

    QObject::connect(m_valueCtrl, SIGNAL(textChanged()), this, SLOT(onModifyValue()));
    this->updating();
}

//------------------------------------------------------------------------------

void Code::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_valueCtrl, SIGNAL(textChanged(QString)), this, SLOT(onModifyValue(QString)));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void Code::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void Code::updating() throw(::fwTools::Failed)
{
    ::fwData::String::sptr stringObj = this->getObject< ::fwData::String >();
    SLM_ASSERT("Sorry, the object is null", stringObj);

    m_valueCtrl->setText(QString::fromStdString(stringObj->value()));
    OSLM_TRACE(stringObj->getID() << " updated value : " << stringObj->value());
}

//------------------------------------------------------------------------------

void Code::swapping() throw(::fwTools::Failed)
{
    this->updating();
}
//------------------------------------------------------------------------------

void Code::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    ::fwComEd::StringMsg::csptr stringMsg = ::fwComEd::StringMsg::dynamicConstCast(_msg);

    if (stringMsg)
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void Code::info( std::ostream &_sstream )
{
    _sstream << "String Editor";
}

//------------------------------------------------------------------------------

void Code::onModifyValue()
{
    QString value = m_valueCtrl->toPlainText();
    ::fwData::String::sptr stringObj = this->getObject< ::fwData::String >();
    ::fwData::String::NewSptr oldValue;
    oldValue->deepCopy(stringObj);

    std::string strValue = value.toStdString();
    stringObj->value() = strValue;

    if ( oldValue->value() != stringObj->value() )
    {
        OSLM_TRACE( stringObj->getID() << " modified");
        ::fwComEd::StringMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::StringMsg::VALUE_IS_MODIFIED );
        ::fwServices::IEditionService::notify(this->getSptr(), stringObj, msg);
    }
}


} // namespace editor

} // namespace guiQt

