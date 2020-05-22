/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <fwDataTools/PickingInfo.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>

namespace uiImageQt
{

/**
 * @brief   ImageInfo service allows to display image pixel information when it receives the mouse cursor coordinates.
 *
 * @warnig The interaction must be received in pixel coordinates (for example from the the adaptor
 *         ::visuVTKAdaptor::SImagePickerInteractor)
 *
 * @section Slots Slots
 * - \b getInteraction(::fwDataTools::PickingInfo): display image pixel information
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="imageInfo" type="::uiImageQt::ImageInfo" autoConnect="yes">
        <in key="image" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b image [::fwData::Image]: image used to retrieve the pixel value.
 */
class UIIMAGEQT_CLASS_API ImageInfo : public QObject,
                                      public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceMacro(ImageInfo, ::fwGui::editor::IEditor);

    UIIMAGEQT_API ImageInfo() noexcept;

    UIIMAGEQT_API virtual ~ImageInfo() noexcept;

protected:

    /// Starts editor.
    virtual void starting() override;

    /// Stops editor.
    virtual void stopping() override;

    /// Check if the image is valid, if not the editor is disabled
    virtual void updating() override;

    /// Initializes the editor
    virtual void configuring() override;

    virtual void info( std::ostream& _sstream ) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

private:

    /// Slot: get the picking information to display the pixel value
    void getInteraction(::fwDataTools::PickingInfo info);

    /// Label to display the pixel value
    QPointer< QLineEdit >   m_valueText;

};

} // uiImageQt
