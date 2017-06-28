/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/layoutManager/TabLayoutManager.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <QBoxLayout>
#include <QScrollArea>
#include <QTabWidget>

fwGuiRegisterMacro( ::fwGui::TabLayoutManager, ::fwGui::layoutManager::TabLayoutManagerBase::REGISTRY_KEY );

namespace fwGui
{

//-----------------------------------------------------------------------------

TabLayoutManager::TabLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

TabLayoutManager::~TabLayoutManager()
{
}

//-----------------------------------------------------------------------------

void TabLayoutManager::createLayout( ::fwGui::container::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();
    m_parentContainer = ::fwGuiQt::container::QtContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);

    m_tabWidget = new QTabWidget();

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    layout->addWidget( m_tabWidget );

    const std::list< ViewInfo>& views = this->getViewsInfo();

    for ( ViewInfo viewInfo : views)
    {
        QWidget* widget = new QWidget(m_tabWidget);

        ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();
        subContainer->setQtContainer(widget);
        m_subViews.push_back(subContainer);

        int idx = 0;
        if(viewInfo.m_useScrollBar)
        {
            QScrollArea* scrollArea = new QScrollArea(m_tabWidget);
            scrollArea->setWidget(widget);
            scrollArea->setWidgetResizable( true );
            idx = m_tabWidget->addTab( scrollArea, QString::fromStdString(viewInfo.m_caption));
        }
        else
        {
            idx = m_tabWidget->addTab( widget, QString::fromStdString(viewInfo.m_caption));
        }
        if (viewInfo.m_isSelect )
        {
            m_tabWidget->setCurrentIndex(idx);
        }
    }

    m_parentContainer->setLayout(layout);
}

//-----------------------------------------------------------------------------

void TabLayoutManager::destroyLayout()
{
    this->destroySubViews();
    m_tabWidget->clear();
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace fwGui

