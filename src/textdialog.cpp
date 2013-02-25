#include "textdialog.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QTextDocument>
#include <Qt>

TextDialog::TextDialog(QWidget* parent) : QDialog(parent)
{
	currentIndex = new QModelIndex();
	setModal(true);
	QDialogButtonBox* buttons = new QDialogButtonBox;
	buttons->setOrientation(Qt::Horizontal);
	ok = buttons->addButton(QString(tr("Ok")), QDialogButtonBox::ActionRole);
	cancel = buttons->addButton(QString(tr("Cancel")), QDialogButtonBox::ActionRole);
	connect(buttons, SIGNAL(clicked(QAbstractButton*)), this, SLOT(editComplete(QAbstractButton*)));
	TextData = new QTextEdit();
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(TextData);
	layout->addWidget(buttons);
	this->setLayout(layout);
	this->setFocusProxy(TextData);
}

void TextDialog::setCurrentIndex(const QModelIndex & index)
{
	currentIndex = &index;
	QString text = index.data().toString();
	QTextDocument* doc = new QTextDocument();
	doc->setPlainText(text);
	TextData->setDocument(doc);
}

TextDialog::~TextDialog()
{
	delete TextData;
	delete currentIndex;
	this->deleteLater();
}

void TextDialog::editComplete(QAbstractButton* button)
{
	//qDebug(button->text().toLocal8Bit().data());
	//qDebug(TextData->text().toLocal8Bit().data());
	//qDebug((this->parentWidget())->objectName().toLocal8Bit().data());
	if (button == ok)
		{
			QString text = TextData->toPlainText();
			qDebug()<<text;
			//currentIndex->model()->setData(&(*currentIndex), QVariant(text), currentIndex->row());
		};
	this->done(0);
}
