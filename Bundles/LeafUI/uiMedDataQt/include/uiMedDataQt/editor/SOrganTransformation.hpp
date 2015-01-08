/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIMEDDATAQT_EDITOR_SORGANTRANSFORMATION_HPP__
#define _UIMEDDATAQT_EDITOR_SORGANTRANSFORMATION_HPP__

#include <QPointer>
#include <QObject>

#include <map>
#include <string>

#include <fwData/Reconstruction.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiMedDataQt/config.hpp"


class QListWidget;
class QPushButton;
class QListWidgetItem;
class QComboBox;

namespace uiMedData
{
namespace editor
{

/**
* @brief Editor for applying transformation on a model (QT editor)
*/
class UIMEDDATAQT_CLASS_API SOrganTransformation : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro( (SOrganTransformation)(::gui::editor::IEditor) );

    UIMEDDATAQT_API              SOrganTransformation() throw();
    UIMEDDATAQT_API virtual     ~SOrganTransformation() throw();

protected:

    UIMEDDATAQT_API virtual void configuring() throw( ::fwTools::Failed );
    UIMEDDATAQT_API virtual void starting()    throw( ::fwTools::Failed );
    UIMEDDATAQT_API virtual void stopping()    throw( ::fwTools::Failed );
    UIMEDDATAQT_API virtual void swapping()    throw( ::fwTools::Failed );
    UIMEDDATAQT_API virtual void updating()    throw( ::fwTools::Failed );
    UIMEDDATAQT_API virtual void receiving( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed );
    UIMEDDATAQT_API virtual void info    ( ::std::ostream& stream );

private Q_SLOTS:
    void onReconstructionCheck(QListWidgetItem *currentItem);
    void onResetClick();
    void onSaveClick();
    void onLoadClick();

    //test
    void onTestClick();

private:

    void refresh();
    void notitfyTransformationMatrix(::fwData::TransformationMatrix3D::sptr aTransMat);

    // ReconstructionMapType
    typedef ::std::map< ::std::string, ::fwData::Reconstruction::sptr > ReconstructionMapType;
    typedef ::std::map< ::std::string, ::fwData::TransformationMatrix3D::sptr> InnerMatMappingType;
    typedef ::std::map< ::std::string, InnerMatMappingType> SaveMappingType;

    ReconstructionMapType m_reconstructionMap;
    ::std::string      m_TMSUid;
    QPointer< QPushButton > m_saveButton;
    QPointer< QPushButton > m_loadButton;
    QPointer< QPushButton > m_resetButton;
    QPointer< QListWidget > m_reconstructionListBox;
    QPointer< QComboBox > m_saveSelectionComboBox;

    //for testing
    QPointer< QPushButton > m_testButton;

    //variables for the functionalities of saving & loading
    SaveMappingType m_saveListing;
    unsigned int m_saveCount;
};

} // namespace editor
} // namespace uiMedData


#endif // _UIMEDDATAQT_EDITOR_SORGANTRANSFORMATION_HPP__
