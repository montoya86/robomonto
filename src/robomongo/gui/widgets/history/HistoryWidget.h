//
// Created by jmontoya on 8/02/19.
//

#ifndef ROBOMONGO_HISTORYWIDGET_H
#define ROBOMONGO_HISTORYWIDGET_H

#include <QWidget>
#include <fstream>

QT_BEGIN_NAMESPACE
class QTextEdit;

class QAction;

QT_END_NAMESPACE

class HistoryWidget : public QWidget {

Q_OBJECT

public:
    typedef QWidget BaseClass;

    HistoryWidget(QWidget *parent = 0);

public Q_SLOTS:

    void addMessage(const QString &message);

    void addMessage(const std::string &message);

private:
    std::ofstream historyFile;
    QTextEdit *const _historyTextEdit;
    QAction *_clear;

    void readHistoryFile();

    void scrollToBottom() const;
};


#endif //ROBOMONGO_HISTORYWIDGET_H
