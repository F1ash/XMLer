#include "urldialog.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <Qt>

UrlDialog::UrlDialog(QWidget* parent) : QDialog(parent)
{
	setModal(true);
	QDialogButtonBox* buttons = new QDialogButtonBox;
	buttons->setOrientation(Qt::Horizontal);
	ok = buttons->addButton(QString(tr("Ok")), QDialogButtonBox::ActionRole);
	cancel = buttons->addButton(QString(tr("Cancel")), QDialogButtonBox::ActionRole);
	connect(buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(editComplete(QAbstractButton*)));
	urlPath = new QLineEdit();
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(urlPath);
	layout->addWidget(buttons);
	this->setLayout(layout);
	this->setFocusProxy(urlPath);
}

UrlDialog::~UrlDialog()
{
	this->deleteLater();
}

void UrlDialog::editComplete(QAbstractButton* button)
{
	//qDebug(button->text().toLocal8Bit().data());
	//qDebug(urlPath->text().toLocal8Bit().data());
	//qDebug((this->parentWidget())->objectName().toLocal8Bit().data());
	if (button == ok) emit urlGrabbed(urlPath->text());
	this->done(0);
}
