#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStyledItemDelegate>
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

//多开怎么办

class CharaterBoxDelegate : public QStyledItemDelegate
{
public:
    CharaterBoxDelegate(QObject* parent = nullptr)
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
    QString checkBoxName, spinBoxName;
    QCheckBox *checkBox;
    QDoubleSpinBox *spinBox;

    ui->setupUi(this);
    ui->comboBox_GameWindows->setItemDelegate(new CharaterBoxDelegate);

    //-----------------------------------------------------------------------------------------------------------
    QueryPerformanceFrequency(&counterFrequency);
    //-----------------------------------------------------------------------------------------------------------

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
    setFixedSize(QSize(width(), height()));

    for (int index = 0; index < 10; ++index)
    {
        checkBoxName.sprintf("checkBox_F%d", index + 1);
        spinBoxName.sprintf("doubleSpinBox_F%d", index + 1);

        checkBox = findChild<QCheckBox *>(checkBoxName);
        spinBox = findChild<QDoubleSpinBox *>(spinBoxName);

        enumrateControls[index].first = checkBox;
        enumrateControls[index].second = spinBox;

        connect(checkBox, &QCheckBox::toggled, this, &MainWindow::on_any_checkBox_toggled);
    }

    connect(&pressTimer, &QTimer::timeout, this, &MainWindow::pressTimerProc);
    connect(&supplyTimer, &QTimer::timeout, this, &MainWindow::supplyTimerProc);

    pressTimer.setTimerType(Qt::PreciseTimer);
    supplyTimer.setTimerType(Qt::PreciseTimer);

    readConfig(getConfigPath());

    pressTimer.start(50);
    supplyTimer.start(900);
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
        if (!enumrateControls[key_index].first->isChecked())
        {
            continue;
        }

        if (getCountersDiffInSeconds(timeStamps[key_index], currentCounter) >= enumrateControls[key_index].second->value())
        {
            timeStamps[key_index] = currentCounter;
            pressFunctionKey(gameWindows[window_index], VK_F1 + key_index);
        }
    }
}

void MainWindow::supplyTimerProc()
{
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
            DEFINE_ADAPTER(healthPicture);

            if (!healthPicture.isNull())
            {
                QPoint samplePoint = getPlayerHealthSamplePoint(healthPicture, ui->spinBox_MinPlayerHealth->value());

                if (samplePoint.x() != -1 && samplePoint.y() != -1)
                {
                    if (isPixelLowResource(healthPicture.pixel(samplePoint)))
                    {
                        pressFunctionKey(gameWindows[window_index], VK_F1 + key_index);
                    }
                }
            }
        }
    }

    if (ui->checkBox_AutoPetSupply->isChecked())
    {
        int key_index = ui->comboBox_PetHealthKey->currentIndex();

        if (key_index != -1)
        {
            QImage healthPicture = getGamePicture(gameWindows[window_index], petResourceRect);
            DEFINE_ADAPTER(healthPicture);

            if (!healthPicture.isNull())
            {
                QPair<QPoint, QPoint> samplePoints = getPetResourceSamplePoints(healthPicture, ui->spinBox_MinPetHealth->value());

                if (samplePoints.first.x() != -1 && samplePoints.first.y() != -1)
                {
                    if (isPixelLowResource(healthPicture.pixel(samplePoints.first)) || isPixelLowResource(healthPicture.pixel(samplePoints.second)))
                    {
                        pressFunctionKey(gameWindows[window_index], VK_F1 + key_index);
                    }
                }
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

    gameWindows.clear();
    ui->comboBox_GameWindows->clear();
    ui->checkBox_Switch->setChecked(false);

    HWND hWindow = FindWindowEx(nullptr, nullptr, nullptr, nullptr);

    while (hWindow != nullptr)
    {
        GetWindowTextA(hWindow, c_string, 512);

        if (strstr(c_string, "QQ自由幻想") != nullptr)
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
                QImage healthPicture = getGamePicture(hWindow, playerHealthRect);
                QImage petPicture = getGamePicture(hWindow, petResourceRect);
                DEFINE_ADAPTER(characterPicture);
                DEFINE_ADAPTER(healthPicture);
                DEFINE_ADAPTER(petPicture);

                if (!characterPicture.isNull())
                {
                    gameWindows.push_back(hWindow);
                    ui->comboBox_GameWindows->addItem(QIcon(QPixmap::fromImage(characterPicture)), nullptr);
                }
            }
        }

        hWindow = FindWindowEx(nullptr, hWindow, nullptr, nullptr);
    }
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
    static const int x_table[34] =
    {
        13,11,8,7,6,5,4,
        3,2,2,1,1,1,1,
        1,1,1,1,1,1,1,
        1,1,1,2,2,3,4,
        5,6,7,8,10,13
    };

    DEFINE_ADAPTER(image);

    if (image.isNull() || image.size() != petResourceRect.size())
    {
        return qMakePair(QPoint(-1, -1), QPoint(-1, -1));
    }

    int sample_height = static_cast<int>(ceil(percent / 100.0f * petResourceRect.height())) - 1;

    QPoint healthPoint(petResourceRect.width() - x_table[sample_height] - 1, sample_height);

    QPoint manaPoint(x_table[sample_height], sample_height);

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

bool MainWindow::isPixelLowResource(QRgb pixel)
{
    static constexpr int threshold = 100;

    auto normalized = normalizePixel(pixel);

    return false;
}

QString MainWindow::getConfigPath()
{
    QFileInfo exePath = QCoreApplication::applicationFilePath();
    QDir dirPath = QCoreApplication::applicationDirPath();

    dirPath.mkdir(QStringLiteral("config"));
    dirPath.cd(QStringLiteral("config"));

    return dirPath.absoluteFilePath(QStringLiteral("%1.json").arg(exePath.baseName()));
}

void MainWindow::readConfig(const QString & filename)
{
    QFile file;
    QJsonDocument doc;
    QJsonObject root;

    file.setFileName(filename);
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        return;
    }

    doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull())
    {
        return;
    }

    applyConfigToUI(jsonToConfig(doc.object()));
}

