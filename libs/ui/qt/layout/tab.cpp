/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/qt/layout/tab.hpp"

#include "ui/qt/App.hpp"

#include <core/base.hpp>

#include <data/tools/color.hpp>

#include <ui/__/macros.hpp>

#include <QBoxLayout>
#include <QScrollArea>
#include <QTabWidget>

SIGHT_REGISTER_GUI(sight::ui::qt::layout::tab, sight::ui::layout::tab::REGISTRY_KEY);

namespace sight::ui::qt::layout
{

//-----------------------------------------------------------------------------

void tab::createLayout(ui::container::widget::sptr parent, const std::string& id)
{
    m_parentContainer = std::dynamic_pointer_cast<ui::qt::container::widget>(parent);
    SIGHT_ASSERT("dynamicCast widget to widget failed", m_parentContainer);
    m_parentContainer->getQtContainer()->setObjectName(QString::fromStdString(id));

    auto* layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_parentContainer->setLayout(layout);

    m_tabWidget = new QTabWidget();
    layout->addWidget(m_tabWidget);

    const std::list<ViewInfo>& views = this->getViewsInfo();

    for(const ViewInfo& viewInfo : views)
    {
        int leftBorder   = 0;
        int topBorder    = 0;
        int rightBorder  = 0;
        int bottomBorder = 0;
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

        auto* const widget      = new QWidget(m_tabWidget);
        const int minWidthSize  = std::max(viewInfo.m_minSize.first, 0);
        const int minHeightSize = std::max(viewInfo.m_minSize.second, 0);
        widget->setMinimumSize(minWidthSize, minHeightSize);
        widget->setContentsMargins(leftBorder, topBorder, rightBorder, bottomBorder);

        if(!viewInfo.m_backgroundColor.empty())
        {
            std::array<std::uint8_t, 4> rgba {};
            data::tools::color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
            std::stringstream ss;
            ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
            << static_cast<std::int16_t>(rgba[1]) << ','
            << static_cast<std::int16_t>(rgba[2]) << ','
            << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
            const QString style = QString::fromStdString(ss.str());
            widget->setStyleSheet(style + qApp->styleSheet());
        }

        ui::qt::container::widget::sptr subContainer = ui::qt::container::widget::make();
        subContainer->setQtContainer(widget);
        m_subViews.push_back(subContainer);

        int idx = 0;
        if(viewInfo.m_useScrollBar)
        {
            auto* const scrollArea = new QScrollArea(m_tabWidget);
            scrollArea->setWidget(widget);
            scrollArea->setWidgetResizable(true);
            if(!viewInfo.m_backgroundColor.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                << static_cast<std::int16_t>(rgba[1]) << ','
                << static_cast<std::int16_t>(rgba[2]) << ','
                << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
                scrollArea->setStyleSheet(style + qApp->styleSheet());
            }

            idx = m_tabWidget->addTab(scrollArea, QString::fromStdString(viewInfo.m_caption));
        }
        else
        {
            idx = m_tabWidget->addTab(widget, QString::fromStdString(viewInfo.m_caption));
        }

        if(viewInfo.m_isSelect)
        {
            m_tabWidget->setCurrentIndex(idx);
        }
    }
}

//-----------------------------------------------------------------------------

void tab::destroyLayout()
{
    this->destroySubViews();
    m_tabWidget->clear();
    m_parentContainer->clean();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::qt::layout
