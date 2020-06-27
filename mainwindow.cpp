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
#include <QSettings>
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

    readGlobalConfig();

    for (int index = 0; index < 10; ++index)
    {
        QCheckBox* checkBox = findChild<QCheckBox *>(QStringLiteral("checkBox_F%1").arg(index + 1));
        QDoubleSpinBox* spinBox = findChild<QDoubleSpinBox *>(QStringLiteral("doubleSpinBox_F%1").arg(index + 1));

        enumerateControls[index].first = checkBox;
        enumerateControls[index].second = spinBox;

        connect(checkBox, &QCheckBox::toggled, this, &MainWindow::on_any_Fx_checkBox_toggled);
    }

    connect(&pressTimer, &QTimer::timeout, this, &MainWindow::pressProc);

    pressTimer.setTimerType(Qt::PreciseTimer);
    pressTimer.start(50);

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
}

MainWindow::~MainWindow()
{
    pressTimer.stop();
    autoWriteConfig();
    writeGlobalConfig();

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
        std::chrono::milliseconds anyInterval(static_cast<long long>(ui->doubleSpinBox_QueueInterval->value() * 1000));

        if (differFromSelf >= selfInterval && differFromAny >= anyInterval)
        {
            lastPressedTimePoint[key_index] += selfInterval;
            lastAnyPressedTimePoint = nowTimePoint;

            pressKey(gameWindows[window_index], VK_F1 + key_index);
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

    //类名大概是叫QQSwordWinClass，不知道是否会与其他游戏重复，所以不用。
    HWND hWindow = FindWindow(nullptr, nullptr);

    while (hWindow != nullptr)
    {
        GetWindowTextW(hWindow, c_string, 512);

        if (std::wcsstr(c_string, L"QQ自由幻想") != nullptr) //此处必须判断标题，qqffo.exe有很多窗口
        {
            DWORD pid;
            GetWindowThreadProcessId(hWindow, &pid);
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
            GetProcessImageFileNameW(hProcess, c_string, 512);
            CloseHandle(hProcess);

            std::wstring cppStr = c_string;
            if (cppStr.rfind(L"\\qqffo.exe") == (cppStr.length() - 10))
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

    QString summary = QStringLiteral("共找到 %1 个游戏窗口，其中截图成功 %2 个，失败 %3 个。").arg(found + invalid).arg(found).arg(invalid);
    QMessageBox::information(this, QStringLiteral("摘要"), summary);
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

QString MainWindow::getGlobalConfigPath()
{
    return  QCoreApplication::applicationDirPath() + "/global.json";
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
        result.fxPresses[index].first = enumerateControls[index].first->isChecked();
        result.fxPresses[index].second = enumerateControls[index].second->value();
    }

    result.title = ui->lineEdit_WindowTitle->text();
    result.interval = ui->doubleSpinBox_QueueInterval->value();
    return result;
}

void MainWindow::applyConfigToUI(const SConfigData &config)
{
    for (int index = 0; index < 10; ++index)
    {
        enumerateControls[index].first->setChecked(config.fxPresses[index].first);
        enumerateControls[index].second->setValue(config.fxPresses[index].second);
    }

    ui->lineEdit_WindowTitle->setText(config.title);
    ui->doubleSpinBox_QueueInterval->setValue(config.interval);
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
        keyObject[QStringLiteral("Enabled")] = config.fxPresses[index].first;
        keyObject[QStringLiteral("Interval")] = config.fxPresses[index].second;
        pressArray.append(keyObject);
    }
    result[QStringLiteral("AutoPress")] = pressArray;

    result["Title"] = config.title;
    result["Interval"] = config.interval;

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
        result.fxPresses.fill(std::make_pair(false, 1.0));
    }
    else
    {
        for (int index = 0; index < 10; index++)
        {
            QJsonObject keyObject = pressArray[index].toObject();
            result.fxPresses[index].first = keyObject.take(QStringLiteral("Enabled")).toBool(false);
            result.fxPresses[index].second = keyObject.take(QStringLiteral("Interval")).toDouble(1.0);
        }
    }

    result.title = json.take("Title").toString("");
    result.interval = json.take("Interval").toDouble(0.7);

    return result;
}

void MainWindow::writeGlobalConfig()
{
    QSettings settings("CC", "FxPresser");

    auto rect = geometry();

    settings.setValue("X", rect.x());
    settings.setValue("Y", rect.y());
}

void MainWindow::readGlobalConfig()
{
    QSettings settings("CC", "FxPresser");

    auto rect = geometry();

    int x = settings.value("X", 100).toInt();
    int y = settings.value("Y", 100).toInt();

    setGeometry(x, y, rect.width(), rect.height());
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
