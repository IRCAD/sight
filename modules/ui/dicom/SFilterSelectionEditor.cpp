/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SFilterSelectionEditor.hpp"

#include <core/runtime/operations.hpp>

#include <data/helper/SeriesDB.hpp>
#include <data/Vector.hpp>

#include <filter/dicom/composite/IComposite.hpp>
#include <filter/dicom/exceptions/FilterFailure.hpp>
#include <filter/dicom/helper/Filter.hpp>
#include <filter/dicom/registry/detail.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QSignalMapper>
#include <QVBoxLayout>

namespace sight::module::ui::dicom
{

//------------------------------------------------------------------------------

SFilterSelectionEditor::SFilterSelectionEditor() noexcept
{
}

//------------------------------------------------------------------------------

SFilterSelectionEditor::~SFilterSelectionEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::info(std::ostream& _sstream)
{
    _sstream << "SFilterSelectionEditor::info";
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::configuring()
{
    sight::ui::base::IGuiContainer::initialize();
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::starting()
{
    // Get Destination SeriesDB
    m_destinationSeriesDB = this->getInOut<data::SeriesDB>("target");
    SIGHT_ASSERT("The SeriesDB \"" + m_destinationSeriesDBID + "\" doesn't exist.", m_destinationSeriesDB);

    data::Vector::csptr dataVector = this->getInput<data::Vector>("selection");
    SIGHT_ASSERT("Vector object should not be null.", dataVector);

    sight::ui::base::IGuiContainer::create();
    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(getContainer());

    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    qtContainer->setLayout(mainLayout);

    // Size policy
    QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Top widget
    QHBoxLayout* topLayout = new QHBoxLayout();
    QWidget* topWidget     = new QWidget();
    topWidget->setLayout(topLayout);
    topLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    mainLayout->addWidget(topWidget);

    // Available filter list
    m_availableFilterListWidget = new QComboBox();
    this->fillAvailableFilters();
    topLayout->addWidget(m_availableFilterListWidget);

    auto path = core::runtime::getModuleResourcePath("sight::module::ui::icons");

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
    QHBoxLayout* applyLayout = new QHBoxLayout();
    QWidget* applyWidget     = new QWidget();
    applyWidget->setLayout(applyLayout);
    applyWidget->setSizePolicy(policy);
    applyLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_forcedApplyCheckBox = new QCheckBox("Ignore errors");
    applyLayout->addWidget(m_forcedApplyCheckBox);
    mainLayout->addWidget(applyWidget, 0, Qt::AlignRight);

    // Bottom widget
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QWidget* bottomWidget     = new QWidget();
    bottomWidget->setLayout(bottomLayout);
    bottomLayout->setContentsMargins(QMargins(0, 0, 0, 0));
    mainLayout->addWidget(bottomWidget);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QWidget* buttonWidget     = new QWidget();
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
        new QShortcut(QKeySequence(Qt::Key_Delete), m_selectedFilterListWidget, 0, 0, Qt::WidgetShortcut);

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

void SFilterSelectionEditor::onForceChecked(int state)
{
    if(state == Qt::Checked)
    {
        sight::ui::base::dialog::MessageDialog::show(
            "Be careful",
            "You asked to ignore reading errors, there are high risks of issues for resulting image.",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::fillAvailableFilters()
{
    unsigned int index = 0;

    std::vector<filter::dicom::IFilter::sptr> sortedFilters;
    for(const std::string& key : filter::dicom::registry::get()->getFactoryKeys())
    {
        filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(key);
        sortedFilters.push_back(filter);
    }

    std::sort(sortedFilters.begin(), sortedFilters.end(), SFilterSelectionEditor::sortFilters);

    for(const filter::dicom::IFilter::sptr& filter : sortedFilters)
    {
        // If the filter doesn't have a configuration or if it is configurable using GUI
        if(!filter->isConfigurationRequired() || filter->isConfigurableWithGUI())
        {
            // Create filter
            m_availableFilterListWidget->addItem(filter->getName().c_str(), filter->getClassname().c_str());

            // Set icon
            m_availableFilterListWidget->setItemIcon(index, SFilterSelectionEditor::getFilterIcon(filter));

            // Set description
            m_availableFilterListWidget->setItemData(
                index,
                SFilterSelectionEditor::getFilterDescription(
                    filter
                ).c_str(),
                Qt::ToolTipRole
            );

            ++index;
        }
    }
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::stopping()
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

void SFilterSelectionEditor::updating()
{
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::addFilter(int filterTypeIndex)
{
    int index = m_selectedFilterListWidget->currentRow() + 1;
    this->addFilter(filterTypeIndex, index);
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::addFilterAtTheEnd()
{
    this->addFilter(m_availableFilterListWidget->currentIndex(), m_selectedFilterListWidget->count());
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::addFilter(int filterTypeIndex, int position)
{
    // Get information from selected filter
    QIcon icon          = m_availableFilterListWidget->itemIcon(filterTypeIndex);
    QString name        = m_availableFilterListWidget->itemText(filterTypeIndex);
    QString description = m_availableFilterListWidget->itemData(filterTypeIndex, Qt::ToolTipRole).toString();
    std::string key     = m_availableFilterListWidget->itemData(filterTypeIndex).toString().toStdString();

    // Create filter
    filter::dicom::IFilter::sptr filter = filter::dicom::factory::New(key);
    std::string id                      = filter->getID();
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

void SFilterSelectionEditor::removeFilter()
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

void SFilterSelectionEditor::configureFilter()
{
    int selectedFilterIndex = m_selectedFilterListWidget->currentRow();
    std::string id          =
        m_selectedFilterListWidget->item(selectedFilterIndex)->data(Qt::UserRole).toString().toStdString();
    m_filtersMap[id]->configureWithGUI();
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::splitFilter()
{
    int currentIndex        = m_selectedFilterListWidget->currentRow();
    std::string compositeId =
        m_selectedFilterListWidget->item(currentIndex)->data(Qt::UserRole).toString().toStdString();
    filter::dicom::composite::IComposite::sptr composite =
        filter::dicom::composite::IComposite::dynamicCast(m_filtersMap[compositeId]);

    // Remove composite filter
    this->removeFilter();

    // Add filters
    int position = currentIndex;
    for(const filter::dicom::IFilter::sptr& filter : composite->getChildren())
    {
        std::string id = filter->getID();
        m_filtersMap[id] = filter;
        m_selectedFilterListWidget->insertItem(position, filter->getName().c_str());
        m_selectedFilterListWidget->item(position)->setIcon(SFilterSelectionEditor::getFilterIcon(filter));
        m_selectedFilterListWidget->item(position)->setToolTip(
            SFilterSelectionEditor::getFilterDescription(filter).c_str()
        );
        m_selectedFilterListWidget->item(position)->setData(Qt::UserRole, id.c_str());
        ++position;
    }

    m_selectedFilterListWidget->setCurrentRow(currentIndex);
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::updateButtonStatus(int filterIndex)
{
    bool hasFilter = (filterIndex != -1);
    m_removeFilterButton->setEnabled(hasFilter);
    if(hasFilter)
    {
        std::string id = m_selectedFilterListWidget->item(filterIndex)->data(Qt::UserRole).toString().toStdString();

        // Configure filter button
        m_configureFilterButton->setEnabled(m_filtersMap[id]->isConfigurableWithGUI());

        // Split filter button
        m_splitFilterButton->setEnabled(m_filtersMap[id]->getFilterType() == filter::dicom::IFilter::COMPOSITE);
    }
    else
    {
        m_configureFilterButton->setEnabled(false);
        m_splitFilterButton->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::applyFilters()
{
    typedef std::vector<data::DicomSeries::sptr> DicomSeriesContainertype;
    typedef std::vector<filter::dicom::IFilter::sptr> FilterContainertype;

    // Get selected DicomSeries
    data::Vector::csptr vector = this->getInput<data::Vector>("selection");
    SIGHT_ASSERT("Vector object should not be null.", vector);

    // Display the informations
    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setIcon(sight::ui::base::dialog::IMessageDialog::INFO);
    messageBox.addButton(sight::ui::base::dialog::IMessageDialog::OK);
    messageBox.setTitle("Filters information");

    // Clear destination SeriesDB
    data::helper::SeriesDB sDBhelper(*m_destinationSeriesDB);
    sDBhelper.clear();

    // Be sure series are selected
    if(vector->empty())
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
        for(const data::Object::sptr& obj : vector->getContainer())
        {
            data::DicomSeries::sptr srcDicomSeries = data::DicomSeries::dynamicCast(obj);
            SIGHT_ASSERT("The series should be a DicomSeries.", srcDicomSeries);

            data::DicomSeries::sptr dicomSeries = data::DicomSeries::New();
            dicomSeries->deepCopy(srcDicomSeries);
            dicomSeriesContainer.push_back(dicomSeries);
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
        for(const filter::dicom::IFilter::sptr& filter : filterContainer)
        {
            ssFilters << "- " << filter->getName() << " -> ";
            try
            {
                filter::dicom::helper::Filter::applyFilter(dicomSeriesContainer, filter, forcedApply);
                ssFilters << "<font color=\"Green\">OK</font><br />";
            }
            catch(filter::dicom::exceptions::FilterFailure& e)
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
            // Add filtered series to SeriesDB
            for(const data::DicomSeries::sptr& series : dicomSeriesContainer)
            {
                sDBhelper.add(series);
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

    // Notify
    sDBhelper.notify();
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::showContextMenuForSelectedFilter(const QPoint& pos)
{
    // Create context menu
    QMenu contextMenu("Context menu", m_selectedFilterListWidget);

    // Add menu
    QMenu* addMenu = contextMenu.addMenu("Add");

    // Use a mapper to retrieve index of the selected filter type
    QPointer<QSignalMapper> mapper = new QSignalMapper();

    // Fill the menu with the available filters
    for(int i = 0 ; i < m_availableFilterListWidget->count() ; ++i)
    {
        QString text             = m_availableFilterListWidget->itemText(i);
        QIcon icon               = m_availableFilterListWidget->itemIcon(i);
        QPointer<QAction> action = new QAction(icon, text, m_selectedFilterListWidget);
        action->setIconVisibleInMenu(true);
        addMenu->addAction(action);

        mapper->setMapping(action, i);
        QObject::connect(action, SIGNAL(triggered()), mapper, SLOT(map()));
    }

    // Connect mapper
    QObject::connect(mapper, SIGNAL(mapped(int)), this, SLOT(addFilter(int)));

    // Check id the menu is requested from a filter
    QListWidgetItem* filterItem = m_selectedFilterListWidget->itemAt(pos);
    if(filterItem)
    {
        // Get filter
        std::string id                      = filterItem->data(Qt::UserRole).toString().toStdString();
        filter::dicom::IFilter::sptr filter = m_filtersMap[id];

        // Remove action
        QPointer<QAction> removeAction = new QAction("Remove", m_selectedFilterListWidget);
        QObject::connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFilter()));
        contextMenu.addAction(removeAction);

        // Configure action
        QPointer<QAction> configureAction = new QAction("Configure", m_selectedFilterListWidget);
        configureAction->setEnabled(filter->isConfigurableWithGUI());
        QObject::connect(configureAction, SIGNAL(triggered()), this, SLOT(configureFilter()));
        contextMenu.addAction(configureAction);

        // Split action
        QPointer<QAction> splitAction = new QAction("Split", m_selectedFilterListWidget);
        splitAction->setEnabled(filter->getFilterType() == filter::dicom::IFilter::COMPOSITE);
        QObject::connect(splitAction, SIGNAL(triggered()), this, SLOT(splitFilter()));
        contextMenu.addAction(splitAction);
    }

    // Display menu
    contextMenu.exec(m_selectedFilterListWidget->mapToGlobal(pos));
}

//------------------------------------------------------------------------------

bool SFilterSelectionEditor::sortFilters(
    const filter::dicom::IFilter::sptr& a,
    const filter::dicom::IFilter::sptr& b
)
{
    if(a->getFilterType() == b->getFilterType())
    {
        return a->getName() < b->getName();
    }

    return a->getFilterType() > b->getFilterType();
}

//------------------------------------------------------------------------------

QIcon SFilterSelectionEditor::getFilterIcon(filter::dicom::IFilter::sptr filter)
{
    const std::filesystem::path path = core::runtime::getModuleResourcePath(std::string("sight::module::ui::icons"));
    QIcon icons[]                    = {
        QIcon(QString::fromStdString((path / "Modifier.svg").string())),
        QIcon(QString::fromStdString((path / "Sorter.svg").string())),
        QIcon(QString::fromStdString((path / "Splitter.svg").string())),
        QIcon(QString::fromStdString((path / "Composite.svg").string())),
        QIcon(QString::fromStdString((path / "Custom.svg").string()))
    };
    return icons[filter->getFilterType()];
}

//------------------------------------------------------------------------------

std::string SFilterSelectionEditor::getFilterDescription(filter::dicom::IFilter::sptr filter)
{
    std::string types[]     = {"Modifier", "Sorter", "Splitter", "Composite", "Custom"};
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
