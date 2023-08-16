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

#include "ui/qt/series/SelectorModel.hpp"

#include <core/runtime/path.hpp>
#include <core/tools/fwID.hpp>

#include <data/dicom/Sop.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Series.hpp>

#include <boost/algorithm/string/split.hpp>
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

enum class When
{
    STUDY,
    SERIES
};

struct ColumnDisplayInformation
{
    std::string header;
    std::function<QStandardItem* (data::Series::csptr, When)> getInfo;
};

/* *INDENT-OFF* */
static const std::map<std::string, ColumnDisplayInformation> columnMap {
    {"PatientName", {.header = "Name", .getInfo =
        [](data::Series::csptr series, When when)
        {
            if(when == When::SERIES)
            {
                return new QStandardItem;
            }

            QTextCodec* codec = QTextCodec::codecForName(series->getEncoding().c_str());
            if(codec == nullptr)
            {
                codec = QTextCodec::codecForName("UTF-8");
            }

            QString res = codec->toUnicode(series->getPatientName().c_str());
            QString upperPatientName = res.toUpper();
            if(upperPatientName.isEmpty() || upperPatientName.contains("ANONYMIZED") || upperPatientName == "UNKNOWN"
                || upperPatientName.contains("ANONYMOUS") || upperPatientName == "NONE" || upperPatientName == "NA")
            {
                // Automatically switch to Patient ID if the Patient Name is anonymized
                res = QString::fromStdString(series->getPatientID());
            }

            return new QStandardItem(res);
        }
     }
    },
    {"SeriesInstanceUID", {.header = "Name", .getInfo =
        [](data::Series::csptr series, When when)
        {
            if(when == When::STUDY)
            {
                return new QStandardItem;
            }

            if(std::string seriesInstanceUID = series->getSeriesInstanceUID();
                !seriesInstanceUID.empty())
            {
                return new QStandardItem(QString::fromStdString(seriesInstanceUID));
            }

            return new QStandardItem(QString::fromStdString(series->getID()));
        }
     }
    },
    {"PatientName/SeriesInstanceUID", {.header = "Name", .getInfo =
        [](data::Series::csptr series, When when)
        {
            if(when == When::STUDY)
            {
                QTextCodec* codec = QTextCodec::codecForName(series->getEncoding().c_str());
                if(codec == nullptr)
                {
                    codec = QTextCodec::codecForName("UTF-8");
                }

                QString res = codec->toUnicode(series->getPatientName().c_str());
                QString upperPatientName = res.toUpper();
                if(upperPatientName.isEmpty() || upperPatientName.contains("ANONYMIZED") || upperPatientName == "UNKNOWN"
                    || upperPatientName.contains("ANONYMOUS") || upperPatientName == "NONE" || upperPatientName == "NA")
                {
                    // Automatically switch to Patient ID if the Patient Name is anonymized
                    res = QString::fromStdString(series->getPatientID());
                }

                return new QStandardItem(res);
            }

            if(std::string seriesInstanceUID = series->getSeriesInstanceUID(); !seriesInstanceUID.empty())
            {
                return new QStandardItem(QString::fromStdString(seriesInstanceUID));
            }

            return new QStandardItem(QString::fromStdString(series->getID()));
        }
     }
    },
    {"PatientSex", {.header = "Sex", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(when == When::STUDY ? series->getPatientSex() : ""));
        }
     }
    },
    {"PatientBirthDate", {.header = "Birthdate", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(
                QString::fromStdString(when == When::STUDY ? formatDate(series->getPatientBirthDate()) : ""));
        }
     }
    },
    {"Icon", {.header = "Icon", .getInfo =
        [](data::Series::csptr /*series*/, When when)
        {
            if(when == When::STUDY)
            {
                return new QStandardItem;
            }

            auto* item = new QStandardItem;
            item->setData(true, SelectorModel::Role::ICON);
            return item;
        }
     }
    },
    {"PatientBirthDate/Icon", {.header = "Birthdate", .getInfo =
        [](data::Series::csptr series, When when)
        {
            if(when == When::STUDY)
            {
                return new QStandardItem(QString::fromStdString(formatDate(series->getPatientBirthDate())));
            }

            auto* item = new QStandardItem;
            item->setData(true, SelectorModel::Role::ICON);
            return item;
        }
     }
    },
    {"Modality", {.header = "Modality", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(when == When::SERIES ? series->getModality() : ""));
        }
     }
    },
    {"StudyDescription", {.header = "Description", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(when == When::STUDY ? series->getStudyDescription() : ""));
        }
     }
    },
    {"SeriesDescription", {.header = "Description", .getInfo =
        [](data::Series::csptr series, When when)
        {
            if(when == When::STUDY)
            {
                return new QStandardItem;
            }

            std::string infos(series->getSOPClassName());
            const auto& rows    = series->getRows();
            const auto& columns = series->getColumns();
            const auto& frames  = series->numFrames();
            if(rows && columns)
            {
                infos += " ( " + std::to_string(*rows)
                        + "x" + std::to_string(*columns)
                        + "x" + std::to_string(frames) + " )";
            }

            const std::string& description = series->getSeriesDescription();

            if(!description.empty())
            {
                infos += ": " + description;
            }

            return new QStandardItem(QString::fromStdString(infos));
        }
     }
    },
    {"StudyDescription/SeriesDescription", {.header = "Description", .getInfo =
        [](data::Series::csptr series, When when)
        {
            if(when == When::STUDY)
            {
                return new QStandardItem(QString::fromStdString(series->getDescription()));
            }

            std::string infos(series->getSOPClassName());
            const auto& rows = series->getRows();
            const auto& columns = series->getColumns();
            const auto& frames = series->numFrames();
            if(rows && columns)
            {
                infos += " ( " + std::to_string(*rows)
                        + "x" + std::to_string(*columns)
                        + "x" + std::to_string(frames) + " )";
            }

            const std::string& description = series->getSeriesDescription();

            if(!description.empty())
            {
                infos += ": " + description;
            }

            return new QStandardItem(QString::fromStdString(infos));
        }
     }
    },
    {"StudyDate", {.header = "Date", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(
                QString::fromStdString(when == When::STUDY ? formatDate(series->getStudyDate()) : ""));
        }
     }
    },
    {"SeriesDate", {.header = "Date", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(
                QString::fromStdString(when == When::SERIES ? formatDate(series->getSeriesDate()) : ""));
        }
     }
    },
    {"StudyDate/SeriesDate", {.header = "Date", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(
                formatDate(when == When::STUDY ? series->getStudyDate() : series->getSeriesDate())));
        }
     }
    },
    {"StudyTime", {.header = "Time", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(
                QString::fromStdString(when == When::STUDY ? formatTime(series->getStudyTime()) : ""));
        }
     }
    },
    {"SeriesTime", {.header = "Time", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(
                QString::fromStdString(when == When::SERIES ? formatTime(series->getSeriesTime()) : ""));
        }
     }
    },
    {"StudyTime/SeriesTime", {.header = "Time", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(
                formatTime(when == When::STUDY ? series->getStudyTime() : series->getSeriesTime())));
        }
     }
    },
    {"PatientAge", {.header = "Patient age", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(when == When::STUDY ? series->getPatientAge() : ""));
        }
     }
    },
    {"BodyPartExamined", {.header = "Body part examined", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(
                when == When::SERIES && series->getDicomType() == data::Series::DicomType::IMAGE
                    ? series->getBodyPartExamined()
                    : ""));
        }
     }
    },
    {"PatientPositionString", {.header = "Patient position", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(
                when == When::SERIES && series->getDicomType() == data::Series::DicomType::IMAGE
                    ? series->getPatientPositionString()
                    : ""));
        }
     }
    },
    {"ContrastBolusAgent", {.header = "Contrast agent", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(
                when == When::SERIES && series->getDicomType() == data::Series::DicomType::IMAGE
                    ? series->getContrastBolusAgent()
                    : ""));
        }
     }
    },
    {"AcquisitionTime", {.header = "Acquisition time", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(
                when == When::SERIES && series->getDicomType() == data::Series::DicomType::IMAGE
                    ? formatTime(series->getAcquisitionTime())
                    : ""));
        }
     }
    },
    {"ContrastBolusStartTime", {.header = "Contrast/bolus time", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(
                when == When::SERIES && series->getDicomType() == data::Series::DicomType::IMAGE
                    ? formatTime(series->getContrastBolusStartTime())
                    : ""));
        }
     }
    },
    {"PatientID", {.header = "Patient ID", .getInfo =
        [](data::Series::csptr series, When when)
        {
            return new QStandardItem(QString::fromStdString(when == When::STUDY ? series->getPatientID() : ""));
        }
     }
    }
};
/* *INDENT-ON* */

