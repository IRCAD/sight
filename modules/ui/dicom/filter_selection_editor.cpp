/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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
#include <filter/dicom/exceptions/filter_failure.hpp>
#include <filter/dicom/helper/filter.hpp>
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
    SIGHT_ASSERT("The series_set \"" + m_dest_series_set_id + "\" doesn't exist.", dest_series_set);

    const auto selected_dicom_series = m_selected_dicom_series.lock();
    SIGHT_ASSERT("Selected dicom Series vector should not be null.", selected_dicom_series);

    sight::ui::service::create();
    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(get_container());

    auto* main_layout = new QVBoxLayout();
    main_layout->setAlignment(Qt::AlignTop);
    qt_container->set_layout(main_layout);

    // Size policy
    QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    // Top widget
    auto* top_layout = new QHBoxLayout();
    auto* top_widget = new QWidget();
    top_widget->setLayout(top_layout);
    top_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    main_layout->addWidget(top_widget);

    // Available filter list
    m_available_filter_list_widget = new QComboBox();
    this->fill_available_filters();
    top_layout->addWidget(m_available_filter_list_widget);

    auto path = core::runtime::get_module_resource_path("sight::module::ui::icons");

    // Add filter button
    m_add_filter_button = new QPushButton(QIcon(QString::fromStdString((path / "GreenPlus.svg").string())), "Add");
    m_add_filter_button->setSizePolicy(policy);
    top_layout->addWidget(m_add_filter_button);

    // Add selected filters
    m_selected_filter_list_widget = new QListWidget();
    main_layout->addWidget(m_selected_filter_list_widget);
    m_selected_filter_list_widget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_selected_filter_list_widget->setDragEnabled(true);
    m_selected_filter_list_widget->viewport()->setAcceptDrops(true);
    m_selected_filter_list_widget->setDropIndicatorShown(true);
    m_selected_filter_list_widget->setDragDropMode(QAbstractItemView::InternalMove);
    m_selected_filter_list_widget->setContextMenuPolicy(Qt::CustomContextMenu);

    // Add forced apply checkbox
    auto* apply_layout = new QHBoxLayout();
    auto* apply_widget = new QWidget();
    apply_widget->setLayout(apply_layout);
    apply_widget->setSizePolicy(policy);
    apply_layout->setContentsMargins(QMargins(0, 0, 0, 0));
    m_forced_apply_check_box = new QCheckBox("Ignore errors");
    apply_layout->addWidget(m_forced_apply_check_box);
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
    m_apply_filters_button =
        new QPushButton(QIcon(QString::fromStdString((path / "GreenCheck.svg").string())), "Apply");
    m_apply_filters_button->setSizePolicy(policy);
    button_layout->addWidget(m_apply_filters_button);

    // Configure filter button
    m_configure_filter_button = new QPushButton(
        QIcon(QString::fromStdString((path / "gear.svg").string())),
        "Configure"
    );
    m_configure_filter_button->setSizePolicy(policy);
    m_configure_filter_button->setEnabled(false);
    button_layout->addWidget(m_configure_filter_button);

    // Split filter button
    m_split_filter_button = new QPushButton(QIcon(QString::fromStdString((path / "Split.svg").string())), "Split");
    m_split_filter_button->setSizePolicy(policy);
    m_split_filter_button->setEnabled(false);
    button_layout->addWidget(m_split_filter_button);

    // Remove filter button
    m_remove_filter_button =
        new QPushButton(QIcon(QString::fromStdString((path / "RedMinus.svg").string())), "Remove");
    m_remove_filter_button->setSizePolicy(policy);
    m_remove_filter_button->setEnabled(false);
    button_layout->addWidget(m_remove_filter_button);

    bottom_layout->addWidget(button_widget, 0, Qt::AlignRight);

    // Create shortcut
    m_delete_shortcut =
        new QShortcut(
            QKeySequence(Qt::Key_Delete),
            m_selected_filter_list_widget,
            nullptr,
            nullptr,
            Qt::WidgetShortcut
        );

    // Connect the signals
    QObject::connect(
        m_selected_filter_list_widget,
        SIGNAL(currentRowChanged(int)),
        this,
        SLOT(update_button_status(int))
    );
    QObject::connect(m_add_filter_button, &QPushButton::clicked, this, &self_t::add_filter_at_the_end);
    QObject::connect(m_remove_filter_button, &QPushButton::clicked, this, &self_t::remove_filter);
    QObject::connect(m_configure_filter_button, &QPushButton::clicked, this, &self_t::configure_filter);
    QObject::connect(m_split_filter_button, &QPushButton::clicked, this, &self_t::split_filter);
    QObject::connect(m_apply_filters_button, &QPushButton::clicked, this, &self_t::apply_filters);
    QObject::connect(m_delete_shortcut, &QShortcut::activated, this, &self_t::remove_filter);
    QObject::connect(
        m_selected_filter_list_widget,
        SIGNAL(customContextMenuRequested(const QPoint&)),
        this,
        SLOT(show_context_menu_for_selected_filter(const QPoint&))
    );
    QObject::connect(m_forced_apply_check_box, &QCheckBox::stateChanged, this, &self_t::on_force_checked);
}

