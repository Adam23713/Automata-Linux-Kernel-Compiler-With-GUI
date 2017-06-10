#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMovies();
    getSystemInformation();
    initVariables();
    setTableLabels(false);
    changeStretch(15,1,0,84);
    setStatusLabelString(int(Clear::NONE));
}

MainWindow::~MainWindow()
{

    for( auto i : pictureLabelVector ) delete i;
    for( auto i : lineEditVector )     delete i;
    for( auto i : progressBarVector )  delete i;
    for( auto i : statusLabelVector )  delete i;
    for( auto i : movies )             delete i;

    if(console != nullptr)          delete console;
    if( fileDownloader != nullptr ) delete fileDownloader;
    if( palette != nullptr )        delete palette;
    if( information != nullptr )    delete information;

    connectOrDisConect(FuncValue::DISCONNECT);

    for( auto i : buttonVector ) delete i;

    delete ui;
}

//Kernel Downloaded Slot
void MainWindow::kernelDownloaded()
{
    int index = fileDownloader->GetCurrentIndex();
    if( index == 0 ){ movies.at(4)->start(); movies.at(7)->start();}

    pictureLabelVector.at(index)->clear();
    QFile file( workDirectory + kernelNamexz.at(index));
    file.open(QIODevice::WriteOnly);
    if(file.isOpen())
    {
        pictureLabelVector.at(index)->setMovie(movies.at(4));
        file.write( fileDownloader->GetDownloadedDatas() );
        file.close();
    }
    else
    {
         pictureLabelVector.at(index)->setMovie(movies.at(7));
         QMessageBox::critical(this,"File Error!", "You can't write: " + workDirectory + kernelNamexz.at(index));
    }
    pictureLabelVector.at(index)->show();

    if( !fileDownloader->StartNextDownload() )
    {
        ui->pictureLabel->clear();
        movies.at(1)->start();
        movies.at(5)->start();
        movies.at(6)->start();
        ui->pictureLabel->setMovie(movies.at(1));
        ui->pictureLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        ui->pictureLabel->setScaledContents(true);
        ui->pictureLabel->show();
        ui->nextButton->setEnabled(true);
        decompressFiles();
    }

}

//Kernel Buttons Slot
void MainWindow::kernelVersionClicked()
{
    unsigned int index = 0;
    QObject *obj = QObject::sender();
    QString senderobjName = obj->objectName();

    if( senderobjName.contains("selectedButtonDate") )
    {
        index = (senderobjName.split("selectedButtonDate"))[1].toInt();
        if(oldActiveButton ==  buttonVector.at(index))
        {
            buttonVector.at(index)->setChecked(true);
            return;
        }

        if(!printKernelChangeInfo(index))
            return;

        for(int i = 2; i < buttonVector.size(); i +=3)
        {
            if((unsigned int)i == index) continue;

            if(buttonVector.at(i)->isChecked())
                buttonVector.at(i)->setChecked(false);
        }
        oldActiveButton = buttonVector.at(index);
        return;
    }

    int number = senderobjName.contains("selectedButtonSource") ? 1 : -1;
    index = (senderobjName.split( number == 1 ? "selectedButtonSource" : "selectedButtonPatch" ))[1].toInt() + number;
    if(buttonVector.at(index)->isChecked())
    {
            buttonVector.at(index)->setChecked(false);
    }
    return;

}

