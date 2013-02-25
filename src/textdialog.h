#include <QDialog>
#include <QAbstractButton>
#include <QString>
#include <QTextEdit>
#include <QWidget>
#include <QPushButton>
#include <QModelIndex>

class TextDialog : public QDialog
{
    Q_OBJECT;
    public:
        explicit TextDialog(QWidget* parent=0);
        ~TextDialog();
        void setCurrentIndex(const QModelIndex & index);
    private:
        QTextEdit* TextData;
        QPushButton* ok;
        QPushButton* cancel;
        const QModelIndex* currentIndex;

    private slots:
        void editComplete(QAbstractButton* button);
};
