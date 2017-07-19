/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUQT_POINTEDITOR_HPP__
#define __UIVISUQT_POINTEDITOR_HPP__



#include "uiVisuQt/config.hpp"

#include <fwData/Point.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>


namespace uiVisu
{

/**
 * @brief   PointEditor service allows to display point information.
 */
class UIVISUQT_CLASS_API PointEditor : public QObject,
                                       public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (PointEditor)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIVISUQT_API PointEditor() noexcept;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~PointEditor() noexcept;

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting();

    ///This method launches the IEditor::stopping method.
    virtual void stopping();

    void updating();

    void swapping();

    void configuring();

    /// Overrides
    virtual void info( std::ostream& _sstream );


private:

    /// Slot: get the interaction information
    void getInteraction(::fwDataTools::PickingInfo info);

    QPointer< QLineEdit >   m_textCtrl_x;
    QPointer< QLineEdit >   m_textCtrl_y;
    QPointer< QLineEdit >   m_textCtrl_z;

};

} // uiData

#endif /*__UIVISUQT_POINTEDITOR_HPP___*/
