/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPSOFA_SOFAMESHEDITORSRV_HPP__
#define __OPSOFA_SOFAMESHEDITORSRV_HPP__

#include <QKeyEvent>
#include <QWidget>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

namespace fwData
{
class String;
}

namespace opSofa
{

/**
 * @brief Service allow to show panel editor sofa
 */
class SofaMeshEditorSrv : public QWidget,
                          public ::gui::editor::IEditor
{
Q_OBJECT

public:
    fwCoreServiceClassDefinitionsMacro ( (SofaMeshEditorSrv)(::gui::editor::IEditor) );

    SofaMeshEditorSrv() noexcept;
    virtual ~SofaMeshEditorSrv() noexcept;
    void configuring();
    void starting();
    void stopping();
    void updating();
    void receiving( ::fwServices::ObjectMsg::csptr msg );

protected Q_SLOTS:
    void onStrengthSlider(int value);
    void moveOrgan(QKeyEvent*);

private:
    CSPTR(::fwData::String) m_idReconstruction;
};

}

#endif // __OPSOFA_SOFAMESHEDITORSRV_HPP__

