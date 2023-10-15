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

    const auto selectedDicomSeries = m_selectedDicomSeries.lock();
    SIGHT_ASSERT("Selected dicom Series vector should not be null.", selectedDicomSeries);

    sight::ui::service::create();
    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(getContainer());

    auto* mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    qtContainer->setLayout(mainLayout);

    // Size policy
    QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Top widget
    auto* topLayout = new QHBoxLayout();
    auto* topWidget = new QWidget();
    topWidget->setLayout(topLayout);
    topLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    mainLayout->addWidget(topWidget);

    // Available filter list
    m_availableFilterListWidget = new QComboBox();
    this->fillAvailableFilters();
    topLayout->addWidget(m_availableFilterListWidget);

    auto path = core::runtime::get_module_resource_path("sight::module::ui::icons");

    // Add filter button
    m_addFilterButton = new QPushButton(QIcon(QString::fromStdString((path / "Plus.svg").string())), "Add");
    m_addFilterButton->setSizePolicy(policy);
    topLayout->addWidget(m_addFilterButton);

    // Add selected filters
    m_selectedFilterListWidget = new QListWidget();
    mainLayout->addWidget(m_selectedFilterListWidget);
    m_selectedFilterListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_selectedFilterListWidget->setDragEnabled(true);
    m_selectedFilterListWidget->viewport()->setAcceptDrops(true);
    m_selectedFilterListWidget->setDropIndicatorShown(true);
    m_selectedFilterListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    m_selectedFilterListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Add forced apply checkbox
    auto* applyLayout = new QHBoxLayout();
    auto* applyWidget = new QWidget();
    applyWidget->setLayout(applyLayout);
    applyWidget->setSizePolicy(policy);
    applyLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_forcedApplyCheckBox = new QCheckBox("Ignore errors");
    applyLayout->addWidget(m_forcedApplyCheckBox);
    mainLayout->addWidget(applyWidget, 0, Qt::AlignRight);

    // Bottom widget
    auto* bottomLayout = new QHBoxLayout();
    auto* bottomWidget = new QWidget();
    bottomWidget->setLayout(bottomLayout);
    bottomLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    mainLayout->addWidget(bottomWidget);

    auto* buttonLayout = new QHBoxLayout();
    auto* buttonWidget = new QWidget();
    buttonWidget->setLayout(buttonLayout);
    buttonWidget->setSizePolicy(policy);
    buttonLayout->setContentsMargins(QMargins(0, 0, 0, 0));

    // Apply filters button
    m_applyFiltersButton = new QPushButton(QIcon(QString::fromStdString((path / "Apply.svg").string())), "Apply");
    m_applyFiltersButton->setSizePolicy(policy);
    buttonLayout->addWidget(m_applyFiltersButton);

    // Configure filter button
    m_configureFilterButton = new QPushButton(
        QIcon(QString::fromStdString((path / "Settings.svg").string())),
        "Configure"
    );
    m_configureFilterButton->setSizePolicy(policy);
    m_configureFilterButton->setEnabled(false);
    buttonLayout->addWidget(m_configureFilterButton);

    // Split filter button
    m_splitFilterButton = new QPushButton(QIcon(QString::fromStdString((path / "Split.svg").string())), "Split");
    m_splitFilterButton->setSizePolicy(policy);
    m_splitFilterButton->setEnabled(false);
    buttonLayout->addWidget(m_splitFilterButton);

    // Remove filter button
    m_removeFilterButton =
        new QPushButton(QIcon(QString::fromStdString((path / "Minus.svg").string())), "Remove");
    m_removeFilterButton->setSizePolicy(policy);
    m_removeFilterButton->setEnabled(false);
    buttonLayout->addWidget(m_removeFilterButton);

    bottomLayout->addWidget(buttonWidget, 0, Qt::AlignRight);

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

void filter_selection_editor::onForceChecked(int state)
{
    if(state == Qt::Checked)
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

    std::vector<sight::filter::dicom::filter::sptr> sortedFilters;
    for(const std::string& key : sight::filter::dicom::registry::get()->get_factory_keys())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(key);
        sortedFilters.push_back(filter);
    }

    std::sort(sortedFilters.begin(), sortedFilters.end(), filter_selection_editor::sortFilters);

    for(const sight::filter::dicom::filter::sptr& filter : sortedFilters)
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

void filter_selection_editor::addFilter(int filterTypeIndex)
{
    int index = m_selectedFilterListWidget->currentRow() + 1;
    this->addFilter(filterTypeIndex, index);
}

//------------------------------------------------------------------------------

void filter_selection_editor::addFilterAtTheEnd()
{
    this->addFilter(m_availableFilterListWidget->currentIndex(), m_selectedFilterListWidget->count());
}

//------------------------------------------------------------------------------

