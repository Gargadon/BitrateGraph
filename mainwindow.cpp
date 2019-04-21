#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QScatterSeries>
#include <QFile>
#include <QTemporaryFile>
#include <QProcess>
#include <QPixmap>

QString nombrearchivo;
QString jsonarchivo;
QString val;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->setWindowState(Qt::WindowMaximized);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_file_triggered()
{
    //Abrimos el archivo (por el momento abriremos directamente los JSON, mas
    //adelante será .mkv, .mp4, .ts, .m2ts y .avi
    QString filename = QFileDialog::getOpenFileName(this,tr("Select file"),
                                                        nombrearchivo,
                                                        tr("All files (*.*)"));
    nombrearchivo = filename;
    leerunjson();
}

void MainWindow::on_actionQuit_triggered()
{
    close();
}

void MainWindow::leerunjson()
{
    if(ui->allframes->isEnabled()==false)
    {
        ui->allframes->setEnabled(true);
        ui->ipbframes->setEnabled(true);
    }
    QString ffprobe = "ffprobe";
    QStringList argumentos;
    QTemporaryFile file;
    QString nombretemp;
    jsonarchivo = nombretemp;
    file.open();
    nombretemp = file.fileName();
    argumentos<<"-select_streams"<<"v"<<"-show_entries"<<"frame=pkt_size,pict_type"<<nombrearchivo<<"-of"<<"json";
    QProcess myProcess;
    myProcess.setStandardOutputFile(nombretemp);
    myProcess.start(ffprobe, argumentos);
    myProcess.waitForFinished(-1);
    QString val2 = file.readAll();
    file.close();
    val = val2;
    if(ui->ipbframes->isChecked())
    {
        showipb();
    }
    else
    {
        showall();
    }

    //Leemos el json
}

void MainWindow::on_ipbframes_clicked()
{
    showipb();
}

void MainWindow::on_allframes_clicked()
{
    showall();
}

void MainWindow::showipb()
{
    ui->bitrate->repaint();
    QJsonObject json2 = QJsonDocument::fromJson(val.toUtf8()).object();
    QJsonArray lista = json2["frames"].toArray();
    QLineSeries *seriesI = new QLineSeries();
    seriesI->setName("I frames");
    QLineSeries *seriesP = new QLineSeries();
    seriesP->setName("P frames");
    QLineSeries *seriesB = new QLineSeries();
    seriesB->setName("B frames");
    QLineSeries *eje = new QLineSeries();
    int final = lista.count();
    int IFrames=0,PFrames=0,BFrames=0;
    for(int i=0;i<final;i++)
    {
        //
        QJsonObject a = lista.at(i).toObject();
        QString e = a["pkt_size"].toString();
        int b = e.toInt() * 8 / 1000;
        QString c = a["pict_type"].toString();
        eje->append(i,0);
        if(c=="I")
        {
            seriesI->append(i,0);
            seriesI->append(i,b);
            seriesI->append(i,0);
            IFrames++;
        }
        else if (c=="P") {
            seriesP->append(i,0);
            seriesP->append(i,b);
            seriesP->append(i,0);
            PFrames++;
        }
        else {
            seriesB->append(i,0);
            seriesB->append(i,b);
            seriesB->append(i,0);
            BFrames++;
        }
    }
    QChart *chart = new QChart();
        chart->addSeries(seriesI);
        chart->addSeries(seriesP);
        chart->addSeries(seriesB);
        chart->addSeries(eje);
        chart->legend()->markers(eje)[0]->setVisible(false);
        chart->createDefaultAxes();
        QPen pen = seriesI->pen();
        pen.setWidth(1);
        pen.setBrush(QBrush("blue")); // or just pen.setColor("red");
        seriesI->setPen(pen);
        pen = seriesP->pen();
        pen.setWidth(1);
        pen.setBrush(QBrush("red")); // or just pen.setColor("red");
        seriesP->setPen(pen);
        pen = seriesB->pen();
        pen.setWidth(1);
        pen.setBrush(QBrush("green")); // or just pen.setColor("red");
        seriesB->setPen(pen);
        pen = seriesB->pen();
        pen.setWidth(1);
        pen.setBrush(QBrush("gray")); // or just pen.setColor("red");
        eje->setPen(pen);

    ui->bitrate->setChart(chart);
    ui->bitrate->setRenderHint(QPainter::Antialiasing);

    // Show information
    ui->frames->setText(QString::number(final));
    ui->IFrames->setText(QString::number(IFrames));
    ui->BFrames->setText(QString::number(BFrames));
    ui->PFrames->setText(QString::number(PFrames));
}


