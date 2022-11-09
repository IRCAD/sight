/************************************************************************
 *
 * Copyright (C) 2016-2022 IRCAD France
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

#include "SSequencer.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/runtime/path.hpp>
#include <core/tools/Os.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QApplication>
#include <QDir>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QVBoxLayout>

namespace sight::module::ui::qt::activity
{

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType s_ACTIVITY_CREATED_SIG = "activityCreated";
const core::com::Signals::SignalKeyType s_DATA_REQUIRED_SIG    = "dataRequired";
const core::com::Signals::SignalKeyType s_HAS_PREVIOUS_SIG     = "hasPrevious";
const core::com::Signals::SignalKeyType s_HAS_NEXT_SIG         = "hasNext";
const core::com::Signals::SignalKeyType s_NEXT_ENABLED_SIG     = "nextEnabled";

const core::com::Slots::SlotKeyType s_GO_TO_SLOT      = "goTo";
const core::com::Slots::SlotKeyType s_CHECK_NEXT_SLOT = "checkNext";
const core::com::Slots::SlotKeyType s_NEXT_SLOT       = "next";
const core::com::Slots::SlotKeyType s_PREVIOUS_SLOT   = "previous";
const core::com::Slots::SlotKeyType s_SEND_INFO_SLOT  = "sendInfo";

static const std::string s_CLEAR_ACTIVITIES_CONFIG = "clearActivities";
static const std::string s_THEME_CONFIG            = "theme";
static const std::string s_CLEAR_CONFIG            = "clear";
static const std::string s_ACCENT_CONFIG           = "accent";
static const std::string s_FOREGROUND_CONFIG       = "foreground";
static const std::string s_BACKGROUND_CONFIG       = "background";
static const std::string s_PRIMARY_CONFIG          = "primary";
static const std::string s_ELEVATION_CONFIG        = "elevation";

//------------------------------------------------------------------------------

SSequencer::SSequencer() noexcept :
    m_sigActivityCreated(newSignal<ActivityCreatedSignalType>(s_ACTIVITY_CREATED_SIG)),
    m_sigDataRequired(newSignal<DataRequiredSignalType>(s_DATA_REQUIRED_SIG)),
    m_sigHasPrevious(newSignal<BoolSignalType>(s_HAS_PREVIOUS_SIG)),
    m_sigHasNext(newSignal<BoolSignalType>(s_HAS_NEXT_SIG)),
    m_sigNextEnabled(newSignal<BoolSignalType>(s_NEXT_ENABLED_SIG))
{
    newSlot(s_GO_TO_SLOT, &SSequencer::goTo, this);
    newSlot(s_CHECK_NEXT_SLOT, &SSequencer::checkNext, this);
    newSlot(s_NEXT_SLOT, &SSequencer::next, this);
    newSlot(s_PREVIOUS_SLOT, &SSequencer::previous, this);
    newSlot(s_SEND_INFO_SLOT, &SSequencer::sendInfo, this);
}

//------------------------------------------------------------------------------

SSequencer::~SSequencer() noexcept =
    default;

//------------------------------------------------------------------------------

void SSequencer::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();

    const service::IService::ConfigType config = this->getConfiguration();

    auto pair = config.equal_range("activity");
    auto it   = pair.first;
    for( ; it != pair.second ; ++it)
    {
        m_activityIds.push_back(it->second.get<std::string>("<xmlattr>.id"));
        m_activityNames.push_back(it->second.get<std::string>("<xmlattr>.name", ""));
    }

    m_clearActivities = config.get<bool>(s_CLEAR_ACTIVITIES_CONFIG, m_clearActivities);

    m_theme      = config.get<std::string>(s_THEME_CONFIG, m_theme);
    m_clear      = config.get<std::string>(s_CLEAR_CONFIG, m_clear);
    m_accent     = config.get<std::string>(s_ACCENT_CONFIG, m_accent);
    m_foreground = config.get<std::string>(s_FOREGROUND_CONFIG, m_foreground);
    m_background = config.get<std::string>(s_BACKGROUND_CONFIG, m_background);
    m_primary    = config.get<std::string>(s_PRIMARY_CONFIG, m_primary);
    m_elevation  = config.get<std::string>(s_ELEVATION_CONFIG, m_elevation);
}

//------------------------------------------------------------------------------

void SSequencer::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    auto* mainLayout = new QVBoxLayout();
    mainLayout->setContentsMargins(0, 0, 0, 0);

    m_widget = new QQuickWidget();
    mainLayout->addWidget(m_widget);

    const auto path =
        core::runtime::getModuleResourceFilePath("sight::module::ui::qt", "ActivitySequencer.qml");
    QWidget* parent = qtContainer->getQtContainer();
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
    std::filesystem::path qt5LibDir              = core::tools::os::getSharedLibraryPath("Qt5Core").remove_filename();
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

    QStringList activitiesName;

    auto activityReg = sight::activity::extension::Activity::getDefault();
    for(std::size_t i = 0 ; i < m_activityIds.size() ; ++i)
    {
        std::string name = m_activityNames[i];
        if(name.empty())
        {
            const auto info = activityReg->getInfo(m_activityIds[i]);
            name = info.title;
        }

        activitiesName.append(QString::fromStdString(name));
    }

    engine->rootContext()->setContextProperty("activityNameList", activitiesName);
    engine->rootContext()->setContextProperty("widgetWidth", m_widget->width());
    engine->rootContext()->setContextProperty(QString::fromStdString(s_THEME_CONFIG), theme);
    engine->rootContext()->setContextProperty(
        QString::fromStdString(s_ACCENT_CONFIG),
        QString::fromStdString(m_accent)
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(s_FOREGROUND_CONFIG),
        QString::fromStdString(m_foreground)
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(s_BACKGROUND_CONFIG),
        QString::fromStdString(m_background)
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(s_PRIMARY_CONFIG),
        QString::fromStdString(
            m_primary
        )
    );
    engine->rootContext()->setContextProperty(
        QString::fromStdString(s_ELEVATION_CONFIG),
        QString::fromStdString(m_elevation)
    );

    m_widget->setSource(QUrl::fromLocalFile(QString::fromStdString(path.string())));

    QObject::connect(m_widget->rootObject(), SIGNAL(activitySelected(int)), this, SLOT(goTo(int)));

    qtContainer->setLayout(mainLayout);
}

//------------------------------------------------------------------------------

void SSequencer::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

void SSequencer::updating()
{
    {
        auto activity_set = m_activity_set.lock();
        SIGHT_ASSERT("Missing '" << s_ACTIVITY_SET_INOUT << "' activity_set", activity_set);

        m_currentActivity = this->parseActivities(*activity_set);
    }
    if(m_currentActivity >= 0)
    {
        for(int i = 0 ; i <= m_currentActivity ; ++i)
        {
            this->enableActivity(i);
        }

        // launch the last activity
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

void SSequencer::goTo(int index)
{
    if(index < 0 || index >= static_cast<int>(m_activityIds.size()))
    {
        SIGHT_ERROR("no activity to launch at index " << index)
        return;
    }

    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << s_ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    // Clear activities if go backward.
    if(m_clearActivities && m_currentActivity > index)
    {
        auto dialog = sight::ui::base::dialog::MessageDialog(
            "Sequencer",
            "The data will be deleted! \nDo you want to continue?",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
        dialog.addButton(sight::ui::base::dialog::IMessageDialog::YES_NO);
        const auto button = dialog.show();

        if((button == sight::ui::base::dialog::IMessageDialog::NO))
        {
            return;
        }

        // Disable all next activities (including current)
        for(int i = index + 1, end = int(activity_set->size()) ; i < end ; ++i)
        {
            this->disableActivity(i);
        }

        // Remove all last activities.
        this->removeLastActivities(*activity_set, std::size_t(index));
    }
    // Store data otherwise.
    else if(m_currentActivity >= 0)
    {
        this->storeActivityData(*activity_set, std::size_t(m_currentActivity));
    }

    const auto newIdx = static_cast<std::size_t>(index);

    data::Activity::sptr activity = this->getActivity(*activity_set, newIdx, m_slotUpdate);

    bool ok = true;
    std::string errorMsg;

    std::tie(ok, errorMsg) = sight::module::ui::qt::activity::SSequencer::validateActivity(activity);
    if(ok)
    {
        m_sigActivityCreated->asyncEmit(activity);

        m_currentActivity = index;
        QObject* object = m_widget->rootObject();
        QMetaObject::invokeMethod(object, "setCurrentActivity", Q_ARG(QVariant, index));
        QMetaObject::invokeMethod(object, "enableActivity", Q_ARG(QVariant, index));
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show("Activity not valid", errorMsg);
        m_sigDataRequired->asyncEmit(activity);
    }
}

//------------------------------------------------------------------------------

void SSequencer::checkNext()
{
    auto activity_set = m_activity_set.lock();
    SIGHT_ASSERT("Missing '" << s_ACTIVITY_SET_INOUT << "' activity_set", activity_set);

    // Store current activity data before checking the next one,
    // new data can be added in the current activity during the process.
    if(m_currentActivity >= 0)
    {
        this->storeActivityData(*activity_set, std::size_t(m_currentActivity));
    }

    const auto nextIdx = static_cast<std::size_t>(m_currentActivity) + 1;
    if(nextIdx < m_activityIds.size())
    {
        data::Activity::sptr nextActivity = this->getActivity(*activity_set, nextIdx, m_slotUpdate);

        bool ok = true;
        std::string errorMsg;

        std::tie(ok, errorMsg) = sight::module::ui::qt::activity::SSequencer::validateActivity(nextActivity);
        if(ok)
        {
            this->enableActivity(m_currentActivity + 1);
            m_sigNextEnabled->asyncEmit(true);
        }
        else
        {
            m_sigNextEnabled->asyncEmit(false);
            SIGHT_DEBUG(errorMsg);
        }
    }
}

//------------------------------------------------------------------------------

void SSequencer::next()
{
    this->goTo(m_currentActivity + 1);
}

//------------------------------------------------------------------------------

void SSequencer::previous()
{
    this->goTo(m_currentActivity - 1);
}

//------------------------------------------------------------------------------

void SSequencer::sendInfo() const
{
    const bool previousEnabled = (m_currentActivity > 0);
    m_sigHasPrevious->asyncEmit(previousEnabled);

    const bool nextEnabled = (m_currentActivity < static_cast<int>(m_activityIds.size()) - 1);
    m_sigHasNext->asyncEmit(nextEnabled);
}

//------------------------------------------------------------------------------

void SSequencer::enableActivity(int index)
{
    QObject* object = m_widget->rootObject();
    QMetaObject::invokeMethod(object, "enableActivity", Q_ARG(QVariant, index));
}

//------------------------------------------------------------------------------

void SSequencer::disableActivity(int index)
{
    QObject* object = m_widget->rootObject();
    QMetaObject::invokeMethod(object, "disableActivity", Q_ARG(QVariant, index));
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SSequencer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_ACTIVITY_SET_INOUT, data::ActivitySet::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_ACTIVITY_SET_INOUT, data::ActivitySet::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
