/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
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
#include <fwData/Material.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/ModelSeriesMsg.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>
#include <fwComEd/helper/Composite.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include "uiMedDataQt/editor/SOrganTransformation.hpp"

namespace uiMedData
{
namespace editor
{

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiMedData::editor::SOrganTransformation, ::fwMedData::ModelSeries );

SOrganTransformation::SOrganTransformation() throw() :
    m_saveButton( 0 ),
    m_loadButton( 0 ),
    m_resetButton( 0 ),
    m_reconstructionListBox( 0 ),
    m_testButton( 0 ),
    m_saveCount( 0 )
{
    //addNewHandledEvent( ::fwComEd::ModelSeriesMsg::ADD_RECONSTRUCTION );
}

//------------------------------------------------------------------------------

SOrganTransformation::~SOrganTransformation() throw()
{}

//------------------------------------------------------------------------------

void SOrganTransformation::configuring() throw( ::fwTools::Failed )
{
    this->initialize();
    if( m_configuration->findConfigurationElement( "TMSUid" ) )
    {
        m_TMSUid = m_configuration->findConfigurationElement( "TMSUid" )->getValue();
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::starting() throw( ::fwTools::Failed )
{
    this->create();
    ::fwGuiQt::container::QtContainer::sptr qtContainer =
            ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    QVBoxLayout* layout = new QVBoxLayout(container);

    QGroupBox* groupBox = new QGroupBox(tr("Organs"), container );
    layout->addWidget( groupBox);

    QVBoxLayout* layoutGroupBox = new QVBoxLayout(container);
    groupBox->setLayout(layoutGroupBox);

    m_reconstructionListBox = new QListWidget( groupBox);
    m_resetButton           = new QPushButton(tr("Reset"), container );
    m_saveButton            = new QPushButton(tr("Save"), container );
    m_loadButton            = new QPushButton(tr("Load"), container );
    m_saveSelectionComboBox = new QComboBox(container);


    QObject::connect(m_reconstructionListBox,
                     SIGNAL(itemClicked (QListWidgetItem * )),
                     this,
                     SLOT(onReconstructionCheck(QListWidgetItem *)));
    QObject::connect(m_resetButton, SIGNAL(clicked( )), this, SLOT(onResetClick()));
    QObject::connect(m_saveButton, SIGNAL(clicked( )), this, SLOT(onSaveClick()));
    QObject::connect(m_loadButton, SIGNAL(clicked( )), this, SLOT(onLoadClick()));

    //testing
    m_testButton = new QPushButton(tr("Test"), container);
    QObject::connect(m_testButton, SIGNAL(clicked()), this, SLOT(onTestClick()));

    layoutGroupBox->addWidget( m_reconstructionListBox, 1);
    layoutGroupBox->addWidget( m_resetButton, 0);
    layoutGroupBox->addWidget( m_saveButton, 0);
    layoutGroupBox->addWidget( m_saveSelectionComboBox,0);
    layoutGroupBox->addWidget( m_loadButton, 0);

    //test
    layoutGroupBox->addWidget( m_testButton, 0);

    container->setLayout( layout );

    this->updating();
}

//------------------------------------------------------------------------------

void SOrganTransformation::stopping() throw( ::fwTools::Failed )
{
    QObject::disconnect(m_reconstructionListBox,
                        SIGNAL(itemClicked (QListWidgetItem * )),
                        this,
                        SLOT(onReconstructionCheck(QListWidgetItem *)));
    QObject::disconnect(m_resetButton, SIGNAL(clicked( )), this, SLOT(onResetClick()));
    QObject::disconnect(m_saveButton, SIGNAL(clicked( )), this, SLOT(onSaveClick()));
    QObject::disconnect(m_loadButton, SIGNAL(clicked( )), this, SLOT(onLoadClick()));

    //test
    QObject::disconnect(m_testButton, SIGNAL(clicked( )), this, SLOT(onTestClick()));

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SOrganTransformation::swapping() throw( ::fwTools::Failed )
{
    this->updating();
}

//------------------------------------------------------------------------------

void SOrganTransformation::updating() throw( ::fwTools::Failed )
{
    this->refresh();
}

//------------------------------------------------------------------------------

void SOrganTransformation::receiving( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed )
{
    ::fwComEd::ModelSeriesMsg::csptr pMessage = ::fwComEd::ModelSeriesMsg::dynamicConstCast( msg );

    if( pMessage && pMessage->hasEvent( ::fwComEd::ModelSeriesMsg::ADD_RECONSTRUCTION ) )
    {
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::info( ::std::ostream& sstream )
{}

//------------------------------------------------------------------------------

void SOrganTransformation::refresh()
{
    m_reconstructionMap.clear();
    m_reconstructionListBox->clear();

    ::fwMedData::ModelSeries::sptr series = this->getObject< ::fwMedData::ModelSeries >();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
            ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
    QWidget* const container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    bool hasReconstructions = !series->getReconstructionDB().empty();
    container->setEnabled( hasReconstructions );

    if(hasReconstructions)
    {
        ::fwData::Composite::sptr pComposite;
        if (::fwTools::fwID::exist(m_TMSUid))
        {
            pComposite = ::fwData::Composite::dynamicCast(::fwTools::fwID::getObject(m_TMSUid));
            SLM_ASSERT("Sorry, '"<< m_TMSUid <<"' object is not a composite", pComposite);
        }

        BOOST_FOREACH(::fwData::Reconstruction::sptr rec, series->getReconstructionDB())
        {
            m_reconstructionMap[ rec->getOrganName() ] = rec;
        }

        for( ReconstructionMapType::iterator it = m_reconstructionMap.begin(); it != m_reconstructionMap.end(); ++it )
        {
            std::string organName = it->first;
            QListWidgetItem* item = new QListWidgetItem(QString::fromStdString(organName), m_reconstructionListBox);
            if( pComposite && pComposite->find(organName) != pComposite->end())
            {
                item->setCheckState(Qt::Checked);
            }
            else
            {
                item->setCheckState(Qt::Unchecked);
            }
            m_reconstructionListBox->addItem (item);
        }
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::notitfyTransformationMatrix(::fwData::TransformationMatrix3D::sptr aTransMat)
{
    ::fwComEd::TransformationMatrix3DMsg::sptr message = ::fwComEd::TransformationMatrix3DMsg::New();
    message->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
    ::fwServices::IEditionService::notify( getSptr(), aTransMat, message );
}

//------------------------------------------------------------------------------

void SOrganTransformation::onReconstructionCheck(QListWidgetItem *currentItem)
{
    ::fwData::Composite::sptr pComposite;
    if (::fwTools::fwID::exist(m_TMSUid))
    {
        pComposite = ::fwData::Composite::dynamicCast(::fwTools::fwID::getObject(m_TMSUid));
        SLM_ASSERT("Sorry, '"<< m_TMSUid <<"' object is not a composite", pComposite);

        ::std::string item_name = currentItem->text().toStdString();
        ::fwData::Reconstruction::sptr pReconstruction = m_reconstructionMap[item_name];
        ::fwData::Mesh::sptr pMesh = pReconstruction->getMesh();

        ::fwComEd::helper::Composite aCompositeHelper(pComposite);
        if ((currentItem->checkState()) == Qt::Checked)
        {
            if (pComposite->find(item_name) == pComposite->end())
            {
                aCompositeHelper.add(item_name, pMesh);
            }
            else
            {
                aCompositeHelper.swap(item_name, pMesh);
            }
        }
        else
        {
            if (pComposite->find(item_name) != pComposite->end())
            {
                aCompositeHelper.remove(item_name);
            }
        }
        aCompositeHelper.notify(this->getSptr());
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onResetClick()
{
    ::fwMedData::ModelSeries::sptr series = this->getObject< ::fwMedData::ModelSeries >();

    //search the corresponding triangular mesh
    BOOST_FOREACH(::fwData::Reconstruction::sptr rec, series->getReconstructionDB())
    {
        ::fwData::Mesh::sptr pTmpTrMesh = rec->getMesh();

        ::fwData::TransformationMatrix3D::sptr pTmpMat =
                pTmpTrMesh->getField< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
        if (pTmpMat)
        {
            ::fwData::TransformationMatrix3D::sptr pIdentMat;
            pTmpMat = ::fwData::Object::copy(pIdentMat);
            this->notitfyTransformationMatrix(pTmpMat);
        }
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onSaveClick()
{
    InnerMatMappingType matMap;

    ::fwMedData::ModelSeries::sptr series = this->getObject< ::fwMedData::ModelSeries >();

    if(!series->getReconstructionDB().empty())
    {
        BOOST_FOREACH(::fwData::Reconstruction::sptr rec, series->getReconstructionDB())
        {
            ::fwData::Mesh::sptr pTmpTrMesh = rec->getMesh();
            ::fwData::TransformationMatrix3D::sptr pTmpMat =
                    pTmpTrMesh->getField< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
            if (pTmpMat)
            {
                ::fwData::TransformationMatrix3D::sptr pCpyTmpMat;
                pCpyTmpMat = ::fwData::Object::copy(pTmpMat);
                matMap[pTmpTrMesh->getID()] = pCpyTmpMat;
            }
        }

        ::std::stringstream tmpSaveName;
        tmpSaveName << "save_" << m_saveCount;
        m_saveListing[tmpSaveName.str()] = matMap;
        m_saveSelectionComboBox->addItem(QString::fromStdString(tmpSaveName.str()));
        m_saveCount++;
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onLoadClick()
{
    if (m_saveSelectionComboBox->count() != 0)
    {
        InnerMatMappingType matMap = m_saveListing[m_saveSelectionComboBox->currentText().toStdString()];

        ::fwMedData::ModelSeries::sptr series = this->getObject< ::fwMedData::ModelSeries >();

        //search the corresponding triangular mesh
        BOOST_FOREACH(::fwData::Reconstruction::sptr rec, series->getReconstructionDB())
        {
            ::fwData::Mesh::sptr pTmpTrMesh = rec->getMesh();
            if (matMap.find(pTmpTrMesh->getID()) != matMap.end())
            {
                ::fwData::TransformationMatrix3D::sptr pTmpMat =
                        pTmpTrMesh->getField< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
                if (pTmpMat)
                {
                    pTmpMat->shallowCopy(matMap[pTmpTrMesh->getID()]);
                    this->notitfyTransformationMatrix(pTmpMat);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

void SOrganTransformation::onTestClick()
{
    ::fwData::TransformationMatrix3D::sptr pRandTmpMat = ::fwData::TransformationMatrix3D::New();
    srand(time(NULL));

    //randomize the translation parts
    //pRandTmpMat->setCoefficient(0, 3, (double)(-rand()%50) + 25);
    //pRandTmpMat->setCoefficient(1, 3, (double)(-rand()%50) + 25);
    //pRandTmpMat->setCoefficient(2, 3, (double)(-rand()%50) + 25);

    //randomize the 12 parameters of affine transformation matrix
    for (unsigned int i = 0; i < 3; i++)
    {
        for(unsigned int j = 0; j < 4; j++)
        {
            pRandTmpMat->setCoefficient(i, j, (double)(-rand()%4 + 2));
        }
    }

    InnerMatMappingType matMap = m_saveListing[m_saveSelectionComboBox->currentText().toStdString()];

    ::fwMedData::ModelSeries::sptr series = this->getObject< ::fwMedData::ModelSeries >();

    //search the corresponding triangular mesh
    BOOST_FOREACH(::fwData::Reconstruction::sptr rec, series->getReconstructionDB())
    {
        ::fwData::Mesh::sptr pTmpTrMesh = rec->getMesh();

        ::fwData::TransformationMatrix3D::sptr pTmpMat =
                pTmpTrMesh->getField< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
        if (pTmpMat)
        {
            pTmpMat->setCoefficients(pRandTmpMat->getCoefficients());
            this->notitfyTransformationMatrix(pTmpMat);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace editor
} // namespace uiMedData

