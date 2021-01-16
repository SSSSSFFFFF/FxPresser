#include "fxmainwindow.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QCryptographicHash>
#include <QApplication>
#include <QButtonGroup>

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

FxMainWindow::FxMainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    currentGameWindow = nullptr;
    currentDC = nullptr;
    currentCDC = nullptr;

    setupUI();

    applyBlankPixmapForLabel(ui.label_player_image);
    applyBlankPixmapForLabel(ui.label_pet_image);

    connect(&pressTimer, &QTimer::timeout, this, &FxMainWindow::pressProc);
    connect(&supplyTimer, &QTimer::timeout, this, &FxMainWindow::supplyProc);

    QDir dir = QCoreApplication::applicationDirPath();
    dir.mkdir(QStringLiteral("config"));

    //读取参数
    loadConfig();

    //扫描游戏窗口
    scanGameWindows();

    //首次自动选择游戏窗口
    autoSelectAndRenameGameWindow(currentHash);

    pressTimer.setTimerType(Qt::PreciseTimer);
    pressTimer.start(50);

    supplyTimer.setTimerType(Qt::PreciseTimer);
    supplyTimer.start(1000);
}

FxMainWindow::~FxMainWindow()
{
    pressTimer.stop();
    supplyTimer.stop();

    autoWriteConfig();
    clearGDI();
}

void FxMainWindow::autoSelectAndRenameGameWindow(const QByteArray& hash)
{
    int index = -1;

    if (!gameWindows.isEmpty())
    {
        if (!hash.isEmpty())
        {
            for (int hash_index = 0; hash_index < playerNameImages.size(); ++hash_index)
            {
                if (playerNameHashes[hash_index] == hash)
                {
                    index = hash_index;
                    break;
                }
            }
        }
    }

    ui.combo_windows->setCurrentIndex(index);

    //找到窗口之后自动更改窗口标题
    if (index != -1)
    {
        initGDI(gameWindows[index]);
        changeWindowTitle();
    }
    else
    {
        clearGDI();
    }
}

void FxMainWindow::initGDI(HWND window)
{
    if (window == currentGameWindow)
        return;

    clearGDI();

    currentGameWindow = window;
    currentDC = GetDC(window);
    currentCDC = CreateCompatibleDC(currentDC);
}

void FxMainWindow::clearGDI()
{
    if (currentCDC != nullptr)
        DeleteDC(currentCDC);

    if (currentGameWindow != nullptr && currentDC != nullptr)
        ReleaseDC(currentGameWindow, currentDC);

    currentGameWindow = nullptr;
    currentDC = nullptr;
    currentCDC = nullptr;
}

void FxMainWindow::pressProc()
{
    if (!ui.check_global_switch->isChecked())
    {
        return;
    }

    int window_index = ui.combo_windows->currentIndex();

    if (window_index == -1)
    {
        return;
    }

    if (currentDefaultKey != -1 && !defaultKeyTriggered)
    {
        tryPressKey(gameWindows[window_index], currentDefaultKey, true);
        defaultKeyTriggered = true;
    }

    for (int key_index = 0; key_index < 10; ++key_index)
    {
        //保证缺省键只触发一次
        if (key_index == currentDefaultKey || !ui.key_checks[key_index]->isChecked())
        {
            continue;
        }

        tryPressKey(gameWindows[window_index], key_index, false);
    }
}

void FxMainWindow::supplyProc()
{
    if (!ui.check_global_switch->isChecked())
    {
        return;
    }

    int window_index = ui.combo_windows->currentIndex();

    if (window_index == -1)
    {
        return;
    }

    if (ui.check_player_supply->isChecked())
    {
        int key_index = ui.combo_player_supply_key->currentIndex();

        if (key_index != -1)
        {
            QImage healthPicture = getGamePicture(playerHealthRect);

            if (isPlayerLowHealth(healthPicture, ui.spin_player_supply->value()))
            {
                pressKey(gameWindows[window_index], VK_F1 + key_index);
            }

            ui.label_player_image->setPixmap(QPixmap::fromImage(healthPicture));
        }
    }

    if (ui.check_pet_supply->isChecked())
    {
        int key_index = ui.combo_pet_supply_key->currentIndex();

        if (key_index != -1)
        {
            QImage healthPicture = getGamePicture(petResourceRect);

            if (isPetLowResource(healthPicture, ui.spin_pet_supply->value()))
            {
                pressKey(gameWindows[window_index], VK_F1 + key_index);
            }

            ui.label_pet_image->setPixmap(QPixmap::fromImage(healthPicture));
        }
    }
}

