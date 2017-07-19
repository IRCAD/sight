/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITOOLS_EDITOR_SSHOWVECTORSIZE_HPP__
#define __UITOOLS_EDITOR_SSHOWVECTORSIZE_HPP__

#include "uiTools/config.hpp"

#include <fwData/Vector.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QPointer>
#include <QLabel>

class QPushButton;

namespace uiTools
{

namespace editor
{

/**
 * @brief   SShowVectorSize service show the size of a fwData::Vector
 * @class   SShowVectorSize
 */
class UITOOLS_CLASS_API SShowVectorSize : public QObject,
                                          public ::gui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SShowVectorSize)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UITOOLS_API SShowVectorSize() noexcept;

    /// Destructor. Do nothing.
    UITOOLS_API virtual ~SShowVectorSize() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Vector::s_ADDED_OBJECTS_SIG to this::s_ADD_OBJECTS_SLOT
     * Connect Vector::s_REMOVED_OBJECTS_SIG to this::s_REMOVE_OBJECTS_SLOT
     */
    UITOOLS_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     */
    virtual void starting();

    /**
     * @brief Destroy the layout.
     */
    virtual void stopping();

    /// Does nothing
    virtual void updating();

    /// Does nothing
    virtual void swapping();

    /**
     * @brief Configure the service
     *
     * @code{.xml}
        <service uid="..." type="::gui::editor::IEditor" impl="::uiTools::editor::SShowVectorSize" autoConnect="yes">
            <text> my text </text>
        </service>
       @endcode
     * - \b text (optional, by default "") : the text to show before size of the vector
     * - \b
     */
    virtual void configuring();

    /// Overrides
    virtual void info( std::ostream &_sstream );


private:

    /// Slot: add objects
    void addObjects(::fwData::Vector::ContainerType objects);

    /// Slot: remove objects
    void removeObjects(::fwData::Vector::ContainerType objects);

    size_t m_vectorSize; ///< size of the vector
    QPointer< QLabel > m_label; ///< label where the text will be displayed
    QString m_textToShow; ///< text to show next to the size
};

} // namespace editor
} // namespace uiTools

#endif /*__UITOOLS_EDITOR_SSHOWVECTORSIZE_HPP__*/


