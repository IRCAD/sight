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

#include "DumpEditor.hpp"

#include <core/base.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/memory/BufferManager.hpp>
#include <core/memory/ByteSize.hpp>
#include <core/memory/IPolicy.hpp>
#include <core/memory/tools/MemoryMonitorTools.hpp>
#include <core/thread/ActiveWorkers.hpp>
#include <core/tools/fwID.hpp>
#include <core/tools/Stringizer.hpp>

#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/IMessageDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/qt/container/QtContainer.hpp>

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

core::memory::BufferManager::BufferInfoMapType m_bufferInfos;
core::memory::BufferManager::BufferStats m_bufferStats = {0, 0};

//------------------------------------------------------------------------------

QString getHumanReadableSize(core::memory::ByteSize::SizeType bytes)
{
    return QString::fromStdString(core::memory::ByteSize(bytes));
}

//------------------------------------------------------------------------------

class PolicyComboBoxDelegate : public QItemDelegate
{
public:

    PolicyComboBoxDelegate(QObject* parent = 0) :
        QItemDelegate(parent)
    {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

//------------------------------------------------------------------------------

QWidget* PolicyComboBoxDelegate::createEditor(
    QWidget* parent,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
) const
{
    QComboBox* policyComboBox = new QComboBox(parent);

    const std::string value = index.model()->data(index, Qt::DisplayRole).toString().toStdString();

    const core::memory::policy::registry::Type::KeyVectorType& factories =
        core::memory::policy::registry::get()->getFactoryKeys();

    for(const core::memory::policy::registry::KeyType& policy : factories)
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

    QComboBox* policyComboBox = static_cast<QComboBox*>(editor);

    int idx = policyComboBox->findText(value);
    if(idx != -1)
    {
        policyComboBox->setCurrentIndex(idx);
    }
}

//------------------------------------------------------------------------------

void PolicyComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* policyComboBox = static_cast<QComboBox*>(editor);
    QString value             = policyComboBox->currentText();

    model->setData(index, value, Qt::EditRole);
}

//------------------------------------------------------------------------------

void PolicyComboBoxDelegate::updateEditorGeometry(
    QWidget* editor,
    const QStyleOptionViewItem& option,
    const QModelIndex& index
) const
{
    editor->setGeometry(option.rect);
}

class PolicyTableModel : public QAbstractTableModel
{
public:

    PolicyTableModel(QObject* parent = 0);

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    static const int s_EXTRA_INFO_NB;

private:

    core::memory::BufferManager::sptr m_buffManager;
};

const int PolicyTableModel::s_EXTRA_INFO_NB = 1;

PolicyTableModel::PolicyTableModel(QObject* parent) :
    QAbstractTableModel(parent)
{
    m_buffManager = core::memory::BufferManager::getDefault();
}

//------------------------------------------------------------------------------

int PolicyTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    size_t nbParam = 0;
    if(m_buffManager)
    {
        core::mt::ReadLock lock(m_buffManager->getMutex());
        core::memory::IPolicy::sptr currentPolicy = m_buffManager->getDumpPolicy();
        nbParam = currentPolicy->getParamNames().size();
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
        return QVariant();
    }

    core::mt::ReadLock lock(m_buffManager->getMutex());
    core::memory::IPolicy::sptr currentPolicy = m_buffManager->getDumpPolicy();

    if(index.row() > (s_EXTRA_INFO_NB + currentPolicy->getParamNames().size()) || index.row() < 0)
    {
        return QVariant();
    }

    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            const core::memory::IPolicy::ParamNamesType& names = currentPolicy->getParamNames();
            if(index.row() == 0)
            {
                return QString::fromStdString(currentPolicy->getLeafClassname());
            }
            else if((unsigned int) index.row() <= names.size())
            {
                const core::memory::IPolicy::ParamNamesType::value_type& name = names.at(index.row() - 1);

                return QString::fromStdString(currentPolicy->getParam(name));
            }
        }
    }

    return QVariant();
}

//------------------------------------------------------------------------------

QVariant PolicyTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
    {
        return QVariant();
    }

    if(m_buffManager && orientation == Qt::Vertical)
    {
        core::mt::ReadLock lock(m_buffManager->getMutex());
        core::memory::IPolicy::sptr currentPolicy          = m_buffManager->getDumpPolicy();
        const core::memory::IPolicy::ParamNamesType& names = currentPolicy->getParamNames();
        if(section <= 0)
        {
            return QString("Current policy");
        }
        else if((unsigned int) section <= names.size())
        {
            const core::memory::IPolicy::ParamNamesType::value_type& name = names.at(section - 1);
            return QString::fromStdString(name);
        }
    }

    return QVariant();
}

//------------------------------------------------------------------------------

bool PolicyTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(m_buffManager && index.isValid() && role == Qt::EditRole)
    {
        int row                    = index.row();
        int col                    = index.column();
        const std::string strvalue = value.toString().toStdString();

        core::mt::ReadLock lock(m_buffManager->getMutex());
        core::memory::IPolicy::sptr currentPolicy          = m_buffManager->getDumpPolicy();
        const core::memory::IPolicy::ParamNamesType& names = currentPolicy->getParamNames();

        if(col == 0 && (unsigned int) row <= names.size())
        {
            core::memory::IPolicy::sptr dumpPolicy;
            switch(row)
            {
                case 0:
                    if(strvalue != currentPolicy->getLeafClassname())
                    {
                        dumpPolicy = core::memory::policy::registry::get()->create(strvalue);
                        if(dumpPolicy)
                        {
                            core::mt::ReadToWriteLock lock(m_buffManager->getMutex());
                            m_buffManager->setDumpPolicy(dumpPolicy);
                        }

                        this->beginResetModel();
                        this->endResetModel();
                    }

                    break;

                default:
                    const core::memory::IPolicy::ParamNamesType::value_type& name = names.at(row - 1);
                    currentPolicy->setParam(name, strvalue);
                    return true;
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

    InfoTableModel(QObject* parent = 0);

    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:

    core::memory::BufferManager::sptr m_buffManager;
};

InfoTableModel::InfoTableModel(QObject* parent) :
    QAbstractTableModel(parent)
{
    m_buffManager = core::memory::BufferManager::getDefault();
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
        return QVariant();
    }

    if(index.row() > this->rowCount(index))
    {
        return QVariant();
    }

    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
        {
            std::uint64_t sysMem;
            core::memory::BufferManager::SizeType bufferManagerMem;
            switch(index.row())
            {
                case 0:
                    sysMem = core::memory::tools::MemoryMonitorTools::getTotalSystemMemory();
                    return QString(getHumanReadableSize(sysMem));

                    break;

                case 1:
                    sysMem = core::memory::tools::MemoryMonitorTools::getFreeSystemMemory();
                    return QString(getHumanReadableSize(sysMem));

                    break;

                case 2:
                    bufferManagerMem = m_bufferStats.totalManaged;
                    return QString(getHumanReadableSize(bufferManagerMem));

                    break;

                case 3:
                    bufferManagerMem = m_bufferStats.totalDumped;
                    return QString(getHumanReadableSize(bufferManagerMem));

                    break;
            }
        }
    }

    return QVariant();
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
        }
    }

    return QVariant();
}

//------------------------------------------------------------------------------

DumpEditor::DumpEditor() noexcept
{
}

//------------------------------------------------------------------------------

DumpEditor::~DumpEditor() noexcept
{
}

//------------------------------------------------------------------------------

void DumpEditor::starting()
{
    this->sight::ui::base::IGuiContainer::create();

    auto qtContainer = sight::ui::qt::container::QtContainer::dynamicCast(this->getContainer());

    m_updateTimer = new QTimer(qtContainer->getQtContainer());
    m_updateTimer->setInterval(300);
    m_updateTimer->setSingleShot(true);

    m_list   = new QTableWidget();
    m_mapper = new QSignalMapper();

    m_list->setColumnCount(5);
    QStringList header;
    header.push_back("Size");
    header.push_back("Status");
    header.push_back("Timestamp");
    header.push_back("Locked");
    header.push_back("Action");
    m_list->setHorizontalHeaderLabels(header);

    m_refresh = new QPushButton(tr("Refresh"));
    QVBoxLayout* sizer = new QVBoxLayout();

    QHBoxLayout* sizerButton = new QHBoxLayout();
    sizerButton->addWidget(m_refresh);

    sizerButton->addItem(new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    QFrame* verticalLine = new QFrame();
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setFrameShadow(QFrame::Sunken);
    sizerButton->addWidget(verticalLine);

    sizer->addLayout(sizerButton);
    sizer->addWidget(m_list, 2);

    m_policyEditor = new QTableView();
    PolicyComboBoxDelegate* policyComboBoxDelegate = new PolicyComboBoxDelegate(m_policyEditor);
    PolicyTableModel* policyTableModel             = new PolicyTableModel(m_policyEditor);
    m_policyEditor->setModel(policyTableModel);
    m_policyEditor->setItemDelegateForRow(0, policyComboBoxDelegate);
    m_policyEditor->setSortingEnabled(false);
    m_policyEditor->horizontalHeader()->hide();

    InfoTableModel* infoTableModel = new InfoTableModel();
    m_infoEditor = new QTableView();
    m_infoEditor->setModel(infoTableModel);
    m_infoEditor->horizontalHeader()->hide();

    QHBoxLayout* tablesLayout = new QHBoxLayout();
    tablesLayout->addWidget(m_infoEditor);
    tablesLayout->addWidget(m_policyEditor);

    sizer->addLayout(tablesLayout);

    qtContainer->setLayout(sizer);

    QObject::connect(m_refresh, SIGNAL(clicked()), this, SLOT(onRefreshButton()));
    QObject::connect(m_mapper, SIGNAL(mapped(int)), this, SLOT(changeStatus(int)));

    QObject::connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(onRefreshButton()));
    QObject::connect(&m_watcher, SIGNAL(finished()), this, SLOT(onBufferInfo()));

    core::memory::BufferManager::sptr buffManager = core::memory::BufferManager::getDefault();
    if(buffManager)
    {
        m_updateSlot = core::com::newSlot(&DumpEditor::onUpdate, this);
        core::thread::ActiveWorkers::sptr workers = core::thread::ActiveWorkers::getDefault();
        m_updateSlot->setWorker(workers->getWorker(core::thread::ActiveWorkers::s_DEFAULT_WORKER));
        m_connection = buffManager->getUpdatedSignal()->connect(m_updateSlot);
    }

    this->updating();
}

//------------------------------------------------------------------------------

void DumpEditor::stopping()
{
    m_connection.disconnect();
    QObject::disconnect(m_refresh, SIGNAL(clicked()), this, SLOT(onRefreshButton()));
    QObject::disconnect(m_mapper, SIGNAL(mapped(int)), this, SLOT(changeStatus(int)));
    QObject::disconnect(&m_watcher, SIGNAL(finished()), this, SLOT(onBufferInfo()));

    this->destroy();
}

//------------------------------------------------------------------------------

void DumpEditor::configuring()
{
    this->sight::ui::base::IGuiContainer::initialize();
}

//------------------------------------------------------------------------------

class SizeTableWidgetItem : public QTableWidgetItem
{
public:

    SizeTableWidgetItem(const QString& text) :
        QTableWidgetItem(text)
    {
    }

    //------------------------------------------------------------------------------

    virtual bool operator<(const QTableWidgetItem& other) const
    {
        return data(Qt::UserRole).toULongLong() < other.data(Qt::UserRole).toULongLong();
    }

protected:

    size_t m_size;
};

//------------------------------------------------------------------------------

core::memory::BufferManager::BufferInfoMapType getInfoMap()
{
    core::memory::BufferManager::BufferInfoMapType infoMap;
    core::memory::BufferManager::sptr buffManager = core::memory::BufferManager::getDefault();
    if(buffManager)
    {
        infoMap = buffManager->getBufferInfos().get();
    }

    return infoMap;
}

//------------------------------------------------------------------------------

void DumpEditor::updating()
{
    m_policyEditor->reset();
    m_policyEditor->resizeColumnsToContents();

    QFuture<core::memory::BufferManager::BufferInfoMapType> qFuture = QtConcurrent::run(getInfoMap);
    m_watcher.setFuture(qFuture);
}

//------------------------------------------------------------------------------

