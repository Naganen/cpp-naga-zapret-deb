# Naga Zapret Control Center (DPI Bypass GUI)

[![Debian Package](https://img.shields.io/badge/Debian-0.0.11-red.svg?logo=debian)](https://github.com/Naganen/cpp-naga-zapret-deb)
[![Qt Version](https://img.shields.io/badge/Qt-6-green.svg?logo=qt)](https://www.qt.io/)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?logo=cplusplus)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20amd64-lightgrey.svg?logo=linux)](https://kernel.org)
[![GUI-First](https://img.shields.io/badge/Console-Free-brightgreen.svg)]()

> **💡 Vizyon / Vision:** Linux'ta internet sansürünü aşmak için terminal komutlarıyla, karmaşık scriptlerle ve DNS dosyalarıyla uğraşma devrine son. Kurulumundan testine, sistem açılış kontrolünden kaldırılmasına kadar **%100 grafik arayüz (GUI)** odaklı bir deneyim!

---

* [🇹🇷 Türkçe Dokümantasyon](#türkçe)
  * [Neden Naga Zapret? (Projenin Amacı)](#projenin-amacı)
  * [Konsol Açmadan Kurulum](#konsolsuz-kurulum)
  * [Konsol Açmadan Kullanım Rehberi](#konsolsuz-kullanım)
  * [Özellikler](#özellikler)
  * [Geliştiriciler İçin (Kaynak Kod & Terminal)](#geliştiriciler-için)
* [🇬🇧 English Documentation](#english)
  * [Why Naga Zapret? (Project Purpose)](#project-purpose)
  * [Console-Free GUI Installation](#gui-installation)
  * [Console-Free Usage Guide](#gui-usage)
  * [Key Features](#key-features)
  * [For Developers (Source Code & CLI)](#for-developers)
* [Yazar & İletişim / Author](#author)

---

<a name="türkçe"></a>
## 🇹🇷 Türkçe

<a name="projenin-amacı"></a>
### 🎯 Neden Naga Zapret? (Projenin Amacı)

Linux dünyasında Discord, Roblox veya benzeri servislere uygulanan DPI (Derin Paket İncelemesi) engellemelerini aşmak için genellikle karmaşık terminal komutları, root ayrıcalıklı betikler ve el ile DNS düzenlemeleri gerekir. Bu durum, Linux'u günlük hayatında **uçbirim (konsol) açmadan** grafik arayüzle kullanmak isteyen kullanıcılar için büyük bir engel teşkil etmektedir.

**Naga Zapret**, tam olarak bu zorluğu ortadan kaldırmak için doğdu. Amacı; son kullanıcının tek bir komut satırı dahi görmeden:
1. Uygulamayı çift tıklayarak kurabilmesi,
2. Tek butonla sansür aşma servisini başlatabilmesi,
3. Discord ve Roblox erişimini grafik arayüzden test edebilmesi,
4. İstediğinde DNS kilidini ve servisi tek tıkla arkasında çöp bırakmadan kaldırabilmesi,
5. Bilgisayarı her açtığında sistemin arka planda durumu denetleyip gerekirse kullanıcıyı grafik bildirimle uyarmasıdır.

---

<a name="konsolsuz-kurulum"></a>
### 🖱️ Konsol Açmadan Kurulum (GUI Yöntemi)

Herhangi bir terminal/uçbirim açmanıza gerek yoktur:

1. **Paketi İndirin:** Projenin [Releases](https://github.com/Naganen/cpp-naga-zapret-deb/releases) sayfasından `naga-zapret-0.0.11_amd64.deb` dosyasını indirin.
2. **Çift Tıklayın:** İndirdiğiniz `.deb` dosyasına çift tıklayın (veya sağ tıklayıp *Yazılım Kur*, *GDebi*, *Ubuntu Yazılım Merkezi* ya da *Discover* ile açın).
3. **"Kur" Butonuna Basın:** Açılan pencerede **Kur (Install)** düğmesine tıklayın.
4. **Şifrenizi Girin:** Sistem sizden kullanıcı şifrenizi istediğinde girin.
5. **Hazır!** Kurulum saniyeler içinde tamamlanır.

---

<a name="konsolsuz-kullanım"></a>
### 🎮 Konsol Açmadan Kullanım Rehberi

#### 1. Uygulamayı Başlatma
* Masaüstü uygulama menünüzü (Başlat menüsü / Uygulamalar) açın.
* Arama çubuğuna **Naga Zapret** yazın ve çıkan simgeye tıklayın.

#### 2. Zapret'i Kurma ve Başlatma
* Uygulama penceresinde yeşil vurgulu **"Zapret Kur ve Başlat"** butonuna tıklayın.
* Ekrana gelen güvenli sistem yetkilendirme penceresine (Polkit) şifrenizi girin.
* Uygulama penceresindeki siyah log ekranında kurulumun adım adım tamamlandığını göreceksiniz. Bittiğinde durum kutucuğu **"Durum: Çalışıyor"** olarak yeşile dönecektir.

#### 3. Canlı Erişim & Gecikme Testi Yapma
* Sağ üstteki **"Bağlantı Testi Yap"** butonuna tıklayın.
* Uygulama, arka planda Discord ve Roblox servislerine 10'ar adet gerçek paket gönderir.
* Gecikme sürelerini (milisaniye cinsinden) log alanından izleyebilir, test bitiminde yeşil **"Erişilebilir (10/10 OK)"** onayını görebilirsiniz.

#### 4. Tek Tıkla Kaldırma ve İnterneti Normale Döndürme
* DPI aşma servisini durdurmak veya tamamen kaldırmak istediğinizde **"Zapret Durdur ve Kaldır"** butonuna basmanız yeterlidir.
* Sistem; Zapret kalıntılarını siler, `/etc/resolv.conf` DNS kilidini çözer ve internet ayarlarınızı varsayılan güvenli DNS sunucularına otomatik olarak döndürür.

#### 5. Akıllı Arka Plan Koruması (Açılışta Otomatik Denetim)
* Uygulamayı her gün elle açmanıza bile gerek yoktur.
* Sistem her açıldığında (10 saniye gecikmeyle) Naga Zapret arka planda sessizce Discord bağlantısını kontrol eder.
* Eğer internet servis sağlayıcınız veya sansür nedeniyle bir kopma yaşanırsa, ekrana şık bir uyarı penceresi gelir ve *"Sansür aşma servisini yeniden başlatmak ister misiniz?"* diye sorar. **"Evet"** dediğiniz anda sorun otomatik olarak çözülür.

---

<a name="özellikler"></a>
### ✨ Temel Özellikler

* 🎨 **Tamamen Grafik Arayüz (Qt6):** Koyu temalı, temiz ve anlaşılır kontrol merkezi.
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
sudo apt install -y ./naga-zapret-0.0.11_amd64.deb
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
dpkg-deb --build naga-zapret-package naga-zapret-0.0.11_amd64.deb
```
</details>

---

<a name="english"></a>
## 🇬🇧 English

<a name="project-purpose"></a>
### 🎯 Why Naga Zapret? (Project Purpose)

On Linux systems, bypassing Deep Packet Inspection (DPI) and regional censorship (such as bans on Discord or Roblox) usually requires terminal knowledge: editing root configuration files, managing iptables/nftables scripts, and troubleshooting DNS manually. For users who prefer a modern desktop operating system **without touching the terminal/console**, this creates an unnecessary barrier.

**Naga Zapret** was created to eliminate this pain point completely. It provides a pure, end-to-end **GUI experience**:
1. Install by simply double-clicking the `.deb` file.
2. Start and configure DPI bypass with a single click.
3. Diagnose connectivity to Discord and Roblox via automated visual tests.
4. Uninstall cleanly and restore default DNS settings with zero terminal commands.
5. Benefit from an automatic background watchdog on system boot that alerts you only if a connection problem is detected.

---

<a name="gui-installation"></a>
### 🖱️ Console-Free GUI Installation

No terminal required at any step:

1. **Download:** Grab the latest `naga-zapret-0.0.11_amd64.deb` from the [Releases](https://github.com/Naganen/cpp-naga-zapret-deb/releases) page.
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
* Click the prominent **"Zapret Kur ve Başlat" (Install & Start)** button.
* Enter your password in the standard graphical Polkit authentication window.
* Follow real-time progress in the embedded log terminal. Once completed, the status label turns green: **"Durum: Çalışıyor" (Running)**.

#### 3. Running Live Connection Tests
* Click **"Bağlantı Testi Yap" (Run Connection Test)**.
* The application sends 10 real packets to Discord and Roblox, reporting latency in milliseconds and displaying a clear green/red success badge (e.g., `10/10 OK`).

#### 4. Clean Removal & DNS Restoration
* If you ever want to remove Zapret, click **"Zapret Durdur ve Kaldır" (Stop & Uninstall)**.
* The tool will cleanly remove Zapret, unprotect `/etc/resolv.conf`, restore default DNS resolvers (8.8.8.8 / 1.1.1.1), and leave your network clean.

#### 5. Background Autostart Watchdog
* You don't even need to open the app on every boot.
* 10 seconds after system startup, Naga Zapret performs a silent background health check.
* If access to Discord/Roblox fails 3 times, a user-friendly modal dialog pops up asking: *"Connection issue detected. Would you like to reinstall/restart the bypass service?"* Clicking **Yes** resolves the issue immediately.

---

<a name="key-features"></a>
### ✨ Key Features

* 🎨 **Pure Graphical Interface (Qt6):** Sleek, distraction-free dark control center.
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
sudo apt install -y ./naga-zapret-0.0.11_amd64.deb
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
dpkg-deb --build naga-zapret-package naga-zapret-0.0.11_amd64.deb
```
</details>

---

<a name="author"></a>
### 👤 Yazar & İletişim / Author & Maintainer

* **Geliştirici / Author:** Alper YILDIRIM (Naganen)
* **E-Posta / Email:** [naganen@outlook.com.tr](mailto:naganen@outlook.com.tr)
* **GitHub Deposu / Repository:** [https://github.com/Naganen/cpp-naga-zapret-deb](https://github.com/Naganen/cpp-naga-zapret-deb)
