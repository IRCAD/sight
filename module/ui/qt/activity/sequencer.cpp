/************************************************************************
 *
 * Copyright (C) 2016-2025 IRCAD France
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
#include <QFrame>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

// cspell:ignore Roboto

namespace sight::module::ui::qt::activity
{

//------------------------------------------------------------------------------

static const std::string CLEAR_ACTIVITIES_CONFIG = "clearActivities";
static const std::string BUTTON_WIDTH            = "buttonWidth";
static const std::string FONT_SIZE               = "fontSize";
static const std::string WARNING_MESSAGE         = "warning_message";

class number_icon final : public QIcon
{
public:

    explicit number_icon(int _number, const QColor& _background = Qt::white)
    {
        QPixmap pixmap(256, 256);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);

        // Draw the white circle
        painter.setBrush(_background);
        painter.drawEllipse(0, 0, 255, 255);

        // Draw the number
        painter.setPen(Qt::transparent);
        painter.setBrush(Qt::transparent);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setBackgroundMode(Qt::TransparentMode);
        painter.setFont(QFont("Roboto", 100, QFont::Bold));
        painter.drawText(pixmap.rect(), Qt::AlignCenter, QString::number(_number));

        painter.end();
        this->addPixmap(pixmap, QIcon::Normal);
    }
};

class sequencer_button final : public QPushButton
{
public:

    explicit sequencer_button(const QString& _text, QWidget* _parent = nullptr) :
        QPushButton(" " + _text, _parent)
    {
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QSize minimumSizeHint() const final
    {
        // This allows well painted rounded button
        return QPushButton::minimumSizeHint().grownBy(QMargins(3, 0, 3, 0));
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QSize sizeHint() const final
    {
        // This allows well painted rounded button
        return QPushButton::sizeHint().grownBy(QMargins(3, 0, 3, 0));
    }
};

//------------------------------------------------------------------------------

inline static void set_button_enabled(
    QButtonGroup* _button_group,
    int _index,
    bool _enabled,
    std::optional<bool> _checked = std::nullopt
)
{
    if(auto* const button = _button_group->button(_index); button != nullptr)
    {
        const auto& sibling = button->parentWidget()->findChildren<QWidget*>();
        if(const auto spacer_index = sibling.indexOf(button) - 1; spacer_index >= 0)
        {
            sibling[spacer_index]->setEnabled(_enabled);
        }

        button->setEnabled(_enabled);

        if(_checked.has_value())
        {
            button->setChecked(*_checked);
        }
    }
}

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

    m_button_width    = config.get<std::string>(BUTTON_WIDTH, m_button_width);
    m_font_size       = config.get<double>(FONT_SIZE, m_font_size);
    m_warning_message = config.get<std::string>(WARNING_MESSAGE, m_warning_message);

    m_warn_user = !m_warning_message.empty();
}

//------------------------------------------------------------------------------

void sequencer::starting()
{
    this->sight::ui::service::create();

    const auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    m_widget = new QWidget();
    m_widget->setObjectName(base_id() + "/sequencer");
    m_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_widget->setProperty("class", "sequencer");

    auto* const widget_layout = new QHBoxLayout();
    widget_layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    // Build buttons
    m_button_group = new QButtonGroup(m_widget);
    m_button_group->setExclusive(true);

    auto activity_reg = sight::activity::extension::activity::get_default();
    for(int i = 0, last = int(m_activity_ids.size()) - 1 ; i <= last ; ++i)
    {
        const auto button_label = QString::fromStdString(
            m_activity_names[std::size_t(i)].empty()
            ? activity_reg->get_info(m_activity_ids[std::size_t(i)]).title
            : m_activity_names[std::size_t(i)]
        );

        auto* const button = new sequencer_button(button_label, m_widget);
        button->setObjectName(m_widget->objectName() + "/" + button_label + "_button");
        button->setProperty("class", "sequencer_button");
        button->setCheckable(true);
        button->setEnabled(false);
        button->setIcon(number_icon(i + 1));
        button->setIconSize(QSize(32, 32));
        widget_layout->addWidget(button);
        m_button_group->addButton(button, i);

        if(i != last)
        {
            auto* const sequencer_spacer = new QFrame(m_widget);
            sequencer_spacer->setObjectName(m_widget->objectName() + "/" + button_label + "_spacer");
            sequencer_spacer->setProperty("class", "sequencer_spacer");
            sequencer_spacer->setFrameShape(QFrame::Shape::HLine);
            sequencer_spacer->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
            sequencer_spacer->setEnabled(false);
            widget_layout->addWidget(sequencer_spacer);
        }
    }

    widget_layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    m_widget->setLayout(widget_layout);

    QObject::connect(m_button_group, &QButtonGroup::idClicked, this, &sequencer::go_to);

    // Add the sequencer to the container
    auto* main_layout = qt_container->get_qt_container()->layout();

    if(main_layout == nullptr)
    {
        main_layout = new QVBoxLayout();
        main_layout->setContentsMargins(0, 0, 0, 0);
        qt_container->set_layout(main_layout);
    }

    main_layout->addWidget(m_widget);
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

        set_button_enabled(m_button_group, _index, true, true);
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
    set_button_enabled(m_button_group, _index, true);
}

//------------------------------------------------------------------------------

void sequencer::disable_activity(int _index)
{
    set_button_enabled(m_button_group, _index, false);
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
