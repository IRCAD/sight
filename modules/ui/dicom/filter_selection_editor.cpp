/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "filter_selection_editor.hpp"

#include <core/runtime/path.hpp>

#include <data/series_set.hpp>
#include <data/vector.hpp>

#include <filter/dicom/composite/base.hpp>
#include <filter/dicom/exceptions/FilterFailure.hpp>
#include <filter/dicom/helper/Filter.hpp>
#include <filter/dicom/registry/detail.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>

namespace sight::module::ui::dicom
{

//------------------------------------------------------------------------------

filter_selection_editor::filter_selection_editor() noexcept =
    default;

//------------------------------------------------------------------------------

filter_selection_editor::~filter_selection_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void filter_selection_editor::info(std::ostream& _sstream)
{
    _sstream << "filter_selection_editor::info";
}

//------------------------------------------------------------------------------

void filter_selection_editor::configuring()
{
    sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

void filter_selection_editor::starting()
{
    // Get Destination series_set
    const auto dest_series_set = m_dest_series_set.lock();
    SIGHT_ASSERT("The series_set \"" + m_dest_series_setID + "\" doesn't exist.", dest_series_set);

    const auto selected_dicom_series = m_selectedDicomSeries.lock();
    SIGHT_ASSERT("Selected dicom Series vector should not be null.", selected_dicom_series);

    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* main_layout = new QVBoxLayout();
    main_layout->setAlignment(Qt::AlignTop);
    qt_container->setLayout(main_layout);

    // Size policy
    QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Top widget
    auto* top_layout = new QHBoxLayout();
    auto* top_widget = new QWidget();
    top_widget->setLayout(top_layout);
    top_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    main_layout->addWidget(top_widget);

    // Available filter list
    m_availableFilterListWidget = new QComboBox();
    this->fillAvailableFilters();
    top_layout->addWidget(m_availableFilterListWidget);

    auto path = core::runtime::get_module_resource_path("sight::module::ui::icons");

    // Add filter button
    m_addFilterButton = new QPushButton(QIcon(QString::fromStdString((path / "Plus.svg").string())), "Add");
    m_addFilterButton->setSizePolicy(policy);
    top_layout->addWidget(m_addFilterButton);

    // Add selected filters
    m_selectedFilterListWidget = new QListWidget();
    main_layout->addWidget(m_selectedFilterListWidget);
    m_selectedFilterListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_selectedFilterListWidget->setDragEnabled(true);
    m_selectedFilterListWidget->viewport()->setAcceptDrops(true);
    m_selectedFilterListWidget->setDropIndicatorShown(true);
    m_selectedFilterListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_selectedFilterListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Add forced apply checkbox
    auto* apply_layout = new QHBoxLayout();
    auto* apply_widget = new QWidget();
    apply_widget->setLayout(apply_layout);
    apply_widget->setSizePolicy(policy);
    apply_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_forcedApplyCheckBox = new QCheckBox("Ignore errors");
    apply_layout->addWidget(m_forcedApplyCheckBox);
    main_layout->addWidget(apply_widget, 0, Qt::AlignRight);

    // Bottom widget
    auto* bottom_layout = new QHBoxLayout();
    auto* bottom_widget = new QWidget();
    bottom_widget->setLayout(bottom_layout);
    bottom_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    main_layout->addWidget(bottom_widget);

    auto* button_layout = new QHBoxLayout();
    auto* button_widget = new QWidget();
    button_widget->setLayout(button_layout);
    button_widget->setSizePolicy(policy);
    button_layout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Apply filters button
    m_applyFiltersButton = new QPushButton(QIcon(QString::fromStdString((path / "Apply.svg").string())), "Apply");
    m_applyFiltersButton->setSizePolicy(policy);
    button_layout->addWidget(m_applyFiltersButton);

    // Configure filter button
    m_configureFilterButton = new QPushButton(
        QIcon(QString::fromStdString((path / "Settings.svg").string())),
        "Configure"
    );
    m_configureFilterButton->setSizePolicy(policy);
    m_configureFilterButton->setEnabled(false);
    button_layout->addWidget(m_configureFilterButton);

    // Split filter button
    m_splitFilterButton = new QPushButton(QIcon(QString::fromStdString((path / "Split.svg").string())), "Split");
    m_splitFilterButton->setSizePolicy(policy);
    m_splitFilterButton->setEnabled(false);
    button_layout->addWidget(m_splitFilterButton);

    // Remove filter button
    m_removeFilterButton =
        new QPushButton(QIcon(QString::fromStdString((path / "Minus.svg").string())), "Remove");
    m_removeFilterButton->setSizePolicy(policy);
    m_removeFilterButton->setEnabled(false);
    button_layout->addWidget(m_removeFilterButton);

    bottom_layout->addWidget(button_widget, 0, Qt::AlignRight);

    // Create shortcut
    m_deleteShortcut =
        new QShortcut(QKeySequence(Qt::Key_Delete), m_selectedFilterListWidget, nullptr, nullptr, Qt::WidgetShortcut);

    // Connect the signals
    QObject::connect(m_selectedFilterListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(updateButtonStatus(int)));
    QObject::connect(m_addFilterButton, SIGNAL(clicked(void)), this, SLOT(addFilterAtTheEnd(void)));
    QObject::connect(m_removeFilterButton, SIGNAL(clicked(void)), this, SLOT(removeFilter(void)));
    QObject::connect(m_configureFilterButton, SIGNAL(clicked(void)), this, SLOT(configureFilter(void)));
    QObject::connect(m_splitFilterButton, SIGNAL(clicked(void)), this, SLOT(splitFilter(void)));
    QObject::connect(m_applyFiltersButton, SIGNAL(clicked(void)), this, SLOT(applyFilters(void)));
    QObject::connect(m_deleteShortcut, SIGNAL(activated()), this, SLOT(removeFilter(void)));
    QObject::connect(
        m_selectedFilterListWidget,
        SIGNAL(customContextMenuRequested(const QPoint&)),
        this,
        SLOT(showContextMenuForSelectedFilter(const QPoint&))
    );
    QObject::connect(m_forcedApplyCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onForceChecked(int)));
}

//------------------------------------------------------------------------------

void filter_selection_editor::onForceChecked(int _state)
{
    if(_state == Qt::Checked)
    {
        sight::ui::dialog::message::show(
            "Be careful",
            "You asked to ignore reading errors, there are high risks of issues for resulting image.",
            sight::ui::dialog::message::WARNING
        );
    }
}

//------------------------------------------------------------------------------

void filter_selection_editor::fillAvailableFilters()
{
    int index = 0;

    std::vector<sight::filter::dicom::filter::sptr> sorted_filters;
    for(const std::string& key : sight::filter::dicom::registry::get()->get_factory_keys())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(key);
        sorted_filters.push_back(filter);
    }

