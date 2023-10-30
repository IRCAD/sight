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

#include "dump_editor.hpp"

#include <core/base.hpp>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/memory/buffer_manager.hpp>
#include <core/memory/byte_size.hpp>
#include <core/memory/policy/base.hpp>
#include <core/memory/tools/memory_monitor_tools.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/qt/container/widget.hpp>

#include <QComboBox>
#include <QFuture>
#include <QHeaderView>
#include <QItemDelegate>
#include <QStringList>
#include <QTableWidgetItem>
#include <QtConcurrentRun>
#include <QTimer>
#include <QVBoxLayout>

namespace sight::module::ui::debug
{

//------------------------------------------------------------------------------

core::memory::buffer_manager::buffer_info_map_t m_buffer_infos;
core::memory::buffer_manager::buffer_stats m_buffer_stats = {0, 0};

//------------------------------------------------------------------------------

QString get_human_readable_size(core::memory::byte_size::size_t _bytes)
{
    return QString::fromStdString(core::memory::byte_size(_bytes));
}

//------------------------------------------------------------------------------

class policy_combo_box_delegate : public QItemDelegate
{
public:

    explicit policy_combo_box_delegate(QObject* _parent = nullptr) :
        QItemDelegate(_parent)
    {
    }

    QWidget* createEditor(
        QWidget* _parent,
        const QStyleOptionViewItem& _option,
        const QModelIndex& _index
    ) const override;

    void setEditorData(QWidget* _editor, const QModelIndex& _index) const override;
    void setModelData(QWidget* _editor, QAbstractItemModel* _model, const QModelIndex& _index) const override;

    void updateEditorGeometry(
        QWidget* _editor,
        const QStyleOptionViewItem& _option,
        const QModelIndex& _index
    ) const override;
};

//------------------------------------------------------------------------------

QWidget* policy_combo_box_delegate::createEditor(
    QWidget* _parent,
    const QStyleOptionViewItem& /*option*/,
    const QModelIndex& _index
) const
{
    auto* policy_combo_box = new QComboBox(_parent);

    const std::string value = _index.model()->data(_index, Qt::DisplayRole).toString().toStdString();

    const auto& factories = core::memory::policy::registry::get()->get_factory_keys();

    for(const auto& policy : factories)
    {
        policy_combo_box->addItem(QString::fromStdString(policy));
        if(value == policy)
        {
            policy_combo_box->setCurrentIndex(policy_combo_box->count() - 1);
        }
    }

    return policy_combo_box;
}

//------------------------------------------------------------------------------

void policy_combo_box_delegate::setEditorData(QWidget* _editor, const QModelIndex& _index) const
{
    QString value = _index.model()->data(_index, Qt::DisplayRole).toString();

    auto* policy_combo_box = static_cast<QComboBox*>(_editor);

    int idx = policy_combo_box->findText(value);
    if(idx != -1)
    {
        policy_combo_box->setCurrentIndex(idx);
    }
}

//------------------------------------------------------------------------------

void policy_combo_box_delegate::setModelData(
    QWidget* _editor,
    QAbstractItemModel* _model,
    const QModelIndex& _index
) const
{
    auto* policy_combo_box = static_cast<QComboBox*>(_editor);
    QString value          = policy_combo_box->currentText();

    _model->setData(_index, value, Qt::EditRole);
}

//------------------------------------------------------------------------------

void policy_combo_box_delegate::updateEditorGeometry(
    QWidget* _editor,
    const QStyleOptionViewItem& _option,
    const QModelIndex&
    /*index*/
) const
{
    _editor->setGeometry(_option.rect);
}

class policy_table_model : public QAbstractTableModel
{
public:

    explicit policy_table_model(QObject* _parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& _parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex& _parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& _index, int _role) const override;
    [[nodiscard]] QVariant headerData(int _section, Qt::Orientation _orientation, int _role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& _index) const override;
    bool setData(const QModelIndex& _index, const QVariant& _value, int _role = Qt::EditRole) override;

    static const int EXTRA_INFO_NB;

private:

    core::memory::buffer_manager::sptr m_buff_manager;
};

const int policy_table_model::EXTRA_INFO_NB = 1;

policy_table_model::policy_table_model(QObject* _parent) :
    QAbstractTableModel(_parent),
    m_buff_manager(core::memory::buffer_manager::get())
{
}

//------------------------------------------------------------------------------

int policy_table_model::rowCount(const QModelIndex& _parent) const
{
    Q_UNUSED(_parent);
    std::size_t nb_param = 0;
    if(m_buff_manager)
    {
        core::mt::read_lock lock(m_buff_manager->get_mutex());
        auto current_policy = m_buff_manager->get_dump_policy();
        nb_param = current_policy->get_param_names().size();
    }

    return static_cast<int>(nb_param + EXTRA_INFO_NB);
}

//------------------------------------------------------------------------------

int policy_table_model::columnCount(const QModelIndex& _parent) const
{
    Q_UNUSED(_parent);
    return 1;
}

//------------------------------------------------------------------------------

QVariant policy_table_model::data(const QModelIndex& _index, int _role) const
{
    if(!m_buff_manager && !_index.isValid())
    {
        return {};
    }

    core::mt::read_lock lock(m_buff_manager->get_mutex());
    auto current_policy = m_buff_manager->get_dump_policy();

    if(_index.row() > int((EXTRA_INFO_NB + current_policy->get_param_names().size())) || _index.row() < 0)
    {
        return {};
    }

    if(_role == Qt::DisplayRole)
    {
        if(_index.column() == 0)
        {
            const auto& names = current_policy->get_param_names();
            if(_index.row() == 0)
            {
                return QString::fromStdString(current_policy->get_leaf_classname());
            }

            if((unsigned int) _index.row() <= names.size())
            {
                const auto& name = names.at(std::size_t(_index.row() - 1));

                return QString::fromStdString(current_policy->get_param(name));
            }
        }
    }

    return {};
}

//------------------------------------------------------------------------------

QVariant policy_table_model::headerData(int _section, Qt::Orientation _orientation, int _role) const
{
    if(_role != Qt::DisplayRole)
    {
        return {};
    }

    if(m_buff_manager && _orientation == Qt::Vertical)
    {
        core::mt::read_lock lock(m_buff_manager->get_mutex());
        auto current_policy = m_buff_manager->get_dump_policy();
        const auto& names   = current_policy->get_param_names();
        if(_section <= 0)
        {
            return QString("Current policy");
        }

        if((unsigned int) _section <= names.size())
        {
            const core::memory::policy::base::param_names_type::value_type& name = names.at(std::size_t(_section - 1));
            return QString::fromStdString(name);
        }
    }

    return {};
}

//------------------------------------------------------------------------------

bool policy_table_model::setData(const QModelIndex& _index, const QVariant& _value, int _role)
{
    if(m_buff_manager && _index.isValid() && _role == Qt::EditRole)
    {
        int row                    = _index.row();
        int col                    = _index.column();
        const std::string strvalue = _value.toString().toStdString();

        core::mt::read_lock lock(m_buff_manager->get_mutex());
        auto current_policy                                       = m_buff_manager->get_dump_policy();
        const core::memory::policy::base::param_names_type& names = current_policy->get_param_names();

        if(col == 0 && (unsigned int) row <= names.size())
        {
            core::memory::policy::base::sptr dump_policy;
            if(row == 0)
            {
                if(strvalue != current_policy->get_leaf_classname())
                {
                    dump_policy = core::memory::policy::registry::get()->create(strvalue);
                    if(dump_policy)
                    {
                        core::mt::read_to_write_lock another_lock(m_buff_manager->get_mutex());
                        m_buff_manager->set_dump_policy(dump_policy);
                    }

                    this->beginResetModel();
                    this->endResetModel();
                }
                else
                {
                    const core::memory::policy::base::param_names_type::value_type& name =
                        names.at(std::size_t(row - 1));
                    current_policy->set_param(name, strvalue);
                    return true;
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

Qt::ItemFlags policy_table_model::flags(const QModelIndex& _index) const
{
    if(!_index.isValid())
    {
        return Qt::ItemIsEnabled;
    }

    return QAbstractTableModel::flags(_index) | Qt::ItemIsEditable;
}

//------------------------------------------------------------------------------

class info_table_model : public QAbstractTableModel
{
public:

    explicit info_table_model(QObject* _parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& _parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex& _parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& _index, int _role) const override;
    [[nodiscard]] QVariant headerData(int _section, Qt::Orientation _orientation, int _role) const override;

private:

    core::memory::buffer_manager::sptr m_buff_manager;
};

info_table_model::info_table_model(QObject* _parent) :
    QAbstractTableModel(_parent),
    m_buff_manager(core::memory::buffer_manager::get())
{
}

//------------------------------------------------------------------------------

int info_table_model::rowCount(const QModelIndex& _parent) const
{
    Q_UNUSED(_parent);
    return 4;
}

//------------------------------------------------------------------------------

int info_table_model::columnCount(const QModelIndex& _parent) const
{
    Q_UNUSED(_parent);
    return 1;
}

//------------------------------------------------------------------------------

QVariant info_table_model::data(const QModelIndex& _index, int _role) const
{
    if(!m_buff_manager || !_index.isValid())
    {
        return {};
    }

    if(_index.row() > this->rowCount(_index))
    {
        return {};
    }

    if(_role == Qt::DisplayRole)
    {
        if(_index.column() == 0)
        {
            std::uint64_t sys_mem                                   = 0;
            core::memory::buffer_manager::size_t buffer_manager_mem = 0;
            switch(_index.row())
            {
                case 0:
                    sys_mem = core::memory::tools::memory_monitor_tools::get_total_system_memory();
                    return QString(get_human_readable_size(sys_mem));

                    break;

                case 1:
                    sys_mem = core::memory::tools::memory_monitor_tools::get_free_system_memory();
                    return QString(get_human_readable_size(sys_mem));

                    break;

                case 2:
                    buffer_manager_mem = m_buffer_stats.total_managed;
                    return QString(get_human_readable_size(buffer_manager_mem));

                    break;

                case 3:
                    buffer_manager_mem = m_buffer_stats.total_dumped;
                    return QString(get_human_readable_size(buffer_manager_mem));

                    break;

                default:
                    SIGHT_ASSERT("Unreachable code", false);
            }
        }
    }

    return {};
}

//------------------------------------------------------------------------------

QVariant info_table_model::headerData(int _section, Qt::Orientation _orientation, int _role) const
{
    if(_role == Qt::DisplayRole && _orientation == Qt::Vertical)
    {
        switch(_section)
        {
            case 0:
                return QString("Total System Memory");

                break;

            case 1:
                return QString("Free System Memory");

                break;

            case 2:
                return QString("Managed");

                break;

            case 3:
                return QString("Dumped");

                break;

            default:
                return {};
        }
    }

    return {};
}

//------------------------------------------------------------------------------

dump_editor::dump_editor() noexcept =
    default;

//------------------------------------------------------------------------------

dump_editor::~dump_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void dump_editor::starting()
{
    this->sight::ui::service::create();

    auto qt_container = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->get_container());

    m_update_timer = new QTimer(qt_container->get_qt_container());
    m_update_timer->setInterval(300);
    m_update_timer->setSingleShot(true);

    m_list = new QTableWidget();

    m_list->setColumnCount(5);
    QStringList header;
    header.push_back("Size");
    header.push_back("Status");
    header.push_back("Timestamp");
    header.push_back("Locked");
    header.push_back("action");
    m_list->setHorizontalHeaderLabels(header);

    m_refresh = new QPushButton(tr("Refresh"));
    auto* sizer = new QVBoxLayout();

    auto* sizer_button = new QHBoxLayout();
    sizer_button->addWidget(m_refresh);

    sizer_button->addItem(new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    auto* vertical_line = new QFrame();
    vertical_line->setFrameShape(QFrame::VLine);
    vertical_line->setFrameShadow(QFrame::Sunken);
    sizer_button->addWidget(vertical_line);

    sizer->addLayout(sizer_button);
    sizer->addWidget(m_list, 2);

    m_policy_editor = new QTableView();
    auto* policy_combo_box_delegate = new class policy_combo_box_delegate (m_policy_editor) ;
                                          auto* policy_table_model = new class policy_table_model (m_policy_editor) ;
                                                                         m_policy_editor->setModel(policy_table_model);
    m_policy_editor->setItemDelegateForRow(0, policy_combo_box_delegate);
    m_policy_editor->setSortingEnabled(false);
    m_policy_editor->horizontalHeader()->hide();

    auto* info_table_model = new class info_table_model () ;
        m_info_editor      = new QTableView();
    m_info_editor->setModel(info_table_model);
    m_info_editor->horizontalHeader()->hide();

    auto* tables_layout = new QHBoxLayout();
    tables_layout->addWidget(m_info_editor);
    tables_layout->addWidget(m_policy_editor);

    sizer->addLayout(tables_layout);

    qt_container->set_layout(sizer);

    QObject::connect(m_refresh, &QPushButton::clicked, this, &dump_editor::on_refresh_button);
    QObject::connect(m_update_timer, &QTimer::timeout, this, &dump_editor::on_refresh_button);
    QObject::connect(
        &m_watcher,
        &QFutureWatcher<core::memory::buffer_manager::buffer_info_map_t>::finished,
        this,
        &dump_editor::on_buffer_info
    );

    core::memory::buffer_manager::sptr buff_manager = core::memory::buffer_manager::get();
    if(buff_manager)
    {
        m_update_slot = core::com::new_slot(&dump_editor::on_update, this);
        m_update_slot->set_worker(core::thread::get_default_worker());
        m_connection = buff_manager->get_updated_signal()->connect(m_update_slot);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void dump_editor::stopping()
{
    m_connection.disconnect();
    m_watcher.disconnect();

    this->destroy();
}

//------------------------------------------------------------------------------

void dump_editor::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

class size_table_widget_item : public QTableWidgetItem
{
public:

    explicit size_table_widget_item(const QString& _text) :
        QTableWidgetItem(_text)
    {
    }

    //------------------------------------------------------------------------------

    bool operator<(const QTableWidgetItem& _other) const override
    {
        return data(Qt::UserRole).toULongLong() < _other.data(Qt::UserRole).toULongLong();
    }
};

//------------------------------------------------------------------------------

core::memory::buffer_manager::buffer_info_map_t get_info_map()
{
    core::memory::buffer_manager::buffer_info_map_t info_map;
    core::memory::buffer_manager::sptr buff_manager = core::memory::buffer_manager::get();
    if(buff_manager)
    {
        info_map = buff_manager->get_buffer_infos().get();
    }

    return info_map;
}

//------------------------------------------------------------------------------

void dump_editor::updating()
{
    m_policy_editor->reset();
    m_policy_editor->resizeColumnsToContents();

    QFuture<core::memory::buffer_manager::buffer_info_map_t> q_future = QtConcurrent::run(get_info_map);
    m_watcher.setFuture(q_future);
}

//------------------------------------------------------------------------------

void dump_editor::on_buffer_info()
{
    m_buffer_infos = m_watcher.result();
    m_buffer_stats = core::memory::buffer_manager::compute_buffer_stats(m_buffer_infos);

    core::com::connection::blocker block(m_connection);

    m_list->clearContents();
    m_objects_uid.clear();

    int item_count = 0;
    m_list->setSortingEnabled(false);
    m_list->setRowCount(static_cast<int>(m_buffer_infos.size()));
    m_list->setColumnCount(5);
    QColor back_color;
    for(const auto& elt : m_buffer_infos)
    {
        m_objects_uid.push_back(elt.first);

        std::string status      = "?";
        std::string date        = "?";
        std::string lock_status = "?";

        const core::memory::buffer_info& dump_buff_info = elt.second;
        bool loaded                                     = dump_buff_info.loaded;
        if(!loaded)
        {
            back_color = Qt::darkYellow;
            status     = "Dumped";
        }
        else
        {
            back_color = Qt::white;
            status     = "-";
        }

        bool is_lock = dump_buff_info.lock_count() > 0;
        if(is_lock)
        {
            lock_status = "locked(" + std::to_string(dump_buff_info.lock_count()) + ")";
        }
        else
        {
            lock_status = "unlocked";
        }

        date = std::to_string(dump_buff_info.last_access.get_logic_stamp());

        QTableWidgetItem* current_size_item = new size_table_widget_item(get_human_readable_size(dump_buff_info.size));
        current_size_item->setData(Qt::UserRole, (qulonglong) dump_buff_info.size);
        current_size_item->setFlags(Qt::ItemIsEnabled);
        current_size_item->setBackground(back_color);
        m_list->setItem(item_count, 0, current_size_item);

        auto* status_item = new QTableWidgetItem(QString::fromStdString(status));
        status_item->setFlags(Qt::ItemIsEnabled);
        status_item->setBackground(back_color);
        m_list->setItem(item_count, 1, status_item);

        auto* date_item = new QTableWidgetItem(QString::fromStdString(date));
        date_item->setFlags(Qt::ItemIsEnabled);
        date_item->setBackground(back_color);
        m_list->setItem(item_count, 2, date_item);

        auto* lock_status_item = new QTableWidgetItem(QString::fromStdString(lock_status));
        lock_status_item->setFlags(Qt::ItemIsEnabled);
        lock_status_item->setBackground(back_color);
        m_list->setItem(item_count, 3, lock_status_item);

        auto* action_item = new QPushButton(QString::fromStdString((loaded) ? "Dump" : "Restore"), m_list);
        action_item->setEnabled(!is_lock && (dump_buff_info.size > 0));
        m_list->setCellWidget(item_count, 4, action_item);

        QObject::connect(action_item, &QPushButton::pressed, this, [this, item_count](){change_status(item_count);});
        ++item_count;
    }

    m_list->setSortingEnabled(true);

    m_info_editor->reset();
    m_info_editor->resizeColumnsToContents();
}

//------------------------------------------------------------------------------

void dump_editor::info(std::ostream& _sstream)
{
    _sstream << "Dump Editor";
}

//------------------------------------------------------------------------------

void dump_editor::on_update()
{
    m_update_timer->start();
}

//------------------------------------------------------------------------------

void dump_editor::on_refresh_button()
{
    this->updating();
}

//------------------------------------------------------------------------------

void dump_editor::change_status(int _index)
{
    core::memory::buffer_manager::sptr buff_manager = core::memory::buffer_manager::get();
    if(buff_manager)
    {
        const core::memory::buffer_manager::buffer_info_map_t buff_info_map = m_buffer_infos;
        const auto* selected_buffer                                         = m_objects_uid[std::size_t(_index)];

        core::memory::buffer_manager::buffer_info_map_t::const_iterator iter;
        iter = buff_info_map.find(selected_buffer);
        if(iter != buff_info_map.end())
        {
            sight::ui::cursor cursor;
            cursor.set_cursor(sight::ui::cursor_base::busy);
            const core::memory::buffer_info& dump_buff_info = iter->second;

            bool is_lock = dump_buff_info.lock_count() > 0;
            if(!is_lock)
            {
                if(dump_buff_info.loaded)
                {
                    buff_manager->dump_buffer(selected_buffer);
                }
                else
                {
                    buff_manager->restore_buffer(selected_buffer);
                }
            }
            else
            {
                sight::ui::dialog::message::show(
                    "Dump process information",
                    "Dump process is locked. It is impossible to dump or restore this object.",
                    sight::ui::dialog::message::warning
                );
            }

            cursor.set_default_cursor();

            this->updating();
        }
        else
        {
            std::stringstream stream;
            stream << "Object " << selected_buffer << " not found, please refresh the grid.";
            sight::ui::dialog::message::show(
                "Dump process information",
                stream.str(),
                sight::ui::dialog::message::warning
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::debug
