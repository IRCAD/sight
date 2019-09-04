/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "guiQml/editor/SActivityView.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/GuiRegistry.hpp>

#include <fwServices/factory/newActivity.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <QObject>

namespace guiQml
{
namespace editor
{

fwServicesRegisterMacro( ::fwGui::view::IActivityView, ::guiQml::editor::SActivityView );

const ::fwCom::Slots::SlotKeyType s_ENABLED_NEXT_SLOT     = "showNext";
const ::fwCom::Slots::SlotKeyType s_ENABLED_PREVIOUS_SLOT = "showPrevious";

const fwCom::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG = "activityLaunched";

//------------------------------------------------------------------------------

SActivityView::SActivityView()
{
    newSlot(s_ENABLED_NEXT_SLOT, &SActivityView::enabledNext, this);
    newSlot(s_ENABLED_PREVIOUS_SLOT, &SActivityView::enabledPrevious, this);
    m_sigActivityLaunched = newSignal< ActivityLaunchedSignalType >(s_ACTIVITY_LAUNCHED_SIG);

    // get the qml engine QmlApplicationEngine
    m_engine = ::fwQml::QmlEngine::getDefault();
}

//------------------------------------------------------------------------------

SActivityView::~SActivityView()
{
}

//------------------------------------------------------------------------------

void SActivityView::starting()
{
    //this->::fwGui::IGuiContainerSrv::create();

    m_configManager = ::fwServices::IAppConfigManager::New();

    if (!m_mainActivityId.empty())
    {
        ::fwMedData::ActivitySeries::sptr activity = this->createMainActivity();
        if (activity)
        {
            this->launchActivity(activity);
        }
    }
}

//------------------------------------------------------------------------------

void SActivityView::stopping()
{
    if (m_configManager && m_configManager->isStarted())
    {
        m_configManager->stopAndDestroy();
    }

    // possibly delete activityview but still continue

    //this->destroy();
}

//------------------------------------------------------------------------------

void SActivityView::updating()
{
}

//------------------------------------------------------------------------------

void SActivityView::launchActivity(::fwMedData::ActivitySeries::sptr activitySeries)
{
    const auto& rootObjects = m_engine->getRootObjects();
    if (!m_activityView)
    {
        for (const auto& root: rootObjects)
        {
            m_activityView = root->findChild<QObject*>("guiQml_SActivityView");
            if (m_activityView)
            {
                break;
            }
        }
        SLM_ASSERT("Missing ActivityView in QML", m_activityView);

    }
    SLM_ASSERT("Missing ActivityView in QML", m_activityView);
    if (this->validateActivity(activitySeries))
    {
        if (m_configManager->isStarted())
        {
            m_configManager->stopAndDestroy();
        }
        ::fwActivities::registry::ActivityInfo info;
        info = ::fwActivities::registry::Activities::getDefault()->getInfo(activitySeries->getActivityConfigId());

        ReplaceMapType replaceMap;
        this->translateParameters(m_parameters, replaceMap);
        this->translateParameters(activitySeries->getData(), info.appConfig.parameters, replaceMap);
        replaceMap["AS_UID"]       = activitySeries->getID();
        replaceMap[ "WID_PARENT" ] = m_wid;
        std::string genericUidAdaptor = ::fwServices::registry::AppConfig::getUniqueIdentifier(info.appConfig.id);
        replaceMap["GENERIC_UID"] = genericUidAdaptor;
        try
        {
            auto activityManager = ::fwServices::factory::NewActivity(info.appConfig.id);
            activityManager->setId(info.appConfig.id);
            activityManager->setStringObject(replaceMap);
            if (m_activityManager)
            {
                m_activityManager->deleteActivityInView();
                activityManager->setObject(m_activityManager->getObject());
            }
            activityManager->launchActivityInView();
            auto objectList = activityManager->getObject();
            m_activityManager = activityManager;

            m_sigActivityLaunched->asyncEmit(activitySeries);
        }
        catch( std::exception& e )
        {
            ::fwGui::dialog::MessageDialog::showMessageDialog("Activity launch failed",
                                                              e.what(),
                                                              ::fwGui::dialog::IMessageDialog::CRITICAL);
            OSLM_ERROR(e.what());
        }
    }
}

//------------------------------------------------------------------------------

void SActivityView::enabledPrevious(bool isEnabled)
{
    SLM_ASSERT("Missing ActivityView in QML", m_activityView);
    QObject* previousButton = m_activityView->findChild<QObject*>("guiQml_PreviousActivity");
    SLM_ASSERT("Missing in SActivityView guiQml_PreviousActivity", previousButton);
    previousButton->setProperty("enabled", isEnabled);
}

//------------------------------------------------------------------------------

void SActivityView::enabledNext(bool isEnabled)
{
    SLM_ASSERT("Missing ActivityView in QML", m_activityView);
    QObject* nextButton = m_activityView->findChild<QObject*>("guiQml_NextActivity");
    SLM_ASSERT("Missing in SActivityView guiQml_NextActivity", nextButton);
    nextButton->setProperty("enabled", isEnabled);
}

}// namespace editor
}// namespace guiQml
