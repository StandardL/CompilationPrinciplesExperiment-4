#pragma once

#include <QtWidgets/QMainWindow>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <sstream>
#include "ui_SLR_GUI.h"
#include "SLR.h"

class SLR_GUI : public QMainWindow
{
    Q_OBJECT

public:
    SLR_GUI(QWidget *parent = nullptr);
    ~SLR_GUI();

private slots:
    void btnOpenFileClicked();
    void btnSaveFileClicked();
    void btnAnalyzeClicked();

private:
    Ui::SLR_GUIClass ui;

    SLR slr;
    
};
