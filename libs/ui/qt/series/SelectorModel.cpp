/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "ui/qt/series/SelectorModel.hpp"

#include <core/runtime/operations.hpp>
#include <core/tools/fwID.hpp>

#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Series.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/math/special_functions/round.hpp>

#include <QFont>
#include <QPushButton>
#include <QStandardItem>
#include <QString>
#include <QTextCodec>
#include <QTreeView>

#include <regex>

namespace sight::ui::qt::series
{

//------------------------------------------------------------------------------

inline static bool isDate(const std::string& date)
{
    if(date.size() != 8)
    {
        return false;
    }

    for(std::size_t i = 0 ; i < 8 ; ++i)
    {
        if(std::isdigit(date[i]) == 0)
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

inline static bool isTime(const std::string& time)
{
    if(time.size() < 6)
    {
        return false;
    }

    for(std::size_t i = 0 ; i < 6 ; ++i)
    {
        if(std::isdigit(time[i]) == 0)
        {
            return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

inline static std::string formatTime(const std::string& time)
{
    if(isTime(time))
    {
        return time.substr(0, 2) + ":" + time.substr(2, 2) + ":" + time.substr(4, 2);
    }

    return time;
}

//------------------------------------------------------------------------------

inline static std::string formatDate(const std::string& date)
{
    if(isDate(date))
    {
        return date.substr(4, 2) + "/" + date.substr(6, 2) + "/" + date.substr(0, 4);
    }

    return date;
}

//-----------------------------------------------------------------------------

SelectorModel::SelectorModel(QWidget* _parent) :
    QStandardItemModel(_parent)
{
    this->init();
}

//-----------------------------------------------------------------------------

void SelectorModel::init()
{
    m_studyRowCount = 0;
    m_items.clear();

    QStringList headers;
    headers << "Name" << "Sex" << "Birthdate"
    << "Modality"
    << "Description"
    << "Date" << "Time"
    << "Patient age"
    << "Body part examined" << "Patient position" << "Contrast agent"
    << "Acquisition time" << "Contrast/bolus time";

    if(m_removeAllowed)
    {
        headers << "Remove";
    }

    this->setHorizontalHeaderLabels(headers);
}

//-----------------------------------------------------------------------------

SelectorModel::ItemType SelectorModel::getItemType(const QModelIndex& _index)
{
    QModelIndex idx     = this->createIndex(_index.row(), 0, _index.internalPointer());
    QStandardItem* item = this->itemFromIndex(idx);
    return (SelectorModel::ItemType) item->data(Role::ITEM_TYPE).toInt();
}

//-----------------------------------------------------------------------------

void SelectorModel::clear()
{
    this->QStandardItemModel::clear();
    this->init();
}

//-----------------------------------------------------------------------------

data::Image::Spacing roundSpacing(const data::Image::Spacing& _spacing)
{
    data::Image::Spacing roundSpacing;
    for(std::size_t i = 0 ; i < 3 ; ++i)
    {
        data::Image::Spacing::value_type roundVal = boost::math::round(_spacing[i] * 100.) / 100.;
        roundSpacing[i] = roundVal;
    }

    return roundSpacing;
}

//------------------------------------------------------------------------------

void SelectorModel::addSeries(data::Series::sptr _series)
{
    const auto studyInstanceUID  = _series->getStudyInstanceUID();
    auto itr                     = m_items.find(studyInstanceUID);
    QStandardItem* studyRootItem = nullptr;

    // Get the text codec for the current series.
    QTextCodec* codec = QTextCodec::codecForName(_series->getEncoding().c_str());

    // Fallback
    if(codec == nullptr)
    {
        codec = QTextCodec::codecForName("UTF-8");
    }

    if(itr != m_items.end())
    {
        studyRootItem = itr->second;
    }
    else
    {
        auto* patientName = new QStandardItem(codec->toUnicode(_series->getPatientName().c_str()));

        patientName->setData(QVariant((int) ItemType::STUDY), Role::ITEM_TYPE);
        patientName->setData(QVariant(QString::fromStdString(studyInstanceUID)), Role::UID);

        auto* patientSex = new QStandardItem(QString::fromStdString(_series->getPatientSex()));

        const std::string& birthDate = formatDate(_series->getPatientBirthDate());

        auto* patientBirthdate = new QStandardItem(QString::fromStdString(birthDate));

        auto* studyDescription = new QStandardItem(codec->toUnicode(_series->getStudyDescription().c_str()));

        const std::string& studyDate = formatDate(_series->getStudyDate());

        auto* studyDateItem = new QStandardItem(QString::fromStdString(studyDate));

        const std::string& studyTime = formatTime(_series->getStudyTime());

        auto* studyTimeItem = new QStandardItem(QString::fromStdString(studyTime));

        auto* studyPatientAge = new QStandardItem(QString::fromStdString(_series->getPatientAge()));

        this->setItem(m_studyRowCount, int(ColumnSeriesType::NAME), patientName);
        this->setItem(m_studyRowCount, int(ColumnSeriesType::SEX), patientSex);
        this->setItem(m_studyRowCount, int(ColumnSeriesType::BIRTHDATE), patientBirthdate);
        this->setItem(m_studyRowCount, int(ColumnSeriesType::MODALITY), new QStandardItem());
        this->setItem(m_studyRowCount, int(ColumnSeriesType::DESCRIPTION), studyDescription);
        this->setItem(m_studyRowCount, int(ColumnSeriesType::DATE), studyDateItem);
        this->setItem(m_studyRowCount, int(ColumnSeriesType::TIME), studyTimeItem);
        this->setItem(m_studyRowCount, int(ColumnSeriesType::PATIENT_AGE), studyPatientAge);
        this->setItem(m_studyRowCount, int(ColumnSeriesType::BODY_PART_EXAMINED), new QStandardItem());
        this->setItem(m_studyRowCount, int(ColumnSeriesType::PATIENT_POSITION), new QStandardItem());
        this->setItem(m_studyRowCount, int(ColumnSeriesType::CONTRAST_AGENT), new QStandardItem());
        this->setItem(m_studyRowCount, int(ColumnSeriesType::ACQUISITION_TIME), new QStandardItem());
        this->setItem(m_studyRowCount, int(ColumnSeriesType::CONTRAST_BOLUS_START_TIME), new QStandardItem());

        // Add a remove button to each studies.
        if(m_removeAllowed && !m_removeStudyIcon.empty())
        {
            this->setItem(m_studyRowCount, int(ColumnSeriesType::REMOVE), new QStandardItem());

            auto* const selector = static_cast<QTreeView*>(this->parent());
            SIGHT_ASSERT("The QTreeView parent must be given to the constructor", selector);

            auto* const removeButton = new QPushButton(QIcon(m_removeStudyIcon.string().c_str()), "");
            selector->setIndexWidget(this->index(m_studyRowCount, int(ColumnSeriesType::REMOVE)), removeButton);

            // When the remove button is clicked, emit a signal with the study UID.
            QObject::connect(
                removeButton,
                &QPushButton::clicked,
                this,
                [ =, this]()
                {
                    Q_EMIT removeStudyInstanceUID(studyInstanceUID);
                });
        }

        m_studyRowCount++;
        studyRootItem             = patientName;
        m_items[studyInstanceUID] = studyRootItem;
    }

    const std::string& seriesID = _series->getID();

    const auto& seriesInstanceUID = _series->getSeriesInstanceUID();
    auto* seriesRole              = new QStandardItem(
        seriesInstanceUID.empty()
        ? QString::fromStdString(seriesID)
        : QString::fromStdString(seriesInstanceUID)
    );

    seriesRole->setData(QVariant((int) ItemType::SERIES), Role::ITEM_TYPE);
    seriesRole->setData(QVariant(QString::fromStdString(seriesID)), Role::UID);

    auto* seriesIcon     = new QStandardItem();
    auto* seriesModality = new QStandardItem(QString::fromStdString(_series->getModality()));

    auto* seriesDescription = new QStandardItem(
        [&]
        {
            const std::string& description = _series->getSeriesDescription();

            if(!description.empty())
            {
                return codec->toUnicode(description.c_str());
            }

            // If there is no description, build some information string from the series.
            std::string infos   = _series->getSOPClassName();
            const auto& rows    = _series->getRows();
            const auto& columns = _series->getColumns();
            if(rows && columns)
            {
                infos += " (" + std::to_string(*rows) + "x" + std::to_string(*columns) + ")";
            }

            return QString::fromStdString(infos);
        }()
    );

    const std::string& seriesDate = formatDate(_series->getSeriesDate());

    auto* seriesDateItem = new QStandardItem(QString::fromStdString(seriesDate));

    const std::string& seriesTime = formatTime(_series->getSeriesTime());

    auto* seriesTimeItem = new QStandardItem(QString::fromStdString(seriesTime));

    const int nbRow = studyRootItem->rowCount();
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::NAME), seriesRole);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::BIRTHDATE), seriesIcon);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::MODALITY), seriesModality);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::DESCRIPTION), seriesDescription);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::DATE), seriesDateItem);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::TIME), seriesTimeItem);

    // Add a remove button to each series.
    if(m_removeAllowed && !m_removeSeriesIcon.empty())
    {
        auto* const removeItem = new QStandardItem();
        studyRootItem->setChild(nbRow, int(ColumnSeriesType::REMOVE), removeItem);

        auto* const selector = static_cast<QTreeView*>(this->parent());
        SIGHT_ASSERT("The QTreeView parent must be given to the constructor", selector);

        auto* const removeButton = new QPushButton(QIcon(m_removeSeriesIcon.string().c_str()), "");
        selector->setIndexWidget(this->indexFromItem(removeItem), removeButton);

        // When the remove button is clicked, emit a signal with the study UID.
        QObject::connect(
            removeButton,
            &QPushButton::clicked,
            this,
            [ =, this]()
            {
                Q_EMIT removeSeriesID(seriesID);
            });
    }

    if(_series->getDicomType() == data::Series::DicomType::IMAGE)
    {
        studyRootItem->setChild(
            nbRow,
            int(ColumnSeriesType::BODY_PART_EXAMINED),
            new QStandardItem(QString::fromStdString(_series->getBodyPartExamined()))
        );

        const std::string& patientPosition = _series->getPatientPositionString();

        studyRootItem->setChild(
            nbRow,
            int(ColumnSeriesType::PATIENT_POSITION),
            new QStandardItem(QString::fromStdString(patientPosition))
        );

        studyRootItem->setChild(
            nbRow,
            int(ColumnSeriesType::CONTRAST_AGENT),
            new QStandardItem(QString::fromStdString(_series->getContrastBolusAgent()))
        );

        const std::string& acquisitionTime = formatTime(_series->getAcquisitionTime());

        studyRootItem->setChild(
            nbRow,
            int(ColumnSeriesType::ACQUISITION_TIME),
            new QStandardItem(QString::fromStdString(acquisitionTime))
        );

        const std::string& contrastTime = formatTime(_series->getContrastBolusStartTime());

        studyRootItem->setChild(
            nbRow,
            int(ColumnSeriesType::CONTRAST_BOLUS_START_TIME),
            new QStandardItem(QString::fromStdString(contrastTime))
        );
    }

    if(m_insert)
    {
        const int nbColumns = studyRootItem->columnCount();
        for(int i = 0 ; i < nbColumns ; ++i)
        {
            QStandardItem* item = studyRootItem->child(nbRow, i);
            if(item == nullptr)
            {
                studyRootItem->setChild(nbRow, i, new QStandardItem());
                item = studyRootItem->child(nbRow, i);
            }

            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
        }
    }

    this->addSeriesIcon(_series, seriesIcon);
}

//-----------------------------------------------------------------------------

void SelectorModel::addSeriesIcon(data::Series::sptr _series, QStandardItem* _item)
{
    auto iter = m_seriesIcons.find(_series->getClassname());
    if(iter != m_seriesIcons.end())
    {
        _item->setIcon(QIcon(QString::fromStdString(iter->second)));
    }
    else
    {
        std::filesystem::path icon_path;

        if(const auto& type = _series->getDicomType(); type == data::Series::DicomType::IMAGE)
        {
            icon_path = core::runtime::getModuleResourceFilePath("sight::module::ui::icons", "ImageSeries.svg");
        }
        else if(type == data::Series::DicomType::MODEL)
        {
            icon_path = core::runtime::getModuleResourceFilePath("sight::module::ui::icons", "ModelSeries.svg");
        }

        if(!icon_path.empty())
        {
            _item->setIcon(QIcon(QString::fromStdString(icon_path.string())));
        }
    }
}

//-----------------------------------------------------------------------------

void SelectorModel::removeSeries(data::Series::sptr _series)
{
    QStandardItem* seriesItem = this->findSeriesItem(_series);

    if(seriesItem != nullptr)
    {
        this->removeSeriesItem(seriesItem);
    }
}

//-----------------------------------------------------------------------------

QModelIndex SelectorModel::getIndex(const QModelIndex& _index, int _column)
{
    QModelIndex idx = this->createIndex(_index.row(), _column, _index.internalPointer());
    return idx;
}

