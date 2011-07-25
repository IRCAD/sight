/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QVBoxLayout>
#include <QList>
#include <QVariant>
#include <QByteArray>
#include <QIcon>
#include <QPixmap>

#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>

#include <fwCore/base.hpp>

#include <fwData/Object.hpp>
#include <fwData/PatientDB.hpp>
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
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiPatientDBQt/PatientDBGuiSelectorService.hpp"

using namespace fwServices;

namespace uiPatientDB
{

static const char * eye_xpm[] = {
"16 16 2 1",
"   c None",
"X  c Black",
"                ",
"                ",
"      XXXXX     ",
"   XXXXXXXXXX   ",
" XX      XXXXXX ",
"X           XXXX",
"              XX",
"    XXXXXXX     ",
"  XXXXXXXXXXX   ",
" XX  XXXXX  XX  ",
"X    XX XX    X ",
" X   XXXXX   X  ",
"  XX  XXX  XX   ",
"    XXXXXXX     ",
"                ",
"                "};

//------------------------------------------------------------------------------

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiPatientDB::PatientDBGuiSelectorService , ::fwData::PatientDB ) ;

//------------------------------------------------------------------------------

PatientDBGuiSelectorService::PatientDBGuiSelectorService()
{
    SLM_TRACE_FUNC();
    addNewHandledEvent( ::fwComEd::PatientDBMsg::ADD_PATIENT );
    addNewHandledEvent( ::fwComEd::PatientDBMsg::NEW_PATIENT );
    addNewHandledEvent( ::fwComEd::PatientDBMsg::CLEAR_PATIENT );
}

//------------------------------------------------------------------------------

PatientDBGuiSelectorService::~PatientDBGuiSelectorService() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PatientDBGuiSelectorService::info(std::ostream &_sstream )
{
    // Update message
    _sstream << std::string("PatientDBGuiSelectorService");
}

//------------------------------------------------------------------------------

void PatientDBGuiSelectorService::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->::fwGui::IGuiContainerSrv::create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout();

    m_pSelectorPanel = new QTreeWidget(container);
    m_pSelectorPanel->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pSelectorPanel->setAlternatingRowColors( true );

    QStringList labels;
    labels << tr("Name") << tr("Modality") << tr("Acq. date") << tr("Image size") << tr("Voxel size") << tr("Comment");
    m_pSelectorPanel->setHeaderLabels(labels);

    int cellWidthT1 = 80;
    int cellWidthT2 = 160;
    int cellWidthT3 = 200;
    int cellWidthT4 = 400;
    m_pSelectorPanel->setColumnWidth(0, cellWidthT3);
    m_pSelectorPanel->setColumnWidth(1, cellWidthT1);
    m_pSelectorPanel->setColumnWidth(2, cellWidthT2);
    m_pSelectorPanel->setColumnWidth(3, cellWidthT2);
    m_pSelectorPanel->setColumnWidth(4, cellWidthT2);
    m_pSelectorPanel->setColumnWidth(5, cellWidthT4);

    layout->addWidget( m_pSelectorPanel, 1);
    layout->setContentsMargins(0,0,0,0);
    container->setLayout( layout );

    QObject::connect(m_pSelectorPanel, SIGNAL(currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* )), this, SLOT(onSelectionChange( QTreeWidgetItem*, QTreeWidgetItem* )));

    this->updating();
}

//------------------------------------------------------------------------------

void PatientDBGuiSelectorService::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // If message is a pPatientDBMsg
    ::fwComEd::PatientDBMsg::csptr pPatientDBMsg = ::fwComEd::PatientDBMsg::dynamicConstCast( _msg ) ;
    if ( pPatientDBMsg )
    {
        if (pPatientDBMsg->hasEvent(::fwComEd::PatientDBMsg::ADD_PATIENT))
        {
            ::fwData::Object::csptr dataInfo = pPatientDBMsg->getDataInfo(::fwComEd::PatientDBMsg::ADD_PATIENT);
            if ( dataInfo )
            {
                ::fwData::Integer::csptr index = ::fwData::Integer::dynamicConstCast(dataInfo);
                this->selectLastAddedImage(index->value());
            }
        }
        this->updating();
    }
}

//------------------------------------------------------------------------------

void PatientDBGuiSelectorService::stopping() throw(::fwTools::Failed)
{
    QObject::disconnect(m_pSelectorPanel, SIGNAL(currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* )), this, SLOT(onSelectionChange( QTreeWidgetItem*, QTreeWidgetItem* )));

    this->getContainer()->clean();
    this->::fwGui::IGuiContainerSrv::destroy();
}

