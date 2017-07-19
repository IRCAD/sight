/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_EDITOR_DUMMYEDITOR_HPP__
#define __GUIQT_EDITOR_DUMMYEDITOR_HPP__

#include <QPointer>
#include <QLabel>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "guiQt/config.hpp"

namespace gui
{

namespace editor
{


/**
 * @brief   Defines the service interface managing the basic editor service for object. Do nothing.
 * @class   DummyEditor
 *
 * @todo ACH: This class has been created in order to build test application. Do we remove it now ??
 */
class GUIQT_CLASS_API DummyEditor : public ::gui::editor::IEditor
{

public:


    fwCoreServiceClassDefinitionsMacro ( (DummyEditor)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    GUIQT_API DummyEditor() noexcept;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~DummyEditor() noexcept;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */
    /**
     * @brief This method launches the IEditor::starting method.
     */
    GUIQT_API virtual void starting();

    /**
     * @brief This method launches the IEditor::stopping method.
     */
    GUIQT_API virtual void stopping();

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUIQT_API virtual void updating();

    /**
     * @brief This method is used to configure the class parameters. Do nothing.
     */
    GUIQT_API virtual void configuring();

    /**
     * @brief This method is used to give information about the service. Do nothing.
     */
    GUIQT_API virtual void info(std::ostream &_sstream );

    ///@}

private:
    /**
     * @brief optional text
     */
    std::string m_text;

    QPointer< QLabel > m_staticText;
};

}
}

#endif /*__GUIQT_EDITOR_DUMMYEDITOR_HPP__*/


