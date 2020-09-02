/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include <QFont>
#include <QStandardItem>
#include <QString>

#include <regex>

namespace uiMedDataQt
{
namespace widget
{

//-----------------------------------------------------------------------------

SelectorModel::SelectorModel(QWidget* _parent) :
    QStandardItemModel(_parent)
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
    headers << "Name" << "Sex" << "Birthdate"
            << "Modality"
            << "Description"
            << "Date" << "Time"
            << "Patient age"
            << "Body part examined" << "Patient position" << "Contrast agent"
            << "Acquisition time" << "Contrast/bolus time";
    this->setHorizontalHeaderLabels(headers);
}

//-----------------------------------------------------------------------------

void SelectorModel::setInsertMode(bool _insert)
{
    m_insert = _insert;
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

::fwData::Image::Spacing roundSpacing(const ::fwData::Image::Spacing& _spacing)
{
    ::fwData::Image::Spacing roundSpacing;
    for(size_t i = 0; i < 3; ++i)
    {
        ::fwData::Image::Spacing::value_type roundVal = ::boost::math::round(_spacing[i] * 100.)/100.;
        roundSpacing[i]                               = roundVal;
    }
    return roundSpacing;
}

//-----------------------------------------------------------------------------

std::string formatTime(const std::string& _time)
{
    std::string formatTime = _time;
    ::boost::algorithm::trim(formatTime);

    const std::string regexHour = "[0-9]{2}";
    const std::string regexMin  = "[0-9]{2}";
    const std::string regexSec  = "[0-9]{2}";
    const std::string regexEnd  = "[.0-9]*";

    const std::string regexStr = "("+regexHour+")"+"("+regexMin+")"+"("+regexSec+")"+regexEnd;
    std::regex re(regexStr);
    std::smatch match;
    if( std::regex_match(formatTime, match, re) )
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

void SelectorModel::addSeries(::fwMedData::Series::sptr _series)
{
    ::fwMedData::Study::sptr study       = _series->getStudy();
    ::fwMedData::DicomValueType studyUID = study->getInstanceUID();
    StudyUidItemMapType::iterator itr = m_items.find(studyUID);
    QStandardItem* studyRootItem;

    if(itr != m_items.end())
    {
        studyRootItem = itr->second;
    }
    else
    {
        ::fwMedData::Patient::sptr patient     = _series->getPatient();
        ::fwMedData::Equipment::sptr equipment = _series->getEquipment();

        QStandardItem* patientName = new QStandardItem( QString::fromStdString(patient->getName()) );
        patientName->setData(QVariant((int)ItemType::STUDY), Role::ITEM_TYPE);
        patientName->setData(QVariant(QString::fromStdString(study->getInstanceUID())), Role::UID);

        QStandardItem* patientSex = new QStandardItem( QString::fromStdString(patient->getSex()) );

        std::string birthDate = patient->getBirthdate();
        if(!birthDate.empty() && birthDate != "unknown")
        {
            birthDate.insert(4, "-");
            birthDate.insert(7, "-");
        }
        QStandardItem* patientBirthdate = new QStandardItem( QString::fromStdString(birthDate) );

        QStandardItem* studyDescription = new QStandardItem( QString::fromStdString(study->getDescription()));

        std::string studyDate = study->getDate();
        if(!studyDate.empty())
        {
            studyDate.insert(4, "/");
            studyDate.insert(7, "/");
        }
        QStandardItem* studyDateItem = new QStandardItem( QString::fromStdString(studyDate));

        std::string studyTime = study->getTime();
        studyTime = studyTime.substr(0, 6);
        if(!studyTime.empty())
        {
            studyTime.insert(2, ":");
            studyTime.insert(5, ":");
        }
        QStandardItem* studyTimeItem = new QStandardItem( QString::fromStdString(studyTime));

        QStandardItem* studyPatientAge = new QStandardItem( QString::fromStdString(study->getPatientAge()));

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

        m_studyRowCount++;
        studyRootItem     = patientName;
        m_items[studyUID] = studyRootItem;
    }

    QStandardItem* seriesIcon        = new QStandardItem();
    QStandardItem* seriesModality    = new QStandardItem(QString::fromStdString(_series->getModality()));
    QStandardItem* seriesDescription = new QStandardItem(QString::fromStdString(_series->getDescription()));
    seriesDescription->setData(QVariant((int)ItemType::SERIES), Role::ITEM_TYPE);
    seriesDescription->setData(QVariant(QString::fromStdString(_series->getID())), Role::UID);

    std::string seriesDate = _series->getDate();
    if(!seriesDate.empty())
    {
        seriesDate.insert(4, "/");
        seriesDate.insert(7, "/");
    }
    QStandardItem* seriesDateItem = new QStandardItem( QString::fromStdString(seriesDate));

    std::string seriesTime = _series->getTime();
    seriesTime = seriesTime.substr(0, 6);
    if(!seriesTime.empty())
    {
        seriesTime.insert(2, ":");
        seriesTime.insert(5, ":");
    }
    QStandardItem* seriesTimeItem = new QStandardItem( QString::fromStdString(seriesTime));

    const int nbRow = studyRootItem->rowCount();
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::BIRTHDATE), seriesIcon);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::MODALITY), seriesModality);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::DESCRIPTION), seriesDescription);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::DATE), seriesDateItem);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::TIME), seriesTimeItem);

    const ::fwMedData::ImageSeries::csptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(_series);
    if(imageSeries)
    {
        studyRootItem->setChild(nbRow, int(ColumnSeriesType::BODY_PART_EXAMINED),
                                new QStandardItem(QString::fromStdString(imageSeries->getBodyPartExamined())));

        std::string patientPosition = imageSeries->getPatientPosition();
        if(!patientPosition.empty())
        {
            // Code string can contains leading or trailing spaces, we removed it first.
            const std::string::const_iterator forward
                = std::remove_if( patientPosition.begin(), patientPosition.end(), [&](unsigned char _c)
                    {
                        return _c == ' ';
                    });
            patientPosition.erase(forward);
            if(patientPosition.compare("HFP") == 0)
            {
                patientPosition = "Head First-Prone";
            }
            else if(patientPosition.compare("HFS") == 0)
            {
                patientPosition = "Head First-Supine";
            }
            else if(patientPosition.compare("HFDR") == 0)
            {
                patientPosition = "Head First-Decubitus Right";
            }
            else if(patientPosition.compare("HFDL") == 0)
            {
                patientPosition = "Head First-Decubitus Left";
            }
            else if(patientPosition.compare("FFDR") == 0)
            {
                patientPosition = "Feet First-Decubitus Right";
            }
            else if(patientPosition.compare("FFDL") == 0)
            {
                patientPosition = "Feet First-Decubitus Left";
            }
            else if(patientPosition.compare("FFP") == 0)
            {
                patientPosition = "Feet First-Prone";
            }
            else if(patientPosition.compare("FFS") == 0)
            {
                patientPosition = "Feet First-Supine";
            }
            else if(patientPosition.compare("LFP") == 0)
            {
                patientPosition = "Left First-Prone";
            }
            else if(patientPosition.compare("LFS") == 0)
            {
                patientPosition = "Left First-Supine";
            }
            else if(patientPosition.compare("RFP") == 0)
            {
                patientPosition = "Right First-Prone";
            }
            else if(patientPosition.compare("RFS") == 0)
            {
                patientPosition = "Right First-Supine";
            }
            else if(patientPosition.compare("AFDR") == 0)
            {
                patientPosition = "Anterior First-Decubitus Right";
            }
            else if(patientPosition.compare("AFDL") == 0)
            {
                patientPosition = "Anterior First-Decubitus Left";
            }
            else if(patientPosition.compare("PFDR") == 0)
            {
                patientPosition = "Posterior First-Decubitus Right";
            }
            else if(patientPosition.compare("PFDL") == 0)
            {
                patientPosition = "Posterior First-Decubitus Left";
            }
        }
        studyRootItem->setChild(nbRow, int(ColumnSeriesType::PATIENT_POSITION),
                                new QStandardItem(QString::fromStdString(patientPosition)));
        studyRootItem->setChild(nbRow, int(ColumnSeriesType::CONTRAST_AGENT),
                                new QStandardItem(QString::fromStdString(imageSeries->getContrastAgent())));

        std::string acquisitionTime = imageSeries->getAcquisitionTime();
        acquisitionTime = acquisitionTime.substr(0, 6);
        if(!acquisitionTime.empty())
        {
            acquisitionTime.insert(2, ":");
            acquisitionTime.insert(5, ":");
        }
        studyRootItem->setChild(nbRow, int(ColumnSeriesType::ACQUISITION_TIME),
                                new QStandardItem(QString::fromStdString(acquisitionTime)));

        std::string contrastTime = imageSeries->getContrastStartTime();
        contrastTime = contrastTime.substr(0, 6);
        if(!contrastTime.empty())
        {
            contrastTime.insert(2, ":");
            contrastTime.insert(5, ":");
        }
        studyRootItem->setChild(nbRow, int(ColumnSeriesType::CONTRAST_BOLUS_START_TIME),
                                new QStandardItem(QString::fromStdString(contrastTime)));
    }

    if(m_insert)
    {
        ::uiMedDataQt::InsertSeries::sptr insertSeries = ::uiMedDataQt::InsertSeries::dynamicCast(_series);

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

    this->addSeriesIcon(_series, seriesIcon);
}