void filter_selection_editor::addFilter(int filterTypeIndex, int position)
{
    // Get information from selected filter
    QIcon icon          = m_availableFilterListWidget->itemIcon(filterTypeIndex);
    QString name        = m_availableFilterListWidget->itemText(filterTypeIndex);
    QString description = m_availableFilterListWidget->itemData(filterTypeIndex, Qt::ToolTipRole).toString();
    std::string key     = m_availableFilterListWidget->itemData(filterTypeIndex).toString().toStdString();

    // Create filter
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(key);
    std::string id                            = filter->get_id();
    m_filtersMap[id] = filter;

    // Add a the filter in the list
    m_selectedFilterListWidget->insertItem(position, name);
    m_selectedFilterListWidget->item(position)->setIcon(icon);
    m_selectedFilterListWidget->item(position)->setToolTip(description);
    m_selectedFilterListWidget->item(position)->setData(Qt::UserRole, id.c_str());
    m_selectedFilterListWidget->setCurrentRow(position);

    // Update apply button
    m_applyFiltersButton->setEnabled(m_selectedFilterListWidget->count() > 0);
}

//------------------------------------------------------------------------------

void filter_selection_editor::removeFilter()
{
    int currentIndex = m_selectedFilterListWidget->currentRow();
    if(currentIndex >= 0)
    {
        // Remove selected filter from map
        std::string id = m_selectedFilterListWidget->item(currentIndex)->data(Qt::UserRole).toString().toStdString();
        m_filtersMap.erase(id);

        // Remove selected filter from widget
        m_selectedFilterListWidget->takeItem(currentIndex);
    }
}

//------------------------------------------------------------------------------

void filter_selection_editor::configureFilter()
{
    int selectedFilterIndex = m_selectedFilterListWidget->currentRow();
    std::string id          =
        m_selectedFilterListWidget->item(selectedFilterIndex)->data(Qt::UserRole).toString().toStdString();
    m_filtersMap[id]->configureWithGUI();
}

//------------------------------------------------------------------------------

void filter_selection_editor::splitFilter()
{
    int currentIndex        = m_selectedFilterListWidget->currentRow();
    std::string compositeId =
        m_selectedFilterListWidget->item(currentIndex)->data(Qt::UserRole).toString().toStdString();
    sight::filter::dicom::composite::base::sptr composite =
        std::dynamic_pointer_cast<sight::filter::dicom::composite::base>(m_filtersMap[compositeId]);

    // Remove composite filter
    this->removeFilter();

    // Add filters
    int position = currentIndex;
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

    m_selectedFilterListWidget->setCurrentRow(currentIndex);
}

//------------------------------------------------------------------------------

void filter_selection_editor::updateButtonStatus(int filterIndex)
{
    bool hasFilter = (filterIndex != -1);
    m_removeFilterButton->setEnabled(hasFilter);
    if(hasFilter)
    {
        std::string id = m_selectedFilterListWidget->item(filterIndex)->data(Qt::UserRole).toString().toStdString();

        // Configure filter button
        m_configureFilterButton->setEnabled(m_filtersMap[id]->isConfigurableWithGUI());

        // Split filter button
        m_splitFilterButton->setEnabled(m_filtersMap[id]->getFilterType() == sight::filter::dicom::filter::COMPOSITE);
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
    const auto selectedDicomSeries = m_selectedDicomSeries.lock();
    SIGHT_ASSERT("Selected dicom Series vector  object should not be null.", selectedDicomSeries);

    // Display the informations
    sight::ui::dialog::message messageBox;
    messageBox.setIcon(sight::ui::dialog::message::INFO);
    messageBox.addButton(sight::ui::dialog::message::OK);
    messageBox.setTitle("Filters information");

    // Clear destination series_set
    const auto dest_series_set = m_dest_series_set.lock();
    SIGHT_ASSERT("The series_set \"" + m_dest_series_setID + "\" doesn't exist.", dest_series_set);

    const auto scoped_emitter = dest_series_set->scoped_emit();
    dest_series_set->clear();

    // Be sure series are selected
    if(selectedDicomSeries->empty())
    {
        messageBox.setMessage("You must select series on which you want to apply your filters.");
    }
    else if(m_selectedFilterListWidget->count() == 0)
    {
        messageBox.setMessage("You must select the filters that you want to apply on your series.");
    }
    else
    {
        // Create containers
        DicomSeriesContainertype dicomSeriesContainer;
        FilterContainertype filterContainer;

        // Copy selected DicomSeries
        for(const data::object::sptr& obj : *selectedDicomSeries)
        {
            data::dicom_series::sptr srcDicomSerie = std::dynamic_pointer_cast<data::dicom_series>(obj);
            SIGHT_ASSERT("The series should be a DicomSerie.", srcDicomSerie);

            data::dicom_series::sptr destinationDicomSerie = std::make_shared<data::dicom_series>();
            destinationDicomSerie->deep_copy(srcDicomSerie);
            dicomSeriesContainer.push_back(destinationDicomSerie);
        }

        // Create filter vector
        for(int i = 0 ; i < m_selectedFilterListWidget->count() ; ++i)
        {
            std::string id = m_selectedFilterListWidget->item(i)->data(Qt::UserRole).toString().toStdString();
            filterContainer.push_back(m_filtersMap[id]);
        }

        std::stringstream ssFilters;
        std::stringstream ssInfos;
        bool forcedApply = m_forcedApplyCheckBox->isChecked();

        ssFilters << "<b>Filters :</b><br />";
        // Let's apply all the filters
        for(const sight::filter::dicom::filter::sptr& filter : filterContainer)
        {
            ssFilters << "- " << filter->getName() << " -> ";
            try
            {
                sight::filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, forcedApply);
                ssFilters << "<font color=\"Green\">OK</font><br />";
            }
            catch(sight::filter::dicom::exceptions::FilterFailure& e)
            {
                ssFilters << "<font color=\"Red\">ERROR</font><br />";
                ssInfos << "- " << e.what() << "<br />";
                if(!forcedApply)
                {
                    break;
                }
            }
        }

        // Push series
        if(forcedApply || ssInfos.str().empty())
        {
            // Add filtered series to series_set
            for(const auto& series : dicomSeriesContainer)
            {
                dest_series_set->push_back(series);
            }
        }

        // If there is no error
        if(ssInfos.str().empty())
        {
            ssInfos << "All filters have been correctly applied.";
        }

        std::string msg = ssFilters.str() + "<br /><br /><b>Informations :</b><br />" + ssInfos.str();
        messageBox.setMessage(msg);
    }

    // Diplay message
    messageBox.show();
}