//Search Button
void MainWindow::on_pushButton_2_clicked()
{
    ui->pushButton_2->setEnabled(false);
    currentStatus = int(Status::CHOOSE);

    changeStretch(15,1,50,34);

    //Search new kernels---------------------------------------------------------------------------------
    bool inf_1 = information->downloadHtmlSourceCodeTo(kernelVersions,"https://www.kernel.org/finger_banner");
    bool inf_2 = information->downloadHtmlSourceCodeTo(kernelDatas,"https://www.kernel.org");

    if( inf_1 && inf_2 )
    {
       setTableLabels(true);
       showWidget(FuncValue::TABWIDGET);
       showWidget(FuncValue::SCROLLWIDGET);
       makeButtons();
    }
    else
    {
       if( yesOrNoMessageBox("No conection!","Do you search new kernel on your machine?") )
       {
            if( !openKernelFromDrive() )
            {
                on_backButton_clicked();
                return;
            }
       }
       else
       {
           on_backButton_clicked();
           return;
       }
    }
    //-------------------------------------------------------------------------------------------------

    setStatusLabelString(int(Clear::UP));
    ui->nextButton->setEnabled(true);
    ui->backButton->setEnabled(true);
    ui->pushButton_2->setText("Re-Search");

    ui->pushButton_2->setEnabled(true);
}

//Next Button
void MainWindow::on_nextButton_clicked()
{
    currentStatus++;
    if(currentStatus == int(Status::CHOOSE))
    {
        changeStretch(15,1,50,34);
        showWidget(FuncValue::TABWIDGET);
        showWidget(FuncValue::SCROLLWIDGET);
    }
    else if(currentStatus == int(Status::DOWNLOAD))
    {

        if( checkKernelButtons() )
        {
            downloadKernels();
        }
        else
        {
            QMessageBox::critical(this,"Error!","No selected kernel!");
            currentStatus--;
            return;
        }
        ui->pushButton_2->setEnabled(false);
        ui->scrollArea->setVisible(false);
        ui->tabWidget->setVisible(false);
        ui->downloadWidget->setVisible(true);
        ui->pictureLabel->setVisible(true);
    }
    else if(currentStatus == int(Status::CONFIGURE))
    {
        changeStretch(15,1,84,0);
        ui->configureWidget->setVisible(true);
        setConsoleWidget();
        ui->downloadWidget->setVisible(false);
        ui->pictureLabel->setVisible(false);
    }
    else if(currentStatus == int(Status::COMPILE))
    {
        changeStretch(15,1,50,34);
        ui->configureWidget->setVisible(false);
    }
    else if(currentStatus == int(Status::INSTALL))
    {
        ui->nextButton->setEnabled(false);
    }

    setStatusLabelString(int(Clear::UP));
    ui->backButton->setEnabled(true);

}

//Back Button
void MainWindow::on_backButton_clicked()
{
    bool value = true;
    currentStatus--;
    setStatusLabelString(int(Clear::DOWN));

    if(currentStatus == int(Status::INFORMATION))
    {
        ui->scrollArea->setVisible(false);
        changeStretch(15,1,1,83);
        showWidget(FuncValue::INFOWIDGET);
        ui->backButton->setEnabled(false);
        ui->pushButton_2->setEnabled(true);
        if(buttonVector.isEmpty())
            value = false;
    }
    else if(currentStatus == int(Status::CHOOSE))
    {
        ui->pushButton_2->setEnabled(true);
        ui->scrollArea->setVisible(true);
        ui->tabWidget->setVisible(true);
        ui->downloadWidget->setVisible(false);
        ui->pictureLabel->setVisible(false);
    }
    else if(currentStatus == int(Status::DOWNLOAD))
    {
        changeStretch(15,1,50,34);
        ui->configureWidget->setVisible(false);
        ui->downloadWidget->setVisible(true);
        ui->pictureLabel->setVisible(true);
    }
    else if(currentStatus == int(Status::CONFIGURE))
    {
        changeStretch(15,1,84,0);
        ui->configureWidget->setVisible(true);
    }
    else if(currentStatus == int(Status::COMPILE))
    {

    }

    ui->nextButton->setEnabled(value);

}


//Menu panel------------------------------------------------------------

//Exit from program
void MainWindow::on_actionExit_triggered()
{
    if( !saveStatus || configFilePath.isEmpty() )
    {
        if( yesOrNoMessageBox("Changes","Do you save the changes?") )
        {
            if(configFilePath.isEmpty())
            {
                on_actionSave_Configure_As_triggered();
                if( !saveStatus )
                {
                    this->close();
                    return;
                }
            }
            qDebug() << "Save Function";
        }
    }
    this->close();
}

