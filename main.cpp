#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QProgressBar>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFileInfo>
#include <QTimer>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QLocale>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QPixmap>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QPalette>
#include <QMoveEvent>
#include <QCloseEvent>
#include <unistd.h>

// Sistem temasına (açık/koyu) ve dosya konumlarına göre logo yükleyici
static QIcon getAppIcon() {
    bool isDark = QApplication::palette().color(QPalette::Window).lightness() < 128;
    QString whiteLogo = "naga-zapret-logo-white.svg";
    QString blackLogo = "naga-zapret-logo.svg";
    QString primary = isDark ? whiteLogo : blackLogo;
    QString secondary = isDark ? blackLogo : whiteLogo;

    QStringList searchDirs = {
        "/usr/share/naga-zapret/",
        "/usr/share/naga-zapret/icons/",
        "/usr/share/icons/hicolor/scalable/apps/",
        QCoreApplication::applicationDirPath() + "/",
        QCoreApplication::applicationDirPath() + "/assets/",
        QDir::currentPath() + "/",
        QDir::currentPath() + "/assets/"
    };

    // 1. Tercih edilen tema rengindeki logoyu ara
    for (const QString &dir : searchDirs) {
        if (QFileInfo::exists(dir + primary)) {
            QIcon icon(dir + primary);
            if (!icon.isNull()) return icon;
        }
    }
    // 2. Alternatif logoyu ara
    for (const QString &dir : searchDirs) {
        if (QFileInfo::exists(dir + secondary)) {
            QIcon icon(dir + secondary);
            if (!icon.isNull()) return icon;
        }
    }

    // 3. Dosyalar henüz dizine eklenmemişse vektörel fallback simgesi üret
    QPixmap pix(64, 64);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor(isDark ? "#96335d" : "#7d2a4e"));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(4, 4, 56, 56, 12, 12);
    p.setBrush(Qt::white);
    QPolygonF triangle;
    triangle << QPointF(24, 18) << QPointF(46, 32) << QPointF(24, 46);
    p.drawPolygon(triangle);
    return QIcon(pix);
}

// Harici font veya resim dosyası gerektirmeyen vektörel ikon üreticileri
static QPixmap createGlobePixmap(int size, const QColor &color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(color, 1.5);
    p.setPen(pen);
    p.drawEllipse(1, 1, size - 2, size - 2);
    p.drawLine(1, size / 2, size - 1, size / 2);
    p.drawEllipse(size / 4, 1, size / 2, size - 2);
    return pix;
}

static QIcon createPlayIcon(int size, const QColor &color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    QPolygonF triangle;
    triangle << QPointF(size * 0.25, size * 0.15)
             << QPointF(size * 0.85, size * 0.5)
             << QPointF(size * 0.25, size * 0.85);
    p.drawPolygon(triangle);
    return QIcon(pix);
}

static QIcon createStopIcon(int size, const QColor &color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(QRectF(size * 0.2, size * 0.2, size * 0.6, size * 0.6), 2, 2);
    return QIcon(pix);
}

static QIcon createRadarIcon(int size, const QColor &color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(color, 1.6, Qt::SolidLine, Qt::RoundCap);
    p.setPen(pen);
    p.drawArc(2, 2, size - 4, size - 4, 45 * 16, 90 * 16);
    p.drawArc(4, 4, size - 8, size - 8, 45 * 16, 90 * 16);
    p.setBrush(color);
    p.drawEllipse(size / 2 - 2, size / 2 - 2, 4, 4);
    p.drawLine(size / 2, size / 2, size * 0.8, size * 0.2);
    return QIcon(pix);
}

static QIcon createTerminalIcon(int size, const QColor &color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(color, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p.setPen(pen);
    p.drawLine(size * 0.2, size * 0.25, size * 0.45, size * 0.5);
    p.drawLine(size * 0.45, size * 0.5, size * 0.2, size * 0.75);
    p.drawLine(size * 0.55, size * 0.75, size * 0.82, size * 0.75);
    return QIcon(pix);
}

static QPixmap createStatusDot(const QColor &color) {
    QPixmap pix(10, 10);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawEllipse(1, 1, 8, 8);
    return pix;
}

// Dönen yükleniyor animasyonu widget'ı
class SpinnerWidget : public QWidget {
    Q_OBJECT
    int angle = 0;
    QTimer *timer;
    bool spinning = false;
public:
    SpinnerWidget(QWidget *parent = nullptr) : QWidget(parent) {
        setFixedSize(22, 22);
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this]() {
            angle = (angle + 30) % 360;
            update();
        });
    }

    void start() {
        spinning = true;
        if (!timer->isActive()) timer->start(60);
        show();
        update();
    }

    void stop() {
        spinning = false;
        timer->stop();
        hide();
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override {
        if (!spinning) return;
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.translate(width() / 2, height() / 2);
        p.rotate(angle);

        QPen pen(QColor("#c04278"), 2.5, Qt::SolidLine, Qt::RoundCap);
        p.setPen(pen);
        p.drawArc(-8, -8, 16, 16, 0, 270 * 16);
    }
};

