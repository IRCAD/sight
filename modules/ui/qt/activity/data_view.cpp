/************************************************************************
 *
 * Copyright (C) 2016-2024 IRCAD France
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

#include "data_view.hpp"

#include <activity/validator/activity.hpp>
#include <activity/validator/base.hpp>
#include <activity/validator/object.hpp>

#include <core/runtime/helper.hpp>

#include <data/boolean.hpp>
#include <data/image_series.hpp>
#include <data/integer.hpp>
#include <data/map.hpp>
#include <data/matrix4.hpp>
#include <data/real.hpp>
#include <data/series.hpp>
#include <data/series_set.hpp>
#include <data/string.hpp>
#include <data/vector.hpp>

#include <io/__/service/io_types.hpp>

#include <service/base.hpp>
#include <service/extension/config.hpp>
#include <service/op.hpp>
#include <service/registry.hpp>

#include <QApplication>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QMimeData>
#include <QPixmap>
#include <QPushButton>
#include <QStandardItem>
#include <QTableWidget>
#include <QVBoxLayout>

#include <algorithm>
#include <iomanip>

namespace sight::module::ui::qt::activity
{

const int data_view::UID_ROLE = Qt::UserRole + 1;

using sight::activity::extension::activity_info;
using sight::activity::extension::activity_requirement;

//-----------------------------------------------------------------------------

data_view::data_view(QWidget* _parent) :
    QTabWidget(_parent)
{
}

//-----------------------------------------------------------------------------

data_view::~data_view()
= default;

//-----------------------------------------------------------------------------

void data_view::clear()
{
    m_imported_object.clear();
    m_tree_widgets.clear();
    QTabWidget::clear();
}

//-----------------------------------------------------------------------------

bool data_view::eventFilter(QObject* _obj, QEvent* _event)
{
    // get dropped data in tree widget
    if(_event->type() == QEvent::Drop)
    {
        auto* drop_event = static_cast<QDropEvent*>(_event);

        auto index                       = static_cast<std::size_t>(this->currentIndex());
        activity_requirement requirement = m_activity_info.requirements[index];
        QPointer<QTreeWidget> tree       = m_tree_widgets[index];

        // get dropped item from event mimedata
        const QMimeData* q_mime_data = drop_event->mimeData();

        QByteArray encoded = q_mime_data->data("application/x-qabstractitemmodeldatalist");
        QDataStream stream(&encoded, QIODevice::ReadOnly);

        QList<QTreeWidgetItem*> item_list;
        QTreeWidgetItem* item = nullptr;

        // Get the dropped item
        while(!stream.atEnd())
        {
            int row = 0;
            int col = 0;

            QMap<int, QVariant> role_data_map;

            stream >> row >> col >> role_data_map;

            if(col == 0)
            {
                item = new QTreeWidgetItem();
                item_list.push_back(item);
            }

            SIGHT_ASSERT("item is null", item != nullptr);

            QList<int> keys = role_data_map.keys();
            for(int key : keys)
            {
                item->setData(col, key, role_data_map[key]);
            }
        }

        // check if the limit number of data is exceeded
        int nb_child = tree->topLevelItemCount() + item_list.size();
        if(static_cast<unsigned int>(nb_child) > requirement.max_occurs)
        {
            QMessageBox::warning(
                this,
                "Drop",
                "The maximum number of element is reached.\n"
                "You must remove one before adding another."
            );
        }
        else
        {
            // add the dropped item in the tree
            for(QTreeWidgetItem* item_to_add : item_list)
            {
                item_to_add->setFlags(item_to_add->flags() & ~Qt::ItemIsDropEnabled);
                std::string uid = item_to_add->data(int(column_commun_t::id), UID_ROLE).toString().toStdString();
                if(!uid.empty())
                {
                    // insert the object if it is in the required type
                    data::object::sptr obj = std::dynamic_pointer_cast<data::object>(core::id::get_object(uid));
                    if(obj && obj->is_a(requirement.type))
                    {
                        // Insert the new object
                        tree->addTopLevelItem(item_to_add);
                    }
                }
            }
        }

        return true;
    }

    if(_event->type() == QEvent::KeyPress)
    {
        auto* key_event = static_cast<QKeyEvent*>(_event);
        if(key_event->key() == Qt::Key_Delete)
        {
            this->remove_selected_objects();
            return true;
        }
    }

    // standard event processing
    return QObject::eventFilter(_obj, _event);
}

//-----------------------------------------------------------------------------

void data_view::fill_information(const activity_info& _info)
{
    m_activity_info = _info;
    this->clear();

    const activity_info::requirements_t& req_vect = m_activity_info.requirements;
    for(const auto& req : req_vect)
    {
        auto* const layout = new QVBoxLayout();
        auto* const widget = new QWidget();
        widget->setLayout(layout);

        auto* const info_layout = new QHBoxLayout();
        layout->addLayout(info_layout);

        auto* const type_layout = new QVBoxLayout();
        auto* const txt_layout  = new QVBoxLayout();
        info_layout->addLayout(type_layout);
        info_layout->addSpacerItem(new QSpacerItem(20, 0));
        info_layout->addLayout(txt_layout, 1);

        auto iter = m_object_icons.find(req.type);
        if(iter != m_object_icons.end())
        {
            const QString filename = QString::fromStdString(iter->second);

            this->addTab(widget, QIcon(filename), QString::fromStdString(req.name));
            auto* const icon = new QLabel();
            icon->setAlignment(Qt::AlignHCenter);
            QPixmap pixmap(filename);
            icon->setPixmap(pixmap.scaled(100, 100, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            type_layout->addWidget(icon);
        }
        else
        {
            this->addTab(widget, QString::fromStdString(req.name));
        }

        auto* const type = new QLabel(QString("<small>%1</small>").arg(QString::fromStdString(req.type)));
        type->setAlignment(Qt::AlignHCenter);
        type_layout->addWidget(type);

        auto* const name = new QLabel(QString("<h2>%1</h2>").arg(QString::fromStdString(req.name)));
        name->setStyleSheet("QLabel { font: bold; }");
        txt_layout->addWidget(name);

        auto* const description = new QLabel(QString::fromStdString(req.description));
        description->setStyleSheet("QLabel { font: italic; }");
        txt_layout->addWidget(description);

        txt_layout->addStretch();

        auto* const nb = new QLabel();
        nb->setStyleSheet("QLabel { font: bold; }");
        layout->addWidget(nb);

        QPointer<QTreeWidget> tree = new QTreeWidget();
        m_tree_widgets.push_back(tree);
        if(req.max_occurs == 0)
        {
            nb->setText("No object is required, it will be automatically created.");
            tree->setEnabled(false);
        }
        else if(req.min_occurs == 1 && req.max_occurs == 1)
        {
            nb->setText("One object is required: ");
        }
        else
        {
            QString nb_obj("Number of required object");
            if(req.max_occurs == std::numeric_limits<unsigned int>::max())
            {
                nb_obj.append(QString(" >= %1").arg(req.min_occurs));
            }
            else
            {
                nb_obj.append(QString(": [%1-%2]").arg(req.min_occurs).arg(req.max_occurs));
            }

            nb->setText(nb_obj);
        }

        QStringList headers;
        headers << "" << "Description";

        auto* const tree_layout   = new QHBoxLayout();
        auto* const button_layout = new QVBoxLayout();
        if(req.type == "sight::data::string" || req.type == "sight::data::boolean"
           || req.type == "sight::data::integer" || req.type == "sight::data::real"
           || req.type == "sight::data::matrix4")
        {
            auto* const button_new = new QPushButton("New");
            button_new->setToolTip("Create a new empty object");
            button_layout->addWidget(button_new);
            QObject::connect(button_new, &QPushButton::clicked, this, &data_view::create_new_object);
        }

        auto* const button_add    = new QPushButton("Load");
        auto* const button_remove = new QPushButton("Remove");
        auto* const button_clear  = new QPushButton("Clear");
        button_layout->addWidget(button_add);
        button_add->setToolTip(QString("Load an object of type '%1'.").arg(QString::fromStdString(req.type)));

        // If the type is a Series, we add a button to import the data from a series_set,
        // we also improve the tree header by adding more informations.
        data::object::sptr new_object = data::factory::make(req.type);
        if(new_object && std::dynamic_pointer_cast<data::series>(new_object))
        {
            auto* const button_add_from_sdb = new QPushButton("Import");
            button_layout->addWidget(button_add_from_sdb);
            button_add_from_sdb->setToolTip(
                QString(
                    "Import a series_set and extract the N first objects of type '%1', with "
                    "N the maximum number of required objects."
                ).
                arg(QString::fromStdString(req.type))
            );
            QObject::connect(button_add_from_sdb, &QPushButton::clicked, this, &data_view::import_object_from_sdb);

            headers.clear();
            headers << "" << "Name" << "Sex" << "Birthdate"
            << "Modality" << "Description"
            << "Study description" << "Date" << "Time"
            << "Patient age";

            if(std::dynamic_pointer_cast<data::image_series>(new_object))
            {
                headers << "Body part examined" << "Patient position" << "Contrast agent" << "Acquisition time"
                << "Contrast/bolus time";
            }
        }

        button_layout->addWidget(button_remove);
        button_remove->setToolTip(QString("Remove the selected objects"));
        button_layout->addWidget(button_clear);
        button_clear->setToolTip(QString("Remove all the objects"));
        button_layout->addStretch();
        QObject::connect(button_add, &QPushButton::clicked, this, &data_view::import_object);
        QObject::connect(button_remove, &QPushButton::clicked, this, &data_view::remove_selected_objects);
        QObject::connect(button_clear, &QPushButton::clicked, this, &data_view::clear_tree);

        tree_layout->addLayout(button_layout);
        tree_layout->addWidget(tree, 1);

        tree->setHeaderLabels(headers);
        tree->setAlternatingRowColors(true);
        tree->setAcceptDrops(true);
        tree->setDragDropMode(QAbstractItemView::DropOnly);
        tree->viewport()->installEventFilter(this);
        tree->installEventFilter(this);

        QObject::connect(
            tree.data(),
            &QTreeWidget::itemDoubleClicked,
            this,
            &data_view::on_tree_item_double_clicked
        );
        layout->addLayout(tree_layout, 1);

        //TODO better management of map container
    }

    for(int i = 1 ; i < this->count() ; ++i)
    {
        this->setTabEnabled(i, false);
    }
}

//-----------------------------------------------------------------------------

void data_view::fill_information(const data::activity::sptr& _activity)
{
    activity_info info = sight::activity::extension::activity::get_default()->get_info(
        _activity->get_activity_config_id()
    );
    m_activity_info = info;

    this->fill_information(info);

    for(std::size_t i = 0 ; i < m_activity_info.requirements.size() ; ++i)
    {
        activity_requirement req = m_activity_info.requirements[i];

        auto obj = _activity->get(req.name);
        if(obj)
        {
            if((req.min_occurs == 0 && req.max_occurs == 0)
               || (req.min_occurs == 1 && req.max_occurs == 1)
               || req.create)
            {
                this->add_object_item(i, obj);
            }
            else
            {
                if(req.container == "vector")
                {
                    data::vector::sptr vector = std::dynamic_pointer_cast<data::vector>(obj);
                    if(vector)
                    {
                        for(const auto& sub_obj : *vector)
                        {
                            this->add_object_item(i, sub_obj);
                        }
                    }
                    else
                    {
                        SIGHT_ERROR("Object param '" + req.name + "' must be a 'data::vector'");
                    }
                }
                else // container == map
                {
                    data::map::sptr map = std::dynamic_pointer_cast<data::map>(obj);
                    if(map)
                    {
                        for(const auto& sub_obj : *map)
                        {
                            this->add_object_item(i, sub_obj.second);
                        }
                    }
                    else
                    {
                        SIGHT_ERROR("Object param '" + req.name + "' must be a 'data::map'");
                    }
                }
            }
        }

        this->setTabEnabled(int(i), true);
    }
}

//-----------------------------------------------------------------------------

data::object::sptr data_view::check_data(std::size_t _index, std::string& _error_msg)
{
    data::object::sptr object;

    activity_requirement req   = m_activity_info.requirements[_index];
    QPointer<QTreeWidget> tree = m_tree_widgets[_index];

    bool ok = true;
    if((req.min_occurs == 1 && req.max_occurs == 1)
       || (req.min_occurs == 0 && req.max_occurs == 0)
       || req.create) // One object is required
    {
        if(tree->topLevelItemCount() == 1)
        {
            QTreeWidgetItem* item = tree->topLevelItem(0);

            std::string uid =
                item->data(int(column_commun_t::id), data_view::UID_ROLE).toString().toStdString();

            data::object::sptr obj = std::dynamic_pointer_cast<data::object>(core::id::get_object(uid));
            if(obj && obj->is_a(req.type))
            {
                object = obj;
            }
            else
            {
                _error_msg += "\n - The parameter '" + req.name + "' must be a '" + req.type + "'.";
            }
        }
        else
        {
            if((req.min_occurs == 0 && req.max_occurs == 0) || req.create)
            {
                object = data::factory::make(req.type);
            }
            else
            {
                _error_msg += "\n - The parameter '" + req.name + "' is required but is not defined.";
            }
        }
    }
    else // optional object or several objects
    {
        auto nb_obj = static_cast<unsigned int>(tree->topLevelItemCount());

        if(nb_obj < req.min_occurs)
        {
            _error_msg += "\n - The parameter '" + req.name + "' must contain at least "
                          + std::to_string(req.min_occurs) + " elements.";
        }
        else if(nb_obj > req.max_occurs)
        {
            _error_msg += "\n - The parameter '" + req.name + "' must contain at most "
                          + std::to_string(req.max_occurs) + " elements.";
        }
        else
        {
            if(req.container == "vector")
            {
                data::vector::sptr vector = std::make_shared<data::vector>();

                for(unsigned int i = 0 ; i < nb_obj ; ++i)
                {
                    QTreeWidgetItem* item_data = tree->topLevelItem(int(i));
                    std::string uid            =
                        item_data->data(int(column_commun_t::id), UID_ROLE).toString().toStdString();

                    data::object::sptr obj = std::dynamic_pointer_cast<data::object>(core::id::get_object(uid));
                    if(obj && obj->is_a(req.type))
                    {
                        vector->push_back(obj);
                    }
                    else
                    {
                        ok          = false;
                        _error_msg += "\n - The parameter '" + req.name + "' must be a " + req.type + ".";
                    }
                }

                if(ok)
                {
                    object = vector;
                }
            }
            else // container == map
            {
                data::map::sptr map = std::make_shared<data::map>();

                for(unsigned int i = 0 ; i < nb_obj ; ++i)
                {
                    QTreeWidgetItem* item_data = tree->topLevelItem(int(i));
                    std::string uid            =
                        item_data->data(int(column_commun_t::id), UID_ROLE).toString().toStdString();

                    data::object::sptr obj = std::dynamic_pointer_cast<data::object>(core::id::get_object(uid));
                    if(obj && obj->is_a(req.type))
                    {
                        std::string key = req.keys[i].key;
                        (*map)[key] = obj;
                    }
                    else
                    {
                        ok          = false;
                        _error_msg += "\n - The parameter '" + req.name + "' must be a " + req.type + ".";
                    }
                }

                if(ok)
                {
                    object = map;
                }
            }
        }
    }

    if(object && !req.validator.empty())
    {
        /// Process object validator
        auto validator      = sight::activity::validator::factory::make(req.validator);
        auto data_validator = std::dynamic_pointer_cast<sight::activity::validator::object>(validator);
        SIGHT_ASSERT("Validator '" + req.validator + "' instantiation failed", data_validator);

        sight::activity::validator::return_t validation = data_validator->validate(object);
        if(!validation.first)
        {
            _error_msg += "\n" + validation.second;
            object      = nullptr;
        }
    }

    return object;
}

//-----------------------------------------------------------------------------

bool data_view::check_and_compute_data(const data::activity::sptr& _activity, std::string& _error_msg)
{
    bool ok = true;
    _error_msg += "The required data are not correct:";

    // Check if all required data are present
    for(std::size_t i = 0 ; i < m_activity_info.requirements.size() ; ++i)
    {
        activity_requirement req = m_activity_info.requirements[i];
        std::string msg;
        data::object::sptr obj = this->check_data(i, msg);
        if(obj)
        {
            (*_activity)[req.name] = obj;
        }
        else
        {
            ok          = false;
            _error_msg += msg;
        }
    }

    for(const std::string& validatot_impl : m_activity_info.validators_impl)
    {
        /// Process activity validator
        auto validator          = sight::activity::validator::factory::make(validatot_impl);
        auto activity_validator = std::dynamic_pointer_cast<sight::activity::validator::activity>(validator);

        SIGHT_ASSERT("Validator '" + validatot_impl + "' instantiation failed", activity_validator);

        sight::activity::validator::return_t validation = activity_validator->validate(_activity);
        if(!validation.first)
        {
            ok          = false;
            _error_msg += "\n" + validation.second;
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------

void data_view::remove_selected_objects()
{
    auto tab_index                = static_cast<std::size_t>(this->currentIndex());
    QPointer<QTreeWidget> tree    = m_tree_widgets[tab_index];
    QList<QTreeWidgetItem*> items = tree->selectedItems();
    for(QTreeWidgetItem* item : items)
    {
        if(item != nullptr)
        {
            int item_index                  = tree->indexOfTopLevelItem(item);
            QTreeWidgetItem* item_to_remove = tree->takeTopLevelItem(item_index);
            if(item_to_remove != nullptr)
            {
                delete item_to_remove;
                this->update();
            }
        }
    }
}

//-----------------------------------------------------------------------------

void data_view::clear_tree()
{
    auto tab_index             = static_cast<std::size_t>(this->currentIndex());
    QPointer<QTreeWidget> tree = m_tree_widgets[tab_index];
    tree->clear();
}

//-----------------------------------------------------------------------------

void data_view::create_new_object()
{
    auto index = static_cast<std::size_t>(this->currentIndex());

    activity_requirement req = m_activity_info.requirements[index];

    std::string type = req.type;

    QPointer<QTreeWidget> tree = m_tree_widgets[index];

    auto nb_items = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nb_items >= req.max_occurs)
    {
        const QString message("Can't create more '" + QString::fromStdString(type)
                              + "', please remove one to create another.");
        QMessageBox::warning(this, "New", message);
        return;
    }

    data::object::sptr new_object = data::factory::make(type);

    m_imported_object.push_back(new_object);
    this->add_object_item(index, new_object);
}

//-----------------------------------------------------------------------------

void data_view::import_object()
{
    const auto index = static_cast<std::size_t>(this->currentIndex());

    activity_requirement req = m_activity_info.requirements[index];

    const std::string type = req.type;

    QPointer<QTreeWidget> tree = m_tree_widgets[index];

    const auto nb_items = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nb_items >= req.max_occurs)
    {
        const QString message("Can't load more '" + QString::fromStdString(type)
                              + "', please remove one to load another.");
        QMessageBox::warning(this, "Import", message);
        return;
    }

    auto obj = sight::module::ui::qt::activity::data_view::read_object(type, m_io_selector_srv_config);
    if(obj)
    {
        m_imported_object.push_back(obj);

        this->add_object_item(index, obj);
    }
}

//-----------------------------------------------------------------------------

void data_view::import_object_from_sdb()
{
    const auto index = static_cast<std::size_t>(this->currentIndex());

    activity_requirement req = m_activity_info.requirements[index];

    const std::string type = req.type;

    QPointer<QTreeWidget> tree = m_tree_widgets[index];

    const auto nb_items = static_cast<unsigned int>(tree->topLevelItemCount());

    if(nb_items >= req.max_occurs)
    {
        const QString message("Can't load more '" + QString::fromStdString(type)
                              + "', please remove one to load another.");
        QMessageBox::warning(this, "Import from series_set", message);
        return;
    }

    data::object::sptr new_object = data::factory::make(type);
    if(new_object)
    {
        SIGHT_ERROR_IF(
            "Imported object must inherit from 'Series'.",
            !std::dynamic_pointer_cast<data::series>(new_object)
        );

        // We use the series_set reader and then extract the object of this type.
        auto obj = sight::module::ui::qt::activity::data_view::read_object(
            "sight::data::series_set",
            m_sdb_io_selector_srv_config
        );

        auto series_set = std::dynamic_pointer_cast<data::series_set>(obj);
        if(series_set)
        {
            unsigned int nb_imported_obj = 0;
            for(const data::series::sptr& series : *series_set)
            {
                if(series->is_a(type))
                {
                    ++nb_imported_obj;
                    m_imported_object.push_back(series);

                    this->add_object_item(index, series);

                    if(nb_imported_obj >= req.max_occurs)
                    {
                        break;
                    }
                }
            }
        }
    }
    else
    {
        std::string msg = "Can not create object '" + type + "'";
        SIGHT_ERROR(msg);
        QMessageBox message_box(QMessageBox::Warning, "Error", QString::fromStdString(msg), QMessageBox::Ok);
    }
}

//-----------------------------------------------------------------------------

data::object::sptr data_view::read_object(
    const std::string& _classname,
    const std::string& _io_selector_srv_config
)
{
    data::object::sptr obj;
    service::base::sptr io_selector_srv;
    io_selector_srv = service::add("sight::module::ui::io::selector");

    auto io_config = service::extension::config::get_default()->get_service_config(
        _io_selector_srv_config,
        "sight::module::ui::io::selector"
    );

    io_config.add("type.<xmlattr>.class", _classname); // add the class of the output object

    try
    {
        obj = data::factory::make(_classname);
        io_selector_srv->set_config(io_config);
        io_selector_srv->configure();
        io_selector_srv->set_inout(obj, io::service::DATA_KEY);
        io_selector_srv->start();
        io_selector_srv->update();
        io_selector_srv->stop();
        service::unregister_service(io_selector_srv);
    }
    catch(std::exception& e)
    {
        std::stringstream msg;
        msg << "The object can not be imported: " << e.what();
        SIGHT_ERROR(msg.str());

        QMessageBox message_box(QMessageBox::Warning, "Error", QString::fromStdString(msg.str()), QMessageBox::Ok);
        if(io_selector_srv->started())
        {
            io_selector_srv->stop();
        }

        service::unregister_service(io_selector_srv);
    }
    return obj;
}

//-----------------------------------------------------------------------------

void data_view::add_object_item(std::size_t _index, const data::object::csptr& _obj)
{
    QPointer<QTreeWidget> tree = m_tree_widgets[_index];

    auto* const new_item = new QTreeWidgetItem();
    new_item->setFlags(new_item->flags() & ~Qt::ItemIsDropEnabled);
    new_item->setData(int(column_commun_t::id), UID_ROLE, QVariant(QString::fromStdString(_obj->get_id())));

    const auto series    = std::dynamic_pointer_cast<const data::series>(_obj);
    const auto str_obj   = std::dynamic_pointer_cast<const data::string>(_obj);
    const auto int_obj   = std::dynamic_pointer_cast<const data::integer>(_obj);
    const auto float_obj = std::dynamic_pointer_cast<const data::real>(_obj);
    const auto bool_obj  = std::dynamic_pointer_cast<const data::boolean>(_obj);
    const auto trf       = std::dynamic_pointer_cast<const data::matrix4>(_obj);
    if(series)
    {
        new_item->setText(int(column_series_t::name), QString::fromStdString(series->get_patient_name()));
        new_item->setText(int(column_series_t::sex), QString::fromStdString(series->get_patient_sex()));
        std::string birthdate = series->get_patient_birth_date();
        if(!birthdate.empty() && birthdate != "unknown")
        {
            birthdate.insert(4, "-");
            birthdate.insert(7, "-");
        }

        new_item->setText(int(column_series_t::birthdate), QString::fromStdString(birthdate));

        new_item->setText(int(column_series_t::modality), QString::fromStdString(series->get_modality_string()));
        new_item->setText(
            int(column_series_t::modality_desc),
            QString::fromStdString(series->get_series_description())
        );

        new_item->setText(
            int(column_series_t::study_desc),
            QString::fromStdString(series->get_study_description())
        );
        std::string date = series->get_series_date();
        if(!date.empty())
        {
            date.insert(4, "/");
            date.insert(7, "/");
        }

        new_item->setText(int(column_series_t::date), QString::fromStdString(date));

        std::string time = series->get_series_time();
        if(!time.empty())
        {
            time.insert(2, ":");
            time.insert(5, ":");
        }

        new_item->setText(int(column_series_t::time), QString::fromStdString(time.substr(0, 8)));

        std::string patient_age = series->get_patient_age();
        if(!patient_age.empty())
        {
            patient_age.insert(3, " ");
            if(patient_age[0] == '0')
            {
                patient_age.erase(0, 1);
            }
        }

        new_item->setText(int(column_series_t::patient_age), QString::fromStdString(patient_age));

        const auto image_series = std::dynamic_pointer_cast<const data::image_series>(_obj);
        if(image_series)
        {
            new_item->setText(
                int(column_image_series_t::body_part_examined),
                QString::fromStdString(image_series->get_body_part_examined())
            );
            std::string patient_position = image_series->get_patient_position();
            if(!patient_position.empty())
            {
                // Code string can contains leading or trailing spaces, we removed it first.
                const std::string::const_iterator forward =
                    std::remove_if(
                        patient_position.begin(),
                        patient_position.end(),
                        [&](unsigned char _c)
                    {
                        return _c == ' ';
                    });
                patient_position.erase(forward, patient_position.end());
                if(patient_position == "HFP")
                {
                    patient_position = "Head First-Prone";
                }
                else if(patient_position == "HFS")
                {
                    patient_position = "Head First-Supine";
                }
                else if(patient_position == "HFDR")
                {
                    patient_position = "Head First-Decubitus Right";
                }
                else if(patient_position == "HFDL")
                {
                    patient_position = "Head First-Decubitus Left";
                }
                else if(patient_position == "FFDR")
                {
                    patient_position = "Feet First-Decubitus Right";
                }
                else if(patient_position == "FFDL")
                {
                    patient_position = "Feet First-Decubitus Left";
                }
                else if(patient_position == "FFP")
                {
                    patient_position = "Feet First-Prone";
                }
                else if(patient_position == "FFS")
                {
                    patient_position = "Feet First-Supine";
                }
                else if(patient_position == "LFP")
                {
                    patient_position = "Left First-Prone";
                }
                else if(patient_position == "LFS")
                {
                    patient_position = "Left First-Supine";
                }
                else if(patient_position == "RFP")
                {
                    patient_position = "Right First-Prone";
                }
                else if(patient_position == "RFS")
                {
                    patient_position = "Right First-Supine";
                }
                else if(patient_position == "AFDR")
                {
                    patient_position = "Anterior First-Decubitus Right";
                }
                else if(patient_position == "AFDL")
                {
                    patient_position = "Anterior First-Decubitus Left";
                }
                else if(patient_position == "PFDR")
                {
                    patient_position = "Posterior First-Decubitus Right";
                }
                else if(patient_position == "PFDL")
                {
                    patient_position = "Posterior First-Decubitus Left";
                }
            }

            new_item->setText(
                int(column_image_series_t::patient_position),
                QString::fromStdString(patient_position)
            );
            new_item->setText(
                int(column_image_series_t::contrast_agent),
                QString::fromStdString(image_series->get_contrast_bolus_agent())
            );
            std::string acquisition_time = image_series->get_acquisition_time();
            if(!acquisition_time.empty())
            {
                acquisition_time.insert(2, ":");
                acquisition_time.insert(5, ":");
            }

            new_item->setText(
                int(column_image_series_t::acquisition_time),
                QString::fromStdString(acquisition_time.substr(0, 8))
            );
            std::string contrast_time = image_series->get_contrast_bolus_start_time();
            if(!contrast_time.empty())
            {
                contrast_time.insert(2, ":");
                contrast_time.insert(5, ":");
            }

            new_item->setText(
                int(column_image_series_t::contrast_bolus_start_time),
                QString::fromStdString(contrast_time.substr(0, 8))
            );
        }
    }
    else if(str_obj)
    {
        std::string description = str_obj->value();
        if(description.empty())
        {
            description = _obj->get_classname();
        }

        new_item->setText(int(column_object_t::desc), QString::fromStdString(description));
    }
    else if(int_obj)
    {
        new_item->setText(int(column_object_t::desc), QString("%1").arg(int_obj->value()));
    }
    else if(float_obj)
    {
        new_item->setText(int(column_object_t::desc), QString("%1").arg(float_obj->value()));
    }
    else if(bool_obj)
    {
        new_item->setText(int(column_object_t::desc), bool_obj->value() ? "true" : "false");
    }
    else if(trf)
    {
        std::stringstream str;
        str << *trf;
        new_item->setText(int(column_object_t::desc), QString::fromStdString(str.str()));
    }
    else
    {
        new_item->setText(int(column_object_t::desc), QString::fromStdString(_obj->get_classname()));
    }

    // set icon
    auto iter = m_object_icons.find(_obj->get_classname());
    if(iter != m_object_icons.end())
    {
        new_item->setIcon(int(column_commun_t::id), QIcon(QString::fromStdString(iter->second)));
    }

    tree->addTopLevelItem(new_item);
    for(int i = 0 ; i < tree->columnCount() ; ++i)
    {
        tree->resizeColumnToContents(i);
    }
}

//-----------------------------------------------------------------------------

void data_view::on_tree_item_double_clicked(QTreeWidgetItem* _item, int /*unused*/)
{
    if(_item != nullptr)
    {
        std::string uid = _item->data(int(column_commun_t::id), UID_ROLE).toString().toStdString();
        if(!uid.empty())
        {
            data::object::sptr obj = std::dynamic_pointer_cast<data::object>(core::id::get_object(uid));
            if(obj)
            {
                if(obj->is_a("sight::data::string"))
                {
                    data::string::sptr str = std::dynamic_pointer_cast<data::string>(obj);
                    bool is_ok_clicked     = false;
                    QString value          = QInputDialog::getText(
                        this,
                        "Edition",
                        "Enter the String value:",
                        QLineEdit::Normal,
                        QString::fromStdString(str->value()),
                        &is_ok_clicked
                    );

                    if(is_ok_clicked)
                    {
                        str->value() = value.toStdString();
                        _item->setText(int(column_object_t::desc), value);
                    }
                }
                else if(obj->is_a("sight::data::integer"))
                {
                    data::integer::sptr int_obj = std::dynamic_pointer_cast<data::integer>(obj);

                    bool is_ok_clicked = false;
                    int value          = QInputDialog::getInt(
                        this,
                        "Edition",
                        "Enter the Integer value:",
                        int(int_obj->value()),
                        std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max(),
                        1,
                        &is_ok_clicked
                    );
                    if(is_ok_clicked)
                    {
                        int_obj->value() = value;
                        _item->setText(int(column_object_t::desc), QString("%1").arg(value));
                    }
                }
                else if(obj->is_a("sight::data::real"))
                {
                    data::real::sptr float_obj = std::dynamic_pointer_cast<data::real>(obj);

                    bool is_ok_clicked = false;
                    double value       = QInputDialog::getDouble(
                        this,
                        "Edition",
                        "Enter the Integer value:",
                        float_obj->value(),
                        std::numeric_limits<int>::min(),
                        std::numeric_limits<int>::max(),
                        3,
                        &is_ok_clicked
                    );
                    if(is_ok_clicked)
                    {
                        float_obj->value() = static_cast<float>(value);
                        _item->setText(int(column_object_t::desc), QString("%1").arg(value));
                    }
                }
                else if(obj->is_a("sight::data::boolean"))
                {
                    data::boolean::sptr bool_obj       = std::dynamic_pointer_cast<data::boolean>(obj);
                    QMessageBox::StandardButton button = QMessageBox::question(
                        this,
                        "Edition",
                        "Defines the Boolean value"
                    );
                    bool_obj->value() = (button == QMessageBox::Yes);
                    _item->setText(int(column_object_t::desc), bool_obj->value() ? "true" : "false");
                }
                else if(obj->is_a("sight::data::matrix4"))
                {
                    data::matrix4::sptr trf = std::dynamic_pointer_cast<data::matrix4>(obj);
                    std::stringstream str;
                    str << *trf;

                    bool is_ok_clicked = false;
                    QString value      = QInputDialog::getMultiLineText(
                        this,
                        "Edition",
                        "Enter the Matrix coefficient (separated by a space):",
                        QString::fromStdString(str.str()),
                        &is_ok_clicked
                    );

                    QStringList coeff_list = value.trimmed().split(QRegularExpression("\\s+"));
                    if(is_ok_clicked && coeff_list.size() == 16)
                    {
                        data::matrix4::container_t coeffs;

                        bool conversion_ok = false;
                        for(int i = 0 ; i < 16 ; ++i)
                        {
                            coeffs[std::size_t(i)] = coeff_list[i].toDouble(&conversion_ok);
                            if(!conversion_ok)
                            {
                                QMessageBox::warning(
                                    this,
                                    "ERROR",
                                    "This values cannot be converted to matrix coefficients"
                                );
                                return;
                            }
                        }

                        (*trf) = coeffs;
                        _item->setText(int(column_object_t::desc), value.trimmed());
                    }
                    else if(is_ok_clicked)
                    {
                        QMessageBox::warning(
                            this,
                            "ERROR",
                            "This values cannot be converted to matrix coefficients. It must contain "
                            "16 values"
                        );
                        return;
                    }
                }
                else
                {
                    SIGHT_DEBUG("Object of type '" + obj->classname() + "' can not yet be editted");
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::qt::activity
