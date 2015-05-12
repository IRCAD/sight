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
 *
 * @date    2011.
 */
class UIIMAGEQT_CLASS_API ImageTransparency : public QObject,
                                              public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (ImageTransparency)(::gui::editor::IEditor) );

    UIIMAGEQT_API ImageTransparency() throw();

    UIIMAGEQT_API virtual ~ImageTransparency() throw();

protected:
    /// Starts editor.
    virtual void starting() throw(::fwTools::Failed);

    /// Stops editor.
    virtual void stopping() throw(::fwTools::Failed);

    /// Receive InteractionMsg and updates text informations
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw( ::fwTools::Failed);

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
