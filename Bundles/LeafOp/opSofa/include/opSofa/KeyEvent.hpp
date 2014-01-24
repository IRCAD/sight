/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QLabel>
#include <QKeyEvent>

class KeyEvent : public QLabel {
    Q_OBJECT

    public :
        KeyEvent();
        void keyPressEvent(QKeyEvent* event);
        void keyReleaseEvent(QKeyEvent* event);

    Q_SIGNALS:
        void keyPress(QKeyEvent*);
};
