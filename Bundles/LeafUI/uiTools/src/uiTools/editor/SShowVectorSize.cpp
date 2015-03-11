/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QHBoxLayout>
#include <QString>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <fwCore/base.hpp>

#include <fwComEd/VectorMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/IService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiTools/editor/SShowVectorSize.hpp"

namespace uiTools
{

namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiTools::editor::SShowVectorSize, ::fwData::Vector );

//-----------------------------------------------------------------------------

SShowVectorSize::SShowVectorSize() throw() : m_vectorSize(0),
                                             m_textToShow("")
{

}

//------------------------------------------------------------------------------

SShowVectorSize::~SShowVectorSize() throw()
{
}

//------------------------------------------------------------------------------

void SShowVectorSize::starting() throw(::fwTools::Failed)
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget *container = qtContainer->getQtContainer();

    QHBoxLayout * layout = new QHBoxLayout();

    m_label = new QLabel();
    QString text = m_textToShow + QString::number(this->m_vectorSize);
    m_label->setText(text);

    layout->addWidget(m_label);

    container->setLayout(layout);
}

//------------------------------------------------------------------------------

void SShowVectorSize::stopping() throw(::fwTools::Failed)
{
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    qtContainer->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SShowVectorSize::configuring() throw(fwTools::Failed)
{
    this->initialize();
    if(m_configuration->findConfigurationElement("text"))
    {
        std::string configText = m_configuration->findConfigurationElement("text")->getValue();
        m_textToShow = QString::fromStdString(configText);
    }

}

//------------------------------------------------------------------------------

void SShowVectorSize::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SShowVectorSize::swapping() throw(::fwTools::Failed)
{

}
//------------------------------------------------------------------------------

void SShowVectorSize::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    if(_msg->hasEvent(::fwComEd::VectorMsg::ADDED_OBJECTS))
    {
        this->m_vectorSize++;
        QString text = m_textToShow + QString::number(this->m_vectorSize);
        m_label->setText(text);

    }
    else if(_msg->hasEvent(::fwComEd::VectorMsg::REMOVED_OBJECTS))
    {
        this->m_vectorSize--;
        QString text = m_textToShow + QString::number(this->m_vectorSize);
        m_label->setText(text);

    }
}

//------------------------------------------------------------------------------

void SShowVectorSize::info( std::ostream &_sstream )
{
}



} // namespace editor
} // namespace auroraTracker
