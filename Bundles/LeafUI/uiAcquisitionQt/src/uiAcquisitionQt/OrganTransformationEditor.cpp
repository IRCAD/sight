/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QListWidget>
#include <QListWidgetItem>
#include <QStringList>
#include <QString>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>

#include <map>

#include <fwTools/fwID.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Acquisition.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/AcquisitionMsg.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>
#include <fwComEd/helper/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiAcquisitionQt/OrganTransformationEditor.hpp"
#include "uiAcquisitionQt/OrganTransformationEditor.hpp"

namespace uiAcquisition
{

// OrganTransformationEditor

REGISTER_SERVICE( ::gui::editor::IEditor, OrganTransformationEditor, ::fwData::Acquisition );

OrganTransformationEditor::OrganTransformationEditor() throw()
:   ::gui::editor::IEditor(),
    mReconstructionMap(),
    mTMSUid(),
    mpReconstructionListBox( 0 ),
    mpResetButton( 0 ),
    mpSaveButton( 0 ),
    mpLoadButton( 0 ),
    mpTestButton( 0 )
{
    addNewHandledEvent( ::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION );
    this->mSaveCount = 0;
}

//------------------------------------------------------------------------------

OrganTransformationEditor::~OrganTransformationEditor() throw()
{}

//------------------------------------------------------------------------------

void OrganTransformationEditor::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
    if( m_configuration->findConfigurationElement( "TMSUid" ) )
    {
        mTMSUid = m_configuration->findConfigurationElement( "TMSUid" )->getValue();
    }
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::starting() throw( ::fwTools::Failed )
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout(container);

    QGroupBox* groupBox = new QGroupBox(tr("Organs"), container );
    layout->addWidget( groupBox);

    QVBoxLayout* layoutGroupBox = new QVBoxLayout(container);
    groupBox->setLayout(layoutGroupBox);

    mpReconstructionListBox = new QListWidget( groupBox);
    mpResetButton           = new QPushButton(tr("Reset"), container );
    mpSaveButton            = new QPushButton(tr("Save"), container );
    mpLoadButton            = new QPushButton(tr("Load"), container );
    mpSaveSelectionComboBox = new QComboBox(container);


    QObject::connect(mpReconstructionListBox, SIGNAL(itemClicked (QListWidgetItem * )), this, SLOT(OnReconstructionCheck(QListWidgetItem *)));
    QObject::connect(mpResetButton, SIGNAL(clicked( )), this, SLOT(OnResetClick()));
    QObject::connect(mpSaveButton, SIGNAL(clicked( )), this, SLOT(OnSaveClick()));
    QObject::connect(mpLoadButton, SIGNAL(clicked( )), this, SLOT(OnLoadClick()));

    //testing
    mpTestButton = new QPushButton(tr("Test"), container);
    QObject::connect(mpTestButton, SIGNAL(clicked()), this, SLOT(OnTestClick()));

    layoutGroupBox->addWidget( mpReconstructionListBox, 1);
    layoutGroupBox->addWidget( mpResetButton, 0);
    layoutGroupBox->addWidget( mpSaveButton, 0);
    layoutGroupBox->addWidget( mpSaveSelectionComboBox,0);
    layoutGroupBox->addWidget( mpLoadButton, 0);

    //test
    layoutGroupBox->addWidget( mpTestButton, 0);

    container->setLayout( layout );

