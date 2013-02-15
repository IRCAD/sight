/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QStandardItem>
#include <QString>

#include <fwData/Image.hpp>

#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>

#include "uiMedDataQt/widget/Selector.hpp"

namespace uiMedData
{
namespace widget
{

Selector::Selector(QWidget *parent) :
    QTreeView(parent),
    m_studyRowCount(0)
{
    m_model = new QStandardItemModel();
    this->setModel(m_model);

    QStringList headers;
    headers << "Patient name" << "Patient ID" << "Modality" << "Date Acquired" << "Institution" << "Birthdate"
            << "Sex" << "Age" << "Referring physician" << "Performing physician " << "Study description"
            << "Image number" << "Voxel size" << "Patient position";

    m_model->setHorizontalHeaderLabels(headers);

    this->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->setAlternatingRowColors( true );
}

Selector::~Selector()
{
}

void Selector::addSeries(::fwMedData::Series::sptr series)
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
        QStandardItem *patientId   = new QStandardItem( QString::fromStdString(patient->getId()) );
        QStandardItem *patientBirthdate   = new QStandardItem( QString::fromStdString(patient->getBirthdate()) );
        QStandardItem *patientSex   = new QStandardItem( QString::fromStdString(patient->getSex()) );

        std::string studyDateTime(study->getDate()+ " " + study->getTime());
        QStandardItem *studyDate = new QStandardItem( QString::fromStdString(studyDateTime));
        QStandardItem *studyReferringPhysicianName = new QStandardItem( QString::fromStdString(study->getReferringPhysicianName()));
        QStandardItem *studyDescription = new QStandardItem( QString::fromStdString(study->getDescription()));
        QStandardItem *studyPatientAge = new QStandardItem( QString::fromStdString(study->getPatientAge()));

        QStandardItem *institution = new QStandardItem( QString::fromStdString(equipment->getInstitutionName()));

        m_model->setItem(m_studyRowCount, 0, patientName);
        m_model->setItem(m_studyRowCount, 1, patientId);
        m_model->setItem(m_studyRowCount, 3, studyDate);
        m_model->setItem(m_studyRowCount, 4, institution);
        m_model->setItem(m_studyRowCount, 5, patientBirthdate);
        m_model->setItem(m_studyRowCount, 6, patientSex);
        m_model->setItem(m_studyRowCount, 7, studyPatientAge);
        m_model->setItem(m_studyRowCount, 8, studyReferringPhysicianName);
        m_model->setItem(m_studyRowCount, 10, studyDescription);

        m_studyRowCount++;
        studyRootItem = patientName;
        m_items[studyUID] = studyRootItem;
    }


    QStandardItem *seriesModality = new QStandardItem(QString::fromStdString(series->getModality()));
    std::string seriesDateTime(series->getDate()+ " " + series->getTime());
    QStandardItem *seriesDate = new QStandardItem( QString::fromStdString(seriesDateTime));


    QStandardItem* seriesPerformingPhysician = this->getInfo< ::fwMedData::DicomValuesType >(series->getPerformingPhysiciansName(), ", ");
    QStandardItem * seriesDescription1 = new QStandardItem(QString::fromStdString(series->getDescription()));
    QStandardItem * seriesDescription2 = new QStandardItem(QString::fromStdString(series->getDescription()));

    int nbRow = studyRootItem->rowCount();
    studyRootItem->setChild(nbRow, 0, seriesDescription1);
    studyRootItem->setChild(nbRow, 2, seriesModality);
    studyRootItem->setChild(nbRow, 3, seriesDate);
    studyRootItem->setChild(nbRow, 9, seriesPerformingPhysician);
    studyRootItem->setChild(nbRow, 10, seriesDescription2);

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

    this->resizeColumnToContents(0);
    this->resizeColumnToContents(9);
    this->resizeColumnToContents(10);
    this->resizeColumnToContents(11);

}

} // namespace widget
} // namespace uiMedData
