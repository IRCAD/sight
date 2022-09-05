/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/qt/layoutManager/ToolboxLayoutManager.hpp"

#include "ui/qt/App.hpp"
#include "ui/qt/container/QtContainer.hpp"

#include <core/base.hpp>

#include <data/tools/Color.hpp>

#include <ui/base/registry/macros.hpp>

#include <QScrollArea>
#include <QStyle>
#include <QVBoxLayout>

// cspell: ignore QWIDGETSIZE_MAX

fwGuiRegisterMacro(
    sight::ui::qt::ToolboxLayoutManager,
    sight::ui::base::layoutManager::ToolboxLayoutManagerBase::REGISTRY_KEY
);

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

ToolboxLayoutManager::ToolboxLayoutManager(ui::base::GuiBaseObject::Key /*unused*/)
{
}

//-----------------------------------------------------------------------------

ToolboxLayoutManager::~ToolboxLayoutManager()
= default;

//-----------------------------------------------------------------------------

void ToolboxLayoutManager::createLayout(ui::base::container::fwContainer::sptr parent, const std::string& id)
{
    const QString qId                                    = QString::fromStdString(id);
    ui::qt::container::QtContainer::sptr parentContainer =
        ui::qt::container::QtContainer::dynamicCast(parent);
    parentContainer->getQtContainer()->setObjectName(qId);

    auto* layout = new QVBoxLayout();
    parentContainer->setLayout(layout);

    layout->setContentsMargins(0, 0, 0, 0);

    auto* sv      = new QScrollArea();
    auto* toolbox = new ui::qt::widget::QfwToolBox(sv);
    sv->setWidget(toolbox);
    sv->setWidgetResizable(true);
    layout->addWidget(sv);

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

        auto* panel = new QWidget();
        panel->setObjectName(qId + '/' + viewInfo.m_caption.c_str());
        panel->setMinimumSize(std::max(viewInfo.m_minSize.first, 0), std::max(viewInfo.m_minSize.second, 0));
        panel->setMaximumSize(
            std::min(viewInfo.m_maxSize.first, QWIDGETSIZE_MAX),
            std::min(viewInfo.m_maxSize.second, QWIDGETSIZE_MAX)
        );
        panel->setContentsMargins(leftBorder, topBorder, rightBorder, bottomBorder);
        if(!viewInfo.m_backgroundColor.empty())
        {
            std::array<std::uint8_t, 4> rgba {};
            data::tools::Color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
            std::stringstream ss;
            ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
            << static_cast<std::int16_t>(rgba[1]) << ','
            << static_cast<std::int16_t>(rgba[2]) << ','
            << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
            const QString style = QString::fromStdString(ss.str());
            panel->setStyleSheet(style + qApp->styleSheet());
        }

        ui::qt::container::QtContainer::sptr subContainer = ui::qt::container::QtContainer::New();
        subContainer->setQtContainer(panel);
        m_subViews.push_back(subContainer);

        int index = 0;
        if(viewInfo.m_useScrollBar)
        {
            auto* scrollArea = new QScrollArea(toolbox);
            scrollArea->setWidget(panel);
            scrollArea->setWidgetResizable(true);
            if(!viewInfo.m_backgroundColor.empty())
            {
                std::array<std::uint8_t, 4> rgba {};
                data::tools::Color::hexaStringToRGBA(viewInfo.m_backgroundColor, rgba);
                std::stringstream ss;
                ss << "QWidget { background-color: rgba(" << static_cast<std::int16_t>(rgba[0]) << ','
                << static_cast<std::int16_t>(rgba[1]) << ','
                << static_cast<std::int16_t>(rgba[2]) << ','
                << (static_cast<float>(rgba[3]) / 255.F) * 100 << "%); } ";
                const QString style = QString::fromStdString(ss.str());
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

        if(!viewInfo.m_visible)
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

} // namespace sight::ui::qt
