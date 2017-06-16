/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "guiQt/editor/SActivityView.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/GuiRegistry.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/AppConfig2.hpp>

#include <QVBoxLayout>
#include <QWidget>

namespace guiQt
{
namespace editor
{

fwServicesRegisterMacro( ::gui::view::IActivityView, ::guiQt::editor::SActivityView );

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

void SActivityView::starting() throw(::fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr parentContainer
        = ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );

    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* widget     = new QWidget();
    layout->addWidget( widget );

    ::fwGuiQt::container::QtContainer::sptr subContainer = ::fwGuiQt::container::QtContainer::New();

    subContainer->setQtContainer(widget);
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

void SActivityView::stopping() throw(::fwTools::Failed)
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

void SActivityView::updating() throw(::fwTools::Failed)
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
        this->translateParameters(this->getObject(), m_parameters, replaceMap);
        this->translateParameters(activitySeries->getData(), info.appConfig.parameters, replaceMap);
        replaceMap["AS_UID"]       = activitySeries->getID();
        replaceMap[ "WID_PARENT" ] = m_wid;
        std::string genericUidAdaptor = ::fwServices::registry::AppConfig2::getUniqueIdentifier(info.appConfig.id);
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
}// namespace guiQt
