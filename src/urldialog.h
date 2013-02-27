#include <QDialog>
#include <QAbstractButton>
#include <QString>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>
#include <QDialogButtonBox>

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
        QDialogButtonBox* buttons;

    signals:
        void urlGrabbed(QString url);

    private slots:
        void editComplete(QAbstractButton* button);
};
