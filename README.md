# Naga Zapret Control Center (DPI Bypass GUI)

[![Debian Package](https://img.shields.io/badge/Debian-0.0.15-red.svg?logo=debian)](https://github.com/Naganen/naga-zapret)
[![Qt Version](https://img.shields.io/badge/Qt-6-green.svg?logo=qt)](https://www.qt.io/)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?logo=cplusplus)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20amd64-lightgrey.svg?logo=linux)](https://kernel.org)
[![GUI-First](https://img.shields.io/badge/Console-Free-brightgreen.svg)]()

> **💡 Vizyon / Vision:** Linux'ta internet sansürünü aşmak için terminal komutlarıyla, karmaşık scriptlerle ve DNS dosyalarıyla uğraşma devrine son. Kurulumundan testine, sistem açılış kontrolünden kaldırılmasına kadar **%100 grafik arayüz (GUI)** odaklı bir deneyim!

---

* [🇹🇷 Türkçe Dokümantasyon](#türkçe)
  * [Neler Yeni? (v0.0.15)](#neler-yeni)
  * [Neden Naga Zapret? (Projenin Amacı)](#projenin-amacı)
  * [Konsol Açmadan Kurulum](#konsolsuz-kurulum)
  * [Konsol Açmadan Kullanım Rehberi](#konsolsuz-kullanım)
  * [Özellikler](#özellikler)
  * [Geliştiriciler İçin (Kaynak Kod & Terminal)](#geliştiriciler-için)
  * [Teşekkürler ve Atıflar](#teşekkürler-ve-atıflar)
* [🇬🇧 English Documentation](#english)
  * [What's New? (v0.0.15)](#whats-new)
  * [Why Naga Zapret? (Project Purpose)](#project-purpose)
  * [Console-Free GUI Installation](#gui-installation)
  * [Console-Free Usage Guide](#gui-usage)
  * [Key Features](#key-features)
  * [For Developers (Source Code & CLI)](#for-developers)
  * [Credits & Acknowledgements](#credits)
* [Yazar & İletişim / Author](#author)

---

<a name="türkçe"></a>
## 🇹🇷 Türkçe

<a name="neler-yeni"></a>
### 🚀 Neler Yeni? (v0.0.15)

Naga Zapret **v0.0.15** sürümü ile birlikte arka plan çalışma mekanizması, sistem entegrasyonu ve görsel kimlikte önemli yenilikler getirildi:

* 🛡️ **Sistem Tepsisi (Tray Icon) & Arka Planda Çalışma:** Pencere kapatıldığında (`X`) uygulama sonlanmaz; arka planda sistem tepsisine küçülür. Tepsi simgesine tıklanarak pencere anında geri açılabilir veya sağ tık menüsünden durum takip edilebilir.
* 🔔 **Tepsiden Bağlantı Testi & Canlı Bildirimler:** Arayüz kapalıyken bile sistem tepsisinden "Bağlantı Testi Yap" seçildiğinde test arka planda başlatılır ve sonuçlar (Discord/Roblox paket başarı oranları) masaüstü bildirim balonu ile anlık iletilir.
* ⚡ **Tek Çalışma Örneği (Single Instance):** Uygulama arka planda veya sistem tepsisinde açıkken masaüstü kısayolundan tekrar çalıştırıldığında ikinci bir kopya açılmaz; mevcut çalışan pencere otomatik olarak öne ve ekrana getirilir.
* 🎨 **Resmi Vektörel Logolar & Karanlık Tema Uyumu:** Açık ve koyu masaüstleri için SVG logolar eklendi. Karanlık Linux masaüstlerinde görev çubuğu ve uygulama menüsü kısayolunun beyaz logo ile kusursuz görünmesi sağlandı.
* 📦 **Gelişmiş Paket Yöneticisi Desteği (AppStream MetaInfo):** Ubuntu Software Center, KDE Discover ve GNOME Software gibi paket yöneticilerinde zengin açıklama, kategori ve resmi logo desteği sağlandı.

---

<a name="projenin-amacı"></a>
### 🎯 Neden Naga Zapret? (Projenin Amacı)

Linux dünyasında Discord, Roblox veya benzeri servislere uygulanan DPI (Derin Paket İncelemesi) engellemelerini aşmak için genellikle karmaşık terminal komutları, root ayrıcalıklı betikler ve el ile DNS düzenlemeleri gerekir. Bu durum, Linux'u günlük hayatında **uçbirim (konsol) açmadan** grafik arayüzle kullanmak isteyen kullanıcılar için büyük bir engel teşkil etmektedir.

**Naga Zapret**, tam olarak bu zorluğu ortadan kaldırmak için doğdu. Amacı; son kullanıcının tek bir komut satırı dahi görmeden:
1. Uygulamayı çift tıklayarak kurabilmesi,
2. Tek butonla sansür aşma servisini başlatabilmesi,
3. Discord ve Roblox erişimini grafik arayüzden test edebilmesi,
4. İstediğinde DNS kilidini ve servisi tek tıkla arkasında çöp bırakmadan kaldırabilmesi,
5. Sistem tepsisi ve arka plan nöbetçisi ile sürekli kesintisiz bağlantı güvencesine sahip olmasıdır.

---

<a name="konsolsuz-kurulum"></a>
### 🖱️ Konsol Açmadan Kurulum (GUI Yöntemi)

Herhangi bir terminal/uçbirim açmanıza gerek yoktur:

1. **Paketi İndirin:** Projenin [Releases](https://github.com/Naganen/naga-zapret/releases) sayfasından `naga-zapret-0.0.15_amd64.deb` dosyasını indirin.
2. **Çift Tıklayın:** İndirdiğiniz `.deb` dosyasına çift tıklayın (veya sağ tıklayıp *Yazılım Kur*, *GDebi*, *Ubuntu Yazılım Merkezi* ya da *Discover* ile açın).
3. **"Kur" Butonuna Basın:** Açılan pencerede **Kur (Install)** düğmesine tıklayın.
4. **Şifrenizi Girin:** Sistem sizden kullanıcı şifrenizi istediğinde girin.
5. **Hazır!** Kurulum saniyeler içinde tamamlanır.

---

<a name="konsolsuz-kullanım"></a>
### 🎮 Konsol Açmadan Kullanım Rehberi

#### 1. Uygulamayı Başlatma
* Masaüstü uygulama menünüzü (Başlat menüsü / Uygulamalar) açın.
* Arama çubuğuna **Naga Zapret** yazın ve çıkan logoya tıklayın.

#### 2. Zapret'i Kurma ve Başlatma
* Uygulama penceresinde vurgulu **"Zapret Kur ve Başlat"** butonuna tıklayın.
* Ekrana gelen güvenli sistem yetkilendirme penceresine (Polkit) şifrenizi girin.
* Canlı yükleme çubuğu ve dönen gösterge ile sürecin ilerleyişini takip edin. Ayrıntılı terminal çıktılarını izlemek isterseniz **"Canlı Günlük Penceresini Aç"** butonuna tıklayabilirsiniz. Kurulum bittiğinde durum **"Durum: Aktif Çalışıyor"** olarak güncellenir.

#### 3. Canlı Erişim & Gecikme Testi Yapma
* Sağ üstteki **"Bağlantı Testi"** butonuna tıklayın.
* Uygulama, arka planda Discord ve Roblox servislerine 10'ar adet gerçek paket gönderir.
* Canlı ilerleme çubuğu 20 test adımını anlık gösterir; test bitiminde yeşil **"Erişilebilir (10/10 OK)"** onayını görebilirsiniz.

#### 4. Sistem Tepsisi ve Arka Plan Çalışması
* Pencerenin kapatma (**X**) butonuna bastığınızda uygulama kapanmaz; sistem tepsisine küçülerek arka planda kesintisiz çalışmayı sürdürür.
* Sistem tepsisindeki simgeye tıklayarak pencereyi dilediğiniz an geri açabilir veya sağ tıklayarak arayüzü açmadan doğrudan **"Bağlantı Testi Yap"** seçeneğiyle test yapabilirsiniz. Test sonuçları masaüstü bildirim balonu ile anında iletilir.
* Uygulamadan tamamen çıkmak için tepsi menüsündeki **"Tamamen Çıkış Yap"** seçeneğini kullanabilirsiniz.

#### 5. Tek Tıkla Kaldırma ve İnterneti Normale Döndürme
* DPI aşma servisini durdurmak veya tamamen kaldırmak istediğinizde **"Zapret Durdur ve Kaldır"** butonuna basmanız yeterlidir.
* Sistem; Zapret kalıntılarını siler, `/etc/resolv.conf` DNS kilidini çözer ve internet ayarlarınızı varsayılan güvenli DNS sunucularına otomatik olarak döndürür.

#### 6. Akıllı Arka Plan Koruması (Açılışta Otomatik Denetim)
* Uygulamayı her gün elle açmanıza bile gerek yoktur.
* Sistem her açıldığında (10 saniye gecikmeyle) Naga Zapret arka planda sessizce Discord bağlantısını kontrol eder.
* Eğer internet servis sağlayıcınız veya sansür nedeniyle bir kopma yaşanırsa, ekrana şık bir uyarı penceresi gelir ve *"Sansür aşma servisini yeniden başlatmak ister misiniz?"* diye sorar. **"Evet"** dediğiniz anda sorun otomatik olarak çözülür.

---

<a name="özellikler"></a>
### ✨ Temel Özellikler

* 🎨 **Tamamen Grafik Arayüz (Qt6):** Koyu temalı, temiz, keskin hatlı modern kontrol merkezi.
* 🛡️ **Sistem Tepsisi & Arka Plan Modu:** Kapatıldığında tepsiye küçülme, tepsiden test yapabilme ve canlı masaüstü bildirimleri.
* ⚡ **Tek Örnek (Single Instance):** Uygulama açıkken tekrar başlatıldığında mevcut pencereyi öne odaklama.
* 🌐 **Çift Dil Desteği:** Türkçe ve İngilizce tam dil desteği (otomatik sistem dili algılama ve anlık geçiş).
* 🗂️ **Snaplenebilir Canlı Konsol:** Ana pencereden bağımsız açılan ve ana pencereyi takip eden canlı log ekranı.
* ⏳ **Görsel İlerleme Takibi:** Dönen yükleniyor animasyonu ve canlı ilerleme çubuğu.
* 📦 **Akıllı Kurulum Motoru:** Arka planda çalışan ve [keift/zapret](https://github.com/keift/zapret) projesinden sağlanan otomatik yapılandırma betiği.
* ⚡ **10x Canlı Paket Testi:** Discord ve Roblox'a anlık erişim doğrulama ve ms gecikme analizi.
* 🛡️ **Polkit (`pkexec`) Koruması:** Terminal şifresi girmek yerine standart Linux yönetici onay penceresi.
* 🧹 **Kalıntısız Temizlik:** DNS kilitlemelerini otomatik çözer, internet kopmalarına yol açmaz.
* 🔔 **Sessiz Açılış Nöbetçisi (`--startup-check`):** Arka planda çalışıp sadece bir sorun olduğunda kullanıcıyı haberdar eden otomatik sağlık kontrolü.

---

<a name="geliştiriciler-için"></a>
### 🛠️ Geliştiriciler & İleri Düzey Kullanıcılar İçin

<details>
<summary><b>Uçbirim (Terminal) ile Kurulum, Derleme ve Paketleme Adımlarını Göster</b></summary>

#### Terminal ile `.deb` Kurulumu:
```bash
sudo apt update
sudo apt install -y ./naga-zapret-0.0.15_amd64.deb
```

#### Bağımlılıklar:
`libqt6widgets6`, `libqt6network6`, `pkexec`, `curl`, `jq`, `tar`, `gzip`, `iptables`, `nftables`

#### Kaynak Koddan Derleme (CMake & Qt6):
```bash
# Geliştirme kütüphanelerini yükleyin
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools

# Derleyin
mkdir build && cd build
cmake ..
make -j$(nproc)

# Çalıştırın
./naga-zapret
```

#### Debian Paketini Yeniden Oluşturma:
```bash
cp build/naga-zapret naga-zapret-package/usr/bin/
chmod 755 naga-zapret-package/usr/bin/naga-zapret
chmod 755 naga-zapret-package/usr/share/naga-zapret/install.sh
dpkg-deb --build naga-zapret-package naga-zapret-0.0.15_amd64.deb
```
</details>

---

<a name="teşekkürler-ve-atıflar"></a>
### 🙏 Teşekkürler ve Atıflar

Bu projenin arka planında çalışan sistem yapılandırmalarında açık kaynak topluluğunun değerli çalışmalarından faydalanılmıştır:

* **[keift/zapret](https://github.com/keift/zapret):** Bu paketin içerisinde yer alan (`/usr/share/naga-zapret/install.sh`) gelişmiş kurulum, otomatik dağıtım algılama ve DNS yapılandırma betiği **keift** deposundan alınmıştır. Katkıları için teşekkür ederiz.
* **[bol-van/zapret](https://github.com/bol-van/zapret):** Derin Paket İncelemesi (DPI) engellemelerini aşan efsanevi orijinal Zapret çekirdeğinin geliştiricisi.

---

<a name="english"></a>
## 🇬🇧 English

<a name="whats-new"></a>
### 🚀 What's New? (v0.0.15)

With the Naga Zapret **v0.0.15** release, major improvements were introduced for background operations, system integration, and branding:

* 🛡️ **System Tray Integration & Minimize to Tray:** Closing the window (`X`) no longer terminates the process; it neatly minimizes to the system tray and continues running in the background. Left-click or use the context menu to reopen.
* 🔔 **Tray Diagnostic Tests & Live Desktop Notifications:** Trigger connection tests directly from the tray context menu even when the UI is hidden; receive instant desktop notification bubbles for test start and completion results.
* ⚡ **Single-Instance Application Architecture:** Launching the app while an instance is already running or minimized to the tray seamlessly activates and focuses the existing window instead of spawning duplicate processes.
* 🎨 **Official Vector Branding & Dark Theme Optimization:** Integrated tailored light and dark SVG logos. App launchers and taskbars now display a crisp white icon on dark Linux desktop environments.
* 📦 **AppStream MetaInfo & Software Center Integration:** Added rich metadata specification for modern Linux package managers (Ubuntu Software Center, Discover, GNOME Software) including icon, localized synopsis, and feature breakdown.

---

<a name="project-purpose"></a>
### 🎯 Why Naga Zapret? (Project Purpose)

On Linux systems, bypassing Deep Packet Inspection (DPI) and regional censorship (such as bans on Discord or Roblox) usually requires terminal knowledge: editing root configuration files, managing iptables/nftables scripts, and troubleshooting DNS manually. For users who prefer a modern desktop operating system **without touching the terminal/console**, this creates an unnecessary barrier.

**Naga Zapret** was created to eliminate this pain point completely. It provides a pure, end-to-end **GUI experience**:
1. Install by simply double-clicking the `.deb` file.
2. Start and configure DPI bypass with a single click.
3. Diagnose connectivity to Discord and Roblox via automated visual tests.
4. Minimize to tray to keep the bypass monitored in the background.
5. Uninstall cleanly and restore default DNS settings with zero terminal commands.
6. Benefit from an automatic background watchdog on system boot that alerts you only if a connection problem is detected.

---

<a name="gui-installation"></a>
### 🖱️ Console-Free GUI Installation

No terminal required at any step:

1. **Download:** Grab the latest `naga-zapret-0.0.15_amd64.deb` from the [Releases](https://github.com/Naganen/naga-zapret/releases) page.
2. **Double Click:** Double-click the downloaded `.deb` file (or right-click -> *Open with Software Install* / *GDebi* / *Discover* / *App Center*).
3. **Click Install:** Hit the **Install** button on the package installer window.
4. **Enter Password:** Type your user password when prompted by your desktop environment.
5. **Done!** The app is installed and ready in your applications menu.

---

<a name="gui-usage"></a>
### 🎮 Console-Free Usage Guide

#### 1. Launching the App
* Open your desktop Application Menu (Start menu).
* Search for **Naga Zapret** and click its icon.

#### 2. Installing and Starting Zapret
* Click the prominent **"Install & Start Zapret"** button.
* Enter your password in the standard graphical Polkit authentication window.
* Monitor progress with the live loading bar and spinner. To view verbose terminal output, click **"Show Live Logs Console"** to open the side console. Once completed, the status label turns green: **"Status: Running Active"**.

#### 3. Running Live Connection Tests
* Click **"Test Connection"**.
* The application sends 10 real packets to Discord and Roblox, reporting latency in milliseconds and displaying a clear green/red success badge (e.g., `10/10 OK`). Progress can be observed on the live progress bar.

#### 4. System Tray & Background Operations
* When you close the window (**X**), the app does not quit; it minimizes to the system tray and continues running in the background.
* Click the tray icon to restore the window, or right-click to run **"Run Connection Test"** directly from the tray without opening the main window. Test outcomes are delivered via desktop notification bubbles.
* To exit the application completely, select **"Exit Completely"** from the tray menu.

#### 5. Clean Removal & DNS Restoration
* If you ever want to remove Zapret, click **"Zapret Durdur ve Kaldır" (Stop & Uninstall)**.
* The tool will cleanly remove Zapret, unprotect `/etc/resolv.conf`, restore default DNS resolvers (8.8.8.8 / 1.1.1.1), and leave your network clean.

#### 6. Background Autostart Watchdog
* You don't even need to open the app on every boot.
* 10 seconds after system startup, Naga Zapret performs a silent background health check.
* If access to Discord/Roblox fails 3 times, a user-friendly modal dialog pops up asking: *"Connection issue detected. Would you like to reinstall/restart the bypass service?"* Clicking **Yes** resolves the issue immediately.

---

<a name="key-features"></a>
### ✨ Key Features

* 🎨 **Pure Graphical Interface (Qt6):** Sleek, sharp, distraction-free modern dark cyber control center.
* 🛡️ **System Tray & Background Mode:** Minimize-to-tray capability, tray-triggered connectivity tests, and instant notification bubbles.
* ⚡ **Single Instance Architecture:** Seamlessly focuses and restores existing instance when launched again.
* 🌐 **Bilingual Support:** Full Turkish and English language support with automatic locale detection.
* 🗂️ **Snappable Live Console:** Standalone log window that neatly docks next to the main application.
* ⏳ **Visual Progress Tracking:** Integrated progress bar and rotating loader animation.
* 📦 **Automated Setup Engine:** Sourced from [keift/zapret](https://github.com/keift/zapret) for multi-distro setup & DNS configuration.
* ⚡ **10x Live Diagnostic Ping Test:** Real HTTP packet tests with millisecond latency tracking.
* 🛡️ **Polkit Integration (`pkexec`):** Secure native administrative prompts without CLI passwords.
* 🧹 **Zero-Leftover Cleanup:** Automatically unlocks DNS files and restores standard internet settings.
* 🔔 **Silent Startup Watchdog (`--startup-check`):** XDG autostart service that keeps your connection alive silently.

---

<a name="for-developers"></a>
### 🛠️ For Developers & Power Users

<details>
<summary><b>Click to expand CLI installation, build instructions, and package creation</b></summary>

#### Install via Terminal:
```bash
sudo apt update
sudo apt install -y ./naga-zapret-0.0.15_amd64.deb
```

#### Dependencies:
`libqt6widgets6`, `libqt6network6`, `pkexec`, `curl`, `jq`, `tar`, `gzip`, `iptables`, `nftables`

#### Building from Source (CMake & Qt6):
```bash
# Install development headers
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./naga-zapret
```

#### Rebuilding the `.deb` Package:
```bash
cp build/naga-zapret naga-zapret-package/usr/bin/
chmod 755 naga-zapret-package/usr/bin/naga-zapret
chmod 755 naga-zapret-package/usr/share/naga-zapret/install.sh
dpkg-deb --build naga-zapret-package naga-zapret-0.0.15_amd64.deb
```
</details>

---

<a name="credits"></a>
### 🙏 Credits & Acknowledgements

Naga Zapret relies on excellent open-source work by the community:

* **[keift/zapret](https://github.com/keift/zapret):** The automated installer, multi-distro detection, and DNS setup script bundled in this package (`/usr/share/naga-zapret/install.sh`) is sourced from **keift**'s repository. Special thanks for their work!
* **[bol-van/zapret](https://github.com/bol-van/zapret):** The foundational core DPI bypass engine.

---

<a name="author"></a>
### 👤 Yazar & İletişim / Author & Maintainer

* **Geliştirici / Author:** Alper YILDIRIM (Naganen)
* **E-Posta / Email:** [naganen@outlook.com.tr](mailto:naganen@outlook.com.tr)
* **GitHub Deposu / Repository:** [https://github.com/Naganen/naga-zapret](https://github.com/Naganen/naga-zapret)
