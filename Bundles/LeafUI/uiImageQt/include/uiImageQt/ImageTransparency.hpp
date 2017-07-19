/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIIMAGEQT_IMAGETRANSPARENCY_HPP__
#define __UIIMAGEQT_IMAGETRANSPARENCY_HPP__


#include <QObject>
#include <QSlider>
#include <QCheckBox>
#include <QPointer>
#include <QAction>

#include <fwTools/Failed.hpp>

#include <fwData/Point.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiImageQt/config.hpp"

namespace uiImage
{

/**
 * @brief   ImageTransparency service allows to change image transparency.
 * @class   ImageTransparency
 */
class UIIMAGEQT_CLASS_API ImageTransparency : public QObject,
                                              public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageTransparency)(::gui::editor::IEditor) );

    UIIMAGEQT_API ImageTransparency() noexcept;

    UIIMAGEQT_API virtual ~ImageTransparency() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_TRANSPARENCY_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    UIIMAGEQT_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:
    /// Starts editor.
    virtual void starting();

    /// Stops editor.
    virtual void stopping();

    virtual void updating();

    virtual void swapping();

    virtual void configuring();

    virtual void info( std::ostream &_sstream );

protected Q_SLOTS:

    /**
     * @brief This method is called when the visibility value change using action shortcut.
     */
    void onModifyVisibility(bool value);

    /**
     * @brief This method is called when the visibility value change clicking on checkbox.
     */
    void onModifyVisibility(int value);

    /**
     * @brief This method is called when the transparency value change moving slider.
     */
    void onModifyTransparency(int value);

private:

    /// Notify VISIBILITY event on image
    void notifyVisibility(bool isVisible);

    QPointer< QSlider >   m_valueSlider;
    QPointer< QCheckBox > m_valueCheckBox;
    QPointer< QAction >   m_action;
    std::string m_shortcut;

};

} // uiImage

#endif /*__UIIMAGEQT_IMAGETRANSPARENCY_HPP__*/
