#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QPair>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <Windows.h>

#include <vector>
#include <array>
#include <utility>
#include <tuple>
#include <chrono>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct QImageAdapter
{
    int width = 0;
    int height = 0;
    int stride = 0;
    uchar *pixels = nullptr;

    QImageAdapter() = default;

    QImageAdapter(QImage &image)
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
    QString title;

    bool fxSwitch[10]; //Fx开关
    double fxCD[10]; //单个按键的间隔
    double fxInterval; //Fx排队间隔

    bool playerSwitch; //人物补给开关
    int playerPrecent; //人物补给百分比
    int playerKey; //人物补给按键
    double playerCD; //人物补给间隔

    bool petSwitch; //宠物补给开关
    int petPrecent; //宠物补给百分比
    int petKey; //宠物补给按键
    double petCD; //宠物补给间隔

    SConfigData()
    {
        std::fill(fxSwitch, fxSwitch + 10, false);
        std::fill(fxCD, fxCD + 10, 1.0);
        fxInterval = 0.8;

        playerSwitch = false;
        playerPrecent = 50;
        playerKey = 0;
        playerCD = 5.2;

        petSwitch = false;
        petPrecent = 50;
        petKey = 0;
        petCD = 5.2;
    }
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
    void on_pushButton_UpdateGameWindows_clicked();

    void on_any_Fx_checkBox_toggled(bool checked);

    void on_comboBox_GameWindows_currentIndexChanged(int index);

    void on_pushButton_SaveConfigAs_clicked();

    void on_pushButton_RenameConfig_clicked();

    void on_comboBox_Configs_currentIndexChanged(int index);

    void on_checkBox_Switch_toggled(bool checked);

    void on_pushButton_DeleteConfig_clicked();

    void on_pushButton_ChangeWindowTitle_clicked();

    void on_pushButton_SetForeground_clicked();

    void on_pushButton_ReadImage_clicked();

    void on_pushButton_TestPlayerSupply_clicked();

    void on_pushButton_TestPetSupply_clicked();

    void on_checkBox_AutoPlayerHealth_toggled(bool checked);

    void on_checkBox_AutoPetSupply_toggled(bool checked);

private:
    Ui::MainWindow *ui;

    //读取的图像
    QImage sampleImage;

    void applyBlankPixmapForPlayer();
    void applyBlankPixmapForPet();
    void applyBlankPixmapForSample();

    //按照设定百分比截取血条图片中的一点
    static QPoint getPlayerHealthSamplePoint(QImage image, int percent);
    static QPair<QPoint, QPoint> getPetResourceSamplePoints(QImage image, int percent);

    //像素归一化
    static std::array<float, 3> normalizePixel(QRgb pixel);
    //颜色类型转换
    static std::array<float, 3> rgb2HSV(QRgb rgbColor);

    //判断一点是否为空血条
    static bool isPixelPetLowResource(QRgb pixel);
    static bool isPixelPlayerLowHealth(QRgb pixel);

    //对图片判断是否血量低
    static bool isPlayerLowHealth(QImage sample, int precent);
    static bool isPetLowHealth(QImage sample, int precent);

    //计时部分
    QTimer pressTimer; //固定间隔按键的计时器
    QTimer supplyTimer; //补给计时器

    //10个界面控件
    std::array<std::pair<QCheckBox *, QDoubleSpinBox *>, 10> enumerateControls;
    //每个按键上次触发的时间点，用于计算单个按键的间隔
    std::array<std::chrono::steady_clock::time_point, 10> lastPressedTimePoint;
    //最后一次按键的时间点，用于确定实际按键的时机
    std::chrono::steady_clock::time_point lastAnyPressedTimePoint;

    //上次人物补给的时间点
    std::chrono::steady_clock::time_point lastPlayerSupplyTimer;
    //上次吃宝宝糖果的时间点
    std::chrono::steady_clock::time_point lastPetSupplyTimer;

    //游戏窗口的句柄
    std::vector<HWND> gameWindows;

    //当前配置的名称，切换时用
    QString currentConfigName;

    //定时器执行的函数
    void pressProc();

    //重置按键的计时
    void resetTimeStamp(int index);
    void resetAllTimeStamps();

    //扫描游戏窗口
    void updateGameWindows();

    //执行某个按键
    void pressKey(HWND window, UINT code);

    //截取游戏窗口的某个区域
    QImage getGamePicture(HWND window, QRect rect);

    //扫描配置文件
    void scanConfigs();

    //计算?配置文件的路径
    QString getConfigPath(const QString &name);
    QString getGlobalConfigPath();

    //读取配置文件
    SConfigData readConfig(const QString &filename);
    //写入配置文件
    void writeConfig(const QString &filename, const SConfigData &config);
    //加载配置文件到UI
    void loadConfig(const QString &name);
    //保存当前配置
    void autoWriteConfig();
    //从UI生成配置
    SConfigData makeConfigFromUI();
    //将配置应用到UI
    void applyConfigToUI(const SConfigData &config);
    //应用默认配置
    void applyDefaultConfigToUI();

    //读写json配置文件用
    QJsonObject configToJson(const SConfigData &config);
    SConfigData jsonToConfig(QJsonObject json);

    //读写窗口位置
    void writeWindowPos();
    void readWindowPos();
};
#endif // MAINWINDOW_H
