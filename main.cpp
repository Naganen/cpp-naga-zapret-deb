#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>

class ZapretWindow : public QWidget {
public:
    ZapretWindow() {
        setWindowTitle("Zapret UI - DPI Bypass Tool - By Naganen");
        resize(500, 400);

        auto *layout = new QVBoxLayout(this);

        statusLabel = new QLabel("Durum: Hazır", this);
        statusLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        layout->addWidget(statusLabel);

        auto *btnStart = new QPushButton("Zapret Kur / Başlat ve Test Et", this);
        btnStart->setStyleSheet("background-color: #2e7d32; color: white; padding: 10px;");
        layout->addWidget(btnStart);

        discordLabel = new QLabel("Discord: Test Edilmedi", this);
        robloxLabel = new QLabel("Roblox: Test Edilmedi", this);
        layout->addWidget(discordLabel);
        layout->addWidget(robloxLabel);

        logArea = new QTextEdit(this);
        logArea->setReadOnly(true);
        logArea->setStyleSheet("background-color: #1e1e1e; color: #00ff00;");
        layout->addWidget(logArea);

        connect(btnStart, &QPushButton::clicked, this, &ZapretWindow::startZapret);
    }

private:
    QLabel *statusLabel;
    QLabel *discordLabel;
    QLabel *robloxLabel;
    QTextEdit *logArea;

    void startZapret() {
        logArea->append("Kurulum başlatılıyor (Root yetkisi isteniyor)...");
        statusLabel->setText("Durum: Kuruluyor...");

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
            if (exitCode == 0) {
                logArea->append("Zapret başarıyla kuruldu ve başlatıldı!");
                statusLabel->setText("Durum: Çalışıyor. Testler yapılıyor...");
                runTests();
            } else {
                logArea->append("Hata: Kurulum başarısız oldu.");
                statusLabel->setText("Durum: Hata oluştu");
            }
        });

        process->start("pkexec", QStringList() << "bash" << "-c" << cmd);
    }

    void runTests() {
        auto *manager = new QNetworkAccessManager(this);

        QNetworkReply *replyDiscord = manager->get(QNetworkRequest(QUrl("https://discord.com")));
        connect(replyDiscord, &QNetworkReply::finished, [this, replyDiscord]() {
            if (replyDiscord->error() == QNetworkReply::NoError) {
                discordLabel->setText("Discord: Bağlantı Başarılı (Erişilebilir)");
                discordLabel->setStyleSheet("color: green; font-weight: bold;");
            } else {
                discordLabel->setText("Discord: Bağlantı Başarısız");
                discordLabel->setStyleSheet("color: red; font-weight: bold;");
            }
        });

        QNetworkReply *replyRoblox = manager->get(QNetworkRequest(QUrl("https://www.roblox.com")));
        connect(replyRoblox, &QNetworkReply::finished, [this, replyRoblox]() {
            if (replyRoblox->error() == QNetworkReply::NoError) {
                robloxLabel->setText("Roblox: Bağlantı Başarılı (Erişilebilir)");
                robloxLabel->setStyleSheet("color: green; font-weight: bold;");
            } else {
                robloxLabel->setText("Roblox: Bağlantı Başarısız");
                robloxLabel->setStyleSheet("color: red; font-weight: bold;");
            }
            statusLabel->setText("Durum: İşlem Tamamlandı");
        });
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    ZapretWindow window;
    window.show();
    return app.exec();
}