// Ana pencerenin yanına yapışan (snap olan) Canlı Günlük Penceresi
class LogWindow : public QWidget {
    Q_OBJECT
    QTextEdit *logArea;
    QLabel *lblTitle;
    QPushButton *btnClear;
    std::function<void()> onCloseCallback;
public:
    LogWindow(QWidget *parent = nullptr) : QWidget(parent, Qt::Window) {
        setWindowTitle("Naga Zapret - Live Logs");
        resize(440, 480);
        setFocusPolicy(Qt::StrongFocus);
        setWindowIcon(getAppIcon());

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(14, 14, 14, 14);
        layout->setSpacing(10);

        auto *headerLayout = new QHBoxLayout();
        lblTitle = new QLabel(this);
        lblTitle->setStyleSheet("font-weight: bold; font-size: 13px; color: #F3F4F4;");
        headerLayout->addWidget(lblTitle);
        headerLayout->addStretch();

        btnClear = new QPushButton(this);
        btnClear->setObjectName("btnClearLog");
        btnClear->setCursor(Qt::PointingHandCursor);
        connect(btnClear, &QPushButton::clicked, this, [this]() {
            logArea->clear();
        });
        headerLayout->addWidget(btnClear);
        layout->addLayout(headerLayout);

        logArea = new QTextEdit(this);
        logArea->setReadOnly(true);
        logArea->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
        layout->addWidget(logArea);

        updateTexts(false);
    }

    void setOnCloseCallback(std::function<void()> cb) { onCloseCallback = cb; }

    void updateTexts(bool isEnglish) {
        lblTitle->setText(isEnglish ? "Live Console & Logs" : "Canlı Sistem Günlüğü");
        btnClear->setText(isEnglish ? "Clear Log" : "Günlüğü Temizle");
        setWindowTitle(isEnglish ? "Naga Zapret - Live Logs" : "Naga Zapret - Canlı Günlük");
    }

    void appendLog(const QString &text) {
        logArea->append(text);
        logArea->ensureCursorVisible();
        QApplication::processEvents();
    }

protected:
    void closeEvent(QCloseEvent *event) override {
        event->ignore();
        hide();
        if (onCloseCallback) onCloseCallback();
    }
};

// Arka plan başlatıcı ve ağ test edici (--startup-check argümanı için)
class StartupChecker : public QObject {
    Q_OBJECT
    int attempts = 0;
    bool isEn = false;
    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayMenu = nullptr;
    QAction *actStatus = nullptr;
public:
    StartupChecker(QObject *parent = nullptr) : QObject(parent) {
        isEn = !QLocale::system().name().startsWith("tr", Qt::CaseInsensitive);
        setupTrayIcon();
    }

    void setupTrayIcon() {
        trayIcon = new QSystemTrayIcon(getAppIcon(), this);
        trayMenu = new QMenu();
        trayMenu->setStyleSheet(R"(
            QMenu { 
                background-color: #1e1b24; 
                color: #f1f5f9; 
                border: 1px solid #3d2438; 
                padding: 4px; 
                border-radius: 4px; 
                font-family: 'Segoe UI', Ubuntu, sans-serif;
            }
            QMenu::item { 
                padding: 7px 22px; 
                font-size: 12px; 
                border-radius: 3px; 
            }
            QMenu::item:selected { 
                background-color: #96335d; 
                color: #ffffff; 
            }
            QMenu::separator { 
                height: 1px; 
                background: #3d2438; 
                margin: 4px 8px; 
            }
        )");

        actStatus = trayMenu->addAction(isEn ? "Status: Checking connection..." : "Durum: Bağlantı denetleniyor...");
        actStatus->setEnabled(false);

        trayMenu->addSeparator();

        auto *actOpen = trayMenu->addAction(isEn ? "Open Naga Zapret" : "Naga Zapret'i Aç");
        connect(actOpen, &QAction::triggered, this, []() {
            QProcess::startDetached(QApplication::applicationFilePath(), QStringList());
        });

        auto *actRecheck = trayMenu->addAction(isEn ? "Re-check Connection" : "Yeniden Kontrol Et");
        connect(actRecheck, &QAction::triggered, this, [this]() {
            attempts = 0;
            updateTrayStatus(isEn ? "Status: Checking... (1/3)" : "Durum: Kontrol ediliyor... (1/3)");
            pingTest();
        });

        trayMenu->addSeparator();

        auto *actQuit = trayMenu->addAction(isEn ? "Exit Check" : "Kontrolü Kapat");
        connect(actQuit, &QAction::triggered, qApp, &QApplication::quit);

        trayIcon->setContextMenu(trayMenu);
        trayIcon->setToolTip(isEn ? "Naga Zapret - Background Connection Check" : "Naga Zapret - Arka Plan Bağlantı Denetimi");
        trayIcon->show();

        connect(trayIcon, &QSystemTrayIcon::activated, this, [](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick || reason == QSystemTrayIcon::Trigger) {
                QProcess::startDetached(QApplication::applicationFilePath(), QStringList());
            }
        });
    }

    void updateTrayStatus(const QString &text) {
        if (actStatus) actStatus->setText(text);
        if (trayIcon) trayIcon->setToolTip(QString("Naga Zapret - %1").arg(text));
    }

    void runCheck() {
        updateTrayStatus(isEn ? "Status: Checking... (1/3)" : "Durum: Kontrol ediliyor... (1/3)");
        QTimer::singleShot(5000, this, &StartupChecker::pingTest);
    }