//------------------------------------------------------------------------------

void filter_selection_editor::on_force_checked(int _state)
{
    if(_state == Qt::Checked)
    {
        sight::ui::dialog::message::show(
            "Be careful",
            "You asked to ignore reading errors, there are high risks of issues for resulting image.",
            sight::ui::dialog::message::warning
        );
    }
}

//------------------------------------------------------------------------------

void filter_selection_editor::fill_available_filters()
{
    int index = 0;

    std::vector<sight::filter::dicom::filter::sptr> sorted_filters;
    for(const std::string& key : sight::filter::dicom::registry::get()->get_factory_keys())
    {
        sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(key);
        sorted_filters.push_back(filter);
    }

    std::sort(sorted_filters.begin(), sorted_filters.end(), filter_selection_editor::sort_filters);

    for(const sight::filter::dicom::filter::sptr& filter : sorted_filters)
    {
        // If the filter doesn't have a configuration or if it is configurable using GUI
        if(!filter->is_configuration_required() || filter->is_configurable_with_gui())
        {
            // Create filter
            m_available_filter_list_widget->addItem(filter->get_name().c_str(), filter->get_classname().c_str());

            // Set icon
            m_available_filter_list_widget->setItemIcon(index, filter_selection_editor::get_filter_icon(filter));

            // Set description
            m_available_filter_list_widget->setItemData(
                index,
                filter_selection_editor::get_filter_description(
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
        m_selected_filter_list_widget,
        SIGNAL(currentRowChanged(int)),
        this,
        SLOT(update_button_status(int))
    );
    QObject::disconnect(m_add_filter_button, &QPushButton::clicked, this, &self_t::add_filter_at_the_end);
    QObject::disconnect(m_remove_filter_button, &QPushButton::clicked, this, &self_t::remove_filter);
    QObject::disconnect(m_configure_filter_button, &QPushButton::clicked, this, &self_t::configure_filter);
    QObject::disconnect(m_split_filter_button, &QPushButton::clicked, this, &self_t::split_filter);
    QObject::disconnect(m_apply_filters_button, &QPushButton::clicked, this, &self_t::apply_filters);
    QObject::disconnect(m_delete_shortcut, &QShortcut::activated, this, &self_t::remove_filter);
    QObject::disconnect(
        m_selected_filter_list_widget,
        SIGNAL(customContextMenuRequested(const QPoint&)),
        this,
        SLOT(show_context_menu_for_selected_filter(const QPoint&))
    );

    this->destroy();
}

//------------------------------------------------------------------------------

void filter_selection_editor::updating()
{
}

//------------------------------------------------------------------------------

void filter_selection_editor::add_filter(int _filter_type_index)
{
    int index = m_selected_filter_list_widget->currentRow() + 1;
    this->add_filter(_filter_type_index, index);
}

//------------------------------------------------------------------------------

void filter_selection_editor::add_filter_at_the_end()
{
    this->add_filter(m_available_filter_list_widget->currentIndex(), m_selected_filter_list_widget->count());
}

//------------------------------------------------------------------------------

void filter_selection_editor::add_filter(int _filter_type_index, int _position)
{
    // Get information from selected filter
    QIcon icon          = m_available_filter_list_widget->itemIcon(_filter_type_index);
    QString name        = m_available_filter_list_widget->itemText(_filter_type_index);
    QString description = m_available_filter_list_widget->itemData(_filter_type_index, Qt::ToolTipRole).toString();
    std::string key     = m_available_filter_list_widget->itemData(_filter_type_index).toString().toStdString();

    // Create filter
    sight::filter::dicom::filter::sptr filter = sight::filter::dicom::factory::make(key);
    std::string id                            = filter->get_id();
    m_filters_map[id] = filter;

    // Add a the filter in the list
    m_selected_filter_list_widget->insertItem(_position, name);
    m_selected_filter_list_widget->item(_position)->setIcon(icon);
    m_selected_filter_list_widget->item(_position)->setToolTip(description);
    m_selected_filter_list_widget->item(_position)->setData(Qt::UserRole, id.c_str());
    m_selected_filter_list_widget->setCurrentRow(_position);

    // Update apply button
    m_apply_filters_button->setEnabled(m_selected_filter_list_widget->count() > 0);
}

//------------------------------------------------------------------------------

void filter_selection_editor::remove_filter()
{
    int current_index = m_selected_filter_list_widget->currentRow();
    if(current_index >= 0)
    {
        // Remove selected filter from map
        std::string id =
            m_selected_filter_list_widget->item(current_index)->data(Qt::UserRole).toString().toStdString();
        m_filters_map.erase(id);

        // Remove selected filter from widget
        m_selected_filter_list_widget->takeItem(current_index);
    }
}

//------------------------------------------------------------------------------

void filter_selection_editor::configure_filter()
{
    int selected_filter_index = m_selected_filter_list_widget->currentRow();
    std::string id            =
        m_selected_filter_list_widget->item(selected_filter_index)->data(Qt::UserRole).toString().toStdString();
    m_filters_map[id]->configure_with_gui();
}

//------------------------------------------------------------------------------

void filter_selection_editor::split_filter()
{
    int current_index        = m_selected_filter_list_widget->currentRow();
    std::string composite_id =
        m_selected_filter_list_widget->item(current_index)->data(Qt::UserRole).toString().toStdString();
    sight::filter::dicom::composite::base::sptr composite =
        std::dynamic_pointer_cast<sight::filter::dicom::composite::base>(m_filters_map[composite_id]);

    // Remove composite filter
    this->remove_filter();

    // Add filters
    int position = current_index;
    for(const sight::filter::dicom::filter::sptr& filter : composite->get_children())
    {
        std::string id = filter->get_id();
        m_filters_map[id] = filter;
        m_selected_filter_list_widget->insertItem(position, filter->get_name().c_str());
        m_selected_filter_list_widget->item(position)->setIcon(filter_selection_editor::get_filter_icon(filter));
        m_selected_filter_list_widget->item(position)->setToolTip(
            filter_selection_editor::get_filter_description(filter).c_str()
        );
        m_selected_filter_list_widget->item(position)->setData(Qt::UserRole, id.c_str());
        ++position;
    }

    m_selected_filter_list_widget->setCurrentRow(current_index);
}

//------------------------------------------------------------------------------

void filter_selection_editor::update_button_status(int _filter_index)
{
    bool has_filter = (_filter_index != -1);
    m_remove_filter_button->setEnabled(has_filter);
    if(has_filter)
    {
        std::string id =
            m_selected_filter_list_widget->item(_filter_index)->data(Qt::UserRole).toString().toStdString();

        // Configure filter button
        m_configure_filter_button->setEnabled(m_filters_map[id]->is_configurable_with_gui());

        // Split filter button
        m_split_filter_button->setEnabled(
            m_filters_map[id]->get_filter_type() == sight::filter::dicom::filter::composite
        );
    }
    else
    {
        m_configure_filter_button->setEnabled(false);
        m_split_filter_button->setEnabled(false);
    }
}

//------------------------------------------------------------------------------

void filter_selection_editor::apply_filters()
{
    using dicom_series_containertype = std::vector<data::dicom_series::sptr>;
    using filter_containertype       = std::vector<sight::filter::dicom::filter::sptr>;

    // Get selected DicomSeries
    const auto selected_dicom_series = m_selected_dicom_series.lock();
    SIGHT_ASSERT("Selected dicom Series vector  object should not be null.", selected_dicom_series);

    // Display the informations
    sight::ui::dialog::message message_box;
    message_box.set_icon(sight::ui::dialog::message::info);
    message_box.add_button(sight::ui::dialog::message::ok);
    message_box.set_title("Filters information");

    // Clear destination series_set
    const auto dest_series_set = m_dest_series_set.lock();
    SIGHT_ASSERT("The series_set \"" + m_dest_series_set_id + "\" doesn't exist.", dest_series_set);

    const auto scoped_emitter = dest_series_set->scoped_emit();
    dest_series_set->clear();

    // Be sure series are selected
    if(selected_dicom_series->empty())
    {
        message_box.set_message("You must select series on which you want to apply your filters.");
    }
    else if(m_selected_filter_list_widget->count() == 0)
    {
        message_box.set_message("You must select the filters that you want to apply on your series.");
    }
    else
    {
        // Create containers
        dicom_series_containertype dicom_series_container;
        filter_containertype filter_container;

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
        for(int i = 0 ; i < m_selected_filter_list_widget->count() ; ++i)
        {
            std::string id = m_selected_filter_list_widget->item(i)->data(Qt::UserRole).toString().toStdString();
            filter_container.push_back(m_filters_map[id]);
        }

        std::stringstream ss_filters;
        std::stringstream ss_infos;
        bool forced_apply = m_forced_apply_check_box->isChecked();

        ss_filters << "<b>Filters :</b><br />";
        // Let's apply all the filters
        for(const sight::filter::dicom::filter::sptr& filter : filter_container)
        {
            ss_filters << "- " << filter->get_name() << " -> ";
            try
            {
                sight::filter::dicom::helper::filter::apply_filter(dicom_series_container, filter, forced_apply);
                ss_filters << "<font color=\"Green\">OK</font><br />";
            }
            catch(sight::filter::dicom::exceptions::filter_failure& e)
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
        message_box.set_message(msg);
    }

    // Diplay message
    message_box.show();
}

//------------------------------------------------------------------------------

void filter_selection_editor::show_context_menu_for_selected_filter(const QPoint& _pos)
{
    // Create context menu
    QMenu context_menu("Context menu", m_selected_filter_list_widget);

    // Add menu
    QMenu* add_menu = context_menu.addMenu("Add");

    // Fill the menu with the available filters
    for(int i = 0 ; i < m_available_filter_list_widget->count() ; ++i)
    {
        QString text = m_available_filter_list_widget->itemText(i);
        QIcon icon   = m_available_filter_list_widget->itemIcon(i);
        auto* action = new QAction(icon, text, m_selected_filter_list_widget);
        action->setIconVisibleInMenu(true);
        add_menu->addAction(action);

        QObject::connect(action, &QAction::triggered, [this, i]{add_filter(i);});
    }

    // Check id the menu is requested from a filter
    QListWidgetItem* filter_item = m_selected_filter_list_widget->itemAt(_pos);
    if(filter_item != nullptr)
    {
        // Get filter
        std::string id                            = filter_item->data(Qt::UserRole).toString().toStdString();
        sight::filter::dicom::filter::sptr filter = m_filters_map[id];

        // Remove action
        auto* remove_action = new QAction("Remove", m_selected_filter_list_widget);
        QObject::connect(remove_action, &QAction::triggered, this, &filter_selection_editor::remove_filter);
        context_menu.addAction(remove_action);

        // Configure action
        auto* configure_action = new QAction("Configure", m_selected_filter_list_widget);
        configure_action->setEnabled(filter->is_configurable_with_gui());
        QObject::connect(configure_action, &QAction::triggered, this, &filter_selection_editor::remove_filter);
        context_menu.addAction(configure_action);

        // Split action
        auto* split_action = new QAction("Split", m_selected_filter_list_widget);
        split_action->setEnabled(filter->get_filter_type() == sight::filter::dicom::filter::composite);
        QObject::connect(split_action, &QAction::triggered, this, &filter_selection_editor::remove_filter);
        context_menu.addAction(split_action);
    }

    // Display menu
    context_menu.exec(m_selected_filter_list_widget->mapToGlobal(_pos));
}

//------------------------------------------------------------------------------

bool filter_selection_editor::sort_filters(
    const sight::filter::dicom::filter::sptr& _a,
    const sight::filter::dicom::filter::sptr& _b
)
{
    if(_a->get_filter_type() == _b->get_filter_type())
    {
        return _a->get_name() < _b->get_name();
    }

    return _a->get_filter_type() > _b->get_filter_type();
}

//------------------------------------------------------------------------------

QIcon filter_selection_editor::get_filter_icon(sight::filter::dicom::filter::sptr _filter)
{
    const std::filesystem::path path = core::runtime::get_module_resource_path(std::string("sight::module::ui::icons"));
    std::array icons {
        QIcon(QString::fromStdString((path / "OrangeLoop.svg").string())),
        QIcon(QString::fromStdString((path / "Sorter.svg").string())),
        QIcon(QString::fromStdString((path / "Splitter.svg").string())),
        QIcon(QString::fromStdString((path / "Composite.svg").string())),
        QIcon(QString::fromStdString((path / "Custom.svg").string()))
    };
    return icons[_filter->get_filter_type()];
}

//------------------------------------------------------------------------------

std::string filter_selection_editor::get_filter_description(sight::filter::dicom::filter::sptr _filter)
{
    std::array types {"Modifier", "Sorter", "Splitter", "Composite", "Custom"};
    std::string description =
        "<b>Name :</b> " + _filter->get_name() + "<br />"
                                                 "<b>Type :</b> " + types[_filter->get_filter_type()] + "<br />"
                                                                                                        "<b>Configurable :</b> "
        + ((_filter->is_configurable_with_gui()) ? "true" : "false") + "<br />"
                                                                       "<b>Informations :</b><br />"
        + _filter->get_description();
    return description;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::dicom
