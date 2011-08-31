/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>
#include <QList>
#include <QVariant>
#include <QByteArray>

#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>
#include <fwData/PatientDB.hpp>
#include <fwData/Patient.hpp>
#include <fwData/Study.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectMsg.hpp>

#include <fwServices/Base.hpp>
#include <fwComEd/Dictionary.hpp>
#include <fwComEd/PatientDBMsg.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/fieldHelper/BackupHelper.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiPatientDBQt/PatientDBInfoEditor.hpp"

using namespace fwServices;

namespace uiPatientDB
{

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiPatientDB::PatientDBInfoEditor , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

PatientDBInfoEditor::PatientDBInfoEditor()
{
    SLM_TRACE_FUNC();
    addNewHandledEvent( ::fwComEd::PatientDBMsg::CLEAR_PATIENT );
    addNewHandledEvent( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED );
}

//------------------------------------------------------------------------------

PatientDBInfoEditor::~PatientDBInfoEditor() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PatientDBInfoEditor::info(std::ostream &_sstream )
{
    // Update message
    _sstream << std::string("PatientDBInfoEditor");
}

//------------------------------------------------------------------------------

void PatientDBInfoEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout();

    m_editorPanel = new QTreeWidget(container);
    m_editorPanel->setSelectionMode(QAbstractItemView::SingleSelection);
    m_editorPanel->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_editorPanel->setColumnCount(2);
    m_editorPanel->setAlternatingRowColors( true );
    m_editorPanel->setColumnWidth(0,170);

    QStringList labels;
    labels << tr("Field") << tr("Value");
    m_editorPanel->setHeaderLabels(labels);

    layout->addWidget( m_editorPanel, 1);
    layout->setContentsMargins(0,0,0,0);
    container->setLayout( layout );

    this->updating();
}

//------------------------------------------------------------------------------

void PatientDBInfoEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // If message is a pPatientDBMsg
    ::fwComEd::PatientDBMsg::csptr pPatientDBMsg = ::fwComEd::PatientDBMsg::dynamicConstCast( _msg ) ;
    if ( pPatientDBMsg )
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void PatientDBInfoEditor::stopping() throw(::fwTools::Failed)
{
    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void PatientDBInfoEditor::updating() throw(::fwTools::Failed)
{
    m_editorPanel->clearSelection();
    m_editorPanel->clear();

    ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();
    ::fwData::Patient::sptr patient = ::fwComEd::fieldHelper::BackupHelper::getSelectedPatient(pPatientDB);
    ::fwData::Study::sptr study = ::fwComEd::fieldHelper::BackupHelper::getSelectedStudy(pPatientDB);
    ::fwData::Acquisition::sptr acq = ::fwComEd::fieldHelper::BackupHelper::getSelectedAcquisition(pPatientDB);

    if (patient)
    {
        QTreeWidgetItem* patientItem = new QTreeWidgetItem();
        patientItem->setText(0, tr("Patient"));
        m_editorPanel->addTopLevelItem( patientItem );

        QTreeWidgetItem* birthdateItem = new QTreeWidgetItem();
        std::string date    = boost::posix_time::to_iso_extended_string( patient->getBirthdate() );
        date =  date.substr(0,10);
        birthdateItem->setText(0, tr("Birthdate"));
        birthdateItem->setText(1, QString::fromStdString(date));
        patientItem->addChild(birthdateItem);

        QTreeWidgetItem* sexItem = new QTreeWidgetItem();
        sexItem->setText(0, tr("Sex"));
        std::string sex = patient->getIsMale()?"Male":"Female";
        sexItem->setText(1, QString::fromStdString(sex));
        patientItem->addChild(sexItem);

        QTreeWidgetItem* dicomIdItem = new QTreeWidgetItem();
        dicomIdItem->setText(0, tr("DICOM ID"));
        std::string dicomId = patient->getIDDicom();
        dicomIdItem->setText(1, QString::fromStdString(dicomId));
        patientItem->addChild(dicomIdItem);
    }
    if (study)
    {
        // study
        QTreeWidgetItem* studyItem = new QTreeWidgetItem();
        studyItem->setText(0, tr("Study"));
        m_editorPanel->addTopLevelItem( studyItem );

        QTreeWidgetItem* hospitalItem = new QTreeWidgetItem();
        hospitalItem->setText(0, tr("Hospital"));
        hospitalItem->setText(1, QString::fromStdString(study->getHospital()));
        studyItem->addChild(hospitalItem);
    }
    if (acq)
    {
        // serie
        QTreeWidgetItem* serieItem = new QTreeWidgetItem();
        serieItem->setText(0, tr("Serie"));
        m_editorPanel->addTopLevelItem( serieItem );


        QTreeWidgetItem* bitsPerPixelItem = new QTreeWidgetItem();
        bitsPerPixelItem->setText(0, tr("BitsPerPixel"));
        std::string bitsPerPixel = ::fwTools::getString<int>(acq->getBitsPerPixel());
        bitsPerPixelItem->setText(1, QString::fromStdString(bitsPerPixel));
        serieItem->addChild(bitsPerPixelItem);

        QTreeWidgetItem* imageTypeItem = new QTreeWidgetItem();
        imageTypeItem->setText(0, tr("ImagePixelType"));
        imageTypeItem->setText(1, QString::fromStdString(acq->getImage()->getPixelType().string()));
        serieItem->addChild(imageTypeItem);

#ifdef USE_INTERNAL_FEATURES

        QTreeWidgetItem* laboIdItem = new QTreeWidgetItem();
        laboIdItem->setText(0, tr("Labo ID"));
        std::string laboId = ::fwTools::getString<int>(acq->getLaboID());
        laboIdItem->setText(1, QString::fromStdString(laboId));
        serieItem->addChild(laboIdItem);
#endif

    }
    m_editorPanel->expandAll();
}

//------------------------------------------------------------------------------

void PatientDBInfoEditor::configuring() throw(::fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

} // namespace uiPatientDB
