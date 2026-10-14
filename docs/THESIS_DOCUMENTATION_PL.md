# Dokumentacja Techniczna Pracy Inżynierskiej: System SmartSSH

**Tytuł pracy inżynierskiej:** *Projekt i implementacja zdalnego dostępu do prywatnych zasobów plikowych z wykorzystaniem platformy Raspberry Pi*  
**Kierunek / Specjalność:** Informatyka / Inżynieria Oprogramowania  
**Platforma docelowa:** Serwer Raspberry Pi (Raspberry Pi OS / Debian Linux) oraz Wieloplatformowy Klient (`smart-ssh`)  
**Język i standard:** C++20 | CMake | Clang (`clang-cl` / `clang++`) | GoogleTest  
**Język dokumentu:** Polski  

---

## 1. Streszczenie i Wymagania Systemowe

**System Zdalnego Dostępu SmartSSH** to zintegrowana platforma klient-serwer przeznaczona do przekształcenia mikrokomputera Raspberry Pi w bezpieczne, prywatne centrum magazynowania danych, hosting kontenerów procesowych oraz monitor telemetryczny.

Architektura systemu obejmuje dwa główne środowiska wykonawcze:
1. **Środowisko Serwerowe (Platforma Raspberry Pi)**: Bezobsługowa instancja systemu Linux wyposażona w serwer OpenSSH, sieć P2P VPN ZeroTier, demon zasobów sieciowych Samba (SMB/CIFS), środowisko uruchomieniowe kontenerów Docker, system nadzoru usług systemd oraz mechanizm kwot dyskowych POSIX.
2. **Aplikacja Kliencka (`smart-ssh`)**: Wieloplatformowa aplikacja w języku C++20 działająca w oparciu o **Model Wiązania z Jednym Hostem (Single-Host Binding Model)**. Zapewnia jednolite, menu sterowane interfejsem CLI zarządzanie zdalnymi zasobami plikowymi, udziałami sieciowymi Samba, kontenerami Docker, usługami systemd oraz statystykami sprzętowymi z wykorzystaniem bezhasłowej autoryzacji SSH.

### Główne Cele i Zakres Systemu
* **Bezpieczny Zdalny Dostęp do Danych**: Szyfrowany zarząd plikami oraz współdzielenie zasobów SMB/CIFS w niebezpiecznych sieciach publicznych.
* **Model Wiązania z Jednym Hostem**: Wyeliminowanie chaotycznego zarządzania połączeniami poprzez bezpośrednie związanie aplikacji z serwerem Raspberry Pi podczas pierwszego uruchomienia i zapisanie stanu w `smartssh_config.cfg`.
* **Zero Trwałego Przechowywania Hasła**: Wykorzystanie kluczy asymetrycznych Ed25519, co całkowicie eliminuje przechowywanie jawnych haseł w plikach konfiguracyjnych lub pamięci podręcznej.
* **Wieloplatformowość**: Natywna kompilacja i działanie w systemach Windows, Linux oraz macOS z pojedynczego kodu źródłowego C++20.

---

## 2. Infrastruktura Serwerowa i Technologie Hosta (Raspberry Pi)