//Open Config file
void MainWindow::on_actionOpen_Configure_File_2_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Config File"), QDir::homePath(), tr("*.conf"));
    if(fileName.isEmpty()) return;

    configFilePath = fileName;
    saveStatus = true;
    setWindowTitle("Update Linux Kernel\t" + configFilePath);
}

//Open downloaded kernel file
void MainWindow::on_actionOpen_Downloaded_Kernel_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Config File"), QDir::homePath(), tr("*.xz\n*.tar.xz"));
    if(fileName.isEmpty()) return;
}

//Save Config File
void MainWindow::on_actionSave_Configure_triggered()
{
    if(configFilePath.isEmpty())
    {
        on_actionSave_Configure_As_triggered();
        return;
    }
    saveStatus = true;
    qDebug() << "Save Function";
}

//Save Config File As...
void MainWindow::on_actionSave_Configure_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Config File"), QDir::homePath(), tr("*.conf"));
    if(fileName.isEmpty())
    {
        saveStatus = false;
        return;
    }

    if( !fileName.contains(".conf") )
        fileName += ".conf";

    saveStatus = true;
    configFilePath = fileName;
    setWindowTitle("Update Linux Kernel\t" + configFilePath);

    qDebug() << "Save As Function";
}

//----------------------------------------------------------------------

//Functions
void MainWindow::setMovies()
{
    movies = {new QMovie("datas/logos/downloadingAnimation.gif"), new QMovie("datas/logos/zipAnimation.gif"),
             new QMovie("datas/logos/done.jpg"), new QMovie("datas/logos/download.gif"), new QMovie("datas/logos/zip.png"),
             new QMovie("datas/logos/extract_logo.png"), new QMovie("datas/logos/success.png"),
             new QMovie("datas/logos/cancel.png")};

}

bool MainWindow::checkKernelButtons()
{
    selectedKernelName.clear();
    for(auto i : buttonVector)
    {
        if( i->isChecked() &&  !((i->objectName()).contains("selectedButtonDate")) )
        {
                selectedKernelName.append(i->text());
        }
    }
    return selectedKernelName.length() >= 1;
}

