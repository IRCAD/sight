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

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/GuiRegistry.hpp>

#include <fwGuiQml/container/QmlContainer.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig.hpp>

#include <QVBoxLayout>
#include <QWidget>

namespace guiQml
{
namespace editor
{

fwServicesRegisterMacro( ::fwGui::view::IActivityView, ::guiQml::editor::SActivityView );

const fwCom::Signals::SignalKeyType s_ACTIVITY_LAUNCHED_SIG = "activityLaunched";

//------------------------------------------------------------------------------

SActivityView::SActivityView()
{
    m_sigActivityLaunched = newSignal< ActivityLaunchedSignalType >(s_ACTIVITY_LAUNCHED_SIG);
}

//------------------------------------------------------------------------------

SActivityView::~SActivityView()
{
}

//------------------------------------------------------------------------------

void SActivityView::starting()
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQml::container::QmlContainer::sptr parentContainer
        = ::fwGuiQml::container::QmlContainer::dynamicCast( this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* widget     = new QWidget();
    layout->addWidget( widget );

    ::fwGuiQml::container::QmlContainer::sptr subContainer = ::fwGuiQml::container::QmlContainer::New();

    subContainer->setQmlContainer(widget);
    m_wid = this->getID() + "_container";
    ::fwGui::GuiRegistry::registerWIDContainer(m_wid, subContainer);

    parentContainer->setLayout(layout);

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

    ::fwGui::container::fwContainer::sptr subContainer = ::fwGui::GuiRegistry::getWIDContainer(m_wid);
    ::fwGui::GuiRegistry::unregisterWIDContainer(m_wid);

    subContainer->destroyContainer();

    this->destroy();
}

//------------------------------------------------------------------------------

void SActivityView::updating()
{
}

//------------------------------------------------------------------------------

void SActivityView::launchActivity(::fwMedData::ActivitySeries::sptr activitySeries)
{
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
            m_configManager->setConfig( info.appConfig.id, replaceMap );
            m_configManager->launch();

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

}// namespace editor
}// namespace guiQml
