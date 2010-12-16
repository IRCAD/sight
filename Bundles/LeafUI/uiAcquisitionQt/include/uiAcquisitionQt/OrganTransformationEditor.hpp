#ifndef _UIACQUISITIONQT_ORGANTRANSFORMATIONEDITOR_HPP_
#define _UIACQUISITIONQT_ORGANTRANSFORMATIONEDITOR_HPP_

#include <QPointer>
#include <QObject>

#include <map>
#include <string>
#include <fwData/Reconstruction.hpp>
#include <gui/editor/IEditor.hpp>
#include <uiAcquisitionQt/Config.hpp>


class QListWidget;
class QPushButton;
class QListWidgetItem;

namespace uiAcquisition
{

// OrganTransformationEditor
class UIACQUISITIONQT_CLASS_API OrganTransformationEditor : public QObject, public ::gui::editor::IEditor
{
	Q_OBJECT
public:

	fwCoreServiceClassDefinitionsMacro( (OrganTransformationEditor)(::gui::editor::IEditor) );

	UIACQUISITIONQT_API              OrganTransformationEditor() throw();
	UIACQUISITIONQT_API virtual     ~OrganTransformationEditor() throw();

protected:

	UIACQUISITIONQT_API virtual void configuring() throw( ::fwTools::Failed );
	UIACQUISITIONQT_API virtual void starting()    throw( ::fwTools::Failed );
	UIACQUISITIONQT_API virtual void stopping()    throw( ::fwTools::Failed );
	UIACQUISITIONQT_API virtual void swapping()    throw( ::fwTools::Failed );
	UIACQUISITIONQT_API virtual void updating()    throw( ::fwTools::Failed );
	UIACQUISITIONQT_API virtual void updating( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed );
	UIACQUISITIONQT_API virtual void info    ( ::std::ostream& stream );
	

	
private slots:
	void OnReconstructionCheck(QListWidgetItem *currentItem);
	void OnResetClick         ( );

private:

	void Refresh();
	void Notify();
	// tReconstructionMap
	typedef ::std::map< ::std::string, ::fwData::Reconstruction::sptr > tReconstructionMap;

	tReconstructionMap mReconstructionMap;
	::std::string      mTMSUid;
	QPointer< QPushButton > mpResetButton;
	QPointer< QListWidget > mpReconstructionListBox;

};

}


#endif