//-----------------------------------------------------------------------------

void SelectorModel::removeRows(const QModelIndexList _indexes)
{
    QList<QStandardItem*> seriesItems;
    QList<QStandardItem*> studyItems;

    for(QModelIndex index : _indexes)
    {
        SIGHT_ASSERT("Index must be in the name column.", index.column() == int(ColumnSeriesType::NAME));
        QStandardItem* item = this->itemFromIndex(index);
        if(item->data(Role::ITEM_TYPE) == ItemType::STUDY)
        {
            studyItems.append(item);
        }
        else if(item->data(Role::ITEM_TYPE) == ItemType::SERIES)
        {
            seriesItems.append(item);
        }
    }

    // Remove series items from selector
    for(QStandardItem* item : seriesItems)
    {
        QStandardItem* studyItem = item->parent();

        // Remove series item if it is not included in a study which will be remove.
        if(std::find(studyItems.begin(), studyItems.end(), studyItem) == studyItems.end())
        {
            this->removeSeriesItem(item);
        }
    }

    // Remove study items from selector
    for(QStandardItem* item : studyItems)
    {
        this->removeStudyItem(item);
    }
}

//-----------------------------------------------------------------------------

bool SelectorModel::removeStudyItem(QStandardItem* _item)
{
    bool isRemoved = false;
    SIGHT_ASSERT("Index must represent a study.", _item->data(Role::ITEM_TYPE) == ItemType::STUDY);
    QString uid                      = _item->data(Role::UID).toString();
    data::DicomValueType instanceUID = uid.toStdString();

    isRemoved = this->QStandardItemModel::removeRow(_item->row());
    SIGHT_ASSERT("Remove can not be done!", isRemoved);
    m_items.erase(instanceUID);
    --m_studyRowCount;

    return isRemoved;
}

