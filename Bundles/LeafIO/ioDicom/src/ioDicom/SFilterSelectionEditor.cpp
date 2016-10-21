/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioDicom/SFilterSelectionEditor.hpp"

#include <fwData/Vector.hpp>

#include <fwDicomIOFilter/composite/IComposite.hpp>
#include <fwDicomIOFilter/exceptions/FilterFailure.hpp>
#include <fwDicomIOFilter/helper/Filter.hpp>
#include <fwDicomIOFilter/registry/detail.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QSignalMapper>
#include <QVBoxLayout>

#include <boost/foreach.hpp>

namespace ioDicom
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::ioDicom::SFilterSelectionEditor, ::fwData::Vector );

//------------------------------------------------------------------------------

SFilterSelectionEditor::SFilterSelectionEditor() throw()
{
}
//------------------------------------------------------------------------------

SFilterSelectionEditor::~SFilterSelectionEditor() throw()
{
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::info(std::ostream& _sstream )
{
    _sstream << "SFilterSelectionEditor::info";
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::configuring() throw(::fwTools::Failed)
{
    ::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Get Destination SeriesDB
    m_destinationSeriesDB = this->getInOut< ::fwMedData::SeriesDB>("target");
    SLM_ASSERT("The SeriesDB \"" + m_destinationSeriesDBID + "\" doesn't exist.", m_destinationSeriesDB);

    ::fwData::Vector::csptr dataVector = this->getInput< ::fwData::Vector >("selection");
    SLM_ASSERT("Vector object should not be null.", dataVector);

    ::fwGui::IGuiContainerSrv::create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QWidget* const container = qtContainer->getQtContainer();
    QVBoxLayout* mainLayout  = new QVBoxLayout();
    mainLayout->setAlignment(Qt::AlignTop);
    container->setLayout(mainLayout);

    // Size policy
    QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Top widget
    QHBoxLayout* topLayout = new QHBoxLayout();
    QWidget* topWidget     = new QWidget();
    topWidget->setLayout(topLayout);
    topLayout->setContentsMargins(QMargins(0,0,0,0));
    mainLayout->addWidget(topWidget);

    // Available filter list
    m_availableFilterListWidget = new QComboBox();
    this->fillAvailableFilters();
    topLayout->addWidget(m_availableFilterListWidget);

    // Add filter button
    m_addFilterButton = new QPushButton(QIcon(QString(BUNDLE_PREFIX) + "/media_0-1/icons/Plus.svg"), "Add");
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
    applyLayout->setContentsMargins(QMargins(0,0,0,0));
    m_forcedApplyCheckBox = new QCheckBox("Ignore errors");
    applyLayout->addWidget(m_forcedApplyCheckBox);
    mainLayout->addWidget(applyWidget, 0, Qt::AlignRight);

    // Bottom widget
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QWidget* bottomWidget     = new QWidget();
    bottomWidget->setLayout(bottomLayout);
    bottomLayout->setContentsMargins(QMargins(0,0,0,0));
    mainLayout->addWidget(bottomWidget);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QWidget* buttonWidget     = new QWidget();
    buttonWidget->setLayout(buttonLayout);
    buttonWidget->setSizePolicy(policy);
    buttonLayout->setContentsMargins(QMargins(0,0,0,0));

    // Apply filters button
    m_applyFiltersButton = new QPushButton(QIcon(QString(BUNDLE_PREFIX) +"/media_0-1/icons/Apply.svg"),
                                           "Apply");
    m_applyFiltersButton->setSizePolicy(policy);
    buttonLayout->addWidget(m_applyFiltersButton);

    // Configure filter button
    m_configureFilterButton = new QPushButton(QIcon(QString(BUNDLE_PREFIX) +"/media_0-1/icons/Settings.svg"),
                                              "Configure");
    m_configureFilterButton->setSizePolicy(policy);
    m_configureFilterButton->setEnabled(false);
    buttonLayout->addWidget(m_configureFilterButton);

    // Split filter button
    m_splitFilterButton = new QPushButton(QIcon(QString(BUNDLE_PREFIX) +"/media_0-1/icons/Split.svg"), "Split");
    m_splitFilterButton->setSizePolicy(policy);
    m_splitFilterButton->setEnabled(false);
    buttonLayout->addWidget(m_splitFilterButton);

    // Remove filter button
    m_removeFilterButton = new QPushButton(QIcon(QString(BUNDLE_PREFIX) + "/media_0-1/icons/Minus.svg"),
                                           "Remove");
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
    QObject::connect(m_selectedFilterListWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this,
                     SLOT(showContextMenuForSelectedFilter(const QPoint &)));
    QObject::connect(m_forcedApplyCheckBox, SIGNAL(stateChanged(int)), this, SLOT(onForceChecked(int)));
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::onForceChecked(int state)
{
    if(state == Qt::Checked)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Be careful",
            "You asked to ignore reading errors, there are high risks of issues for resulting image.",
            ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::fillAvailableFilters()
{
    unsigned int index = 0;


    std::vector< ::fwDicomIOFilter::IFilter::sptr > sortedFilters;
    for(const std::string& key: ::fwDicomIOFilter::registry::get()->getFactoryKeys())
    {
        ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(key);
        sortedFilters.push_back(filter);
    }

    std::sort(sortedFilters.begin(), sortedFilters.end(), SFilterSelectionEditor::sortFilters);

    for(const ::fwDicomIOFilter::IFilter::sptr& filter: sortedFilters)
    {
        // If the filter doesn't have a configuration or if it is configurable using GUI
        if(!filter->isConfigurationRequired() || filter->isConfigurableWithGUI())
        {
            // Create filter
            m_availableFilterListWidget->addItem(filter->getName().c_str(), filter->getClassname().c_str());

            // Set icon
            m_availableFilterListWidget->setItemIcon(index, SFilterSelectionEditor::getFilterIcon(filter));

            // Set description
            m_availableFilterListWidget->setItemData(index,
                                                     SFilterSelectionEditor::getFilterDescription(
                                                         filter).c_str(), Qt::ToolTipRole);

            ++index;
        }
    }
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // Disconnect the signals
    QObject::disconnect(m_selectedFilterListWidget, SIGNAL(currentRowChanged(int)), this,
                        SLOT(updateButtonStatus(int)));
    QObject::disconnect(m_addFilterButton, SIGNAL(clicked(void)), this, SLOT(addFilterAtTheEnd(void)));
    QObject::disconnect(m_removeFilterButton, SIGNAL(clicked(void)), this, SLOT(removeFilter(void)));
    QObject::disconnect(m_configureFilterButton, SIGNAL(clicked(void)), this, SLOT(configureFilter(void)));
    QObject::disconnect(m_splitFilterButton, SIGNAL(clicked(void)), this, SLOT(splitFilter(void)));
    QObject::disconnect(m_applyFiltersButton, SIGNAL(clicked(void)), this, SLOT(applyFilters(void)));
    QObject::disconnect(m_deleteShortcut, SIGNAL(activated()), this, SLOT(removeFilter(void)));
    QObject::disconnect(m_selectedFilterListWidget, SIGNAL(customContextMenuRequested(const QPoint &)), this,
                        SLOT(showContextMenuForSelectedFilter(const QPoint &)));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SFilterSelectionEditor::addFilter(int filterTypeIndex)
{
    int index = m_selectedFilterListWidget->currentRow()+1;
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
    ::fwDicomIOFilter::IFilter::sptr filter = ::fwDicomIOFilter::factory::New(key);
    std::string id = filter->getID();
    m_filtersMap[id] = filter;

    // Add a the filter in the list
    m_selectedFilterListWidget->insertItem(position, name);
    m_selectedFilterListWidget->item(position)->setIcon(icon);
    m_selectedFilterListWidget->item(position)->setToolTip(description);
    m_selectedFilterListWidget->item(position)->setData(Qt::UserRole, id.c_str());
    m_selectedFilterListWidget->setCurrentRow(position);

    // Update apply button
    m_applyFiltersButton->setEnabled(m_selectedFilterListWidget->count()>0);
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
    ::fwDicomIOFilter::composite::IComposite::sptr composite =
        ::fwDicomIOFilter::composite::IComposite::dynamicCast(m_filtersMap[compositeId]);

    // Remove composite filter
    this->removeFilter();

    // Add filters
    int position = currentIndex;
    for(const ::fwDicomIOFilter::IFilter::sptr& filter: composite->getChildren())
    {
        std::string id = filter->getID();
        m_filtersMap[id] = filter;
        m_selectedFilterListWidget->insertItem(position, filter->getName().c_str());
        m_selectedFilterListWidget->item(position)->setIcon(SFilterSelectionEditor::getFilterIcon(filter));
        m_selectedFilterListWidget->item(position)->setToolTip(
            SFilterSelectionEditor::getFilterDescription(filter).c_str());
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
        m_splitFilterButton->setEnabled(m_filtersMap[id]->getFilterType() == ::fwDicomIOFilter::IFilter::COMPOSITE);
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
    typedef std::vector< ::fwMedData::DicomSeries::sptr > DicomSeriesContainertype;
    typedef std::vector< ::fwDicomIOFilter::IFilter::sptr > FilterContainertype;

    // Get selected DicomSeries
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector >();

    // Display the informations
    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.setTitle("Filters information");

    // Clear destination SeriesDB
    ::fwMedDataTools::helper::SeriesDB sDBhelper(m_destinationSeriesDB);
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
        for(const ::fwData::Object::sptr& obj: vector->getContainer())
        {
            ::fwMedData::DicomSeries::sptr srcDicomSeries = ::fwMedData::DicomSeries::dynamicCast(obj);
            SLM_ASSERT("The series should be a DicomSeries.", srcDicomSeries);

            ::fwMedData::DicomSeries::sptr dicomSeries = ::fwMedData::DicomSeries::New();
            dicomSeries->deepCopy(srcDicomSeries);
            dicomSeriesContainer.push_back(dicomSeries);
        }

        // Create filter vector
        for(int i = 0; i < m_selectedFilterListWidget->count(); ++i)
        {
            std::string id = m_selectedFilterListWidget->item(i)->data(Qt::UserRole).toString().toStdString();
            filterContainer.push_back(m_filtersMap[id]);
        }

        std::stringstream ssFilters;
        std::stringstream ssInfos;
        bool forcedApply = m_forcedApplyCheckBox->isChecked();

        ssFilters << "<b>Filters :</b><br />";
        // Let's apply all the filters
        for(const ::fwDicomIOFilter::IFilter::sptr& filter: filterContainer)
        {
            ssFilters << "- " << filter->getName() << " -> ";
            try
            {
                ::fwDicomIOFilter::helper::Filter::applyFilter(dicomSeriesContainer, filter, forcedApply);
                ssFilters << "<font color=\"Green\">OK</font><br />";
            }
            catch(::fwDicomIOFilter::exceptions::FilterFailure& e)
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
            for(const ::fwMedData::DicomSeries::sptr& series: dicomSeriesContainer)
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
    QPointer< QSignalMapper > mapper = new QSignalMapper();

    // Fill the menu with the available filters
    for(int i = 0; i < m_availableFilterListWidget->count(); ++i)
    {
        QString text               = m_availableFilterListWidget->itemText(i);
        QIcon icon                 = m_availableFilterListWidget->itemIcon(i);
        QPointer< QAction > action = new QAction(icon, text, m_selectedFilterListWidget);
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
        std::string id = filterItem->data(Qt::UserRole).toString().toStdString();
        ::fwDicomIOFilter::IFilter::sptr filter = m_filtersMap[id];

        // Remove action
        QPointer< QAction > removeAction = new QAction("Remove", m_selectedFilterListWidget);
        QObject::connect(removeAction, SIGNAL(triggered()), this, SLOT(removeFilter()));
        contextMenu.addAction(removeAction);

        // Configure action
        QPointer< QAction > configureAction = new QAction("Configure", m_selectedFilterListWidget);
        configureAction->setEnabled(filter->isConfigurableWithGUI());
        QObject::connect(configureAction, SIGNAL(triggered()), this, SLOT(configureFilter()));
        contextMenu.addAction(configureAction);

        // Split action
        QPointer< QAction > splitAction = new QAction("Split", m_selectedFilterListWidget);
        splitAction->setEnabled(filter->getFilterType() == ::fwDicomIOFilter::IFilter::COMPOSITE);
        QObject::connect(splitAction, SIGNAL(triggered()), this, SLOT(splitFilter()));
        contextMenu.addAction(splitAction);

    }

    // Display menu
    contextMenu.exec(m_selectedFilterListWidget->mapToGlobal(pos));

}

//------------------------------------------------------------------------------

bool SFilterSelectionEditor::sortFilters(const ::fwDicomIOFilter::IFilter::sptr& a,
                                         const ::fwDicomIOFilter::IFilter::sptr& b)
{
    if(a->getFilterType() == b->getFilterType())
    {
        return a->getName() < b->getName();
    }
    return a->getFilterType() > b->getFilterType();
}

//------------------------------------------------------------------------------

QIcon SFilterSelectionEditor::getFilterIcon(::fwDicomIOFilter::IFilter::sptr filter)
{
    QIcon icons[] = {
        QIcon(QString(BUNDLE_PREFIX) + "/media_0-1/icons/Modifier.svg"),
        QIcon(QString(BUNDLE_PREFIX) + "/media_0-1/icons/Sorter.svg"),
        QIcon(QString(BUNDLE_PREFIX) + "/media_0-1/icons/Splitter.svg"),
        QIcon(QString(BUNDLE_PREFIX) + "/media_0-1/icons/Composite.svg"),
        QIcon(QString(BUNDLE_PREFIX) + "/media_0-1/icons/Custom.svg")
    };
    return icons[filter->getFilterType()];
}

//------------------------------------------------------------------------------

std::string SFilterSelectionEditor::getFilterDescription(::fwDicomIOFilter::IFilter::sptr filter)
{
    std::string types[]     = { "Modifier", "Sorter", "Splitter", "Composite", "Custom" };
    std::string description =
        "<b>Name :</b> "+filter->getName()+"<br />"
        "<b>Type :</b> "+types[filter->getFilterType()]+"<br />"
        "<b>Configurable :</b> "+((filter->isConfigurableWithGUI()) ? "Yes" : "No")+"<br />"
        "<b>Informations :</b><br />"+filter->getDescription();
    return description;
}

//------------------------------------------------------------------------------

} // namespace ioDicom
