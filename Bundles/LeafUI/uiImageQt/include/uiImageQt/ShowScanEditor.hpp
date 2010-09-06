/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEQT_SHOW_SCAN_EDITOR_HPP_
#define _UIIMAGEQT_SHOW_SCAN_EDITOR_HPP_

#include <QObject>
#include <QPointer>
#include <QIcon>

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiImageQt/config.hpp"

class QPushButton;

namespace uiImage
{

/**
 * @brief   This editor service represent a button to show/hide scan in a generic scene.
 * @class   ShowScanEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIIMAGEQT_CLASS_API ShowScanEditor : public QObject, public ::gui::editor::IEditor
{

    Q_OBJECT
public :

    fwCoreServiceClassDefinitionsMacro ( (ShowScanEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGEQT_API ShowScanEditor() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~ShowScanEditor() throw() ;

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

    /// Do nothing
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
     <service uid="showScanNegato3DEditor" type="::gui::editor::IEditor" implementation="::uiImage::ShowScanEditor" autoComChannel="no">
         <negatoAdaptor uid="myNegatoMPR" />
     </service>
       @endverbatim
       \b myNegatoMPR is the uid of the ::visuVTKAdaptor::NegatoMPR service where the scan will be show/hide.
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

protected slots:
    /**
     * @brief This method is called when the scan button is clicked.
     *
     * This service notifies the modification.
     */
    void onChangeScanMode();

private:

    std::string m_adaptorUID;

    QIcon m_imageShowScan;
    QIcon m_imageHideScan;
    bool m_scanAreShown;
    QPointer< QPushButton > m_showScanButton;

};

} // uiImage

#endif /*_UIIMAGEQT_SHOW_SCAN_EDITOR_HPP_*/


