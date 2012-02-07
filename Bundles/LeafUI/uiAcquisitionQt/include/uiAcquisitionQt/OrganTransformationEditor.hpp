#ifndef _UIACQUISITIONQT_ORGANTRANSFORMATIONEDITOR_HPP_
#define _UIACQUISITIONQT_ORGANTRANSFORMATIONEDITOR_HPP_

/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QPointer>
#include <QObject>

#include <map>
#include <string>

#include <fwData/Reconstruction.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiAcquisitionQt/config.hpp"


class QListWidget;
class QPushButton;
class QListWidgetItem;
class QComboBox;

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
    void OnSaveClick();
    void OnLoadClick();

    //test
    void OnTestClick();

private:

    void Refresh();
    void NotitfyTransformationMatrix(::fwData::TransformationMatrix3D::sptr aTransMat);

    // tReconstructionMap
    typedef ::std::map< ::std::string, ::fwData::Reconstruction::sptr > tReconstructionMap;
    typedef ::std::map< ::std::string, ::fwData::TransformationMatrix3D::sptr> tInnerMatMapping;
    typedef ::std::map< ::std::string, tInnerMatMapping> tSaveMapping;

    tReconstructionMap mReconstructionMap;
    ::std::string      mTMSUid;
    QPointer< QPushButton > mpSaveButton;
    QPointer< QPushButton > mpLoadButton;
    QPointer< QPushButton > mpResetButton;
    QPointer< QListWidget > mpReconstructionListBox;
    QPointer< QComboBox > mpSaveSelectionComboBox;

    //for testing
    QPointer< QPushButton > mpTestButton;

    //variables for the functionalities of saving & loading
    tSaveMapping mSaveListing;
    unsigned int mSaveCount;
};

}


#endif
