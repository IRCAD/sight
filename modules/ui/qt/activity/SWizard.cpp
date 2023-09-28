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

#include "SWizard.hpp"

#include <activity/builder/data.hpp>
#include <activity/validator/base.hpp>

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/ActivitySet.hpp>
#include <data/Composite.hpp>
#include <data/Series.hpp>

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

const core::com::slots::key_t SWizard::CREATE_ACTIVITY_SLOT   = "createActivity";
const core::com::slots::key_t SWizard::UPDATE_ACTIVITY_SLOT   = "updateActivity";
const core::com::signals::key_t SWizard::ACTIVITY_CREATED_SIG = "activityCreated";
const core::com::signals::key_t SWizard::ACTIVITY_UPDATED_SIG = "activityUpdated";
const core::com::signals::key_t SWizard::CANCELED_SIG         = "canceled";

using sight::activity::extension::ActivityInfo;
using sight::activity::extension::Activity;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

SWizard::SWizard() noexcept
{
    new_slot(CREATE_ACTIVITY_SLOT, &SWizard::createActivity, this);
    new_slot(UPDATE_ACTIVITY_SLOT, &SWizard::updateActivity, this);

    m_sigActivityCreated = new_signal<ActivityCreatedSignalType>(ACTIVITY_CREATED_SIG);
    m_sigActivityUpdated = new_signal<ActivityUpdatedSignalType>(ACTIVITY_UPDATED_SIG);
    m_sigCanceled        = new_signal<CanceledSignalType>(CANCELED_SIG);
}

//------------------------------------------------------------------------------

SWizard::~SWizard() noexcept =
    default;

//------------------------------------------------------------------------------

void SWizard::configuring()
{
    sight::ui::service::initialize();

    const auto config = this->getConfiguration();

    m_ioSelectorConfig = config.get("ioSelectorConfig", "");
    SIGHT_ASSERT("ioSelector Configuration must not be empty", !m_ioSelectorConfig.empty());

    m_sdbIoSelectorConfig = config.get("sdbIoSelectorConfig", "");
    if(m_sdbIoSelectorConfig.empty())
    {
        m_sdbIoSelectorConfig = m_ioSelectorConfig;
    }

    m_confirmUpdate = config.get("confirm", m_confirmUpdate);
    m_isCancelable  = config.get("cancel", m_isCancelable);

    const auto iconsCfg = config.get_child("icons");
    const auto iconCfg  = iconsCfg.equal_range("icon");
    for(auto itIcon = iconCfg.first ; itIcon != iconCfg.second ; ++itIcon)
    {
        const auto anotherIconCfg = itIcon->second.get_child("<xmlattr>");

        const auto type = anotherIconCfg.get<std::string>("type");
        SIGHT_ASSERT("'type' attribute must not be empty", !type.empty());
        const auto icon = anotherIconCfg.get<std::string>("icon");
        SIGHT_ASSERT("'icon' attribute must not be empty", !icon.empty());

        const auto file = core::runtime::get_resource_file_path(icon);
        m_objectIcons[type] = file.string();
    }

    SIGHT_ASSERT("icons are empty", !m_objectIcons.empty());
}

//------------------------------------------------------------------------------