void DumpEditor::onBufferInfo()
{
    m_bufferInfos = m_watcher.result();
    m_bufferStats = core::memory::BufferManager::computeBufferStats(m_bufferInfos);

    m_mapper->blockSignals(true);
    core::com::Connection::Blocker block(m_connection);

    for(int row = 0 ; row < m_list->rowCount() ; row++)
    {
        m_mapper->removeMappings(m_list->cellWidget(row, 4));
    }

    m_list->clearContents();
    m_objectsUID.clear();

    int itemCount = 0;
    m_list->setSortingEnabled(false);
    m_list->setRowCount(static_cast<int>(m_bufferInfos.size()));
    m_list->setColumnCount(5);
    QColor backColor;
    for(const core::memory::BufferManager::BufferInfoMapType::value_type& elt : m_bufferInfos)
    {
        m_objectsUID.push_back(elt.first);

        std::string status     = "?";
        std::string date       = "?";
        std::string lockStatus = "?";

        const core::memory::BufferInfo& dumpBuffInfo = elt.second;
        bool loaded                                  = dumpBuffInfo.loaded;
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

        bool isLock = dumpBuffInfo.lockCount() > 0;
        if(isLock)
        {
            lockStatus = "locked(" + core::tools::getString(dumpBuffInfo.lockCount()) + ")";
        }
        else
        {
            lockStatus = "unlocked";
        }

        date = core::tools::getString(dumpBuffInfo.lastAccess.getLogicStamp());

        QTableWidgetItem* currentSizeItem = new SizeTableWidgetItem(getHumanReadableSize(dumpBuffInfo.size));
        currentSizeItem->setData(Qt::UserRole, (qulonglong) dumpBuffInfo.size);
        currentSizeItem->setFlags(Qt::ItemIsEnabled);
        currentSizeItem->setBackgroundColor(backColor);
        m_list->setItem(itemCount, 0, currentSizeItem);

        QTableWidgetItem* statusItem = new QTableWidgetItem(QString::fromStdString(status));
        statusItem->setFlags(Qt::ItemIsEnabled);
        statusItem->setBackgroundColor(backColor);
        m_list->setItem(itemCount, 1, statusItem);

        QTableWidgetItem* dateItem = new QTableWidgetItem(QString::fromStdString(date));
        dateItem->setFlags(Qt::ItemIsEnabled);
        dateItem->setBackgroundColor(backColor);
        m_list->setItem(itemCount, 2, dateItem);

        QTableWidgetItem* lockStatusItem = new QTableWidgetItem(QString::fromStdString(lockStatus));
        lockStatusItem->setFlags(Qt::ItemIsEnabled);
        lockStatusItem->setBackgroundColor(backColor);
        m_list->setItem(itemCount, 3, lockStatusItem);

        QPushButton* actionItem = new QPushButton(QString::fromStdString((loaded) ? "Dump" : "Restore"), m_list);
        actionItem->setEnabled(!isLock && (dumpBuffInfo.size > 0));
        m_list->setCellWidget(itemCount, 4, actionItem);
        QObject::connect(actionItem, SIGNAL(pressed()), m_mapper, SLOT(map()));
        m_mapper->setMapping(actionItem, itemCount);

        ++itemCount;
    }

    m_list->setSortingEnabled(true);

    m_mapper->blockSignals(false);

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
    core::memory::BufferManager::sptr buffManager = core::memory::BufferManager::getDefault();
    if(buffManager)
    {
        const core::memory::BufferManager::BufferInfoMapType buffInfoMap = m_bufferInfos;
        core::memory::BufferManager::ConstBufferPtrType selectedBuffer   = m_objectsUID[index];

        core::memory::BufferManager::BufferInfoMapType::const_iterator iter;
        iter = buffInfoMap.find(selectedBuffer);
        if(iter != buffInfoMap.end())
        {
            sight::ui::base::Cursor cursor;
            cursor.setCursor(sight::ui::base::ICursor::BUSY);
            const core::memory::BufferInfo& dumpBuffInfo = iter->second;

            bool isLock = dumpBuffInfo.lockCount() > 0;
            if(!isLock)
            {
                if(dumpBuffInfo.loaded)
                {
                    buffManager->dumpBuffer(selectedBuffer);
                }
                else
                {
                    buffManager->restoreBuffer(selectedBuffer);
                }
            }
            else
            {
                sight::ui::base::dialog::MessageDialog::show(
                    "Dump process information",
                    "Dump process is locked. It is impossible to dump or restore this object.",
                    sight::ui::base::dialog::IMessageDialog::WARNING
                );
            }

            cursor.setDefaultCursor();

            this->updating();
        }
        else
        {
            std::stringstream stream;
            stream << "Object " << selectedBuffer << " not found, please refresh the grid.";
            sight::ui::base::dialog::MessageDialog::show(
                "Dump process information",
                stream.str(),
                sight::ui::base::dialog::IMessageDialog::WARNING
            );
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module