//-----------------------------------------------------------------------------

void SelectorModel::addSeriesIcon(::fwMedData::Series::sptr _series, QStandardItem* _item)
{
    SeriesIconType::iterator iter = m_seriesIcons.find(_series->getClassname());
    if (iter != m_seriesIcons.end())
    {
        _item->setIcon(QIcon(QString::fromStdString(iter->second)));
    }
    else
    {
        ::fwMedData::ImageSeries::sptr imageSeries       = ::fwMedData::ImageSeries::dynamicCast(_series);
        ::fwMedData::ModelSeries::sptr modelSeries       = ::fwMedData::ModelSeries::dynamicCast(_series);
        ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::dynamicCast(_series);
        if(imageSeries)
        {
            const auto path = ::fwRuntime::getModuleResourceFilePath("media", "icons/ImageSeries.svg");
            _item->setIcon(QIcon(QString::fromStdString(path.string())));
        }
        else if (modelSeries)
        {
            const auto path = ::fwRuntime::getModuleResourceFilePath("media", "icons/ModelSeries.svg");
            _item->setIcon(QIcon(QString::fromStdString(path.string())));
        }
        else if (activitySeries)
        {
            ::fwActivities::registry::Activities::sptr registry = ::fwActivities::registry::Activities::getDefault();
            std::string id = activitySeries->getActivityConfigId();
            OSLM_ASSERT("Activity information not found for" << id, registry->hasInfo(id));

            ::fwActivities::registry::ActivityInfo activityInfo;
            activityInfo = registry->getInfo(id);
            _item->setIcon(QIcon(QString::fromStdString(activityInfo.icon)));
        }
        else
        {
            OSLM_WARN("This type of series is not defined (" << _series->getClassname() << ")");
        }
    }
}

