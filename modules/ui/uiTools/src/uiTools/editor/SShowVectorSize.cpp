/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

#include <filesystem>
#include <QHBoxLayout>
#include <QString>

namespace uiTools
{

namespace editor
{

const ::fwCom::Slots::SlotKeyType SShowVectorSize::s_ADD_OBJECTS_SLOT    = "addObject";
const ::fwCom::Slots::SlotKeyType SShowVectorSize::s_REMOVE_OBJECTS_SLOT = "removeObjects";

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiTools::editor::SShowVectorSize, ::fwData::Vector );

//-----------------------------------------------------------------------------

SShowVectorSize::SShowVectorSize() noexcept :
    m_vectorSize(0),
    m_textToShow("")
{
    newSlot(s_ADD_OBJECTS_SLOT, &SShowVectorSize::addObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &SShowVectorSize::removeObjects, this);

}

//------------------------------------------------------------------------------

SShowVectorSize::~SShowVectorSize() noexcept
{
}

//------------------------------------------------------------------------------

void SShowVectorSize::starting()
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

void SShowVectorSize::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SShowVectorSize::configuring()
{
    this->initialize();

    ::fwServices::IService::ConfigType config = this->getConfigTree();
    m_textToShow                              = QString::fromStdString(config.get< std::string >("text", ""));

}

//------------------------------------------------------------------------------

void SShowVectorSize::updating()
{
}

//------------------------------------------------------------------------------

void SShowVectorSize::swapping()
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

::fwServices::IService::KeyConnectionsMap SShowVectorSize::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push("vector", ::fwData::Vector::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT );
    connections.push("vector", ::fwData::Vector::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace auroraTracker
