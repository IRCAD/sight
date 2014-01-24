/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opSofa/KeyEvent.hpp"

KeyEvent::KeyEvent() {
    this->setText("mouve mouse");
    this->setFocusPolicy(Qt::StrongFocus);
}

void KeyEvent::keyPressEvent(QKeyEvent* event) {
        Q_EMIT keyPress(event);
}

void KeyEvent::keyReleaseEvent(QKeyEvent* event) {
}