void FxMainWindow::resetTimeStamp(int index)
{
    lastPressedTimePoint[index] = std::chrono::steady_clock::now();
}

void FxMainWindow::resetAllTimeStamps()
{
    //为了实现点击全局开关时自动触发一次，此处将每个按键的上次时间设为0
    lastPressedTimePoint.fill(std::chrono::steady_clock::time_point());
    lastAnyPressedTimePoint = std::chrono::steady_clock::time_point();
}

void FxMainWindow::scanGameWindows()
{
    wchar_t c_string[512];

    int found = 0, invalid = 0;

    gameWindows.clear();
    playerNameImages.clear();
    playerNameHashes.clear();
    ui.combo_windows->clear();
    ui.check_global_switch->setChecked(false);

    HWND hWindow = FindWindowW(L"QQSwordWinClass", nullptr);

    ui.combo_windows->blockSignals(true);

    while (hWindow != nullptr)
    {
        GetWindowTextW(hWindow, c_string, 512);

        if (QString::fromWCharArray(c_string).startsWith(QStringLiteral("QQ自由幻想")))
        {
            initGDI(hWindow);
            QImage playerNameImage = getGamePicture(playerNameRect);
            clearGDI();

            DEFINE_IMAGE_ADAPTER(playerNameImage);

            if (!playerNameImage.isNull())
            {
                ++found;
                gameWindows.push_back(hWindow);
                playerNameHashes.push_back(imageHash(playerNameImage));
                playerNameImages.push_back(playerNameImage);
                ui.combo_windows->addItem(QIcon(QPixmap::fromImage(playerNameImage)), nullptr);
            }
            else
            {
                ++invalid;
            }
        }

        hWindow = FindWindowExW(nullptr, hWindow, L"QQSwordWinClass", nullptr);
    }

    ui.combo_windows->blockSignals(false);

    if (found + invalid == 0)
    {
        QMessageBox::information(this, QStringLiteral("摘要"), QStringLiteral("没有找到游戏窗口。"));
    }
    else if (invalid != 0)
    {
        QString summary = QStringLiteral("共找到 %1 个游戏窗口，其中截图成功 %2 个，失败 %3 个。").arg(found + invalid).arg(found).arg(invalid);
        QMessageBox::information(this, QStringLiteral("摘要"), summary);
    }
}

void FxMainWindow::changeWindowTitle()
{
    int window_index = ui.combo_windows->currentIndex();

    if (window_index == -1)
    {
        return;
    }

    QString text = ui.line_title->text();

    if (!text.isEmpty())
    {
        SetWindowTextW(gameWindows[window_index], QStringLiteral("QQ自由幻想 - %1").arg(text).toStdWString().c_str());
    }
}

void FxMainWindow::tryPressKey(HWND window, int key_index, bool force)
{
    auto nowTimePoint = std::chrono::steady_clock::now();

    std::chrono::milliseconds differFromSelf = std::chrono::duration_cast<std::chrono::milliseconds>(nowTimePoint - lastPressedTimePoint[key_index]);
    std::chrono::milliseconds differFromAny = std::chrono::duration_cast<std::chrono::milliseconds>(nowTimePoint - lastAnyPressedTimePoint);
    std::chrono::milliseconds selfInterval(static_cast<long long>(ui.key_intervals[key_index]->value() * 1000));
    std::chrono::milliseconds anyInterval(static_cast<long long>(ui.spin_global_interval->value() * 1000));

    if (force || differFromSelf >= selfInterval && differFromAny >= anyInterval)
    {
        lastPressedTimePoint[key_index] += selfInterval;
        lastAnyPressedTimePoint = nowTimePoint;

        pressKey(window, VK_F1 + key_index);
    }
}

void FxMainWindow::pressKey(HWND window, UINT code)
{
    PostMessageA(window, WM_KEYDOWN, code, 0);
    PostMessageA(window, WM_KEYUP, code, 0);
}