//-----------------------------------------------------------------------------

bool SelectorModel::removeSeriesItem(QStandardItem* _item)
{
    bool isRemoved = false;

    SIGHT_ASSERT("Index must represent series", _item->data(Role::ITEM_TYPE) == ItemType::SERIES);
    QStandardItem* parent = _item->parent();
    isRemoved = this->QStandardItemModel::removeRow(_item->row(), this->indexFromItem(parent));
    SIGHT_ASSERT("Remove can not be done!", isRemoved);
    if(parent != nullptr && parent->rowCount() == 0)
    {
        this->removeStudyItem(parent);
    }

    return isRemoved;
}

//-----------------------------------------------------------------------------

QStandardItem* SelectorModel::findSeriesItem(data::Series::sptr _series)
{
    QStandardItem* seriesItem = nullptr;
    QStandardItem* studyItem  = this->findStudyItem(_series);

    if(studyItem != nullptr)
    {
        int nbRow = studyItem->rowCount();
        for(int row = 0 ; row < nbRow ; ++row)
        {
            QStandardItem* child = studyItem->child(row, int(ColumnSeriesType::NAME));
            std::string seriesId = child->data(Role::UID).toString().toStdString();
            if(seriesId == _series->getID())
            {
                seriesItem = child;
                break;
            }
        }
    }

    return seriesItem;
}

//-----------------------------------------------------------------------------

QStandardItem* SelectorModel::findStudyItem(data::Series::sptr _series)
{
    data::DicomValueType studyInstanceUid = _series->getStudyInstanceUID();

    QStandardItem* studyItem = m_items[studyInstanceUid];
    return studyItem;
}

//-----------------------------------------------------------------------------

void SelectorModel::setSeriesIcons(const SeriesIconType& _seriesIcons)
{
    m_seriesIcons = _seriesIcons;
}

} // namespace sight::ui::qt::series
