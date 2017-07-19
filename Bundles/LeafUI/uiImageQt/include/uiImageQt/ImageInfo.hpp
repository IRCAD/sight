/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIMAGEQT_IMAGEINFO_HPP__
#define __UIIMAGEQT_IMAGEINFO_HPP__

#include "uiImageQt/config.hpp"

#include <fwData/Point.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>


namespace uiImage
{

/**
 * @brief   ImageInfo service allows to display image pixel information.
 */
class UIIMAGEQT_CLASS_API ImageInfo : public QObject,
                                      public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageInfo)(::gui::editor::IEditor) );

    UIIMAGEQT_API ImageInfo() noexcept;

    UIIMAGEQT_API virtual ~ImageInfo() noexcept;

protected:

    /// Starts editor.
    virtual void starting();

    /// Stops editor.
    virtual void stopping();

    virtual void updating();

    virtual void swapping();

    virtual void configuring();

    virtual void info( std::ostream& _sstream );


private:

    /// Slot: get the picking information
    void getInteraction(::fwDataTools::PickingInfo info);

    QPointer< QLineEdit >   m_valueText;

};

} // uiImage

#endif /*__UIIMAGEQT_IMAGEINFO_HPP__*/