//------------------------------------------------------------------------------

void filter_selection_editor::showContextMenuForSelectedFilter(const QPoint& pos)
{
    // Create context menu
    QMenu contextMenu("Context menu", m_selectedFilterListWidget);

    // Add menu
    QMenu* addMenu = contextMenu.addMenu("Add");

    // Fill the menu with the available filters
    for(int i = 0 ; i < m_availableFilterListWidget->count() ; ++i)
    {
        QString text = m_availableFilterListWidget->itemText(i);
        QIcon icon   = m_availableFilterListWidget->itemIcon(i);
        auto* action = new QAction(icon, text, m_selectedFilterListWidget);
        action->setIconVisibleInMenu(true);
        addMenu->addAction(action);

        QObject::connect(action, &QAction::triggered, [this, i]{addFilter(i);});
    }

    // Check id the menu is requested from a filter
    QListWidgetItem* filterItem = m_selectedFilterListWidget->itemAt(pos);
    if(filterItem != nullptr)
    {
        // Get filter
        std::string id                            = filterItem->data(Qt::UserRole).toString().toStdString();
        sight::filter::dicom::filter::sptr filter = m_filtersMap[id];

        // Remove action
        auto* removeAction = new QAction("Remove", m_selectedFilterListWidget);
        QObject::connect(removeAction, &QAction::triggered, this, &filter_selection_editor::removeFilter);
        contextMenu.addAction(removeAction);

        // Configure action
        auto* configureAction = new QAction("Configure", m_selectedFilterListWidget);
        configureAction->setEnabled(filter->isConfigurableWithGUI());
        QObject::connect(configureAction, &QAction::triggered, this, &filter_selection_editor::removeFilter);
        contextMenu.addAction(configureAction);

        // Split action
        auto* splitAction = new QAction("Split", m_selectedFilterListWidget);
        splitAction->setEnabled(filter->getFilterType() == sight::filter::dicom::filter::COMPOSITE);
        QObject::connect(splitAction, &QAction::triggered, this, &filter_selection_editor::removeFilter);
        contextMenu.addAction(splitAction);
    }

    // Display menu
    contextMenu.exec(m_selectedFilterListWidget->mapToGlobal(pos));
}

//------------------------------------------------------------------------------

bool filter_selection_editor::sortFilters(
    const sight::filter::dicom::filter::sptr& a,
    const sight::filter::dicom::filter::sptr& b
)
{
    if(a->getFilterType() == b->getFilterType())
    {
        return a->getName() < b->getName();
    }

    return a->getFilterType() > b->getFilterType();
}

//------------------------------------------------------------------------------

QIcon filter_selection_editor::getFilterIcon(sight::filter::dicom::filter::sptr filter)
{
    const std::filesystem::path path = core::runtime::get_module_resource_path(std::string("sight::module::ui::icons"));
    std::array icons {
        QIcon(QString::fromStdString((path / "Modifier.svg").string())),
        QIcon(QString::fromStdString((path / "Sorter.svg").string())),
        QIcon(QString::fromStdString((path / "Splitter.svg").string())),
        QIcon(QString::fromStdString((path / "Composite.svg").string())),
        QIcon(QString::fromStdString((path / "Custom.svg").string()))
    };
    return icons[filter->getFilterType()];
}

//------------------------------------------------------------------------------

std::string filter_selection_editor::getFilterDescription(sight::filter::dicom::filter::sptr filter)
{
    std::array types {"Modifier", "Sorter", "Splitter", "Composite", "Custom"};
    std::string description =
        "<b>Name :</b> " + filter->getName() + "<br />"
                                               "<b>Type :</b> " + types[filter->getFilterType()] + "<br />"
                                                                                                   "<b>Configurable :</b> "
        + ((filter->isConfigurableWithGUI()) ? "true" : "false") + "<br />"
                                                                   "<b>Informations :</b><br />"
        + filter->getDescription();
    return description;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::dicom
