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
#include <unistd.h>

// Sistem fontuna veya emoji desteğine bağımlı olmayan vektörel dünya ikonu
static QPixmap createGlobePixmap(int size, const QColor &color) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(color, 1.6);
    p.setPen(pen);
    p.drawEllipse(1, 1, size - 2, size - 2);
    p.drawLine(1, size / 2, size - 1, size / 2);
    p.drawEllipse(size / 4, 1, size / 2, size - 2);
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
        if (!timer->isActive()) {
            timer->start(60);
        }
        show();
        update();
    }

    void stop() {
        spinning = false;
        timer->stop();
        hide();
        update();
    }

    bool isSpinning() const { return spinning; }

protected:
    void paintEvent(QPaintEvent *) override {
        if (!spinning) return;
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.translate(width() / 2, height() / 2);
        p.rotate(angle);

        QPen pen(QColor("#a04565"), 3, Qt::SolidLine, Qt::RoundCap);
        p.setPen(pen);
        p.drawArc(-8, -8, 16, 16, 0, 270 * 16);
    }
};

// Arka plan başlatıcı ve ağ test edici
class StartupChecker : public QObject {
    Q_OBJECT
    int attempts = 0;
public:
    StartupChecker(QObject *parent = nullptr) : QObject(parent) {}
    void runCheck() {
        QTimer::singleShot(5000, this, &StartupChecker::pingTest);
    }
private:
    void pingTest() {
        attempts++;
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
                    cleanup(); QApplication::quit(); 
                } else {
                    cleanup(); handleFailure();
                }
            }
        });
        timer->start(3000);
    }

    void handleFailure() {
        if (attempts < 3) {
            QTimer::singleShot(5000, this, &StartupChecker::pingTest);
        } else {
            askUser();
        }
    }

    void askUser() {
        bool isEn = !QLocale::system().name().startsWith("tr", Qt::CaseInsensitive);
        QMessageBox msgBox;
        msgBox.setWindowTitle(isEn ? "Naga Zapret - Connection Error" : "Naga Zapret - Bağlantı Hatası");
        msgBox.setText(isEn 
            ? "A connection issue with Discord/Roblox was detected at system startup.\n\nWould you like to reinstall/restart the censorship bypass service?"
            : "Sistem açılışında Discord/Roblox bağlantısında sorun tespit edildi.\n\nSansür aşma servisini yeniden kurmak/başlatmak ister misiniz?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setStyleSheet("background-color: #2C2C2C; color: #F3F4F4; font-size: 13px;");

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
        resize(600, 430);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setAlignment(Qt::AlignTop);
        mainLayout->setSpacing(14);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        auto *headerLayout = new QHBoxLayout();
        auto *lblTitle = new QLabel("Naga Zapret Control Center", this);
        lblTitle->setObjectName("title");
        headerLayout->addWidget(lblTitle);
        headerLayout->addStretch();

        lblGlobe = new QLabel(this);
        lblGlobe->setPixmap(createGlobePixmap(16, QColor("#F3F4F4")));
        headerLayout->addWidget(lblGlobe);

        lblLang = new QLabel("Dil:", this);
        lblLang->setObjectName("lblLang");
        lblLang->setStyleSheet("font-size: 13px; font-weight: bold; color: #F3F4F4;");
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

        auto *statusCard = new QWidget(this);
        statusCard->setObjectName("card");
        auto *statusLayout = new QVBoxLayout(statusCard);
        
        statusLabel = new QLabel(this);
        statusLabel->setObjectName("status");
        statusLayout->addWidget(statusLabel);
        mainLayout->addWidget(statusCard);

        auto *actionLayout = new QHBoxLayout();
        actionLayout->setSpacing(10);

        btnStart = new QPushButton(this);
        btnStart->setObjectName("btnStart");
        btnStart->setCursor(Qt::PointingHandCursor);

        btnStop = new QPushButton(this);
        btnStop->setObjectName("btnStop");
        btnStop->setCursor(Qt::PointingHandCursor);

        actionLayout->addWidget(btnStart);
        actionLayout->addWidget(btnStop);
        mainLayout->addLayout(actionLayout);

        auto *testCard = new QWidget(this);
        testCard->setObjectName("card");
        auto *testLayout = new QVBoxLayout(testCard);
        testLayout->setSpacing(10);

        auto *testHeaderLayout = new QHBoxLayout();
        lblTestTitle = new QLabel(this);
        lblTestTitle->setStyleSheet("font-weight: bold; color: #F3F4F4;");
        
        btnTest = new QPushButton(this);
        btnTest->setObjectName("btnTest");
        btnTest->setCursor(Qt::PointingHandCursor);

        testHeaderLayout->addWidget(lblTestTitle);
        testHeaderLayout->addStretch();
        testHeaderLayout->addWidget(btnTest);
        testLayout->addLayout(testHeaderLayout);

        discordLabel = new QLabel(this);
        robloxLabel = new QLabel(this);
        discordLabel->setStyleSheet("color: #d1d5db;");
        robloxLabel->setStyleSheet("color: #d1d5db;");

        testLayout->addWidget(discordLabel);
        testLayout->addWidget(robloxLabel);
        mainLayout->addWidget(testCard);

        // İlerleme çubuğu ve dönen spinner alanı
        progressContainer = new QWidget(this);
        auto *progressLayout = new QHBoxLayout(progressContainer);
        progressLayout->setContentsMargins(0, 0, 0, 0);
        progressLayout->setSpacing(10);

        spinner = new SpinnerWidget(this);
        progressBar = new QProgressBar(this);
        progressBar->setTextVisible(true);
        progressBar->setAlignment(Qt::AlignCenter);

        progressLayout->addWidget(spinner);
        progressLayout->addWidget(progressBar);
        progressContainer->setVisible(false);
        mainLayout->addWidget(progressContainer);

        // Kapatılıp açılabilir günlük (log) alanı
        btnToggleLog = new QPushButton(this);
        btnToggleLog->setObjectName("btnToggleLog");
        btnToggleLog->setCursor(Qt::PointingHandCursor);
        mainLayout->addWidget(btnToggleLog);

        logArea = new QTextEdit(this);
        logArea->setReadOnly(true);
        logArea->setFixedHeight(170);
        logArea->setVisible(false); // Varsayılan olarak kapalı
        mainLayout->addWidget(logArea);

        connect(langCombo, QOverload<int>::of(&QComboBox::activated), this, [this](int index) {
            isEnglish = (index == 1);
            langCombo->hidePopup();
            updateUiTexts();
        });

        connect(btnToggleLog, &QPushButton::clicked, this, [this]() {
            bool willShow = !logArea->isVisible();
            logArea->setVisible(willShow);
            updateToggleLogText();
            if (willShow) {
                resize(width(), height() + 184);
            } else {
                resize(width(), height() - 184);
            }
        });

        connect(btnStart, &QPushButton::clicked, this, &ZapretWindow::startZapret);
        connect(btnStop, &QPushButton::clicked, this, &ZapretWindow::stopZapret);
        connect(btnTest, &QPushButton::clicked, this, &ZapretWindow::startMultiTests);

        updateUiTexts();
        checkStatus();
    }

private:
    bool isEnglish = false;
    StatusState currentStatus = StatusState::Checking;

    TestStatus discordTestState = TestStatus::Untested;
    int discordProgress = 0;
    int discordSuccess = 0;

    TestStatus robloxTestState = TestStatus::Untested;
    int robloxProgress = 0;
    int robloxSuccess = 0;

    QLabel *lblGlobe;
    QLabel *lblLang;
    QComboBox *langCombo;
    QLabel *lblTestTitle;
    QLabel *statusLabel;
    QLabel *discordLabel;
    QLabel *robloxLabel;
    QWidget *progressContainer;
    SpinnerWidget *spinner;
    QProgressBar *progressBar;
    QPushButton *btnToggleLog;
    QTextEdit *logArea;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QPushButton *btnTest;

    bool isRoot() { return geteuid() == 0; }

    void appendLog(const QString &text) {
        logArea->append(text);
        logArea->ensureCursorVisible();
        QApplication::processEvents();
    }

    void setButtonsEnabled(bool enabled) {
        btnStart->setEnabled(enabled);
        btnStop->setEnabled(enabled);
        btnTest->setEnabled(enabled);
        langCombo->setEnabled(enabled);
    }

    void updateToggleLogText() {
        if (logArea->isVisible()) {
            btnToggleLog->setText(isEnglish ? "▲ Hide Logs" : "▲ Günlüğü Gizle");
        } else {
            btnToggleLog->setText(isEnglish ? "▼ Show Logs" : "▼ Günlüğü Göster");
        }
    }

    void updateUiTexts() {
        lblLang->setText(isEnglish ? "Language:" : "Dil:");
        lblTestTitle->setText(isEnglish ? "Access Status (10x Packet Test)" : "Erişim Durumu (10x Paket Testi)");
        btnStart->setText(isEnglish ? "Install & Start Zapret" : "Zapret Kur ve Başlat");
        btnStop->setText(isEnglish ? "Stop & Remove Zapret" : "Zapret Durdur ve Kaldır");
        btnTest->setText(isEnglish ? "Test Connection" : "Bağlantı Testi Yap");

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
                statusLabel->setStyleSheet("color: #d1d5db;");
                break;
            case StatusState::Installed:
                statusLabel->setText(isEnglish ? "Status: Zapret Installed" : "Durum: Zapret Kurulu");
                statusLabel->setStyleSheet("color: #4ade80;");
                break;
            case StatusState::NotInstalled:
                statusLabel->setText(isEnglish ? "Status: Not Installed" : "Durum: Kurulu Değil");
                statusLabel->setStyleSheet("color: #d1d5db;");
                break;
            case StatusState::Installing:
                statusLabel->setText(isEnglish ? "Status: Installing..." : "Durum: Kuruluyor...");
                statusLabel->setStyleSheet("color: #f59e0b;");
                break;
            case StatusState::Running:
                statusLabel->setText(isEnglish ? "Status: Running" : "Durum: Çalışıyor");
                statusLabel->setStyleSheet("color: #4ade80;");
                break;
            case StatusState::Removing:
                statusLabel->setText(isEnglish ? "Status: Removing..." : "Durum: Kaldırılıyor...");
                statusLabel->setStyleSheet("color: #f59e0b;");
                break;
            case StatusState::Removed:
                statusLabel->setText(isEnglish ? "Status: Removed" : "Durum: Kaldırıldı");
                statusLabel->setStyleSheet("color: #f87171;");
                break;
        }
    }

    void updateServiceLabel(QLabel *lbl, const QString &serviceName, TestStatus state, int progress, int successCount) {
        switch (state) {
            case TestStatus::Untested:
                lbl->setText(QString("• %1: %2").arg(serviceName, isEnglish ? "Not Tested" : "Test Edilmedi"));
                lbl->setStyleSheet("color: #d1d5db;");
                break;
            case TestStatus::Waiting:
                lbl->setText(QString("• %1: %2").arg(serviceName, isEnglish ? "Waiting..." : "Bekliyor..."));
                lbl->setStyleSheet("color: #d1d5db;");
                break;
            case TestStatus::Testing:
                lbl->setText(QString("• %1: %2 (%3/10)").arg(serviceName, isEnglish ? "Testing" : "Test Ediliyor").arg(progress));
                lbl->setStyleSheet("color: #f59e0b;");
                break;
            case TestStatus::Success:
                lbl->setText(QString("• %1: %2 (%3/10 OK)").arg(serviceName, isEnglish ? "Accessible" : "Erişilebilir").arg(successCount));
                lbl->setStyleSheet("color: #4ade80; font-weight: bold;");
                break;
            case TestStatus::Failed:
                lbl->setText(QString("• %1: %2 (%3/10 OK)").arg(serviceName, isEnglish ? "Inaccessible" : "Erişilemedi").arg(successCount));
                lbl->setStyleSheet("color: #f87171; font-weight: bold;");
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
        progressBar->setRange(0, 0); // Sürekli hareket eden yükleme animasyonu
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
        progressBar->setRange(0, 20); // 10 Discord + 10 Roblox
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
    app.setStyleSheet(R"(
        QWidget { background-color: #2C2C2C; color: #F3F4F4; font-family: 'Segoe UI', Ubuntu, sans-serif; }
        QFrame#card { background-color: #612D53; border-radius: 4px; border: 1px solid #853953; }
        QLabel { font-size: 13px; color: #F3F4F4; }
        QLabel#title { font-size: 20px; font-weight: bold; color: #F3F4F4; }
        QLabel#status { font-size: 14px; font-weight: 600; color: #F3F4F4; }
        QPushButton { color: #F3F4F4; font-weight: bold; font-size: 13px; padding: 10px; border-radius: 3px; border: none; }
        QPushButton#btnStart { background-color: #853953; }
        QPushButton#btnStart:hover { background-color: #a04565; }
        QPushButton#btnStop { background-color: #44213a; border: 1px solid #853953; }
        QPushButton#btnStop:hover { background-color: #582b4b; }
        QPushButton#btnTest { background-color: #612D53; border: 1px solid #853953; }
        QPushButton#btnTest:hover { background-color: #7a3869; }
        QPushButton#btnToggleLog { background-color: #383838; color: #d1d5db; font-size: 12px; font-weight: 600; padding: 7px; border-radius: 3px; border: 1px solid #4f4f4f; }
        QPushButton#btnToggleLog:hover { background-color: #484848; color: #F3F4F4; }
        QPushButton:disabled { background-color: #1e1e1e; color: #666666; border: none; }
        QComboBox#langCombo { background-color: #44213a; color: #F3F4F4; border: 1px solid #853953; border-radius: 3px; padding: 4px 10px; font-weight: bold; font-size: 12px; }
        QComboBox#langCombo:hover { background-color: #582b4b; }
        QComboBox#langCombo::drop-down { border: none; }
        QComboBox#langCombo QAbstractItemView { background-color: #2C2C2C; color: #F3F4F4; selection-background-color: #853953; selection-color: #F3F4F4; border: 1px solid #853953; outline: none; }
        QProgressBar { background-color: #1e1e1e; color: #F3F4F4; border: 1px solid #853953; border-radius: 3px; text-align: center; font-size: 11px; font-weight: bold; height: 16px; }
        QProgressBar::chunk { background-color: #853953; border-radius: 2px; }
        QTextEdit { background-color: #1e1e1e; color: #F3F4F4; font-family: 'Consolas', 'Courier New', monospace; font-size: 12px; border: 1px solid #612D53; border-radius: 3px; padding: 8px; }
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