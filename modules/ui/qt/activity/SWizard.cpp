/************************************************************************
 *
 * Copyright (C) 2016-2021 IRCAD France
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

#include <activity/IValidator.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/runtime/ConfigurationElement.hpp>
#include <core/runtime/operations.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/Equipment.hpp>
#include <data/helper/SeriesDB.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>
#include <data/Study.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/InputDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QVBoxLayout>

namespace sight::module::ui::qt
{

namespace activity
{

//------------------------------------------------------------------------------

const core::com::Slots::SlotKeyType SWizard::s_CREATE_ACTIVITY_SLOT        = "createActivity";
const core::com::Slots::SlotKeyType SWizard::s_UPDATE_ACTIVITY_SLOT        = "updateActivity";
const core::com::Slots::SlotKeyType SWizard::s_UPDATE_ACTIVITY_SERIES_SLOT = "updateActivitySeries";
const core::com::Signals::SignalKeyType SWizard::s_ACTIVITY_CREATED_SIG    = "activityCreated";
const core::com::Signals::SignalKeyType SWizard::s_ACTIVITY_UPDATED_SIG    = "activityUpdated";
const core::com::Signals::SignalKeyType SWizard::s_CANCELED_SIG            = "canceled";

static const service::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

using sight::activity::extension::ActivityInfo;
using sight::activity::extension::Activity;

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

SWizard::SWizard() noexcept :
    m_mode(Mode::CREATE),
    m_confirmUpdate(true),
    m_isCancelable(true)
{
    newSlot(s_CREATE_ACTIVITY_SLOT, &SWizard::createActivity, this);
    newSlot(s_UPDATE_ACTIVITY_SLOT, &SWizard::updateActivity, this);
    newSlot(s_UPDATE_ACTIVITY_SERIES_SLOT, &SWizard::updateActivitySeries, this);

    m_sigActivityCreated = newSignal<ActivityCreatedSignalType>(s_ACTIVITY_CREATED_SIG);
    m_sigActivityUpdated = newSignal<ActivityUpdatedSignalType>(s_ACTIVITY_UPDATED_SIG);
    m_sigCanceled        = newSignal<CanceledSignalType>(s_CANCELED_SIG);
}

//------------------------------------------------------------------------------

SWizard::~SWizard() noexcept
{
}

//------------------------------------------------------------------------------

void SWizard::configuring()
{
    sight::ui::base::IGuiContainer::initialize();

    const auto config = this->getConfigTree();

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
        const auto iconCfg = itIcon->second.get_child("<xmlattr>");

        const std::string type = iconCfg.get<std::string>("type");
        SIGHT_ASSERT("'type' attribute must not be empty", !type.empty());
        const std::string icon = iconCfg.get<std::string>("icon");
        SIGHT_ASSERT("'icon' attribute must not be empty", !icon.empty());

        const auto file = core::runtime::getResourceFilePath(icon);
        m_objectIcons[type] = file.string();
    }

    SIGHT_ASSERT("icons are empty", !m_objectIcons.empty());
}

//------------------------------------------------------------------------------

void SWizard::starting()
{
    sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();

    QVBoxLayout* layout = new QVBoxLayout();
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

    QHBoxLayout* buttonLayout = new QHBoxLayout();
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
    auto as = this->getInOut<data::ActivitySeries>("activitySeries");
    if(as)
    {
        this->updateActivity(as);
    }

    SIGHT_DEBUG_IF("activity series is not defined, it cannot be updated", !as);
}

//------------------------------------------------------------------------------

void SWizard::createActivity(std::string activityID)
{
    m_mode = Mode::CREATE;
    ActivityInfo info;
    info = Activity::getDefault()->getInfo(activityID);

    // load activity module
    std::shared_ptr<core::runtime::Module> module = core::runtime::findModule(info.bundleId);
    if(!module->isStarted())
    {
        module->start();
    }

    m_actSeries = data::ActivitySeries::New();

    m_actSeries->setModality("OT");
    m_actSeries->setInstanceUID("fwActivities." + core::tools::UUID::generateUUID());

    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    m_actSeries->setDate(core::tools::getDate(now));
    m_actSeries->setTime(core::tools::getTime(now));

    m_actSeries->setActivityConfigId(info.id);

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

        this->slot(s_SHOW_SLOT)->asyncRun();
    }
    else
    {
        // Create data automatically if they are not provided by the user
        for(const auto& req : info.requirements)
        {
            SIGHT_ASSERT("minOccurs and maxOccurs should be 0", req.minOccurs == 0 && req.maxOccurs == 0);
            data::Composite::sptr data = m_actSeries->getData();
            (*data)[req.name] = data::factory::New(req.type);
        }

        data::SeriesDB::sptr seriesDB = this->getInOut<data::SeriesDB>(s_SERIESDB_INOUT);
        SIGHT_ASSERT("The inout key '" + s_SERIESDB_INOUT + "' is not defined.", seriesDB);

        data::helper::SeriesDB helper(*seriesDB);
        helper.add(m_actSeries);
        helper.notify();
        m_sigActivityCreated->asyncEmit(m_actSeries);
    }
}

//------------------------------------------------------------------------------

void SWizard::updateActivity(data::ActivitySeries::sptr activitySeries)
{
    ActivityInfo info;
    info = Activity::getDefault()->getInfo(activitySeries->getActivityConfigId());

    // load activity module
    std::shared_ptr<core::runtime::Module> module = core::runtime::findModule(info.bundleId);
    if(!module->isStarted())
    {
        module->start();
    }

    m_title->setText(QString("<h1>%1</h1>").arg(QString::fromStdString(info.title)));
    m_description->setText(QString::fromStdString(info.description));

    m_mode      = Mode::UPDATE;
    m_actSeries = activitySeries;

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
        m_DataView->fillInformation(m_actSeries);
        if(m_DataView->count() > 1)
        {
            m_okButton->setText("Next");
        }
    }
    else
    {
        // Start immediately without popping any configuration UI
        data::Object::ModifiedSignalType::sptr sig;
        sig = m_actSeries->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
        m_sigActivityUpdated->asyncEmit(m_actSeries);
    }
}

//------------------------------------------------------------------------------

void SWizard::updateActivitySeries(data::Series::sptr series)
{
    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::dynamicCast(series);
    if(activitySeries)
    {
        this->updateActivity(activitySeries);
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
    if(m_actSeries)
    {
        ActivityInfo info;
        info = Activity::getDefault()->getInfo(m_actSeries->getActivityConfigId());
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
    m_sigCanceled->asyncEmit();
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
    if(m_DataView->checkData(size_t(index), errorMsg))
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
                else if(button == QMessageBox::Yes)
                {
                    m_actSeries = data::Object::copy(m_actSeries);
                    m_mode      = Mode::CREATE; // The new activity should be added in the seriesDB
                }
            }

            // check all data and create/update the activity
            bool ok = m_DataView->checkAndComputeData(m_actSeries, errorMsg);
            if(ok)
            {
                data::Composite::sptr data = m_actSeries->getData();

                // Copy the patient/study information of a series
                data::Series::sptr series;
                for(const auto& elt : (*data))
                {
                    series = data::Series::dynamicCast(elt.second);
                    if(series)
                    {
                        m_actSeries->setPatient(data::Object::copy(series->getPatient()));
                        m_actSeries->setStudy(data::Object::copy(series->getStudy()));
                        m_actSeries->setEquipment(data::Object::copy(series->getEquipment()));
                        break;
                    }
                }

                if(m_mode == Mode::CREATE)
                {
                    // Add the new activity series in seriesDB
                    ActivityInfo info;
                    info = Activity::getDefault()->getInfo(
                        m_actSeries->getActivityConfigId()
                    );

                    std::string description = sight::ui::base::dialog::InputDialog::showInputDialog(
                        "Activity creation",
                        "Please, give a description of the activity.",
                        info.title
                    );
                    if(description.empty())
                    {
                        return;
                    }

                    m_actSeries->setDescription(description);
                    data::SeriesDB::sptr seriesDB = this->getInOut<data::SeriesDB>(s_SERIESDB_INOUT);
                    SIGHT_ASSERT("The inout key '" + s_SERIESDB_INOUT + "' is not defined.", seriesDB);

                    data::helper::SeriesDB helper(*seriesDB);
                    helper.add(m_actSeries);
                    helper.notify();
                    m_sigActivityCreated->asyncEmit(m_actSeries);
                }
                else // m_mode == Mode::UPDATE
                {
                    data::Object::ModifiedSignalType::sptr sig;
                    sig = m_actSeries->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
                    sig->asyncEmit();
                    m_sigActivityUpdated->asyncEmit(m_actSeries);
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

} //namespace activity

} //namespace sight::module::ui::qt
