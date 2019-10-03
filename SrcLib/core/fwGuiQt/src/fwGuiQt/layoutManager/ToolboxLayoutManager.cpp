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

#include "fwGuiQt/layoutManager/ToolboxLayoutManager.hpp"

#include "fwGuiQt/App.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/registry/macros.hpp>

#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

fwGuiRegisterMacro( ::fwGui::ToolboxLayoutManager, ::fwGui::layoutManager::ToolboxLayoutManagerBase::REGISTRY_KEY );

namespace fwGui
{

//-----------------------------------------------------------------------------

ToolboxLayoutManager::ToolboxLayoutManager(::fwGui::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

ToolboxLayoutManager::~ToolboxLayoutManager()
{
}

//-----------------------------------------------------------------------------

void ToolboxLayoutManager::createLayout( ::fwGui::container::fwContainer::sptr parent )
{
    SLM_TRACE_FUNC();

    ::fwGuiQt::container::QtContainer::sptr parentContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(parent);

    QVBoxLayout* layout = new QVBoxLayout();
    parentContainer->setLayout(layout);

    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* sv = new QScrollArea();
    ::fwGuiQt::widget::QfwToolBox* toolbox = new ::fwGuiQt::widget::QfwToolBox(sv);
    sv->setWidget(toolbox);
    sv->setWidgetResizable(true);
    layout->addWidget(sv);

    const std::list< ViewInfo>& views = this->getViewsInfo();
    for ( ViewInfo viewInfo : views)
    {
        int border     = viewInfo.m_border;
        QWidget* panel = new QWidget();
        panel->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
        panel->setContentsMargins(border, border, border, border);
        if(!viewInfo.m_backgroundColor.empty())
        {
            const QString style = QString::fromStdString(
                "QWidget { background-color: " + viewInfo.m_backgroundColor + ";}");
            panel->setStyleSheet(style + qApp->styleSheet());
        }

        ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();
        subContainer->setQtContainer(panel);
        m_subViews.push_back(subContainer);

        int index = 0;
        if(viewInfo.m_useScrollBar)
        {
            QScrollArea* scrollArea = new QScrollArea(toolbox);
            scrollArea->setWidget(panel);
            scrollArea->setWidgetResizable( true );
            if(!viewInfo.m_backgroundColor.empty())
            {
                const QString style = QString::fromStdString(
                    "QWidget { background-color: " + viewInfo.m_backgroundColor + ";}");
                scrollArea->setStyleSheet(style + qApp->styleSheet());
            }
            index = toolbox->addItem(scrollArea, QString::fromStdString(viewInfo.m_caption));
        }
        else
        {
            index = toolbox->addItem(panel, QString::fromStdString(viewInfo.m_caption));
        }

        if(viewInfo.m_expanded)
        {
            toolbox->expandItem(index);
        }

        if(false == viewInfo.m_visible)
        {
            subContainer->setVisible(false);
        }
    }
}

//-----------------------------------------------------------------------------

void ToolboxLayoutManager::destroyLayout()
{
    this->destroySubViews();
}

//-----------------------------------------------------------------------------

} // namespace fwGui
