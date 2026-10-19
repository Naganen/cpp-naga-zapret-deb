# Naga Zapret Control Center (DPI Bypass GUI)

[![Debian Package](https://img.shields.io/badge/Debian-0.0.11-red.svg?logo=debian)](https://github.com/Naganen/cpp-naga-zapret-deb)
[![Qt Version](https://img.shields.io/badge/Qt-6-green.svg?logo=qt)](https://www.qt.io/)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg?logo=cplusplus)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20amd64-lightgrey.svg?logo=linux)](https://kernel.org)

* [Türkçe Dokümantasyon](#türkçe)
* [English Documentation](#english)

---

<a name="türkçe"></a>
## 🇹🇷 Türkçe

### Genel Bakış

**Naga Zapret**, Linux tabanlı sistemlerde (özellikle Debian/Ubuntu ve türevleri) internet sansürlerini ve Derin Paket İncelemesi (DPI - Deep Packet Inspection) engellemelerini aşmak için kullanılan **Zapret** aracını modern bir Qt6 grafik arayüzü ile yönetmenizi sağlayan bir kontrol merkezidir. 

Başta **Discord**, **Roblox** ve bölgesel olarak erişim engeli uygulanan diğer servislerin kesintisiz çalışmasını sağlamak amacıyla geliştirilmiştir.

---

### Öne Çıkan Özellikler

* 🖥️ **Modern & Şık Grafik Arayüz (Qt6):** Koyu tema destekli, sade ve kullanıcı dostu kontrol paneli.
* 🚀 **Tek Tıkla Kurulum ve Başlatma:** Gelişmiş `install.sh` betiğini arka planda çalıştırarak Zapret ve DNSCrypt/DoT yapılandırmalarını otomatik tamamlar.
* 🧹 **Temiz Kaldırma & Sistem Onarımı:** Zapret'i durdurup kaldırırken `/etc/resolv.conf` DNS ayarlarını otomatik olarak sıfırlar ve internet bağlantısını varsayılan ayarlara güvenle döndürür.
* 📊 **10x Canlı Paket Erişim Testi:** Discord ve Roblox bağlantılarını 10'ar ardışık sorgu ile milisaniye cinsinden gecikme (latency) ve başarı oranıyla test eder.
* 🛡️ **Güvenli Yetkilendirme (`pkexec`):** Root ayrıcalığı gerektiren ağ ve servis işlemlerini PolicyKit (`pkexec`) üzerinden güvenli bir şekilde yürütür.
* 🔄 **Sistem Açılışı Sağlık Kontrolü (`--startup-check`):**
  * `XDG Autostart` sayesinde sistem açılışında 10 saniye sonra arka planda sessizce bağlantı testi yapar.
  * Eğer bağlantıda sorun tespit edilirse (3 başarısız deneme sonrası), kullanıcıyı bilgilendirerek servisi tek tıkla yeniden kurma/başlatma seçeneği sunar.

---

### Sistem Gereksinimleri & Bağımlılıklar

* **İşletim Sistemi:** Linux (Debian, Ubuntu, Linux Mint, Pop!_OS ve diğer Debian tabanlı 64-bit dağıtımlar)
* **Paket Bağımlılıkları:**
  * `libqt6widgets6`
  * `libqt6network6`
  * `pkexec` (PolicyKit)
  * `curl`, `jq`, `tar`, `gzip`
  * `iptables`, `nftables`

---

### Kurulum

#### 1. Hazır `.deb` Paketi ile Kurulum (Önerilen)

Hazır derlenmiş `.deb` paketini doğrudan terminalden kurabilirsiniz:

```bash
sudo apt update
sudo apt install -y ./naga-zapret-0.0.11_amd64.deb
```

Eksik bağımlılıklar oluşursa:
```bash
sudo apt --fix-broken install
```

Kurulum tamamlandıktan sonra uygulamayı uygulama menünüzden (`Naga Zapret GUI`) veya terminalden başlatabilirsiniz:
```bash
naga-zapret
```

---

### Kaynak Koddan Derleme (Build from Source)

Projeyi kendi sisteminizde derlemek isterseniz:

#### 1. Derleme Araçlarını ve Bağımlılıkları Yükleyin:
```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools
```

#### 2. Projeyi Klonlayın ve Derleyin:
```bash
git clone https://github.com/Naganen/cpp-naga-zapret-deb.git
cd cpp-naga-zapret-deb

mkdir build && cd build
cmake ..
make -j$(nproc)
```

Oluşan `naga-zapret` çalıştırılabilir dosyasını test edebilirsiniz:
```bash
./naga-zapret
```

---

### Debian Paketi Oluşturma

Projeyi `.deb` paketi haline getirmek için:

```bash
# 1. Derlenen ikili dosyayı paket klasörüne kopyalayın
cp build/naga-zapret naga-zapret-package/usr/bin/
chmod 755 naga-zapret-package/usr/bin/naga-zapret
chmod 755 naga-zapret-package/usr/share/naga-zapret/install.sh

# 2. Paketi oluşturun
dpkg-deb --build naga-zapret-package naga-zapret-0.0.11_amd64.deb
```

---

### Proje Dosya Yapısı

```
naga-zapret-deb/
├── CMakeLists.txt                 # CMake derleme yapılandırması
├── main.cpp                       # Qt6 GUI ve arka plan servis kontrolü kaynak kodu
├── naga-zapret-0.0.11_amd64.deb   # Dağıtıma hazır Debian kurulum paketi
└── naga-zapret-package/           # Debian paket şablon dizini
    ├── DEBIAN/
    │   └── control                # Paket metadata ve bağımlılık tanımları
    ├── etc/
    │   └── xdg/
    │       └── autostart/         # Açılış denetleyici masaüstü girdisi (--startup-check)
    └── usr/
        ├── bin/                   # naga-zapret çalıştırılabilir ikili dosya
        └── share/
            ├── applications/      # Masaüstü uygulama kısayolu (.desktop)
            └── naga-zapret/       # Kurulum betiği (install.sh)
```

---

<a name="english"></a>
## 🇬🇧 English

### Overview

**Naga Zapret** is a modern Qt6 graphical user interface and autostart monitor designed to configure and manage **Zapret** (Deep Packet Inspection / DPI bypass utility) on Linux systems, with primary packaging for Debian and Ubuntu-based distributions.

It is specifically tailored to bypass internet censorship and throttling on services such as **Discord**, **Roblox**, and other regionally blocked platforms.

---

### Features

* 🖥️ **Modern Qt6 GUI:** Sleek dark-mode interface with clear status indicators and real-time logs.
* 🚀 **One-Click Setup & Launch:** Executes the bundled `install.sh` script to automate DPI bypass and DNS encryption (DNSCrypt/DoT).
* 🧹 **Clean Removal & DNS Restoration:** Completely uninstalls Zapret and automatically cleans up `/etc/resolv.conf`, reverting nameservers safely to defaults (8.8.8.8 / 1.1.1.1).
* 📊 **10x Live Packet Access Test:** Sequentially sends 10 ping/HTTP requests to Discord and Roblox with millisecond-precision response times and health reports.
* 🛡️ **Polkit Integration (`pkexec`):** Safely requests elevated privileges for system-level network configurations.
* 🔄 **System Startup Health Watchdog (`--startup-check`):**
  * Launches silently in the background 10 seconds after desktop login via XDG Autostart.
  * Verifies network accessibility to target platforms.
  * If connectivity issues persist across 3 consecutive checks, prompts the user via a native dialog with an option to repair/reinstall the bypass service.

---

### Requirements & Dependencies

* **OS:** 64-bit Linux (Debian, Ubuntu, Linux Mint, Pop!_OS, etc.)
* **Runtime Dependencies:**
  * `libqt6widgets6`
  * `libqt6network6`
  * `pkexec` (PolicyKit)
  * `curl`, `jq`, `tar`, `gzip`
  * `iptables`, `nftables`

---

### Installation

#### 1. Quick Install via `.deb` Package (Recommended)

Install the prebuilt `.deb` package using `apt`:

```bash
sudo apt update
sudo apt install -y ./naga-zapret-0.0.11_amd64.deb
```

If missing dependencies occur, resolve them with:
```bash
sudo apt --fix-broken install
```

Launch the application from your app menu (`Naga Zapret GUI`) or via terminal:
```bash
naga-zapret
```

---

### Building from Source

To compile the application yourself:

#### 1. Install Build Dependencies:
```bash
sudo apt update
sudo apt install -y build-essential cmake qt6-base-dev qt6-base-dev-tools
```

#### 2. Clone and Build:
```bash
git clone https://github.com/Naganen/cpp-naga-zapret-deb.git
cd cpp-naga-zapret-deb

mkdir build && cd build
cmake ..
make -j$(nproc)
```

Run the built binary:
```bash
./naga-zapret
```

---

### Building the `.deb` Package

To rebuild the Debian package from the workspace:

```bash
# 1. Copy the compiled binary into the package tree
cp build/naga-zapret naga-zapret-package/usr/bin/
chmod 755 naga-zapret-package/usr/bin/naga-zapret
chmod 755 naga-zapret-package/usr/share/naga-zapret/install.sh

# 2. Build the package
dpkg-deb --build naga-zapret-package naga-zapret-0.0.11_amd64.deb
```

---

### File Structure

```
naga-zapret-deb/
├── CMakeLists.txt                 # CMake build configuration
├── main.cpp                       # Qt6 GUI & startup watchdog source code
├── naga-zapret-0.0.11_amd64.deb   # Ready-to-install Debian package
└── naga-zapret-package/           # Debian package skeleton
    ├── DEBIAN/
    │   └── control                # Package metadata and runtime dependencies
    ├── etc/
    │   └── xdg/
    │       └── autostart/         # Autostart desktop entry (--startup-check)
    └── usr/
        ├── bin/                   # naga-zapret executable
        └── share/
            ├── applications/      # Desktop launcher entry (.desktop)
            └── naga-zapret/       # Installer script (install.sh)
```

---

### Author & Maintainer

* **Author:** Alper YILDIRIM (Naganen)
* **Email:** [naganen@outlook.com.tr](mailto:naganen@outlook.com.tr)
* **Repository:** [https://github.com/Naganen/cpp-naga-zapret-deb](https://github.com/Naganen/cpp-naga-zapret-deb)

