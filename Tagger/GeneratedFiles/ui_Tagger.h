/********************************************************************************
** Form generated from reading UI file 'Tagger.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TAGGER_H
#define UI_TAGGER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TaggerClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TaggerClass)
    {
        if (TaggerClass->objectName().isEmpty())
            TaggerClass->setObjectName(QStringLiteral("TaggerClass"));
        TaggerClass->resize(600, 400);
        menuBar = new QMenuBar(TaggerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        TaggerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(TaggerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        TaggerClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(TaggerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        TaggerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(TaggerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        TaggerClass->setStatusBar(statusBar);

        retranslateUi(TaggerClass);

        QMetaObject::connectSlotsByName(TaggerClass);
    } // setupUi

    void retranslateUi(QMainWindow *TaggerClass)
    {
        TaggerClass->setWindowTitle(QApplication::translate("TaggerClass", "Tagger", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TaggerClass: public Ui_TaggerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TAGGER_H
