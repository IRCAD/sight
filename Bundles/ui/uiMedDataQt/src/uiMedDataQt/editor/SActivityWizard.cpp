/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/editor/SActivityWizard.hpp"

#include <fwActivities/IValidator.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>

#include <fwGui/dialog/InputDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QVBoxLayout>

namespace uiMedDataQt
{
namespace editor
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::editor::IEditor, ::uiMedDataQt::editor::SActivityWizard, ::fwMedData::SeriesDB );

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SActivityWizard::s_CREATE_ACTIVITY_SLOT        = "createActivity";
const ::fwCom::Slots::SlotKeyType SActivityWizard::s_UPDATE_ACTIVITY_SLOT        = "updateActivity";
const ::fwCom::Slots::SlotKeyType SActivityWizard::s_UPDATE_ACTIVITY_SERIES_SLOT = "updateActivitySeries";
const ::fwCom::Signals::SignalKeyType SActivityWizard::s_ACTIVITY_CREATED_SIG    = "activityCreated";
const ::fwCom::Signals::SignalKeyType SActivityWizard::s_ACTIVITY_UPDATED_SIG    = "activityUpdated";
const ::fwCom::Signals::SignalKeyType SActivityWizard::s_CANCELED_SIG            = "canceled";

//------------------------------------------------------------------------------

SActivityWizard::SActivityWizard() noexcept :
    m_mode(Mode::CREATE),
    m_confirmUpdate(true),
    m_isCancelable(true)
{
    newSlot(s_CREATE_ACTIVITY_SLOT, &SActivityWizard::createActivity, this);
    newSlot(s_UPDATE_ACTIVITY_SLOT, &SActivityWizard::updateActivity, this);
    newSlot(s_UPDATE_ACTIVITY_SERIES_SLOT, &SActivityWizard::updateActivitySeries, this);

    m_sigActivityCreated = newSignal<ActivityCreatedSignalType>(s_ACTIVITY_CREATED_SIG);
    m_sigActivityUpdated = newSignal<ActivityUpdatedSignalType>(s_ACTIVITY_UPDATED_SIG);
    m_sigCanceled        = newSignal< CanceledSignalType >(s_CANCELED_SIG);
}

//------------------------------------------------------------------------------

SActivityWizard::~SActivityWizard() noexcept
{
}

//------------------------------------------------------------------------------

void SActivityWizard::configuring()
{
    ::fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("ioSelectorConfig");
    if (config)
    {
        m_ioSelectorConfig = config->getValue();
    }

    ::fwRuntime::ConfigurationElement::sptr confirmConfig = m_configuration->findConfigurationElement("confirm");
    if (confirmConfig)
    {
        std::string confirmStr = confirmConfig->getValue();
        SLM_ASSERT("'confirm' value must be 'true' or 'false', actual: '" + confirmStr + "'",
                   confirmStr == "true" || confirmStr == "false");
        m_confirmUpdate = (confirmStr == "true");
    }

    ::fwRuntime::ConfigurationElement::sptr cancelConfig = m_configuration->findConfigurationElement("cancel");
    if (cancelConfig)
    {
        std::string cancelStr = cancelConfig->getValue();
        SLM_ASSERT("'cancel' value must be 'true' or 'false', actual: '" + cancelStr + "'",
                   cancelStr == "true" || cancelStr == "false");
        m_isCancelable = (cancelStr == "true");
    }

    std::vector < ::fwRuntime::ConfigurationElement::sptr > iconsCfg = m_configuration->find("icons");
    if (!iconsCfg.empty())
    {
        SLM_ASSERT("Only one 'config' tag is allowed for SSelector configuration", iconsCfg.size() == 1);

        std::vector < ::fwRuntime::ConfigurationElement::sptr > cfg = iconsCfg.front()->find("icon");

        for(::fwRuntime::ConfigurationElement::sptr elt :  cfg)
        {
            const std::string type = elt->getAttributeValue("type");
            SLM_ASSERT("'series' attribute is missing", !type.empty());

            const std::string icon = elt->getAttributeValue("icon");
            SLM_ASSERT("'icon' attribute is missing", !icon.empty());

            const auto file = ::fwRuntime::getResourceFilePath(icon);
            m_objectIcons[type] = file.string();
        }
    }
}

//------------------------------------------------------------------------------

