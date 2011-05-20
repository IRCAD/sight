/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEQT_IMAGETRANSPARENCY_HPP
#define _UIIMAGEQT_IMAGETRANSPARENCY_HPP


#include <QObject>
#include <QSlider>
#include <QCheckBox>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <fwData/Point.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiImageQt/config.hpp"

namespace uiImage
{

/**
 * @brief   ImageTransparency service allows to change image transparency.
 * @class   ImageTransparency.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class UIIMAGEQT_CLASS_API ImageTransparency : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (ImageTransparency)(::gui::editor::IEditor) ) ;


protected:

    /// Constructor. Do nothing.
    UIIMAGEQT_API ImageTransparency() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~ImageTransparency() throw() ;

    /// Starts editor.
    virtual void starting() throw(::fwTools::Failed);

    /// Stops editor.
    virtual void stopping() throw(::fwTools::Failed);

    /// Receive InteractionMsg and updates text informations
    virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw( ::fwTools::Failed);

    virtual void info( std::ostream &_sstream ) ;

protected slots:

    /**
     * @brief This method is called when the value change.
     */
    void onModifyVisibility(int value);
    void onModifyTransparency(int value);

private:

    QPointer< QSlider >   m_valueSlider;
    QPointer< QCheckBox > m_valueCheckBox;
    std::string m_shortcut;

};

} // uiImage

#endif /*_UIIMAGEQT_IMAGETRANSPARENCY_HPP*/
