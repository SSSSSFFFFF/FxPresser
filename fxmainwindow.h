#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTimer>
#include <QImage>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <Windows.h>

#include <array>
#include <utility>
#include <chrono>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct QImageAdapter
{
    int width = 0;
    int height = 0;
    int stride = 0;
    uchar* pixels = nullptr;

    QImageAdapter() = default;

    QImageAdapter(QImage& image)
        :QImageAdapter()
    {
        if (image.format() == QImage::Format_RGB888)
        {
            width = image.width();
            height = image.height();
            stride = image.bytesPerLine();
            pixels = image.bits();
        }
    }
};

#define DEFINE_IMAGE_ADAPTER(var) QImageAdapter var##A(var)

struct SConfigData
{
    bool fxSwitch[10]; //Fx开关
    double fxCD[10]; //单个按键的间隔
    double fxInterval; //Fx排队间隔

    bool playerSwitch; //人物补给开关
    int playerPercent; //人物补给百分比
    int playerKey; //人物补给按键

    bool petSwitch; //宠物补给开关
    int petPercent; //宠物补给百分比
    int petKey; //宠物补给按键

    QString title; //游戏窗口标题
    QByteArray hash; //角色名图片hash

    int x, y; //程序位置

    SConfigData()
    {
        std::fill(fxSwitch, fxSwitch + 10, false);
        std::fill(fxCD, fxCD + 10, 1.0);
        fxInterval = 0.75;

        playerSwitch = false;
        playerPercent = 50;
        playerKey = 0;

        petSwitch = false;
        petPercent = 50;
        petKey = 0;

        x = -1;
        y = -1;
    }
};

class FxMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    FxMainWindow(QWidget* parent = nullptr);
    ~FxMainWindow();

private slots:
    void on_pushButton_UpdateGameWindows_clicked();

    void on_any_Fx_checkBox_toggled(bool checked);

    void on_comboBox_GameWindows_currentIndexChanged(int index);

    void on_checkBox_Switch_toggled(bool checked);

    void on_pushButton_ChangeWindowTitle_clicked();

    void on_pushButton_SetForeground_clicked();

    void on_checkBox_AutoPlayerSupply_toggled(bool checked);

    void on_checkBox_AutoPetSupply_toggled(bool checked);

private:
    Ui::MainWindow* ui;

    void applyBlankPixmapForPlayerHealth();
    void applyBlankPixmapForPetResource();

    //按照设定百分比截取血条图片中的一点
    static QPoint getPlayerHealthSamplePoint(QImage image, int percent);
    static QPair<QPoint, QPoint> getPetResourceSamplePoints(QImage image, int percent);

    //像素归一化
    static std::array<float, 3> normalizePixel(QRgb pixel);
    //颜色类型转换
    static std::array<float, 3> rgb2HSV(QRgb rgbColor);
    //获取图片特征值
    static QByteArray imageHash(QImage image);

    //判断一点是否为空血条
    static bool isPixelPetLowResource(QRgb pixel);
    static bool isPixelPlayerLowHealth(QRgb pixel);

    //对图片判断是否血量低
    static bool isPlayerLowHealth(QImage& sample, int precent);
    static bool isPetLowResource(QImage& sample, int precent);

    //计时部分
    QTimer pressTimer; //固定间隔按键的计时器
    QTimer supplyTimer; //补给计时器

    //10个界面控件
    std::array<std::pair<QCheckBox*, QDoubleSpinBox*>, 10> enumerateControls;
    //每个按键上次触发的时间点，用于计算单个按键的间隔
    std::array<std::chrono::steady_clock::time_point, 10> lastPressedTimePoint;
    //最后一次按键的时间点，用于确定实际按键的时机
    std::chrono::steady_clock::time_point lastAnyPressedTimePoint;

    //上次人物补给的时间点
    std::chrono::steady_clock::time_point lastPlayerSupplyTimer;
    //上次吃宝宝糖果的时间点
    std::chrono::steady_clock::time_point lastPetSupplyTimer;

    //扫描到的游戏窗口数据
    QVector<HWND> gameWindows;
    QVector<QImage> playerNameImages;
    QVector<QByteArray> playerNameHashes;

    //当前游戏窗口的数据
    HWND currentGameWindow;
    HDC currentDC, currentCDC;
    QByteArray currentHash;

    //在启动的时候运行一次，根据保存的hash查找对应游戏窗口并设置窗口标题
    void autoSelectAndRenameGameWindow(const QByteArray& hash);

    void initGDI(HWND window);
    void clearGDI();

    //定时器执行的函数
    void pressProc();
    void supplyProc();

    //重置按键的计时
    void resetTimeStamp(int index);
    void resetAllTimeStamps();

    //扫描游戏窗口
    void scanGameWindows();

    //执行某个按键
    void pressKey(HWND window, UINT code);

    //截取游戏窗口的某个区域
    QImage getGamePicture(QRect rect);

    //计算?配置文件的路径
    QString getConfigPath();

    //读取配置文件
    SConfigData readConfig(const QString& filename);
    //写入配置文件
    void writeConfig(const QString& filename, const SConfigData& config);
    //加载配置文件到UI
    void loadConfig();
    //保存当前配置
    void autoWriteConfig();
    //从UI生成配置
    SConfigData makeConfigFromUI();
    //将配置应用到UI
    void applyConfigToUI(const SConfigData& config);
    //应用默认配置
    void applyDefaultConfigToUI();

    //读写json配置文件用
    QJsonObject configToJson(const SConfigData& config);
    SConfigData jsonToConfig(QJsonObject json);
};
#endif // MAINWINDOW_H