//------------------------------------------------------------------------------

void PatientDBGuiSelectorService::updating() throw(::fwTools::Failed)
{
    m_pSelectorPanel->clearSelection();
    m_pSelectorPanel->clear();

    ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();
    ::fwData::PatientDB::PatientIterator patientBegin = pPatientDB->getPatients().first;
    ::fwData::PatientDB::PatientIterator patientEnd   = pPatientDB->getPatients().second;
    ::fwData::PatientDB::PatientIterator patient      = patientBegin;

    int selPatient = 0;
    int selStudy = 0;
    int selAcq = 0;
    ::fwTools::Field::sptr pDataInfo = pPatientDB->getField( fwComEd::Dictionary::m_imageSelectedId );
    if (pDataInfo)
    {
        ::fwData::Integer::sptr myIntPat = ::fwData::Integer::dynamicCast( pDataInfo->children().at(0) );
        ::fwData::Integer::sptr myIntStu = ::fwData::Integer::dynamicCast( pDataInfo->children().at(1) );
        ::fwData::Integer::sptr myIntAcq = ::fwData::Integer::dynamicCast( pDataInfo->children().at(2) );
        selPatient = myIntPat->value();
        selStudy = myIntStu->value();
        selAcq = myIntAcq->value();
    }

    int indexP = 0;
    while ( patient != patientEnd )
    {
        int indexS = 0;
        int indexA = 0;
        indexP = patient - patientBegin;

        ::fwData::Patient::sptr pPatient = *patient;
        ::fwData::Patient::StudyIterator studyBegin = pPatient->getStudies().first;
        ::fwData::Patient::StudyIterator studyEnd = pPatient->getStudies().second;
        ::fwData::Patient::StudyIterator study = studyBegin;

        QTreeWidgetItem* patientItem = new QTreeWidgetItem();
        std::string name = pPatient->getName() + " " + pPatient->getFirstname();
        patientItem->setText(0, QString::fromStdString(name));
        QByteArray selectionP;
        selectionP.append(indexP);
        selectionP.append(indexS);
        selectionP.append(indexA);
        QVariant v(selectionP);
        patientItem->setData(0, Qt::UserRole, v);
        m_pSelectorPanel->addTopLevelItem( patientItem );

        while ( study != studyEnd )
        {
            indexS = study - studyBegin;
            ::fwData::Study::sptr pStudy = *study;
            ::fwData::Study::AcquisitionIterator acquisitionBegin = pStudy->getAcquisitions().first;
            ::fwData::Study::AcquisitionIterator acquisitionEnd = pStudy->getAcquisitions().second;
            ::fwData::Study::AcquisitionIterator acquisition = acquisitionBegin;

            while ( acquisition != acquisitionEnd )
            {
                indexA = acquisition - acquisitionBegin;
                ::fwData::Acquisition::sptr pAcquisition = *acquisition;

                // Get Infos
                std::string zone = (pStudy->getAcquisitionZone().length())?pStudy->getAcquisitionZone():"Unknown zone";
                std::string voxelSize = "";
                std::string nbImages = "";
                std::string acqDate = ::boost::posix_time::to_iso_extended_string( pAcquisition->getCreationDate() );
                acqDate =  acqDate.substr(0,10) + " " + acqDate.substr(11,5);
                if ( pAcquisition->getImage()->getDimension() == 3 )
                {
                    std::stringstream nbImagesStream;
                    nbImagesStream << pAcquisition->getImage()->getSize()[0] << "  x " << pAcquisition->getImage()->getSize()[1] << " x " << pAcquisition->getImage()->getSize()[2];
                    nbImages = nbImagesStream.str();

                    std::stringstream voxelSizeStream;
                    voxelSizeStream << pAcquisition->getImage()->getSpacing()[0] << " x "
                                    << pAcquisition->getImage()->getSpacing()[1] << " x "
                                    << pAcquisition->getImage()->getSpacing()[2];
                    voxelSize = voxelSizeStream.str();
                }
                ::fwComEd::fieldHelper::MedicalImageHelpers::checkComment(pAcquisition->getImage());
                if ( ! pAcquisition->getImage()->getFieldSize( ::fwComEd::Dictionary::m_imageLabelId) )
                {
                    ::fwComEd::fieldHelper::MedicalImageHelpers::setImageLabel(pPatient, pAcquisition->getImage());
                }
                std::string comment = pAcquisition->getImage()->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_imageLabelId )->value();
                comment += " : " + pAcquisition->getImage()->getFieldSingleElement< ::fwData::String >( ::fwComEd::Dictionary::m_commentId )->value();

                QTreeWidgetItem* acquisitionItem = new QTreeWidgetItem();
                acquisitionItem->setText(0, QString::fromStdString(zone));
                acquisitionItem->setText(1, QString::fromStdString(pStudy->getModality()));
                acquisitionItem->setText(2, QString::fromStdString(acqDate));
                acquisitionItem->setText(3, QString::fromStdString(nbImages));
                acquisitionItem->setText(4, QString::fromStdString(voxelSize));
                acquisitionItem->setText(5, QString::fromStdString(comment));
                QByteArray selection;
                selection.append(indexP);
                selection.append(indexS);
                selection.append(indexA);
                QVariant v(selection);
                acquisitionItem->setData(0, Qt::UserRole, v);
                patientItem->addChild(acquisitionItem);

                if (indexP == selPatient && indexS == selStudy && indexA == selAcq)
                {
                    m_pSelectorPanel->setCurrentItem(acquisitionItem);
                    m_pSelectorPanel->expandItem(acquisitionItem);
                }
                ++acquisition;
            }
            ++study;
        }

        ++patient;
    }
}