void SWizard::starting()
{
    sight::ui::service::create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    QWidget* const container = qtContainer->getQtContainer();

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

    m_DataView = new DataView();
    m_DataView->setIOSelectorConfig(m_ioSelectorConfig);
    m_DataView->setSDBIOSelectorConfig(m_sdbIoSelectorConfig);

    m_DataView->setObjectIconAssociation(m_objectIcons);

    layout->addWidget(m_DataView, 1);

    auto* buttonLayout = new QHBoxLayout();
    layout->addLayout(buttonLayout);

    if(m_isCancelable)
    {
        m_cancelButton = new QPushButton("Cancel");
        m_cancelButton->setToolTip("Cancel the activity creation");
        buttonLayout->addWidget(m_cancelButton);
    }

    m_resetButton = new QPushButton("Clear");
    m_resetButton->setToolTip("Clear the current selected data");
    buttonLayout->addWidget(m_resetButton);

    m_okButton = new QPushButton("Apply");
    m_okButton->setToolTip("Create or update the activity with the selected data");
    buttonLayout->addWidget(m_okButton);

    container->setLayout(layout);

    QObject::connect(
        m_DataView.data(),
        &DataView::currentChanged,
        this,
        &SWizard::onTabChanged
    );
    QObject::connect(m_okButton.data(), &QPushButton::clicked, this, &SWizard::onBuildActivity);
    QObject::connect(m_resetButton.data(), &QPushButton::clicked, this, &SWizard::onReset);
    if(m_isCancelable)
    {
        QObject::connect(m_cancelButton.data(), &QPushButton::clicked, this, &SWizard::onCancel);
    }
}

//------------------------------------------------------------------------------

