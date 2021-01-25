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

#include "uiMedDataQt/widget/SelectorModel.hpp"

#include "uiMedDataQt/InsertSeries.hpp"

#include <activities/registry/Activities.hpp>

#include <core/runtime/operations.hpp>
#include <core/tools/fwID.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Equipment.hpp>
#include <data/Image.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>
#include <data/Patient.hpp>
#include <data/Series.hpp>

#include <boost/algorithm/string/trim.hpp>
#include <boost/math/special_functions/round.hpp>

#include <QFont>
#include <QPushButton>
#include <QStandardItem>
#include <QString>
#include <QTreeView>

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

    if(m_allowedRemove)
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
    for(size_t i = 0; i < 3; ++i)
    {
        data::Image::Spacing::value_type roundVal = ::boost::math::round(_spacing[i] * 100.)/100.;
        roundSpacing[i] = roundVal;
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
        SLM_ASSERT("Wrong match for "<<formatTime, match.size() >= 4);
        hour.assign(match[1].first, match[1].second);
        min.assign(match[2].first, match[2].second);
        sec.assign(match[3].first, match[3].second);

        formatTime = hour + ":" + min + ":" + sec;
    }

    return formatTime;
}

//-----------------------------------------------------------------------------

void SelectorModel::addSeries(data::Series::sptr _series)
{
    data::Study::sptr study           = _series->getStudy();
    data::DicomValueType studyUID     = study->getInstanceUID();
    StudyUidItemMapType::iterator itr = m_items.find(studyUID);
    QStandardItem* studyRootItem;

    if(itr != m_items.end())
    {
        studyRootItem = itr->second;
    }
    else
    {
        data::Patient::sptr patient     = _series->getPatient();
        data::Equipment::sptr equipment = _series->getEquipment();

        const std::string studyInstanceUID = study->getInstanceUID();

        QStandardItem* patientName = new QStandardItem( QString::fromStdString(patient->getName()) );
        patientName->setData(QVariant((int)ItemType::STUDY), Role::ITEM_TYPE);
        patientName->setData(QVariant(QString::fromStdString(studyInstanceUID)), Role::UID);

        QStandardItem* patientSex = new QStandardItem( QString::fromStdString(patient->getSex()) );

        std::string birthDate = patient->getBirthdate();
        if(!birthDate.empty() && birthDate != "unknown")
        {
            birthDate = birthDate.substr(4, 2) + "/" + birthDate.substr(6, 2) + "/" + birthDate.substr(0, 4);
        }
        QStandardItem* patientBirthdate = new QStandardItem( QString::fromStdString(birthDate) );

        QStandardItem* studyDescription = new QStandardItem( QString::fromStdString(study->getDescription()));

        std::string studyDate = study->getDate();
        if(!studyDate.empty())
        {
            studyDate = studyDate.substr(4, 2) + "/" + studyDate.substr(6, 2) + "/" + studyDate.substr(0, 4);
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

        // Add a remove button to each studies.
        if(m_allowedRemove && !m_removeStudyIcon.empty())
        {
            this->setItem(m_studyRowCount, int(ColumnSeriesType::REMOVE), new QStandardItem(QString("")));

            QTreeView* const selector = static_cast< QTreeView* >(this->parent());
            SLM_ASSERT("The QTreeView parent must be given to the constructor", selector);

            QPushButton* const removeButton = new QPushButton(QIcon(m_removeStudyIcon.string().c_str()), "");
            selector->setIndexWidget(this->index(m_studyRowCount, int(ColumnSeriesType::REMOVE)), removeButton);

            // When the remove button is clicked, emit a signal with the study UID.
            QObject::connect(removeButton, &QPushButton::clicked, this, [ = ]()
                    {
                        Q_EMIT removeStudyInstanceUID(studyInstanceUID);
                    });
        }

        m_studyRowCount++;
        studyRootItem     = patientName;
        m_items[studyUID] = studyRootItem;
    }

    const std::string serieID = _series->getID();

    QStandardItem* seriesRole = new QStandardItem();
    seriesRole->setData(QVariant((int)ItemType::SERIES), Role::ITEM_TYPE);
    seriesRole->setData(QVariant(QString::fromStdString(serieID)), Role::UID);

    QStandardItem* seriesIcon        = new QStandardItem();
    QStandardItem* seriesModality    = new QStandardItem(QString::fromStdString(_series->getModality()));
    QStandardItem* seriesDescription = new QStandardItem(QString::fromStdString(_series->getDescription()));

    std::string seriesDate = _series->getDate();
    if(!seriesDate.empty())
    {
        seriesDate = seriesDate.substr(4, 2) + "/" + seriesDate.substr(6, 2) + "/" + seriesDate.substr(0, 4);
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
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::NAME), seriesRole);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::BIRTHDATE), seriesIcon);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::MODALITY), seriesModality);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::DESCRIPTION), seriesDescription);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::DATE), seriesDateItem);
    studyRootItem->setChild(nbRow, int(ColumnSeriesType::TIME), seriesTimeItem);

    // Add a remove button to each series.
    if(m_allowedRemove && !m_removeSerieIcon.empty())
    {
        QStandardItem* const removeItem = new QStandardItem(QString(""));
        studyRootItem->setChild(nbRow, int(ColumnSeriesType::REMOVE), removeItem);

        QTreeView* const selector = static_cast< QTreeView* >(this->parent());
        SLM_ASSERT("The QTreeView parent must be given to the constructor", selector);

        QPushButton* const removeButton = new QPushButton(QIcon(m_removeSerieIcon.string().c_str()), "");
        selector->setIndexWidget(this->indexFromItem(removeItem), removeButton);

        // When the remove button is clicked, emit a signal with the study UID.
        QObject::connect(removeButton, &QPushButton::clicked, this, [ = ]()
                {
                    Q_EMIT removeSerieID(serieID);
                });
    }

    const data::ImageSeries::csptr imageSeries = data::ImageSeries::dynamicCast(_series);
    if(imageSeries)
    {
        studyRootItem->setChild(nbRow, int(ColumnSeriesType::BODY_PART_EXAMINED),
                                new QStandardItem(QString::fromStdString(imageSeries->getBodyPartExamined())));

        std::string patientPosition = imageSeries->getPatientPosition();
        if(!patientPosition.empty())
        {
            // Code string can contains leading or trailing spaces, we removed it first.
            const std::string::const_iterator forward
                = std::remove_if(patientPosition.begin(), patientPosition.end(), [&](unsigned char _c)
                    {
                        return _c == ' ';
                    });
            patientPosition.erase(forward, patientPosition.end());
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

void SelectorModel::addSeriesIcon(data::Series::sptr _series, QStandardItem* _item)
{
    SeriesIconType::iterator iter = m_seriesIcons.find(_series->getClassname());
    if (iter != m_seriesIcons.end())
    {
        _item->setIcon(QIcon(QString::fromStdString(iter->second)));
    }
    else
    {
        data::ImageSeries::sptr imageSeries       = data::ImageSeries::dynamicCast(_series);
        data::ModelSeries::sptr modelSeries       = data::ModelSeries::dynamicCast(_series);
        data::ActivitySeries::sptr activitySeries = data::ActivitySeries::dynamicCast(_series);
        if(imageSeries)
        {
            const auto path = core::runtime::getModuleResourceFilePath("media", "icons/ImageSeries.svg");
            _item->setIcon(QIcon(QString::fromStdString(path.string())));
        }
        else if (modelSeries)
        {
            const auto path = core::runtime::getModuleResourceFilePath("media", "icons/ModelSeries.svg");
            _item->setIcon(QIcon(QString::fromStdString(path.string())));
        }
        else if (activitySeries)
        {
            activities::registry::Activities::sptr registry = activities::registry::Activities::getDefault();
            std::string id                                  = activitySeries->getActivityConfigId();
            SLM_ASSERT("Activity information not found for" << id, registry->hasInfo(id));

            activities::registry::ActivityInfo activityInfo;
            activityInfo = registry->getInfo(id);
            _item->setIcon(QIcon(QString::fromStdString(activityInfo.icon)));
        }
        else
        {
            SLM_WARN("This type of series is not defined (" << _series->getClassname() << ")");
        }
    }
}

//-----------------------------------------------------------------------------

void SelectorModel::removeSeries(data::Series::sptr _series)
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
        SLM_ASSERT("Index must be in the name column.", index.column() == int(ColumnSeriesType::NAME));
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
    QString uid                      = _item->data(Role::UID).toString();
    data::DicomValueType instanceUID = uid.toStdString();

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

QStandardItem* SelectorModel::findSeriesItem(data::Series::sptr _series)
{
    QStandardItem* seriesItem = nullptr;
    data::Study::sptr study   = _series->getStudy();
    QStandardItem* studyItem  = this->findStudyItem(study);

    int nbRow = studyItem->rowCount();
    for(int row = 0; row < nbRow; ++row)
    {
        QStandardItem* child = studyItem->child(row, int(ColumnSeriesType::NAME));
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

QStandardItem* SelectorModel::findStudyItem(data::Study::sptr _study)
{
    data::DicomValueType studyInstanceUid = _study->getInstanceUID();

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
