#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QStatusBar>
#include <QPainter>
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

class CharacterBoxDelegate : public QStyledItemDelegate
{
public:
    CharacterBoxDelegate(QObject* parent = nullptr)
        : QStyledItemDelegate(parent) {}

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        auto o = option;
        initStyleOption(&o, index);
        o.decorationSize.setWidth(o.rect.width());
        auto style = o.widget ? o.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &o, painter, o.widget);
    }
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->comboBox_GameWindows->setItemDelegate(new CharacterBoxDelegate);
    setWindowTitle(" ");

    setFixedSize(QSize(width(), height()));
    applyBlankPixmapForPlayer();
    applyBlankPixmapForPet();
    applyBlankPixmapForSample();

    for (int index = 0; index < 10; ++index)
    {
        QCheckBox* checkBox = findChild<QCheckBox *>(QStringLiteral("checkBox_F%1").arg(index + 1));
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox *>(QStringLiteral("doubleSpinBox_F%1").arg(index + 1));

        enumerateControls[index].first = checkBox;
        enumerateControls[index].second = spinBox;

        connect(checkBox, &QCheckBox::toggled, this, &MainWindow::on_any_Fx_checkBox_toggled);
    }

    connect(&pressTimer, &QTimer::timeout, this, &MainWindow::pressTimerProc);
    connect(&supplyTimer, &QTimer::timeout, this, &MainWindow::supplyTimerProc);

    pressTimer.setTimerType(Qt::PreciseTimer);
    supplyTimer.setTimerType(Qt::PreciseTimer);
    pressTimer.start(50);
    supplyTimer.start(900);

    scanConfigs();

    //-----------------------------------------------------------------------------------------------------------
    QueryPerformanceFrequency(&counterFrequency);

    //抄的代码..
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
    tp.PrivilegeCount = 1;
    LookupPrivilegeValueW(nullptr, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr);
    CloseHandle(hToken);

    //-----------------------------------------------------------------------------------------------------------
}

MainWindow::~MainWindow()
{
    pressTimer.stop();
    supplyTimer.stop();
    autoWriteConfig();

    delete ui;
}

void MainWindow::on_pushButton_UpdateGameWindows_clicked()
{
    updateGameWindows();
}

void MainWindow::pressTimerProc()
{
    LARGE_INTEGER currentCounter;

    if (!ui->checkBox_Switch->isChecked())
    {
        return;
    }

    int window_index = ui->comboBox_GameWindows->currentIndex();

    if (window_index == -1)
    {
        return;
    }

    QueryPerformanceCounter(&currentCounter);

    for (int key_index = 0; key_index < 10; ++key_index)
    {
        if (!enumerateControls[key_index].first->isChecked())
        {
            continue;
        }

        double differ = getCountersDiffInSeconds(timeStamps[key_index], currentCounter);
        double value = enumerateControls[key_index].second->value();
        if (differ >= value)
        {
            timeStamps[key_index].QuadPart += static_cast<LONGLONG>(enumerateControls[key_index].second->value() * counterFrequency.QuadPart);
            pressFunctionKey(gameWindows[window_index], VK_F1 + key_index);
        }
    }
}

void MainWindow::supplyTimerProc()
{
    if (!ui->checkBox_Switch->isChecked())
    {
        return;
    }

    int window_index = ui->comboBox_GameWindows->currentIndex();

    if (window_index == -1)
    {
        return;
    }

    if (ui->checkBox_AutoPlayerHealth->isChecked())
    {
        int key_index = ui->comboBox_PlayerHealthKey->currentIndex();

        if (key_index != -1)
        {
            QImage healthPicture = getGamePicture(gameWindows[window_index], playerHealthRect);

            if (!healthPicture.isNull())
            {
                QPoint samplePoint = getPlayerHealthSamplePoint(healthPicture, ui->spinBox_MinPlayerHealth->value());

                if (samplePoint.x() != -1 && samplePoint.y() != -1)
                {
                    if (isPlayerLowHealth(healthPicture.pixel(samplePoint)))
                    {
                        pressFunctionKey(gameWindows[window_index], VK_F1 + key_index);
                    }
                }

                ui->label_PlayerHealth->setPixmap(QPixmap::fromImage(healthPicture));
            }
        }
    }

    if (ui->checkBox_AutoPetSupply->isChecked())
    {
        int key_index = ui->comboBox_PetHealthKey->currentIndex();

        if (key_index != -1)
        {
            QImage healthPicture = getGamePicture(gameWindows[window_index], petResourceRect);

            if (!healthPicture.isNull())
            {
                QPair<QPoint, QPoint> samplePoints = getPetResourceSamplePoints(healthPicture, ui->spinBox_MinPetHealth->value());

                if (samplePoints.first.x() != -1 && samplePoints.first.y() != -1)
                {
                    if (isPetLowResource(healthPicture.pixel(samplePoints.first)) || isPetLowResource(healthPicture.pixel(samplePoints.second)))
                    {
                        pressFunctionKey(gameWindows[window_index], VK_F1 + key_index);
                    }
                }

                ui->label_PlayerHealth->setPixmap(QPixmap::fromImage(healthPicture));
            }
        }
    }
}

