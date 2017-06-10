#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "kernelinformation.h"
#include "filedownloader.h"
#include "qtermwidget.h"
#include <QProgressBar>
#include <QSpacerItem>
#include <QPushButton>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QProcess>
#include <QLabel>
#include <QMovie>
#include <QDir>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void kernelDownloaded();
    void kernelVersionClicked();
    void on_actionExit_triggered();
    void on_pushButton_2_clicked();
    void on_nextButton_clicked();
    void on_backButton_clicked();
    void on_actionSave_Configure_triggered();
    void on_actionSave_Configure_As_triggered();
    void on_actionOpen_Configure_File_2_triggered();
    void on_actionOpen_Downloaded_Kernel_triggered();

private:
    enum class Clear : int {UP = -1, NONE = 0, DOWN = 1};
    enum class FuncValue : int {DISCONNECT, CONNECT, SCROLLWIDGET ,TABWIDGET, INFOWIDGET};
    enum class Status : int {INFORMATION, CHOOSE, DOWNLOAD, CONFIGURE, COMPILE, INSTALL};

    //Private variables-------------------------------------------
    Ui::MainWindow *ui;
    QTermWidget *console = nullptr;

    //int doneCounter;
    int currentStatus;
    bool successDownload;
    bool saveStatus;

    //QString
    QString configFilePath;
    const QString workDirectory = "/home/adam/Asztal/work/";
    const QString kernelDatas = "kernelDatas";
    const QString kernelVersions = "kernelVersions";

    //My Class Pointer
    FileDownloader *fileDownloader = nullptr;
    KernelInformation *information = nullptr;

    //QPalette
    QPalette *palette = nullptr;

    //QMovie
    QList<QMovie*> movies;
    //QMovie *movie = nullptr;
    //QMovie *downMovie = nullptr;
    //QMovie *downMovie2 = nullptr;

    //QPushButton
    QPushButton* oldActiveButton = nullptr;

    //QList
    QList<QLineEdit*> lineEditVector;
    QList<QLabel*> pictureLabelVector;
    QList<QProgressBar*> progressBarVector;
    //QList<QString> kernelLinks;
    QList<QString> kernelNamexz;
    QList<QString> selectedKernelName;
    QList<QLabel*> statusLabelVector;
    QList<QPushButton*> buttonVector;
    QList<QSpacerItem*> verticalSpacerVector;
    //---------------------------------------------------------


    //Private Functions
    void setMovies();
    void decompressFiles();
    void setConsoleWidget();
    bool checkKernelButtons();
    void downloadKernels();
    QString runShellCommand(QString order);
    bool openKernelFromDrive();
    bool yesOrNoMessageBox(const char* title, const char* question);
    bool printKernelChangeInfo(int version);
    void connectOrDisConect(FuncValue value);
    void showWidget(FuncValue value);
    void setTableLabels(bool value);
    void initVariables();
    void changeStretch(int first, int second, int third, int fourth);
    void makeButtons();
    void setStatusLabelString(int clear);
    void getSystemInformation();
};

#endif // MAINWINDOW_H