void SWizard::stopping()
{
    m_DataView->clear();

    QObject::disconnect(
        m_DataView.data(),
        &DataView::currentChanged,
        this,
        &SWizard::onTabChanged
    );
    QObject::disconnect(m_okButton.data(), &QPushButton::clicked, this, &SWizard::onBuildActivity);
    QObject::disconnect(m_resetButton.data(), &QPushButton::clicked, this, &SWizard::onReset);
    if(m_isCancelable)
    {
        QObject::disconnect(m_cancelButton.data(), &QPushButton::clicked, this, &SWizard::onCancel);
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void SWizard::updating()
{
    auto as = m_activity.lock();
    if(as)
    {
        this->updateActivity(as.get_shared());
    }
    else
    {
        SIGHT_DEBUG("activity is not defined, it cannot be updated");
    }
}

//------------------------------------------------------------------------------

void SWizard::createActivity(std::string activityID)
{
    m_mode = Mode::CREATE;
    ActivityInfo info;
    info = Activity::getDefault()->getInfo(activityID);

    // load activity module
    core::runtime::start_module(info.bundleId);

    m_new_activity = std::make_shared<data::Activity>();
    m_new_activity->setActivityConfigId(info.id);

    m_title->setText(QString("<h1>%1</h1>").arg(QString::fromStdString(info.title)));
    m_description->setText(QString::fromStdString(info.description));

    bool needConfig = false;

    // If we have requirements but they are not needed to start (maxOccurs = 0), we can skip the config as well
    for(const auto& req : info.requirements)
    {
        if(req.maxOccurs > 0)
        {
            needConfig = true;
            break;
        }
    }

    if(needConfig)
    {
        m_DataView->fillInformation(info);
        if(m_DataView->count() > 1)
        {
            m_okButton->setText("Next");
        }

        this->slot(slots::SHOW)->async_run();
    }
    else
    {
        // Create data automatically if they are not provided by the user
        for(const auto& req : info.requirements)
        {
            SIGHT_ASSERT("minOccurs and maxOccurs should be 0", req.minOccurs == 0 && req.maxOccurs == 0);
            (*m_new_activity)[req.name] = sight::activity::detail::data::create(req.type, req.objectConfig);
        }

        const auto activity_set = m_activity_set.lock();
        SIGHT_ASSERT("The inout key '" << s_ACTIVITY_SET << "' is not defined.", activity_set);

        const auto scoped_emitter = activity_set->scoped_emit();
        activity_set->push_back(m_new_activity);

        m_sigActivityCreated->async_emit(m_new_activity);
    }
}

//------------------------------------------------------------------------------

void SWizard::updateActivity(data::Activity::sptr activity)
{
    ActivityInfo info;
    info = Activity::getDefault()->getInfo(activity->getActivityConfigId());

    // load activity module
    core::runtime::start_module(info.bundleId);

    m_title->setText(QString("<h1>%1</h1>").arg(QString::fromStdString(info.title)));
    m_description->setText(QString::fromStdString(info.description));

    m_mode         = Mode::UPDATE;
    m_new_activity = activity;

    bool needConfig = false;

    // If we have requirements but they are not needed to start (maxOccurs = 0), we can skip the config as well
    for(const auto& req : info.requirements)
    {
        if(req.maxOccurs != 0)
        {
            needConfig = true;
            break;
        }
    }

    if(needConfig)
    {
        m_DataView->fillInformation(m_new_activity);
        if(m_DataView->count() > 1)
        {
            m_okButton->setText("Next");
        }
    }
    else
    {
        // Start immediately without popping any configuration UI
        data::Object::ModifiedSignalType::sptr sig;
        sig = m_new_activity->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
        sig->async_emit();
        m_sigActivityUpdated->async_emit(m_new_activity);
    }
}

//------------------------------------------------------------------------------

void SWizard::onTabChanged(int index)
{
    if(index == m_DataView->count() - 1)
    {
        m_okButton->setText("Apply");
    }
    else
    {
        m_okButton->setText("Next");
    }
}

//------------------------------------------------------------------------------

void SWizard::onReset()
{
    if(m_new_activity)
    {
        ActivityInfo info;
        info = Activity::getDefault()->getInfo(m_new_activity->getActivityConfigId());
        m_DataView->fillInformation(info);

        if(m_DataView->count() > 1)
        {
            m_okButton->setText("Next");
        }
    }
}

//------------------------------------------------------------------------------

void SWizard::onCancel()
{
    m_DataView->clear();
    m_sigCanceled->async_emit();
}

//------------------------------------------------------------------------------

void SWizard::onBuildActivity()
{
    int index   = m_DataView->currentIndex();
    int lastTab = m_DataView->count() - 1;

    if(index < 0)
    {
        return;
    }

    std::string errorMsg;
    // Check current data
    if(m_DataView->checkData(std::size_t(index), errorMsg))
    {
        if(index != lastTab)
        {
            // enable and select the next tab
            m_DataView->setTabEnabled(index + 1, true);
            m_DataView->setCurrentIndex(index + 1);
        }
        else // index == lastTab
        {
            // Create/update activity
            if(m_mode == Mode::UPDATE && m_confirmUpdate)
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
                    m_new_activity = data::Object::copy(m_new_activity);
                    m_mode         = Mode::CREATE; // The new activity should be added in the activity_set
                }
            }

            // check all data and create/update the activity
            bool ok = m_DataView->checkAndComputeData(m_new_activity, errorMsg);
            if(ok)
            {
                if(m_mode == Mode::CREATE)
                {
                    // Add the new activity in activity_set
                    ActivityInfo info = Activity::getDefault()->getInfo(m_new_activity->getActivityConfigId());

                    const auto& [description, input_ok] = sight::ui::dialog::input::showInputDialog(
                        "Activity creation",
                        "Please, give a description of the activity.",
                        info.title
                    );

                    if(!input_ok && description.empty())
                    {
                        return;
                    }

                    m_new_activity->setDescription(description);
                    const auto activity_set = m_activity_set.lock();
                    SIGHT_ASSERT("The inout key '" << s_ACTIVITY_SET << "' is not defined.", activity_set);

                    const auto scoped_emitter = activity_set->scoped_emit();
                    activity_set->push_back(m_new_activity);

                    m_sigActivityCreated->async_emit(m_new_activity);
                }
                else // m_mode == Mode::UPDATE
                {
                    data::Object::ModifiedSignalType::sptr sig;
                    sig = m_new_activity->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
                    sig->async_emit();
                    m_sigActivityUpdated->async_emit(m_new_activity);
                }
            }
            else
            {
                QString message = "This activity can not be created : \n";
                message.append(QString::fromStdString(errorMsg));
                QMessageBox::warning(qApp->activeWindow(), "Activity Creation", message);
                SIGHT_ERROR(errorMsg);
            }
        }
    }
    else
    {
        QMessageBox::warning(qApp->activeWindow(), "Error", QString::fromStdString(errorMsg));
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
