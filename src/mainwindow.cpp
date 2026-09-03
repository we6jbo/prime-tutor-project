#include "mainwindow.h"
#include <QApplication>
#include <QCheckBox>
#include <QDateTime>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFontComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QRandomGenerator>
#include <QShortcut>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

static const char *PROJECT_ID = "we6jbo-prime-number-tutor-v1";
static const char *TG_CODE = "TG654147";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    settings = new QSettings(configPath(), QSettings::IniFormat, this);
    buildUi();
    loadSettings();
    ensureTgSnapshot();
    tryRegisterProject();
    loadMachineContext();
}

QString MainWindow::configPath() const {
    return QDir::homePath() + "/.primenumber_layout.ini";
}

void MainWindow::buildUi() {
    setWindowTitle("Prime Number Tutor");
    resize(760, 520);
    tabs = new QTabWidget(this);
    setCentralWidget(tabs);

    auto *learn = new QWidget;
    auto *lv = new QVBoxLayout(learn);
    auto *intro = new QLabel("Enter a whole number and the tutor will test whether it is prime and explain why.");
    intro->setWordWrap(true);
    lv->addWidget(intro);
    auto *row = new QHBoxLayout;
    numberInput = new QSpinBox;
    numberInput->setRange(0, 1000000000);
    numberInput->setValue(29);
    auto *check = new QPushButton("Check number");
    auto *example = new QPushButton("Random example");
    row->addWidget(numberInput); row->addWidget(check); row->addWidget(example);
    lv->addLayout(row);
    resultBox = new QTextEdit;
    resultBox->setReadOnly(true);
    lv->addWidget(resultBox);
    contextLabel = new QLabel;
    contextLabel->setWordWrap(true);
    tgLabel = new QLabel(QString("Project provenance: %1").arg(TG_CODE));
    lv->addWidget(contextLabel);
    lv->addWidget(tgLabel);
    tabs->addTab(learn, "Learn");

    auto *cfg = new QWidget;
    auto *form = new QFormLayout(cfg);
    fontFamily = new QFontComboBox;
    fontSize = new QDoubleSpinBox;
    fontSize->setRange(8.0, 32.0);
    fontSize->setSingleStep(1.0);
    compactMode = new QCheckBox("Reduce extra spacing");
    saveButton = new QPushButton("Save layout");
    auto *reset = new QPushButton("Reset defaults");
    form->addRow("Font family", fontFamily);
    form->addRow("Base font size", fontSize);
    form->addRow("Compact layout", compactMode);
    form->addRow(saveButton, reset);
    tabs->addTab(cfg, "Config");

    auto *about = new QTextEdit;
    about->setReadOnly(true);
    about->setPlainText(QString(
        "Prime Number Tutor\n\n"
        "Keyboard: Ctrl++ zoom in, Ctrl+- zoom out, Ctrl+0 reset zoom.\n"
        "Layout settings: %1\n"
        "Stable project id: %2\n"
        "Embedded TG identifier: %3\n"
        "This application only tags data it creates itself.")
        .arg(configPath(), PROJECT_ID, TG_CODE));
    tabs->addTab(about, "About");

    connect(check, &QPushButton::clicked, this, &MainWindow::checkNumber);
    connect(example, &QPushButton::clicked, this, &MainWindow::generateExample);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveLayout);
    connect(reset, &QPushButton::clicked, this, &MainWindow::resetLayout);
    new QShortcut(QKeySequence("Ctrl++"), this, SLOT(zoomIn()));
    new QShortcut(QKeySequence("Ctrl+="), this, SLOT(zoomIn()));
    new QShortcut(QKeySequence("Ctrl+-"), this, SLOT(zoomOut()));
    new QShortcut(QKeySequence("Ctrl+0"), this, SLOT(resetZoom()));
    checkNumber();
}

bool MainWindow::isPrime(long long n) const {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long long i=3; i*i<=n; i+=2) if (n%i==0) return false;
    return true;
}