//------------------------------------------------------------------------------

void PatientDBGuiSelectorService::onSelectionChange(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
    SLM_TRACE_FUNC();

    if (current && (current != previous))
    {
        if(previous)
        {
            if(previous->childCount() > 0)
            {
                //patient deselected, we deselect first acquisition
                previous->child(0)->setIcon(0, QIcon());
            }
            else
            {
                previous->setIcon(0, QIcon());
            }
        }
        QPixmap pixmap(eye_xpm);
        if(current->childCount() > 0)
        {
            //patient selected, we select first acquisition
            current->child(0)->setIcon(0, QIcon(pixmap));
        }
        else
        {
            current->setIcon(0, QIcon(pixmap));
        }


        ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();

        QVariant variant = current->data(0, Qt::UserRole);
        QByteArray list = variant.value< QByteArray >();

        ::fwData::Object::NewSptr acqSelected;
        acqSelected->children().push_back( ::fwData::Integer::NewSptr(list[0]) );
        acqSelected->children().push_back( ::fwData::Integer::NewSptr(list[1]) );
        acqSelected->children().push_back( ::fwData::Integer::NewSptr(list[2]) );

        // Add Field
        pPatientDB->removeField( ::fwComEd::Dictionary::m_imageSelectedId );
        pPatientDB->addFieldElement( ::fwComEd::Dictionary::m_imageSelectedId, ::fwData::Integer::NewSptr(list[0]));
        pPatientDB->addFieldElement( ::fwComEd::Dictionary::m_imageSelectedId, ::fwData::Integer::NewSptr(list[1]));
        pPatientDB->addFieldElement( ::fwComEd::Dictionary::m_imageSelectedId, ::fwData::Integer::NewSptr(list[2]));

        // notification
        ::fwComEd::PatientDBMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::PatientDBMsg::NEW_IMAGE_SELECTED, acqSelected );
        ::fwServices::IEditionService::notify(this->getSptr(), pPatientDB, msg);
    }
}

//------------------------------------------------------------------------------

void PatientDBGuiSelectorService::configuring() throw(::fwTools::Failed)
{
    this->::fwGui::IGuiContainerSrv::initialize();
}

//------------------------------------------------------------------------------

void PatientDBGuiSelectorService::selectLastAddedImage(int patientIndex)
{
    ::fwData::PatientDB::sptr pPatientDB = this->getObject< ::fwData::PatientDB >();

    ::fwData::PatientDB::PatientIterator patientIter = pPatientDB->getPatients().first;
    patientIter += patientIndex;

    int studyIndex = (*patientIter)->getStudySize() - 1;
    ::fwData::Patient::StudyIterator studyIter = (*patientIter)->getStudies().first;
    studyIter += studyIndex;

    int acqIndex = (*studyIter)->getAcquisitionSize() - 1;

    pPatientDB->removeField( ::fwComEd::Dictionary::m_imageSelectedId );
    pPatientDB->addFieldElement( ::fwComEd::Dictionary::m_imageSelectedId, ::fwData::Integer::NewSptr(patientIndex));
    pPatientDB->addFieldElement( ::fwComEd::Dictionary::m_imageSelectedId, ::fwData::Integer::NewSptr(studyIndex));
    pPatientDB->addFieldElement( ::fwComEd::Dictionary::m_imageSelectedId, ::fwData::Integer::NewSptr(acqIndex));
}

//------------------------------------------------------------------------------

} // namespace uiPatientDB