    std::sort(sorted_filters.begin(), sorted_filters.end(), filter_selection_editor::sortFilters);

    for(const sight::filter::dicom::filter::sptr& filter : sorted_filters)
    {
        // If the filter doesn't have a configuration or if it is configurable using GUI
        if(!filter->isConfigurationRequired() || filter->isConfigurableWithGUI())
        {
            // Create filter
            m_availableFilterListWidget->addItem(filter->getName().c_str(), filter->get_classname().c_str());

            // Set icon
            m_availableFilterListWidget->setItemIcon(index, filter_selection_editor::getFilterIcon(filter));

            // Set description
            m_availableFilterListWidget->setItemData(
                index,
                filter_selection_editor::getFilterDescription(
                    filter
                ).c_str(),
                Qt::ToolTipRole
            );

            ++index;
        }
    }
}

//------------------------------------------------------------------------------

void filter_selection_editor::stopping()
{
    // Disconnect the signals
    QObject::disconnect(
        m_selectedFilterListWidget,
        SIGNAL(currentRowChanged(int)),
        this,
        SLOT(updateButtonStatus(int))
    );
    QObject::disconnect(m_addFilterButton, SIGNAL(clicked(void)), this, SLOT(addFilterAtTheEnd(void)));
    QObject::disconnect(m_removeFilterButton, SIGNAL(clicked(void)), this, SLOT(removeFilter(void)));
    QObject::disconnect(m_configureFilterButton, SIGNAL(clicked(void)), this, SLOT(configureFilter(void)));
    QObject::disconnect(m_splitFilterButton, SIGNAL(clicked(void)), this, SLOT(splitFilter(void)));
    QObject::disconnect(m_applyFiltersButton, SIGNAL(clicked(void)), this, SLOT(applyFilters(void)));
    QObject::disconnect(m_deleteShortcut, SIGNAL(activated()), this, SLOT(removeFilter(void)));
    QObject::disconnect(
        m_selectedFilterListWidget,
        SIGNAL(customContextMenuRequested(const QPoint&)),
        this,
        SLOT(showContextMenuForSelectedFilter(const QPoint&))
    );

    this->destroy();
}

