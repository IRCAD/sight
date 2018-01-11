/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __BEGINNERTRAINING_TUTO03_SSTRINGEDITOR_HPP__
#define __BEGINNERTRAINING_TUTO03_SSTRINGEDITOR_HPP__

#include "beginnerTraining/config.hpp"

#include <gui/editor/IEditor.hpp>

#include <QPointer>
#include <qtextedit.h>

namespace beginnerTraining
{
namespace tuto03
{

/**
 * @brief   Editor that display and modify the content of a ::fwData::String.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::beginnerTraining::tuto03::SStringEditor">
            <inout key="editString" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b string [::fwData::String]: string to display and to modify.
 */
class BEGINNERTRAINING_CLASS_API SStringEditor : public QObject,
                                                 public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SStringEditor)(::gui::editor::IEditor) );

    BEGINNERTRAINING_API SStringEditor();

    BEGINNERTRAINING_API virtual ~SStringEditor() noexcept;

protected Q_SLOTS:

    BEGINNERTRAINING_API void onTextChanged();

protected:

    /// Overrides
    BEGINNERTRAINING_API virtual void configuring() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void starting() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void stopping() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void updating() override;

    /// Method to notify modification on data
    BEGINNERTRAINING_API void notifyMessage();

private:
    /// Text editor manage by the service
    QPointer<QTextEdit> m_textEditor;
};

} // namespace tuto03
} // namespace beginnerTraining

#endif /*__BEGINNERTRAINING_TUTO03_SSTRINGEDITOR_HPP__*/

