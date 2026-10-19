#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QFileInfo>
#include <QTimer>
#include <QElapsedTimer>
#include <QMessageBox>
#include <unistd.h>

// Arka planda çalışan başlangıç denetleyicisi
class StartupChecker : public QObject {
    Q_OBJECT
    int attempts = 0;
public:
    StartupChecker(QObject *parent = nullptr) : QObject(parent) {}

    void runCheck() {
        // Ağın oturması için ilk denemeyi 5 saniye gecikmeli başlat
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
            timer->stop();
            timer->deleteLater();
            reply->deleteLater();
            manager->deleteLater();
        };

        connect(timer, &QTimer::timeout, [this, cleanup]() {
            cleanup();
            handleFailure();
        });

        connect(reply, &QNetworkReply::finished, [this, reply, timer, cleanup]() {
            if (timer->isActive()) {
                timer->stop();
                if (reply->error() == QNetworkReply::NoError) {
                    cleanup();
                    QApplication::quit(); // Bağlantı var, sessizce kapan
                } else {
                    cleanup();
                    handleFailure();
                }
            }
        });

        timer->start(3000); // 3 saniye timeout
    }

    void handleFailure() {
        if (attempts < 3) {
            // Başarısız olursa 5 saniye sonra tekrar dene (toplam 3 deneme)
            QTimer::singleShot(5000, this, &StartupChecker::pingTest);
        } else {
            // 3 deneme de başarısızsa kullanıcıya sor
            askUser();
        }
    }

    void askUser() {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Naga Zapret - Bağlantı Hatası");
        msgBox.setText("Sistem açılışında Discord/Roblox bağlantısında sorun tespit edildi.\n\nSansür aşma servisini (Zapret) yeniden başlatmak ister misiniz?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setStyleSheet("background-color: #2C2C2C; color: #F3F4F4; font-size: 13px;");

        if (msgBox.exec() == QMessageBox::Yes) {
            // Kullanıcı onaylarsa root izni isteyerek servisi yeniden başlat
            QProcess::startDetached("pkexec", QStringList() << "systemctl" << "restart" << "zapret");
        }
        QApplication::quit();
    }
};

