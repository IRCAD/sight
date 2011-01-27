#include <QLabel>
#include <QKeyEvent>
	
class KeyEvent : public QLabel {
    Q_OBJECT

	public :
		KeyEvent();
		void keyPressEvent(QKeyEvent* event);
		void keyReleaseEvent(QKeyEvent* event);

    signals :
        void keyPress(QKeyEvent*);
};
