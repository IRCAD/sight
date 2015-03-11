/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UITOOLS_EDITOR_SSHOWVECTORSIZE_HPP_
#define _UITOOLS_EDITOR_SSHOWVECTORSIZE_HPP_

#include <QPointer>
#include <QLabel>
#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiTools/config.hpp"

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
    UITOOLS_API SShowVectorSize() throw();

    /// Destructor. Do nothing.
    UITOOLS_API virtual ~SShowVectorSize() throw();

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

    /// Does nothing
    virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /// Does nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Does nothing
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure the service
     *
     * @verbatim
        <service uid="..." type="::gui::editor::IEditor" impl="::uiTools::editor::SShowVectorSize" autoConnect="yes">
            <text> my text </text>
        </service>
       @endverbatim
     * - \b text (optional, by default "") : the text to show before size of the vector
     * - \b
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream );


private:

    unsigned int m_vectorSize; ///< size of the vector
    QPointer< QLabel > m_label; ///< label where the text will be displayed
    QString m_textToShow; ///< text to show next to the size
};

} // namespace editor
} // namespace uiTools

#endif /*_UITOOLS_EDITOR_SSHOWVECTORSIZE_HPP_*/