class ZapretWindow : public QWidget {
    Q_OBJECT
public:
    ZapretWindow() {
        setWindowTitle("Naga Zapret - DPI Bypass Tool");
        resize(580, 650);

        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(16);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        // Başlık
        auto *lblTitle = new QLabel("Naga Zapret Control Center", this);
        lblTitle->setObjectName("title");
        mainLayout->addWidget(lblTitle);

        // Durum Kartı
        auto *statusCard = new QWidget(this);
        statusCard->setObjectName("card");
        auto *statusLayout = new QVBoxLayout(statusCard);
        
        statusLabel = new QLabel("Durum: Kontrol Ediliyor...", this);
        statusLabel->setObjectName("status");
        statusLayout->addWidget(statusLabel);
        mainLayout->addWidget(statusCard);

        // Zapret Kontrol Butonları
        auto *actionLayout = new QHBoxLayout();
        actionLayout->setSpacing(10);

        btnStart = new QPushButton("Zapret Kur ve Başlat", this);
        btnStart->setObjectName("btnStart");
        btnStart->setCursor(Qt::PointingHandCursor);

        btnStop = new QPushButton("Zapret Durdur ve Kaldır", this);
        btnStop->setObjectName("btnStop");
        btnStop->setCursor(Qt::PointingHandCursor);

        actionLayout->addWidget(btnStart);
        actionLayout->addWidget(btnStop);
        mainLayout->addLayout(actionLayout);

        // Bağlantı Testleri Kartı
        auto *testCard = new QWidget(this);
        testCard->setObjectName("card");
        auto *testLayout = new QVBoxLayout(testCard);
        testLayout->setSpacing(10);

        auto *testHeaderLayout = new QHBoxLayout();
        auto *lblTestTitle = new QLabel("Erişim Durumu (10x Paket Testi)", this);
        lblTestTitle->setStyleSheet("font-weight: bold; color: #F3F4F4;");
        
        btnTest = new QPushButton("Bağlantı Testi Yap", this);
        btnTest->setObjectName("btnTest");
        btnTest->setCursor(Qt::PointingHandCursor);

        testHeaderLayout->addWidget(lblTestTitle);
        testHeaderLayout->addStretch();
        testHeaderLayout->addWidget(btnTest);
        testLayout->addLayout(testHeaderLayout);

        discordLabel = new QLabel("• Discord: Test Edilmedi", this);
        robloxLabel = new QLabel("• Roblox: Test Edilmedi", this);
        discordLabel->setStyleSheet("color: #d1d5db;");
        robloxLabel->setStyleSheet("color: #d1d5db;");

        testLayout->addWidget(discordLabel);
        testLayout->addWidget(robloxLabel);
        mainLayout->addWidget(testCard);

        // Terminal Log Alanı
        logArea = new QTextEdit(this);
        logArea->setReadOnly(true);
        mainLayout->addWidget(logArea);

        // Sinyal Bağlantıları
        connect(btnStart, &QPushButton::clicked, this, &ZapretWindow::startZapret);
        connect(btnStop, &QPushButton::clicked, this, &ZapretWindow::stopZapret);
        connect(btnTest, &QPushButton::clicked, this, &ZapretWindow::startMultiTests);

        checkStatus();
    }

private:
    QLabel *statusLabel;
    QLabel *discordLabel;
    QLabel *robloxLabel;
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
    }

    void checkStatus() {
        bool installed = QFileInfo::exists("/opt/zapret") || QFileInfo::exists("/etc/systemd/system/zapret.service");
        if (installed) {
            statusLabel->setText("Durum: Zapret Kurulu");
            statusLabel->setStyleSheet("color: #4ade80;");
        } else {
            statusLabel->setText("Durum: Kurulu Değil");
            statusLabel->setStyleSheet("color: #d1d5db;");
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
        if (isRoot()) {
            appendLog("> Kurulum başlatılıyor (Root yetkisi mevcut)...");
        } else {
            appendLog("> Kurulum başlatılıyor (Root yetkisi isteniyor)...");
        }
        statusLabel->setText("Durum: Kuruluyor...");
        statusLabel->setStyleSheet("color: #f59e0b;");

        QString cmd = "export DEBIAN_FRONTEND=noninteractive && "
                      "echo '> Paketler güncelleniyor...' && "
                      "apt-get update -qq && "
                      "apt-get install -y curl jq iptables nftables tar wget bind9-dnsutils dnscrypt-proxy && "
                      "echo '> Zapret indiriliyor...' && "
                      "mkdir -p /opt/zapret && "
                      "wget -q -O /tmp/zapret.tar.gz https://github.com/bol-van/zapret/releases/download/v72.13/zapret-v72.13.tar.gz && "
                      "tar -xzf /tmp/zapret.tar.gz -C /tmp && cp -rf /tmp/zapret-v72.13/* /opt/zapret/ && "
                      "echo '> Ön gereksinimler kuruluyor...' && "
                      "yes '' | /opt/zapret/install_prereq.sh && "
                      "echo '> İkili dosyalar (binaries) derleniyor...' && "
                      "/opt/zapret/install_bin.sh && "
                      "echo '> Yapılandırma ayarlanıyor...' && "
                      "cp -f /opt/zapret/config.default /opt/zapret/config && "
                      "sed -i 's/^FWTYPE=.*/FWTYPE=nftables/' /opt/zapret/config && "
                      "sed -i 's/^MODE=.*/MODE=nfqws/' /opt/zapret/config && "
                      "sed -i 's/^NFQWS_OPT_DESYNC=.*/NFQWS_OPT_DESYNC=\"--dpi-desync=fake,split2 --dpi-desync-ttl=0\"/' /opt/zapret/config && "
                      "echo '> Servis başlatılıyor...' && "
                      "cp -f /opt/zapret/init.d/systemd/zapret.service /etc/systemd/system/zapret.service 2>/dev/null || true && "
                      "systemctl daemon-reload 2>/dev/null || true && "
                      "systemctl enable zapret 2>/dev/null || true && "
                      "systemctl restart zapret 2>/dev/null || true && "
                      "echo '> Kurulum tamamlandı!'";

        runCommandAsRoot(cmd, [this](int exitCode) {
            setButtonsEnabled(true);
            if (exitCode == 0) {
                appendLog("> Zapret başarıyla kuruldu ve hizmet başlatıldı!");
                statusLabel->setText("Durum: Çalışıyor");
                statusLabel->setStyleSheet("color: #4ade80;");
                startMultiTests();
            } else {
                appendLog(QString("> Hata: Kurulum başarısız oldu (Çıkış Kodu: %1).").arg(exitCode));
                checkStatus();
            }
        });
    }

    void stopZapret() {
        bool exists = QFileInfo::exists("/opt/zapret") || QFileInfo::exists("/etc/systemd/system/zapret.service");
        if (!exists) {
            appendLog("> Sistemde kurulu bir Zapret bulunamadı.");
            statusLabel->setText("Durum: Kurulu Değil");
            statusLabel->setStyleSheet("color: #d1d5db;");
            return;
        }

        setButtonsEnabled(false);
        appendLog("> Zapret durduruluyor ve sistemden kaldırılıyor...");
        statusLabel->setText("Durum: Kaldırılıyor...");
        statusLabel->setStyleSheet("color: #f59e0b;");

        QString cmd = "if [ -f /opt/zapret/uninstall_easy.sh ]; then "
                      "echo -e 'Y\\n' | /opt/zapret/uninstall_easy.sh; "
                      "fi; "
                      "systemctl stop zapret 2>/dev/null || true; "
                      "systemctl disable zapret 2>/dev/null || true; "
                      "rm -rf /opt/zapret; "
                      "rm -f /etc/systemd/system/zapret.service";

        runCommandAsRoot(cmd, [this](int exitCode) {
            setButtonsEnabled(true);
            if (exitCode == 0) {
                appendLog("> Zapret başarıyla durduruldu ve kaldırıldı.");
                statusLabel->setText("Durum: Kaldırıldı");
                statusLabel->setStyleSheet("color: #f87171;");
            } else {
                appendLog(QString("> Hata: Kaldırma işlemi tamamlanamadı (Çıkış Kodu: %1).").arg(exitCode));
                checkStatus();
            }
        });
    }

    void startMultiTests() {
        setButtonsEnabled(false);
        appendLog("=========================================");
        appendLog("> Discord ve Roblox için 10'ar adet canlı erişim testi başlatılıyor...");
        
        discordLabel->setText("• Discord: Test Ediliyor (0/10)");
        discordLabel->setStyleSheet("color: #f59e0b;");
        robloxLabel->setText("• Roblox: Bekliyor...");
        robloxLabel->setStyleSheet("color: #d1d5db;");

        runPingTest("Discord", "https://discord.com", 1, 0, [this](int discordSuccessCount) {
            if (discordSuccessCount >= 5) {
                discordLabel->setText(QString("• Discord: Erişilebilir (%1/10 OK)").arg(discordSuccessCount));
                discordLabel->setStyleSheet("color: #4ade80; font-weight: bold;");
                appendLog(QString("> [SONUÇ] Discord erişilebilir (%1/10 başarı).").arg(discordSuccessCount));
            } else {
                discordLabel->setText(QString("• Discord: Erişilemedi (%1/10 OK)").arg(discordSuccessCount));
                discordLabel->setStyleSheet("color: #f87171; font-weight: bold;");
                appendLog(QString("> [SONUÇ] Discord erişilemedi (%1/10 başarı).").arg(discordSuccessCount));
            }

            appendLog("-----------------------------------------");
            robloxLabel->setText("• Roblox: Test Ediliyor (0/10)");
            robloxLabel->setStyleSheet("color: #f59e0b;");

            runPingTest("Roblox", "https://www.roblox.com", 1, 0, [this](int robloxSuccessCount) {
                if (robloxSuccessCount >= 5) {
                    robloxLabel->setText(QString("• Roblox: Erişilebilir (%1/10 OK)").arg(robloxSuccessCount));
                    robloxLabel->setStyleSheet("color: #4ade80; font-weight: bold;");
                    appendLog(QString("> [SONUÇ] Roblox erişilebilir (%1/10 başarı).").arg(robloxSuccessCount));
                } else {
                    robloxLabel->setText(QString("• Roblox: Erişilemedi (%1/10 OK)").arg(robloxSuccessCount));
                    robloxLabel->setStyleSheet("color: #f87171; font-weight: bold;");
                    appendLog(QString("> [SONUÇ] Roblox erişilemedi (%1/10 başarı).").arg(robloxSuccessCount));
                }
                appendLog("=========================================");
                setButtonsEnabled(true);
            });
        });
    }

    void runPingTest(const QString &serviceName, const QString &url, int currentAttempt, int successCount, std::function<void(int)> onComplete) {
        if (currentAttempt > 10) {
            onComplete(successCount);
            return;
        }

        if (serviceName == "Discord") {
            discordLabel->setText(QString("• Discord: Test Ediliyor (%1/10)").arg(currentAttempt));
        } else if (serviceName == "Roblox") {
            robloxLabel->setText(QString("• Roblox: Test Ediliyor (%1/10)").arg(currentAttempt));
        }

        auto *manager = new QNetworkAccessManager(this);
        QNetworkRequest request((QUrl(url)));
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply *reply = manager->get(request);

        auto *elapsedTimer = new QElapsedTimer();
        elapsedTimer->start();

        auto *timer = new QTimer(this);
        timer->setSingleShot(true);

        auto cleanup = [manager, reply, timer, elapsedTimer]() {
            timer->stop();
            timer->deleteLater();
            reply->deleteLater();
            manager->deleteLater();
            delete elapsedTimer;
        };

        connect(timer, &QTimer::timeout, [this, reply, serviceName, url, currentAttempt, successCount, onComplete, cleanup]() {
            reply->abort();
            appendLog(QString(" > [%1 %2/10] Zaman Aşımı (Timeout - 3000ms)").arg(serviceName).arg(currentAttempt));
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
                    appendLog(QString(" > [%1 %2/10] Başarılı - Yanıt: %3 ms").arg(serviceName).arg(currentAttempt).arg(responseTime));
                } else {
                    appendLog(QString(" > [%1 %2/10] Başarısız - Hata: %3").arg(serviceName).arg(currentAttempt).arg(reply->errorString()));
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

    // Global Tema
    app.setStyleSheet(R"(
        QWidget {
            background-color: #2C2C2C;
            color: #F3F4F4;
            font-family: 'Segoe UI', Ubuntu, sans-serif;
        }
        QFrame#card {
            background-color: #612D53;
            border-radius: 10px;
            border: 1px solid #853953;
        }
        QLabel { font-size: 13px; color: #F3F4F4; }
        QLabel#title { font-size: 20px; font-weight: bold; color: #F3F4F4; }
        QLabel#status { font-size: 14px; font-weight: 600; color: #F3F4F4; }
        QPushButton {
            color: #F3F4F4; font-weight: bold; font-size: 13px;
            padding: 10px; border-radius: 8px; border: none;
        }
        QPushButton#btnStart { background-color: #853953; }
        QPushButton#btnStart:hover { background-color: #a04565; }
        QPushButton#btnStop { background-color: #44213a; border: 1px solid #853953; }
        QPushButton#btnStop:hover { background-color: #582b4b; }
        QPushButton#btnTest { background-color: #612D53; border: 1px solid #853953; }
        QPushButton#btnTest:hover { background-color: #7a3869; }
        QPushButton:disabled { background-color: #1e1e1e; color: #666666; border: none; }
        QTextEdit {
            background-color: #1e1e1e; color: #F3F4F4;
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 12px; border: 1px solid #612D53;
            border-radius: 8px; padding: 8px;
        }
    )");

    QStringList args = app.arguments();
    
    // Sistem Açılış Kontrolü Mekanizması
    if (args.contains("--startup-check")) {
        StartupChecker checker;
        checker.runCheck();
        return app.exec();
    }

    // Normal Arayüz Mekanizması
    ZapretWindow window;
    window.show();
    return app.exec();
}

#include "main.moc"