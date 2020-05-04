/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.9
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_18;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QPushButton *pushButton_UpdateGameWindows;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_12;
    QComboBox *comboBox_GameWindows;
    QHBoxLayout *horizontalLayout_20;
    QLabel *label_WindowTitlePrefix;
    QLineEdit *lineEdit_WindowTitle;
    QPushButton *pushButton_ChangeWindowTitle;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_16;
    QSpacerItem *horizontalSpacer_16;
    QCheckBox *checkBox_Switch;
    QSpacerItem *horizontalSpacer_12;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox_F1;
    QDoubleSpinBox *doubleSpinBox_F1;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *checkBox_F2;
    QDoubleSpinBox *doubleSpinBox_F2;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *checkBox_F3;
    QDoubleSpinBox *doubleSpinBox_F3;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *checkBox_F4;
    QDoubleSpinBox *doubleSpinBox_F4;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *checkBox_F5;
    QDoubleSpinBox *doubleSpinBox_F5;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_6;
    QCheckBox *checkBox_F6;
    QDoubleSpinBox *doubleSpinBox_F6;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_7;
    QCheckBox *checkBox_F7;
    QDoubleSpinBox *doubleSpinBox_F7;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout_8;
    QCheckBox *checkBox_F8;
    QDoubleSpinBox *doubleSpinBox_F8;
    QSpacerItem *horizontalSpacer_8;
    QHBoxLayout *horizontalLayout_9;
    QCheckBox *checkBox_F9;
    QDoubleSpinBox *doubleSpinBox_F9;
    QSpacerItem *horizontalSpacer_9;
    QHBoxLayout *horizontalLayout_10;
    QCheckBox *checkBox_F10;
    QDoubleSpinBox *doubleSpinBox_F10;
    QSpacerItem *horizontalSpacer_10;
    QHBoxLayout *horizontalLayout_13;
    QSpacerItem *horizontalSpacer_11;
    QLabel *label_PlayerHealth;
    QSpacerItem *horizontalSpacer_13;
    QHBoxLayout *horizontalLayout_12;
    QCheckBox *checkBox_AutoPlayerHealth;
    QSpinBox *spinBox_MinPlayerHealth;
    QComboBox *comboBox_PlayerHealthKey;
    QHBoxLayout *horizontalLayout_17;
    QSpacerItem *horizontalSpacer_14;
    QLabel *label_PetResource;
    QSpacerItem *horizontalSpacer_15;
    QHBoxLayout *horizontalLayout_15;
    QCheckBox *checkBox_AutoPetSupply;
    QSpinBox *spinBox_MinPetHealth;
    QComboBox *comboBox_PetHealthKey;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label;
    QComboBox *comboBox_Configs;
    QPushButton *pushButton_SaveConfigAs;
    QPushButton *pushButton_RenameConfig;
    QPushButton *pushButton_DeleteConfig;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_19;
    QSpacerItem *horizontalSpacer_17;
    QLabel *label_SampleImage;
    QSpacerItem *horizontalSpacer_18;
    QPushButton *pushButton_ReadImage;
    QPushButton *pushButton_TestPlayerSupply;
    QPushButton *pushButton_TestPetSupply;
    QSpacerItem *verticalSpacer;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(274, 871);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout_18 = new QHBoxLayout(centralwidget);
        horizontalLayout_18->setObjectName(QStringLiteral("horizontalLayout_18"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        verticalLayout_5 = new QVBoxLayout(tab);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        groupBox_3 = new QGroupBox(tab);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        pushButton_UpdateGameWindows = new QPushButton(groupBox_3);
        pushButton_UpdateGameWindows->setObjectName(QStringLiteral("pushButton_UpdateGameWindows"));

        verticalLayout_3->addWidget(pushButton_UpdateGameWindows);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        label_12 = new QLabel(groupBox_3);
        label_12->setObjectName(QStringLiteral("label_12"));

        horizontalLayout_11->addWidget(label_12);

        comboBox_GameWindows = new QComboBox(groupBox_3);
        comboBox_GameWindows->setObjectName(QStringLiteral("comboBox_GameWindows"));
        comboBox_GameWindows->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
        comboBox_GameWindows->setIconSize(QSize(90, 14));

        horizontalLayout_11->addWidget(comboBox_GameWindows);

        horizontalLayout_11->setStretch(1, 1);

        verticalLayout_3->addLayout(horizontalLayout_11);

        horizontalLayout_20 = new QHBoxLayout();
        horizontalLayout_20->setObjectName(QStringLiteral("horizontalLayout_20"));
        label_WindowTitlePrefix = new QLabel(groupBox_3);
        label_WindowTitlePrefix->setObjectName(QStringLiteral("label_WindowTitlePrefix"));

        horizontalLayout_20->addWidget(label_WindowTitlePrefix);

        lineEdit_WindowTitle = new QLineEdit(groupBox_3);
        lineEdit_WindowTitle->setObjectName(QStringLiteral("lineEdit_WindowTitle"));

        horizontalLayout_20->addWidget(lineEdit_WindowTitle);

        horizontalLayout_20->setStretch(1, 1);

        verticalLayout_3->addLayout(horizontalLayout_20);

        pushButton_ChangeWindowTitle = new QPushButton(groupBox_3);
        pushButton_ChangeWindowTitle->setObjectName(QStringLiteral("pushButton_ChangeWindowTitle"));

        verticalLayout_3->addWidget(pushButton_ChangeWindowTitle);


        verticalLayout_5->addWidget(groupBox_3);

        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        horizontalSpacer_16 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_16->addItem(horizontalSpacer_16);

        checkBox_Switch = new QCheckBox(groupBox_2);
        checkBox_Switch->setObjectName(QStringLiteral("checkBox_Switch"));
        checkBox_Switch->setChecked(false);
        checkBox_Switch->setTristate(false);

        horizontalLayout_16->addWidget(checkBox_Switch);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_16->addItem(horizontalSpacer_12);


        verticalLayout_2->addLayout(horizontalLayout_16);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        checkBox_F1 = new QCheckBox(groupBox_2);
        checkBox_F1->setObjectName(QStringLiteral("checkBox_F1"));

        horizontalLayout->addWidget(checkBox_F1);

        doubleSpinBox_F1 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F1->setObjectName(QStringLiteral("doubleSpinBox_F1"));
        doubleSpinBox_F1->setDecimals(1);
        doubleSpinBox_F1->setMinimum(0.1);
        doubleSpinBox_F1->setMaximum(365);
        doubleSpinBox_F1->setSingleStep(0.1);
        doubleSpinBox_F1->setValue(1);

        horizontalLayout->addWidget(doubleSpinBox_F1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        checkBox_F2 = new QCheckBox(groupBox_2);
        checkBox_F2->setObjectName(QStringLiteral("checkBox_F2"));

        horizontalLayout_2->addWidget(checkBox_F2);

        doubleSpinBox_F2 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F2->setObjectName(QStringLiteral("doubleSpinBox_F2"));
        doubleSpinBox_F2->setDecimals(1);
        doubleSpinBox_F2->setMinimum(0.1);
        doubleSpinBox_F2->setMaximum(365);
        doubleSpinBox_F2->setSingleStep(0.1);
        doubleSpinBox_F2->setValue(1);

        horizontalLayout_2->addWidget(doubleSpinBox_F2);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        checkBox_F3 = new QCheckBox(groupBox_2);
        checkBox_F3->setObjectName(QStringLiteral("checkBox_F3"));

        horizontalLayout_3->addWidget(checkBox_F3);

        doubleSpinBox_F3 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F3->setObjectName(QStringLiteral("doubleSpinBox_F3"));
        doubleSpinBox_F3->setDecimals(1);
        doubleSpinBox_F3->setMinimum(0.1);
        doubleSpinBox_F3->setMaximum(365);
        doubleSpinBox_F3->setSingleStep(0.1);
        doubleSpinBox_F3->setValue(1);

        horizontalLayout_3->addWidget(doubleSpinBox_F3);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        checkBox_F4 = new QCheckBox(groupBox_2);
        checkBox_F4->setObjectName(QStringLiteral("checkBox_F4"));

        horizontalLayout_4->addWidget(checkBox_F4);

        doubleSpinBox_F4 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F4->setObjectName(QStringLiteral("doubleSpinBox_F4"));
        doubleSpinBox_F4->setDecimals(1);
        doubleSpinBox_F4->setMinimum(0.1);
        doubleSpinBox_F4->setMaximum(365);
        doubleSpinBox_F4->setSingleStep(0.1);
        doubleSpinBox_F4->setValue(1);

        horizontalLayout_4->addWidget(doubleSpinBox_F4);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        checkBox_F5 = new QCheckBox(groupBox_2);
        checkBox_F5->setObjectName(QStringLiteral("checkBox_F5"));

        horizontalLayout_5->addWidget(checkBox_F5);

        doubleSpinBox_F5 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F5->setObjectName(QStringLiteral("doubleSpinBox_F5"));
        doubleSpinBox_F5->setDecimals(1);
        doubleSpinBox_F5->setMinimum(0.1);
        doubleSpinBox_F5->setMaximum(365);
        doubleSpinBox_F5->setSingleStep(0.1);
        doubleSpinBox_F5->setValue(1);

        horizontalLayout_5->addWidget(doubleSpinBox_F5);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout_2->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        checkBox_F6 = new QCheckBox(groupBox_2);
        checkBox_F6->setObjectName(QStringLiteral("checkBox_F6"));

        horizontalLayout_6->addWidget(checkBox_F6);

        doubleSpinBox_F6 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F6->setObjectName(QStringLiteral("doubleSpinBox_F6"));
        doubleSpinBox_F6->setDecimals(1);
        doubleSpinBox_F6->setMinimum(0.1);
        doubleSpinBox_F6->setMaximum(365);
        doubleSpinBox_F6->setSingleStep(0.1);
        doubleSpinBox_F6->setValue(1);

        horizontalLayout_6->addWidget(doubleSpinBox_F6);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_6);


        verticalLayout_2->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        checkBox_F7 = new QCheckBox(groupBox_2);
        checkBox_F7->setObjectName(QStringLiteral("checkBox_F7"));

        horizontalLayout_7->addWidget(checkBox_F7);

        doubleSpinBox_F7 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F7->setObjectName(QStringLiteral("doubleSpinBox_F7"));
        doubleSpinBox_F7->setDecimals(1);
        doubleSpinBox_F7->setMinimum(0.1);
        doubleSpinBox_F7->setMaximum(365);
        doubleSpinBox_F7->setSingleStep(0.1);
        doubleSpinBox_F7->setValue(1);

        horizontalLayout_7->addWidget(doubleSpinBox_F7);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_7);


        verticalLayout_2->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        checkBox_F8 = new QCheckBox(groupBox_2);
        checkBox_F8->setObjectName(QStringLiteral("checkBox_F8"));

        horizontalLayout_8->addWidget(checkBox_F8);

        doubleSpinBox_F8 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F8->setObjectName(QStringLiteral("doubleSpinBox_F8"));
        doubleSpinBox_F8->setDecimals(1);
        doubleSpinBox_F8->setMinimum(0.1);
        doubleSpinBox_F8->setMaximum(365);
        doubleSpinBox_F8->setSingleStep(0.1);
        doubleSpinBox_F8->setValue(1);

        horizontalLayout_8->addWidget(doubleSpinBox_F8);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_8);


        verticalLayout_2->addLayout(horizontalLayout_8);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        checkBox_F9 = new QCheckBox(groupBox_2);
        checkBox_F9->setObjectName(QStringLiteral("checkBox_F9"));

        horizontalLayout_9->addWidget(checkBox_F9);

        doubleSpinBox_F9 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F9->setObjectName(QStringLiteral("doubleSpinBox_F9"));
        doubleSpinBox_F9->setDecimals(1);
        doubleSpinBox_F9->setMinimum(0.1);
        doubleSpinBox_F9->setMaximum(365);
        doubleSpinBox_F9->setSingleStep(0.1);
        doubleSpinBox_F9->setValue(1);

        horizontalLayout_9->addWidget(doubleSpinBox_F9);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_9);


        verticalLayout_2->addLayout(horizontalLayout_9);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        checkBox_F10 = new QCheckBox(groupBox_2);
        checkBox_F10->setObjectName(QStringLiteral("checkBox_F10"));

        horizontalLayout_10->addWidget(checkBox_F10);

        doubleSpinBox_F10 = new QDoubleSpinBox(groupBox_2);
        doubleSpinBox_F10->setObjectName(QStringLiteral("doubleSpinBox_F10"));
        doubleSpinBox_F10->setDecimals(1);
        doubleSpinBox_F10->setMinimum(0.1);
        doubleSpinBox_F10->setMaximum(365);
        doubleSpinBox_F10->setSingleStep(0.1);
        doubleSpinBox_F10->setValue(1);

        horizontalLayout_10->addWidget(doubleSpinBox_F10);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_10);


        verticalLayout_2->addLayout(horizontalLayout_10);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_11);

        label_PlayerHealth = new QLabel(groupBox_2);
        label_PlayerHealth->setObjectName(QStringLiteral("label_PlayerHealth"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_PlayerHealth->sizePolicy().hasHeightForWidth());
        label_PlayerHealth->setSizePolicy(sizePolicy);
        label_PlayerHealth->setMinimumSize(QSize(198, 14));
        label_PlayerHealth->setMaximumSize(QSize(198, 14));
        label_PlayerHealth->setFrameShape(QFrame::Box);
        label_PlayerHealth->setFrameShadow(QFrame::Plain);
        label_PlayerHealth->setLineWidth(1);
        label_PlayerHealth->setMidLineWidth(0);
        label_PlayerHealth->setScaledContents(true);

        horizontalLayout_13->addWidget(label_PlayerHealth);

        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_13);


        verticalLayout_2->addLayout(horizontalLayout_13);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        checkBox_AutoPlayerHealth = new QCheckBox(groupBox_2);
        checkBox_AutoPlayerHealth->setObjectName(QStringLiteral("checkBox_AutoPlayerHealth"));

        horizontalLayout_12->addWidget(checkBox_AutoPlayerHealth);

        spinBox_MinPlayerHealth = new QSpinBox(groupBox_2);
        spinBox_MinPlayerHealth->setObjectName(QStringLiteral("spinBox_MinPlayerHealth"));
        spinBox_MinPlayerHealth->setMinimum(1);
        spinBox_MinPlayerHealth->setValue(50);

        horizontalLayout_12->addWidget(spinBox_MinPlayerHealth);

        comboBox_PlayerHealthKey = new QComboBox(groupBox_2);
        comboBox_PlayerHealthKey->setObjectName(QStringLiteral("comboBox_PlayerHealthKey"));

        horizontalLayout_12->addWidget(comboBox_PlayerHealthKey);

        horizontalLayout_12->setStretch(2, 1);

        verticalLayout_2->addLayout(horizontalLayout_12);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setObjectName(QStringLiteral("horizontalLayout_17"));
        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_17->addItem(horizontalSpacer_14);

        label_PetResource = new QLabel(groupBox_2);
        label_PetResource->setObjectName(QStringLiteral("label_PetResource"));
        sizePolicy.setHeightForWidth(label_PetResource->sizePolicy().hasHeightForWidth());
        label_PetResource->setSizePolicy(sizePolicy);
        label_PetResource->setMinimumSize(QSize(68, 68));
        label_PetResource->setMaximumSize(QSize(68, 68));
        label_PetResource->setFrameShape(QFrame::Box);
        label_PetResource->setFrameShadow(QFrame::Plain);
        label_PetResource->setScaledContents(true);

        horizontalLayout_17->addWidget(label_PetResource);

        horizontalSpacer_15 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_17->addItem(horizontalSpacer_15);


        verticalLayout_2->addLayout(horizontalLayout_17);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        checkBox_AutoPetSupply = new QCheckBox(groupBox_2);
        checkBox_AutoPetSupply->setObjectName(QStringLiteral("checkBox_AutoPetSupply"));

        horizontalLayout_15->addWidget(checkBox_AutoPetSupply);

        spinBox_MinPetHealth = new QSpinBox(groupBox_2);
        spinBox_MinPetHealth->setObjectName(QStringLiteral("spinBox_MinPetHealth"));
        spinBox_MinPetHealth->setMinimum(1);
        spinBox_MinPetHealth->setValue(50);

        horizontalLayout_15->addWidget(spinBox_MinPetHealth);

        comboBox_PetHealthKey = new QComboBox(groupBox_2);
        comboBox_PetHealthKey->setObjectName(QStringLiteral("comboBox_PetHealthKey"));
        comboBox_PetHealthKey->setMaxVisibleItems(10);

        horizontalLayout_15->addWidget(comboBox_PetHealthKey);

        horizontalLayout_15->setStretch(2, 1);

        verticalLayout_2->addLayout(horizontalLayout_15);


        verticalLayout_5->addWidget(groupBox_2);

        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_14->addWidget(label);

        comboBox_Configs = new QComboBox(groupBox);
        comboBox_Configs->setObjectName(QStringLiteral("comboBox_Configs"));

        horizontalLayout_14->addWidget(comboBox_Configs);

        horizontalLayout_14->setStretch(1, 1);

        verticalLayout->addLayout(horizontalLayout_14);

        pushButton_SaveConfigAs = new QPushButton(groupBox);
        pushButton_SaveConfigAs->setObjectName(QStringLiteral("pushButton_SaveConfigAs"));

        verticalLayout->addWidget(pushButton_SaveConfigAs);

        pushButton_RenameConfig = new QPushButton(groupBox);
        pushButton_RenameConfig->setObjectName(QStringLiteral("pushButton_RenameConfig"));

        verticalLayout->addWidget(pushButton_RenameConfig);

        pushButton_DeleteConfig = new QPushButton(groupBox);
        pushButton_DeleteConfig->setObjectName(QStringLiteral("pushButton_DeleteConfig"));

        verticalLayout->addWidget(pushButton_DeleteConfig);


        verticalLayout_5->addWidget(groupBox);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_4 = new QVBoxLayout(tab_2);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        horizontalLayout_19 = new QHBoxLayout();
        horizontalLayout_19->setObjectName(QStringLiteral("horizontalLayout_19"));
        horizontalSpacer_17 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_19->addItem(horizontalSpacer_17);

        label_SampleImage = new QLabel(tab_2);
        label_SampleImage->setObjectName(QStringLiteral("label_SampleImage"));
        sizePolicy.setHeightForWidth(label_SampleImage->sizePolicy().hasHeightForWidth());
        label_SampleImage->setSizePolicy(sizePolicy);
        label_SampleImage->setMinimumSize(QSize(200, 200));
        label_SampleImage->setMaximumSize(QSize(200, 200));
        label_SampleImage->setFrameShape(QFrame::Box);
        label_SampleImage->setFrameShadow(QFrame::Plain);

        horizontalLayout_19->addWidget(label_SampleImage);

        horizontalSpacer_18 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_19->addItem(horizontalSpacer_18);


        verticalLayout_4->addLayout(horizontalLayout_19);

        pushButton_ReadImage = new QPushButton(tab_2);
        pushButton_ReadImage->setObjectName(QStringLiteral("pushButton_ReadImage"));

        verticalLayout_4->addWidget(pushButton_ReadImage);

        pushButton_TestPlayerSupply = new QPushButton(tab_2);
        pushButton_TestPlayerSupply->setObjectName(QStringLiteral("pushButton_TestPlayerSupply"));

        verticalLayout_4->addWidget(pushButton_TestPlayerSupply);

        pushButton_TestPetSupply = new QPushButton(tab_2);
        pushButton_TestPetSupply->setObjectName(QStringLiteral("pushButton_TestPetSupply"));

        verticalLayout_4->addWidget(pushButton_TestPetSupply);

        verticalSpacer = new QSpacerItem(20, 490, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        tabWidget->addTab(tab_2, QString());

        horizontalLayout_18->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralwidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);
        comboBox_PlayerHealthKey->setCurrentIndex(0);
        comboBox_PetHealthKey->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QString());
        groupBox_3->setTitle(QApplication::translate("MainWindow", "\345\220\257\345\212\250\345\214\272", Q_NULLPTR));
        pushButton_UpdateGameWindows->setText(QApplication::translate("MainWindow", "\346\233\264\346\226\260\346\270\270\346\210\217\347\252\227\345\217\243\345\210\227\350\241\250", Q_NULLPTR));
        label_12->setText(QApplication::translate("MainWindow", "\351\200\211\346\213\251\347\252\227\345\217\243", Q_NULLPTR));
        label_WindowTitlePrefix->setText(QApplication::translate("MainWindow", "QQ\350\207\252\347\224\261\345\271\273\346\203\263 - ", Q_NULLPTR));
        pushButton_ChangeWindowTitle->setText(QApplication::translate("MainWindow", "\344\277\256\346\224\271\347\252\227\345\217\243\346\240\207\351\242\230", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "\350\256\276\347\275\256\345\214\272", Q_NULLPTR));
        checkBox_Switch->setText(QApplication::translate("MainWindow", "\345\205\250\345\261\200\345\274\200\345\205\263", Q_NULLPTR));
        checkBox_F1->setText(QApplication::translate("MainWindow", "F1", Q_NULLPTR));
        doubleSpinBox_F1->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F2->setText(QApplication::translate("MainWindow", "F2", Q_NULLPTR));
        doubleSpinBox_F2->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F3->setText(QApplication::translate("MainWindow", "F3", Q_NULLPTR));
        doubleSpinBox_F3->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F4->setText(QApplication::translate("MainWindow", "F4", Q_NULLPTR));
        doubleSpinBox_F4->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F5->setText(QApplication::translate("MainWindow", "F5", Q_NULLPTR));
        doubleSpinBox_F5->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F6->setText(QApplication::translate("MainWindow", "F6", Q_NULLPTR));
        doubleSpinBox_F6->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F7->setText(QApplication::translate("MainWindow", "F7", Q_NULLPTR));
        doubleSpinBox_F7->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F8->setText(QApplication::translate("MainWindow", "F8", Q_NULLPTR));
        doubleSpinBox_F8->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F9->setText(QApplication::translate("MainWindow", "F9", Q_NULLPTR));
        doubleSpinBox_F9->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        checkBox_F10->setText(QApplication::translate("MainWindow", "F10", Q_NULLPTR));
        doubleSpinBox_F10->setSuffix(QApplication::translate("MainWindow", " s", Q_NULLPTR));
        label_PlayerHealth->setText(QString());
        checkBox_AutoPlayerHealth->setText(QApplication::translate("MainWindow", "\350\247\222\350\211\262\350\241\245\347\273\231", Q_NULLPTR));
        spinBox_MinPlayerHealth->setSuffix(QApplication::translate("MainWindow", " %", Q_NULLPTR));
        comboBox_PlayerHealthKey->clear();
        comboBox_PlayerHealthKey->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "F1", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F2", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F3", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F4", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F5", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F6", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F7", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F8", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F9", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F10", Q_NULLPTR)
        );
        label_PetResource->setText(QString());
        checkBox_AutoPetSupply->setText(QApplication::translate("MainWindow", "\345\256\240\347\211\251\350\241\245\347\273\231", Q_NULLPTR));
        spinBox_MinPetHealth->setSuffix(QApplication::translate("MainWindow", " %", Q_NULLPTR));
        comboBox_PetHealthKey->clear();
        comboBox_PetHealthKey->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "F1", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F2", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F3", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F4", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F5", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F6", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F7", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F8", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F9", Q_NULLPTR)
         << QApplication::translate("MainWindow", "F10", Q_NULLPTR)
        );
        groupBox->setTitle(QApplication::translate("MainWindow", "\345\217\202\346\225\260\345\214\272", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "\345\275\223\345\211\215\345\217\202\346\225\260", Q_NULLPTR));
        pushButton_SaveConfigAs->setText(QApplication::translate("MainWindow", "\345\217\246\345\255\230\344\270\272", Q_NULLPTR));
        pushButton_RenameConfig->setText(QApplication::translate("MainWindow", "\351\207\215\345\221\275\345\220\215", Q_NULLPTR));
        pushButton_DeleteConfig->setText(QApplication::translate("MainWindow", "\345\210\240\351\231\244", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "\345\270\270\347\224\250", Q_NULLPTR));
        label_SampleImage->setText(QString());
        pushButton_ReadImage->setText(QApplication::translate("MainWindow", "\350\257\273\345\217\226\345\233\276\347\211\207", Q_NULLPTR));
        pushButton_TestPlayerSupply->setText(QApplication::translate("MainWindow", "\346\265\213\350\257\225\344\272\272\347\211\251\350\241\245\347\273\231", Q_NULLPTR));
        pushButton_TestPetSupply->setText(QApplication::translate("MainWindow", "\346\265\213\350\257\225\345\256\240\347\211\251\350\241\245\347\273\231", Q_NULLPTR));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "\346\265\213\350\257\225", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
