/************************************************************************
 *
 * Copyright (C) 2016-2019 IRCAD France
 * Copyright (C) 2016-2019 IHU Strasbourg
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

#include "uiActivitiesQt/editor/SActivitySequencer.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <QApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QVBoxLayout>

namespace uiActivitiesQt
{
namespace editor
{

//------------------------------------------------------------------------------

const ::fwServices::IService::KeyType s_SERIESDB_INOUT = "seriesDB";

const ::fwCom::Signals::SignalKeyType s_ACTIVITY_CREATED_SIG = "activityCreated";
const ::fwCom::Signals::SignalKeyType s_DATA_REQUIRED_SIG    = "dataRequired";
const ::fwCom::Signals::SignalKeyType s_ENABLED_PREVIOUS_SIG = "enabledPrevious";
const ::fwCom::Signals::SignalKeyType s_ENABLED_NEXT_SIG     = "enabledNext";

const ::fwCom::Slots::SlotKeyType s_GO_TO_SLOT      = "goTo";
const ::fwCom::Slots::SlotKeyType s_CHECK_NEXT_SLOT = "checkNext";
const ::fwCom::Slots::SlotKeyType s_NEXT_SLOT       = "next";
const ::fwCom::Slots::SlotKeyType s_PREVIOUS_SLOT   = "previous";
const ::fwCom::Slots::SlotKeyType s_SEND_INFO_SLOT  = "sendInfo";

//------------------------------------------------------------------------------

SActivitySequencer::SActivitySequencer() noexcept
{
    m_sigActivityCreated = newSignal< ActivityCreatedSignalType >(s_ACTIVITY_CREATED_SIG);
    m_sigDataRequired    = newSignal< DataRequiredSignalType >(s_DATA_REQUIRED_SIG);
    m_sigEnabledPrevious = newSignal< EnabledPreviousSignalType >(s_ENABLED_PREVIOUS_SIG);
    m_sigEnabledNext     = newSignal< EnabledNextSignalType >(s_ENABLED_NEXT_SIG);
    newSlot(s_GO_TO_SLOT, &SActivitySequencer::goTo, this);
    newSlot(s_CHECK_NEXT_SLOT, &SActivitySequencer::checkNext, this);
    newSlot(s_NEXT_SLOT, &SActivitySequencer::next, this);
    newSlot(s_PREVIOUS_SLOT, &SActivitySequencer::previous, this);
    newSlot(s_SEND_INFO_SLOT, &SActivitySequencer::sendInfo, this);
}

//------------------------------------------------------------------------------

SActivitySequencer::~SActivitySequencer() noexcept
{
}

//------------------------------------------------------------------------------

void SActivitySequencer::configuring()
{
    this->::fwGui::IGuiContainerSrv::initialize();

    const ::fwServices::IService::ConfigType config = this->getConfigTree();

    auto pair = config.equal_range("activity");
    auto it   = pair.first;
    for (; it != pair.second; ++it)
    {
        m_activityIds.push_back(it->second.get<std::string>("<xmlattr>.id"));
        m_activityNames.push_back(it->second.get<std::string>("<xmlattr>.name", ""));
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::starting()
{
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QVBoxLayout* mainLayout = new QVBoxLayout();

    m_widget = new QQuickWidget();
    mainLayout->addWidget(m_widget);

    const auto path = ::fwRuntime::getBundleResourceFilePath("uiActivitiesQt", "uiActivitiesQt/ActivitySequencer.qml");
    QWidget* parent = qtContainer->getQtContainer();
    auto engine     = m_widget->engine();
    m_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);

    QColor color = parent->palette().color(QPalette::Background);

    QString style = "light";
    // styleSheet override QPalette
    // we assume that styleSheet is the dark style
    if(!qApp->styleSheet().isEmpty())
    {
        color = QColor("#31363b");
        style = "dark";
    }
    m_widget->setClearColor(color);

    engine->addImportPath(QML_IMPORT_PATH);

    QStringList activitiesName;

    auto activityReg = ::fwActivities::registry::Activities::getDefault();
    for (size_t i = 0; i < m_activityIds.size(); ++i)
    {
        std::string name = m_activityNames[i];
        if (name.empty())
        {
            const auto info = activityReg->getInfo(m_activityIds[i]);
            name = info.title;
        }
        activitiesName.append(QString::fromStdString(name));
    }

    engine->rootContext()->setContextProperty("activityNameList", activitiesName);
    engine->rootContext()->setContextProperty("widgetWidth", m_widget->width());
    engine->rootContext()->setContextProperty("style", style);

    m_widget->setSource(QUrl::fromLocalFile(QString::fromStdString(path.string())));

    QObject::connect(m_widget->rootObject(), SIGNAL(activitySelected(int)), this, SLOT(goTo(int)));

    qtContainer->setLayout(mainLayout);
}

//------------------------------------------------------------------------------

void SActivitySequencer::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SActivitySequencer::updating()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);
    SLM_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    m_currentActivity = this->parseActivities(seriesDB);
    if (m_currentActivity >= 0)
    {
        // launch the last series
        this->goTo(m_currentActivity);
    }
    else
    {
        // launch the first activity
        this->enableActivity(0);
        this->goTo(0);
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::goTo(int index)
{
    if (index < 0 || index >= static_cast<int>(m_activityIds.size()))
    {
        OSLM_ERROR("no activity to launch at index " << index)
        return;
    }
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);
    SLM_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    if (m_currentActivity >= 0)
    {
        this->storeActivityData(seriesDB);
    }

    const size_t newIdx = static_cast<size_t>(index);

    ::fwMedData::ActivitySeries::sptr activity = this->getActivity(seriesDB, newIdx, m_slotUpdate);

    bool ok = true;
    std::string errorMsg;

    std::tie(ok, errorMsg) = this->validateActivity(activity);
    if(ok)
    {
        m_sigActivityCreated->asyncEmit(activity);

        m_currentActivity = index;
        QObject* object = m_widget->rootObject();
        QMetaObject::invokeMethod(object, "setCurrentActivity", Q_ARG(QVariant, index));
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Activity not valid", errorMsg);
        m_sigDataRequired->asyncEmit(activity);
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::checkNext()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(s_SERIESDB_INOUT);
    SLM_ASSERT("Missing '" + s_SERIESDB_INOUT +"' seriesDB", seriesDB);

    const size_t nextIdx = static_cast<size_t>(m_currentActivity + 1);
    if (nextIdx < m_activityIds.size())
    {
        ::fwMedData::ActivitySeries::sptr nextActivity = this->getActivity(seriesDB, nextIdx, m_slotUpdate);

        bool ok = true;
        std::string errorMsg;

        std::tie(ok, errorMsg) = this->validateActivity(nextActivity);
        if (ok)
        {
            this->enableActivity(m_currentActivity + 1);
        }
    }
}

//------------------------------------------------------------------------------

void SActivitySequencer::next()
{
    this->goTo(m_currentActivity + 1);
}

//------------------------------------------------------------------------------

void SActivitySequencer::previous()
{
    this->goTo(m_currentActivity - 1);
}

//------------------------------------------------------------------------------

void SActivitySequencer::sendInfo() const
{
    const bool previousEnabled = (m_currentActivity > 0);
    m_sigEnabledPrevious->asyncEmit(previousEnabled);

    const bool nextEnabled = (m_currentActivity < static_cast<int>(m_activityIds.size()) -1);
    m_sigEnabledNext->asyncEmit(nextEnabled);
}

//------------------------------------------------------------------------------

void SActivitySequencer::enableActivity(int index)
{
    QObject* object = m_widget->rootObject();
    QMetaObject::invokeMethod(object, "enableActivity", Q_ARG(QVariant, index));

}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SActivitySequencer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SERIESDB_INOUT, ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG, s_UPDATE_SLOT );
    connections.push( s_SERIESDB_INOUT, ::fwMedData::SeriesDB::s_MODIFIED_SIG, s_UPDATE_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace activities
