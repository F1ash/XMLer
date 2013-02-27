#include "urldialog.h"
#include <QVBoxLayout>
#include <Qt>
#include <QApplication>
#include <QDesktopWidget>

UrlDialog::UrlDialog(QWidget* parent) : QDialog(parent)
{
	setModal(true);
	buttons = new QDialogButtonBox;
	buttons->setOrientation(Qt::Horizontal);
	ok = buttons->addButton(QString(tr("Ok")), QDialogButtonBox::ActionRole);
	cancel = buttons->addButton(QString(tr("Cancel")), QDialogButtonBox::ActionRole);
	connect(buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(editComplete(QAbstractButton*)));
	urlPath = new QLineEdit();
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(urlPath);
	layout->addWidget(buttons);
	layout->setAlignment (Qt::AlignCenter);
	this->setLayout(layout);
	QDesktopWidget *d = QApplication::desktop();
	this->setFixedWidth (2*d->width()/3);
	this->setFocusProxy(urlPath);
}

UrlDialog::~UrlDialog()
{
	this->deleteLater();
}

void UrlDialog::editComplete(QAbstractButton* button)
{
	disconnect(buttons, SIGNAL(clicked(QAbstractButton*)));
	if (button == ok) emit urlGrabbed(urlPath->text());
	this->done(0);
}
