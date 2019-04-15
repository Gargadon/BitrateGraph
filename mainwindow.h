#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_file_triggered();

    void on_actionQuit_triggered();

    void leerunjson();

    void on_ipbframes_clicked();

    void on_allframes_clicked();

    void showipb();

    void showall();

    void on_actionOpen_audio_file_triggered();

private:
    Ui::MainWindow *ui;
};

extern QString nombrearchivo;
extern QString jsonarchivo;
extern QString val;

#endif // MAINWINDOW_H
