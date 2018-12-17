/************************************************************************
 *
 * Copyright (C) 2016-2018 IRCAD France
 * Copyright (C) 2016-2018 IHU Strasbourg
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

#include "activities/action/SCreateActivity.hpp"

#include <fwActivities/IBuilder.hpp>
#include <fwActivities/IValidator.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <fwRuntime/Bundle.hpp>
#include <fwRuntime/Convert.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <boost/foreach.hpp>

#include <QApplication>
#include <QDialog>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QStandardItemModel>
#include <QVBoxLayout>

Q_DECLARE_METATYPE(::fwActivities::registry::ActivityInfo)

namespace activities
{
namespace action
{

//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::activities::action::SCreateActivity, ::fwData::Vector );

//------------------------------------------------------------------------------

const ::fwCom::Signals::SignalKeyType SCreateActivity::s_ACTIVITY_ID_SELECTED_SIG = "activityIDSelected";
const ::fwCom::Signals::SignalKeyType SCreateActivity::s_ACTIVITY_SELECTED_SIG    = "activitySelected";

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
    this->::fwGui::IActionSrv::initialize();
    typedef ::fwServices::IService::ConfigType ConfigType;

    const ::fwServices::IService::ConfigType srvconfig = this->getConfigTree();

    if(srvconfig.count("filter") == 1 )
    {
        const ::fwServices::IService::ConfigType& configFilter = srvconfig.get_child("filter");
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

::fwActivities::registry::ActivityInfo SCreateActivity::show( const ActivityInfoContainer& infos )
{
    QWidget* parent = qApp->activeWindow();

    QDialog* dialog = new QDialog(parent);
    dialog->setWindowTitle(QString::fromStdString("Choose an activity"));
    dialog->resize(600, 400);

    QStandardItemModel* model = new QStandardItemModel(dialog);
    for( const ::fwActivities::registry::ActivityInfo& info :  infos)
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

        QStandardItem* item = new QStandardItem(QIcon(info.icon.c_str()), QString::fromStdString(text));
        item->setData(QVariant::fromValue(info));
        item->setEditable(false);
        model->appendRow(item);
    }

    QListView* selectionList = new QListView();
    selectionList->setIconSize(QSize(40, 40));
    selectionList->setUniformItemSizes(true);
    selectionList->setModel(model);

    QModelIndex index = model->index( 0, 0 );
    if ( index.isValid() )
    {
        selectionList->selectionModel()->select( index, QItemSelectionModel::Select );
    }

    QPushButton* okButton     = new QPushButton("Ok");
    QPushButton* cancelButton = new QPushButton("Cancel");

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(okButton);
    hLayout->addWidget(cancelButton);

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addWidget(selectionList);
    vLayout->addLayout(hLayout);

    dialog->setLayout(vLayout);
    QObject::connect(okButton, SIGNAL(clicked()), dialog, SLOT(accept()));
    QObject::connect(cancelButton, SIGNAL(clicked()), dialog, SLOT(reject()));
    QObject::connect(selectionList, SIGNAL(doubleClicked(const QModelIndex&)), dialog, SLOT(accept()));

    ::fwActivities::registry::ActivityInfo info;
    if(dialog->exec())
    {
        QModelIndex currentIndex = selectionList->selectionModel()->currentIndex();
        QStandardItem* item      = model->itemFromIndex( currentIndex );
        QVariant var             = item->data();
        info = var.value< ::fwActivities::registry::ActivityInfo >();
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

        for(auto info : infos)
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
    ActivityInfoContainer infos = ::fwActivities::registry::Activities::getDefault()->getInfos();
    infos = this->getEnabledActivities(infos);

    if ( !infos.empty())
    {
        ::fwActivities::registry::ActivityInfo info;
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
        ::fwGui::dialog::MessageDialog::showMessageDialog("Activity launcher",
                                                          "No available activity for the current selection.",
                                                          ::fwGui::dialog::MessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

}
}