void MainWindow::showall()
{
    ui->bitrate->repaint();
    QJsonObject json2 = QJsonDocument::fromJson(val.toUtf8()).object();
    QJsonArray lista = json2["frames"].toArray();
    QLineSeries *seriestotal = new QLineSeries();
    seriestotal->setName("All frames");
    QLineSeries *eje = new QLineSeries();
    int final = lista.count();
    for(int i=0;i<final;i++)
    {
        //
        QJsonObject a = lista.at(i).toObject();
        QString e = a["pkt_size"].toString();
        int b = e.toInt() * 8 / 1000;
        QString c = a["pict_type"].toString();
        seriestotal->append(i,0);
        seriestotal->append(i,b);
        seriestotal->append(i,0);
        eje->append(i,0);
    }
    QChart *chart = new QChart();
        chart->addSeries(seriestotal);
        chart->addSeries(eje);
        chart->legend()->markers(eje)[0]->setVisible(false);
        chart->createDefaultAxes();
        QPen pen = seriestotal->pen();
        pen.setWidth(1);
        pen.setBrush(QBrush("blue")); // or just pen.setColor("red");
        seriestotal->setPen(pen);
        pen = seriestotal->pen();
                pen.setWidth(1);
                pen.setBrush(QBrush("gray")); // or just pen.setColor("red");
                eje->setPen(pen);

    ui->bitrate->setChart(chart);
    ui->bitrate->setRenderHint(QPainter::Antialiasing);

    // Show information
    ui->frames->setText(QString::number(final));
}

void MainWindow::leerunjsonaudio()
{
    QString ffprobe = "ffprobe";
    QStringList argumentos;
    QTemporaryFile file;
    QString nombretemp;
    jsonarchivo = nombretemp;
    file.open();
    nombretemp = file.fileName();
    argumentos<<"-select_streams"<<"a"<<"-show_entries"<<"packet=size"<<nombrearchivo<<"-of"<<"json";
    QProcess myProcess;
    myProcess.setStandardOutputFile(nombretemp);
    myProcess.start(ffprobe, argumentos);
    myProcess.waitForFinished(-1);
    QString val2 = file.readAll();
    file.close();
    val = val2;
    showallaudio();
    //Leemos el json
}

void MainWindow::showallaudio()
{
    ui->bitrate->repaint();
    QJsonObject json2 = QJsonDocument::fromJson(val.toUtf8()).object();
    QJsonArray lista = json2["packets"].toArray();
    QLineSeries *seriestotal = new QLineSeries();
    seriestotal->setName("All packets");
    QLineSeries *eje = new QLineSeries();
    int final = lista.count();
    for(int i=0;i<final;i++)
    {
        //
        QJsonObject a = lista.at(i).toObject();
        QString e = a["size"].toString();
        int b = e.toInt() * 8 / 1000;
        seriestotal->append(i,0);
        seriestotal->append(i,b);
        seriestotal->append(i,0);
        eje->append(i,0);
    }
    QChart *chart = new QChart();
        chart->addSeries(seriestotal);
        chart->addSeries(eje);
        chart->legend()->markers(eje)[0]->setVisible(false);
        chart->createDefaultAxes();
        QPen pen = seriestotal->pen();
        pen.setWidth(1);
        pen.setBrush(QBrush("blue")); // or just pen.setColor("red");
        seriestotal->setPen(pen);
        pen = seriestotal->pen();
                pen.setWidth(1);
                pen.setBrush(QBrush("gray")); // or just pen.setColor("red");
                eje->setPen(pen);

    ui->bitrate->setChart(chart);
    ui->bitrate->setRenderHint(QPainter::Antialiasing);

    // Show information
    ui->frames->setText(QString::number(final));
    ui->allframes->setEnabled(false);
    ui->ipbframes->setEnabled(false);
}

void MainWindow::on_actionOpen_audio_file_triggered()
{
    //Abrimos el archivo (por el momento abriremos directamente los JSON, mas
    //adelante será .mkv, .mp4, .ts, .m2ts y .avi
    QString filename = QFileDialog::getOpenFileName(this,tr("Select file"),
                                                        nombrearchivo,
                                                        tr("Audio files (*.aac *.ac3 *.mp3 *.mp4 *.m4a *.oga *.ogg *.flac *.ape *.tak *.alac)"));
    nombrearchivo = filename;
    leerunjsonaudio();
}

void MainWindow::on_actionExport_graph_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Save graph as"), "",
            tr("PNG (*.png)"));
    QPixmap p = ui->bitrate->grab();
    p.save(fileName);
    statusBar()->showMessage(tr("Exported."));
}