private:
    void pingTest() {
        attempts++;
        updateTrayStatus(isEn 
            ? QString("Status: Testing Discord (%1/3)...").arg(attempts) 
            : QString("Durum: Discord Test Ediliyor (%1/3)...").arg(attempts));

        auto *manager = new QNetworkAccessManager(this);
        QNetworkRequest request((QUrl("https://discord.com")));
        auto *reply = manager->get(request);
        auto *timer = new QTimer(this);
        timer->setSingleShot(true);

        auto cleanup = [manager, reply, timer]() {
            timer->stop(); timer->deleteLater();
            reply->deleteLater(); manager->deleteLater();
        };

        connect(timer, &QTimer::timeout, [this, cleanup]() {
            cleanup(); handleFailure();
        });

        connect(reply, &QNetworkReply::finished, [this, reply, timer, cleanup]() {
            if (timer->isActive()) {
                timer->stop();
                if (reply->error() == QNetworkReply::NoError) {
                    cleanup();
                    updateTrayStatus(isEn ? "Status: Connected (Active)" : "Durum: Bağlantı Başarılı (Aktif)");
                    if (trayIcon) {
                        trayIcon->showMessage(
                            "Naga Zapret",
                            isEn ? "DPI bypass is active. Connection healthy." : "Sansür aşma servisi aktif. Bağlantı sorunsuz.",
                            QSystemTrayIcon::Information,
                            3000
                        );
                    }
                    QTimer::singleShot(4000, qApp, &QApplication::quit);
                } else {
                    cleanup(); 
                    handleFailure();
                }
            }
        });
        timer->start(3000);
    }

    void handleFailure() {
        if (attempts < 3) {
            updateTrayStatus(isEn 
                ? QString("Status: Retrying (%1/3)...").arg(attempts + 1)
                : QString("Durum: Yeniden deneniyor (%1/3)...").arg(attempts + 1));
            QTimer::singleShot(5000, this, &StartupChecker::pingTest);
        } else {
            updateTrayStatus(isEn ? "Status: Connection Issue Detected" : "Durum: Bağlantı Sorunu Algılandı");
            askUser();
        }
    }

    void askUser() {
        QMessageBox msgBox;
        msgBox.setWindowIcon(getAppIcon());
        msgBox.setWindowTitle(isEn ? "Naga Zapret - Connection Error" : "Naga Zapret - Bağlantı Hatası");
        msgBox.setText(isEn 
            ? "A connection issue with Discord/Roblox was detected at system startup.\n\nWould you like to reinstall/restart the censorship bypass service?"
            : "Sistem açılışında Discord/Roblox bağlantısında sorun tespit edildi.\n\nSansür aşma servisini yeniden kurmak/başlatmak ister misiniz?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setStyleSheet("background-color: #1a1a20; color: #F3F4F4; font-size: 13px;");

        if (msgBox.exec() == QMessageBox::Yes) {
            QProcess::startDetached("pkexec", QStringList() << "bash" << "/usr/share/naga-zapret/install.sh");
        }
        QApplication::quit();
    }
};

class ZapretWindow : public QWidget {
    Q_OBJECT
public:
    enum class StatusState {
        Checking,
        Installed,
        NotInstalled,
        Installing,
        Running,
        Removing,
        Removed
    };

    enum class TestStatus {
        Untested,
        Waiting,
        Testing,
        Success,
        Failed
    };

    ZapretWindow() {
        setWindowTitle("Naga Zapret - DPI Bypass Tool");
        setWindowIcon(getAppIcon());
        setFixedSize(560, 480);

        logWindow = new LogWindow(nullptr);
        logWindow->setOnCloseCallback([this]() {
            updateToggleLogText();
        });

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setSpacing(14);
        mainLayout->setContentsMargins(22, 22, 22, 22);

        // Header
        auto *headerLayout = new QHBoxLayout();
        auto *lblTitle = new QLabel("Naga Zapret", this);
        lblTitle->setObjectName("title");
        headerLayout->addWidget(lblTitle);

        auto *lblSubtitle = new QLabel("Control Center", this);
        lblSubtitle->setObjectName("subtitle");
        headerLayout->addWidget(lblSubtitle);
        headerLayout->addStretch();

        lblGlobe = new QLabel(this);
        lblGlobe->setPixmap(createGlobePixmap(16, QColor("#cbd5e1")));
        headerLayout->addWidget(lblGlobe);

        lblLang = new QLabel("Dil:", this);
        lblLang->setObjectName("lblLang");
        headerLayout->addWidget(lblLang);

        langCombo = new QComboBox(this);
        langCombo->setObjectName("langCombo");
        langCombo->addItem("Türkçe", "tr");
        langCombo->addItem("English", "en");
        langCombo->setCursor(Qt::PointingHandCursor);

        bool isSystemTr = QLocale::system().name().startsWith("tr", Qt::CaseInsensitive);
        isEnglish = !isSystemTr;
        langCombo->setCurrentIndex(isEnglish ? 1 : 0);

        headerLayout->addWidget(langCombo);
        mainLayout->addLayout(headerLayout);

        // Status Card
        auto *statusCard = new QWidget(this);
        statusCard->setObjectName("cardStatus");
        auto *statusLayout = new QHBoxLayout(statusCard);
        statusLayout->setContentsMargins(16, 14, 16, 14);
        
        statusIconLabel = new QLabel(this);
        statusIconLabel->setPixmap(createStatusDot(QColor("#94a3b8")));
        statusLayout->addWidget(statusIconLabel);

        statusLabel = new QLabel(this);
        statusLabel->setObjectName("status");
        statusLayout->addWidget(statusLabel);
        statusLayout->addStretch();
        mainLayout->addWidget(statusCard);

        // Actions
        auto *actionLayout = new QHBoxLayout();
        actionLayout->setSpacing(12);

        btnStart = new QPushButton(this);
        btnStart->setObjectName("btnStart");
        btnStart->setCursor(Qt::PointingHandCursor);
        btnStart->setIcon(createPlayIcon(14, QColor("#FFFFFF")));

        btnStop = new QPushButton(this);
        btnStop->setObjectName("btnStop");
        btnStop->setCursor(Qt::PointingHandCursor);
        btnStop->setIcon(createStopIcon(14, QColor("#F3F4F4")));

        actionLayout->addWidget(btnStart);
        actionLayout->addWidget(btnStop);
        mainLayout->addLayout(actionLayout);

        // Test Card
        auto *testCard = new QWidget(this);
        testCard->setObjectName("card");
        auto *testLayout = new QVBoxLayout(testCard);
        testLayout->setContentsMargins(16, 14, 16, 14);
        testLayout->setSpacing(12);

        auto *testHeaderLayout = new QHBoxLayout();
        lblTestTitle = new QLabel(this);
        lblTestTitle->setStyleSheet("font-weight: 700; font-size: 13px; color: #f1f5f9;");
        
        btnTest = new QPushButton(this);
        btnTest->setObjectName("btnTest");
        btnTest->setCursor(Qt::PointingHandCursor);
        btnTest->setIcon(createRadarIcon(14, QColor("#f1f5f9")));

        testHeaderLayout->addWidget(lblTestTitle);
        testHeaderLayout->addStretch();
        testHeaderLayout->addWidget(btnTest);
        testLayout->addLayout(testHeaderLayout);

        discordLabel = new QLabel(this);
        robloxLabel = new QLabel(this);
        discordLabel->setStyleSheet("color: #94a3b8; font-size: 13px;");
        robloxLabel->setStyleSheet("color: #94a3b8; font-size: 13px;");

        testLayout->addWidget(discordLabel);
        testLayout->addWidget(robloxLabel);
        mainLayout->addWidget(testCard);

        // Progress bar ve spinner
        progressContainer = new QWidget(this);
        auto *progressLayout = new QHBoxLayout(progressContainer);
        progressLayout->setContentsMargins(0, 4, 0, 0);
        progressLayout->setSpacing(10);

        spinner = new SpinnerWidget(this);
        progressBar = new QProgressBar(this);
        progressBar->setTextVisible(true);
        progressBar->setAlignment(Qt::AlignCenter);

        progressLayout->addWidget(spinner);
        progressLayout->addWidget(progressBar);
        progressContainer->setVisible(false);
        mainLayout->addWidget(progressContainer);

        // Yan pencereyi açan günlük butonu
        btnToggleLog = new QPushButton(this);
        btnToggleLog->setObjectName("btnToggleLog");
        btnToggleLog->setCursor(Qt::PointingHandCursor);
        btnToggleLog->setIcon(createTerminalIcon(14, QColor("#cbd5e1")));
        mainLayout->addWidget(btnToggleLog);

        connect(langCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
            isEnglish = (index == 1);
            langCombo->hidePopup();
            updateUiTexts();
        });

