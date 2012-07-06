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
