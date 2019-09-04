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

#pragma once

#include "fwGuiQt/config.hpp"
#include "fwGuiQt/container/QtContainer.hpp"

#include <fwCore/base.hpp>

#include <fwGui/layoutManager/IFrameLayoutManager.hpp>

#include <QObject>
#include <QPoint>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QMainWindow;
QT_END_NAMESPACE

namespace fwGui
{

/**
 * @brief   Defines the frame manager.
 */
class FWGUIQT_CLASS_API FrameLayoutManager : public QObject,
                                             public ::fwGui::layoutManager::IFrameLayoutManager
{
Q_OBJECT
public:

    fwCoreClassDefinitionsWithFactoryMacro( (FrameLayoutManager)(::fwGui::layoutManager::IFrameLayoutManager),
                                            ::fwGui::factory::New< FrameLayoutManager > );

    FWGUIQT_API FrameLayoutManager(::fwGui::GuiBaseObject::Key key);

    FWGUIQT_API virtual ~FrameLayoutManager();

    /**
     * @brief Instantiate frame.
     */
    FWGUIQT_API virtual void createFrame() override;

    /**
     * @brief Destroy local frame with sub containers.
     */
    FWGUIQT_API virtual void destroyFrame() override;

private Q_SLOTS:
    void onCloseFrame();

private:

    void setState(FrameState state);
    FrameState getState();
    bool isOnScreen(const QPoint& pos);

    QPointer<QMainWindow> m_qtWindow;
};

} // namespace fwGui