void SActivityWizard::starting()
{
    ::fwGui::IGuiContainerSrv::create();

    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();

    QVBoxLayout* layout = new QVBoxLayout();

    m_title = new QLabel("");
    m_title->setStyleSheet("QLabel { font: bold; color: blue; }");
    m_title->setAlignment(Qt::AlignHCenter);
    layout->addWidget(m_title);

    m_description = new QLabel("");
    m_description->setStyleSheet("QLabel { font: italic; border: solid 1px;}");
    m_description->setAlignment(Qt::AlignHCenter);
    layout->addWidget(m_description);

    m_activityDataView = new widget::ActivityDataView();
    m_activityDataView->setIOSelectorConfig(m_ioSelectorConfig);
    m_activityDataView->setObjectIconAssociation(m_objectIcons);

    layout->addWidget(m_activityDataView, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    layout->addLayout(buttonLayout);

    if (m_isCancelable)
    {
        m_cancelButton = new QPushButton("Cancel");
        m_cancelButton->setToolTip("Cacnel the activity creation");
        buttonLayout->addWidget(m_cancelButton);
    }

    m_resetButton = new QPushButton("Clear");
    m_resetButton->setToolTip("Clear the current selected data");
    buttonLayout->addWidget(m_resetButton);

    m_okButton = new QPushButton("Apply");
    m_okButton->setToolTip("Create or update the activity with the selected data");
    buttonLayout->addWidget(m_okButton);

    container->setLayout(layout);

    QObject::connect(m_activityDataView.data(), &widget::ActivityDataView::currentChanged,
                     this, &SActivityWizard::onTabChanged);
    QObject::connect(m_okButton.data(), &QPushButton::clicked, this, &SActivityWizard::onBuildActivity);
    QObject::connect(m_resetButton.data(), &QPushButton::clicked, this, &SActivityWizard::onReset);
    if (m_isCancelable)
    {
        QObject::connect(m_cancelButton.data(), &QPushButton::clicked, this, &SActivityWizard::onCancel);
    }
}

//------------------------------------------------------------------------------

void SActivityWizard::stopping()
{
    m_activityDataView->clear();

    QObject::disconnect(m_activityDataView.data(), &widget::ActivityDataView::currentChanged,
                        this, &SActivityWizard::onTabChanged);
    QObject::disconnect(m_okButton.data(), &QPushButton::clicked, this, &SActivityWizard::onBuildActivity);
    QObject::disconnect(m_resetButton.data(), &QPushButton::clicked, this, &SActivityWizard::onReset);
    if (m_isCancelable)
    {
        QObject::disconnect(m_cancelButton.data(), &QPushButton::clicked, this, &SActivityWizard::onCancel);
    }

    this->destroy();
}

//------------------------------------------------------------------------------

void SActivityWizard::updating()
{
    auto as = this->getInOut< ::fwMedData::ActivitySeries>("activitySeries");
    if (as)
    {
        this->updateActivity(as);
    }

    SLM_DEBUG_IF("activity series is not defined, it cannot be updated", !as);
}

//------------------------------------------------------------------------------

void SActivityWizard::createActivity(std::string activityID)
{
    m_mode = Mode::CREATE;
    ::fwActivities::registry::ActivityInfo info;
    info = ::fwActivities::registry::Activities::getDefault()->getInfo(activityID);

    // load activity bundle
    std::shared_ptr< ::fwRuntime::Bundle > bundle = ::fwRuntime::findBundle(info.bundleId,
                                                                            info.bundleVersion);
    if (!bundle->isStarted())
    {
        bundle->start();
    }

    m_actSeries = ::fwMedData::ActivitySeries::New();

    m_actSeries->setModality("OT");
    m_actSeries->setInstanceUID("fwActivities." + ::fwTools::UUID::generateUUID() );

    ::boost::posix_time::ptime now = ::boost::posix_time::second_clock::local_time();
    m_actSeries->setDate(::fwTools::getDate(now));
    m_actSeries->setTime(::fwTools::getTime(now));

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

    if (needConfig)
    {
        m_activityDataView->fillInformation(info);
        if (m_activityDataView->count() > 1)
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
            SLM_ASSERT("minOccurs and maxOccurs should be 0", req.minOccurs == 0 && req.maxOccurs == 0);
            ::fwData::Composite::sptr data = m_actSeries->getData();
            (*data)[req.name]              = ::fwData::factory::New(req.type);
        }

        ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();
        ::fwMedDataTools::helper::SeriesDB helper(seriesDB);
        helper.add(m_actSeries);
        helper.notify();
        m_sigActivityCreated->asyncEmit(m_actSeries);
    }
}

//------------------------------------------------------------------------------

