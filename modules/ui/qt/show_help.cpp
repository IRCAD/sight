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

#include "show_help.hpp"

#include <core/base.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/message.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QObject>
#include <QSplitter>
#include <QTextBrowser>

#include <filesystem>

namespace sight::module::ui::qt
{

//------------------------------------------------------------------------------
/** @brief Help browser */
class help_browser : public QTextBrowser
{
public:

    explicit help_browser(QHelpEngine* _help_engine, QWidget* _parent = nullptr) :
        QTextBrowser(_parent),
        m_help_engine(_help_engine)
    {
    }

    //------------------------------------------------------------------------------

    QVariant loadResource(int _type, const QUrl& _url) override
    {
        if(_url.scheme() == "qthelp")
        {
            return {m_help_engine->fileData(_url)};
        }

        return QTextBrowser::loadResource(_type, _url);
    }

private:

    QHelpEngine* m_help_engine;
};
//------------------------------------------------------------------------------

void show_help::info(std::ostream& _sstream)
{
    _sstream << "Action for show help contents" << std::endl;
}

//------------------------------------------------------------------------------

void show_help::configuring()
{
    /*
     * .qhp/.qch (source/binary) : Contains a table of contents,
     *                             an index of items in the documentation, and a file manifest.
     * .qhcp/.qhc (source/binary): Contains information that is used to customize
     *                             the appearance and available features of Qt Assistant.
     */
    this->sight::ui::action::initialize();

    const auto configuration = this->get_config();
    const auto filename      = configuration.get<std::string>("filename.<xmlattr>.id");
    m_fs_help_path            = std::filesystem::path(filename);
    m_b_service_is_configured = std::filesystem::exists(m_fs_help_path);
    SIGHT_WARN_IF("Help file " << filename << " doesn't exist", !m_b_service_is_configured);
}

//------------------------------------------------------------------------------

void show_help::updating()
{
    SIGHT_ASSERT("The Help service isn't configured properly.", m_b_service_is_configured);

    auto* dialog = new QDialog(qApp->activeWindow());
    dialog->setWindowTitle(QString("Help"));
    auto* help_engine = new QHelpEngine(QString::fromStdString(m_fs_help_path.string()), dialog);
    if(!help_engine->setupData())
    {
        SIGHT_ERROR("HelpEngine error: " << help_engine->error().toStdString());
        sight::ui::dialog::message message_box;
        message_box.set_title("Warning");
        message_box.set_message("Help file is missing or not correct.");
        message_box.set_icon(sight::ui::dialog::message::warning);
        message_box.add_button(sight::ui::dialog::message::ok);
        message_box.show();
        // Setup help engine information failed.
        // qhc (Qt Help Collection) or qch (Qt Compressed Help) file is not correct.
    }
    else
    {
        auto* help_panel = new QSplitter(Qt::Horizontal);
        auto* help_browser = new class help_browser (help_engine, dialog) ;
                                 help_panel->insertWidget(0, help_engine->contentWidget());
        help_panel->insertWidget(1, help_browser);
        help_panel->setStretchFactor(1, 1);

        auto* h_layout = new QHBoxLayout();
        h_layout->addWidget(help_panel);
        dialog->setLayout(h_layout);
        QObject::connect(
            help_engine->contentWidget(),
            SIGNAL(linkActivated(const QUrl&)),
            help_browser,
            SLOT(setSource(const QUrl&))
        );

        dialog->exec();
    }
}

//------------------------------------------------------------------------------

void show_help::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------

void show_help::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt
