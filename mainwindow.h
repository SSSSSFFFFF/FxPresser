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

struct SConfigData
{
    QString title;
    std::array<std::pair<bool, double>, 10> fxPresses; //F*
    double interval;

    SConfigData()
    {
        fxPresses.fill(std::make_pair(false, 0.1));
        interval = 0.7;
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

private:
    Ui::MainWindow *ui;

    //角色名取样区域
    const QRect characterNameRect{ 80,22,90,14 };

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
    //上次吃宝宝糖果的时间点，间隔固定为5秒
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
    void writeGlobalConfig();
    void readGlobalConfig();

    static std::array<float, 3> rgb2HSV(QRgb rgbColor);
};
#endif // MAINWINDOW_H