QImage FxMainWindow::getGamePicture(QRect rect)
{
    std::vector<uchar> pixelBuffer;
    QImage result;

    BITMAPINFO b;

    if ((IsWindow(currentGameWindow) == FALSE) || (IsIconic(currentGameWindow) == TRUE))
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

    HBITMAP hBitmap = CreateCompatibleBitmap(currentDC, rect.width(), rect.height());
    SelectObject(currentCDC, hBitmap);

    BitBlt(currentCDC, 0, 0, rect.width(), rect.height(), currentDC, rect.left(), rect.top(), SRCCOPY);
    pixelBuffer.resize(rect.width() * rect.height() * 4);
    GetDIBits(currentCDC, hBitmap, 0, rect.height(), pixelBuffer.data(), &b, DIB_RGB_COLORS);
    DeleteObject(hBitmap);

    return QImage(pixelBuffer.data(), rect.width(), rect.height(), (rect.width() * 3 + 3) & (~3), QImage::Format_RGB888).rgbSwapped().mirrored();
}

QString FxMainWindow::getConfigPath()
{
    //exe目录/config/exe文件名.json
    auto dirp = QCoreApplication::applicationDirPath();
    auto exep = QCoreApplication::applicationFilePath();

    return (dirp + "/config/%1.json").arg(exep.mid(dirp.length() + 1, exep.length() - dirp.length() - 5));
}

SConfigData FxMainWindow::readConfig(const QString& filename)
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

void FxMainWindow::writeConfig(const QString& filename, const SConfigData& config)
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

void FxMainWindow::loadConfig()
{
    applyConfigToUI(readConfig(getConfigPath()));
}

void FxMainWindow::autoWriteConfig()
{
    writeConfig(getConfigPath(), makeConfigFromUI());
}

SConfigData FxMainWindow::makeConfigFromUI()
{
    SConfigData result;

    for (int index = 0; index < 10; ++index)
    {
        result.fxSwitch[index] = ui.key_checks[index]->isChecked();
        result.fxCD[index] = ui.key_intervals[index]->value();
    }

    result.globalInterval = ui.spin_global_interval->value();
    result.defaultKey = currentDefaultKey;

    result.playerSwitch = ui.check_player_supply->isChecked();
    result.playerPercent = ui.spin_player_supply->value();
    result.playerKey = ui.combo_player_supply_key->currentIndex();

    result.petSwitch = ui.check_pet_supply->isChecked();
    result.petPercent = ui.spin_pet_supply->value();
    result.petKey = ui.combo_pet_supply_key->currentIndex();

    result.hash = currentHash;
    result.title = ui.line_title->text();

    auto rect = geometry();

    result.x = rect.x();
    result.y = rect.y();

    return result;
}

void FxMainWindow::applyConfigToUI(const SConfigData& config)
{
    for (int index = 0; index < 10; ++index)
    {
        ui.key_checks[index]->setChecked(config.fxSwitch[index]);
        ui.key_intervals[index]->setValue(config.fxCD[index]);
    }

    ui.spin_global_interval->setValue(config.globalInterval);

    currentDefaultKey = config.defaultKey;
    for (int index = 0; index < 10; ++index)
    {
        ui.key_defaults[index]->setChecked(index == config.defaultKey);
    }

    ui.check_player_supply->setChecked(config.playerSwitch);
    ui.spin_player_supply->setValue(config.playerPercent);
    ui.combo_player_supply_key->setCurrentIndex(config.playerKey);

    ui.check_pet_supply->setChecked(config.petSwitch);
    ui.spin_pet_supply->setValue(config.petPercent);
    ui.combo_pet_supply_key->setCurrentIndex(config.petKey);

    currentHash = config.hash;
    ui.line_title->setText(config.title);

    auto rect = geometry();

    if (config.x != -1 && config.y != -1)
    {
        setGeometry(config.x, config.y, rect.width(), rect.height());
    }
}

void FxMainWindow::applyDefaultConfigToUI()
{
    applyConfigToUI(SConfigData());
}