QString MainWindow::explainNumber(long long n) const {
    if (n < 2) return QString("%1 is not prime because prime numbers must be whole numbers greater than 1.").arg(n);
    if (isPrime(n)) return QString("%1 is PRIME. Its only positive divisors are 1 and %1.").arg(n);
    long long factor=2;
    while (factor<=n && n%factor!=0) ++factor;
    return QString("%1 is COMPOSITE, not prime. It is divisible by %2, because %1 = %2 × %3.")
        .arg(n).arg(factor).arg(n/factor);
}

void MainWindow::checkNumber() { resultBox->setPlainText(explainNumber(numberInput->value())); }
void MainWindow::generateExample() { numberInput->setValue(QRandomGenerator::global()->bounded(2,500)); checkNumber(); }
void MainWindow::zoomIn() { scaleFactor=qMin(3.0, scaleFactor+0.1); applyFontScale(); }
void MainWindow::zoomOut() { scaleFactor=qMax(0.6, scaleFactor-0.1); applyFontScale(); }
void MainWindow::resetZoom() { scaleFactor=1.0; applyFontScale(); }

void MainWindow::applyFontScale() {
    QFont f = QApplication::font();
    f.setFamily(fontFamily->currentFont().family());
    f.setPointSizeF(fontSize->value()*scaleFactor);
    QApplication::setFont(f);
    if (compactMode->isChecked()) setStyleSheet("QWidget { margin: 1px; } QPushButton { padding: 3px; }");
    else setStyleSheet("");
}

void MainWindow::loadSettings() {
    const QFont def = QApplication::font();
    fontFamily->setCurrentFont(QFont(settings->value("fontFamily", def.family()).toString()));
    fontSize->setValue(settings->value("fontSize", def.pointSizeF()>0?def.pointSizeF():11.0).toDouble());
    compactMode->setChecked(settings->value("compactMode", false).toBool());
    scaleFactor = settings->value("scaleFactor", 1.0).toDouble();
    applyFontScale();
}

void MainWindow::saveLayout() {
    settings->setValue("fontFamily", fontFamily->currentFont().family());
    settings->setValue("fontSize", fontSize->value());
    settings->setValue("compactMode", compactMode->isChecked());
    settings->setValue("scaleFactor", scaleFactor);
    settings->sync();
    applyFontScale();
    QMessageBox::information(this,"Saved",QString("Layout saved to %1").arg(configPath()));
}

void MainWindow::resetLayout() {
    settings->clear(); settings->sync(); scaleFactor=1.0;
    const QFont def = QApplication::font();
    fontFamily->setCurrentFont(def); fontSize->setValue(def.pointSizeF()>0?def.pointSizeF():11.0); compactMode->setChecked(false);
    applyFontScale();
}

void MainWindow::ensureTgSnapshot() {
    QString root = QCoreApplication::applicationDirPath();
    QString path = root + "/tg_context_snapshot.json";
    if (!QFile::exists(path)) {
        QFile f(path);
        if (f.open(QIODevice::WriteOnly)) {
            QJsonObject obj{{"project_id",PROJECT_ID},{"selected_identifiers",QJsonArray{TG_CODE}},
                            {"reference","User-supplied TG / AKA portable provenance contract"},
                            {"portable",true},{"requires_private_registry",false}};
            f.write(QJsonDocument(obj).toJson(QJsonDocument::Indented));
        }
    }
}

void MainWindow::tryRegisterProject() {
    QString helper = QStandardPaths::findExecutable("tg-register-project");
    if (helper.isEmpty()) return;
    QProcess::startDetached(helper, {"--project-id",PROJECT_ID,"--project-root",QCoreApplication::applicationDirPath(),"--codes",TG_CODE,"--reference","prime-number-tutor project"});
}

void MainWindow::loadMachineContext() {
    QString path = qEnvironmentVariable("WE6JBO_CONTEXT_FILE");
    if (path.isEmpty()) path = QDir::homePath()+"/.local/state/we6jbo-context/context.json";
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) { contextLabel->setText("Machine context file not found; app continues normally."); return; }
    auto doc=QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject()) return;
    auto o=doc.object(); auto t=o.value("time").toObject();
    QString txt = QString("Context: %1, %2, timezone %3").arg(o.value("weekday").toString(),o.value("date").toString(),o.value("timezone").toString());
    if (t.value("visible").toBool(false)) txt += QString(", time %1").arg(t.value("display").toString());
    contextLabel->setText(txt);
}