Infrastruktura serwerowa opiera się na wydajnych, standardowych usługach sieciowych i magazynowych wdrożonych w systemie Raspberry Pi OS.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      Platforma Serwerowa Raspberry Pi                   │
│                                                                         │
│   ┌──────────────────────┐    ┌─────────────────────────────────────┐   │
│   │   ZeroTier P2P VPN   │    │           Serwer OpenSSH            │   │
│   │ (ChaCha20-Poly1305)  │    │      (Port 22, Autoryzacja ed25519) │   │
│   └──────────┬───────────┘    └──────────────────┬──────────────────┘   │
│              │ Wirtualny Karta `zt0`             │ TTY / Wykonanie      │
│              ▼                                   ▼                      │
│   ┌─────────────────────────────────────────────────────────────────┐   │
│   │                          Jądro Linux                            │   │
│   │                (System Plików POSIX & Uprawnienia)              │   │
│   └───────┬─────────────────────────┬───────────────────────┬───────┘   │
│           │                         │                       │           │
│           ▼                         ▼                       ▼           │
│  ┌─────────────────┐       ┌─────────────────┐     ┌─────────────────┐  │
│  │ Izolacja Użytk. │       │ Samba SMB/CIFS  │     │ Srodowisko Docker│  │
│  │ /home/minow     │       │ Udziały Szyfr.  │     │ & Usługi systemd│  │
│  │ /home/rachel    │       │ /home/*/shared  │     │ Kontrola Procesu│  │
│  └─────────────────┘       └─────────────────┘     └─────────────────┘  │
└─────────────────────────────────────────────────────────────────────────┘
```

### 2.1 System Operacyjny i Usługi Bazowe
* **Raspberry Pi OS (Debian Linux)**: Zapewnia POSIX-ową izolację procesów, zarządzanie kontami użytkowników, kontrolę demona systemd oraz obsługę sprzętu.
* **Demon OpenSSH (`sshd`)**: Realizuje bezpieczne wykonywanie poleceń powłoki, transfer plików przez SFTP oraz alokację interaktywnych pseudoterminali TTY. Skonfigurowany na wymuszoną autoryzację kluczem publicznym (`PubkeyAuthentication yes`) oraz zablokowane logowanie konta root.

### 2.2 Wirtualna Sieć Prywatna (ZeroTier P2P VPN)
* **Szyfrowana Sieć Mesh Peer-to-Peer**: ZeroTier tworzy wirtualny przełącznik Ethernet warstwy 2 w oparciu o UDP/IP przy użyciu mechanizmów pokonywania NAT (STUN/TURN).
* **Protokoły Kryptograficzne**: Wszystkie pakiety przesyłane pomiędzy klientem `smart-ssh` a serwerem Raspberry Pi są szyfrowane end-to-end przy użyciu wymiany kluczy Curve25519 i algorytmu ChaCha20-Poly1305.
* **Izolacja Sieciowa**: Tworzy prywatną wirtualną kartę sieciową (`zt0`) ze stałym adresem IP (np. `10.147.19.x`), eliminując potrzebę przekierowywania portów na routerze oraz ekspozycji na publiczne adresy IP.

### 2.3 Architektura Magazynu Danych Samba (SMB/CIFS)
W celu umożliwienia bezszwowego montowania dysków sieciowych na komputerach klienckich, serwer Samba 4 został skonfigurowany z rygorystyczną izolacją katalogów:
* **Izolacja Katalogów Użytkowników**:
  * Użytkownik `minow`: Prywatny katalog domowy `/home/minow`.
  * Użytkownik `rachel`: Prywatny katalog domowy `/home/rachel`.
* **Współdzielony Magazyn Administratora**:
  * Udział współdzielony `/home/minow/shared` umieszczony jest w ścieżce domowej głównego administratora (`minow`), co daje pełną kontrolę zarządczą, możliwość audytu oraz zarządzania uprawnieniami do plików publicznych.
* **Wfragment Konfiguracji Samba (`/etc/samba/smb.conf`)**:
  ```ini
  [minow]
      path = /home/minow
      valid users = minow
      read only = no
      browsable = yes
      force create mode = 0600
      force directory mode = 0700

  [rachel]
      path = /home/rachel
      valid users = rachel
      read only = no
      browsable = yes
      force create mode = 0600
      force directory mode = 0700

  [shared]
      path = /home/minow/shared
      valid users = minow rachel
      read only = no
      browsable = yes
      force create mode = 0664
      force directory mode = 0775
  ```

### 2.4 Konteneryzacja i Zarządzanie Systemem
* **Środowisko Uruchomieniowe Docker**: Odpowiada za kontenerowe mikrousługi na Raspberry Pi. Aplikacja kliencka umożliwia podgląd aktywnych kontenerów (`docker ps -a`), pobieranie logów (`docker logs`) oraz wykonywanie operacji cyklu życia (`docker start`, `stop`, `restart`).
* **Demon Inicjalizacji systemd**: Monitoruje demony systemowe (np. `smbd`, `zerotier-one`, `ssh`), pozwalając na zdalny podgląd ich statusu.
* **Kwoty Dyskowe POSIX**: Limity pamięci masowej użytkowników są kontrolowane przez mechanizm kwot dyskowych Linuksa (`quota -u <username>`), co pozwala monitorować i ograniczać zużycie dysku.

### 2.5 Architektura Portów, Współbieżność Połączeń i Model Wielu Klientów

W architekturze systemu SmartSSH kluczowym zagadnieniem jest obsługa równoczesnych połączeń od wielu użytkowników bez konieczności dynamicznego przydzielania portów czy implementacji dedykowanej puli portów (*port pooling*).

#### 2.5.1 Identyfikacja Połączeń w Modelu TCP/IP (4-krotka Połączenia)
W protokole TCP pojedyncze logiczne połączenie sieciowe jest jednoznacznie identyfikowane przez **4-krotkę (4-tuple)** w nagłówkach pakietów IP i TCP:
$$\{\text{Źródłowy Adres IP},\, \text{Źródłowy Port Efemeryczny},\, \text{Docelowy Adres IP Serwera},\, \text{Docelowy Port Usługi}\}$$

* **Port Serwera (Docelowy)**: Usługi systemowe Raspberry Pi nasłuchują na stałych, dobrze znanych portach (OpenSSH: `22`, Samba: `445`). Gniazdo serwera znajduje się w stanie nasłuchiwania (`LISTEN`).
* **Port Klienta (Źródłowy)**: Podczas nawiązywania połączenia jądro systemu operacyjnego stacji klienckiej przydziela procesowi wolny port z puli portów efemerycznych (zgodnie z IANA: zakres `49152–65535`).
* **Brak Kolizji Portów**: Gdy dwaj różni użytkownicy (np. z adresów `10.147.19.11` i `10.147.19.12`) lub dwie niezależne instancje aplikacji łączą się równolegle z serwerem Raspberry Pi na port `22`, każde z tych połączeń tworzy unikalne gniazdo w tablicy stanu połączeń jądra Linuksa w stanie `ESTABLISHED`. Nie dochodzi do konfliktu portów po stronie serwera.

#### 2.5.2 Model Współbieżności Usług Serwera (OpenSSH i Samba)
1. **Wieloprocesowy Model OpenSSH (`sshd`)**:
   * Główny demon `sshd` działa z uprawnieniami administratora i oczekuje na pakiety `SYN`.
   * Po zaakceptowaniu połączenia funkcją systemową `accept()`, demon wywołuje `fork()`, tworząc odrębny proces potomny dedykowany wyłącznie tej konkretnej sesji sieciowej.
   * Proces potomny przeprowadza wymianę kluczy kryptograficznych, dokonuje obniżenia uprawnień (*privilege separation*) i po uwierzytelnieniu kluczem Ed25519 uruchamia powłokę użytkownika w kontekście jego `UID`/`GID`.
   * Dzięki temu sesje użytkowników są w pełni odizolowane procesowo i pamięciowo.

2. **Współbieżność Demona Samba (`smbd`)**:
   * Podobnie jak OpenSSH, demon `smbd` tworzy osobny proces lub wątek roboczy dla każdego nawiązanego połączenia SMB (port `445`).
   * Równoczesny dostęp do udziałów sieciowych jest synchronizowany przez mechanizmy blokad na poziomie jądra Linuksa oraz protokołu SMB (*opportunistic locks / oplocks*).

#### 2.5.3 Analiza Zasadności Dynamicznych Pul Portów
W trakcie projektowania systemu rozważano ewentualną konieczność dynamicznego zarządzania portami serwera. Wykazano, że:
* **Pula portów jest zbędna**: Dynamiczne porty po stronie serwera byłyby konieczne wyłącznie w przypadku tunelowania zwrotnego (*SSH Remote Port Forwarding*, flaga `-R`), w którym klienci rezerwują porty nasłuchujące po stronie serwera pod własne usługi. SmartSSH nie wykorzystuje mechanizmu `-R`, lecz bezpośrednie połączenia do usług serwerowych.
* **Skalowalność**: Standardowe demony Linuksa bez trudu obsługują dziesiątki równoległych połączeń. Ograniczeniem wydajnościowym na platformie Raspberry Pi jest pamięć RAM i czas procesora, a nie dostępność gniazd TCP na porcie `22` czy `445`.

---

## 3. Architektura Oprogramowania Klienckiego (`smart-ssh`)

Aplikacja kliencka została napisana w czystym standardzie C++20 w oparciu o wielowarstwową zdematerializowaną strukturę.

```
┌────────────────────────────────────────────────────────────────────────┐
│                        Rdzeń Klienta SmartSSH                          │
│                                                                        │
│   ┌────────────────────────────────────────────────────────────────┐   │
│   │                      Warstwa Prezentacji                       │   │
│   │  ┌──────────────────────────────────────────────────────────┐  │   │
│   │  │ CLI (`ui/cli/cli_app.cpp`, `ui/terminal.cpp`)            │  │   │
│   │  │ Menu ANSI | Natychmiastowe Klawisze | Auto-Czyszczenie   │  │   │
│   │  └────────────────────────────┬─────────────────────────────┘  │   │
│   └───────────────────────────────┼────────────────────────────────┘   │
│                                   │                                    │
│   ┌───────────────────────────────▼────────────────────────────────┐   │
│   │                        Rdzeń Aplikacji                         │   │
│   │  ┌───────────────────┐ ┌───────────────────┐ ┌───────────────┐  │   │
│   │  │    Application    │ │   Configuration   │ │ ConnectionMgr │  │   │
│   │  └───────────────────┘ └───────────────────┘ └───────────────┘  │   │
│   │  ┌───────────────────┐ ┌───────────────────┐ ┌───────────────┐  │   │
│   │  │    KeyManager     │ │     SSHConfig     │ │ SystemOps     │  │   │
│   │  └───────────────────┘ └───────────────────┘ └───────────────┘  │   │
│   └───────────────────────────────┬────────────────────────────────┘   │
│                                   │                                    │
│   ┌───────────────────────────────▼────────────────────────────────┐   │
│   │                       Moduły Funkcjonalne                      │   │
│   │  ┌───────────────────┐ ┌───────────────────┐ ┌───────────────┐  │   │
│   │  │    FileManager    │ │  ProcessManager   │ │ ResourceMon   │  │   │
│   │  └───────────────────┘ └───────────────────┘ └───────────────┘  │   │
│   └────────────────────────────────────────────────────────────────┘   │
└────────────────────────────────────────────────────────────────────────┘
```

### 3.1 Model Wiązania z Jednym Hostem (Single-Host Binding Model)
W przeciwieństwie do ogólnych menedżerów połączeń SSH, program `smart-ssh` realizuje dedykowany **Model Wiązania z Jednym Hostem**:
1. **Kreator Pierwszego Uruchomienia (`installer::setup_wizard`)**: Jeśli plik `smartssh_config.cfg` nie istnieje lub opcja `is_configured = false`, aplikacja uruchamia interaktywny kreator konfigurujący adres IP serwera, nazwę użytkownika SSH, port SSH, ścieżkę klucza oraz nazwy udziałów sieciowych.
2. **Automatyczne Generowanie Kluczy (`core::key_manager`)**: Kreator uruchamia komendę `ssh-keygen -t ed25519` w celu lokalnego wygenerowania pary kluczy asymetrycznych (jeśli nie istnieją), a następnie wysyła klucz publiczny na Raspberry Pi za pomocą `ssh-copy-id`.
3. **Trwała Konfiguracja Wiązania (`core::configuration`)**: Ustawienia zostają zapisane w pliku `smartssh_config.cfg`. Każde kolejne uruchomienie programu automatycznie wczytuje zapisanego hosta bez pytania użytkownika.
4. **Reset Konfiguracji**: Użytkownik może zresetować stan aplikacji poprzez opcję **Settings → Reset Configuration**, co usuwa lokalny plik konfiguracyjny i przywraca stan początkowy bez jakiejkolwiek ingerencji w pliki na serwerze Raspberry Pi.

### 3.2 Warstwa Prezentacji (`ui/`)
* **`ui/terminal.cpp`**: Realizuje niekanoniczny, natychmiastowy odczyt klawiszy (`read_key_immediate()`) w systemach POSIX (`termios`) oraz Windows (`_getch()`). Odpowiada za czyszczenie ekranu (`clear_screen()`) i oczekiwanie na klawisz powrotu (`wait_for_key()`).
* **`ui/menu.cpp`**: Generuje interaktywne menu konsolowe z kolorowaniem ANSI.
* **`ui/cli/cli_app.cpp`**: Steruje wyborem opcji w menu i przekazuje wykonanie do modułów funkcjonalnych. Czyści ekran terminala po dokonaniu wyboru.

### 3.3 Warstwa Rdzenia Aplikacji (`core/`)
* **`core::application`**: Główny kontroler zarządzający cyklem życia aplikacji, detekcją kreatora oraz pętlą zdarzeń menu.
* **`core::configuration`**: Obsługuje zapis i odczyt parametrów wiązania hosta (`host_alias`, `host_address`, `username`, `port`, `identity_file`, `is_configured`).
* **`core::connection_manager`**: Wykonuje komendy zdalne w procesie potomnym OpenSSH (`exec_command()`) oraz zapewnia interaktywny tryb TTY (`connect_interactive()`).
* **`core::key_manager`**: Automatyzuje proces generowania i kopiowania kluczy Ed25519.
* **`core::ssh_config`**: Parser i generator standardowych plików konfiguracyjnych OpenSSH (`~/.ssh/config`).
* **`core::system_operations`**: Obsługuje mapowanie ścieżek Windows UNC (`\\<IP>\<share>`) oraz wywoływanie systemowych eksploratorów plików (`explorer.exe`, `xdg-open`, `open`).

### 3.4 Moduły Funkcjonalne (`modules/`)
* **`modules::file_manager`**: Zarządza zdalnymi katalogami (`ls -la`), tworzeniem katalogów (`mkdir`), usuwaniem plików (`rm -rf`), transferem plików (`scp`) oraz otwieraniem dysków sieciowych Samba.
* **`modules::process_manager`**: Podgląd kontenerów Docker (`docker ps -a`), pobieranie logów (`docker logs`), sterowanie kontenerami (`start`, `stop`, `restart`) oraz kontrola jednostek systemd.
* **`modules::resource_monitor`**: Odczyt telemetryczny obciążenia CPU (`ps aux`), pamięci RAM (`free -h`), zużycia dysków (`df -h`) oraz limitów kwot dyskowych POSIX (`quota -u`).

---

## 4. Projekt Wieloplatformowy i Implementacja

Aplikacja `smart-ssh` została zaimplementowana w standardzie C++20 z wykorzystaniem warstw abstrakcji dla poszczególnych systemów operacyjnych:

### 4.1 Abstrakcje Platformowe

| Komponent | Implementacja Windows | Implementacja Linux / macOS |
| :--- | :--- | :--- |
| **Natychmiastowy Odczyt Klawisza** | `_getch()` z biblioteki `<conio.h>` | `tcgetattr()` / `tcsetattr()` z trybem surowym `termios` |
| **Czyszczenie Terminala** | Kody ANSI `\033[2J\033[1;1H` + Windows Console Mode | Sekwencja sterująca ANSI `\033[2J\033[1;1H` |
| **Wykonanie Procesu** | `_popen()` / `_pclose()` poprzez `utils::exec` | `popen()` / `pclose()` poprzez `utils::exec` |
| **Menedżer Dysków Sieciowych**| Ścieżki Windows UNC (`\\<IP>\<share>`) z wywołaniem `ShellExecuteW` | Natywne schematy URL SMB (`smb://<IP>/<share>`) z `xdg-open` / `open` |

### 4.2 Struktura Przenośna CMake
System budowania wykorzystuje celowe abstrakcje CMake (`smart_ssh_utils`, `smart_ssh_core`, `smart_ssh_ui`, `smart_ssh_installer`, `smart_ssh_modules`, `smart-ssh`), co umożliwia kompilację w środowiskach Windows MSVC/ClangCL, Linux GCC/Clang oraz macOS Apple Clang.

---

## 5. Procedura Kompilacji Wersji Wydaniowej (Release Standalone Binary)

Poniższe instrukcje przedstawiają proces tworzenia pojedynczego, samowystarczalnego pliku wykonywalnego bez zewnętrznych plików symboli debugowania (pliki `.pdb` w systemie Windows).

### 5.1 Kompilacja w Systemie Windows (Plik `.exe` bez `.pdb`)
Przykładowe wywołanie w wierszu poleceń Visual Studio 2022 lub PowerShell przy użyciu narzędzi ClangCL / MSVC:

```powershell
# 1. Konfiguracja katalogu kompilacji Release z zestawem narzędzi ClangCL
cmake -B build_release -S . -G "Visual Studio 17 2022" -T ClangCL -DCMAKE_BUILD_TYPE=Release

# 2. Kompilacja samodzielnego pliku wykonywalnego Release
cmake --build build_release --config Release

# 3. Ścieżka Wynikowa Pliku Executable:
# build_release\out\Release\smart-ssh.exe
```

> [!NOTE]
> Powstały plik `smart-ssh.exe` zawiera cały skompilowany kod statycznie zlinkowany z bibliotekami uruchomieniowymi C++ i nie wymaga do działania zewnętrznych plików `.pdb` ani `.dll`.

### 5.2 Kompilacja w Systemie Linux (Pojedynczy Binarny)
W systemach Debian / Ubuntu / Raspberry Pi OS:

```bash
# 1. Konfiguracja kompilacji Release z użyciem Clang / GCC
cmake -B build_release -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++

# 2. Budowanie pliku wykonywalnego
cmake --build build_release --config Release

# 3. Usunięcie symboli debugowania (strip)
strip build_release/out/smart-ssh

# Ścieżka Wynikowa Pliku Executable:
# build_release/out/smart-ssh
```

### 5.3 Kompilacja w Systemie macOS (Pojedynczy Binarny)
W systemie macOS (Intel / Apple Silicon):

```bash
# 1. Konfiguracja kompilacji Release z użyciem Apple Clang
cmake -B build_release -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++

# 2. Budowanie pliku wykonywalnego
cmake --build build_release --config Release

# 3. Usunięcie symboli debugowania (strip)
strip build_release/out/smart-ssh

# Ścieżka Wynikowa Pliku Executable:
# build_release/out/smart-ssh
```

---

## 6. Model Bezpieczeństwa i Izolacji Poświadczeń

1. **Klucze Asymetryczne Ed25519**: Tożsamość jest weryfikowana za pomocą 256-bitowych kluczy krzywych eliptycznych Edwardsa (Ed25519).
2. **Lokalny Magazyn Poświadczeń**: Klucze prywatne (`id_ed25519_<host>`) są przechowywane wyłącznie w chronionym katalogu `.ssh` użytkownika (`~/.ssh/`) z uprawnieniami `0600` (lub listami ACL w systemie Windows). Klucz prywatny nigdy nie trafia do pliku wykonywalnego ani nie jest przesyłany przez sieć.
3. **Izolacja Danych przy Resecie**: Reset lokalnego stanu klienta (`smartssh_config.cfg`) kasuje tylko lokalne parametry połączenia. Wszystkie pliki na Raspberry Pi, udziały Samba oraz kontenery Docker pozostają nienaruszone.
4. **Podwójna Warstwa Szyfrowania**: Szyfrowany tunel ZeroTier VPN (ChaCha20-Poly1305) + sesja OpenSSH SSHv2 (AES-GCM / ChaCha20-Poly1305).

---

## 7. Weryfikacja i Testy Automatyczne

Projekt zawiera automatyczne testy jednostkowe i regresyjne GoogleTest (`gtest`) połączone ze środowiskiem **Atrapy Serwera SSH (`tests::ssh_stub`)**:

```
[==========] Running 31 tests from 11 test suites ran. (2023 ms total)
[  PASSED  ] 31 tests.
```

### Podsumowanie Zestawu Testów (11 Pakietów Testowych)
* **`FunctionalModuleTest` (4 testy)**: Weryfikacja monitoringu CPU/RAM, formatowania dysków, parsowania limitów kwot dyskowych POSIX oraz pełnego cyklu życia aplikacji.
* **`SSHStubEnvironmentTest` (5 testów)**: Symulacja odpowiedzi serwera SSH (stubbing) dla dysków, procesów Docker, kwot użytkowników oraz listowania plików.
* **`SmartSSHConfigurationTest` (1 test)**: Zapis, odczyt i reset pliku konfiguracyjnego.
* **`SmartSSHExecTest` (1 test)**: Wykonywanie poleceń w podprocesach.
* **`SmartSSHConfigTest` (1 test)**: Odizolowany parser i generator plików konfiguracji SSH.
* **`RegressionTest` (6 testów)**: Odporność na uszkodzone pliki konfiguracyjne, niestandardowe i graniczne numery portów, spacje w ścieżkach tożsamości, wielokrotne resety oraz translację aliasów na surowe adresy IP udziałów Samba.
* **`CoreUnitConfigurationTest` (3 testy)**: Domyślny stan, zapisywanie poprawnych danych i resetowanie konfiguracji.
* **`CoreUnitSSHConfigTest` (2 testy)**: Dodawanie, modyfikacja i wyszukiwanie wpisów SSH config.
* **`CoreUnitKeyManagerTest` (2 testy)**: Rozwiązywanie ścieżek katalogu `.ssh` i listowanie kluczy.
* **`CoreUnitSystemOperationsTest` (2 testy)**: Formatowanie komend sprawdzania dysku i kwot.
* **`UtilsUnitExecTest` (4 testy)**: Wykonywanie poleceń powłoki, weryfikacja kodów wyjścia oraz obecności narzędzi systemowych.
