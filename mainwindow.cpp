#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QSettings>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

//角色名取样区域
static const QRect playerNameRect{ 80,22,90,14 };
//人物血条取样区域
static const QRect playerHealthRect{ 81,38,89,7 };
//宠物血条蓝条取样区域
static const QRect petResourceRect{ 21,103,34,34 };

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

    applyBlankPixmapForPlayerName();
    applyBlankPixmapForPlayerHealth();
    applyBlankPixmapForPetResource();

    readWindowPos();

    for (int index = 0; index < 10; ++index)
    {
        QCheckBox* checkBox = findChild<QCheckBox *>(QStringLiteral("checkBox_F%1").arg(index + 1));
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox *>(QStringLiteral("doubleSpinBox_F%1").arg(index + 1));

        enumerateControls[index].first = checkBox;
        enumerateControls[index].second = spinBox;

        connect(checkBox, &QCheckBox::toggled, this, &MainWindow::on_any_Fx_checkBox_toggled);
    }

    connect(&pressTimer, &QTimer::timeout, this, &MainWindow::pressProc);
    connect(&supplyTimer, &QTimer::timeout, this, &MainWindow::supplyProc);

    QDir dir = QCoreApplication::applicationDirPath();
    dir.mkdir(QStringLiteral("screenshot"));
    dir.mkdir(QStringLiteral("config"));

    scanConfigs();

    //-----------------------------------------------------------------------------------------------------------
    //抄的代码..获取读进程信息的权限
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
    tp.PrivilegeCount = 1;
    LookupPrivilegeValueW(nullptr, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), nullptr, nullptr);
    CloseHandle(hToken);
    //-----------------------------------------------------------------------------------------------------------

    pressTimer.setTimerType(Qt::PreciseTimer);
    pressTimer.start(50);

    supplyTimer.setTimerType(Qt::PreciseTimer);
    supplyTimer.start(1000);
}

MainWindow::~MainWindow()
{
    pressTimer.stop();
    supplyTimer.stop();

    autoWriteConfig();
    writeWindowPos();

    delete ui;
}

void MainWindow::on_pushButton_UpdateGameWindows_clicked()
{
    updateGameWindows();
}

void MainWindow::pressProc()
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

    for (int key_index = 0; key_index < 10; ++key_index)
    {
        if (!enumerateControls[key_index].first->isChecked())
        {
            continue;
        }

        auto nowTimePoint = std::chrono::steady_clock::now();

        std::chrono::milliseconds differFromSelf = std::chrono::duration_cast<std::chrono::milliseconds>(nowTimePoint - lastPressedTimePoint[key_index]);
        std::chrono::milliseconds differFromAny = std::chrono::duration_cast<std::chrono::milliseconds>(nowTimePoint - lastAnyPressedTimePoint);
        std::chrono::milliseconds selfInterval(static_cast<long long>(enumerateControls[key_index].second->value() * 1000));
        std::chrono::milliseconds anyInterval(static_cast<long long>(ui->doubleSpinBox_FxInterval->value() * 1000));

        if (differFromSelf >= selfInterval && differFromAny >= anyInterval)
        {
            lastPressedTimePoint[key_index] += selfInterval;
            lastAnyPressedTimePoint = nowTimePoint;

            pressKey(gameWindows[window_index], VK_F1 + key_index);
        }
    }
}

void MainWindow::supplyProc()
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

    if (ui->checkBox_AutoPlayerSupply->isChecked())
    {
        int key_index = ui->comboBox_PlayerHealthKey->currentIndex();

        if (key_index != -1)
        {
            QImage healthPicture = getGamePicture(gameWindows[window_index], playerHealthRect);

            if (isPlayerLowHealth(healthPicture, ui->spinBox_MinPlayerHealth->value()))
            {
                pressKey(gameWindows[window_index], VK_F1 + key_index);
            }

            ui->label_PlayerHealth->setPixmap(QPixmap::fromImage(healthPicture));
        }
    }

    if (ui->checkBox_AutoPetSupply->isChecked())
    {
        int key_index = ui->comboBox_PetHealthKey->currentIndex();

        if (key_index != -1)
        {
            QImage healthPicture = getGamePicture(gameWindows[window_index], petResourceRect);

            if (isPetLowResource(healthPicture, ui->spinBox_MinPetResource->value()))
            {
                pressKey(gameWindows[window_index], VK_F1 + key_index);
            }

            ui->label_PetResource->setPixmap(QPixmap::fromImage(healthPicture));
        }
    }
}

void MainWindow::resetTimeStamp(int index)
{
    lastPressedTimePoint[index] = std::chrono::steady_clock::now();
}

void MainWindow::resetAllTimeStamps()
{
    lastPressedTimePoint.fill(std::chrono::steady_clock::now());
    lastAnyPressedTimePoint = std::chrono::steady_clock::time_point();
}

void MainWindow::updateGameWindows()
{
    wchar_t c_string[512];

    int found = 0, invalid = 0;

    gameWindows.clear();
    ui->comboBox_GameWindows->clear();
    ui->checkBox_Switch->setChecked(false);

    HWND hWindow = FindWindow(nullptr, nullptr);

    while (hWindow != nullptr)
    {
        GetWindowTextW(hWindow, c_string, 512);

        if (QString::fromWCharArray(c_string).startsWith(QStringLiteral("QQ自由幻想"))) //此处必须判断标题，qqffo.exe有很多窗口
        {
            DWORD pid;
            GetWindowThreadProcessId(hWindow, &pid);
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
            GetProcessImageFileNameW(hProcess, c_string, 512);
            CloseHandle(hProcess);

            if (QString::fromWCharArray(c_string).endsWith(QStringLiteral("\\qqffo.exe")))
            {
                QImage playerNameImage = getGamePicture(hWindow, playerNameRect);

                if (!playerNameImage.isNull())
                {
                    ++found;
                    gameWindows.push_back(hWindow);
                    QPixmap playerNamePix = QPixmap::fromImage(playerNameImage);
                    playerNamePixmaps.push_back(playerNamePix);
                    ui->comboBox_GameWindows->addItem(QIcon(playerNamePix), nullptr);

                }
                else
                {
                    ++invalid;
                }
            }
        }

        hWindow = FindWindowEx(nullptr, hWindow, nullptr, nullptr);
    }

    if (found + invalid == 0)
    {
        QMessageBox::information(this, QStringLiteral("摘要"), QStringLiteral("没有找到游戏窗口。"));
    }
    else if (invalid != 0)
    {
        QString summary = QStringLiteral("共找到 %1 个游戏窗口，其中成功 %2 个，失败 %3 个。").arg(found + invalid).arg(found).arg(invalid);
        QMessageBox::information(this, QStringLiteral("摘要"), summary);
    }
}

void MainWindow::pressKey(HWND window, UINT code)
{
    PostMessageA(window, WM_KEYDOWN, code, 0);
    PostMessageA(window, WM_KEYUP, code, 0);
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

void MainWindow::scanConfigs()
{
    ui->comboBox_Configs->clear();

    QDir dir = QCoreApplication::applicationDirPath();
    dir.cd(QStringLiteral("config"));

    QStringList names = dir.entryList(QStringList(QStringLiteral("*.json")), QDir::Files).replaceInStrings(QRegularExpression(QStringLiteral("\\.json$")), QStringLiteral(""));
    ui->comboBox_Configs->addItems(names);
    loadConfig(ui->comboBox_Configs->currentText());
}

QString MainWindow::getConfigPath(const QString &name)
{
    return (QCoreApplication::applicationDirPath() + "/config/%1.json").arg(name);
}

QString MainWindow::getScreenShotPath()
{
    QString filename = QDateTime::currentDateTime().toString("yyyy-MM-dd HH-mm-ss-zzz.png");
    return QCoreApplication::applicationDirPath() + "/screenshot/" + filename;
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

    result.title = ui->lineEdit_WindowTitle->text();

    for (int index = 0; index < 10; ++index)
    {
        result.fxSwitch[index] = enumerateControls[index].first->isChecked();
        result.fxCD[index] = enumerateControls[index].second->value();
    }
    result.fxInterval = ui->doubleSpinBox_FxInterval->value();

    result.playerSwitch = ui->checkBox_AutoPlayerSupply->isChecked();
    result.playerPercent = ui->spinBox_MinPlayerHealth->value();
    result.playerKey = ui->comboBox_PlayerHealthKey->currentIndex();

    result.petSwitch = ui->checkBox_AutoPetSupply->isChecked();
    result.petPercent = ui->spinBox_MinPetResource->value();
    result.petKey = ui->comboBox_PetHealthKey->currentIndex();

    return result;
}

void MainWindow::applyConfigToUI(const SConfigData &config)
{
    ui->lineEdit_WindowTitle->setText(config.title);

    for (int index = 0; index < 10; ++index)
    {
        enumerateControls[index].first->setChecked(config.fxSwitch[index]);
        enumerateControls[index].second->setValue(config.fxCD[index]);
    }
    ui->doubleSpinBox_FxInterval->setValue(config.fxInterval);

    ui->checkBox_AutoPlayerSupply->setChecked(config.playerSwitch);
    ui->spinBox_MinPlayerHealth->setValue(config.playerPercent);
    ui->comboBox_PlayerHealthKey->setCurrentIndex(config.playerKey);

    ui->checkBox_AutoPetSupply->setChecked(config.petSwitch);
    ui->spinBox_MinPetResource->setValue(config.petPercent);
    ui->comboBox_PetHealthKey->setCurrentIndex(config.petKey);
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
        keyObject[QStringLiteral("Enabled")] = config.fxSwitch[index];
        keyObject[QStringLiteral("Interval")] = config.fxCD[index];
        pressArray.append(keyObject);
    }
    result[QStringLiteral("AutoPress")] = pressArray;

    result["Title"] = config.title;
    result["Interval"] = config.fxInterval;

    result["PlayerSwitch"] = config.playerSwitch;
    result["PlayerPercent"] = config.playerPercent;
    result["PlayerKey"] = config.playerKey;

    result["PetSwitch"] = config.petSwitch;
    result["PetPercent"] = config.petPercent;
    result["PetKey"] = config.petKey;

    return result;
}

SConfigData MainWindow::jsonToConfig(QJsonObject json)
{
    SConfigData result;
    QJsonArray pressArray;
    QJsonObject supplyObject;

    pressArray = json.take(QStringLiteral("AutoPress")).toArray();

    if (pressArray.size() == 10)
    {
        for (int index = 0; index < 10; index++)
        {
            QJsonObject keyObject = pressArray[index].toObject();
            result.fxSwitch[index] = keyObject.take(QStringLiteral("Enabled")).toBool(false);
            result.fxCD[index] = keyObject.take(QStringLiteral("Interval")).toDouble(1.0);
        }
    }

    result.title = json.take("Title").toString("");
    result.fxInterval = json.take("Interval").toDouble(0.7);

    result.playerSwitch = json.take("PlayerSwitch").toBool(false);
    result.playerPercent = json.take("PlayerPercent").toInt(50);
    result.playerKey = json.take("PlayerKey").toInt(8);

    result.petSwitch = json.take("PetSwitch").toBool(false);
    result.petPercent = json.take("PetPercent").toInt(50);
    result.petKey = json.take("PetKey").toInt(9);

    return result;
}

void MainWindow::writeWindowPos()
{
    QSettings settings("CC", "FxPresser");

    auto rect = geometry();

    settings.setValue("X", rect.x());
    settings.setValue("Y", rect.y());
}

void MainWindow::readWindowPos()
{
    QSettings settings("CC", "FxPresser");

    auto rect = geometry();

    int x = settings.value("X", 100).toInt();
    int y = settings.value("Y", 100).toInt();

    setGeometry(x, y, rect.width(), rect.height());
}

std::array<float, 3> MainWindow::rgb2HSV(QRgb rgbColor)
{
    std::array<float, 3> result;

    //opencv->color.cpp->RGB2HSV_f::operator()
    float b = qBlue(rgbColor), g = qGreen(rgbColor), r = qRed(rgbColor);
    float h, s, v;

    float vmin, diff;

    v = vmin = r;
    if (v < g) v = g;
    if (v < b) v = b;
    if (vmin > g) vmin = g;
    if (vmin > b) vmin = b;

    diff = v - vmin;
    s = diff / (float)(fabs(v) + FLT_EPSILON);
    diff = (float)(60. / (diff + FLT_EPSILON));
    if (v == r)
        h = (g - b)*diff;
    else if (v == g)
        h = (b - r)*diff + 120.f;
    else
        h = (r - g)*diff + 240.f;

    if (h < 0) h += 360.f;

    result[0] = h;
    result[1] = s;
    result[2] = v;

    return result;
}

bool MainWindow::isPixelPetLowResource(QRgb pixel)
{
    std::array<float, 3> normalized = normalizePixel(pixel);

    //有一个分量大于40%，即有血条/蓝条覆盖
    //无覆盖时接近1:1:1
    return std::count_if(normalized.begin(), normalized.end(), [](float val) {return val > 0.4f; }) == 0;
}

bool MainWindow::isPixelPlayerLowHealth(QRgb pixel)
{
    auto fp_equal = [](float v1, float v2) {return fabs(v1 - v2) < 0.01f; };

    //绿/黄/红
    //绿血: H126.67 S0.90  V170.00
    //黄血:  H36.92 S0.87  V255.00
    //红血:  H13.85 S1.00  V221.00

    std::array<float, 3> hsvPix = rgb2HSV(pixel);

    float hVal = hsvPix[0];
    float sVal = hsvPix[1];

    //不是三种血条颜色之一则为需要补给
    return !(
        (fp_equal(hVal, 126.67f) && fp_equal(sVal, 0.90f)) ||
        (fp_equal(hVal, 36.92f) && fp_equal(sVal, 0.87f)) ||
        (fp_equal(hVal, 13.85f) && fp_equal(sVal, 1.00f)));
}

bool MainWindow::isPlayerLowHealth(QImage &sample, int precent)
{
    DEFINE_IMAGE_ADAPTER(sample);

    if (sample.isNull())
    {
        return false;
    }

    QPoint samplePoint = getPlayerHealthSamplePoint(sample, precent);
    bool result = isPixelPlayerLowHealth(sample.pixel(samplePoint));

    QPainter painter(&sample);
    painter.setPen(Qt::white);
    painter.drawLine(QPoint(samplePoint.x(), 0), QPoint(samplePoint.x(), sample.height()));

    return result;
}

bool MainWindow::isPetLowResource(QImage &sample, int precent)
{
    DEFINE_IMAGE_ADAPTER(sample);

    if (sample.isNull())
    {
        return false;
    }

    QPair<QPoint, QPoint> samplePoints = getPetResourceSamplePoints(sample, precent);
    bool result = isPixelPetLowResource(sample.pixel(samplePoints.first)) || isPixelPetLowResource(sample.pixel(samplePoints.second));

    QPainter painter(&sample);
    painter.setPen(Qt::white);
    painter.drawLine(QPoint(samplePoints.first.x(), 0), QPoint(samplePoints.first.x(), sample.height()));
    painter.drawLine(QPoint(samplePoints.second.x(), 0), QPoint(samplePoints.second.x(), sample.height()));

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

void MainWindow::on_comboBox_GameWindows_currentIndexChanged(int index)
{
    ui->checkBox_Switch->setChecked(false);

    applyBlankPixmapForPlayerHealth();
    applyBlankPixmapForPetResource();

    if (index == -1)
    {
        applyBlankPixmapForPlayerName();
    }
    else
    {
        ui->label_PlayerName->setPixmap(playerNamePixmaps[index]);
    }
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

    SetWindowTextW(gameWindows[window_index], QStringLiteral("QQ自由幻想 - %1").arg(text).toStdWString().c_str());
}

void MainWindow::on_pushButton_SetForeground_clicked()
{
    int window_index = ui->comboBox_GameWindows->currentIndex();

    if (window_index == -1)
    {
        return;
    }

    SetForegroundWindow(gameWindows[window_index]);
}

void MainWindow::on_checkBox_AutoPlayerHealth_toggled(bool checked)
{
    ui->spinBox_MinPlayerHealth->setEnabled(!checked);
    ui->comboBox_PlayerHealthKey->setEnabled(!checked);

    if (!checked)
    {
        applyBlankPixmapForPlayerHealth();
    }
}

void MainWindow::on_checkBox_AutoPetSupply_toggled(bool checked)
{
    ui->spinBox_MinPetResource->setEnabled(!checked);
    ui->comboBox_PetHealthKey->setEnabled(!checked);

    if (!checked)
    {
        applyBlankPixmapForPetResource();
    }
}

void MainWindow::applyBlankPixmapForPlayerName()
{
    QPixmap playerNamePixmap(playerNameRect.size());
    playerNamePixmap.fill(Qt::gray);
    ui->label_PlayerName->setPixmap(playerNamePixmap);
}

void MainWindow::applyBlankPixmapForPlayerHealth()
{
    QPixmap playerHealthPixmap(playerHealthRect.size() * 2);
    playerHealthPixmap.fill(Qt::gray);
    ui->label_PlayerHealth->setPixmap(playerHealthPixmap);
}

void MainWindow::applyBlankPixmapForPetResource()
{
    QPixmap petResourcePixmap(petResourceRect.size() * 2);
    petResourcePixmap.fill(Qt::gray);
    ui->label_PetResource->setPixmap(petResourcePixmap);
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

void MainWindow::on_pushButton_ScreenShot_clicked()
{
    int window_index = ui->comboBox_GameWindows->currentIndex();

    if (window_index == -1)
    {
        return;
    }

    RECT clientRect;
    GetClientRect(gameWindows[window_index], &clientRect);
    QImage image = getGamePicture(gameWindows[window_index], QRect(0, 0, clientRect.right, clientRect.bottom));

    if (!image.isNull())
    {
        image.save(getScreenShotPath());
    }
}

void MainWindow::on_pushButton_OpenScreenShotFolder_clicked()
{
    QDesktopServices::openUrl(QUrl(QCoreApplication::applicationDirPath() + "/screenshot", QUrl::TolerantMode));
}