void MainWindow::downloadKernels()
{
    if( successDownload ) return;

    ui->nextButton->setEnabled(false);
    //Get Download links-------------------------------------------
    kernelNamexz.clear();
    QList<QString> kernelLinks;
    for(int i = 0; i < selectedKernelName.length(); i++)
    {
        QString regExp;
        if(selectedKernelName.at(i).contains("Source"))
        {
            QString tmp = "linux-" + selectedKernelName.at(i);
            tmp.remove(QRegExp(" Source"));
            kernelNamexz.append(tmp + ".tar.xz");
            regExp = "https://cdn\\.kernel\\.org/pub/linux/kernel/v[0-9]{1,2}\\.x/(|testing/)" + tmp + "\\.tar\\.xz";
        }
        else if(selectedKernelName.at(i).contains("Patch"))
        {
            QString tmp = "patch-" + selectedKernelName.at(i);
            tmp.remove(QRegExp(" Patch"));
            tmp.remove(QRegExp("\\(EOL\\)"));

            qDebug() << tmp;
            QString sNumber = "";
            int j = tmp.length() - 1;
            while( tmp[j] != '.' )
            {
                j--;
            }
            for(int k = j + 1; k < tmp.length(); k++)
                sNumber += tmp[k];
            QString fNumber = QString::number(sNumber.toInt() - 1);

            tmp = tmp.replace(sNumber,fNumber);
            tmp += "-" + sNumber;
            kernelNamexz.append(tmp + ".xz");
            regExp = "https://cdn\\.kernel\\.org/pub/linux/kernel/v[0-9]{1,2}\\.x/incr/" + tmp + "\\.xz";
        }

        kernelLinks.append( (information->getDataToSource(kernelDatas,regExp))[0] );
    }
    //-------------------------------------------------------------

    //Set Pictures---------------------------------------------------------------------
    ui->pictureLabel->setMovie(movies.at(0));
    ui->pictureLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->pictureLabel->setScaledContents(true);
    movies.at(0)->start();
    movies.at(3)->start();
    //----------------------------------------------------------------------------------

    int row = 0;
    int column;
    for(int i = 0; i < selectedKernelName.length(); i++)
    {
        column = 0;

        //Make Label
        QLabel *label = new QLabel(this);
        label->setFixedSize(40,40);
        label->setMovie(movies.at(3));
        label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        label->setScaledContents(true);
        label->show();

        //Make LineEdit
        QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setText(selectedKernelName.at(i));
        lineEdit->setReadOnly(true);

        if( palette == nullptr )
        {
            palette = new QPalette();
            palette->setColor(QPalette::Base,Qt::gray);
            palette->setColor(QPalette::Text,Qt::darkGray);
        }
        lineEdit->setPalette(*palette);

        //Make QProcessBar
        QProgressBar *pBar = new QProgressBar();
        pBar->setValue(0);

        //Make Space Holder
        QSpacerItem* spacer_1 = new QSpacerItem(40,40,QSizePolicy::Ignored, QSizePolicy::Ignored);

        //Add to layout
        ui->gridLayout_2->addWidget(label, row, column++);
        ui->gridLayout_2->addItem(spacer_1, row, column-1);
        ui->gridLayout_2->addWidget(lineEdit, row, column++);
        ui->gridLayout_2->addWidget(pBar, row, column);

        //Add to vector
        pictureLabelVector.append(label);
        lineEditVector.append(lineEdit);
        progressBarVector.append(pBar);
        verticalSpacerVector.append(spacer_1);

        row++;
    }

    //Download kernels
    fileDownloader = new FileDownloader(kernelLinks, this);
    connect(fileDownloader, SIGNAL (downloaded()), this, SLOT (kernelDownloaded()));
    fileDownloader->StartNextDownload();

    successDownload = true;
}

void MainWindow::getSystemInformation()
{

    //Get System Informations
    QString sysInfoString = runShellCommand("./datas/screenfetch-dev -N -n");
    QString shellVersion = runShellCommand("./datas/shellType.sh");

    QList<QString> datas;
    QString tmp = "";
    for(int i = 0; i < sysInfoString.length(); i++)
    {
        if( sysInfoString.at(i) == '\n' )
        {
            if(tmp == "" || tmp == "\n")
            {
                tmp = "";
                continue;
            }
            datas.append(tmp);
            tmp = "";
        }
        else
            tmp += sysInfoString.at(i);
    }

    //Make HTML web page
    QList<QString> imagesName = {"workstation.png","Mint.png","kernel.jpg","uptime.jpg","packages.png","shell.png",
                                "resolution.jpg","desktopenvironment.jpg","wm.png","theme.png","icons.jpg",
                                "font.png","cpu.jpg","ram.png"};

    QList<QString> infoName = {"Computer Name","Operation System","Kernel","Uptime","Packages","Shell",
                               "Resolution","Desktop Enviroment","Window Manager","GTK Theme","Icon Theme",
                              "Font","CPU","RAM"};


    QString newTag;
    QString webPageSourceCode;
    webPageSourceCode = "<!doctype html>\n<html>\n<head>\n<style> body{background-color: #FFFFFF;} </style>\n</head>\n<body>\n<table>\n";
    for(int j = 0; j < imagesName.length(); j++)
    {

        newTag = "<tr>\n<td><img src=\"datas/logos/" + imagesName.at(j) + "\" alt=\"" + imagesName.at(j) + "\"><br>" + infoName.at(j);
        newTag += "\n<td><center><br><br>" + datas.at(j) + "</center></td>\n</tr>\n";
        webPageSourceCode += newTag;
    }
    webPageSourceCode += "</table></body>\n</html>";

    ui->informationTextEdit->clear();
    ui->informationTextEdit->setHtml(webPageSourceCode);

}