        connect(btnToggleLog, &QPushButton::clicked, this, &ZapretWindow::toggleLogWindow);
        connect(btnStart, &QPushButton::clicked, this, &ZapretWindow::startZapret);
        connect(btnStop, &QPushButton::clicked, this, &ZapretWindow::stopZapret);
        connect(btnTest, &QPushButton::clicked, this, &ZapretWindow::startMultiTests);

        setupTrayIcon();
        updateUiTexts();
        checkStatus();
    }

protected:
    void moveEvent(QMoveEvent *event) override {
        QWidget::moveEvent(event);
        if (logWindow && logWindow->isVisible()) {
            snapLogWindow();
        }
    }

    void closeEvent(QCloseEvent *event) override {
        // Çıkış doğrudan tepsiden tetiklenmemişse pencereyi kapatmak yerine tepsiye gizle
        if (!isQuitting && trayIcon && trayIcon->isVisible()) {
            event->ignore();
            hide();
            if (logWindow && logWindow->isVisible()) {
                logWindow->hide();
            }
            if (!hasShownTrayNotification) {
                trayIcon->showMessage(
                    "Naga Zapret",
                    isEnglish ? "Application is running in the background. Click the tray icon to reopen."
                              : "Uygulama arka planda çalışmaya devam ediyor. Açmak için tepsi simgesine tıklayın.",
                    QSystemTrayIcon::Information,
                    2500
                );
                hasShownTrayNotification = true;
            }
        } else {
            if (logWindow) {
                logWindow->deleteLater();
            }
            QWidget::closeEvent(event);
        }
    }

