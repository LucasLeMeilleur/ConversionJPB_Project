#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>  // Pour QFileDialog
#include <QMessageBox>  // Pour QMessageBox
#include <QDir>         // Pour QDir::homePath()
#include <QDebug>
#include <QFileInfo>
#include <QString>
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void Alert(QWidget *parent){

    qDebug() << QString::fromUtf8("Image créé");
    QMessageBox::warning(parent, QString::fromUtf8("Fichier créé"), QString::fromUtf8("Le fichier a été créé correctement"));
}


void MainWindow::on_pushButton_clicked()
{


    QString extensionInput = (ui->comboBox->currentText()).toLower();
    qDebug() << extensionInput;
    QString extensionOutput = (ui->comboBox_2->currentText()).toLower();
    qDebug() << extensionOutput;

    if(extensionInput == extensionOutput){
        QMessageBox::warning(this, "Erreur", "Les fichier sont similaire");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Selectionnez une image",
        QDir::homePath(),
        ("Images (*."+extensionInput.toLower()+")")
        );

    QFileInfo fileInfo(filePath);
    QString extension = fileInfo.suffix();

    if (!filePath.isEmpty()) {
        qDebug() << "Fichier selectionné : " << extension;
    } else {
        QMessageBox::warning(this, "Aucun fichier selectionné", "Vous n'avez pas selectionné de fichier !");
        return;
    }

    QString folderPath = QFileDialog::getExistingDirectory(
        nullptr,
        "Selectionner le dossier d'enregistrement",
        QDir::homePath(),
        QFileDialog::ShowDirsOnly
        );

    if(folderPath.isEmpty()){
        QMessageBox::warning(this,"Aucun dossier selectionné", "Le dossier selectionné est eronné ou n'existe pas !");
        return;
    }else{
        qDebug() << "Dossier selectionné : " << folderPath;
    }


    QString OutputFile = folderPath + "/image." + extensionOutput;
    std::string stdfilePath = filePath.toStdString();
    std::string stdOutputFile = OutputFile.toStdString();

    if(extensionInput == "png" && extensionOutput == "jpg"){
        Convert->ConvertPNGtoJPG(stdfilePath,stdOutputFile);
        Alert(this);
    }
    else if(extensionInput == "png" && extensionOutput == "bmp"){
        Convert->ConvertPNGtoBMP(stdfilePath, stdOutputFile);
        Alert(this);
    }
    else if(extensionInput == "jpg" && extensionOutput == "png"){
        Convert->ConvertJPGtoPNG(stdfilePath, stdOutputFile);
        Alert(this);
    }
    else if(extensionInput == "jpg" && extensionOutput == "bmp"){
        Convert->ConvertJPGtoBMP(stdfilePath, stdOutputFile);
        Alert(this);
    }
    else if(extensionInput == "bmp" && extensionOutput == "png"){
        Convert->ConvertJPGtoPNG(stdfilePath, stdOutputFile);
        Alert(this);
    }
    else if(extensionInput == "bmp" && extensionOutput == "jpg"){
        Convert->ConvertBMPtoJPG(stdfilePath, stdOutputFile);
        Alert(this  );
    }


}