QString MainWindow::runShellCommand(QString order)
{
    QString resultString;
    QProcess p;
    p.start(order);
    p.waitForFinished();
    resultString = p.readAllStandardOutput();
    p.close();

    return resultString;
}

void MainWindow::setStatusLabelString(int clear)
{
    if(currentStatus > 5) return;

    QFont font = statusLabelVector.at(currentStatus)->font();
    font.setBold(true);
    statusLabelVector.at(currentStatus)->setFont(font);
    statusLabelVector.at(currentStatus)->setText( statusLabelVector.at(currentStatus)->text() + "..." );

    if( clear == int(Clear::NONE) ) return;

    font = statusLabelVector.at(currentStatus + clear)->font();
    font.setBold(false);
    statusLabelVector.at(currentStatus + clear)->setFont(font);
    QString strText = statusLabelVector.at(currentStatus + clear)->text();
    strText.replace(QString("..."), QString(""));
    statusLabelVector.at(currentStatus + clear)->setText(strText);
}

bool MainWindow::yesOrNoMessageBox(const char* title, const char* question)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, title, question, QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
        return true;
    else
        return false;
}

bool MainWindow::openKernelFromDrive()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Search kernel"), QDir::homePath(), tr("*.tar.xz\n*.xz"));
    if(fileName.isEmpty()) return false;
    return true;
}

void MainWindow::makeButtons()
{
    QString regExpVersion = " {2}[0-9]\\.([0-9]{1,3}\\.{0,1}){1,3}(| \\(EOL\\)|-rc[0-9])";
    QString regExpDate = "[0-9]{4}-[0-9]{2}-[0-9]{2}";
    QList<QString> kernelName = information->getDataToSource(kernelVersions,regExpVersion);
    QList<QString> kernelDate = information->getDataToSource(kernelDatas,regExpDate);

    int replaceCount = 0;
    for(auto i : kernelName)
        kernelName[replaceCount++] = i.replace( " ", "" );

    if( !kernelName.length() ) return;

    int row = 1;
    int column;
    int countId = 0;
    for(int i = 0; i < kernelName.length(); i++)
    {
        column = 0;
        QPushButton* butt_1 = new QPushButton(kernelName.at(i) + " Source",this);
        QPushButton* butt_2 = new QPushButton(kernelName.at(i) + " Patch",this);
        QPushButton* butt_3 = new QPushButton(kernelDate.at(i),this);

        butt_1->setObjectName("selectedButtonSource" + QString::number(countId++));
        butt_2->setObjectName("selectedButtonPatch" + QString::number(countId++));
        butt_3->setObjectName("selectedButtonDate" + QString::number(countId++));

        butt_1->setCheckable(true);
        butt_2->setCheckable(true);
        butt_3->setCheckable(true);

        ui->ScroolAreagridLayout->addWidget(butt_1, row, column++);
        ui->ScroolAreagridLayout->addWidget(butt_2, row, column++);
        ui->ScroolAreagridLayout->addWidget(butt_3, row, column);


        row++;
        column = 0;
        QSpacerItem* spacer_1 = new QSpacerItem(40,40,QSizePolicy::Ignored, QSizePolicy::Ignored);
        QSpacerItem* spacer_2 = new QSpacerItem(40,40,QSizePolicy::Ignored, QSizePolicy::Ignored);
        QSpacerItem* spacer_3 = new QSpacerItem(40,40,QSizePolicy::Ignored, QSizePolicy::Ignored);

        ui->ScroolAreagridLayout->addItem(spacer_1,row,column++,-1,-1);
        ui->ScroolAreagridLayout->addItem(spacer_2,row,column++,-1,-1);
        ui->ScroolAreagridLayout->addItem(spacer_3,row,column,-1,-1);

        verticalSpacerVector.push_back(spacer_1);
        verticalSpacerVector.push_back(spacer_2);
        verticalSpacerVector.push_back(spacer_3);

        buttonVector.push_back(butt_1);
        buttonVector.push_back(butt_2);
        buttonVector.push_back(butt_3);

        row++;
    }
    connectOrDisConect(FuncValue::CONNECT);

    if( kernelName.at(0).contains("rc") )
         buttonVector.at(1)->setEnabled(false);

    /*for( auto i : kernelName )
    {
        qDebug() << i;
        for(int j = i.length(); j <=0 j++)
        {

        }
    }*/


}