//------------------------------------------------------------------------------

void filter_selection_editor::updating()
{
}

//------------------------------------------------------------------------------

void filter_selection_editor::addFilter(int _filter_type_index)
{
    int index = m_selectedFilterListWidget->currentRow() + 1;
    this->addFilter(_filter_type_index, index);
}

//------------------------------------------------------------------------------

void filter_selection_editor::addFilterAtTheEnd()
{
    this->addFilter(m_availableFilterListWidget->currentIndex(), m_selectedFilterListWidget->count());
}

//------------------------------------------------------------------------------

void filter_selection_editor::addFilter(int _filter_type_index, int _position)
{
    // Get information from selected filter
    QIcon icon          = m_availableFilterListWidget->itemIcon(_filter_type_index);
    QString name        = m_availableFilterListWidget->itemText(_filter_type_index);
    QString description = m_availableFilterListWidget->itemData(_filter_type_index, Qt::ToolTipRole).toString();
    std::string key     = m_availableFilterListWidget->itemData(_filter_type_index).toString().toStdString();

    // Create filter
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(key);
    std::string id                            = filter->get_id();
    m_filtersMap[id] = filter;

    // Add a the filter in the list
    m_selectedFilterListWidget->insertItem(_position, name);
    m_selectedFilterListWidget->item(_position)->setIcon(icon);
    m_selectedFilterListWidget->item(_position)->setToolTip(description);
    m_selectedFilterListWidget->item(_position)->setData(Qt::UserRole, id.c_str());
    m_selectedFilterListWidget->setCurrentRow(_position);

    // Update apply button
    m_applyFiltersButton->setEnabled(m_selectedFilterListWidget->count() > 0);
}

//------------------------------------------------------------------------------

void filter_selection_editor::removeFilter()
{
    int current_index = m_selectedFilterListWidget->currentRow();
    if(current_index >= 0)
    {
        // Remove selected filter from map
        std::string id = m_selectedFilterListWidget->item(current_index)->data(Qt::UserRole).toString().toStdString();
        m_filtersMap.erase(id);

        // Remove selected filter from widget
        m_selectedFilterListWidget->takeItem(current_index);
    }
}

//------------------------------------------------------------------------------

void filter_selection_editor::configureFilter()
{
    int selected_filter_index = m_selectedFilterListWidget->currentRow();
    std::string id            =
        m_selectedFilterListWidget->item(selected_filter_index)->data(Qt::UserRole).toString().toStdString();
    m_filtersMap[id]->configureWithGUI();
}

//------------------------------------------------------------------------------

void filter_selection_editor::splitFilter()
{
    int current_index        = m_selectedFilterListWidget->currentRow();
    std::string composite_id =
        m_selectedFilterListWidget->item(current_index)->data(Qt::UserRole).toString().toStdString();
    sight::filter::dicom::composite::base::sptr composite =
        std::dynamic_pointer_cast<sight::filter::dicom::composite::base>(m_filtersMap[composite_id]);

    // Remove composite filter
    this->removeFilter();

    // Add filters
    int position = current_index;
    for(const sight::filter::dicom::filter::sptr& filter : composite->getChildren())
    {
        std::string id = filter->get_id();
        m_filtersMap[id] = filter;
        m_selectedFilterListWidget->insertItem(position, filter->getName().c_str());
        m_selectedFilterListWidget->item(position)->setIcon(filter_selection_editor::getFilterIcon(filter));
        m_selectedFilterListWidget->item(position)->setToolTip(
            filter_selection_editor::getFilterDescription(filter).c_str()
        );
        m_selectedFilterListWidget->item(position)->setData(Qt::UserRole, id.c_str());
        ++position;
    }

    m_selectedFilterListWidget->setCurrentRow(current_index);
}

