/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGuiQml/layoutManager/TabLayoutManager.hpp"

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
    m_parentContainer = ::fwGuiQml::container::QmlContainer::dynamicCast(parent);
    SLM_ASSERT("dynamicCast fwContainer to QmlContainer failed", m_parentContainer);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_parentContainer->setLayout(layout);

    m_tabWidget = new QTabWidget();
    layout->addWidget( m_tabWidget );

    const std::list< ViewInfo>& views = this->getViewsInfo();

    for ( ViewInfo viewInfo : views)
    {
        const int border        = viewInfo.m_border;
        QWidget* const widget   = new QWidget(m_tabWidget);
        const int minWidthSize  = std::max(viewInfo.m_minSize.first, 0);
        const int minHeightSize = std::max(viewInfo.m_minSize.second, 0);
        widget->setMinimumSize(minWidthSize, minHeightSize);
        widget->setContentsMargins(border, border, border, border);

        ::fwGuiQml::container::QmlContainer::sptr subContainer = ::fwGuiQml::container::QmlContainer::New();
        subContainer->setQmlContainer(widget);
        m_subViews.push_back(subContainer);

        int idx = 0;
        if(viewInfo.m_useScrollBar)
        {
            QScrollArea* const scrollArea = new QScrollArea(m_tabWidget);
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
