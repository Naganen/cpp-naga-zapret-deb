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

class ZapretWindow : public QWidget {
public:
    ZapretWindow() {
        setWindowTitle("Naga Zapret - DPI Bypass Tool");
        resize(580, 650);

        setStyleSheet(R"(
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
            QLabel {
                font-size: 13px;
                color: #F3F4F4;
            }
            QLabel#title {
                font-size: 20px;
                font-weight: bold;
                color: #F3F4F4;
            }
            QLabel#status {
                font-size: 14px;
                font-weight: 600;
                color: #F3F4F4;
            }
            QPushButton {
                color: #F3F4F4;
                font-weight: bold;
                font-size: 13px;
                padding: 10px;
                border-radius: 8px;
                border: none;
            }
            QPushButton#btnStart {
                background-color: #853953;
            }
            QPushButton#btnStart:hover {
                background-color: #a04565;
            }
            QPushButton#btnStop {
                background-color: #44213a;
                border: 1px solid #853953;
            }
            QPushButton#btnStop:hover {
                background-color: #582b4b;
            }
            QPushButton#btnTest {
                background-color: #612D53;
                border: 1px solid #853953;
            }
            QPushButton#btnTest:hover {
                background-color: #7a3869;
            }
            QPushButton:disabled {
                background-color: #1e1e1e;
                color: #666666;
                border: none;
            }
            QTextEdit {
                background-color: #1e1e1e;
                color: #F3F4F4;
                font-family: 'Consolas', 'Courier New', monospace;
                font-size: 12px;
                border: 1px solid #612D53;
                border-radius: 8px;
                padding: 8px;
            }
        )");

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

    void startZapret() {
        setButtonsEnabled(false);
        logArea->append("> Kurulum başlatılıyor (Root yetkisi isteniyor)...");
        statusLabel->setText("Durum: Kuruluyor...");
        statusLabel->setStyleSheet("color: #f59e0b;");

        QString cmd = "apt-get update && apt-get install -y curl jq iptables nftables tar wget bind9-dnsutils dnscrypt-proxy && "
                      "mkdir -p /opt/zapret && "
                      "wget -O /tmp/zapret.tar.gz https://github.com/bol-van/zapret/releases/download/v72.13/zapret-v72.13.tar.gz && "
                      "tar -xzf /tmp/zapret.tar.gz -C /tmp && cp -rf /tmp/zapret-v72.13/* /opt/zapret/ && "
                      "/opt/zapret/install_prereq.sh && /opt/zapret/install_bin.sh && "
                      "echo -e \"\\n\\n\\n4\\n\\n\\nY\\n\\n\\n\\n\\n\\n\" | /opt/zapret/install_easy.sh && "
                      "systemctl restart zapret";

        auto *process = new QProcess(this);
        connect(process, &QProcess::readyReadStandardOutput, [this, process]() {
            logArea->append(process->readAllStandardOutput());
        });
        connect(process, &QProcess::readyReadStandardError, [this, process]() {
            logArea->append(process->readAllStandardError());
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, process](int exitCode) {
            setButtonsEnabled(true);
            if (exitCode == 0) {
                logArea->append("> Zapret başarıyla kuruldu ve hizmet başlatıldı!");
                statusLabel->setText("Durum: Çalışıyor");
                statusLabel->setStyleSheet("color: #4ade80;");
                startMultiTests();
            } else {
                logArea->append("> Hata: Kurulum başarısız oldu veya iptal edildi.");
                checkStatus();
            }
            process->deleteLater();
        });

        process->start("pkexec", QStringList() << "bash" << "-c" << cmd);
    }

    void stopZapret() {
        bool exists = QFileInfo::exists("/opt/zapret") || QFileInfo::exists("/etc/systemd/system/zapret.service");
        if (!exists) {
            logArea->append("> Sistemde kurulu bir Zapret bulunamadı.");
            statusLabel->setText("Durum: Kurulu Değil");
            statusLabel->setStyleSheet("color: #d1d5db;");
            return;
        }

        setButtonsEnabled(false);
        logArea->append("> Zapret durduruluyor ve sistemden kaldırılıyor...");
        statusLabel->setText("Durum: Kaldırılıyor...");
        statusLabel->setStyleSheet("color: #f59e0b;");

        QString cmd = "if [ -f /opt/zapret/uninstall_easy.sh ]; then "
                      "echo -e 'Y\\n' | /opt/zapret/uninstall_easy.sh; "
                      "fi; "
                      "systemctl stop zapret 2>/dev/null; "
                      "systemctl disable zapret 2>/dev/null; "
                      "rm -rf /opt/zapret; "
                      "rm -f /etc/systemd/system/zapret.service";

        auto *process = new QProcess(this);
        connect(process, &QProcess::readyReadStandardOutput, [this, process]() {
            logArea->append(process->readAllStandardOutput());
        });
        connect(process, &QProcess::readyReadStandardError, [this, process]() {
            logArea->append(process->readAllStandardError());
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, process](int exitCode) {
            setButtonsEnabled(true);
            if (exitCode == 0) {
                logArea->append("> Zapret başarıyla durduruldu ve kaldırıldı.");
                statusLabel->setText("Durum: Kaldırıldı");
                statusLabel->setStyleSheet("color: #f87171;");
            } else {
                logArea->append("> Hata: Kaldırma işlemi tamamlanamadı.");
                checkStatus();
            }
            process->deleteLater();
        });

        process->start("pkexec", QStringList() << "bash" << "-c" << cmd);
    }

    void startMultiTests() {
        setButtonsEnabled(false);
        logArea->append("> Discord ve Roblox için 10'ar adet erişim isteği atılıyor...");
        
        discordLabel->setText("• Discord: Test Ediliyor (0/10)");
        discordLabel->setStyleSheet("color: #f59e0b;");
        robloxLabel->setText("• Roblox: Bekliyor...");
        robloxLabel->setStyleSheet("color: #d1d5db;");

        runPingTest("https://discord.com", 1, 0, [this](int discordSuccessCount) {
            if (discordSuccessCount >= 5) {
                discordLabel->setText(QString("• Discord: Erişilebilir (%1/10 OK)").arg(discordSuccessCount));
                discordLabel->setStyleSheet("color: #4ade80; font-weight: bold;");
                logArea->append(QString("> [TEST OK] Discord erişilebilir (%1/10 başarı).").arg(discordSuccessCount));
            } else {
                discordLabel->setText(QString("• Discord: Erişilemedi (%1/10 OK)").arg(discordSuccessCount));
                discordLabel->setStyleSheet("color: #f87171; font-weight: bold;");
                logArea->append(QString("> [TEST HATA] Discord erişilemedi (%1/10 başarı).").arg(discordSuccessCount));
            }

            robloxLabel->setText("• Roblox: Test Ediliyor (0/10)");
            robloxLabel->setStyleSheet("color: #f59e0b;");

            runPingTest("https://www.roblox.com", 1, 0, [this](int robloxSuccessCount) {
                if (robloxSuccessCount >= 5) {
                    robloxLabel->setText(QString("• Roblox: Erişilebilir (%1/10 OK)").arg(robloxSuccessCount));
                    robloxLabel->setStyleSheet("color: #4ade80; font-weight: bold;");
                    logArea->append(QString("> [TEST OK] Roblox erişilebilir (%1/10 başarı).").arg(robloxSuccessCount));
                } else {
                    robloxLabel->setText(QString("• Roblox: Erişilemedi (%1/10 OK)").arg(robloxSuccessCount));
                    robloxLabel->setStyleSheet("color: #f87171; font-weight: bold;");
                    logArea->append(QString("> [TEST HATA] Roblox erişilemedi (%1/10 başarı).").arg(robloxSuccessCount));
                }
                setButtonsEnabled(true);
            });
        });
    }

    void runPingTest(const QString &url, int currentAttempt, int successCount, std::function<void(int)> onComplete) {
        if (currentAttempt > 10) {
            onComplete(successCount);
            return;
        }

        auto *manager = new QNetworkAccessManager(this);
        QNetworkRequest request((QUrl(url)));
        request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

        QNetworkReply *reply = manager->get(request);

        // 3 saniyelik zaman aşımı koruması
        auto *timer = new QTimer(this);
        timer->setSingleShot(true);

        auto cleanup = [manager, reply, timer]() {
            timer->stop();
            timer->deleteLater();
            reply->deleteLater();
            manager->deleteLater();
        };

        connect(timer, &QTimer::timeout, [this, reply, url, currentAttempt, successCount, onComplete, cleanup]() {
            reply->abort();
            cleanup();
            runPingTest(url, currentAttempt + 1, successCount, onComplete);
        });

        connect(reply, &QNetworkReply::finished, [this, reply, url, currentAttempt, successCount, onComplete, cleanup, timer]() {
            if (timer->isActive()) {
                timer->stop();
                bool isSuccess = (reply->error() == QNetworkReply::NoError);
                int newSuccessCount = successCount + (isSuccess ? 1 : 0);

                cleanup();
                runPingTest(url, currentAttempt + 1, newSuccessCount, onComplete);
            }
        });

        timer->start(3000); // 3000 ms = 3 saniye
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ZapretWindow window;
    window.show();
    return app.exec();
}