QJsonObject FxMainWindow::configToJson(const SConfigData& config)
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

    result["Interval"] = config.globalInterval;
    result["DefaultKey"] = config.defaultKey;

    result["PlayerSwitch"] = config.playerSwitch;
    result["PlayerPercent"] = config.playerPercent;
    result["PlayerKey"] = config.playerKey;

    result["PetSwitch"] = config.petSwitch;
    result["PetPercent"] = config.petPercent;
    result["PetKey"] = config.petKey;

    result["X"] = config.x;
    result["Y"] = config.y;

    result["Title"] = config.title;
    result["Hash"] = QString::fromUtf8(config.hash);

    return result;
}

SConfigData FxMainWindow::jsonToConfig(QJsonObject json)
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

    result.globalInterval = json.take("Interval").toDouble(0.7);
    result.defaultKey = json.take("DefaultKey").toInt(-1);

    result.playerSwitch = json.take("PlayerSwitch").toBool(false);
    result.playerPercent = json.take("PlayerPercent").toInt(50);
    result.playerKey = json.take("PlayerKey").toInt(8);

    result.petSwitch = json.take("PetSwitch").toBool(false);
    result.petPercent = json.take("PetPercent").toInt(50);
    result.petKey = json.take("PetKey").toInt(9);

    result.x = json.take("X").toInt(-1);
    result.y = json.take("Y").toInt(-1);

    result.title = json.take("Title").toString("");
    result.hash = json.take("Hash").toString("").toUtf8();

    return result;
}

std::array<float, 3> FxMainWindow::rgb2HSV(QRgb rgbColor)
{
    std::array<float, 3> result;

    //opencv->color.cpp->RGB2HSV_f::operator()
    int b = qBlue(rgbColor), g = qGreen(rgbColor), r = qRed(rgbColor);
    float h, s, v;

    float vmin, diff;

    v = vmin = r;
    if (v < g) v = g;
    if (v < b) v = b;
    if (vmin > g) vmin = g;
    if (vmin > b) vmin = b;

    diff = v - vmin;
    s = diff / (fabs(v) + FLT_EPSILON);
    diff = (float)(60.0f / (diff + FLT_EPSILON));
    if (v == r)
        h = (g - b) * diff;
    else if (v == g)
        h = (b - r) * diff + 120.f;
    else
        h = (r - g) * diff + 240.f;

    if (h < 0) h += 360.f;

    result[0] = h;
    result[1] = s;
    result[2] = v;

    return result;
}

QByteArray FxMainWindow::imageHash(QImage image)
{
    if (image.isNull() || image.format() != QImage::Format_RGB888)
        return QByteArray();

    QByteArray imageBytes;
    QDataStream stream(&imageBytes, QIODevice::WriteOnly);

    stream << image;

    return QCryptographicHash::hash(imageBytes, QCryptographicHash::Md5).toBase64();
}

bool FxMainWindow::isPixelPetLowResource(QRgb pixel)
{
    std::array<float, 3> normalized = normalizePixel(pixel);

    //有一个分量大于40%，即有血条/蓝条覆盖
    //无覆盖时接近1:1:1
    return std::count_if(normalized.begin(), normalized.end(), [](float val) {return val > 0.4f; }) == 0;
}

bool FxMainWindow::isPixelPlayerLowHealth(QRgb pixel)
{
    auto fp_equal = [](float v1, float v2) {return fabs(v1 - v2) < 0.01f; };

    //绿/黄/红
    //绿血: H126.67  S0.90  V170.00
    //黄血: H36.92   S0.87  V255.00
    //红血: H13.85   S1.00  V221.00

    std::array<float, 3> hsvPix = rgb2HSV(pixel);

    float hVal = hsvPix[0];
    float sVal = hsvPix[1];

    //不是三种血条颜色之一则为需要补给
    return !(
        (fp_equal(hVal, 126.67f) && fp_equal(sVal, 0.90f)) ||
        (fp_equal(hVal, 36.92f) && fp_equal(sVal, 0.87f)) ||
        (fp_equal(hVal, 13.85f) && fp_equal(sVal, 1.00f)));
}

bool FxMainWindow::isPlayerLowHealth(QImage& sample, int precent)
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

bool FxMainWindow::isPetLowResource(QImage& sample, int precent)
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