void MainWindow::changeStretch(int first, int second, int third, int fourth)
{
    ui->MainWidgetLayout->setStretch(0,first);
    ui->MainWidgetLayout->setStretch(1,second);
    ui->MainWidgetLayout->setStretch(2,third);
    ui->MainWidgetLayout->setStretch(3,fourth);

}

void MainWindow::initVariables()
{
    successDownload = false;
    saveStatus = false;
    currentStatus = int(Status::INFORMATION);
    configFilePath = "";
    oldActiveButton = nullptr;
    ui->tabWidget->setVisible(false);
    ui->scrollArea->setVisible(false);
    ui->configureWidget->setVisible(false);
    ui->tabWidget_2->setVisible(false);
    ui->downloadWidget->setVisible(false);
    ui->pictureLabel->setVisible(false);
    statusLabelVector.push_back(ui->StatusLabel_1);
    statusLabelVector.push_back(ui->StatusLabel_2);
    statusLabelVector.push_back(ui->StatusLabel_3);
    statusLabelVector.push_back(ui->StatusLabel_4);
    statusLabelVector.push_back(ui->StatusLabel_5);
    statusLabelVector.push_back(ui->StatusLabel_6);

    information = new KernelInformation();
    QPalette p = ui->changeLogTextEdit->palette();
    p.setColor(QPalette::Base, QColor(238, 238, 238));
    ui->changeLogTextEdit->setPalette(p);
}

void MainWindow::setTableLabels(bool value)
{
    ui->sourceCodeLabel->setVisible(value);
    ui->patchLabel->setVisible(value);
    ui->dateAndInfoLabel->setVisible(value);
}

void MainWindow::connectOrDisConect(FuncValue value)
{
    if( value == FuncValue::CONNECT )
    {
       for(auto button : buttonVector)
            connect(button, SIGNAL(clicked()), this, SLOT(kernelVersionClicked()));
    }
    else if( value == FuncValue::DISCONNECT )
    {
        for(auto button : buttonVector)
            disconnect(button, SIGNAL(clicked()), this, SLOT(kernelVersionClicked()));
    }
}

void MainWindow::showWidget(FuncValue value)
{
    if(value == FuncValue::TABWIDGET)
    {
        ui->informationTextEdit->setVisible(false);
        ui->tabWidget->setVisible(true);
    }

    if(value == FuncValue::INFOWIDGET)
    {
        ui->informationTextEdit->setVisible(true);
        ui->tabWidget->setVisible(false);
    }

    if(value == FuncValue::SCROLLWIDGET)
    {
        ui->scrollArea->setVisible(true);
    }

}

