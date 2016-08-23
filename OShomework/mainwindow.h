#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "process.h"
#include <QList>
#include <QDebug>
#include <QTimer>
#include <QtCore>
#include <vector>
#include <iostream>
#include <string>

#define STATE_READY   		1
#define STATE_RUNNING		2
#define STATE_WAITING		3
#define STATE_TERMINATED	4

#define SCHEDULE_FCFS 			0
#define SCHEDULE_NP_SJF			1
#define SCHEDULE_NP_PRIORITY    2
#define SCHEDULE_RR			    3


namespace Ui {
class MainWindow;
}

class PCB;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

     QTimer* timer;

     void ttl_countdown();//倒计时

     /*-----------调度函数-----------*/
     void schedule_fcfs();//#part3#
     void schedule_np_sjf();//#part3#
     void schedule_np_priority();//#part3#
     void schedule_rr();//#part3#
     void schedule_io_fcfs();//#part2#
     void schedule_io_np_sjf();//#part2#
     void schedule_io_np_priority();//#part2#
     void schedule_io_rr();//#part2#
     void schedule(int strategy, int strategy_io);//#part2# //#part3#
     void schedule_dump();

    /*--------显示刷新函数-----------*/
    void show_ready_queque();
    void show_waiting_queque();
    void show_memory_block();//#part1#
    void show_process_sum();
    void show_cpu();
    void show_io();
    void show_cur_dir();//#part4#//#part5#

     /*--------文件管理-----------*/
    void file_manage();
    void file_use_process(int file_process_num);
private slots:

    void on_lineEdit_process_delete_editingFinished();

    void on_pushButton_delete_prcess_clicked();

    void on_comboBox_currentIndexChanged(int index_num);

    void on_comboBox_io_currentIndexChanged(int index_num);

    void on_pushButton_clicked();

    void on_pushButton_create_process_CPU_clicked();

    void on_pushButton_create_process_IO_clicked();

    void on_pushButton_create_process_p_clicked();

    void on_pushButton_file_create_clicked();//#part4#

    void on_pushButton_file_delete_clicked();//#part4#

    void on_pushButton_back_clicked();//#part5#

    void on_pushButton_file_open_clicked();//#part5#

    void on_pushButton_file_close_clicked();//#part5#

    void on_pushButton_file_look_clicked(); //#part6#

    void on_pushButton_file_write_clicked(); //#part6#

    void on_lineEdit_priority_editingFinished();

    void on_radioButton_on_clicked();

    void on_radioButton_off_clicked();

    void on_radioButton_fixed_clicked();

    void on_radioButton_random_clicked();

public slots:

    void work_routine();//#part4#

    void on_pushButton_create_process_clicked();

public:
    int file_count;
    std::vector<std::string> file_list;


private:
    Ui::MainWindow *ui;
};



#endif // MAINWINDOW_H
