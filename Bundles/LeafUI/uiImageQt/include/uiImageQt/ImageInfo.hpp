/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEQT_IMAGEINFO_HPP
#define _UIIMAGEQT_IMAGEINFO_HPP


#include <QObject>
#include <QLineEdit>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <fwData/Point.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiImageQt/config.hpp"

namespace uiImage
{

/**
 * @brief   ImageInfo service allows to display image pixel information.
 * @class   ImageInfo.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIIMAGEQT_CLASS_API ImageInfo : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (ImageInfo)(::gui::editor::IEditor) ) ;


protected:

    /// Constructor. Do nothing.
    UIIMAGEQT_API ImageInfo() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEQT_API virtual ~ImageInfo() throw() ;

    /// Starts editor.
    virtual void starting() throw(::fwTools::Failed);

    /// Stops editor.
    virtual void stopping() throw(::fwTools::Failed);

    /// Receive InteractionMsg and updates text informations
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw( ::fwTools::Failed);

    virtual void info( std::ostream &_sstream ) ;


private:

    QPointer< QLineEdit >   m_valueText;

};

} // uiImage

#endif /*_UIIMAGEQT_IMAGEINFO_HPP*/
