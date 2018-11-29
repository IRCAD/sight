/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "uiMedDataQt/widget/SelectorModel.hpp"

#include "uiMedDataQt/InsertSeries.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwData/Image.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>

#include <fwRuntime/operations.hpp>

#include <fwTools/fwID.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/regex.hpp>

#include <QFont>
#include <QStandardItem>
#include <QString>

namespace uiMedDataQt
{
namespace widget
{

//-----------------------------------------------------------------------------

SelectorModel::SelectorModel(QWidget* parent) :
    QStandardItemModel(parent),
    m_studyRowCount(0),
    m_insert(false)
{
    this->init();
}

//-----------------------------------------------------------------------------

SelectorModel::~SelectorModel()
{
}

//-----------------------------------------------------------------------------

void SelectorModel::init()
{
    m_studyRowCount = 0;
    m_items.clear();

    QStringList headers;
    headers << "Patient name" << "Modality" << "Acquisition date" << "Image dimension" << "Voxel size"
            << "Patient position" << "Study description" <<  "Patient ID" << "Age"
            << "Referring physician / Performing physician " << "Sex" << "Birthdate" << "Institution";
    this->setHorizontalHeaderLabels(headers);
}

//-----------------------------------------------------------------------------

void SelectorModel::setInsertMode(bool insert)
{
    m_insert = insert;
}

//-----------------------------------------------------------------------------

SelectorModel::ItemType SelectorModel::getItemType(const QModelIndex& index)
{
    QModelIndex idx     = this->createIndex(index.row(), 0, index.internalPointer());
    QStandardItem* item = this->itemFromIndex(idx);
    return (SelectorModel::ItemType) item->data(SelectorModel::ITEM_TYPE).toInt();
}

//-----------------------------------------------------------------------------

void SelectorModel::clear()
{
    this->QStandardItemModel::clear();
    this->init();
}

//-----------------------------------------------------------------------------

::fwData::Image::SpacingType roundSpacing(const ::fwData::Image::SpacingType& spacing)
{
    ::fwData::Image::SpacingType roundSpacing;
    for(::fwData::Image::SpacingType::value_type val :  spacing)
    {
        ::fwData::Image::SpacingType::value_type roundVal = ::boost::math::round(val * 100.)/100.;
        roundSpacing.push_back(roundVal);
    }
    return roundSpacing;
}

//-----------------------------------------------------------------------------

std::string formatDate(const std::string& date)
{
    std::string formatDate = date;
    ::boost::algorithm::trim(formatDate);

    const std::string regexYyear = "[0-9]{4}";
    const std::string regexMonth = "[0-9]{2}";
    const std::string regexDay   = "[0-9]{2}";

    const std::string regexStr = "("+regexYyear+")"+"("+regexMonth+")"+"("+regexDay+")";
    ::boost::regex re(regexStr);
    ::boost::smatch match;
    if( ::boost::regex_match(formatDate, match, re) )
    {
        std::string year, month, day, hour, min, sec;
        OSLM_ASSERT("Wrong match for "<<formatDate, match.size() >= 4);
        year.assign(match[1].first, match[1].second);
        month.assign(match[2].first, match[2].second);
        day.assign(match[3].first, match[3].second);

        formatDate = year +  "/" + month + "/" + day;
    }

    return formatDate;
}

//-----------------------------------------------------------------------------

std::string formatTime(const std::string& time)
{
    std::string formatTime = time;
    ::boost::algorithm::trim(formatTime);

    const std::string regexHour = "[0-9]{2}";
    const std::string regexMin  = "[0-9]{2}";
    const std::string regexSec  = "[0-9]{2}";
    const std::string regexEnd  = "[.0-9]*";

    const std::string regexStr = "("+regexHour+")"+"("+regexMin+")"+"("+regexSec+")"+regexEnd;
    ::boost::regex re(regexStr);
    ::boost::smatch match;
    if( ::boost::regex_match(formatTime, match, re) )
    {
        std::string year, month, day, hour, min, sec;
        OSLM_ASSERT("Wrong match for "<<formatTime, match.size() >= 4);
        hour.assign(match[1].first, match[1].second);
        min.assign(match[2].first, match[2].second);
        sec.assign(match[3].first, match[3].second);

        formatTime = hour + ":" + min + ":" + sec;
    }

    return formatTime;
}

//-----------------------------------------------------------------------------

void SelectorModel::addSeries(::fwMedData::Series::sptr series)
{
    ::fwMedData::Study::sptr study       = series->getStudy();
    ::fwMedData::DicomValueType studyUID = study->getInstanceUID();
    StudyUidItemMapType::iterator itr = m_items.find(studyUID);
    QStandardItem* studyRootItem;

    if(itr != m_items.end())
    {
        studyRootItem = itr->second;
    }
    else
    {
        ::fwMedData::Patient::sptr patient     = series->getPatient();
        ::fwMedData::Equipment::sptr equipment = series->getEquipment();

        QStandardItem* patientName = new QStandardItem( QString::fromStdString(patient->getName()) );
        patientName->setData(QVariant((int)SelectorModel::STUDY), SelectorModel::ITEM_TYPE);
        patientName->setData(QVariant(QString::fromStdString(study->getInstanceUID())), UID);
        QStandardItem* patientId        = new QStandardItem( QString::fromStdString(patient->getPatientId()) );
        std::string birthDate           = formatDate(patient->getBirthdate());
        QStandardItem* patientBirthdate = new QStandardItem( QString::fromStdString(birthDate) );
        QStandardItem* patientSex       = new QStandardItem( QString::fromStdString(patient->getSex()) );

        std::string studyDateTime                  = formatDate(study->getDate()) + " " + formatTime(study->getTime());
        QStandardItem* studyDate                   = new QStandardItem( QString::fromStdString(studyDateTime));
        QStandardItem* studyReferringPhysicianName = new QStandardItem(
            QString::fromStdString(study->getReferringPhysicianName()));
        QStandardItem* studyDescription = new QStandardItem( QString::fromStdString(study->getDescription()));
        QStandardItem* studyPatientAge  = new QStandardItem( QString::fromStdString(study->getPatientAge()));

        QStandardItem* institution = new QStandardItem( QString::fromStdString(equipment->getInstitutionName()));

        this->setItem(m_studyRowCount, 0, patientName);
        this->setItem(m_studyRowCount, 1, new QStandardItem());
        this->setItem(m_studyRowCount, 2, studyDate);
        this->setItem(m_studyRowCount, 3, new QStandardItem());
        this->setItem(m_studyRowCount, 4, new QStandardItem());
        this->setItem(m_studyRowCount, 5, new QStandardItem());
        this->setItem(m_studyRowCount, 6, studyDescription);
        this->setItem(m_studyRowCount, 7, patientId);
        this->setItem(m_studyRowCount, 8, studyPatientAge);
        this->setItem(m_studyRowCount, 9, studyReferringPhysicianName);
        this->setItem(m_studyRowCount, 10, patientSex);
        this->setItem(m_studyRowCount, 11, patientBirthdate);
        this->setItem(m_studyRowCount, 12, institution);

        const int nbColumns = institution->index().column() + 1;
        for (int i = 0; i < nbColumns; ++i)
        {
            QStandardItem* item = this->item(m_studyRowCount, i);
            item->setFlags(item->flags() & ~Qt::ItemIsSelectable );
        }

        m_studyRowCount++;
        studyRootItem     = patientName;
        m_items[studyUID] = studyRootItem;
    }

    QStandardItem* seriesModality = new QStandardItem(QString::fromStdString(series->getModality()));
    std::string seriesDateTime    = formatDate(series->getDate()) + " " + formatTime(series->getTime());
    QStandardItem* seriesDate     = new QStandardItem( QString::fromStdString(seriesDateTime));

    QStandardItem* seriesPerformingPhysician =
        this->getInfo< ::fwMedData::DicomValuesType >(series->getPerformingPhysiciansName(), ", ");

    QStandardItem* seriesDescription1 = new QStandardItem(QString::fromStdString(series->getDescription()));
    seriesDescription1->setData(QVariant((int)SelectorModel::SERIES), SelectorModel::ITEM_TYPE);
    seriesDescription1->setData(QVariant(QString::fromStdString(series->getID())), UID);
    QStandardItem* seriesDescription2 = new QStandardItem(QString::fromStdString(series->getDescription()));

    const int nbRow = studyRootItem->rowCount();
    studyRootItem->setChild(nbRow, 0, seriesDescription1);
    studyRootItem->setChild(nbRow, 1, seriesModality);
    studyRootItem->setChild(nbRow, 2, seriesDate);
    studyRootItem->setChild(nbRow, 6, seriesDescription2);
    studyRootItem->setChild(nbRow, 9, seriesPerformingPhysician);
    studyRootItem->setChild(nbRow, 12, new QStandardItem());

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    if(imageSeries)
    {

        ::fwData::Image::sptr image = imageSeries->getImage();

        ::fwData::Image::SizeType imageNumber = image->getSize();
        QStandardItem* imageSize = this->getInfo< ::fwData::Image::SizeType>(imageNumber, " x ");
        studyRootItem->setChild(nbRow, 3, imageSize);

        ::fwData::Image::SpacingType voxelSize = roundSpacing(image->getSpacing());
        QStandardItem* voxelSizeItem = this->getInfo< ::fwData::Image::SpacingType>(voxelSize, " x ");
        studyRootItem->setChild(nbRow, 4, voxelSizeItem);

        ::fwData::Image::OriginType patientPosition = image->getOrigin();
        QStandardItem* originItem = this->getInfo< ::fwData::Image::OriginType>(patientPosition, ", ");
        studyRootItem->setChild(nbRow, 5, originItem);
    }

    if(m_insert)
    {
        ::uiMedDataQt::InsertSeries::sptr insertSeries = ::uiMedDataQt::InsertSeries::dynamicCast(series);

        const int nbColumns = studyRootItem->columnCount();
        for(int i = 0; i < nbColumns; ++i)
        {
            QStandardItem* item = studyRootItem->child(nbRow, i);
            if(!item)
            {
                studyRootItem->setChild(nbRow, i, new QStandardItem());
                item = studyRootItem->child(nbRow, i);
            }

            if(insertSeries)
            {
                QFont f = item->font();
                f.setBold(true);
                item->setFont(f);
            }
            else
            {
                item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
            }
        }
    }

    this->addSeriesIcon(series, seriesDescription1);
}

//-----------------------------------------------------------------------------

void SelectorModel::addSeriesIcon(::fwMedData::Series::sptr series, QStandardItem* item)
{
    SeriesIconType::iterator iter = m_seriesIcons.find(series->getClassname());
    if (iter != m_seriesIcons.end())
    {
        item->setIcon(QIcon(QString::fromStdString(iter->second)));
    }
    else
    {
        ::fwMedData::ImageSeries::sptr imageSeries       = ::fwMedData::ImageSeries::dynamicCast(series);
        ::fwMedData::ModelSeries::sptr modelSeries       = ::fwMedData::ModelSeries::dynamicCast(series);
        ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::dynamicCast(series);
        if(imageSeries)
        {
            const auto path = ::fwRuntime::getBundleResourceFilePath("media", "icons/ImageSeries.svg");
            item->setIcon(QIcon(QString::fromStdString(path.string())));
        }
        else if (modelSeries)
        {
            const auto path = ::fwRuntime::getBundleResourceFilePath("media", "icons/ModelSeries.svg");
            item->setIcon(QIcon(QString::fromStdString(path.string())));
        }
        else if (activitySeries)
        {
            ::fwActivities::registry::Activities::sptr registry = ::fwActivities::registry::Activities::getDefault();
            std::string id = activitySeries->getActivityConfigId();
            OSLM_ASSERT("Activity information not found for" << id, registry->hasInfo(id));

            ::fwActivities::registry::ActivityInfo activityInfo;
            activityInfo = registry->getInfo(id);
            item->setIcon(QIcon(QString::fromStdString(activityInfo.icon)));
        }
        else
        {
            OSLM_WARN("This type of series is not defined (" << series->getClassname() << ")");
        }
    }
}

//-----------------------------------------------------------------------------

void SelectorModel::removeSeries(::fwMedData::Series::sptr series)
{
    QStandardItem* seriesItem = this->findSeriesItem(series);
    this->removeSeriesItem(seriesItem);
}

//-----------------------------------------------------------------------------

QModelIndex SelectorModel::getIndex(const QModelIndex& index, int column )
{
    QModelIndex idx = this->createIndex(index.row(), column, index.internalPointer());
    return idx;
}

//-----------------------------------------------------------------------------

void SelectorModel::removeRows(const QModelIndexList indexes)
{
    QList<QStandardItem*> seriesItems;
    QList<QStandardItem*> studyItems;

    for(QModelIndex index :  indexes)
    {
        SLM_ASSERT("Index must be in first column.", index.column() == 0);
        QStandardItem* item = this->itemFromIndex(index);
        if (item->data(SelectorModel::ITEM_TYPE) == SelectorModel::STUDY)
        {
            studyItems.append(item);
        }
        else if (item->data(SelectorModel::ITEM_TYPE) == SelectorModel::SERIES)
        {
            seriesItems.append(item);
        }
    }

    // Remove series items from selector
    for(QStandardItem* item :  seriesItems)
    {
        QStandardItem* studyItem = item->parent();

        // Remove series item if it is not included in a study which will be remove.
        if (std::find(studyItems.begin(), studyItems.end(), studyItem) == studyItems.end())
        {
            this->removeSeriesItem(item);
        }
    }

    // Remove study items from selector
    for(QStandardItem* item :  studyItems)
    {
        this->removeStudyItem(item);
    }
}

//-----------------------------------------------------------------------------

bool SelectorModel::removeStudyItem(QStandardItem* item)
{
    bool isRemoved = false;
    SLM_ASSERT("Index must represent a study.", item->data(SelectorModel::ITEM_TYPE) == SelectorModel::STUDY);
    QString uid = item->data(SelectorModel::UID).toString();
    ::fwMedData::DicomValueType instanceUID = uid.toStdString();

    isRemoved = this->QStandardItemModel::removeRow(item->row());
    SLM_ASSERT("Remove can not be done!", isRemoved);
    m_items.erase(instanceUID);
    --m_studyRowCount;

    return isRemoved;
}

//-----------------------------------------------------------------------------

bool SelectorModel::removeSeriesItem(QStandardItem* item)
{
    bool isRemoved = false;

    SLM_ASSERT("Index must represent series", item->data(SelectorModel::ITEM_TYPE) == SelectorModel::SERIES);
    QStandardItem* parent = item->parent();
    isRemoved = this->QStandardItemModel::removeRow(item->row(), this->indexFromItem(parent));
    SLM_ASSERT("Remove can not be done!", isRemoved);
    if(parent && parent->rowCount() == 0)
    {
        this->removeStudyItem(parent);
    }
    return isRemoved;
}

//-----------------------------------------------------------------------------

QStandardItem* SelectorModel::findSeriesItem(::fwMedData::Series::sptr series)
{
    QStandardItem* seriesItem;
    ::fwMedData::Study::sptr study = series->getStudy();
    QStandardItem* studyItem = this->findStudyItem(study);

    int nbRow = studyItem->rowCount();
    for(int row = 0; row < nbRow; ++row)
    {
        QStandardItem* child = studyItem->child(row);
        std::string seriesId = child->data(SelectorModel::UID).toString().toStdString();
        if(seriesId == series->getID())
        {
            seriesItem = child;
            break;
        }
    }
    return seriesItem;
}

//-----------------------------------------------------------------------------

QStandardItem* SelectorModel::findStudyItem(::fwMedData::Study::sptr study)
{
    ::fwMedData::DicomValueType studyInstanceUid = study->getInstanceUID();

    QStandardItem* studyItem = m_items[studyInstanceUid];
    return studyItem;
}

//-----------------------------------------------------------------------------

void SelectorModel::setSeriesIcons(const SeriesIconType& seriesIcons)
{
    m_seriesIcons = seriesIcons;
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedDataQt
