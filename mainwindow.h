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

struct SConfigData
{
    std::array<std::pair<bool, float>, 10> presses; //启用, 间隔
    std::tuple<bool, int, int> playerSupply; //启用, 百分比, 按键
    std::tuple<bool, int, int> petSupply; //启用, 百分比, 按键
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private slots:
    void on_pushButton_UpdateGameWindows_clicked();

    void on_any_checkBox_toggled(bool checked);

    void on_checkBox_Switch_clicked(bool checked);

    void on_pushButton_TestKey_clicked();

private:
    Ui::MainWindow *ui;

    //角色名取样区域
    const QRect characterNameRect{ 80,22,90,14 };
    //人物血条取样区域
    const QRect playerHealthRect{ 80,38,90,7 };
    //宠物血条蓝条取样区域
    const QRect petResourceRect{ 21,103,34,34 };

    //计时部分
    QTimer pressTimer; //固定间隔按键的计时器
    QTimer supplyTimer; //自动补给的计时器

    std::array<LARGE_INTEGER, 10> timeStamps; //每个按键上次触发的时间点
    LARGE_INTEGER counterFrequency;

    //游戏窗口的句柄
    std::vector<HWND> gameWindows;

    //10个界面控件
    std::array<std::pair<QCheckBox *, QDoubleSpinBox *>, 10> enumrateControls;

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

    //按照设定百分比截取血条图片中的一点，允许失败，百分比为100制
    QPoint getPlayerHealthSamplePoint(QImage image, int percent);
    QPair<QPoint, QPoint> getPetResourceSamplePoints(QImage image, int percent);
    std::array<float, 3> normalizePixel(QRgb pixel);
    bool isPixelLowResource(QRgb pixel);

    QString getConfigPath();
    void readConfig(const QString &filename);
    void writeConfig(const QString &filename);
    void autoWriteConfig();

    SConfigData getConfigFromUI();
    void applyConfigToUI(const SConfigData &config);

    QJsonObject configToJson(const SConfigData &config);
    SConfigData jsonToConfig(QJsonObject json);
};
#endif // MAINWINDOW_H
