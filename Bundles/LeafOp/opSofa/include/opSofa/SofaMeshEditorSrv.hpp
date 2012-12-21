/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _opSofa_SOFAMESHEDITORSRV_HPP_
#define _opSofa_SOFAMESHEDITORSRV_HPP_

#include <QObject>
#include <QWidget>
#include <QKeyEvent>
#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>
#include <fwData/String.hpp>

namespace opSofa
{

/**
 * @brief Service allow to show panel editor sofa
 */
class SofaMeshEditorSrv : public QWidget, public ::gui::editor::IEditor
{
    Q_OBJECT

    public :
        fwCoreServiceClassDefinitionsMacro ( (SofaMeshEditorSrv)(::gui::editor::IEditor) ) ;

        SofaMeshEditorSrv() throw();
        virtual ~SofaMeshEditorSrv() throw();
        void configuring() throw ( ::fwTools::Failed );
        void starting() throw ( ::fwTools::Failed );
        void stopping() throw ( ::fwTools::Failed );
        void updating() throw ( ::fwTools::Failed );
        void receiving( ::fwServices::ObjectMsg::csptr msg ) throw ( ::fwTools::Failed );

    protected Q_SLOTS:
        void onStrengthSlider(int value);
        void moveOrgan(QKeyEvent*);

    private :
        ::fwData::String::csptr idReconstruction;
};

}

#endif // _opSofa_SOFAMESHEDITORSRV_HPP_
