#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QImage>
#include <QPair>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QDir>

#include <Windows.h>

#include <vector>
#include <array>
#include <utility>
#include <tuple>

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

#define DEFINE_ADAPTER(var) QImageAdapter var##A(var)

struct WindowHandles
{
    HWND window;
    HDC windowDC, compatibleDC;
    HBITMAP characterBitmap, playerHealthBitmap, petResourceBitmap;

    WindowHandles(HWND handle)
    {
        window = handle;
        windowDC = GetDC(window);
        compatibleDC = CreateCompatibleDC(windowDC);

        characterBitmap = CreateCompatibleBitmap(windowDC, 1, 1);
        playerHealthBitmap = CreateCompatibleBitmap(windowDC, 1, 1);
        petResourceBitmap = CreateCompatibleBitmap(windowDC, 1, 1);
    }

    ~WindowHandles()
    {
        DeleteObject(characterBitmap);
        DeleteObject(playerHealthBitmap);
        DeleteObject(petResourceBitmap);
        DeleteDC(compatibleDC);
        ReleaseDC(window, windowDC);
    }
};

struct SConfigData
{
    QString title;
    std::array<std::pair<bool, double>, 10> presses; //启用, 间隔
    std::tuple<bool, int, int> playerSupply; //启用, 百分比, 按键
    std::tuple<bool, int, int> petSupply; //启用, 百分比, 按键

    SConfigData()
    {
        for (int index = 0; index < 10; ++index)
        {
            presses[index].first = false;
            presses[index].second = 1.0;
        }

        playerSupply = std::make_tuple(false, 50, 0);
        petSupply = std::make_tuple(false, 50, 0);
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

    void on_checkBox_AutoPlayerHealth_toggled(bool checked);

    void on_checkBox_AutoPetSupply_toggled(bool checked);

    void on_comboBox_GameWindows_currentIndexChanged(int index);

    void on_pushButton_SaveConfigAs_clicked();

    void on_pushButton_RenameConfig_clicked();

    void on_comboBox_Configs_currentIndexChanged(int index);

    void on_checkBox_Switch_toggled(bool checked);

    void on_pushButton_DeleteConfig_clicked();

    void on_pushButton_ReadImage_clicked();

    void on_pushButton_TestPlayerSupply_clicked();

    void on_pushButton_TestPetSupply_clicked();

    void on_pushButton_ChangeWindowTitle_clicked();

private:
    Ui::MainWindow *ui;

    //角色名取样区域
    const QRect characterNameRect{ 80,22,90,14 };
    //人物血条取样区域
    const QRect playerHealthRect{ 81,38,89,7 };
    //宠物血条蓝条取样区域
    const QRect petResourceRect{ 21,103,34,34 };

    //计时部分
    QTimer pressTimer; //固定间隔按键的计时器
    QTimer supplyTimer; //自动补给的计时器

    //10个界面控件
    std::array<std::pair<QCheckBox *, QDoubleSpinBox *>, 10> enumerateControls;
    std::array<LARGE_INTEGER, 10> timeStamps; //每个按键上次触发的时间点
    LARGE_INTEGER counterFrequency;

    //游戏窗口的句柄
    std::vector<HWND> gameWindows;

    //当前配置的名称，切换时用
    QString currentConfigName;

    //定时器执行的函数
    void pressTimerProc();
    void supplyTimerProc();

    //重置按键的计时
    void resetTimeStamp(int index);
    void resetAllTimeStamps();

    //计算时间差
    double getCountersDiffInSeconds(LARGE_INTEGER past, LARGE_INTEGER now);

    //扫描游戏窗口
    void updateGameWindows();
    //对窗口发送Fx按键消息
    void pressFunctionKey(HWND window, UINT code);

    //截取游戏窗口的某个区域
    QImage getGamePicture(HWND window, QRect rect);

    QImage sampleImage;

    void applyBlankPixmapForPlayer();
    void applyBlankPixmapForPet();
    void applyBlankPixmapForSample();

    //按照设定百分比截取血条图片中的一点
    QPoint getPlayerHealthSamplePoint(QImage image, int percent);
    QPair<QPoint, QPoint> getPetResourceSamplePoints(QImage image, int percent);
    //像素归一化
    std::array<float, 3> normalizePixel(QRgb pixel);
    //判断一点是否为空血条
    bool isPetLowResource(QRgb pixel);
    bool isPlayerLowHealth(QRgb pixel);

    //扫描配置文件
    void scanConfigs();

    //计算?配置文件的路径
    QString getConfigPath(const QString &name);

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
};
#endif // MAINWINDOW_H
