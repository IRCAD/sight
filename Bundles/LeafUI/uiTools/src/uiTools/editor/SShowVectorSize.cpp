/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTools/editor/SShowVectorSize.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

#include <boost/filesystem.hpp>
#include <boost/filesystem/convenience.hpp>

#include <QHBoxLayout>
#include <QString>

namespace uiTools
{

namespace editor
{

static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT    = "addObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT = "removeObjects";

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiTools::editor::SShowVectorSize, ::fwData::Vector );

//-----------------------------------------------------------------------------

SShowVectorSize::SShowVectorSize() throw() :
    m_vectorSize(0),
    m_textToShow("")
{
    newSlot(s_ADD_OBJECTS_SLOT, &SShowVectorSize::addObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &SShowVectorSize::removeObjects, this);

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

    QHBoxLayout* layout = new QHBoxLayout();

    m_label = new QLabel();
    QString text = m_textToShow + QString::number(this->m_vectorSize);
    m_label->setText(text);

    layout->addWidget(m_label);

    qtContainer->setLayout(layout);
}

//------------------------------------------------------------------------------

void SShowVectorSize::stopping() throw(::fwTools::Failed)
{
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

void SShowVectorSize::info( std::ostream& _sstream )
{
}

//------------------------------------------------------------------------------

void SShowVectorSize::addObjects(::fwData::Vector::ContainerType objects)
{
    m_vectorSize += objects.size();
    QString text = m_textToShow + QString::number(m_vectorSize);
    m_label->setText(text);
}

//------------------------------------------------------------------------------

void SShowVectorSize::removeObjects(::fwData::Vector::ContainerType objects)
{
    m_vectorSize -= objects.size();
    QString text = m_textToShow + QString::number(m_vectorSize);
    m_label->setText(text);
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SShowVectorSize::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Vector::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Vector::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace auroraTracker