private:
    bool isEnglish = false;
    bool isQuitting = false;
    bool hasShownTrayNotification = false;
    StatusState currentStatus = StatusState::Checking;

    TestStatus discordTestState = TestStatus::Untested;
    int discordProgress = 0;
    int discordSuccess = 0;

    TestStatus robloxTestState = TestStatus::Untested;
    int robloxProgress = 0;
    int robloxSuccess = 0;

    QSystemTrayIcon *trayIcon = nullptr;
    QMenu *trayMenu = nullptr;
    QAction *actTrayStatus = nullptr;
    QAction *actTrayShow = nullptr;
    QAction *actTrayTest = nullptr;
    QAction *actTrayLog = nullptr;
    QAction *actTrayQuit = nullptr;

    LogWindow *logWindow;
    QLabel *lblGlobe;
    QLabel *lblLang;
    QComboBox *langCombo;
    QLabel *statusIconLabel;
    QLabel *statusLabel;
    QLabel *lblTestTitle;
    QLabel *discordLabel;
    QLabel *robloxLabel;
    QWidget *progressContainer;
    SpinnerWidget *spinner;
    QProgressBar *progressBar;
    QPushButton *btnToggleLog;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QPushButton *btnTest;

    bool isRoot() { return geteuid() == 0; }

    void setupTrayIcon() {
        trayIcon = new QSystemTrayIcon(getAppIcon(), this);
        trayMenu = new QMenu(this);
        trayMenu->setStyleSheet(R"(
            QMenu { 
                background-color: #1e1b24; 
                color: #f1f5f9; 
                border: 1px solid #3d2438; 
                padding: 4px; 
                border-radius: 4px; 
                font-family: 'Segoe UI', Ubuntu, sans-serif;
            }
            QMenu::item { 
                padding: 7px 22px; 
                font-size: 12px; 
                border-radius: 3px; 
            }
            QMenu::item:selected { 
                background-color: #96335d; 
                color: #ffffff; 
            }
            QMenu::separator { 
                height: 1px; 
                background: #3d2438; 
                margin: 4px 8px; 
            }
        )");

        actTrayStatus = trayMenu->addAction(isEnglish ? "Status: Checking..." : "Durum: Kontrol Ediliyor...");
        actTrayStatus->setEnabled(false);

        trayMenu->addSeparator();

        actTrayShow = trayMenu->addAction(isEnglish ? "Show Naga Zapret" : "Naga Zapret'i Göster");
        connect(actTrayShow, &QAction::triggered, this, [this]() {
            showNormal();
            raise();
            activateWindow();
        });

        actTrayTest = trayMenu->addAction(isEnglish ? "Run Connection Test" : "Bağlantı Testi Yap");
        connect(actTrayTest, &QAction::triggered, this, &ZapretWindow::startMultiTests);

        actTrayLog = trayMenu->addAction(isEnglish ? "Toggle Live Logs" : "Canlı Günlüğü Aç/Kapat");
        connect(actTrayLog, &QAction::triggered, this, &ZapretWindow::toggleLogWindow);

        trayMenu->addSeparator();

        actTrayQuit = trayMenu->addAction(isEnglish ? "Exit Completely" : "Tamamen Çıkış Yap");
        connect(actTrayQuit, &QAction::triggered, this, [this]() {
            isQuitting = true;
            if (logWindow) logWindow->deleteLater();
            trayIcon->hide();
            QApplication::quit();
        });

        trayIcon->setContextMenu(trayMenu);
        trayIcon->setToolTip("Naga Zapret Control Center");
        trayIcon->show();

        connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
                if (isVisible() && !isMinimized()) {
                    hide();
                    if (logWindow && logWindow->isVisible()) logWindow->hide();
                } else {
                    showNormal();
                    raise();
                    activateWindow();
                }
            }
        });
    }

    void snapLogWindow() {
        int targetX = this->geometry().x() + this->width() + 10;
        int targetY = this->geometry().y();
        logWindow->resize(440, this->height());
        logWindow->move(targetX, targetY);
    }

    void toggleLogWindow() {
        if (logWindow->isVisible()) {
            logWindow->hide();
        } else {
            snapLogWindow();
            logWindow->show();
            logWindow->raise();
            logWindow->activateWindow();
        }
        updateToggleLogText();
    }

    void appendLog(const QString &text) {
        logWindow->appendLog(text);
    }

    void setButtonsEnabled(bool enabled) {
        btnStart->setEnabled(enabled);
        btnStop->setEnabled(enabled);
        btnTest->setEnabled(enabled);
        langCombo->setEnabled(enabled);
    }

    void updateToggleLogText() {
        if (logWindow && logWindow->isVisible()) {
            btnToggleLog->setText(isEnglish ? "Hide Live Logs Console" : "Canlı Günlük Penceresini Kapat");
        } else {
            btnToggleLog->setText(isEnglish ? "Show Live Logs Console" : "Canlı Günlük Penceresini Aç");
        }
    }

    void updateUiTexts() {
        lblLang->setText(isEnglish ? "Language:" : "Dil:");
        lblTestTitle->setText(isEnglish ? "Access Status (10x Packet Test)" : "Erişim Durumu (10x Canlı Paket Testi)");
        btnStart->setText(isEnglish ? "Install & Start Zapret" : "Zapret Kur ve Başlat");
        btnStop->setText(isEnglish ? "Stop & Remove Zapret" : "Zapret Durdur ve Kaldır");
        btnTest->setText(isEnglish ? "Test Connection" : "Bağlantı Testi");

        if (actTrayShow) actTrayShow->setText(isEnglish ? "Show Naga Zapret" : "Naga Zapret'i Göster");
        if (actTrayTest) actTrayTest->setText(isEnglish ? "Run Connection Test" : "Bağlantı Testi Yap");
        if (actTrayLog) actTrayLog->setText(isEnglish ? "Toggle Live Logs" : "Canlı Günlüğü Aç/Kapat");
        if (actTrayQuit) actTrayQuit->setText(isEnglish ? "Exit Completely" : "Tamamen Çıkış Yap");

        logWindow->updateTexts(isEnglish);
        updateToggleLogText();
        updateStatusLabel();
        updateServiceLabel(discordLabel, "Discord", discordTestState, discordProgress, discordSuccess);
        updateServiceLabel(robloxLabel, "Roblox", robloxTestState, robloxProgress, robloxSuccess);
    }

    void setStatus(StatusState state) {
        currentStatus = state;
        updateStatusLabel();
    }

    void updateStatusLabel() {
        switch (currentStatus) {
            case StatusState::Checking:
                statusLabel->setText(isEnglish ? "Status: Checking..." : "Durum: Kontrol Ediliyor...");
                statusIconLabel->setPixmap(createStatusDot(QColor("#94a3b8")));
                break;
            case StatusState::Installed:
                statusLabel->setText(isEnglish ? "Status: Zapret Installed" : "Durum: Zapret Kurulu");
                statusIconLabel->setPixmap(createStatusDot(QColor("#4ade80")));
                break;
            case StatusState::NotInstalled:
                statusLabel->setText(isEnglish ? "Status: Not Installed" : "Durum: Kurulu Değil");
                statusIconLabel->setPixmap(createStatusDot(QColor("#cbd5e1")));
                break;
            case StatusState::Installing:
                statusLabel->setText(isEnglish ? "Status: Installing..." : "Durum: Kuruluyor...");
                statusIconLabel->setPixmap(createStatusDot(QColor("#fbbf24")));
                break;
            case StatusState::Running:
                statusLabel->setText(isEnglish ? "Status: Running Active" : "Durum: Aktif Çalışıyor");
                statusIconLabel->setPixmap(createStatusDot(QColor("#4ade80")));
                break;
            case StatusState::Removing:
                statusLabel->setText(isEnglish ? "Status: Removing..." : "Durum: Kaldırılıyor...");
                statusIconLabel->setPixmap(createStatusDot(QColor("#fbbf24")));
                break;
            case StatusState::Removed:
                statusLabel->setText(isEnglish ? "Status: Removed" : "Durum: Kaldırıldı");
                statusIconLabel->setPixmap(createStatusDot(QColor("#f87171")));
                break;
        }

        if (actTrayStatus) {
            actTrayStatus->setText(statusLabel->text());
        }
        if (trayIcon) {
            trayIcon->setToolTip(QString("Naga Zapret - %1").arg(statusLabel->text()));
        }
    }

    void updateServiceLabel(QLabel *lbl, const QString &serviceName, TestStatus state, int progress, int successCount) {
        switch (state) {
            case TestStatus::Untested:
                lbl->setText(QString("• %1: %2").arg(serviceName, isEnglish ? "Not Tested" : "Test Edilmedi"));
                lbl->setStyleSheet("color: #94a3b8; font-size: 13px;");
                break;
            case TestStatus::Waiting:
                lbl->setText(QString("• %1: %2").arg(serviceName, isEnglish ? "Waiting..." : "Bekliyor..."));
                lbl->setStyleSheet("color: #94a3b8; font-size: 13px;");
                break;
            case TestStatus::Testing:
                lbl->setText(QString("• %1: %2 (%3/10)").arg(serviceName, isEnglish ? "Testing" : "Test Ediliyor").arg(progress));
                lbl->setStyleSheet("color: #fbbf24; font-size: 13px; font-weight: bold;");
                break;
            case TestStatus::Success:
                lbl->setText(QString("• %1: %2 (%3/10 OK)").arg(serviceName, isEnglish ? "Accessible" : "Erişilebilir").arg(successCount));
                lbl->setStyleSheet("color: #4ade80; font-size: 13px; font-weight: bold;");
                break;
            case TestStatus::Failed:
                lbl->setText(QString("• %1: %2 (%3/10 OK)").arg(serviceName, isEnglish ? "Inaccessible" : "Erişilemedi").arg(successCount));
                lbl->setStyleSheet("color: #f87171; font-size: 13px; font-weight: bold;");
                break;
        }
    }

    void checkStatus() {
        if (QFileInfo::exists("/opt/zapret")) {
            setStatus(StatusState::Installed);
        } else {
            setStatus(StatusState::NotInstalled);
        }
    }

    void runCommandAsRoot(const QString &cmd, std::function<void(int)> onFinished) {
        auto *process = new QProcess(this);
        connect(process, &QProcess::readyReadStandardOutput, [this, process]() {
            appendLog(process->readAllStandardOutput().trimmed());
        });
        connect(process, &QProcess::readyReadStandardError, [this, process]() {
            appendLog(process->readAllStandardError().trimmed());
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [process, onFinished](int exitCode) {
            onFinished(exitCode);
            process->deleteLater();
        });

        if (isRoot()) {
            process->start("bash", QStringList() << "-c" << cmd);
        } else {
            process->start("pkexec", QStringList() << "bash" << "-c" << cmd);
        }
    }

    void startZapret() {
        setButtonsEnabled(false);
        progressContainer->setVisible(true);
        progressBar->setRange(0, 0);
        spinner->start();

        appendLog(isEnglish 
            ? "> Starting your custom installation script (install.sh)..." 
            : "> Sizin özel kurulum betiğiniz (install.sh) başlatılıyor...");
        setStatus(StatusState::Installing);

        QString cmd = "chmod +x /usr/share/naga-zapret/install.sh && bash /usr/share/naga-zapret/install.sh";

        runCommandAsRoot(cmd, [this](int exitCode) {
            if (exitCode == 0) {
                appendLog(isEnglish 
                    ? "> Installation script completed!" 
                    : "> Kurulum scripti tamamlandı!");
                setStatus(StatusState::Running);
                startMultiTests();
            } else {
                spinner->stop();
                progressBar->setRange(0, 100);
                progressBar->setValue(0);
                setButtonsEnabled(true);
                appendLog(isEnglish 
                    ? QString("> Error: Script stopped running (Exit: %1).").arg(exitCode)
                    : QString("> Hata: Script çalışmayı durdurdu (Çıkış: %1).").arg(exitCode));
                checkStatus();
            }
        });
    }

    void stopZapret() {
        setButtonsEnabled(false);
        progressContainer->setVisible(true);
        progressBar->setRange(0, 0);
        spinner->start();

        appendLog(isEnglish 
            ? "> Stopping Zapret and cleaning up system..." 
            : "> Zapret durduruluyor ve sistem temizleniyor...");
        setStatus(StatusState::Removing);

        QString cmd = "echo -e 'Y\\n\\n' | /opt/zapret/uninstall_easy.sh 2>/dev/null; "
                      "rm -rf /opt/zapret; "
                      "echo '> DNS kilidi çözülüyor...'; "
                      "chattr -i /etc/resolv.conf 2>/dev/null || true; "
                      "rm -f /etc/resolv.conf; "
                      "echo 'nameserver 8.8.8.8' > /etc/resolv.conf; "
                      "echo 'nameserver 1.1.1.1' >> /etc/resolv.conf; "
                      "systemctl stop dnscrypt-proxy 2>/dev/null || true; "
                      "echo '> Sistem temizlendi!'";

        runCommandAsRoot(cmd, [this](int exitCode) {
            spinner->stop();
            progressBar->setRange(0, 100);
            progressBar->setValue(100);
            setButtonsEnabled(true);
            appendLog(isEnglish 
                ? "> Zapret removed. Internet restored to normal." 
                : "> Zapret kaldırıldı. İnternet normale döndü.");
            setStatus(StatusState::Removed);
            checkStatus();
        });
    }

    void startMultiTests() {
        setButtonsEnabled(false);
        progressContainer->setVisible(true);
        progressBar->setRange(0, 20);
        progressBar->setValue(0);
        spinner->start();

        appendLog("=========================================");
        appendLog(isEnglish 
            ? "> Starting 10 live access tests for Discord and Roblox..." 
            : "> Discord ve Roblox için 10'ar adet canlı erişim testi başlatılıyor...");
        
        discordTestState = TestStatus::Testing;
        discordProgress = 0;
        updateServiceLabel(discordLabel, "Discord", discordTestState, 0, 0);

        robloxTestState = TestStatus::Waiting;
        updateServiceLabel(robloxLabel, "Roblox", robloxTestState, 0, 0);

        runPingTest("Discord", "https://discord.com", 1, 0, [this](int discordSuccessCount) {
            discordSuccess = discordSuccessCount;
            if (discordSuccessCount >= 5) {
                discordTestState = TestStatus::Success;
                appendLog(isEnglish 
                    ? QString("> [RESULT] Discord accessible (%1/10 success).").arg(discordSuccessCount)
                    : QString("> [SONUÇ] Discord erişilebilir (%1/10 başarı).").arg(discordSuccessCount));
            } else {
                discordTestState = TestStatus::Failed;
                appendLog(isEnglish 
                    ? QString("> [RESULT] Discord inaccessible (%1/10 success).").arg(discordSuccessCount)
                    : QString("> [SONUÇ] Discord erişilemedi (%1/10 başarı).").arg(discordSuccessCount));
            }
            updateServiceLabel(discordLabel, "Discord", discordTestState, 10, discordSuccess);

            appendLog("-----------------------------------------");
            robloxTestState = TestStatus::Testing;
            robloxProgress = 0;
            updateServiceLabel(robloxLabel, "Roblox", robloxTestState, 0, 0);

            runPingTest("Roblox", "https://www.roblox.com", 1, 0, [this](int robloxSuccessCount) {
                robloxSuccess = robloxSuccessCount;
                if (robloxSuccessCount >= 5) {
                    robloxTestState = TestStatus::Success;
                    appendLog(isEnglish 
                        ? QString("> [RESULT] Roblox accessible (%1/10 success).").arg(robloxSuccessCount)
                        : QString("> [SONUÇ] Roblox erişilebilir (%1/10 başarı).").arg(robloxSuccessCount));
                } else {
                    robloxTestState = TestStatus::Failed;
                    appendLog(isEnglish 
                        ? QString("> [RESULT] Roblox inaccessible (%1/10 success).").arg(robloxSuccessCount)
                        : QString("> [SONUÇ] Roblox erişilemedi (%1/10 başarı).").arg(robloxSuccessCount));
                }
                updateServiceLabel(robloxLabel, "Roblox", robloxTestState, 10, robloxSuccess);

                appendLog("=========================================");
                spinner->stop();
                progressBar->setValue(20);
                setButtonsEnabled(true);
            });
        });
    }

    void runPingTest(const QString &serviceName, const QString &url, int currentAttempt, int successCount, std::function<void(int)> onComplete) {
        if (currentAttempt > 10) { onComplete(successCount); return; }

        if (serviceName == "Discord") {
            discordProgress = currentAttempt;
            progressBar->setValue(currentAttempt);
            updateServiceLabel(discordLabel, "Discord", TestStatus::Testing, currentAttempt, successCount);
        } else if (serviceName == "Roblox") {
            robloxProgress = currentAttempt;
            progressBar->setValue(10 + currentAttempt);
            updateServiceLabel(robloxLabel, "Roblox", TestStatus::Testing, currentAttempt, successCount);
        }

        auto *manager = new QNetworkAccessManager(this);
        QNetworkRequest request((QUrl(url)));
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply *reply = manager->get(request);
        auto *elapsedTimer = new QElapsedTimer(); elapsedTimer->start();
        auto *timer = new QTimer(this); timer->setSingleShot(true);

        auto cleanup = [manager, reply, timer, elapsedTimer]() {
            timer->stop(); timer->deleteLater();
            reply->deleteLater(); manager->deleteLater(); delete elapsedTimer;
        };

        connect(timer, &QTimer::timeout, [this, reply, serviceName, url, currentAttempt, successCount, onComplete, cleanup]() {
            reply->abort();
            appendLog(isEnglish 
                ? QString(" > [%1 %2/10] Timeout (3000ms)").arg(serviceName).arg(currentAttempt)
                : QString(" > [%1 %2/10] Zaman Aşımı (Timeout - 3000ms)").arg(serviceName).arg(currentAttempt));
            cleanup();
            runPingTest(serviceName, url, currentAttempt + 1, successCount, onComplete);
        });

        connect(reply, &QNetworkReply::finished, [this, reply, serviceName, url, currentAttempt, successCount, onComplete, cleanup, timer, elapsedTimer]() {
            if (timer->isActive()) {
                timer->stop();
                qint64 responseTime = elapsedTimer->elapsed();
                bool isSuccess = (reply->error() == QNetworkReply::NoError);
                int newSuccessCount = successCount + (isSuccess ? 1 : 0);

                if (isSuccess) {
                    appendLog(isEnglish 
                        ? QString(" > [%1 %2/10] Success - Response: %3 ms").arg(serviceName).arg(currentAttempt).arg(responseTime)
                        : QString(" > [%1 %2/10] Başarılı - Yanıt: %3 ms").arg(serviceName).arg(currentAttempt).arg(responseTime));
                } else {
                    appendLog(isEnglish 
                        ? QString(" > [%1 %2/10] Failed - Error: %3").arg(serviceName).arg(currentAttempt).arg(reply->errorString())
                        : QString(" > [%1 %2/10] Başarısız - Hata: %3").arg(serviceName).arg(currentAttempt).arg(reply->errorString()));
                }

                cleanup();
                runPingTest(serviceName, url, currentAttempt + 1, newSuccessCount, onComplete);
            }
        });
        timer->start(3000);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(getAppIcon());
    // Pencereler kapandığında (X tuşuna basıldığında) uygulamanın sistem tepsisinde arka planda çalışmaya devam etmesi için
    QApplication::setQuitOnLastWindowClosed(false);

    app.setStyleSheet(R"(
        QWidget { 
            background-color: #141418; 
            color: #f1f5f9; 
            font-family: 'Inter', 'Segoe UI', Ubuntu, sans-serif; 
        }
        
        /* Kartlar */
        QWidget#card, QWidget#cardStatus { 
            background-color: #1e1b24; 
            border-radius: 4px; 
            border: 1px solid #3d2438; 
        }
        QWidget#cardStatus {
            border-left: 3px solid #853953;
        }

        /* Başlıklar */
        QLabel#title { 
            font-size: 20px; 
            font-weight: 800; 
            color: #ffffff; 
            letter-spacing: -0.5px;
        }
        QLabel#subtitle {
            font-size: 13px; 
            font-weight: 500; 
            color: #94a3b8; 
            margin-top: 5px;
        }
        QLabel#status { 
            font-size: 13px; 
            font-weight: 600; 
            color: #f1f5f9; 
        }
        QLabel#lblLang { 
            font-size: 12px; 
            font-weight: 600; 
            color: #cbd5e1; 
        }

        /* Butonlar */
        QPushButton { 
            font-weight: 600; 
            font-size: 13px; 
            padding: 10px 14px; 
            border-radius: 3px; 
            border: none; 
        }
        QPushButton#btnStart { 
            background-color: #96335d; 
            color: #ffffff;
        }
        QPushButton#btnStart:hover { 
            background-color: #b33d6f; 
        }
        QPushButton#btnStart:pressed { 
            background-color: #7d2a4e; 
        }
        QPushButton#btnStop { 
            background-color: #271c26; 
            color: #f87171;
            border: 1px solid #542445; 
        }
        QPushButton#btnStop:hover { 
            background-color: #382436; 
        }
        QPushButton#btnStop:pressed { 
            background-color: #1f161e; 
        }
        QPushButton#btnTest { 
            background-color: #2e1d2c; 
            color: #f1f5f9;
            border: 1px solid #5a284c; 
            padding: 6px 12px;
            font-size: 12px;
        }
        QPushButton#btnTest:hover { 
            background-color: #42263f; 
        }
        QPushButton#btnToggleLog { 
            background-color: #1c1a22; 
            color: #cbd5e1; 
            font-size: 12px; 
            padding: 8px; 
            border-radius: 3px; 
            border: 1px solid #352b3e; 
        }
        QPushButton#btnToggleLog:hover { 
            background-color: #262230; 
            color: #ffffff;
            border-color: #4c3e59;
        }
        QPushButton#btnClearLog {
            background-color: #262230;
            color: #cbd5e1;
            font-size: 11px;
            padding: 4px 10px;
            border-radius: 3px;
            border: 1px solid #3d354d;
        }
        QPushButton#btnClearLog:hover { 
            background-color: #362f44; 
            color: #ffffff;
        }
        QPushButton:disabled { 
            background-color: #1a1920; 
            color: #52525b; 
            border: 1px solid #27272a; 
        }

        /* Dropdown */
        QComboBox#langCombo { 
            background-color: #241c26; 
            color: #f1f5f9; 
            border: 1px solid #4a2741; 
            border-radius: 3px; 
            padding: 4px 10px; 
            font-weight: 600; 
            font-size: 12px; 
        }
        QComboBox#langCombo:hover { 
            background-color: #332336; 
        }
        QComboBox#langCombo::drop-down { 
            border: none; 
        }
        QComboBox#langCombo QAbstractItemView { 
            background-color: #1e1b24; 
            color: #f1f5f9; 
            selection-background-color: #96335d; 
            selection-color: #ffffff; 
            border: 1px solid #4a2741; 
            outline: none; 
        }

        /* İlerleme Çubuğu */
        QProgressBar { 
            background-color: #18171f; 
            color: #f1f5f9; 
            border: 1px solid #4a2741; 
            border-radius: 3px; 
            text-align: center; 
            font-size: 11px; 
            font-weight: 700; 
            height: 16px; 
        }
        QProgressBar::chunk { 
            background-color: #96335d; 
            border-radius: 2px; 
        }

        /* Günlük Alanı */
        QTextEdit { 
            background-color: #0d0d11; 
            color: #e2e8f0; 
            font-family: 'JetBrains Mono', 'Consolas', 'Courier New', monospace; 
            font-size: 12px; 
            border: 1px solid #2d2638; 
            border-radius: 3px; 
            padding: 10px; 
            line-height: 1.4;
        }
    )");

    QStringList args = app.arguments();
    if (args.contains("--startup-check")) {
        StartupChecker checker;
        checker.runCheck();
        return app.exec();
    }

    ZapretWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"
