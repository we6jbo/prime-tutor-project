#pragma once
#include <QMainWindow>
#include <QSettings>
class QLabel; class QSpinBox; class QTextEdit; class QTabWidget; class QFontComboBox; class QDoubleSpinBox; class QCheckBox; class QPushButton;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
private slots:
    void checkNumber();
    void generateExample();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void saveLayout();
    void resetLayout();
private:
    bool isPrime(long long n) const;
    QString explainNumber(long long n) const;
    void buildUi();
    void loadSettings();
    void applyFontScale();
    void ensureTgSnapshot();
    void tryRegisterProject();
    void loadMachineContext();
    QString configPath() const;
    QTabWidget *tabs{};
    QSpinBox *numberInput{};
    QTextEdit *resultBox{};
    QLabel *contextLabel{};
    QLabel *tgLabel{};
    QFontComboBox *fontFamily{};
    QDoubleSpinBox *fontSize{};
    QCheckBox *compactMode{};
    QPushButton *saveButton{};
    double scaleFactor{1.0};
    QSettings *settings{};
};
