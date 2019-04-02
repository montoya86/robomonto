//
// Created by jmontoya on 8/02/19.
//

#include <QScrollBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QAction>
#include <QtWidgets/QPlainTextEdit>
#include <robomongo/core/utils/QtUtils.h>
#include <QtWidgets/QHBoxLayout>
#include "HistoryWidget.h"
#include <robomongo/core/settings/SettingsManager.h>
#include <fstream>

HistoryWidget::HistoryWidget(QWidget *parent)
        : BaseClass(parent), _historyTextEdit(new QTextEdit(this)) {

    _historyTextEdit->setReadOnly(true);
    _historyTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    // VERIFY(connect(_historyTextEdit, SIGNAL(customContextMenuRequested(
    //                const QPoint&)), this, SLOT(showContextMenu(
    //               const QPoint &))));

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->addWidget(_historyTextEdit);
    _clear = new QAction("Clear All", this);
    //VERIFY(connect(_clear, SIGNAL(triggered()), _historyTextEdit, SLOT(clear())));
    setLayout(hlayout);

    readHistoryFile();
    historyFile.open(Robomongo::HistoryFilePath.toStdString(), std::ios_base::app | std::ios_base::out);
}

//HistoryWidget::~HistoryWidget() {
//    historyFile.close();
//}

void HistoryWidget::addMessage(const std::string &message) {
    addMessage(QString::fromStdString(message));
}

void HistoryWidget::addMessage(const QString &message) {

    // Print time
    _historyTextEdit->moveCursor(QTextCursor::End);
    _historyTextEdit->setTextColor(QColor("#aaaaaa"));

    // Print message
    _historyTextEdit->moveCursor(QTextCursor::End);

    QColor textColor = QColor(Qt::black);
    QString envelop = "\n == == == == == == == == == == == == == == == == == == == ==\n";
    _historyTextEdit->insertPlainText(message.trimmed() + envelop);
    historyFile << message.trimmed().toStdString();
    historyFile << envelop.toStdString();
    historyFile.flush();
    scrollToBottom();

}

void HistoryWidget::scrollToBottom() const {// Scroll to the bottom
    QScrollBar *sb = _historyTextEdit->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void HistoryWidget::readHistoryFile() {
    std::string line;
    std::ifstream myfile(Robomongo::HistoryFilePath.toStdString());
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            _historyTextEdit->insertPlainText(QString::fromStdString(line + "\n"));
        }
        myfile.close();
    }
    scrollToBottom();
}