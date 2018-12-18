/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "uiImageQt/config.hpp"

#include <fwData/Point.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QAction>
#include <QCheckBox>
#include <QObject>
#include <QPointer>
#include <QSlider>

namespace uiImageQt
{

/**
 * @brief   ImageTransparency service allows to change image transparency.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiImageQt::ImageTransparency">
       <inout key="image" uid="..." />
       <shortcut value="Ctrl+U" />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to set the transparency.
 * @subsection Configuration Configuration
 * - \b shortcut : shortcut used to show/hide the image
 */
class UIIMAGEQT_CLASS_API ImageTransparency : public QObject,
                                              public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (ImageTransparency)(::fwGui::editor::IEditor) )

    UIIMAGEQT_API ImageTransparency() noexcept;

    UIIMAGEQT_API virtual ~ImageTransparency() noexcept;

protected:
    /// Starts editor.
    virtual void starting() override;

    /// Stops editor.
    virtual void stopping() override;

    virtual void updating() override;

    virtual void swapping() override;

    virtual void configuring() override;

    virtual void info( std::ostream& _sstream ) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_VISIBILITY_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_TRANSPARENCY_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

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

} // uiImageQt
