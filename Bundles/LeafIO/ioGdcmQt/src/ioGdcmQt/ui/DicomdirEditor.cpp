/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/SelectorDialog.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>

#include <fwTools/fwID.hpp>
#include <fwTools/System.hpp>
#include <fwData/location/Folder.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Vector.hpp>
#include <fwData/String.hpp>
#include <fwData/Integer.hpp>
#include <fwData/location/MultiFiles.hpp>


#include "vtkGdcmIO/DicomPatientDBReader.hpp"

// Defined QT_NO_KEYWORDS because of conflict with boost::signals namespace.
#ifndef QT_NO_KEYWORDS
#define QT_NO_KEYWORDS
#define QT_NO_KEYWORDS_FWDEF
#endif

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTreeWidgetItem>

#ifdef QT_NO_KEYWORDS_FWDEF
#undef QT_NO_KEYWORDS
#undef QT_NO_KEYWORDS_FWDEF
#endif

#include <fwServices/Base.hpp>

#include <fwComEd/PatientDBMsg.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include "ioGdcmQt/ui/DicomdirEditor.hpp"
#include "gdcmIO/Dicomdir.hpp"

namespace ioGdcm
{
namespace ui
{

DicomdirEditor::DicomdirEditor(QWidget* parent, ::boost::filesystem::path& dicomdirPath) throw() : QDialog( parent)
  , m_dicomdirPath(dicomdirPath), m_bServiceIsConfigured(false)
{
    this->createContent();
}
//------------------------------------------------------------------------------

DicomdirEditor::~DicomdirEditor()  throw()
{
    SLM_TRACE_FUNC();
    QObject::disconnect(m_open, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::disconnect(m_DicomdirStructure, SIGNAL(clicked()), this, SLOT(onSelection()));
}

//------------------------------------------------------------------------------

void DicomdirEditor::createContent() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if(!m_dicomdirPath.empty())
    {
        // Get the data
        ::gdcmIO::Dicomdir dicomdir(m_dicomdirPath);
        setWindowTitle("DICOM Tree Viewer");
        setMinimumSize(600, 400);

        QVBoxLayout* layout = new QVBoxLayout(this);
        setLayout(layout);

        m_DicomdirStructure = new QTreeWidget(this);
        m_DicomdirStructure->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_DicomdirStructure->setAlternatingRowColors(true);
        m_DicomdirStructure->setColumnCount(1);
        m_DicomdirStructure->setHeaderLabels(QStringList()<< tr("Dicomdir hierarchy"));

        BOOST_FOREACH( ::gdcmIO::Dicomdir::PatientListType::value_type patient, dicomdir.getPatients())
        {
            QTreeWidgetItem *patientItem = new QTreeWidgetItem();
            patientItem->setText(0, QString::fromStdString(patient.name));
            patientItem->setData(0, Qt::UserRole, QString::fromStdString(patient.id));
            patientItem->setSelected(false);
            patientItem->setData(0, Qt::ToolTipRole, QString("PATIENT"));
            m_DicomdirStructure->addTopLevelItem(patientItem);
            BOOST_FOREACH( ::gdcmIO::Dicomdir::StudyListType::value_type study, dicomdir.getStudies(patient.id))
            {
                QTreeWidgetItem *studyItem = new QTreeWidgetItem(patientItem);
                studyItem->setText(0, QString::fromStdString(study.description));
                studyItem->setData(0, Qt::UserRole, QString::fromStdString(study.instanceUID));
                studyItem->setData(0, Qt::ToolTipRole, QString("STUDY"));
                patientItem->addChild(studyItem);
                BOOST_FOREACH( ::gdcmIO::Dicomdir::SerieListType::value_type serie, dicomdir.getSeries(study.instanceUID))
                {
                    const ::gdcmIO::Dicomdir::ImageListType& imageList = dicomdir.getImages(serie.instanceUID);
                    QTreeWidgetItem *serieItem = new QTreeWidgetItem(studyItem);
                    QString info = QString::fromStdString(serie.description) + QString(" - ( %1, %2 images)").arg(QString::fromStdString(serie.modality)).arg(imageList.size());
                    serieItem->setText(0, info);
                    serieItem->setData(0, Qt::UserRole, QString::fromStdString(serie.instanceUID));
                    serieItem->setData(0, Qt::ToolTipRole, QString("SERIE"));
                    studyItem->addChild(serieItem);
                    BOOST_FOREACH( ::gdcmIO::Dicomdir::ImageListType::value_type image, imageList)
                    {
                        QTreeWidgetItem *imageItem = new QTreeWidgetItem(serieItem);
                        imageItem->setText(0, QString::fromStdString(image.fileID));
                        imageItem->setData(0, Qt::UserRole, QString::fromStdString(serie.instanceUID));
                        ::boost::filesystem::path dicomdirParentPath(m_dicomdirPath.parent_path());
                        dicomdirParentPath/="/";
                        imageItem->setData(0, Qt::ToolTipRole, QString::fromStdString(dicomdirParentPath.string() + image.fileID));
                        serieItem->addChild(serieItem);
                    }
                }
            }
        }
        m_DicomdirStructure->expandAll();
        QHBoxLayout* bottomToolsLayout = new QHBoxLayout();
        QPushButton* closeButton = new QPushButton(tr("Close"), this);
        m_open = new QPushButton(tr("Open"), this);
        m_readers = new QComboBox();

        bottomToolsLayout->addItem(new QSpacerItem(10, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
        bottomToolsLayout->addWidget(m_open);
        bottomToolsLayout->addWidget(closeButton);

        layout->addWidget(m_DicomdirStructure);
        layout->addWidget(m_readers);
        layout->addLayout(bottomToolsLayout);
        QObject::connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
        QObject::connect(m_open, SIGNAL(clicked()), this, SLOT(accept()));
        QObject::connect(m_DicomdirStructure, SIGNAL(itemSelectionChanged()), this, SLOT(onSelection() ));
    }
    else
    {
        std::stringstream ss;
        ss << "Empty DICOMDIR path.";
        ::fwGui::dialog::MessageDialog::showMessageDialog("Warning",
                ss.str(),
                ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//------------------------------------------------------------------------------

void DicomdirEditor::onSelection()
{
    SLM_TRACE_FUNC();
    QTreeWidgetItem *item = m_DicomdirStructure->currentItem();
}

//------------------------------------------------------------------------------

void DicomdirEditor::setReader(const std::map < ::ioGdcm::DicomReader, std::string >& readerList)
{
    SLM_TRACE_FUNC();

    BOOST_FOREACH( readerListType  reader, readerList )
    {
        m_readers->addItem(QString::fromStdString(reader.second), QVariant(reader.first));
    }
}

//------------------------------------------------------------------------------
std::vector< ::boost::filesystem::path > DicomdirEditor::getDicomFiles(QTreeWidgetItem *item)
{

    std::vector< ::boost::filesystem::path > filenames;
    QTreeWidgetItemIterator it(item);
    std::string selectedRole = item->data(0, Qt::ToolTipRole).toString().toStdString();
    std::string currentRole;
    if(item->childCount()!=0)
    {
        // A patient, a study or a serie item hhas been chosen.
        ++it;
        while (*it) {
            currentRole = (*it)->data(0, Qt::ToolTipRole).toString().toStdString();
            if(currentRole != selectedRole)
            {
                if(currentRole !=  "PATIENT" && currentRole !=  "STUDY" && currentRole !=  "SERIE")
                {
                    // For an image, role gives the path of the image.
                    ::boost::filesystem::path path(currentRole);
                    filenames.push_back(path);
                }
            }
            else
            {
                // We treat only the selected patient, study or serie.
                break;
            }
            ++it;
        }
    }
    else
    {
        // The selected item is a file.
        std::string filename;
        filename = (*it)->data(0, Qt::ToolTipRole).toString().toStdString();
        ::boost::filesystem::path path(filename);
        filenames.push_back(path);
    }
    return filenames;
}

//------------------------------------------------------------------------------
std::pair < ::ioGdcm::DicomReader, std::vector< ::boost::filesystem::path > > DicomdirEditor::show()
{
    std::pair< ::ioGdcm::DicomReader, std::vector< ::boost::filesystem::path > > readerAndFilenames;
    if(this->exec())
    {
        // Get the name of the reader.
        int index = m_readers->currentIndex();
        ::ioGdcm::DicomReader reader = ::ioGdcm::toDicomReader (m_readers->itemData(index).toInt());

        // Get the list of file
        QTreeWidgetItem *item = m_DicomdirStructure->currentItem();

        std::vector< ::boost::filesystem::path > filenames;
        filenames = this->getDicomFiles(item);

        readerAndFilenames = std::make_pair(reader, filenames);
    }
    return readerAndFilenames;
}

} //namespace ui
} //namespace ioGdcm
