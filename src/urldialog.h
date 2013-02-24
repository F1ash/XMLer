#include <QDialog>
#include <QAbstractButton>
#include <QString>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>

class UrlDialog : public QDialog
{
    Q_OBJECT;
    public:
        explicit UrlDialog(QWidget* parent=0);
        ~UrlDialog();
        QString getUrl();
    private:
        QLineEdit * urlPath;
        QPushButton* ok;
        QPushButton* cancel;

    private slots:
        void editComplete(QAbstractButton* button);
};
