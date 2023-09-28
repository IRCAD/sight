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

#include "DumpEditor.hpp"

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

core::memory::buffer_manager::buffer_info_map_t m_bufferInfos;
core::memory::buffer_manager::buffer_stats m_bufferStats = {0, 0};

//------------------------------------------------------------------------------

QString getHumanReadableSize(core::memory::byte_size::size_t bytes)
{
    return QString::fromStdString(core::memory::byte_size(bytes));
}

//------------------------------------------------------------------------------

class PolicyComboBoxDelegate : public QItemDelegate
{
public:

    explicit PolicyComboBoxDelegate(QObject* parent = nullptr) :
        QItemDelegate(parent)
    {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;

    void updateEditorGeometry(
        QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& index
    ) const override;
};

//------------------------------------------------------------------------------

QWidget* PolicyComboBoxDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem& /*option*/,
    const QModelIndex& index
) const
{
    auto* policyComboBox = new QComboBox(parent);

    const std::string value = index.model()->data(index, Qt::DisplayRole).toString().toStdString();

    const auto& factories = core::memory::policy::registry::get()->get_factory_keys();

    for(const auto& policy : factories)
    {
        policyComboBox->addItem(QString::fromStdString(policy));
        if(value == policy)
        {
            policyComboBox->setCurrentIndex(policyComboBox->count() - 1);
        }
    }

    return policyComboBox;
}

//------------------------------------------------------------------------------

void PolicyComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    QString value = index.model()->data(index, Qt::DisplayRole).toString();

    auto* policyComboBox = static_cast<QComboBox*>(editor);

    int idx = policyComboBox->findText(value);
    if(idx != -1)
    {
        policyComboBox->setCurrentIndex(idx);
    }
}

//------------------------------------------------------------------------------

void PolicyComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    auto* policyComboBox = static_cast<QComboBox*>(editor);
    QString value        = policyComboBox->currentText();

    model->setData(index, value, Qt::EditRole);
}

//------------------------------------------------------------------------------

void PolicyComboBoxDelegate::updateEditorGeometry(
    QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex&
    /*index*/
) const
{
    editor->setGeometry(option.rect);
}

class PolicyTableModel : public QAbstractTableModel
{
public:

    explicit PolicyTableModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    static const int s_EXTRA_INFO_NB;

private:

    core::memory::buffer_manager::sptr m_buffManager;
};

const int PolicyTableModel::s_EXTRA_INFO_NB = 1;

PolicyTableModel::PolicyTableModel(QObject* parent) :
    QAbstractTableModel(parent),
    m_buffManager(core::memory::buffer_manager::get())
{
}

//------------------------------------------------------------------------------

int PolicyTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    std::size_t nbParam = 0;
    if(m_buffManager)
    {
        core::mt::read_lock lock(m_buffManager->get_mutex());
        auto currentPolicy = m_buffManager->get_dump_policy();
        nbParam = currentPolicy->get_param_names().size();
    }

    return static_cast<int>(nbParam + s_EXTRA_INFO_NB);
}

//------------------------------------------------------------------------------

int PolicyTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

//------------------------------------------------------------------------------

QVariant PolicyTableModel::data(const QModelIndex& index, int role) const
{
    if(!m_buffManager && !index.isValid())
    {
        return {};
    }

    core::mt::read_lock lock(m_buffManager->get_mutex());
    auto currentPolicy = m_buffManager->get_dump_policy();

    if(index.row() > int((s_EXTRA_INFO_NB + currentPolicy->get_param_names().size())) || index.row() < 0)
    {
        return {};
    }

    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            const auto& names = currentPolicy->get_param_names();
            if(index.row() == 0)
            {
                return QString::fromStdString(currentPolicy->get_leaf_classname());
            }

            if((unsigned int) index.row() <= names.size())
            {
                const auto& name = names.at(std::size_t(index.row() - 1));

                return QString::fromStdString(currentPolicy->get_param(name));
            }
        }
    }

    return {};
}

//------------------------------------------------------------------------------

QVariant PolicyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
    {
        return {};
    }

    if(m_buffManager && orientation == Qt::Vertical)
    {
        core::mt::read_lock lock(m_buffManager->get_mutex());
        auto currentPolicy = m_buffManager->get_dump_policy();
        const auto& names  = currentPolicy->get_param_names();
        if(section <= 0)
        {
            return QString("Current policy");
        }

        if((unsigned int) section <= names.size())
        {
            const core::memory::policy::base::param_names_type::value_type& name = names.at(std::size_t(section - 1));
            return QString::fromStdString(name);
        }
    }

    return {};
}

