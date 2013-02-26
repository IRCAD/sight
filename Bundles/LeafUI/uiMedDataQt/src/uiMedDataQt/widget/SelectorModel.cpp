/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QStandardItem>
#include <QString>

#include <fwTools/fwID.hpp>

#include <fwData/Image.hpp>

#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwActivities/registry/Activities.hpp>

#include "uiMedDataQt/widget/SelectorModel.hpp"

namespace uiMedData
{
namespace widget
{

//-----------------------------------------------------------------------------

SelectorModel::SelectorModel(QWidget *parent) :
    QStandardItemModel(parent),
    m_studyRowCount(0)
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
    headers << "Patient name" << "Patient ID" << "Modality" << "Date started" << "Institution" << "Birthdate"
            << "Sex" << "Age" << "Referring physician" << "Performing physician " << "Study description"
            << "Image number" << "Voxel size" << "Patient position";

    this->setHorizontalHeaderLabels(headers);
}

//-----------------------------------------------------------------------------

SelectorModel::ItemType SelectorModel::getItemType(const QModelIndex &index)
{
    QModelIndex idx = this->createIndex(index.row(), 0, index.internalPointer());
    QStandardItem *item = this->itemFromIndex(idx);
    return (SelectorModel::ItemType) item->data(SelectorModel::ITEM_TYPE).toInt();
}

//-----------------------------------------------------------------------------

void SelectorModel::clear()
{
    this->QStandardItemModel::clear();
    this->init();
}

//-----------------------------------------------------------------------------

void SelectorModel::addSeries(::fwMedData::Series::sptr series)
{
    ::fwMedData::Study::sptr study = series->getStudy();
    ::fwMedData::DicomValueType studyUID = study->getInstanceUID();
    StudyUidItemMapType::iterator itr = m_items.find(studyUID);
    QStandardItem* studyRootItem;
    if(itr != m_items.end())
    {
        studyRootItem = itr->second;
    }
    else
    {
        ::fwMedData::Patient::sptr patient = series->getPatient();
        ::fwMedData::Equipment::sptr equipment = series->getEquipment();

        QStandardItem *patientName = new QStandardItem( QString::fromStdString(patient->getName()) );
        patientName->setData(QVariant((int)SelectorModel::STUDY), SelectorModel::ITEM_TYPE);
        patientName->setData(QVariant(QString::fromStdString(study->getInstanceUID())), UID);
        QStandardItem *patientId   = new QStandardItem( QString::fromStdString(patient->getPatientId()) );
        QStandardItem *patientBirthdate   = new QStandardItem( QString::fromStdString(patient->getBirthdate()) );
        QStandardItem *patientSex   = new QStandardItem( QString::fromStdString(patient->getSex()) );

        std::string studyDateTime(study->getDate()+ " " + study->getTime());
        QStandardItem *studyDate = new QStandardItem( QString::fromStdString(studyDateTime));
        QStandardItem *studyReferringPhysicianName = new QStandardItem(
                QString::fromStdString(study->getReferringPhysicianName()));
        QStandardItem *studyDescription = new QStandardItem( QString::fromStdString(study->getDescription()));
        QStandardItem *studyPatientAge = new QStandardItem( QString::fromStdString(study->getPatientAge()));

        QStandardItem *institution = new QStandardItem( QString::fromStdString(equipment->getInstitutionName()));

        this->setItem(m_studyRowCount, 0, patientName);
        this->setItem(m_studyRowCount, 1, patientId);
        this->setItem(m_studyRowCount, 3, studyDate);
        this->setItem(m_studyRowCount, 4, institution);
        this->setItem(m_studyRowCount, 5, patientBirthdate);
        this->setItem(m_studyRowCount, 6, patientSex);
        this->setItem(m_studyRowCount, 7, studyPatientAge);
        this->setItem(m_studyRowCount, 8, studyReferringPhysicianName);
        this->setItem(m_studyRowCount, 10, studyDescription);

        m_studyRowCount++;
        studyRootItem = patientName;
        m_items[studyUID] = studyRootItem;
    }


    QStandardItem *seriesModality = new QStandardItem(QString::fromStdString(series->getModality()));
    std::string seriesDateTime(series->getDate()+ " " + series->getTime());
    QStandardItem *seriesDate = new QStandardItem( QString::fromStdString(seriesDateTime));


    QStandardItem* seriesPerformingPhysician =
            this->getInfo< ::fwMedData::DicomValuesType >(series->getPerformingPhysiciansName(), ", ");
    QStandardItem * seriesDescription1 = new QStandardItem(QString::fromStdString(series->getDescription()));
    seriesDescription1->setData(QVariant((int)SelectorModel::SERIES), SelectorModel::ITEM_TYPE);
    seriesDescription1->setData(QVariant(QString::fromStdString(series->getID())), UID);
    QStandardItem * seriesDescription2 = new QStandardItem(QString::fromStdString(series->getDescription()));

    int nbRow = studyRootItem->rowCount();
    studyRootItem->setChild(nbRow, 0, seriesDescription1);
    studyRootItem->setChild(nbRow, 2, seriesModality);
    studyRootItem->setChild(nbRow, 3, seriesDate);
    studyRootItem->setChild(nbRow, 9, seriesPerformingPhysician);
    studyRootItem->setChild(nbRow, 10, seriesDescription2);
    studyRootItem->setChild(nbRow, 13, new QStandardItem());

    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    if(imageSeries)
    {

        ::fwData::Image::sptr image =  imageSeries->getImage();

        ::fwData::Image::SizeType imageNumber = image->getSize();
        QStandardItem* imageSize = this->getInfo< ::fwData::Image::SizeType>(imageNumber, " x ");
        studyRootItem->setChild(nbRow, 11, imageSize);

        ::fwData::Image::SpacingType voxelSize = image->getSpacing();
        QStandardItem* voxelSizeItem = this->getInfo< ::fwData::Image::SpacingType>(voxelSize, " x ");
        studyRootItem->setChild(nbRow, 12, voxelSizeItem);

        ::fwData::Image::OriginType patientPosition = image->getOrigin();
        QStandardItem* originItem = this->getInfo< ::fwData::Image::OriginType>(patientPosition, ", ");
        studyRootItem->setChild(nbRow, 13, originItem);
    }

    this->addSeriesIcon(series, seriesDescription1);
}

//-----------------------------------------------------------------------------

void SelectorModel::addSeriesIcon(::fwMedData::Series::sptr series, QStandardItem *item)
{
    ::fwMedData::ImageSeries::sptr imageSeries = ::fwMedData::ImageSeries::dynamicCast(series);
    ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);
    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::dynamicCast(series);
    if(imageSeries)
    {
        item->setIcon(QIcon("Bundles/media_0-1/icons/ImageSeries.svg"));
    }
    else if (modelSeries)
    {
        item->setIcon(QIcon("Bundles/media_0-1/icons/ModelSeries.svg"));
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
    QList<QStandardItem *> seriesItems;
    QList<QStandardItem *> studyItems;

    BOOST_FOREACH(QModelIndex index, indexes)
    {
        SLM_ASSERT("Index must be in first column.", index.column() == 0);
        QStandardItem * item = this->itemFromIndex(index);
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
    BOOST_FOREACH(QStandardItem *item, seriesItems)
    {
        QStandardItem * studyItem = item->parent();

        // Remove series item if it is not included in a study which will be remove.
        if (std::find(studyItems.begin(), studyItems.end(), studyItem) == studyItems.end())
        {
            this->removeSeriesItem(item);
        }
    }

    // Remove study items from selector
    BOOST_FOREACH(QStandardItem *item, studyItems)
    {
        this->removeStudyItem(item);
    }
}

//-----------------------------------------------------------------------------

bool SelectorModel::removeStudyItem(QStandardItem *item)
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

bool SelectorModel::removeSeriesItem(QStandardItem *item)
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
    ::fwMedData::DicomValueType studyInstanceUid = study->getInstanceUID();

    QStandardItem* studyItem = m_items[studyInstanceUid];
    int nbRow = studyItem->rowCount();
    for(int row =0; row < nbRow; ++row)
    {
        QStandardItem *child = studyItem->child(row);
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

} // namespace widget
} // namespace uiMedData