void MainWindow::writeConfig(const QString & filename)
{
    QFile file;
    QJsonObject root;
    QJsonDocument doc;

    file.setFileName(filename);
    if (!file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return;
    }

    root = configToJson(getConfigFromUI());
    doc.setObject(root);
    file.write(doc.toJson(QJsonDocument::Indented));
}

void MainWindow::autoWriteConfig()
{
    writeConfig(getConfigPath());
}

SConfigData MainWindow::getConfigFromUI()
{
    SConfigData result;

    for (int index = 0; index < 10; ++index)
    {
        result.presses[index].first = enumrateControls[index].first->isChecked();
        result.presses[index].second = enumrateControls[index].second->value();
    }

    std::get<0>(result.playerSupply) = ui->checkBox_AutoPlayerHealth->isChecked();
    std::get<1>(result.playerSupply) = ui->spinBox_MinPlayerHealth->value();
    std::get<2>(result.playerSupply) = ui->comboBox_PlayerHealthKey->currentIndex();

    std::get<0>(result.petSupply) = ui->checkBox_AutoPetSupply->isChecked();
    std::get<1>(result.petSupply) = ui->spinBox_MinPetHealth->value();
    std::get<2>(result.petSupply) = ui->comboBox_PetHealthKey->currentIndex();

    return result;
}

void MainWindow::applyConfigToUI(const SConfigData &config)
{
    for (int index = 0; index < 10; ++index)
    {
        enumrateControls[index].first->setChecked(config.presses[index].first);
        enumrateControls[index].second->setValue(config.presses[index].second);
    }

    ui->checkBox_AutoPlayerHealth->setChecked(std::get<0>(config.playerSupply));
    ui->spinBox_MinPlayerHealth->setValue(std::get<1>(config.playerSupply));
    ui->comboBox_PlayerHealthKey->setCurrentIndex(std::get<2>(config.playerSupply));

    ui->checkBox_AutoPetSupply->setChecked(std::get<0>(config.petSupply));
    ui->spinBox_MinPetHealth->setValue(std::get<1>(config.petSupply));
    ui->comboBox_PetHealthKey->setCurrentIndex(std::get<2>(config.petSupply));
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

    return result;
}

SConfigData MainWindow::jsonToConfig(QJsonObject json)
{
    SConfigData result;
    QJsonArray pressArray;
    QJsonObject supplyObject;

    pressArray = json.take(QStringLiteral("AutoPress")).toArray();
    for (int index = 0; index < 10; index++)
    {
        QJsonObject keyObject = pressArray[index].toObject();
        result.presses[index].first = keyObject.take(QStringLiteral("Enabled")).toBool();
        result.presses[index].second = keyObject.take(QStringLiteral("Interval")).toDouble();
    }

    supplyObject = json.take(QStringLiteral("AutoPlayerSupply")).toObject();
    std::get<0>(result.playerSupply) = supplyObject.take(QStringLiteral("Enabled")).toBool();
    std::get<1>(result.playerSupply) = supplyObject.take(QStringLiteral("Percent")).toInt();
    std::get<2>(result.playerSupply) = supplyObject.take(QStringLiteral("Key")).toInt();

    supplyObject = json.take(QStringLiteral("AutoPetSupply")).toObject();
    std::get<0>(result.petSupply) = supplyObject.take(QStringLiteral("Enabled")).toBool();
    std::get<1>(result.petSupply) = supplyObject.take(QStringLiteral("Percent")).toInt();
    std::get<2>(result.petSupply) = supplyObject.take(QStringLiteral("Key")).toInt();

    return result;
}

void MainWindow::on_any_checkBox_toggled(bool checked)
{
    QObject *control = sender();

    auto index = std::find_if(enumrateControls.begin(), enumrateControls.end(),
        [control](const std::pair<QCheckBox *, QDoubleSpinBox *> &p)
    {return p.first == control; })
        - enumrateControls.begin();

    enumrateControls[index].second->setEnabled(!checked);
    resetTimeStamp(index);
}

void MainWindow::on_checkBox_Switch_clicked(bool checked)
{
    if (checked)
    {
        resetAllTimeStamps();
    }
}

void MainWindow::on_pushButton_TestKey_clicked()
{
    int game_index = ui->comboBox_GameWindows->currentIndex();
    int key_index = ui->comboBox_PlayerHealthKey->currentIndex();

    if (game_index != -1)
        pressFunctionKey(gameWindows[game_index], VK_F1 + key_index);
}