void FxMainWindow::setupUI()
{
    auto get_h_line = [](QWidget* parent = nullptr) {
        auto line = new QFrame(parent);

        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);

        return line;
    };

    QFont switch_font;
    switch_font.setFamily(QStringLiteral("微软雅黑"));
    switch_font.setPointSize(20);
    switch_font.setBold(true);

    QStringList supply_keys;

    for (int index = 0; index < 10; ++index)
    {
        supply_keys << QString("F%1").arg(index + 1);
    }

    auto main_widget = new QWidget;
    auto vlayout_main = new QVBoxLayout(main_widget);

    ui.btn_scan = new QPushButton(QStringLiteral("扫描游戏窗口"), main_widget);
    connect(ui.btn_scan, &QPushButton::clicked, [this]()
        {
            scanGameWindows();

            if (!gameWindows.isEmpty())
                autoSelectAndRenameGameWindow(currentHash);
        });
    vlayout_main->addWidget(ui.btn_scan);

    ui.combo_windows = new QComboBox(main_widget);
    ui.combo_windows->setItemDelegate(new CharacterBoxDelegate);
    connect(ui.combo_windows, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this](int index)
        {
            ui.check_global_switch->setChecked(false);

            applyBlankPixmapForLabel(ui.label_player_image);
            applyBlankPixmapForLabel(ui.label_pet_image);

            if (index == -1)
            {
                clearGDI();
            }
            else
            {
                initGDI(gameWindows[index]);
                currentHash = playerNameHashes[index];
            }
        });
    vlayout_main->addWidget(ui.combo_windows);

    ui.line_title = new QLineEdit(main_widget);
    auto hlayout_title = new QHBoxLayout(main_widget);
    auto label_title = new QLabel(QStringLiteral("窗口标题"), main_widget);
    label_title->setToolTip(QStringLiteral("方便OBS之类的软件区分/自动切换捕获窗口。"));
    hlayout_title->addWidget(label_title);
    hlayout_title->addWidget(ui.line_title, 1);
    vlayout_main->addLayout(hlayout_title);

    ui.btn_change_title = new QPushButton(QStringLiteral("修改窗口标题"), main_widget);
    connect(ui.btn_change_title, &QPushButton::clicked, this, &FxMainWindow::changeWindowTitle);
    vlayout_main->addWidget(ui.btn_change_title);

    ui.btn_switch_to_window = new QPushButton(QStringLiteral("切换到游戏窗口"), main_widget);
    connect(ui.btn_switch_to_window, &QPushButton::clicked, [this]()
        {
            int window_index = ui.combo_windows->currentIndex();

            if (window_index == -1)
            {
                return;
            }

            SetForegroundWindow(gameWindows[window_index]);
        });
    vlayout_main->addWidget(ui.btn_switch_to_window);

    vlayout_main->addWidget(get_h_line(main_widget));

    ui.check_global_switch = new QCheckBox(QStringLiteral("全局开关"), main_widget);
    ui.check_global_switch->setFont(switch_font);
    connect(ui.check_global_switch, &QCheckBox::toggled, [this](bool checked)
        {
            if (checked)
            {
                defaultKeyTriggered = false;
                resetAllTimeStamps();
            }
        });
    auto hlayout_switch = new QHBoxLayout(main_widget);
    hlayout_switch->addStretch();
    hlayout_switch->addWidget(ui.check_global_switch);
    hlayout_switch->addStretch();
    vlayout_main->addLayout(hlayout_switch);

    ui.spin_global_interval = new QDoubleSpinBox(main_widget);
    ui.spin_global_interval->setSuffix(" s");
    ui.spin_global_interval->setDecimals(2);
    ui.spin_global_interval->setMinimum(0.1);
    ui.spin_global_interval->setMaximum(365.0);
    ui.spin_global_interval->setSingleStep(0.01);
    ui.spin_global_interval->setValue(0.75);
    auto hlayout_press_interval = new QHBoxLayout(main_widget);
    auto label_global_switch = new QLabel(QStringLiteral("全局间隔"), main_widget);
    label_global_switch->setToolTip(QStringLiteral("任意两次触发按键的最小间隔，用于若干个技能同时使用的情况。\n"
        "根据武器的施法速度设置合适的值。\n"
        "比如治疗+解衰弱交替施法：兔牙衔接较快，全局间隔设置为0.7秒，两个技能间隔均设置为1.7秒，即可自动交替使用。"));
    hlayout_press_interval->addStretch();
    hlayout_press_interval->addWidget(label_global_switch);
    hlayout_press_interval->addWidget(ui.spin_global_interval);
    hlayout_press_interval->addStretch();
    vlayout_main->addLayout(hlayout_press_interval);
    vlayout_main->addWidget(get_h_line());

    auto gridlayout_keys = new QGridLayout(main_widget);
    auto label_default_skill = new QLabel(QStringLiteral("是缺省技能"), main_widget);
    label_default_skill->setToolTip(QStringLiteral("指在游戏中右键绿框的技能，最多能设置一个。\n"
        "缺省技能在'全局开关'打开时会首先被触发且只触发一次，建议只用于平砍。\n"
        "药师治疗术等技能可以取消游戏内缺省，通过设置合适的间隔达到无缝衔接。"));
    gridlayout_keys->addWidget(new QLabel(QStringLiteral("启用"), main_widget), 0, 0);
    auto label_interval = new QLabel(QStringLiteral("间隔"), main_widget);
    label_interval->setToolTip(QStringLiteral("两次触发此按键的间隔，设置成CD+施法时间，即可做到无缝衔接，可以在游戏中试出合适的值。\n"
        "因为已经提供了一位小数，不建议像老魔手一样全部设置成1.0秒。"));
    gridlayout_keys->addWidget(label_interval, 0, 1);
    gridlayout_keys->addWidget(label_default_skill, 0, 2);

    for (int index = 0; index < 10; ++index)
    {
        auto check_key = new QCheckBox(QString("F%1").arg(index + 1), main_widget);
        auto spin_key_interval = new QDoubleSpinBox(main_widget);
        auto check_default = new QCheckBox(main_widget);

        spin_key_interval->setSuffix(" s");
        spin_key_interval->setDecimals(1);
        spin_key_interval->setMinimum(0.1);
        spin_key_interval->setMaximum(365.0);
        spin_key_interval->setSingleStep(0.1);
        spin_key_interval->setValue(1.0);
        ui.key_checks[index] = check_key;
        ui.key_intervals[index] = spin_key_interval;
        ui.key_defaults[index] = check_default;

        connect(check_key, &QCheckBox::toggled,
            [this, index](bool checked) {
                ui.key_intervals[index]->setEnabled(!checked);
                resetTimeStamp(index);
            });

        connect(check_default, &QCheckBox::toggled,
            [this, index](bool checked) {
                //模拟QButtonGroup互斥，并能够全部取消选择
                if (checked)
                {
                    currentDefaultKey = index;

                    for (int key_index = 0; key_index < 10; ++key_index)
                    {
                        if (key_index != index)
                            ui.key_defaults[key_index]->setChecked(false);
                    }
                }
                else
                {
                    currentDefaultKey = -1;
                }
            });

        gridlayout_keys->addWidget(check_key, index + 1, 0);
        gridlayout_keys->addWidget(spin_key_interval, index + 1, 1);
        gridlayout_keys->addWidget(check_default, index + 1, 2);
    }

    vlayout_main->addLayout(gridlayout_keys);

    vlayout_main->addWidget(get_h_line(main_widget));

    auto hlayout_player_image = new QHBoxLayout(main_widget);
    ui.label_player_image = new QLabel(main_widget);
    ui.label_player_image->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui.label_player_image->setMinimumSize(89, 7);
    ui.label_player_image->setMaximumSize(89, 7);
    ui.label_player_image->setFrameShape(QFrame::NoFrame);
    ui.label_player_image->setScaledContents(true);
    hlayout_player_image->addStretch();
    hlayout_player_image->addWidget(ui.label_player_image);
    hlayout_player_image->addStretch();
    vlayout_main->addLayout(hlayout_player_image);

    auto hlayout_player_supply = new QHBoxLayout(main_widget);
    ui.check_player_supply = new QCheckBox(QStringLiteral("角色补给"), main_widget);
    ui.spin_player_supply = new QSpinBox(main_widget);
    ui.combo_player_supply_key = new QComboBox(main_widget);
    connect(ui.check_player_supply, &QCheckBox::toggled, [this](bool checked)
        {
            ui.spin_player_supply->setEnabled(!checked);
            ui.combo_player_supply_key->setEnabled(!checked);

            if (!checked)
            {
                applyBlankPixmapForLabel(ui.label_player_image);
            }
        });
    ui.combo_player_supply_key->addItems(supply_keys);
    ui.combo_player_supply_key->setCurrentIndex(0);
    ui.spin_player_supply->setSuffix(" %");
    ui.spin_player_supply->setMinimum(1);
    ui.spin_player_supply->setMaximum(99);
    ui.spin_player_supply->setSingleStep(1);
    ui.spin_player_supply->setValue(50);
    hlayout_player_supply->addWidget(ui.check_player_supply);
    hlayout_player_supply->addWidget(ui.spin_player_supply);
    hlayout_player_supply->addWidget(ui.combo_player_supply_key);
    vlayout_main->addLayout(hlayout_player_supply);

    vlayout_main->addWidget(get_h_line(main_widget));

    auto hlayout_pet_image = new QHBoxLayout(main_widget);
    ui.label_pet_image = new QLabel(main_widget);
    ui.label_pet_image->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui.label_pet_image->setMinimumSize(34, 34);
    ui.label_pet_image->setMaximumSize(34, 34);
    ui.label_pet_image->setFrameShape(QFrame::NoFrame);
    ui.label_pet_image->setScaledContents(true);
    hlayout_pet_image->addStretch();
    hlayout_pet_image->addWidget(ui.label_pet_image);
    hlayout_pet_image->addStretch();
    vlayout_main->addLayout(hlayout_pet_image);

    auto hlayout_pet_supply = new QHBoxLayout(main_widget);
    ui.check_pet_supply = new QCheckBox(QStringLiteral("宠物补给"), main_widget);
    ui.spin_pet_supply = new QSpinBox(main_widget);
    ui.combo_pet_supply_key = new QComboBox(main_widget);
    connect(ui.check_pet_supply, &QCheckBox::toggled, [this](bool checked)
        {
            ui.spin_pet_supply->setEnabled(!checked);
            ui.combo_pet_supply_key->setEnabled(!checked);

            if (!checked)
            {
                applyBlankPixmapForLabel(ui.label_pet_image);
            }
        });
    ui.combo_pet_supply_key->addItems(supply_keys);
    ui.combo_pet_supply_key->setCurrentIndex(0);
    ui.spin_pet_supply->setSuffix(" %");
    ui.spin_pet_supply->setMinimum(1);
    ui.spin_pet_supply->setMaximum(99);
    ui.spin_pet_supply->setSingleStep(1);
    ui.spin_pet_supply->setValue(50);
    hlayout_pet_supply->addWidget(ui.check_pet_supply);
    hlayout_pet_supply->addWidget(ui.spin_pet_supply);
    hlayout_pet_supply->addWidget(ui.combo_pet_supply_key);
    vlayout_main->addLayout(hlayout_pet_supply);

    main_widget->setLayout(vlayout_main);
    this->setCentralWidget(main_widget);
    this->setFixedSize(minimumSize());
}

void FxMainWindow::applyBlankPixmapForLabel(QLabel* label)
{
    QPixmap pixmap(label->size());
    pixmap.fill(Qt::gray);
    label->setPixmap(pixmap);
}

QPoint FxMainWindow::getPlayerHealthSamplePoint(QImage image, int percent)
{
    if (image.isNull() || image.size() != playerHealthRect.size())
    {
        return QPoint(-1, -1);
    }

    return QPoint(
        static_cast<int>(ceil(playerHealthRect.width() * (percent / 100.0f))),
        playerHealthRect.height() / 2);
}

QPair<QPoint, QPoint> FxMainWindow::getPetResourceSamplePoints(QImage image, int percent)
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

std::array<float, 3> FxMainWindow::normalizePixel(QRgb pixel)
{
    float fRed = static_cast<float>(qRed(pixel));
    float fGreen = static_cast<float>(qGreen(pixel));
    float fBlue = static_cast<float>(qBlue(pixel));
    float sum = fRed + fGreen + fBlue;

    return std::array<float, 3>{fRed / sum, fGreen / sum, fBlue / sum};
}
