/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QPushButton *pushButton_create_process;
    QPushButton *pushButton_delete_prcess;
    QTextBrowser *textBrowser_ready_queue;
    QTextBrowser *textBrowser_waiting_queue;
    QLabel *label_2;
    QLabel *label_3;
    QFrame *line;
    QFrame *line_3;
    QLineEdit *lineEdit_process_delete;
    QTextBrowser *textBrowser_CPU;
    QLabel *label_8;
    QLabel *label_9;
    QTextBrowser *textBrowser_IO;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QLabel *label_6;
    QPushButton *pushButton_create_process_CPU;
    QPushButton *pushButton_create_process_IO;
    QPushButton *pushButton_create_process_p;
    QLineEdit *lineEdit_priority;
    QLabel *label_4;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QRadioButton *radioButton_on;
    QRadioButton *radioButton_off;
    QLabel *label_12;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_3;
    QRadioButton *radioButton_random;
    QRadioButton *radioButton_fixed;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_cputime;
    QLabel *label_iotime;
    QTextBrowser *textBrowser_file;
    QLabel *label_15;
    QLineEdit *lineEdut_file_display;
    QPushButton *pushButton_file_create;
    QPushButton *pushButton_file_write;
    QPushButton *pushButton_file_delete;
    QPushButton *pushButton_back;
    QPushButton *pushButton_file_open;
    QTextBrowser *textBrowser_file_info;
    QPushButton *pushButton_file_look;
    QFrame *line_4;
    QLabel *label_16;
    QPushButton *pushButton_file_close;
    QLabel *label_5;
    QFrame *line_6;
    QLabel *label_10;
    QLabel *label_p_sum;
    QLabel *label_7;
    QFrame *line_7;
    QFrame *line_8;
    QLabel *label_18;
    QTextBrowser *textBrowser_memory;
    QMenuBar *menuBar;
    QMenu *menuOS;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(829, 536);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        pushButton_create_process = new QPushButton(centralWidget);
        pushButton_create_process->setObjectName(QStringLiteral("pushButton_create_process"));
        pushButton_create_process->setGeometry(QRect(23, 50, 101, 23));
        QFont font;
        font.setPointSize(12);
        pushButton_create_process->setFont(font);
        pushButton_delete_prcess = new QPushButton(centralWidget);
        pushButton_delete_prcess->setObjectName(QStringLiteral("pushButton_delete_prcess"));
        pushButton_delete_prcess->setGeometry(QRect(150, 80, 101, 23));
        pushButton_delete_prcess->setFont(font);
        textBrowser_ready_queue = new QTextBrowser(centralWidget);
        textBrowser_ready_queue->setObjectName(QStringLiteral("textBrowser_ready_queue"));
        textBrowser_ready_queue->setGeometry(QRect(300, 55, 241, 111));
        textBrowser_waiting_queue = new QTextBrowser(centralWidget);
        textBrowser_waiting_queue->setObjectName(QStringLiteral("textBrowser_waiting_queue"));
        textBrowser_waiting_queue->setGeometry(QRect(570, 55, 241, 111));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(300, 30, 54, 12));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(570, 30, 54, 12));
        line = new QFrame(centralWidget);
        line->setObjectName(QStringLiteral("line"));
        line->setGeometry(QRect(190, 0, 601, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_3 = new QFrame(centralWidget);
        line_3->setObjectName(QStringLiteral("line_3"));
        line_3->setGeometry(QRect(10, 0, 181, 20));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        lineEdit_process_delete = new QLineEdit(centralWidget);
        lineEdit_process_delete->setObjectName(QStringLiteral("lineEdit_process_delete"));
        lineEdit_process_delete->setGeometry(QRect(170, 54, 61, 20));
        lineEdit_process_delete->setFont(font);
        textBrowser_CPU = new QTextBrowser(centralWidget);
        textBrowser_CPU->setObjectName(QStringLiteral("textBrowser_CPU"));
        textBrowser_CPU->setGeometry(QRect(300, 195, 241, 31));
        label_8 = new QLabel(centralWidget);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(300, 175, 81, 20));
        label_9 = new QLabel(centralWidget);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setGeometry(QRect(570, 175, 81, 20));
        textBrowser_IO = new QTextBrowser(centralWidget);
        textBrowser_IO->setObjectName(QStringLiteral("textBrowser_IO"));
        textBrowser_IO->setGeometry(QRect(570, 195, 241, 31));
        comboBox = new QComboBox(centralWidget);
        comboBox->setObjectName(QStringLiteral("comboBox"));
        comboBox->setGeometry(QRect(440, 25, 101, 21));
        comboBox->setFont(font);
        comboBox->setIconSize(QSize(16, 16));
        comboBox->setFrame(false);
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(150, 110, 101, 23));
        pushButton->setFont(font);
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(45, 30, 54, 12));
        pushButton_create_process_CPU = new QPushButton(centralWidget);
        pushButton_create_process_CPU->setObjectName(QStringLiteral("pushButton_create_process_CPU"));
        pushButton_create_process_CPU->setGeometry(QRect(23, 110, 101, 23));
        pushButton_create_process_CPU->setFont(font);
        pushButton_create_process_IO = new QPushButton(centralWidget);
        pushButton_create_process_IO->setObjectName(QStringLiteral("pushButton_create_process_IO"));
        pushButton_create_process_IO->setGeometry(QRect(23, 80, 101, 23));
        pushButton_create_process_IO->setFont(font);
        pushButton_create_process_p = new QPushButton(centralWidget);
        pushButton_create_process_p->setObjectName(QStringLiteral("pushButton_create_process_p"));
        pushButton_create_process_p->setGeometry(QRect(23, 160, 101, 23));
        pushButton_create_process_p->setFont(font);
        lineEdit_priority = new QLineEdit(centralWidget);
        lineEdit_priority->setObjectName(QStringLiteral("lineEdit_priority"));
        lineEdit_priority->setGeometry(QRect(30, 140, 91, 20));
        lineEdit_priority->setFont(font);
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(30, 200, 101, 16));
        horizontalLayoutWidget_2 = new QWidget(centralWidget);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(10, 220, 131, 31));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        radioButton_on = new QRadioButton(horizontalLayoutWidget_2);
        radioButton_on->setObjectName(QStringLiteral("radioButton_on"));
        radioButton_on->setMinimumSize(QSize(4, 5));
        radioButton_on->setBaseSize(QSize(0, 0));
        radioButton_on->setFont(font);

        horizontalLayout_2->addWidget(radioButton_on);

        radioButton_off = new QRadioButton(horizontalLayoutWidget_2);
        radioButton_off->setObjectName(QStringLiteral("radioButton_off"));
        radioButton_off->setEnabled(true);
        radioButton_off->setFont(font);

        horizontalLayout_2->addWidget(radioButton_off);

        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(170, 200, 101, 16));
        QFont font1;
        font1.setPointSize(13);
        label_12->setFont(font1);
        horizontalLayoutWidget_3 = new QWidget(centralWidget);
        horizontalLayoutWidget_3->setObjectName(QStringLiteral("horizontalLayoutWidget_3"));
        horizontalLayoutWidget_3->setGeometry(QRect(147, 220, 131, 31));
        horizontalLayoutWidget_3->setFont(font);
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        radioButton_random = new QRadioButton(horizontalLayoutWidget_3);
        radioButton_random->setObjectName(QStringLiteral("radioButton_random"));
        radioButton_random->setFont(font);

        horizontalLayout_3->addWidget(radioButton_random);

        radioButton_fixed = new QRadioButton(horizontalLayoutWidget_3);
        radioButton_fixed->setObjectName(QStringLiteral("radioButton_fixed"));
        radioButton_fixed->setFont(font);

        horizontalLayout_3->addWidget(radioButton_fixed);

        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setGeometry(QRect(390, 175, 111, 16));
        label_14 = new QLabel(centralWidget);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(670, 175, 111, 16));
        label_cputime = new QLabel(centralWidget);
        label_cputime->setObjectName(QStringLiteral("label_cputime"));
        label_cputime->setGeometry(QRect(510, 175, 54, 16));
        label_iotime = new QLabel(centralWidget);
        label_iotime->setObjectName(QStringLiteral("label_iotime"));
        label_iotime->setGeometry(QRect(780, 175, 54, 16));
        textBrowser_file = new QTextBrowser(centralWidget);
        textBrowser_file->setObjectName(QStringLiteral("textBrowser_file"));
        textBrowser_file->setGeometry(QRect(300, 300, 201, 211));
        label_15 = new QLabel(centralWidget);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(300, 280, 54, 12));
        lineEdut_file_display = new QLineEdit(centralWidget);
        lineEdut_file_display->setObjectName(QStringLiteral("lineEdut_file_display"));
        lineEdut_file_display->setGeometry(QRect(647, 380, 171, 31));
        pushButton_file_create = new QPushButton(centralWidget);
        pushButton_file_create->setObjectName(QStringLiteral("pushButton_file_create"));
        pushButton_file_create->setGeometry(QRect(640, 430, 58, 23));
        pushButton_file_create->setFont(font);
        pushButton_file_write = new QPushButton(centralWidget);
        pushButton_file_write->setObjectName(QStringLiteral("pushButton_file_write"));
        pushButton_file_write->setGeometry(QRect(700, 430, 58, 23));
        pushButton_file_write->setFont(font);
        pushButton_file_delete = new QPushButton(centralWidget);
        pushButton_file_delete->setObjectName(QStringLiteral("pushButton_file_delete"));
        pushButton_file_delete->setGeometry(QRect(760, 430, 58, 23));
        pushButton_file_delete->setFont(font);
        pushButton_back = new QPushButton(centralWidget);
        pushButton_back->setObjectName(QStringLiteral("pushButton_back"));
        pushButton_back->setGeometry(QRect(640, 300, 58, 23));
        pushButton_back->setFont(font);
        pushButton_file_open = new QPushButton(centralWidget);
        pushButton_file_open->setObjectName(QStringLiteral("pushButton_file_open"));
        pushButton_file_open->setGeometry(QRect(640, 340, 58, 23));
        pushButton_file_open->setFont(font);
        textBrowser_file_info = new QTextBrowser(centralWidget);
        textBrowser_file_info->setObjectName(QStringLiteral("textBrowser_file_info"));
        textBrowser_file_info->setGeometry(QRect(520, 300, 111, 211));
        pushButton_file_look = new QPushButton(centralWidget);
        pushButton_file_look->setObjectName(QStringLiteral("pushButton_file_look"));
        pushButton_file_look->setGeometry(QRect(700, 340, 58, 23));
        pushButton_file_look->setFont(font);
        line_4 = new QFrame(centralWidget);
        line_4->setObjectName(QStringLiteral("line_4"));
        line_4->setGeometry(QRect(10, 255, 801, 20));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        label_16 = new QLabel(centralWidget);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(520, 280, 54, 12));
        pushButton_file_close = new QPushButton(centralWidget);
        pushButton_file_close->setObjectName(QStringLiteral("pushButton_file_close"));
        pushButton_file_close->setGeometry(QRect(760, 340, 58, 23));
        pushButton_file_close->setFont(font);
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(410, 30, 32, 12));
        line_6 = new QFrame(centralWidget);
        line_6->setObjectName(QStringLiteral("line_6"));
        line_6->setGeometry(QRect(270, 10, 20, 251));
        line_6->setFont(font);
        line_6->setFrameShape(QFrame::VLine);
        line_6->setFrameShadow(QFrame::Sunken);
        label_10 = new QLabel(centralWidget);
        label_10->setObjectName(QStringLiteral("label_10"));
        label_10->setGeometry(QRect(300, 230, 81, 20));
        QFont font2;
        font2.setPointSize(10);
        label_10->setFont(font2);
        label_p_sum = new QLabel(centralWidget);
        label_p_sum->setObjectName(QStringLiteral("label_p_sum"));
        label_p_sum->setGeometry(QRect(370, 205, 54, 71));
        label_p_sum->setFont(font2);
        label_7 = new QLabel(centralWidget);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(170, 30, 54, 12));
        line_7 = new QFrame(centralWidget);
        line_7->setObjectName(QStringLiteral("line_7"));
        line_7->setGeometry(QRect(130, 30, 20, 161));
        line_7->setFont(font);
        line_7->setFrameShape(QFrame::VLine);
        line_7->setFrameShadow(QFrame::Sunken);
        line_8 = new QFrame(centralWidget);
        line_8->setObjectName(QStringLiteral("line_8"));
        line_8->setGeometry(QRect(270, 260, 20, 251));
        line_8->setFont(font);
        line_8->setFrameShape(QFrame::VLine);
        line_8->setFrameShadow(QFrame::Sunken);
        label_18 = new QLabel(centralWidget);
        label_18->setObjectName(QStringLiteral("label_18"));
        label_18->setGeometry(QRect(20, 280, 54, 12));
        textBrowser_memory = new QTextBrowser(centralWidget);
        textBrowser_memory->setObjectName(QStringLiteral("textBrowser_memory"));
        textBrowser_memory->setGeometry(QRect(20, 300, 241, 211));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 829, 22));
        menuOS = new QMenu(menuBar);
        menuOS->setObjectName(QStringLiteral("menuOS"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuOS->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        pushButton_create_process->setText(QApplication::translate("MainWindow", "\351\232\217\346\234\272", 0));
        pushButton_delete_prcess->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\350\277\233\347\250\213", 0));
        label_2->setText(QApplication::translate("MainWindow", "CPU\351\230\237\345\210\227", 0));
        label_3->setText(QApplication::translate("MainWindow", "IO\351\230\237\345\210\227", 0));
        lineEdit_process_delete->setText(QString());
        label_8->setText(QApplication::translate("MainWindow", "CPU\345\215\240\347\224\250", 0));
        label_9->setText(QApplication::translate("MainWindow", "IO\345\215\240\347\224\250", 0));
        comboBox->clear();
        comboBox->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "FCFS", 0)
         << QApplication::translate("MainWindow", "NP_SJF", 0)
         << QApplication::translate("MainWindow", "NP_PRIORITY", 0)
         << QApplication::translate("MainWindow", "RR", 0)
        );
        pushButton->setText(QApplication::translate("MainWindow", "\346\270\205\351\231\244\346\211\200\346\234\211\350\277\233\347\250\213", 0));
        label_6->setText(QApplication::translate("MainWindow", "\345\210\233\345\273\272\350\277\233\347\250\213", 0));
        pushButton_create_process_CPU->setText(QApplication::translate("MainWindow", "CPU\344\270\272\344\270\273", 0));
        pushButton_create_process_IO->setText(QApplication::translate("MainWindow", "IO\344\270\272\344\270\273", 0));
        pushButton_create_process_p->setText(QApplication::translate("MainWindow", "\346\214\207\345\256\232\344\274\230\345\205\210\347\272\247", 0));
        lineEdit_priority->setText(QString());
        label_4->setText(QApplication::translate("MainWindow", "\350\207\252\345\212\250\345\210\233\345\273\272\345\255\220\350\277\233\347\250\213", 0));
        radioButton_on->setText(QApplication::translate("MainWindow", "\345\274\200\345\220\257", 0));
        radioButton_off->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255", 0));
        label_12->setText(QApplication::translate("MainWindow", "\350\277\233\347\250\213\346\227\266\351\227\264\347\211\207", 0));
        radioButton_random->setText(QApplication::translate("MainWindow", "\351\232\217\346\234\272", 0));
        radioButton_fixed->setText(QApplication::translate("MainWindow", "\345\233\272\345\256\232", 0));
        label_13->setText(QApplication::translate("MainWindow", "CPU\347\264\257\350\256\241\350\277\220\350\241\214\346\227\266\351\227\264\357\274\232", 0));
        label_14->setText(QApplication::translate("MainWindow", "IO\347\264\257\350\256\241\350\277\220\350\241\214\346\227\266\351\227\264\357\274\232", 0));
        label_cputime->setText(QApplication::translate("MainWindow", "0", 0));
        label_iotime->setText(QApplication::translate("MainWindow", "0", 0));
        label_15->setText(QApplication::translate("MainWindow", "\346\226\207\344\273\266\347\263\273\347\273\237", 0));
        pushButton_file_create->setText(QApplication::translate("MainWindow", "\345\210\233\345\273\272", 0));
        pushButton_file_write->setText(QApplication::translate("MainWindow", "\344\277\256\346\224\271", 0));
        pushButton_file_delete->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244", 0));
        pushButton_back->setText(QApplication::translate("MainWindow", "\345\220\216\351\200\200", 0));
        pushButton_file_open->setText(QApplication::translate("MainWindow", "\346\211\223\345\274\200", 0));
        pushButton_file_look->setText(QApplication::translate("MainWindow", "\346\237\245\347\234\213", 0));
        label_16->setText(QApplication::translate("MainWindow", "\350\257\246\347\273\206\344\277\241\346\201\257", 0));
        pushButton_file_close->setText(QApplication::translate("MainWindow", "\345\205\263\351\227\255", 0));
        label_5->setText(QApplication::translate("MainWindow", "\347\255\226\347\225\245", 0));
        label_10->setText(QApplication::translate("MainWindow", "\345\275\223\345\211\215\346\200\273\350\277\233\347\250\213\346\225\260\357\274\232", 0));
        label_p_sum->setText(QApplication::translate("MainWindow", "0", 0));
        label_7->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244\350\277\233\347\250\213", 0));
        label_18->setText(QApplication::translate("MainWindow", "\345\206\205\345\255\230\347\256\241\347\220\206", 0));
        menuOS->setTitle(QApplication::translate("MainWindow", "OS", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