//------------------------------------------------------------------------------

void filter_selection_editor::updateButtonStatus(int _filter_index)
{
    bool has_filter = (_filter_index != -1);
    m_removeFilterButton->setEnabled(has_filter);
    if(has_filter)
    {
        std::string id = m_selectedFilterListWidget->item(_filter_index)->data(Qt::UserRole).toString().toStdString();

        // Configure filter button
        m_configureFilterButton->setEnabled(m_filtersMap[id]->isConfigurableWithGUI());

        // Split filter button
        m_splitFilterButton->setEnabled(m_filtersMap[id]->get_filter_type() == sight::filter::dicom::filter::COMPOSITE);
    }
    else
    {
        m_configureFilterButton->setEnabled(false);
        m_splitFilterButton->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

void filter_selection_editor::applyFilters()
{
    using DicomSeriesContainertype = std::vector<data::dicom_series::sptr>;
    using FilterContainertype      = std::vector<sight::filter::dicom::filter::sptr>;

    // Get selected DicomSeries
    const auto selected_dicom_series = m_selectedDicomSeries.lock();
    SIGHT_ASSERT("Selected dicom Series vector  object should not be null.", selected_dicom_series);

    // Display the informations
    sight::ui::dialog::message message_box;
    message_box.setIcon(sight::ui::dialog::message::INFO);
    message_box.addButton(sight::ui::dialog::message::OK);
    message_box.setTitle("Filters information");

    // Clear destination series_set
    const auto dest_series_set = m_dest_series_set.lock();
    SIGHT_ASSERT("The series_set \"" + m_dest_series_setID + "\" doesn't exist.", dest_series_set);

    const auto scoped_emitter = dest_series_set->scoped_emit();
    dest_series_set->clear();

    // Be sure series are selected
    if(selected_dicom_series->empty())
    {
        message_box.setMessage("You must select series on which you want to apply your filters.");
    }
    else if(m_selectedFilterListWidget->count() == 0)
    {
        message_box.setMessage("You must select the filters that you want to apply on your series.");
    }
    else
    {
        // Create containers
        DicomSeriesContainertype dicom_series_container;
        FilterContainertype filter_container;

        // Copy selected DicomSeries
        for(const data::object::sptr& obj : *selected_dicom_series)
        {
            data::dicom_series::sptr src_dicom_serie = std::dynamic_pointer_cast<data::dicom_series>(obj);
            SIGHT_ASSERT("The series should be a DicomSerie.", src_dicom_serie);

            data::dicom_series::sptr destination_dicom_serie = std::make_shared<data::dicom_series>();
            destination_dicom_serie->deep_copy(src_dicom_serie);
            dicom_series_container.push_back(destination_dicom_serie);
        }

        // Create filter vector
        for(int i = 0 ; i < m_selectedFilterListWidget->count() ; ++i)
        {
            std::string id = m_selectedFilterListWidget->item(i)->data(Qt::UserRole).toString().toStdString();
            filter_container.push_back(m_filtersMap[id]);
        }

        std::stringstream ss_filters;
        std::stringstream ss_infos;
        bool forced_apply = m_forcedApplyCheckBox->isChecked();

        ss_filters << "<b>Filters :</b><br />";
        // Let's apply all the filters
        for(const sight::filter::dicom::filter::sptr& filter : filter_container)
        {
            ss_filters << "- " << filter->getName() << " -> ";
            try
            {
                sight::filter::dicom::helper::Filter::applyFilter(dicom_series_container, filter, forced_apply);
                ss_filters << "<font color=\"Green\">OK</font><br />";
            }
            catch(sight::filter::dicom::exceptions::FilterFailure& e)
            {
                ss_filters << "<font color=\"Red\">ERROR</font><br />";
                ss_infos << "- " << e.what() << "<br />";
                if(!forced_apply)
                {
                    break;
                }
            }
        }

        // Push series
        if(forced_apply || ss_infos.str().empty())
        {
            // Add filtered series to series_set
            for(const auto& series : dicom_series_container)
            {
                dest_series_set->push_back(series);
            }
        }

        // If there is no error
        if(ss_infos.str().empty())
        {
            ss_infos << "All filters have been correctly applied.";
        }

        std::string msg = ss_filters.str() + "<br /><br /><b>Informations :</b><br />" + ss_infos.str();
        message_box.setMessage(msg);
    }

    // Diplay message
    message_box.show();
}

//------------------------------------------------------------------------------

void filter_selection_editor::showContextMenuForSelectedFilter(const QPoint& _pos)
{
    // Create context menu
    QMenu context_menu("Context menu", m_selectedFilterListWidget);

    // Add menu
    QMenu* add_menu = context_menu.addMenu("Add");

    // Fill the menu with the available filters
    for(int i = 0 ; i < m_availableFilterListWidget->count() ; ++i)
    {
        QString text = m_availableFilterListWidget->itemText(i);
        QIcon icon   = m_availableFilterListWidget->itemIcon(i);
        auto* action = new QAction(icon, text, m_selectedFilterListWidget);
        action->setIconVisibleInMenu(true);
        add_menu->addAction(action);

        QObject::connect(action, &QAction::triggered, [this, i]{addFilter(i);});
    }

    // Check id the menu is requested from a filter
    QListWidgetItem* filter_item = m_selectedFilterListWidget->itemAt(_pos);
    if(filter_item != nullptr)
    {
        // Get filter
        std::string id                            = filter_item->data(Qt::UserRole).toString().toStdString();
        sight::filter::dicom::filter::sptr filter = m_filtersMap[id];

        // Remove action
        auto* remove_action = new QAction("Remove", m_selectedFilterListWidget);
        QObject::connect(remove_action, &QAction::triggered, this, &filter_selection_editor::removeFilter);
        context_menu.addAction(remove_action);

        // Configure action
        auto* configure_action = new QAction("Configure", m_selectedFilterListWidget);
        configure_action->setEnabled(filter->isConfigurableWithGUI());
        QObject::connect(configure_action, &QAction::triggered, this, &filter_selection_editor::removeFilter);
        context_menu.addAction(configure_action);

        // Split action
        auto* split_action = new QAction("Split", m_selectedFilterListWidget);
        split_action->setEnabled(filter->get_filter_type() == sight::filter::dicom::filter::COMPOSITE);
        QObject::connect(split_action, &QAction::triggered, this, &filter_selection_editor::removeFilter);
        context_menu.addAction(split_action);
    }

    // Display menu
    context_menu.exec(m_selectedFilterListWidget->mapToGlobal(_pos));
}

//------------------------------------------------------------------------------

bool filter_selection_editor::sortFilters(
    const sight::filter::dicom::filter::sptr& _a,
    const sight::filter::dicom::filter::sptr& _b
)
{
    if(_a->get_filter_type() == _b->get_filter_type())
    {
        return _a->getName() < _b->getName();
    }

    return _a->get_filter_type() > _b->get_filter_type();
}

//------------------------------------------------------------------------------

QIcon filter_selection_editor::getFilterIcon(sight::filter::dicom::filter::sptr _filter)
{
    const std::filesystem::path path = core::runtime::get_module_resource_path(std::string("sight::module::ui::icons"));
    std::array icons {
        QIcon(QString::fromStdString((path / "Modifier.svg").string())),
        QIcon(QString::fromStdString((path / "Sorter.svg").string())),
        QIcon(QString::fromStdString((path / "Splitter.svg").string())),
        QIcon(QString::fromStdString((path / "Composite.svg").string())),
        QIcon(QString::fromStdString((path / "Custom.svg").string()))
    };
    return icons[_filter->get_filter_type()];
}

//------------------------------------------------------------------------------

std::string filter_selection_editor::getFilterDescription(sight::filter::dicom::filter::sptr _filter)
{
    std::array types {"Modifier", "Sorter", "Splitter", "Composite", "Custom"};
    std::string description =
        "<b>Name :</b> " + _filter->getName() + "<br />"
                                                "<b>Type :</b> " + types[_filter->get_filter_type()] + "<br />"
                                                                                                       "<b>Configurable :</b> "
        + ((_filter->isConfigurableWithGUI()) ? "true" : "false") + "<br />"
                                                                    "<b>Informations :</b><br />"
        + _filter->getDescription();
    return description;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::dicom