//------------------------------------------------------------------------------

bool PolicyTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(m_buffManager && index.isValid() && role == Qt::EditRole)
    {
        int row                    = index.row();
        int col                    = index.column();
        const std::string strvalue = value.toString().toStdString();

        core::mt::read_lock lock(m_buffManager->get_mutex());
        auto currentPolicy                                        = m_buffManager->get_dump_policy();
        const core::memory::policy::base::param_names_type& names = currentPolicy->get_param_names();

        if(col == 0 && (unsigned int) row <= names.size())
        {
            core::memory::policy::base::sptr dumpPolicy;
            if(row == 0)
            {
                if(strvalue != currentPolicy->get_leaf_classname())
                {
                    dumpPolicy = core::memory::policy::registry::get()->create(strvalue);
                    if(dumpPolicy)
                    {
                        core::mt::read_to_write_lock anotherLock(m_buffManager->get_mutex());
                        m_buffManager->set_dump_policy(dumpPolicy);
                    }

                    this->beginResetModel();
                    this->endResetModel();
                }
                else
                {
                    const core::memory::policy::base::param_names_type::value_type& name =
                        names.at(std::size_t(row - 1));
                    currentPolicy->set_param(name, strvalue);
                    return true;
                }
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------

Qt::ItemFlags PolicyTableModel::flags(const QModelIndex& index) const
{
    if(!index.isValid())
    {
        return Qt::ItemIsEnabled;
    }

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

//------------------------------------------------------------------------------

class InfoTableModel : public QAbstractTableModel
{
public:

    explicit InfoTableModel(QObject* parent = nullptr);

    [[nodiscard]] int rowCount(const QModelIndex& parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex& parent) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:

    core::memory::buffer_manager::sptr m_buffManager;
};

InfoTableModel::InfoTableModel(QObject* parent) :
    QAbstractTableModel(parent),
    m_buffManager(core::memory::buffer_manager::get())
{
}

//------------------------------------------------------------------------------

int InfoTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 4;
}

//------------------------------------------------------------------------------

int InfoTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

//------------------------------------------------------------------------------

QVariant InfoTableModel::data(const QModelIndex& index, int role) const
{
    if(!m_buffManager || !index.isValid())
    {
        return {};
    }

    if(index.row() > this->rowCount(index))
    {
        return {};
    }

    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            std::uint64_t sysMem                                  = 0;
            core::memory::buffer_manager::size_t bufferManagerMem = 0;
            switch(index.row())
            {
                case 0:
                    sysMem = core::memory::tools::memory_monitor_tools::get_total_system_memory();
                    return QString(getHumanReadableSize(sysMem));

                    break;

                case 1:
                    sysMem = core::memory::tools::memory_monitor_tools::get_free_system_memory();
                    return QString(getHumanReadableSize(sysMem));

                    break;

                case 2:
                    bufferManagerMem = m_bufferStats.total_managed;
                    return QString(getHumanReadableSize(bufferManagerMem));

                    break;

                case 3:
                    bufferManagerMem = m_bufferStats.total_dumped;
                    return QString(getHumanReadableSize(bufferManagerMem));

                    break;

                default:
                    SIGHT_ASSERT("Unreachable code", false);
            }
        }
    }

    return {};
}

//------------------------------------------------------------------------------

QVariant InfoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Vertical)
    {
        switch(section)
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

DumpEditor::DumpEditor() noexcept =
    default;

//------------------------------------------------------------------------------

DumpEditor::~DumpEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void DumpEditor::starting()
{
    this->sight::ui::service::create();

    auto qtContainer = std::dynamic_pointer_cast<sight::ui::qt::container::widget>(this->getContainer());

    m_updateTimer = new QTimer(qtContainer->getQtContainer());
    m_updateTimer->setInterval(300);
    m_updateTimer->setSingleShot(true);

    m_list = new QTableWidget();

    m_list->setColumnCount(5);
    QStringList header;
    header.push_back("Size");
    header.push_back("Status");
    header.push_back("Timestamp");
    header.push_back("Locked");
    header.push_back("Action");
    m_list->setHorizontalHeaderLabels(header);

    m_refresh = new QPushButton(tr("Refresh"));
    auto* sizer = new QVBoxLayout();

    auto* sizerButton = new QHBoxLayout();
    sizerButton->addWidget(m_refresh);

    sizerButton->addItem(new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    auto* verticalLine = new QFrame();
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setFrameShadow(QFrame::Sunken);
    sizerButton->addWidget(verticalLine);

    sizer->addLayout(sizerButton);
    sizer->addWidget(m_list, 2);

    m_policyEditor = new QTableView();
    auto* policyComboBoxDelegate = new PolicyComboBoxDelegate(m_policyEditor);
    auto* policyTableModel       = new PolicyTableModel(m_policyEditor);
    m_policyEditor->setModel(policyTableModel);
    m_policyEditor->setItemDelegateForRow(0, policyComboBoxDelegate);
    m_policyEditor->setSortingEnabled(false);
    m_policyEditor->horizontalHeader()->hide();

    auto* infoTableModel = new InfoTableModel();
    m_infoEditor = new QTableView();
    m_infoEditor->setModel(infoTableModel);
    m_infoEditor->horizontalHeader()->hide();

    auto* tablesLayout = new QHBoxLayout();
    tablesLayout->addWidget(m_infoEditor);
    tablesLayout->addWidget(m_policyEditor);

    sizer->addLayout(tablesLayout);

    qtContainer->setLayout(sizer);

    QObject::connect(m_refresh, &QPushButton::clicked, this, &DumpEditor::onRefreshButton);
    QObject::connect(m_updateTimer, &QTimer::timeout, this, &DumpEditor::onRefreshButton);
    QObject::connect(
        &m_watcher,
        &QFutureWatcher<core::memory::buffer_manager::buffer_info_map_t>::finished,
        this,
        &DumpEditor::on_buffer_info
    );

    core::memory::buffer_manager::sptr buffManager = core::memory::buffer_manager::get();
    if(buffManager)
    {
        m_updateSlot = core::com::new_slot(&DumpEditor::onUpdate, this);
        m_updateSlot->set_worker(core::thread::get_default_worker());
        m_connection = buffManager->get_updated_signal()->connect(m_updateSlot);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void DumpEditor::stopping()
{
    m_connection.disconnect();
    m_watcher.disconnect();

    this->destroy();
}

//------------------------------------------------------------------------------

void DumpEditor::configuring()
{
    this->sight::ui::service::initialize();
}

//------------------------------------------------------------------------------

class SizeTableWidgetItem : public QTableWidgetItem
{
public:

    explicit SizeTableWidgetItem(const QString& text) :
        QTableWidgetItem(text)
    {
    }

    //------------------------------------------------------------------------------

    bool operator<(const QTableWidgetItem& other) const override
    {
        return data(Qt::UserRole).toULongLong() < other.data(Qt::UserRole).toULongLong();
    }
};

//------------------------------------------------------------------------------

core::memory::buffer_manager::buffer_info_map_t getInfoMap()
{
    core::memory::buffer_manager::buffer_info_map_t infoMap;
    core::memory::buffer_manager::sptr buffManager = core::memory::buffer_manager::get();
    if(buffManager)
    {
        infoMap = buffManager->get_buffer_infos().get();
    }

    return infoMap;
}

//------------------------------------------------------------------------------

void DumpEditor::updating()
{
    m_policyEditor->reset();
    m_policyEditor->resizeColumnsToContents();

    QFuture<core::memory::buffer_manager::buffer_info_map_t> qFuture = QtConcurrent::run(getInfoMap);
    m_watcher.setFuture(qFuture);
}

//------------------------------------------------------------------------------

void DumpEditor::on_buffer_info()
{
    m_bufferInfos = m_watcher.result();
    m_bufferStats = core::memory::buffer_manager::compute_buffer_stats(m_bufferInfos);

    core::com::connection::blocker block(m_connection);

    m_list->clearContents();
    m_objectsUID.clear();

    int itemCount = 0;
    m_list->setSortingEnabled(false);
    m_list->setRowCount(static_cast<int>(m_bufferInfos.size()));
    m_list->setColumnCount(5);
    QColor backColor;
    for(const core::memory::buffer_manager::buffer_info_map_t::value_type& elt : m_bufferInfos)
    {
        m_objectsUID.push_back(elt.first);

        std::string status     = "?";
        std::string date       = "?";
        std::string lockStatus = "?";

        const core::memory::buffer_info& dumpBuffInfo = elt.second;
        bool loaded                                   = dumpBuffInfo.loaded;
        if(!loaded)
        {
            backColor = Qt::darkYellow;
            status    = "Dumped";
        }
        else
        {
            backColor = Qt::white;
            status    = "-";
        }

        bool isLock = dumpBuffInfo.lock_count() > 0;
        if(isLock)
        {
            lockStatus = "locked(" + std::to_string(dumpBuffInfo.lock_count()) + ")";
        }
        else
        {
            lockStatus = "unlocked";
        }

        date = std::to_string(dumpBuffInfo.last_access.get_logic_stamp());

        QTableWidgetItem* currentSizeItem = new SizeTableWidgetItem(getHumanReadableSize(dumpBuffInfo.size));
        currentSizeItem->setData(Qt::UserRole, (qulonglong) dumpBuffInfo.size);
        currentSizeItem->setFlags(Qt::ItemIsEnabled);
        currentSizeItem->setBackground(backColor);
        m_list->setItem(itemCount, 0, currentSizeItem);

        auto* statusItem = new QTableWidgetItem(QString::fromStdString(status));
        statusItem->setFlags(Qt::ItemIsEnabled);
        statusItem->setBackground(backColor);
        m_list->setItem(itemCount, 1, statusItem);

        auto* dateItem = new QTableWidgetItem(QString::fromStdString(date));
        dateItem->setFlags(Qt::ItemIsEnabled);
        dateItem->setBackground(backColor);
        m_list->setItem(itemCount, 2, dateItem);

        auto* lockStatusItem = new QTableWidgetItem(QString::fromStdString(lockStatus));
        lockStatusItem->setFlags(Qt::ItemIsEnabled);
        lockStatusItem->setBackground(backColor);
        m_list->setItem(itemCount, 3, lockStatusItem);

        auto* actionItem = new QPushButton(QString::fromStdString((loaded) ? "Dump" : "Restore"), m_list);
        actionItem->setEnabled(!isLock && (dumpBuffInfo.size > 0));
        m_list->setCellWidget(itemCount, 4, actionItem);

        QObject::connect(actionItem, &QPushButton::pressed, this, [this, itemCount](){changeStatus(itemCount);});
        ++itemCount;
    }

    m_list->setSortingEnabled(true);

    m_infoEditor->reset();
    m_infoEditor->resizeColumnsToContents();
}

//------------------------------------------------------------------------------

void DumpEditor::info(std::ostream& _sstream)
{
    _sstream << "Dump Editor";
}

//------------------------------------------------------------------------------

void DumpEditor::onUpdate()
{
    m_updateTimer->start();
}

//------------------------------------------------------------------------------

void DumpEditor::onRefreshButton()
{
    this->updating();
}

//------------------------------------------------------------------------------

void DumpEditor::changeStatus(int index)
{
    core::memory::buffer_manager::sptr buffManager = core::memory::buffer_manager::get();
    if(buffManager)
    {
        const core::memory::buffer_manager::buffer_info_map_t buffInfoMap = m_bufferInfos;
        const auto* selectedBuffer                                        = m_objectsUID[std::size_t(index)];

        core::memory::buffer_manager::buffer_info_map_t::const_iterator iter;
        iter = buffInfoMap.find(selectedBuffer);
        if(iter != buffInfoMap.end())
        {
            sight::ui::cursor cursor;
            cursor.setCursor(sight::ui::cursor_base::BUSY);
            const core::memory::buffer_info& dumpBuffInfo = iter->second;

            bool isLock = dumpBuffInfo.lock_count() > 0;
            if(!isLock)
            {
                if(dumpBuffInfo.loaded)
                {
                    buffManager->dump_buffer(selectedBuffer);
                }
                else
                {
                    buffManager->restore_buffer(selectedBuffer);
                }
            }
            else
            {
                sight::ui::dialog::message::show(
                    "Dump process information",
                    "Dump process is locked. It is impossible to dump or restore this object.",
                    sight::ui::dialog::message::WARNING
                );
            }

            cursor.setDefaultCursor();

            this->updating();
        }
        else
        {
            std::stringstream stream;
            stream << "Object " << selectedBuffer << " not found, please refresh the grid.";
            sight::ui::dialog::message::show(
                "Dump process information",
                stream.str(),
                sight::ui::dialog::message::WARNING
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::debug
