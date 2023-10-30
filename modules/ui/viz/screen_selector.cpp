/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "screen_selector.hpp"

#include <core/com/signal.hxx>

#include <service/macros.hpp>

#include <QApplication>
#include <QDesktopWidget>
#include <QGuiApplication>
#include <QInputDialog>
#include <QPointer>
#include <QScreen>

namespace sight::module::ui::viz
{

static const core::com::signals::key_t SCREEN_SELECTED_SIG = "screenSelected";

//------------------------------------------------------------------------------

screen_selector::screen_selector() :
    m_screen_selected_sig(new_signal<screen_selected_signal_t>(SCREEN_SELECTED_SIG))
{
}

//------------------------------------------------------------------------------

screen_selector::~screen_selector()
= default;

//------------------------------------------------------------------------------

void screen_selector::configuring()
{
    this->initialize();

    const auto config_tree = this->get_config();

    m_mode = config_tree.get("config.<xmlattr>.mode", m_mode);

    SIGHT_ERROR_IF(
        "Unknown selection mode '" + m_mode + "'.",
        m_mode != "current" && m_mode != "neighbor" && m_mode != "select"
    );
}

//------------------------------------------------------------------------------

void screen_selector::starting()
{
    this->action_service_starting();
}

//------------------------------------------------------------------------------

void screen_selector::updating()
{
    int screen_num = -1;
    if(m_mode == "select")
    {
        screen_num = sight::module::ui::viz::screen_selector::select_screen();
    }
    else
    {
        const QDesktopWidget* desktop = QApplication::desktop();

        screen_num = desktop->screenNumber(qApp->activeWindow());

        if(m_mode == "neighbor")
        {
            screen_num++;
        }

        const auto screens = QGuiApplication::screens();
        if(screen_num >= QGuiApplication::screens().count())
        {
            QScreen* screen = QGuiApplication::primaryScreen();
            auto it         = std::ranges::find(screens, screen);
            screen_num = it - screens.begin();
        }
    }

    if(screen_num >= 0)
    {
        m_screen_selected_sig->async_emit(screen_num);
    }
}

//------------------------------------------------------------------------------

void screen_selector::stopping()
{
    this->action_service_stopping();
}

//------------------------------------------------------------------------------

int screen_selector::select_screen()
{
    QStringList screen_names;
    int screen_number = 0;

    if(QGuiApplication::screens().size() <= 1)
    {
        return 0;
    }

    for(QScreen* screen : QGuiApplication::screens())
    {
        const QString number_str = QString::number(screen_number++) + ".";

        // Compute the screen's diagonal length in inches.
        constexpr qreal inches_per_millimeter = 0.03937008;
        const auto screen_size                = screen->physicalSize();
        const qreal diagonal_length_mm        = std::sqrt(
            screen_size.width() * screen_size.width()
            + screen_size.height() * screen_size.height()
        );
        const qreal diagonal_length_inches = diagonal_length_mm * inches_per_millimeter;

        const QString diagonal = QString::number(diagonal_length_inches, 'f', 1) + "\"";

        const auto geom          = screen->geometry();
        const QString resolution = "[" + QString::number(geom.width()) + "x" + QString::number(geom.height()) + "]";

        QString display_name = screen->manufacturer() + " " + screen->model();
        if(display_name.size() == 1)
        {
            display_name = screen->name();
        }

        screen_names << number_str + " " + diagonal + " " + resolution + " " + display_name;
    }

    bool ok_clicked       = false;
    QString selected_item = QInputDialog::getItem(
        nullptr,
        "Select a screen.",
        "Screen:",
        screen_names,
        0,
        false,
        &ok_clicked
    );

    std::int64_t ret_screen = -1;
    if(ok_clicked)
    {
        auto name_it = std::find(screen_names.cbegin(), screen_names.cend(), selected_item);

        if(name_it != screen_names.cend())
        {
            ret_screen = std::distance(screen_names.cbegin(), name_it);
        }
    }

    return static_cast<int>(ret_screen);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::viz
