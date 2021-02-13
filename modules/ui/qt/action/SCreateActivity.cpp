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

#include "modules/ui/qt/action/SCreateActivity.hpp"

#include <activity/IBuilder.hpp>
#include <activity/IValidator.hpp>

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>
#include <data/String.hpp>
#include <data/Vector.hpp>

#include <service/macros.hpp>

#include <boost/foreach.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/SelectorDialog.hpp>

Q_DECLARE_METATYPE(sight::activity::registry::ActivityInfo)

namespace sight::modules::ui::qt
{
namespace action
{

//------------------------------------------------------------------------------

const core::com::Signals::SignalKeyType SCreateActivity::s_ACTIVITY_ID_SELECTED_SIG = "activityIDSelected";
const core::com::Signals::SignalKeyType SCreateActivity::s_ACTIVITY_SELECTED_SIG    = "activitySelected";

//------------------------------------------------------------------------------

SCreateActivity::SCreateActivity() noexcept
{
    m_sigActivityIDSelected = newSignal< ActivityIDSelectedSignalType >(s_ACTIVITY_ID_SELECTED_SIG);
    m_sigActivitySelected   = newSignal< ActivitySelectedSignalType >(s_ACTIVITY_SELECTED_SIG);
}

//------------------------------------------------------------------------------

SCreateActivity::~SCreateActivity() noexcept
{
}

//------------------------------------------------------------------------------

void SCreateActivity::starting()
{
    this->actionServiceStarting();
}

//------------------------------------------------------------------------------

void SCreateActivity::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SCreateActivity::configuring()
{
    this->sight::ui::base::IActionSrv::initialize();
    typedef service::IService::ConfigType ConfigType;

    const service::IService::ConfigType srvconfig = this->getConfigTree();

    if(srvconfig.count("filter") == 1 )
    {
        const service::IService::ConfigType& configFilter = srvconfig.get_child("filter");
        SLM_ASSERT("A maximum of 1 <mode> tag is allowed", configFilter.count("mode") < 2);

        const std::string mode = configFilter.get< std::string >("mode");
        SLM_ASSERT("'" + mode + "' value for <mode> tag isn't valid. Allowed values are : 'include', 'exclude'.",
                   mode == "include" || mode == "exclude");
        m_filterMode = mode;

        BOOST_FOREACH( const ConfigType::value_type& v,  configFilter.equal_range("id") )
        {
            m_keys.push_back(v.second.get<std::string>(""));
        }
    }
    SLM_ASSERT("A maximum of 1 <filter> tag is allowed", srvconfig.count("filter") < 2);
}

//------------------------------------------------------------------------------

activity::registry::ActivityInfo SCreateActivity::show( const ActivityInfoContainer& infos )
{
    QWidget* parent = qApp->activeWindow();

    QDialog* const dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString("Choose an activity"));
    dialog->resize(600, 400);

    QStandardItemModel* const model = new QStandardItemModel(dialog);
    for( const activity::registry::ActivityInfo& info :  infos)
    {
        std::string text;
        if(info.title.empty())
        {
            text = info.id;
        }
        else
        {
            text = info.title + (info.description.empty() ? "" : "\n" + info.description);
        }

        QStandardItem* const item = new QStandardItem(QIcon(info.icon.c_str()), QString::fromStdString(text));
        item->setData(QVariant::fromValue(info));
        item->setEditable(false);
        model->appendRow(item);
    }

    QListView* const selectionList = new QListView();
    selectionList->setIconSize(QSize(40, 40));
    selectionList->setUniformItemSizes(true);
    selectionList->setModel(model);

    QModelIndex index = model->index( 0, 0 );
    if ( index.isValid() )
    {
        selectionList->selectionModel()->select( index, QItemSelectionModel::Select );
    }

    QPushButton* const okButton     = new QPushButton("Ok");
    QPushButton* const cancelButton = new QPushButton("Cancel");

    QHBoxLayout* const hLayout = new QHBoxLayout();
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);

    QVBoxLayout* const vLayout = new QVBoxLayout();
    vLayout->addWidget(selectionList);
    vLayout->addLayout(hLayout);

    dialog->setLayout(vLayout);
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selectionList, SIGNAL(doubleClicked(const QModelIndex&)), dialog, SLOT(accept()));

    activity::registry::ActivityInfo info;
    if(dialog->exec())
    {
        QModelIndex currentIndex = selectionList->selectionModel()->currentIndex();
        QStandardItem* item      = model->itemFromIndex( currentIndex );
        QVariant var             = item->data();
        info = var.value< activity::registry::ActivityInfo >();
    }

    return info;
}

//------------------------------------------------------------------------------

SCreateActivity::ActivityInfoContainer SCreateActivity::getEnabledActivities(const ActivityInfoContainer& infos)
{
    ActivityInfoContainer configs;

    if(m_filterMode == "include" || m_filterMode == "exclude")
    {
        const bool isIncludeMode = m_filterMode == "include";

        for(const auto& info : infos)
        {
            KeysType::iterator keyIt = std::find(m_keys.begin(), m_keys.end(), info.id);

            if(keyIt != m_keys.end() && isIncludeMode)
            {
                configs.push_back(info);
            }
            else if(keyIt == m_keys.end() && !isIncludeMode)
            {
                configs.push_back(info);
            }
        }
    }
    else
    {
        configs = infos;
    }

    return configs;
}

//------------------------------------------------------------------------------

void SCreateActivity::updating()
{
    ActivityInfoContainer infos = activity::registry::Activity::getDefault()->getInfos();
    infos = this->getEnabledActivities(infos);

    if ( !infos.empty())
    {
        activity::registry::ActivityInfo info;
        if((m_keys.size() == 1 && m_filterMode == "include") || (infos.size() == 1))
        {
            info = infos[0];
        }
        else
        {
            info = this->show( infos );
        }

        if( !info.id.empty() )
        {
            m_sigActivityIDSelected->asyncEmit(info.id);
        }
    }
    else
    {
        sight::ui::base::dialog::MessageDialog::show("Activity launcher",
                                                     "No available activity for the current selection.",
                                                     sight::ui::base::dialog::MessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

}
}
