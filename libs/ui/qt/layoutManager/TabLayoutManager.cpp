/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/qt/layoutManager/TabLayoutManager.hpp"

#include "ui/qt/App.hpp"

#include <core/base.hpp>

#include <data/tools/Color.hpp>

#include <ui/base/registry/macros.hpp>

#include <QBoxLayout>
#include <QScrollArea>
#include <QTabWidget>

fwGuiRegisterMacro( sight::ui::qt::TabLayoutManager,
                    sight::ui::base::layoutManager::TabLayoutManagerBase::REGISTRY_KEY );

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

TabLayoutManager::TabLayoutManager(ui::base::GuiBaseObject::Key key)
{
}

//-----------------------------------------------------------------------------

TabLayoutManager::~TabLayoutManager()
{
}

//-----------------------------------------------------------------------------

void TabLayoutManager::createLayout( ui::base::container::fwContainer::sptr parent )
{
    m_parentContainer = ui::qt::container::QtContainer::dynamicCast(parent);
    SIGHT_ASSERT("dynamicCast fwContainer to QtContainer failed", m_parentContainer);

    QBoxLayout* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_parentContainer->setLayout(layout);

    m_tabWidget = new QTabWidget();
    layout->addWidget( m_tabWidget );

    const std::list< ViewInfo>& views = this->getViewsInfo();

    for ( ViewInfo viewInfo : views)
    {
        int leftBorder;
        int topBorder;
        int rightBorder;
        int bottomBorder;
        if(viewInfo.m_border != 0)
        {
            leftBorder = topBorder = rightBorder = bottomBorder = viewInfo.m_border;
        }
        else
        {
            leftBorder   = viewInfo.m_leftBorder;
            topBorder    = viewInfo.m_topBorder;
            rightBorder  = viewInfo.m_rightBorder;
            bottomBorder = viewInfo.m_bottomBorder;
        }

        QWidget* const widget   = new QWidget(m_tabWidget);
        const int minWidthSize  = std::max(viewInfo.m_minSize.first, 0);
        const int minHeightSize = std::max(viewInfo.m_minSize.second, 0);
        widget->setMinimumSize(minWidthSize, minHeightSize);
        widget->setContentsMargins(leftBorder, topBorder, rightBorder, bottomBorder);

        if(!viewInfo.m_backgroundColor.empty())
        {
            std::uint8_t rgba[4];
            data::tools::Color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
            std::stringstream ss;
            ss << "QWidget { background-color: rgba(" << static_cast< short >(rgba[0]) << ','
               << static_cast< short >(rgba[1]) << ','
               << static_cast< short >(rgba[2]) << ','
               << (static_cast< float >(rgba[3])/255.f)*100 << "%); } ";
            const QString style = QString::fromStdString(ss.str());
            widget->setStyleSheet(style + qApp->styleSheet());
        }

        ui::qt::container::QtContainer::sptr subContainer = ui::qt::container::QtContainer::New();
        subContainer->setQtContainer(widget);
        m_subViews.push_back(subContainer);

        int idx = 0;
        if(viewInfo.m_useScrollBar)
        {
            QScrollArea* const scrollArea = new QScrollArea(m_tabWidget);
            scrollArea->setWidget(widget);
            scrollArea->setWidgetResizable( true );
            if(!viewInfo.m_backgroundColor.empty())
            {
                std::uint8_t rgba[4];
                data::tools::Color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast< short >(rgba[0]) << ','
                   << static_cast< short >(rgba[1]) << ','
                   << static_cast< short >(rgba[2]) << ','
                   << (static_cast< float >(rgba[3])/255.f)*100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
                scrollArea->setStyleSheet(style + qApp->styleSheet());
            }
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

} // namespace sight::ui::qt