    updating();
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::stopping() throw( ::fwTools::Failed )
{
    QObject::disconnect(mpReconstructionListBox, SIGNAL(itemClicked (QListWidgetItem * )), this, SLOT(OnReconstructionCheck(QListWidgetItem *)));
    QObject::disconnect(mpResetButton, SIGNAL(clicked( )), this, SLOT(OnResetClick()));
    QObject::disconnect(mpSaveButton, SIGNAL(clicked( )), this, SLOT(OnSaveClick()));
    QObject::disconnect(mpLoadButton, SIGNAL(clicked( )), this, SLOT(OnLoadClick()));

    //test
    QObject::disconnect(mpTestButton, SIGNAL(clicked( )), this, SLOT(OnTestClick()));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::swapping() throw( ::fwTools::Failed )
{
    updating();
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::updating() throw( ::fwTools::Failed )
{
    Refresh();
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::updating( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed )
{
    ::fwComEd::AcquisitionMsg::csptr pMessage = ::fwComEd::AcquisitionMsg::dynamicConstCast( msg );

    if( pMessage )
    {
        if( pMessage->hasEvent( ::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION ) )
        {
            updating();
        }
    }
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::info( ::std::ostream& sstream )
{}

//------------------------------------------------------------------------------

void OrganTransformationEditor::Refresh()
{
    mReconstructionMap.clear();
    mpReconstructionListBox->clear();

    ::fwData::Acquisition::sptr pAcquisition = getObject< ::fwData::Acquisition>();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);
    container->setEnabled( pAcquisition->getReconstructions().first != pAcquisition->getReconstructions().second );

    if( pAcquisition->getReconstructions().first != pAcquisition->getReconstructions().second )
    {
        ::fwData::Composite::sptr pComposite;
        if (::fwTools::fwID::exist(mTMSUid))
        {
            pComposite = ::fwData::Composite::dynamicCast(::fwTools::fwID::getObject(mTMSUid));
            SLM_ASSERT("Sorry, '"<< mTMSUid <<"' object is not a composite", pComposite);
        }

        ::fwData::Acquisition::ReconstructionIterator it = pAcquisition->getReconstructions().first;

        for( ; it!= pAcquisition->getReconstructions().second; ++it )
        {
            mReconstructionMap[ (*it)->getOrganName() ] = (*it);
        }

        for( tReconstructionMap::iterator it = mReconstructionMap.begin(); it != mReconstructionMap.end(); ++it )
        {
            std::string organName = it->first;
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(organName), mpReconstructionListBox);
            if( pComposite && pComposite->find(organName) != pComposite->end())
            {
                item->setCheckState(Qt::Checked);
            }
            else
            {
                item->setCheckState(Qt::Unchecked);
            }
            mpReconstructionListBox->addItem (item);
        }
    }
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::NotitfyTransformationMatrix(::fwData::TransformationMatrix3D::sptr aTransMat)
{
    ::fwComEd::TransformationMatrix3DMsg::NewSptr message;
    message->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
    ::fwServices::IEditionService::notify( getSptr(), aTransMat, message );
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::OnReconstructionCheck(QListWidgetItem *currentItem)
{
    ::fwData::Composite::sptr pComposite;
    if (::fwTools::fwID::exist(mTMSUid))
    {
        pComposite = ::fwData::Composite::dynamicCast(::fwTools::fwID::getObject(mTMSUid));
        SLM_ASSERT("Sorry, '"<< mTMSUid <<"' object is not a composite", pComposite);

        ::std::string item_name = currentItem->text().toStdString();
        ::fwData::Reconstruction::sptr pReconstruction = mReconstructionMap[item_name];
        ::fwData::Mesh::sptr pMesh = pReconstruction->getMesh();

        ::fwComEd::helper::Composite aCompositeHelper(pComposite);
        if ((currentItem->checkState()) == Qt::Checked)
        {
            if (pComposite->find(item_name) == pComposite->end())
                aCompositeHelper.add(item_name, pMesh);
            else
                aCompositeHelper.swap(item_name, pMesh);
        }
        else
        {
            if (pComposite->find(item_name) != pComposite->end())
                aCompositeHelper.remove(item_name);
        }
        aCompositeHelper.notify(this->getSptr());
    }
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::OnResetClick()
{
    ::fwData::Acquisition::sptr pAcquisition = getObject< ::fwData::Acquisition>();

    //search the corresponding triangular mesh
    if( pAcquisition->getReconstructions().first != pAcquisition->getReconstructions().second )
    {
        ::fwData::Acquisition::ReconstructionIterator it = pAcquisition->getReconstructions().first;

        for( ; it!= pAcquisition->getReconstructions().second; ++it )
        {

            ::fwData::Mesh::sptr pTmpTrMesh = (*it)->getMesh();

            ::fwData::TransformationMatrix3D::sptr pTmpMat = pTmpTrMesh->getFieldSingleElement< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
            if (pTmpMat)
            {
                ::fwData::TransformationMatrix3D::NewSptr pIdentMat;
                pTmpMat->deepCopy(pIdentMat);
                NotitfyTransformationMatrix(pTmpMat);
            }
        }
    }
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::OnSaveClick()
{
    tInnerMatMapping matMap;

    ::fwData::Acquisition::sptr pAcquisition = getObject< ::fwData::Acquisition>();

    if( pAcquisition->getReconstructions().first != pAcquisition->getReconstructions().second )
    {
        ::fwData::Acquisition::ReconstructionIterator it = pAcquisition->getReconstructions().first;

        for( ; it!= pAcquisition->getReconstructions().second; ++it )
        {

            ::fwData::Mesh::sptr pTmpTrMesh = (*it)->getMesh();
            ::fwData::TransformationMatrix3D::sptr pTmpMat = pTmpTrMesh->getFieldSingleElement< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
            if (pTmpMat)
            {
                ::fwData::TransformationMatrix3D::NewSptr pCpyTmpMat;
                pCpyTmpMat->deepCopy(pTmpMat);
                matMap[pTmpTrMesh->getID()] = pCpyTmpMat;
            }
        }

        ::std::stringstream tmpSaveName;
        tmpSaveName << "save_" << this->mSaveCount;
        mSaveListing[tmpSaveName.str()] = matMap;
        this->mpSaveSelectionComboBox->addItem(QString::fromStdString(tmpSaveName.str()));
        this->mSaveCount++;
    }

}

//------------------------------------------------------------------------------

void OrganTransformationEditor::OnLoadClick()
{
    if (this->mpSaveSelectionComboBox->count() != 0)
    {

        tInnerMatMapping matMap = mSaveListing[this->mpSaveSelectionComboBox->currentText().toStdString()];

        ::fwData::Acquisition::sptr pAcquisition = getObject< ::fwData::Acquisition>();

        //search the corresponding triangular mesh
        if( pAcquisition->getReconstructions().first != pAcquisition->getReconstructions().second )
        {
            ::fwData::Acquisition::ReconstructionIterator it = pAcquisition->getReconstructions().first;

            for( ; it!= pAcquisition->getReconstructions().second; ++it )
            {

                ::fwData::Mesh::sptr pTmpTrMesh = (*it)->getMesh();
                if (matMap.find(pTmpTrMesh->getID()) != matMap.end())
                {
                    ::fwData::TransformationMatrix3D::sptr pTmpMat = pTmpTrMesh->getFieldSingleElement< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
                    if (pTmpMat)
                    {
                        pTmpMat->shallowCopy(matMap[pTmpTrMesh->getID()]);
                        NotitfyTransformationMatrix(pTmpMat);
                    }
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void OrganTransformationEditor::OnTestClick()
{

    ::fwData::TransformationMatrix3D::NewSptr pRandTmpMat;
    srand(time(NULL));

    //randomize the translation parts
    //pRandTmpMat->setCoefficient(0, 3, (double)(-rand()%50) + 25);
    //pRandTmpMat->setCoefficient(1, 3, (double)(-rand()%50) + 25);
    //pRandTmpMat->setCoefficient(2, 3, (double)(-rand()%50) + 25);

    //randomize the 12 parameters of affine transformation matrix
    for (unsigned int i = 0; i < 3; i++)
        for(unsigned int j = 0; j < 4; j++)
            pRandTmpMat->setCoefficient(i, j, (double)(-rand()%4 + 2));

    tInnerMatMapping matMap = mSaveListing[this->mpSaveSelectionComboBox->currentText().toStdString()];

    ::fwData::Acquisition::sptr pAcquisition = getObject< ::fwData::Acquisition>();

    //search the corresponding triangular mesh
    if( pAcquisition->getReconstructions().first != pAcquisition->getReconstructions().second )
    {
        ::fwData::Acquisition::ReconstructionIterator it = pAcquisition->getReconstructions().first;

        for( ; it!= pAcquisition->getReconstructions().second; ++it )
        {

            ::fwData::Mesh::sptr pTmpTrMesh = (*it)->getMesh();

            ::fwData::TransformationMatrix3D::sptr pTmpMat = pTmpTrMesh->getFieldSingleElement< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
            if (pTmpMat)
            {
                pTmpMat->deepCopy(pRandTmpMat);

                NotitfyTransformationMatrix(pTmpMat);
            }
        }
    }
}

//------------------------------------------------------------------------------

}
