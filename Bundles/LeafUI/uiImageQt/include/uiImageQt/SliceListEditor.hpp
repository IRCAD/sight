/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEQT_SLICE_LIST_EDITOR_HPP_
#define _UIIMAGEQT_SLICE_LIST_EDITOR_HPP_

#include <QObject>
#include <QPointer>

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiImageQt/config.hpp"

class QMenu;
class QAction;
class QPushButton;


namespace uiImage
{

/**
 * @brief   SliceListEditor service.
 * @class   SliceListEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIIMAGEQT_CLASS_API SliceListEditor : public QObject, public ::gui::editor::IEditor
{

    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (SliceListEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGEQT_API SliceListEditor() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~SliceListEditor() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Managment of observations ( overides )
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

protected slots:
    void onChangeSliceMode(bool checked = false);

private:

    std::string m_adaptorUID;

    QPointer < QMenu > m_pDropDownMenu;
    QPointer < QPushButton > m_dropDownButton;
    QPointer < QAction > m_oneSliceItem;
    QPointer < QAction > m_threeSlicesItem;
    QPointer < QAction > m_obliqueSliceItem;
    int m_nbSlice;

};

} // uiImageQt

#endif /*_UIIMAGEQT_SLICE_LIST_EDITOR_HPP_*/