//-----------------------------------------------------------------------------

void SelectorModel::removeSeries(::fwMedData::Series::sptr _series)
{
    QStandardItem* seriesItem = this->findSeriesItem(_series);
    this->removeSeriesItem(seriesItem);
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
        SLM_ASSERT("Index must be in first column.", index.column() == 0);
        QStandardItem* item = this->itemFromIndex(index);
        if (item->data(Role::ITEM_TYPE) == ItemType::STUDY)
        {
            studyItems.append(item);
        }
        else if (item->data(Role::ITEM_TYPE) == ItemType::SERIES)
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

bool SelectorModel::removeStudyItem(QStandardItem* _item)
{
    bool isRemoved = false;
    SLM_ASSERT("Index must represent a study.", _item->data(Role::ITEM_TYPE) == ItemType::STUDY);
    QString uid = _item->data(Role::UID).toString();
    ::fwMedData::DicomValueType instanceUID = uid.toStdString();

    isRemoved = this->QStandardItemModel::removeRow(_item->row());
    SLM_ASSERT("Remove can not be done!", isRemoved);
    m_items.erase(instanceUID);
    --m_studyRowCount;

    return isRemoved;
}

//-----------------------------------------------------------------------------

bool SelectorModel::removeSeriesItem(QStandardItem* _item)
{
    bool isRemoved = false;

    SLM_ASSERT("Index must represent series", _item->data(Role::ITEM_TYPE) == ItemType::SERIES);
    QStandardItem* parent = _item->parent();
    isRemoved = this->QStandardItemModel::removeRow(_item->row(), this->indexFromItem(parent));
    SLM_ASSERT("Remove can not be done!", isRemoved);
    if(parent && parent->rowCount() == 0)
    {
        this->removeStudyItem(parent);
    }
    return isRemoved;
}

//-----------------------------------------------------------------------------

QStandardItem* SelectorModel::findSeriesItem(::fwMedData::Series::sptr _series)
{
    QStandardItem* seriesItem = nullptr;
    ::fwMedData::Study::sptr study = _series->getStudy();
    QStandardItem* studyItem = this->findStudyItem(study);

    int nbRow = studyItem->rowCount();
    for(int row = 0; row < nbRow; ++row)
    {
        QStandardItem* child = studyItem->child(row, int(ColumnSeriesType::DESCRIPTION));
        std::string seriesId = child->data(Role::UID).toString().toStdString();
        if(seriesId == _series->getID())
        {
            seriesItem = child;
            break;
        }
    }
    return seriesItem;
}

//-----------------------------------------------------------------------------

QStandardItem* SelectorModel::findStudyItem(::fwMedData::Study::sptr _study)
{
    ::fwMedData::DicomValueType studyInstanceUid = _study->getInstanceUID();

    QStandardItem* studyItem = m_items[studyInstanceUid];
    return studyItem;
}

//-----------------------------------------------------------------------------

void SelectorModel::setSeriesIcons(const SeriesIconType& _seriesIcons)
{
    m_seriesIcons = _seriesIcons;
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedDataQt
