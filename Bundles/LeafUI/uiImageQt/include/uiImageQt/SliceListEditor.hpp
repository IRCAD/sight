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
 * @brief   SliceListEditor service allows to change the number of slice to show.
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

    /**
     * @brief Install the layout.
     */
    virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping() throw(::fwTools::Failed);

    /// Management of observations : update editor according to the received message
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @verbatim
     <service uid="sliceListNegato3DEditor" type="::gui::editor::IEditor" implementation="::uiImage::SliceListEditor" autoComChannel="yes">
         <negatoAdaptor uid="myNegatoMPR" slices="1"/>
     </service>
       @endverbatim
        - \b uid is the uid of the ::visuVTKAdaptor::NegatoMPR service where the scan will be show/hide.
        - \b slices is the number of slide to show
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

protected slots:
    /// This method is called when the popup menu is clicked. Notify the slice mode changed.
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


