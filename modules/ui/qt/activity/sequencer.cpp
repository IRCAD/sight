/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2021 IHU Strasbourg
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

#include "sequencer.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/tools/os.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QApplication>
#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QVBoxLayout>

namespace sight::module::ui::qt::activity
{

//------------------------------------------------------------------------------

static const std::string CLEAR_ACTIVITIES_CONFIG = "clearActivities";
static const std::string THEME_CONFIG            = "theme";
static const std::string CLEAR_CONFIG            = "clear";
static const std::string ACCENT_CONFIG           = "accent";
static const std::string FOREGROUND_CONFIG       = "foreground";
static const std::string BACKGROUND_CONFIG       = "background";
static const std::string PRIMARY_CONFIG          = "primary";
static const std::string ELEVATION_CONFIG        = "elevation";
static const std::string BUTTON_WIDTH            = "buttonWidth";
static const std::string FONT_SIZE               = "fontSize";
static const std::string WARNING_MESSAGE         = "warning_message";

//------------------------------------------------------------------------------

sequencer::sequencer() noexcept
{
    new_slot(slots::GO_TO, &sequencer::go_to, this);
    new_slot(slots::CHECK_NEXT, &sequencer::check_next, this);
    new_slot(slots::VALIDATE_NEXT, &sequencer::validate_next, this);
    new_slot(slots::NEXT, &sequencer::next, this);
    new_slot(slots::PREVIOUS, &sequencer::previous, this);
    new_slot(slots::SEND_INFO, &sequencer::send_info, this);
    new_slot(slots::RESET_REQUIREMENTS, &sequencer::reset_requirements, this);
    new_slot(slots::ENABLE_USER_WARNING, &sequencer::enable_user_warning, this);
    new_slot(slots::DISABLE_USER_WARNING, &sequencer::disable_user_warning, this);
    new_slot(slots::SET_USER_WARNING, &sequencer::set_user_warning, this);
}

//------------------------------------------------------------------------------

void sequencer::configuring()
{
    this->sight::ui::service::initialize();

    const service::config_t config = this->get_config();

    auto pair = config.equal_range("activity");
    auto it   = pair.first;
    for( ; it != pair.second ; ++it)
    {
        m_activity_ids.push_back(it->second.get<std::string>("<xmlattr>.id"));
        m_activity_names.push_back(it->second.get<std::string>("<xmlattr>.name", ""));
    }

    m_clear_activities = config.get<bool>(CLEAR_ACTIVITIES_CONFIG, m_clear_activities);

    m_theme           = config.get<std::string>(THEME_CONFIG, m_theme);
    m_clear           = config.get<std::string>(CLEAR_CONFIG, m_clear);
    m_accent          = config.get<std::string>(ACCENT_CONFIG, m_accent);
    m_foreground      = config.get<std::string>(FOREGROUND_CONFIG, m_foreground);
    m_background      = config.get<std::string>(BACKGROUND_CONFIG, m_background);
    m_primary         = config.get<std::string>(PRIMARY_CONFIG, m_primary);
    m_elevation       = config.get<std::string>(ELEVATION_CONFIG, m_elevation);
    m_button_width    = config.get<std::string>(BUTTON_WIDTH, m_button_width);
    m_font_size       = config.get<double>(FONT_SIZE, m_font_size);
    m_warning_message = config.get<std::string>(WARNING_MESSAGE, m_warning_message);

    m_warn_user = !m_warning_message.empty();
}

//------------------------------------------------------------------------------

void sequencer::starting()
{
    this->sight::ui::service::create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* main_layout = new QVBoxLayout();
    main_layout->setContentsMargins(0, 0, 0, 0);

    m_widget = new QQuickWidget();
    main_layout->addWidget(m_widget);

    const auto path =
        core::runtime::get_module_resource_file_path("sight::module::ui::qt", "ActivitySequencer.qml");
    QWidget* parent = qt_container->get_qt_container();
    auto* engine    = m_widget->engine();
    m_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QColor clear;
    if(m_clear.empty())
    {
        clear = parent->palette().color(QPalette::Window);

        // styleSheet override QPalette
        // we assume that styleSheet is the dark style
        if(!qApp->styleSheet().isEmpty())
        {
            clear = QColor("#31363b");
        }
    }
    else
    {
        clear = QColor(QString::fromStdString(m_clear));
    }

    m_widget->setClearColor(clear);
    m_widget->setAttribute(Qt::WA_AlwaysStackOnTop);
    QString theme = QString::fromStdString(m_theme);
    if(theme.isEmpty())
    {
        theme = "light";

        // styleSheet override QPalette
        // we assume that styleSheet is the dark style
        if(!qApp->styleSheet().isEmpty())
        {
            theme = "dark";
        }
    }

#ifdef WIN32
    // To get Qml initialized properly, we need to find its plugins
    // This is difficult to do, especially because the location of the deps is different whether
    // you are executing the application in the build tree or in the install tree
    // Thus the strategy here is to locate the Qt5Core library and then compute the path relatively
    // This work in all cases when we use VCPkg.
    std::filesystem::path qt5LibDir =
        core::tools::os::get_shared_library_path("Qt5Core").remove_filename();
    const std::filesystem::path qt5QmlPluginsDir = (qt5LibDir.parent_path().parent_path()) / "qml";

    QDir pluginDir(QString::fromStdString(qt5QmlPluginsDir.string()));
    if(pluginDir.exists())
    {
        SIGHT_INFO("Load Qml plugins from: " + qt5QmlPluginsDir.string());
        engine->addImportPath(pluginDir.absolutePath());
    }
    else
    {
        SIGHT_ERROR("Could not determine Qml plugins path, tried with: " + qt5QmlPluginsDir.string());
    }
#endif

    QStringList activities_name;

    auto activity_reg = sight::activity::extension::activity::get_default();
    for(std::size_t i = 0 ; i < m_activity_ids.size() ; ++i)
    {
        std::string name = m_activity_names[i];
        if(name.empty())
        {
            const auto info = activity_reg->get_info(m_activity_ids[i]);
            name = info.title;
        }

        activities_name.append(QString::fromStdString(name));
    }

    engine->rootContext()->setContextProperty("activityNameList", activities_name);
    engine->rootContext()->setContextProperty("widgetWidth", m_widget->width());
    engine->rootContext()->setContextProperty(QString::fromStdString(THEME_CONFIG), theme);
    engine->rootContext()->setContextProperty(
        QString::fromStdString(ACCENT_CONFIG),
        QString::fromStdString(m_accent)
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(FOREGROUND_CONFIG),
        QString::fromStdString(m_foreground)
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(BACKGROUND_CONFIG),
        QString::fromStdString(m_background)
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(PRIMARY_CONFIG),
        QString::fromStdString(
            m_primary
        )
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(ELEVATION_CONFIG),
        QString::fromStdString(m_elevation)
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(BUTTON_WIDTH),
        QString::fromStdString(m_button_width)
    );
    engine->rootContext()->setContextProperty(QString::fromStdString(FONT_SIZE), m_font_size);

    m_widget->setSource(QUrl::fromLocalFile(QString::fromStdString(path.string())));

    QObject::connect(m_widget->rootObject(), SIGNAL(activitySelected(int)), this, SLOT(go_to(int)));

    qt_container->set_layout(main_layout);
}

//------------------------------------------------------------------------------

void sequencer::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void sequencer::updating()
{
    {
        auto activity_set = m_activity_set.lock();
        SIGHT_ASSERT("Missing '" << ACTIVITY_SET_INOUT << "' activity_set", activity_set);

        m_current_activity = this->parse_activities(*activity_set);
    }
    if(m_current_activity >= 0)
    {
        for(int i = 0 ; i <= m_current_activity ; ++i)
        {
            this->enable_activity(i);
        }

        // launch the last activity
        this->go_to(m_current_activity);
    }
    else
    {
        // launch the first activity
        this->enable_activity(0);
        this->go_to(0);
    }
}

//------------------------------------------------------------------------------

void sequencer::set_user_warning(bool _state)
{
    // If warning message wasn't configured, force value to false.
    if(m_warning_message.empty())
    {
        SIGHT_ERROR("No warning message is configured in sequencer");
        m_warn_user = false;
    }
    else
    {
        m_warn_user = _state;
    }
}

//------------------------------------------------------------------------------

void sequencer::enable_user_warning()
{
    this->set_user_warning(true);
}

//------------------------------------------------------------------------------
void sequencer::disable_user_warning()
{
    this->set_user_warning(false);
}

//------------------------------------------------------------------------------

void sequencer::go_to(int _index)
{
    if(_index < 0 || _index >= static_cast<int>(m_activity_ids.size()))
    {
        SIGHT_ERROR("no activity to launch at index " << _index)
        return;
    }

    if(m_warn_user && m_current_activity != _index && m_current_activity >= 0)
    {
        auto dialog = sight::ui::dialog::message(
            "Warning",
            m_warning_message + "\nDo you want to continue?",
            sight::ui::dialog::message::warning
        );
        dialog.add_button(sight::ui::dialog::message::yes_no);
        const auto button = dialog.show();

        if((button == sight::ui::dialog::message::no))
        {
            return;
        }
    }

    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    // Clear activities if go backward.
    if(m_clear_activities && m_current_activity > _index)
    {
        auto dialog = sight::ui::dialog::message(
            "Sequencer",
            "The data will be deleted! \nDo you want to continue?",
            sight::ui::dialog::message::warning
        );
        dialog.add_button(sight::ui::dialog::message::yes_no);
        const auto button = dialog.show();

        if((button == sight::ui::dialog::message::no))
        {
            return;
        }

        // Disable all next activities (including current)
        for(int i = _index + 1, end = int(activity_set->size()) ; i < end ; ++i)
        {
            this->disable_activity(i);
        }

        // Remove all last activities.
        this->remove_last_activities(*activity_set, std::size_t(_index));
    }
    // Store data otherwise.
    else if(m_current_activity >= 0)
    {
        this->store_activity_data(*activity_set, std::size_t(m_current_activity));
    }

    const auto new_idx = static_cast<std::size_t>(_index);

    data::activity::sptr activity = this->get_activity(*activity_set, new_idx, slot(service::slots::UPDATE));

    bool ok = true;
    std::string error_msg;

    std::tie(ok, error_msg) = sight::module::ui::qt::activity::sequencer::validate_activity(activity);
    if(ok)
    {
        m_activity_created->async_emit(activity);

        m_current_activity = _index;
        QObject* object = m_widget->rootObject();

        ok = QMetaObject::invokeMethod(object, "setCurrentActivity", Q_ARG(QVariant, _index));
        SIGHT_ASSERT("The slot `setCurrentActivity` was not found.", ok);

        ok = QMetaObject::invokeMethod(object, "enableActivity", Q_ARG(QVariant, _index));
        SIGHT_ASSERT("The slot `enableActivity` was not found.", ok);
    }
    else
    {
        sight::ui::dialog::message::show("Activity not valid", error_msg);
        m_data_required->async_emit(activity);
    }
}

//------------------------------------------------------------------------------

void sequencer::check_next()
{
    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    // Store current activity data before checking the next one,
    // new data can be added in the current activity during the process.
    if(m_current_activity >= 0)
    {
        store_activity_data(*activity_set, std::size_t(m_current_activity));
    }

    // Check if the next activity is valid (creates the activity if needed)
    // NOLINTNEXTLINE(bugprone-misplaced-widening-cast)
    const auto next_index = std::size_t(m_current_activity + 1);
    if(next_index < m_activity_ids.size())
    {
        const auto& next_activity = this->get_activity(*activity_set, next_index, slot(service::slots::UPDATE));
        const auto& [ok, error] = sequencer::validate_activity(next_activity);

        if(ok)
        {
            enable_activity(int(next_index));
        }
        else
        {
            disable_activity(int(next_index));
            SIGHT_DEBUG(error);
        }

        m_next_enabled->async_emit(ok);

        // Refresh next activities validity
        std::size_t last_valid = next_index;

        if(ok)
        {
            // Find the last valid activity
            while(++last_valid < activity_set->size())
            {
                const auto& activity = activity_set->at(last_valid);
                const auto& [next_ok, next_error] = sequencer::validate_activity(activity);

                if(next_ok)
                {
                    enable_activity(int(last_valid));
                }
                else
                {
                    disable_activity(int(last_valid));
                    SIGHT_DEBUG(next_error);

                    break;
                }
            }
        }

        // Disable all next activities
        while(++last_valid < activity_set->size())
        {
            disable_activity(int(last_valid));
        }
    }
}

//------------------------------------------------------------------------------

void sequencer::validate_next()
{
    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    // Store current activity data before checking the next one,
    // new data can be added in the current activity during the process.
    if(m_current_activity >= 0)
    {
        store_activity_data(*activity_set, std::size_t(m_current_activity));
    }

    // Check if the next activity is valid (creates the activity if needed)
    // NOLINTNEXTLINE(bugprone-misplaced-widening-cast)
    const auto next_index = std::size_t(m_current_activity + 1);
    if(next_index < m_activity_ids.size())
    {
        const auto& next_activity = this->get_activity(*activity_set, next_index, slot(service::slots::UPDATE));
        const auto& [ok, error] = sight::module::ui::qt::activity::sequencer::validate_activity(next_activity);

        if(ok)
        {
            m_next_valid->async_emit();
        }
        else
        {
            disable_activity(int(next_index));
            SIGHT_DEBUG(error);

            m_next_invalid->async_emit();
        }

        m_next_validated->async_emit(ok);

        // Refresh next activities validity
        std::size_t last_valid = next_index;

        if(ok)
        {
            // Find the last valid activity
            while(++last_valid < activity_set->size())
            {
                const auto& activity = activity_set->at(last_valid);
                const auto& [next_ok, next_error] = sequencer::validate_activity(activity);

                if(!next_ok)
                {
                    disable_activity(int(last_valid));
                    SIGHT_DEBUG(next_error);

                    break;
                }
            }
        }

        // Disable all next activities
        while(++last_valid < activity_set->size())
        {
            disable_activity(int(last_valid));
        }
    }
}

//------------------------------------------------------------------------------

void sequencer::next()
{
    const auto next_index = m_current_activity + 1;

    go_to(next_index);
}

//------------------------------------------------------------------------------

void sequencer::previous()
{
    this->go_to(m_current_activity - 1);
}

//------------------------------------------------------------------------------

void sequencer::send_info() const
{
    const bool previous_enabled = (m_current_activity > 0);
    m_has_previous->async_emit(previous_enabled);

    const bool next_enabled = (m_current_activity < static_cast<int>(m_activity_ids.size()) - 1);
    m_has_next->async_emit(next_enabled);
}

//------------------------------------------------------------------------------

void sequencer::enable_activity(int _index)
{
    QObject* object = m_widget->rootObject();

    [[maybe_unused]] const bool ok = QMetaObject::invokeMethod(object, "enableActivity", Q_ARG(QVariant, _index));
    SIGHT_ASSERT("The slot `enableActivity` was not found.", ok);
}

//------------------------------------------------------------------------------

void sequencer::disable_activity(int _index)
{
    QObject* object = m_widget->rootObject();

    [[maybe_unused]] const bool ok = QMetaObject::invokeMethod(object, "disableActivity", Q_ARG(QVariant, _index));
    SIGHT_ASSERT("The slot `disableActivity` was not found.", ok);
}

//------------------------------------------------------------------------------

service::connections_t sequencer::auto_connections() const
{
    connections_t connections;
    connections.push(ACTIVITY_SET_INOUT, data::activity_set::ADDED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(ACTIVITY_SET_INOUT, data::activity_set::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
