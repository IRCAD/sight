#include <fwTools/UUID.hpp>
#include <fwData/Vector.hpp>
#include <fwData/Acquisition.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/IEditionService.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwComEd/AcquisitionMsg.hpp>
#include <uiAcquisitionQt/OrganTransformationEditor.hpp>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStringList>
#include <QString>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>


#include <fwGuiQt/container/QtContainer.hpp>


namespace uiAcquisition
{

#define DELETE_VTKOBJECT( o ) { if( o ) { o->Delete(); o = 0; } }

// OrganTransformationEditor

REGISTER_SERVICE( ::gui::editor::IEditor, OrganTransformationEditor, ::fwData::Acquisition );

OrganTransformationEditor::OrganTransformationEditor()
: ::gui::editor::IEditor(),
  mReconstructionMap(),
  mTMSUid(),
  mpReconstructionListBox( 0 ),
  mpResetButton( 0 )
{
	addNewHandledEvent( ::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION );
}

OrganTransformationEditor::~OrganTransformationEditor() throw()
{
}

void OrganTransformationEditor::configuring() throw( ::fwTools::Failed )
{
	this->initialize();
	if( m_configuration->findConfigurationElement( "TMSUid" ) )
	{
		mTMSUid = m_configuration->findConfigurationElement( "TMSUid" )->getValue();
	}
}

void OrganTransformationEditor::starting() throw( ::fwTools::Failed )
{
	this->create();
	::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
	QWidget* const container = qtContainer->getQtContainer();
	assert( container ) ;
	
	QVBoxLayout* layout = new QVBoxLayout(container);

	QGroupBox* groupBox = new QGroupBox(tr("Organs"), container );
	layout->addWidget( groupBox);

	QVBoxLayout* layoutGroupBox = new QVBoxLayout(container);
	groupBox->setLayout(layoutGroupBox);

	mpReconstructionListBox = new QListWidget( groupBox);
	mpResetButton           = new QPushButton(tr("Reset"), container );
	
	QObject::connect(mpReconstructionListBox, SIGNAL(itemClicked (QListWidgetItem * )), this, SLOT(OnReconstructionCheck(QListWidgetItem *)));
	QObject::connect(mpResetButton, SIGNAL(clicked( )), this, SLOT(OnResetClick()));
	
	layoutGroupBox->addWidget( mpReconstructionListBox, 1);
	layoutGroupBox->addWidget( mpResetButton, 0);
	
	container->setLayout( layout );
	
	updating();
}

void OrganTransformationEditor::stopping() throw( ::fwTools::Failed )
{
	
	QObject::disconnect(mpReconstructionListBox, SIGNAL(itemClicked (QListWidgetItem * )), this, SLOT(OnReconstructionCheck(QListWidgetItem *)));
	QObject::disconnect(mpResetButton, SIGNAL(clicked( )), this, SLOT(OnResetClick()));

	
	this->getContainer()->clean();
	this->destroy();
	
}

void OrganTransformationEditor::swapping() throw( ::fwTools::Failed )
{
	updating();
}

void OrganTransformationEditor::updating() throw( ::fwTools::Failed )
{
	Refresh();
}

void OrganTransformationEditor::updating( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed )
{
	::fwComEd::AcquisitionMsg::csptr pMessage = ::fwComEd::AcquisitionMsg::dynamicConstCast( msg );

	if( pMessage )
	{
		if( pMessage->hasEvent( ::fwComEd::AcquisitionMsg::ADD_RECONSTRUCTION ) )
		{
			updating();
			Notify();
		}
	}
}

void OrganTransformationEditor::info( ::std::ostream& sstream )
{
}

void OrganTransformationEditor::Refresh()
{
	mReconstructionMap.clear();
	mpReconstructionListBox->clear();

	::fwData::Acquisition::sptr pAcquisition = getObject< ::fwData::Acquisition>();

	::fwGuiQt::container::QtContainer::sptr qtContainer =  ::fwGuiQt::container::QtContainer::dynamicCast( this->getContainer() );
	QWidget* const container = qtContainer->getQtContainer();
	assert( container ) ;
	container->setEnabled( pAcquisition->getReconstructions().first != pAcquisition->getReconstructions().second );

	if( pAcquisition->getReconstructions().first != pAcquisition->getReconstructions().second )
	{
		::fwData::Acquisition::ReconstructionIterator it = pAcquisition->getReconstructions().first;

		for( ; it!= pAcquisition->getReconstructions().second; ++it )
		{
			mReconstructionMap[ (*it)->getOrganName() ] = (*it);
		}

		for( tReconstructionMap::iterator it = mReconstructionMap.begin(); it != mReconstructionMap.end(); ++it )
		{
			QListWidgetItem* item = new QListWidgetItem(QString::fromStdString((*it).first), mpReconstructionListBox);
			item->setCheckState(Qt::Unchecked);
			mpReconstructionListBox->addItem (item);
			
		}

		
	}
}

void OrganTransformationEditor::Notify()
{
	::boost::shared_ptr< ::fwData::Vector> pVector = ::fwTools::UUID::get< ::fwData::Vector>( mTMSUid );

	if( pVector )
	{
		::fwComEd::AcquisitionMsg::NewSptr message;
		message->addEvent( "Widget" );
		::fwServices::IEditionService::notify( getSptr(), pVector, message );
	}
}

void OrganTransformationEditor::OnReconstructionCheck(QListWidgetItem *currentItem)
{
	::boost::shared_ptr< ::fwData::Vector> pVector = ::fwData::Vector::dynamicCast(::fwTools::fwID::getObject(mTMSUid));

	if( pVector )
	{
		pVector->getRefContainer().clear();

		int count = mpReconstructionListBox->count();
		for( int i=0; i<count; ++i )
		{
			if( (mpReconstructionListBox->item( i )->checkState()) == Qt::Checked )
			{
				::std::string item_name = mpReconstructionListBox->item(i)->text().toStdString();
				::fwData::Reconstruction::sptr pReconstruction = mReconstructionMap[item_name];
				::fwData::TriangularMesh::sptr pMesh = pReconstruction->getTriangularMesh();
				if( pMesh )
				{
					pVector->getRefContainer().push_back( pMesh );
				}
			}
		}
	}

	Notify();
}



void OrganTransformationEditor::OnResetClick()
{
	int count = mpReconstructionListBox->count();
	for( int i=0; i<count; ++i )
	{
		if( (mpReconstructionListBox->item( i )->checkState()) == Qt::Checked )
		{
			
			::std::string item_name = mpReconstructionListBox->item(i)->text().toStdString();

			::fwData::Reconstruction::sptr         pReconstruction = mReconstructionMap[item_name];
			::fwData::TriangularMesh::sptr         pMesh           = pReconstruction->getTriangularMesh();
			

			::fwData::TransformationMatrix3D::sptr pTransformation = pMesh->getFieldSingleElement< ::fwData::TransformationMatrix3D>( "TransformMatrix" );
			::fwData::TransformationMatrix3D::NewSptr pIdentMat;
			pTransformation->deepCopy(pIdentMat);

			
		}
	}

	Notify();
}

}
