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