bool MainWindow::printKernelChangeInfo(int id)
{
    bool rc;
    QString version;
    QString regExp;
    QList<QString> link;
    version = (buttonVector.at(id-2)->text()).split(" Source")[0];
    version.replace(" (EOL)","");

    //if(version.contains(QRegExp("rc[0-9]{0,1}")))
    if( id == 2)
    {
        rc = true;
        regExp =   "https://git\\.kernel\\.org/cgit/linux/kernel/git/torvalds/linux\\.git/diff/\\?id=v" + QRegExp::escape(version) + "&id2=v[0-9]{1,3}\\.[0-9]{1,3}&dt=2";
        //regExp = "https://git\\.kernel\\.org/cgit/linux/kernel/git/torvalds/linux\\.git/diff/\\?id=v" + QRegExp::escape(version) + "&id2=v[0-9]{1,3}\\.[0-9]{1,3}(|\\.[0-9]{1,3})-rc[0-9]{0,1}&dt=2";
    }
    else
    {
        rc = false;
        regExp = "https://git\\.kernel\\.org/cgit/linux/kernel/git/stable/linux-stable\\.git/diff/\\?id=v" + QRegExp::escape(version) + "&id2=v[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}&dt=2";
    }
    link = information->getDataToSource(kernelDatas,regExp);
    qDebug() << link;

    if(link.isEmpty())
    {
        QMessageBox::critical(this,"View Diff Error!","RegExp error\nPlease check the settings!");
        buttonVector.at(id)->setChecked(false);
        return false;
    }

    information->downloadHtmlSourceCodeTo("viewDiff",link.at(0));

    QString modString = information->getWebPageSource("viewDiff");

    int begin = modString.indexOf("<table id='header'>");
    int end = modString.indexOf("<div class='content'>");
    end = end - begin;
    modString.remove(begin,end);
    begin = modString.indexOf("<div class='cgit-panel'>");
    end = modString.indexOf("<div class='diffstat-header'>");
    end = end - begin;
    modString.remove(begin,end);
    modString.replace("href='/cgit-korg-0.10.1.css'","href='datas/cgit-korg-0.10.1.css'");
    modString.replace("-rw-r--r--","");

    ui->viewDiffTextEdit->clear();
    ui->viewDiffTextEdit->append(modString);
    ui->viewDiffTextEdit->moveCursor(QTextCursor::Start) ;
    ui->viewDiffTextEdit->ensureCursorVisible() ;

    if(rc)
    {
        ui->changeLogTextEdit->clear();
        return true;
    }

    regExp = "https://cdn\\.kernel\\.org/pub/linux/kernel/v[0-9]{1,2}\\.x/ChangeLog-" + version;
    link = information->getDataToSource(kernelDatas,regExp);
    if(link.isEmpty())
    {
        QMessageBox::critical(this,"ChangeLog Error!","RegExp error\nPlease check the settings!");
        buttonVector.at(id)->setChecked(false);
        return false;
    }

    information->downloadHtmlSourceCodeTo(version,link.at(0));
    ui->changeLogTextEdit->clear();
    ui->changeLogTextEdit->append(information->getWebPageSource(version));
    ui->changeLogTextEdit->moveCursor (QTextCursor::Start) ;
    ui->changeLogTextEdit->ensureCursorVisible();

    return true;
}

void MainWindow::setConsoleWidget()
{
    if(console == nullptr)
    {
        console = new QTermWidget(ui->configureWidget);
        QFont font = QApplication::font();
        font.setFamily("Monospace");
        font.setPointSize(12);
        console->setTerminalFont(font);

        foreach (QString arg, QApplication::arguments())
        {
            if (console->availableColorSchemes().contains(arg))
                console->setColorScheme(arg);
            if (console->availableKeyBindings().contains(arg))
                console->setKeyBindings(arg);
        }
        ui->configureLayout->addWidget(console);
        QObject::connect(console, SIGNAL(finished()), this, SLOT(close()));
    }
}

void MainWindow::decompressFiles()
{
    for(int i = 0; i < kernelNamexz.length(); i++)
    {
        if( pictureLabelVector.at(i)->movie() == movies.at(7) ) continue;

        pictureLabelVector.at(i)->clear();
        if( kernelNamexz.at(i).contains("patch") )
        {
            pictureLabelVector.at(i)->setMovie(movies.at(6));
            pictureLabelVector.at(i)->show();
        }
        else
        {
            pictureLabelVector.at(i)->setMovie(movies.at(5));
            pictureLabelVector.at(i)->show();

            QString command = "sh datas/uncompress.sh " + workDirectory + " " + kernelNamexz.at(i);
            runShellCommand(command);

            pictureLabelVector.at(i)->clear();
            pictureLabelVector.at(i)->setMovie(movies.at(6));
        }


    }

    movies.at(2)->start();
    ui->pictureLabel->clear();
    ui->pictureLabel->setMovie(movies.at(2));
    ui->pictureLabel->show();
}