//-----------------------------------------------------------------------------

SelectorModel::SelectorModel(const std::string& displayColumns, QWidget* _parent, bool allowRemove) :
    QStandardItemModel(_parent),
    m_removeAllowed(allowRemove)
{
    boost::split(m_displayColumns, displayColumns, boost::is_any_of(","));
    this->init();
}

//-----------------------------------------------------------------------------

void SelectorModel::init()
{
    m_studyRowCount = 0;
    m_items.clear();

    QStringList headers;
    for(const std::string& displayColumn : m_displayColumns)
    {
        SIGHT_ASSERT("Display column '" << displayColumn << "' unknown.", columnMap.contains(displayColumn));
        headers << QString::fromStdString(columnMap.at(displayColumn).header);
    }

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

    if(itr != m_items.end())
    {
        studyRootItem = itr->second;
    }
    else
    {
        std::size_t i = 0;
        for(const std::string& displayColumn : m_displayColumns)
        {
            setItem(m_studyRowCount, static_cast<int>(i), columnMap.at(displayColumn).getInfo(_series, When::STUDY));
            i++;
        }

        item(m_studyRowCount)->setData(ItemType::STUDY, Role::ITEM_TYPE);
        item(m_studyRowCount)->setData(QString::fromStdString(studyInstanceUID), Role::UID);

        // Add a remove button to each studies.
        if(m_removeAllowed && !m_removeStudyIcon.empty())
        {
            this->setItem(m_studyRowCount, static_cast<int>(i), new QStandardItem());

            auto* const selector = static_cast<QTreeView*>(this->parent());
            SIGHT_ASSERT("The QTreeView parent must be given to the constructor", selector);

            auto* const removeButton = new QPushButton(QIcon(m_removeStudyIcon.string().c_str()), "");
            selector->setIndexWidget(this->index(m_studyRowCount, static_cast<int>(i)), removeButton);

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

        studyRootItem = item(m_studyRowCount);
        m_studyRowCount++;
        m_items[studyInstanceUID] = studyRootItem;
    }

    const std::string& seriesID = _series->getID();

    const int nbRow = studyRootItem->rowCount();

    QStandardItem* seriesIcon = nullptr;
    std::size_t i             = 0;
    for(const std::string& displayColumn : m_displayColumns)
    {
        QStandardItem* item = columnMap.at(displayColumn).getInfo(_series, When::SERIES);
        studyRootItem->setChild(nbRow, static_cast<int>(i), item);
        if(item->data(Role::ICON).toBool())
        {
            seriesIcon = item;
        }

        i++;
    }

    studyRootItem->child(nbRow)->setData(ItemType::SERIES, Role::ITEM_TYPE);
    studyRootItem->child(nbRow)->setData(QString::fromStdString(seriesID), Role::UID);

    // Add a remove button to each series.
    if(m_removeAllowed && !m_removeSeriesIcon.empty())
    {
        auto* const removeItem = new QStandardItem();
        studyRootItem->setChild(nbRow, static_cast<int>(i), removeItem);

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

    if(m_insert)
    {
        const int nbColumns = studyRootItem->columnCount();
        for(int j = 0 ; j < nbColumns ; ++j)
        {
            QStandardItem* item = studyRootItem->child(nbRow, j);
            if(item == nullptr)
            {
                studyRootItem->setChild(nbRow, j, new QStandardItem());
                item = studyRootItem->child(nbRow, j);
            }

            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
        }
    }

    if(seriesIcon != nullptr)
    {
        this->addSeriesIcon(_series, seriesIcon);
    }
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
        SIGHT_ASSERT("Index must be in the first column.", index.column() == 0);
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
            QStandardItem* child = studyItem->child(row, 0);
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
