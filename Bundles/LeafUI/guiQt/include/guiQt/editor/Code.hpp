/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_EDITOR_CODE_HPP__
#define __GUIQT_EDITOR_CODE_HPP__

#include <QPointer>
#include <QObject>
#include <QTextEdit>
#include <QString>
#include <QSyntaxHighlighter>

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>


#include "guiQt/config.hpp"

namespace guiQt
{

namespace editor
{

/**
 * @class   Code
 * @brief   This service displays a code editor and works on a ::fwData::String.
 *
 * It provides highlighting for python and C++.
 *
 * XML Configuration
 *  @code{.xml}
    <service uid="codeEditor" type="::gui::editor::IEditor" impl="::guiQt::editor::Code" autoConnect="yes">
        <config>
            <language name="Python" />
        </config>
    </service>
    @endcode
 * - \b language name can be "Python" or "Cpp"
 */
class GUIQT_CLASS_API Code : public QObject,
                             public ::gui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceClassDefinitionsMacro ( (Code)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    GUIQT_API Code() noexcept;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~Code() noexcept;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Object::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    GUIQT_API virtual KeyConnectionsType getObjSrvConnections() const;

protected:


    /// Install the layout.
    virtual void starting();

    /// Destroy the layout
    virtual void stopping();

    /// Update the value from the String object.
    virtual void updating();

    /// Update the value from the String object.
    virtual void swapping();

    /// Parses the configuration
    virtual void configuring();

    /// Overrides
    virtual void info( std::ostream &_sstream );

protected:
    static const std::string PYTHON;
    static const std::string CPP;

protected Q_SLOTS:
    /**
     * @brief This method is called when the value change.
     *
     * Update the String object value and notify the modification.
     */
    void onModifyValue();

private:
    std::string m_language;
    QPointer< QTextEdit >          m_valueCtrl;
    QPointer< QSyntaxHighlighter > m_highlighter;
};

} // namespace editor

} // namespace guiQt

#endif /*__GUIQT_EDITOR_CODE_HPP__*/