void SActivityWizard::updateActivity(::fwMedData::ActivitySeries::sptr activitySeries)
{
    ::fwActivities::registry::ActivityInfo info;
    info = ::fwActivities::registry::Activities::getDefault()->getInfo(activitySeries->getActivityConfigId());

    // load activity bundle
    std::shared_ptr< ::fwRuntime::Bundle > bundle = ::fwRuntime::findBundle(info.bundleId,
                                                                            info.bundleVersion);
    if (!bundle->isStarted())
    {
        bundle->start();
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

    if (needConfig)
    {
        m_activityDataView->fillInformation(m_actSeries);
        if (m_activityDataView->count() > 1)
        {
            m_okButton->setText("Next");
        }
    }
    else
    {
        // Start immediately without popping any configuration UI
        ::fwData::Object::ModifiedSignalType::sptr sig;
        sig = m_actSeries->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();
        m_sigActivityUpdated->asyncEmit(m_actSeries);
    }
}

//------------------------------------------------------------------------------

void SActivityWizard::updateActivitySeries(::fwMedData::Series::sptr series)
{
    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::dynamicCast(series);
    if (activitySeries)
    {
        this->updateActivity(activitySeries);
    }
}

//------------------------------------------------------------------------------

void SActivityWizard::onTabChanged(int index)
{
    if (index == m_activityDataView->count() - 1)
    {
        m_okButton->setText("Apply");
    }
    else
    {
        m_okButton->setText("Next");
    }
}

//------------------------------------------------------------------------------

void SActivityWizard::onReset()
{
    if (m_actSeries)
    {
        ::fwActivities::registry::ActivityInfo info;
        info = ::fwActivities::registry::Activities::getDefault()->getInfo(m_actSeries->getActivityConfigId());
        m_activityDataView->fillInformation(info);

        if (m_activityDataView->count() > 1)
        {
            m_okButton->setText("Next");
        }
    }
}

//------------------------------------------------------------------------------

void SActivityWizard::onCancel()
{
    m_activityDataView->clear();
    m_sigCanceled->asyncEmit();
}

//------------------------------------------------------------------------------

void SActivityWizard::onBuildActivity()
{
    int index   = m_activityDataView->currentIndex();
    int lastTab = m_activityDataView->count() -1;

    std::string errorMsg;
    // Check current data
    if (m_activityDataView->checkData(size_t(index), errorMsg))
    {
        if (index != lastTab)
        {
            // enable and select the next tab
            m_activityDataView->setTabEnabled(index+1, true);
            m_activityDataView->setCurrentIndex(index+1);
        }

        else // index == lastTab
        {
            // Create/update activity
            if (m_mode == Mode::UPDATE && m_confirmUpdate)
            {
                QMessageBox::StandardButton button = QMessageBox::question(
                    qApp->activeWindow(),
                    "Update activity",
                    "You will override your activity. You could loose some data.\n"
                    "Would you duplicate your activity ?",
                    QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                    QMessageBox::No);

                if (button == QMessageBox::Cancel)
                {
                    return;
                }
                else if (button == QMessageBox::Yes)
                {
                    m_actSeries = ::fwData::Object::copy(m_actSeries);
                    m_mode      = Mode::CREATE; // The new activity should be added in the seriesDB
                }
            }

            // check all data and create/update the activity
            bool ok = m_activityDataView->checkAndComputeData(m_actSeries, errorMsg);
            if (ok)
            {
                ::fwData::Composite::sptr data = m_actSeries->getData();

                // Copy the patient/study information of a series
                ::fwMedData::Series::sptr series;
                for(const auto& elt : (*data) )
                {
                    series = ::fwMedData::Series::dynamicCast(elt.second);
                    if(series)
                    {
                        m_actSeries->setPatient( ::fwData::Object::copy(series->getPatient()) );
                        m_actSeries->setStudy( ::fwData::Object::copy(series->getStudy()) );
                        m_actSeries->setEquipment( ::fwData::Object::copy(series->getEquipment()) );
                        break;
                    }
                }

                if (m_mode == Mode::CREATE)
                {
                    // Add the new activity series in seriesDB
                    ::fwActivities::registry::ActivityInfo info;
                    info = ::fwActivities::registry::Activities::getDefault()->getInfo(
                        m_actSeries->getActivityConfigId());

                    std::string description = ::fwGui::dialog::InputDialog::showInputDialog(
                        "Activity creation",
                        "Please, give a description of the activity.",
                        info.title);
                    if (description.empty())
                    {
                        return;
                    }
                    m_actSeries->setDescription(description);
                    ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();
                    ::fwMedDataTools::helper::SeriesDB helper(seriesDB);
                    helper.add(m_actSeries);
                    helper.notify();
                    m_sigActivityCreated->asyncEmit(m_actSeries);
                }
                else // m_mode == Mode::UPDATE
                {
                    ::fwData::Object::ModifiedSignalType::sptr sig;
                    sig = m_actSeries->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
                    sig->asyncEmit();
                    m_sigActivityUpdated->asyncEmit(m_actSeries);
                }
            }
            else
            {
                QString message = "This activity can not be created : \n";
                message.append(QString::fromStdString(errorMsg));
                QMessageBox::warning(qApp->activeWindow(), "Activity Creation", message);
                SLM_ERROR(errorMsg);
            }
        }
    }
    else
    {
        QMessageBox::warning(qApp->activeWindow(), "Error", QString::fromStdString(errorMsg));
    }
}

//------------------------------------------------------------------------------

} //namespace editor
} //namespace uiMedDataQt
