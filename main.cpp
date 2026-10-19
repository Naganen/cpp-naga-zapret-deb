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
        
        statusLabel = new QLabel("Durum: Hazır", this);
        statusLabel->setObjectName("status");
        statusLayout->addWidget(statusLabel);
        mainLayout->addWidget(statusCard);

        // Zapret Kontrol Butonları (Yan Yana)
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
        auto *lblTestTitle = new QLabel("Erişim Durumu (DPI Testi)", this);
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
        connect(btnTest, &QPushButton::clicked, this, &ZapretWindow::runTests);
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

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this](int exitCode) {
            setButtonsEnabled(true);
            if (exitCode == 0) {
                logArea->append("> Zapret başarıyla kuruldu ve hizmet başlatıldı!");
                statusLabel->setText("Durum: Çalışıyor");
                statusLabel->setStyleSheet("color: #4ade80;");
                runTests();
            } else {
                logArea->append("> Hata: Kurulum başarısız oldu.");
                statusLabel->setText("Durum: Hata Oluştu");
                statusLabel->setStyleSheet("color: #f87171;");
            }
        });

        process->start("pkexec", QStringList() << "bash" << "-c" << cmd);
    }

    void stopZapret() {
        setButtonsEnabled(false);
        logArea->append("> Zapret durduruluyor ve sistemden kaldırılıyor...");
        statusLabel->setText("Durum: Kaldırılıyor...");
        statusLabel->setStyleSheet("color: #f59e0b;");

        QString cmd = "if [ -f /opt/zapret/uninstall_easy.sh ]; then "
                      "echo -e \"Y\\n\\n\" | /opt/zapret/uninstall_easy.sh; "
                      "fi && "
                      "systemctl stop zapret 2>/dev/null; "
                      "rm -rf /opt/zapret";

        auto *process = new QProcess(this);
        connect(process, &QProcess::readyReadStandardOutput, [this, process]() {
            logArea->append(process->readAllStandardOutput());
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this](int exitCode) {
            setButtonsEnabled(true);
            if (exitCode == 0) {
                logArea->append("> Zapret başarıyla durduruldu ve kaldırıldı.");
                statusLabel->setText("Durum: Durduruldu / Kaldırıldı");
                statusLabel->setStyleSheet("color: #f87171;");
            } else {
                logArea->append("> Hata: Kaldırma işlemi sırasında bir sorun oluştu.");
                statusLabel->setText("Durum: Hata Oluştu");
                statusLabel->setStyleSheet("color: #f87171;");
            }
        });

        process->start("pkexec", QStringList() << "bash" << "-c" << cmd);
    }

    void runTests() {
        btnTest->setEnabled(false);
        logArea->append("> Bağlantı testleri yürütülüyor...");
        
        discordLabel->setText("• Discord: Test ediliyor...");
        discordLabel->setStyleSheet("color: #f59e0b;");
        robloxLabel->setText("• Roblox: Test ediliyor...");
        robloxLabel->setStyleSheet("color: #f59e0b;");

        auto *manager = new QNetworkAccessManager(this);

        QNetworkReply *replyDiscord = manager->get(QNetworkRequest(QUrl("https://discord.com")));
        connect(replyDiscord, &QNetworkReply::finished, [this, replyDiscord]() {
            if (replyDiscord->error() == QNetworkReply::NoError) {
                discordLabel->setText("• Discord: Erişilebilir (OK)");
                discordLabel->setStyleSheet("color: #4ade80; font-weight: bold;");
                logArea->append("> [TEST OK] Discord bağlantısı başarılı.");
            } else {
                discordLabel->setText("• Discord: Erişilemedi");
                discordLabel->setStyleSheet("color: #f87171; font-weight: bold;");
                logArea->append("> [TEST HATA] Discord bağlantısı başarısız.");
            }
        });

        QNetworkReply *replyRoblox = manager->get(QNetworkRequest(QUrl("https://www.roblox.com")));
        connect(replyRoblox, &QNetworkReply::finished, [this, replyRoblox]() {
            btnTest->setEnabled(true);
            if (replyRoblox->error() == QNetworkReply::NoError) {
                robloxLabel->setText("• Roblox: Erişilebilir (OK)");
                robloxLabel->setStyleSheet("color: #4ade80; font-weight: bold;");
                logArea->append("> [TEST OK] Roblox bağlantısı başarılı.");
            } else {
                robloxLabel->setText("• Roblox: Erişilemedi");
                robloxLabel->setStyleSheet("color: #f87171; font-weight: bold;");
                logArea->append("> [TEST HATA] Roblox bağlantısı başarısız.");
            }
        });
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ZapretWindow window;
    window.show();
    return app.exec();
}