void MainWindow::resetTimeStamp(int index)
{
    QueryPerformanceCounter(&timeStamps[index]);
}

void MainWindow::resetAllTimeStamps()
{
    LARGE_INTEGER currentCounter;

    QueryPerformanceCounter(&currentCounter);
    timeStamps.fill(currentCounter);
}

double MainWindow::getCountersDiffInSeconds(LARGE_INTEGER past, LARGE_INTEGER now)
{
    return (double)(now.QuadPart - past.QuadPart) / counterFrequency.QuadPart;
}

void MainWindow::updateGameWindows()
{
    char c_string[512];

    int found = 0, invalid = 0;

    gameWindows.clear();
    ui->comboBox_GameWindows->clear();
    ui->checkBox_Switch->setChecked(false);

    HWND hWindow = FindWindowEx(nullptr, nullptr, nullptr, nullptr);

    while (hWindow != nullptr)
    {
        GetWindowTextA(hWindow, c_string, 512);

        if (strstr(c_string, "QQ自由幻想") != nullptr) //此处必须判断标题，qqffo.exe有很多窗口
        {
            DWORD pid;
            GetWindowThreadProcessId(hWindow, &pid);
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
            GetProcessImageFileNameA(hProcess, c_string, 512);
            CloseHandle(hProcess);

            std::string cppStr = c_string;
            if (cppStr.rfind("\\qqffo.exe") == (cppStr.length() - 10))
            {
                QImage characterPicture = getGamePicture(hWindow, characterNameRect);

                if (!characterPicture.isNull())
                {
                    ++found;
                    gameWindows.push_back(hWindow);
                    ui->comboBox_GameWindows->addItem(QIcon(QPixmap::fromImage(characterPicture)), nullptr);
                }
                else
                {
                    ++invalid;
                }
            }
        }

        hWindow = FindWindowEx(nullptr, hWindow, nullptr, nullptr);
    }

    QString summary = QStringLiteral("共找到 %1 个游戏窗口，其中成功 %2 个，失败 %3 个。").arg(found + invalid).arg(found).arg(invalid);
    QMessageBox::information(this, QStringLiteral("摘要"), summary);
}

void MainWindow::pressFunctionKey(HWND window, UINT code)
{
    if (IsWindow(window) == TRUE)
    {
        PostMessageA(window, WM_KEYDOWN, code, 0);
        PostMessageA(window, WM_KEYUP, code, 0);
    }
}

QImage MainWindow::getGamePicture(HWND window, QRect rect)
{
    std::vector<uchar> pixelBuffer;
    QImage result;

    //抄的代码..
    HBITMAP hBitmap, hOld;
    HDC hDC, hcDC;
    BITMAPINFO b;

    if ((IsWindow(window) == FALSE) || (IsIconic(window) == TRUE))
        return QImage();

    b.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    b.bmiHeader.biWidth = rect.width();
    b.bmiHeader.biHeight = rect.height();
    b.bmiHeader.biPlanes = 1;
    b.bmiHeader.biBitCount = 3 * 8;
    b.bmiHeader.biCompression = BI_RGB;
    b.bmiHeader.biSizeImage = 0;
    b.bmiHeader.biXPelsPerMeter = 0;
    b.bmiHeader.biYPelsPerMeter = 0;
    b.bmiHeader.biClrUsed = 0;
    b.bmiHeader.biClrImportant = 0;
    b.bmiColors[0].rgbBlue = 8;
    b.bmiColors[0].rgbGreen = 8;
    b.bmiColors[0].rgbRed = 8;
    b.bmiColors[0].rgbReserved = 0;

    hDC = GetDC(window);
    hcDC = CreateCompatibleDC(hDC);
    hBitmap = CreateCompatibleBitmap(hDC, rect.width(), rect.height());
    hOld = (HBITMAP)SelectObject(hcDC, hBitmap);

    BitBlt(hcDC, 0, 0, rect.width(), rect.height(), hDC, rect.left(), rect.top(), SRCCOPY);
    pixelBuffer.resize(rect.width() * rect.height() * 4);
    GetDIBits(hcDC, hBitmap, 0, rect.height(), pixelBuffer.data(), &b, DIB_RGB_COLORS);
    ReleaseDC(window, hDC);
    DeleteDC(hcDC);
    DeleteObject(hBitmap);
    DeleteObject(hOld);

    return QImage(pixelBuffer.data(), rect.width(), rect.height(), (rect.width() * 3 + 3) & (~3), QImage::Format_RGB888).rgbSwapped().mirrored();
}

void MainWindow::applyBlankPixmapForPlayer()
{
    QPixmap playerHealthPixmap(playerHealthRect.size() * 2);
    playerHealthPixmap.fill(Qt::black);
    ui->label_PlayerHealth->setPixmap(playerHealthPixmap);
}

void MainWindow::applyBlankPixmapForPet()
{
    QPixmap petResourcePixmap(petResourceRect.size() * 2);
    petResourcePixmap.fill(Qt::black);
    ui->label_PetResource->setPixmap(petResourcePixmap);
}

void MainWindow::applyBlankPixmapForSample()
{
    QPixmap samplePixmap(ui->label_SampleImage->size());
    samplePixmap.fill(Qt::black);
    ui->label_SampleImage->setPixmap(samplePixmap);
}

QPoint MainWindow::getPlayerHealthSamplePoint(QImage image, int percent)
{
    if (image.isNull() || image.size() != playerHealthRect.size())
    {
        return QPoint(-1, -1);
    }

    return QPoint(
        static_cast<int>(ceil(playerHealthRect.width() * (percent / 100.0f))),
        playerHealthRect.height() / 2);
}

QPair<QPoint, QPoint> MainWindow::getPetResourceSamplePoints(QImage image, int percent)
{
    //从x求y
    static const int pet_mana_y_table[16] =
    {
        16,11,9,7,6,5,4,3,
        2,2,1,1,1,0,0,0
    };

    if (image.isNull() || image.size() != petResourceRect.size())
    {
        return qMakePair(QPoint(-1, -1), QPoint(-1, -1));
    }

    int mana_sample_x = static_cast<int>(ceil(percent / 100.0f * 15));

    QPoint healthPoint(18 + mana_sample_x, pet_mana_y_table[15 - mana_sample_x]); //x: 33-18

    QPoint manaPoint(mana_sample_x, pet_mana_y_table[mana_sample_x]); //x: 0-15

    return qMakePair(healthPoint, manaPoint);
}

std::array<float, 3> MainWindow::normalizePixel(QRgb pixel)
{
    float fRed = static_cast<float>(qRed(pixel));
    float fGreen = static_cast<float>(qGreen(pixel));
    float fBlue = static_cast<float>(qBlue(pixel));
    float sum = fRed + fGreen + fBlue;

    return std::array<float, 3>{fRed / sum, fGreen / sum, fBlue / sum};
}

bool MainWindow::isPetLowResource(QRgb pixel)
{
    //验证游戏渐黑时的效果
    std::array<float, 3> normalized = normalizePixel(pixel);

    //有一个分量大于40%，即有血条/蓝条覆盖
    //无覆盖时接近1:1:1
    return std::count_if(normalized.begin(), normalized.end(), [](float val) {return val > 0.4f; }) == 0;
}

bool MainWindow::isPlayerLowHealth(QRgb pixel)
{
    //验证游戏渐黑时的效果
    //绿/黄/红/虚血/空血
    std::array<float, 3> normalized = normalizePixel(pixel);



    return false;
}

void MainWindow::scanConfigs()
{
    ui->comboBox_Configs->clear();

    QDir dir = QCoreApplication::applicationDirPath();
    dir.mkdir(QStringLiteral("config"));
    dir.cd(QStringLiteral("config"));

    QStringList names = dir.entryList(QStringList(QStringLiteral("*.json")), QDir::Files).replaceInStrings(QRegularExpression(QStringLiteral("\\.json$")), QStringLiteral(""));
    ui->comboBox_Configs->addItems(names);
    loadConfig(ui->comboBox_Configs->currentText());
}

QString MainWindow::getConfigPath(const QString &name)
{
    return (QCoreApplication::applicationDirPath() + "/config/%1.json").arg(name);
}

SConfigData MainWindow::readConfig(const QString & filename)
{
    QFile file;
    QJsonDocument doc;
    QJsonObject root;

    file.setFileName(filename);
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        return SConfigData();
    }

    doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull())
    {
        return SConfigData();
    }

    return jsonToConfig(doc.object());
}

void MainWindow::writeConfig(const QString & filename, const SConfigData &config)
{
    QFile file;
    QJsonObject root;
    QJsonDocument doc;

    file.setFileName(filename);
    if (!file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return;
    }

    root = configToJson(config);
    doc.setObject(root);
    file.write(doc.toJson(QJsonDocument::Indented));
}

void MainWindow::loadConfig(const QString & name)
{
    if (name.isEmpty())
    {
        applyDefaultConfigToUI();
    }
    else
    {
        applyConfigToUI(readConfig(getConfigPath(name)));
    }
}

void MainWindow::autoWriteConfig()
{
    if (!currentConfigName.isEmpty())
    {
        writeConfig(getConfigPath(currentConfigName), makeConfigFromUI());
    }
}

SConfigData MainWindow::makeConfigFromUI()
{
    SConfigData result;

    for (int index = 0; index < 10; ++index)
    {
        result.presses[index].first = enumerateControls[index].first->isChecked();
        result.presses[index].second = enumerateControls[index].second->value();
    }

    std::get<0>(result.playerSupply) = ui->checkBox_AutoPlayerHealth->isChecked();
    std::get<1>(result.playerSupply) = ui->spinBox_MinPlayerHealth->value();
    std::get<2>(result.playerSupply) = ui->comboBox_PlayerHealthKey->currentIndex();

    std::get<0>(result.petSupply) = ui->checkBox_AutoPetSupply->isChecked();
    std::get<1>(result.petSupply) = ui->spinBox_MinPetHealth->value();
    std::get<2>(result.petSupply) = ui->comboBox_PetHealthKey->currentIndex();

    result.title = ui->lineEdit_WindowTitle->text();

    return result;
}

void MainWindow::applyConfigToUI(const SConfigData &config)
{
    for (int index = 0; index < 10; ++index)
    {
        enumerateControls[index].first->setChecked(config.presses[index].first);
        enumerateControls[index].second->setValue(config.presses[index].second);
    }

    ui->checkBox_AutoPlayerHealth->setChecked(std::get<0>(config.playerSupply));
    ui->spinBox_MinPlayerHealth->setValue(std::get<1>(config.playerSupply));
    ui->comboBox_PlayerHealthKey->setCurrentIndex(std::get<2>(config.playerSupply));

    ui->checkBox_AutoPetSupply->setChecked(std::get<0>(config.petSupply));
    ui->spinBox_MinPetHealth->setValue(std::get<1>(config.petSupply));
    ui->comboBox_PetHealthKey->setCurrentIndex(std::get<2>(config.petSupply));

    ui->lineEdit_WindowTitle->setText(config.title);
}

void MainWindow::applyDefaultConfigToUI()
{
    applyConfigToUI(SConfigData());
}

QJsonObject MainWindow::configToJson(const SConfigData &config)
{
    QJsonObject result;
    QJsonArray pressArray;
    QJsonObject supplyObject;

    for (int index = 0; index < 10; index++)
    {
        QJsonObject keyObject;
        keyObject[QStringLiteral("Enabled")] = config.presses[index].first;
        keyObject[QStringLiteral("Interval")] = config.presses[index].second;
        pressArray.append(keyObject);
    }
    result[QStringLiteral("AutoPress")] = pressArray;

    supplyObject[QStringLiteral("Enabled")] = std::get<0>(config.playerSupply);
    supplyObject[QStringLiteral("Percent")] = std::get<1>(config.playerSupply);
    supplyObject[QStringLiteral("Key")] = std::get<2>(config.playerSupply);
    result["AutoPlayerSupply"] = supplyObject;

    supplyObject[QStringLiteral("Enabled")] = std::get<0>(config.petSupply);
    supplyObject[QStringLiteral("Percent")] = std::get<1>(config.petSupply);
    supplyObject[QStringLiteral("Key")] = std::get<2>(config.petSupply);
    result["AutoPetSupply"] = supplyObject;

    result["Title"] = config.title;

    return result;
}

SConfigData MainWindow::jsonToConfig(QJsonObject json)
{
    SConfigData result;
    QJsonArray pressArray;
    QJsonObject supplyObject;

    pressArray = json.take(QStringLiteral("AutoPress")).toArray();

    if (pressArray.size() != 10)
    {
        result.presses.fill(std::make_pair(false, 1.0));
    }
    else
    {
        for (int index = 0; index < 10; index++)
        {
            QJsonObject keyObject = pressArray[index].toObject();
            result.presses[index].first = keyObject.take(QStringLiteral("Enabled")).toBool(false);
            result.presses[index].second = keyObject.take(QStringLiteral("Interval")).toDouble(1.0);
        }
    }

    supplyObject = json.take(QStringLiteral("AutoPlayerSupply")).toObject();
    std::get<0>(result.playerSupply) = supplyObject.take(QStringLiteral("Enabled")).toBool(false);
    std::get<1>(result.playerSupply) = supplyObject.take(QStringLiteral("Percent")).toInt(50);
    std::get<2>(result.playerSupply) = supplyObject.take(QStringLiteral("Key")).toInt(0);

    supplyObject = json.take(QStringLiteral("AutoPetSupply")).toObject();
    std::get<0>(result.petSupply) = supplyObject.take(QStringLiteral("Enabled")).toBool(false);
    std::get<1>(result.petSupply) = supplyObject.take(QStringLiteral("Percent")).toInt(50);
    std::get<2>(result.petSupply) = supplyObject.take(QStringLiteral("Key")).toInt(0);

    result.title = json.take("Title").toString("");

    return result;
}

void MainWindow::on_any_Fx_checkBox_toggled(bool checked)
{
    QObject *control = sender();

    auto index = std::find_if(enumerateControls.begin(), enumerateControls.end(),
        [control](const std::pair<QCheckBox *, QDoubleSpinBox *> &p)
    {return p.first == control; })
        - enumerateControls.begin();

    enumerateControls[index].second->setEnabled(!checked);
    resetTimeStamp(index);
}

void MainWindow::on_checkBox_Switch_toggled(bool checked)
{
    if (checked)
    {
        resetAllTimeStamps();
    }
}

void MainWindow::on_checkBox_AutoPlayerHealth_toggled(bool checked)
{
    ui->spinBox_MinPlayerHealth->setEnabled(!checked);
    ui->comboBox_PlayerHealthKey->setEnabled(!checked);
    if (!checked)
    {
        applyBlankPixmapForPlayer();
    }

}

void MainWindow::on_checkBox_AutoPetSupply_toggled(bool checked)
{
    ui->spinBox_MinPetHealth->setEnabled(!checked);
    ui->comboBox_PetHealthKey->setEnabled(!checked);
    if (!checked)
    {
        applyBlankPixmapForPet();
    }
}

void MainWindow::on_comboBox_GameWindows_currentIndexChanged(int index)
{
    ui->checkBox_Switch->setChecked(false);
}

void MainWindow::on_pushButton_SaveConfigAs_clicked()
{
    QString newName = QInputDialog::getText(this, QStringLiteral("另存为"), QStringLiteral("新的名字: "));

    if (!newName.isEmpty())
    {
        writeConfig(getConfigPath(newName), makeConfigFromUI());

        if (ui->comboBox_Configs->findText(newName, Qt::MatchFixedString) == -1)
        {
            ui->comboBox_Configs->addItem(newName);
        }
    }
}

void MainWindow::on_pushButton_RenameConfig_clicked()
{
    QString oldName = ui->comboBox_Configs->currentText();

    if (!oldName.isEmpty())
    {
        QString newName = QInputDialog::getText(this, QStringLiteral("重命名"), QStringLiteral("新的名字: "));

        if (!newName.isEmpty())
        {
            QFile::rename(getConfigPath(oldName), getConfigPath(newName));
            ui->comboBox_Configs->setItemText(ui->comboBox_Configs->currentIndex(), newName);
        }
    }
}

void MainWindow::on_comboBox_Configs_currentIndexChanged(int index)
{
    autoWriteConfig();

    ui->checkBox_Switch->setChecked(false);

    if (index == -1)
    {
        applyDefaultConfigToUI();
    }
    else
    {
        QString name = ui->comboBox_Configs->itemText(index);
        loadConfig(name);

        currentConfigName = name;
    }
}

void MainWindow::on_pushButton_DeleteConfig_clicked()
{
    QString name = ui->comboBox_Configs->currentText();

    if (!name.isEmpty() && QMessageBox::question(this, QStringLiteral("删除"), QStringLiteral("是否真的要删除参数 '%1' ？").arg(name), QStringLiteral("确定"), QStringLiteral("取消")) == 0)
    {
        QFile::remove(getConfigPath(name));
        currentConfigName.clear();
        ui->comboBox_Configs->removeItem(ui->comboBox_Configs->currentIndex());
    }
}

void MainWindow::on_pushButton_ReadImage_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, QStringLiteral("选择一张图片"), QString(), QStringLiteral("Images(*.png *.bmp *.jpg)"));

    if (!filename.isEmpty())
    {
        if (!sampleImage.load(filename) || sampleImage.width() < ui->label_SampleImage->width() || sampleImage.height() < ui->label_SampleImage->height())
        {
            return;
        }

        sampleImage = sampleImage.copy(ui->label_SampleImage->rect());

        if (sampleImage.format() != QImage::Format_RGB888)
        {
            sampleImage = sampleImage.convertToFormat(QImage::Format_RGB888); //看看是否需要这句
        }

        ui->label_SampleImage->setPixmap(QPixmap::fromImage(sampleImage));
    }
}

void MainWindow::on_pushButton_TestPlayerSupply_clicked()
{
    if (sampleImage.isNull())
    {
        return;
    }

    QImage healthPicture = sampleImage.copy(playerHealthRect);

    QPoint samplePoint = getPlayerHealthSamplePoint(healthPicture, ui->spinBox_MinPlayerHealth->value());

    if (samplePoint.x() != -1 && samplePoint.y() != -1)
    {
        QPainter painter(&healthPicture);
        painter.setPen(Qt::white);
        painter.drawLine(QPoint(samplePoint.x(), 0), QPoint(samplePoint.x(), healthPicture.height()));
        if (isPlayerLowHealth(healthPicture.pixel(samplePoint)))
        {
            statusBar()->showMessage(QStringLiteral("人物血量低"));
        }
        else
        {
            statusBar()->showMessage(QStringLiteral("人物血量不低"));
        }
    }

    ui->label_PlayerHealth->setPixmap(QPixmap::fromImage(healthPicture));
}

void MainWindow::on_pushButton_TestPetSupply_clicked()
{
    if (sampleImage.isNull())
    {
        return;
    }

    QImage healthPicture = sampleImage.copy(petResourceRect);

    QPair<QPoint, QPoint> samplePoints = getPetResourceSamplePoints(healthPicture, ui->spinBox_MinPetHealth->value());

    if (samplePoints.first.x() != -1 && samplePoints.first.y() != -1)
    {
        QPainter painter(&healthPicture);
        painter.setPen(Qt::white);
        painter.drawLine(QPoint(samplePoints.first.x(), 0), QPoint(samplePoints.first.x(), healthPicture.height()));
        painter.drawLine(QPoint(samplePoints.second.x(), 0), QPoint(samplePoints.second.x(), healthPicture.height()));

        if (isPetLowResource(healthPicture.pixel(samplePoints.first)) || isPetLowResource(healthPicture.pixel(samplePoints.second)))
        {
            statusBar()->showMessage(QStringLiteral("宠物血量低"));
        }
        else
        {
            statusBar()->showMessage(QStringLiteral("宠物血量不低"));
        }
    }

    ui->label_PetResource->setPixmap(QPixmap::fromImage(healthPicture));
}

void MainWindow::on_pushButton_ChangeWindowTitle_clicked()
{
    int window_index = ui->comboBox_GameWindows->currentIndex();

    if (window_index == -1)
    {
        return;
    }

    QString text = ui->lineEdit_WindowTitle->text();

    if (text.isEmpty())
    {
        return;
    }

    SetWindowTextW(gameWindows[window_index], QStringLiteral("%1%2").arg(ui->label_WindowTitlePrefix->text()).arg(text).toStdWString().c_str());
}
