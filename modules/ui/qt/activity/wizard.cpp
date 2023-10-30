/************************************************************************
 *
 * Copyright (C) 2016-2023 IRCAD France
 * Copyright (C) 2016-2020 IHU Strasbourg
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

#include "wizard.hpp"

#include <activity/builder/data.hpp>
#include <activity/validator/base.hpp>

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/activity_set.hpp>
#include <data/composite.hpp>
#include <data/series.hpp>

#include <ui/__/dialog/input.hpp>
#include <ui/qt/container/widget.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QVBoxLayout>

namespace sight::module::ui::qt::activity
{

//------------------------------------------------------------------------------

const core::com::slots::key_t wizard::CREATE_ACTIVITY_SLOT   = "createActivity";
const core::com::slots::key_t wizard::UPDATE_ACTIVITY_SLOT   = "updateActivity";
const core::com::signals::key_t wizard::ACTIVITY_CREATED_SIG = "activityCreated";
const core::com::signals::key_t wizard::ACTIVITY_UPDATED_SIG = "activityUpdated";
const core::com::signals::key_t wizard::CANCELED_SIG         = "canceled";

using sight::activity::extension::activity_info;
using sight::activity::extension::activity;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

wizard::wizard() noexcept
{
    new_slot(CREATE_ACTIVITY_SLOT, &wizard::create_activity, this);
    new_slot(UPDATE_ACTIVITY_SLOT, &wizard::update_activity, this);

    m_sig_activity_created = new_signal<activity_created_signal_t>(ACTIVITY_CREATED_SIG);
    m_sig_activity_updated = new_signal<activity_updated_signal_t>(ACTIVITY_UPDATED_SIG);
    m_sig_canceled         = new_signal<canceled_signal_t>(CANCELED_SIG);
}

//------------------------------------------------------------------------------

wizard::~wizard() noexcept =
    default;

//------------------------------------------------------------------------------

void wizard::configuring()
{
    sight::ui::service::initialize();

    const auto config = this->get_config();

    m_io_selector_config = config.get("ioSelectorConfig", "");
    SIGHT_ASSERT("ioSelector Configuration must not be empty", !m_io_selector_config.empty());

    m_sdb_io_selector_config = config.get("sdbIoSelectorConfig", "");
    if(m_sdb_io_selector_config.empty())
    {
        m_sdb_io_selector_config = m_io_selector_config;
    }

    m_confirm_update = config.get("confirm", m_confirm_update);
    m_is_cancelable  = config.get("cancel", m_is_cancelable);

    const auto icons_cfg = config.get_child("icons");
    const auto icon_cfg  = icons_cfg.equal_range("icon");
    for(auto it_icon = icon_cfg.first ; it_icon != icon_cfg.second ; ++it_icon)
    {
        const auto another_icon_cfg = it_icon->second.get_child("<xmlattr>");

        const auto type = another_icon_cfg.get<std::string>("type");
        SIGHT_ASSERT("'type' attribute must not be empty", !type.empty());
        const auto icon = another_icon_cfg.get<std::string>("icon");
        SIGHT_ASSERT("'icon' attribute must not be empty", !icon.empty());

        const auto file = core::runtime::get_resource_file_path(icon);
        m_object_icons[type] = file.string();
    }

    SIGHT_ASSERT("icons are empty", !m_object_icons.empty());
}

//------------------------------------------------------------------------------

void wizard::starting()
{
    sight::ui::service::create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    QWidget* const container = qt_container->get_qt_container();

    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    m_title = new QLabel("");
    m_title->setObjectName("SActivityWizard_title");
    m_title->setAlignment(Qt::AlignHCenter);
    layout->addWidget(m_title);

    m_description = new QLabel("");
    m_description->setObjectName("SActivityWizard_description");
    m_description->setAlignment(Qt::AlignHCenter);
    layout->addWidget(m_description);

    // If the style sheet is empty, we are using the default theme.
    // If a style sheet is set, the style must be set in the style sheet.
    if(qApp->styleSheet().isEmpty())
    {
        m_title->setStyleSheet("QLabel { font: bold; color: blue; }");
        m_description->setStyleSheet("QLabel { font: italic; border: solid 1px;}");
    }

    m_data_view = new data_view();
    m_data_view->set_io_selector_config(m_io_selector_config);
    m_data_view->set_sdbio_selector_config(m_sdb_io_selector_config);

    m_data_view->set_object_icon_association(m_object_icons);

    layout->addWidget(m_data_view, 1);

    auto* button_layout = new QHBoxLayout();
    layout->addLayout(button_layout);

    if(m_is_cancelable)
    {
        m_cancel_button = new QPushButton("Cancel");
        m_cancel_button->setToolTip("Cancel the activity creation");
        button_layout->addWidget(m_cancel_button);
    }

    m_reset_button = new QPushButton("Clear");
    m_reset_button->setToolTip("Clear the current selected data");
    button_layout->addWidget(m_reset_button);

    m_ok_button = new QPushButton("Apply");
    m_ok_button->setToolTip("Create or update the activity with the selected data");
    button_layout->addWidget(m_ok_button);

    container->setLayout(layout);

    QObject::connect(
        m_data_view.data(),
        &data_view::currentChanged,
        this,
        &wizard::on_tab_changed
    );
    QObject::connect(m_ok_button.data(), &QPushButton::clicked, this, &wizard::on_build_activity);
    QObject::connect(m_reset_button.data(), &QPushButton::clicked, this, &wizard::on_reset);
    if(m_is_cancelable)
    {
        QObject::connect(m_cancel_button.data(), &QPushButton::clicked, this, &wizard::on_cancel);
    }
}

//------------------------------------------------------------------------------

void wizard::stopping()
{
    m_data_view->clear();

    QObject::disconnect(
        m_data_view.data(),
        &data_view::currentChanged,
        this,
        &wizard::on_tab_changed
    );
    QObject::disconnect(m_ok_button.data(), &QPushButton::clicked, this, &wizard::on_build_activity);
    QObject::disconnect(m_reset_button.data(), &QPushButton::clicked, this, &wizard::on_reset);
    if(m_is_cancelable)
    {
        QObject::disconnect(m_cancel_button.data(), &QPushButton::clicked, this, &wizard::on_cancel);
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void wizard::updating()
{
    auto as = m_activity.lock();
    if(as)
    {
        this->update_activity(as.get_shared());
    }
    else
    {
        SIGHT_DEBUG("activity is not defined, it cannot be updated");
    }
}

//------------------------------------------------------------------------------

void wizard::create_activity(std::string _activity_id)
{
    m_mode = mode::create;
    activity_info info;
    info = activity::get_default()->get_info(_activity_id);

    // load activity module
    core::runtime::start_module(info.bundle_id);

    m_new_activity = std::make_shared<data::activity>();
    m_new_activity->set_activity_config_id(info.id);

    m_title->setText(QString("<h1>%1</h1>").arg(QString::fromStdString(info.title)));
    m_description->setText(QString::fromStdString(info.description));

    bool need_config = false;

    // If we have requirements but they are not needed to start (maxOccurs = 0), we can skip the config as well
    for(const auto& req : info.requirements)
    {
        if(req.max_occurs > 0)
        {
            need_config = true;
            break;
        }
    }

    if(need_config)
    {
        m_data_view->fill_information(info);
        if(m_data_view->count() > 1)
        {
            m_ok_button->setText("Next");
        }

        this->slot(slots::SHOW)->async_run();
    }
    else
    {
        // Create data automatically if they are not provided by the user
        for(const auto& req : info.requirements)
        {
            SIGHT_ASSERT("minOccurs and maxOccurs should be 0", req.min_occurs == 0 && req.max_occurs == 0);
            (*m_new_activity)[req.name] = sight::activity::detail::data::create(req.type, req.object_config);
        }

        const auto activity_set = m_activity_set.lock();
        SIGHT_ASSERT("The inout key '" << ACTIVITY_SET << "' is not defined.", activity_set);

        const auto scoped_emitter = activity_set->scoped_emit();
        activity_set->push_back(m_new_activity);

        m_sig_activity_created->async_emit(m_new_activity);
    }
}

//------------------------------------------------------------------------------

void wizard::update_activity(data::activity::sptr _activity)
{
    activity_info info;
    info = activity::get_default()->get_info(_activity->get_activity_config_id());

    // load activity module
    core::runtime::start_module(info.bundle_id);

    m_title->setText(QString("<h1>%1</h1>").arg(QString::fromStdString(info.title)));
    m_description->setText(QString::fromStdString(info.description));

    m_mode         = mode::update;
    m_new_activity = _activity;

    bool need_config = false;

    // If we have requirements but they are not needed to start (maxOccurs = 0), we can skip the config as well
    for(const auto& req : info.requirements)
    {
        if(req.max_occurs != 0)
        {
            need_config = true;
            break;
        }
    }

    if(need_config)
    {
        m_data_view->fill_information(m_new_activity);
        if(m_data_view->count() > 1)
        {
            m_ok_button->setText("Next");
        }
    }
    else
    {
        // Start immediately without popping any configuration UI
        data::object::modified_signal_t::sptr sig;
        sig = m_new_activity->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
        sig->async_emit();
        m_sig_activity_updated->async_emit(m_new_activity);
    }
}

//------------------------------------------------------------------------------

void wizard::on_tab_changed(int _index)
{
    if(_index == m_data_view->count() - 1)
    {
        m_ok_button->setText("Apply");
    }
    else
    {
        m_ok_button->setText("Next");
    }
}

//------------------------------------------------------------------------------

void wizard::on_reset()
{
    if(m_new_activity)
    {
        activity_info info;
        info = activity::get_default()->get_info(m_new_activity->get_activity_config_id());
        m_data_view->fill_information(info);

        if(m_data_view->count() > 1)
        {
            m_ok_button->setText("Next");
        }
    }
}

//------------------------------------------------------------------------------

void wizard::on_cancel()
{
    m_data_view->clear();
    m_sig_canceled->async_emit();
}

//------------------------------------------------------------------------------

void wizard::on_build_activity()
{
    int index    = m_data_view->currentIndex();
    int last_tab = m_data_view->count() - 1;

    if(index < 0)
    {
        return;
    }

    std::string error_msg;
    // Check current data
    if(m_data_view->check_data(std::size_t(index), error_msg))
    {
        if(index != last_tab)
        {
            // enable and select the next tab
            m_data_view->setTabEnabled(index + 1, true);
            m_data_view->setCurrentIndex(index + 1);
        }
        else // index == lastTab
        {
            // Create/update activity
            if(m_mode == mode::update && m_confirm_update)
            {
                QMessageBox::StandardButton button = QMessageBox::question(
                    qApp->activeWindow(),
                    "Update activity",
                    "You will override your activity. You could loose some data.\n"
                    "Would you duplicate your activity ?",
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                    QMessageBox::No
                );

                if(button == QMessageBox::Cancel)
                {
                    return;
                }

                if(button == QMessageBox::Yes)
                {
                    m_new_activity = data::object::copy(m_new_activity);
                    m_mode         = mode::create; // The new activity should be added in the activity_set
                }
            }

            // check all data and create/update the activity
            bool ok = m_data_view->check_and_compute_data(m_new_activity, error_msg);
            if(ok)
            {
                if(m_mode == mode::create)
                {
                    // Add the new activity in activity_set
                    activity_info info = activity::get_default()->get_info(m_new_activity->get_activity_config_id());

                    const auto& [description, input_ok] = sight::ui::dialog::input::show_input_dialog(
                        "Activity creation",
                        "Please, give a description of the activity.",
                        info.title
                    );

                    if(!input_ok && description.empty())
                    {
                        return;
                    }

                    m_new_activity->set_description(description);
                    const auto activity_set = m_activity_set.lock();
                    SIGHT_ASSERT("The inout key '" << ACTIVITY_SET << "' is not defined.", activity_set);

                    const auto scoped_emitter = activity_set->scoped_emit();
                    activity_set->push_back(m_new_activity);

                    m_sig_activity_created->async_emit(m_new_activity);
                }
                else // m_mode == Mode::UPDATE
                {
                    data::object::modified_signal_t::sptr sig;
                    sig = m_new_activity->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
                    sig->async_emit();
                    m_sig_activity_updated->async_emit(m_new_activity);
                }
            }
            else
            {
                QString message = "This activity can not be created : \n";
                message.append(QString::fromStdString(error_msg));
                QMessageBox::warning(qApp->activeWindow(), "Activity Creation", message);
                SIGHT_ERROR(error_msg);
            }
        }
    }
    else
    {
        QMessageBox::warning(qApp->activeWindow(), "Error", QString::fromStdString(error_msg));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
