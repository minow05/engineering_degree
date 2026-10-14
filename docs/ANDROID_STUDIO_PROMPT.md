# Prompt dla Nowego Projektu: SmartSSH Mobile (Android Studio)

Jesteś doświadczonym programistą Android (Kotlin / Jetpack Compose). Twoim zadaniem jest stworzenie kompletnej, natywnej aplikacji mobilnej w **Android Studio** o nazwie **SmartSSH Mobile**, będącej mobilnym klientem systemu z pracy inżynierskiej:
> *"Projekt i implementacja zdalnego dostępu do prywatnych zasobów plikowych z wykorzystaniem platformy Raspberry Pi"*

---

## 🎯 GŁÓWNY ZAMYSŁ I MODEL SYSTEMU (SINGLE-HOST BINDING MODEL)

Aplikacja **NIE JEST** ogólnym menedżerem setek połączeń SSH (jak Termius czy PuTTY). Działa w oparciu o **Model Wiązania z Jednym Hostem (Single-Host Binding Model)**:

1. **Brak zbędnych aliasów**: Identyfikatorem serwera jest wyłącznie jego **Adres IP / domena** (np. stały adres w wirtualnej sieci ZeroTier VPN, np. `10.147.19.x` lub `172.30.0.1`). Nie używaj pól `host_alias` ani nazw opisowych w bazie/modelach.
2. **Pierwsze uruchomienie (Setup Wizard)**:
   * Wykrycie braku lokalnej konfiguracji i uruchomienie kreatora.
   * Pobranie: Adres IP/domena Raspberry Pi, Port SSH (domyślnie `22`), Nazwa użytkownika (np. `minow`).
   * **Obsługa kluczy SSH Ed25519**: Wygenerowanie asymetrycznej pary kluczy `Ed25519` w aplikacji mobilnej i zapisanie klucza prywatnego w bezpiecznym magazynie (`Android Keystore` / `EncryptedSharedPreferences`). Udostępnienie klucza publicznego (kopiowanie do schowka lub jednorazowy transfer hasłem za pomocą SSH).
   * **Zdalne tworzenie użytkownika (Remote User Provisioning)**: Jeśli podany użytkownik jeszcze nie istnieje na Raspberry Pi, kreator umożliwia jego automatyczne utworzenie na serwerze (poprzez wywołanie skryptu administracyjnego `sudo /home/minow/create_user.sh <username>` lub komendy `sudo useradd -m -s /bin/bash -G ssh-users <username>` z autoryzacją kontem administratora).
   * **Weryfikacja**: Szybki test połączenia TCP na porcie 22 i test sesji SSH.
3. **Trwałość wiązania**: Po ukończeniu konfiguracji stan zostaje zapisany w `EncryptedSharedPreferences` / `DataStore`. Każde kolejne uruchomienie aplikacji wchodzi bezpośrednio do **Ekranu Głównego (Dashboard)** bez pytania o adres IP.
4. **Reset konfiguracji (Settings)**: Opcja *"Reset Configuration"* usuwa wyłącznie lokalny plik/dane konfiguracyjne aplikacji i cofa do Setup Wizarda. **Nigdy** nie usuwa żadnych danych ze zdalnego serwera Raspberry Pi.
5. **Współbieżność i porty**: Połączenia korzystają ze standardowych portów serwera (port `22` dla SSH/SFTP, port `445` dla Samby). Ponieważ protokół TCP identyfikuje połączenia 4-krotką `{IP klienta, port efemeryczny, IP serwera, port 22}`, a OpenSSH forkuje osobny proces dla każdego klienta, aplikacja mobilna łączy się bezpośrednio bez potrzeby dynamicznych pul portów po stronie serwera.

*(UWAGA: Pomiń pisanie testów jednostkowych – skup się na kompletnej, w 100% działającej aplikacji mobilnej).*

---

## 🛠️ STOS TECHNOLOGICZNY (ANDROID)

* **Język**: Kotlin (najnowsza stabilna wersja)
* **UI**: Jetpack Compose + Material 3 (pełne wsparcie dla Dark / Light Mode)
* **Architektura**: MVVM / Clean Architecture (ViewModel, StateFlow, Kotlin Coroutines)
* **Bezpieczeństwo**: Android Keystore, `EncryptedSharedPreferences` (klucz prywatny SSH przechowywany w chronionym magazynie)
* **Biblioteki sieciowe**:
  * **SSH / SFTP / Terminal**: `com.hierynomus:sshj` (zalecane) lub `com.jcraft:jsch`
  * **Samba (SMB/CIFS)**: `com.hierynomus:smbj` (do przeglądania i operacji na udziałach sieciowych SMB)
  * **Kryptografia**: `org.bouncycastle:bcprov-jdk18on` (do generowania par kluczy Ed25519 w formacie OpenSSH)

---

## 🖥️ ŚRODOWISKO HOSTA (RASPBERRY PI)

Serwer Raspberry Pi (Raspberry Pi OS / Debian Linux) posiada:
* **OpenSSH Server**: Port 22, autoryzacja bezhasłowa kluczami publicznymi `Ed25519`.
* **ZeroTier VPN**: Bezpieczny, szyfrowany kanał P2P (wirtualna karta `zt0`, np. `10.147.19.x`).
* **Samba 4 (SMB/CIFS)**:
  * Udział prywatny użytkownika: `\\<IP>\<username>` (ścieżka `/home/<username>`)
  * Udział współdzielony: `\\<IP>\shared` (ścieżka `/home/minow/shared`, grupa `ssh-users`)
* **Telemetria i zasoby**:
  * Dysk: `df -h /`
  * Obciążenie CPU i RAM: `ps aux --sort=-%cpu | head -n 12` oraz `free -h`
  * Limity kwot dyskowych: `quota -u <username>` (lub fallback `du -sh /home/<username>`)
  * Skrypt tworzenia użytkownika: `/home/minow/create_user.sh <username>`

---

## 📱 EKRANY I FUNKCJONALNOŚCI APLIKACJI

### 1. Ekran Kreatora (First-Run Setup Wizard)
* **Krok 1: Parametry Serwera**:
  * Pole tekstowe: Adres IP Raspberry Pi (np. `10.147.19.100`),
  * Pole tekstowe: Port SSH (domyślnie `22`),
  * Pole tekstowe: Nazwa użytkownika (np. `minow`).
* **Krok 2: Klucz SSH & Provisioning Użytkownika**:
  * Przycisk: "Wygeneruj klucz Ed25519" (generuje parę, zapisuje prywatny do Keystore).
  * Wyświetlenie klucza publicznego z przyciskiem "Kopiuj do schowka".
  * Opcja jednorazowego wysłania klucza przez SSH (z podaniem hasła konta).
  * Opcja: "Użytkownik nie istnieje? Stwórz konto na serwerze" (loguje się kontem admina i uruchamia skrypt `create_user.sh`).
* **Krok 3: Test Połączenia**:
  * Przycisk "Testuj połączenie" — sprawdzenie gniazda TCP i handshake SSH.
  * Po sukcesie zapisanie konfiguracji i przejście do Ekranu Głównego.

### 2. Ekran Główny (Host Dashboard)
* **Nagłówek**:
  * Wyświetlanie adresu IP serwera (np. `Serwer: 10.147.19.100`),
  * Badge stanu: `[ONLINE]` (zielony) / `[OFFLINE]` (czerwony) po teście gniazda,
  * Zalogowany użytkownik (np. `Zalogowano jako: minow`).
* **Karty Menu (Moduły)**:
  1. 💻 **Interaktywny Terminal SSH**:
     * Terminal tekstowy z emulacją powłoki,
     * Pasek szybkiego wprowadzania (Ctrl, Tab, Esc, strzałki góra/dół),
     * Wykonywanie poleceń w czasie rzeczywistym.
  2. 📁 **Menedżer Plików i Dysków Sieciowych (Samba & SFTP)**:
     * Przełącznik zakładek: **Katalog Prywatny (`/<username>`)** vs **Katalog Współdzielony (`/shared`)**,
     * Przeglądanie listy plików i podkatalogów (nazwa, ikona typu, rozmiar, data modyfikacji),
     * Pobieranie pliku na urządzenie mobilne (do folderu Downloads),
     * Wysyłanie pliku ze smartfona na Raspberry Pi,
     * Tworzenie nowego katalogu i usuwanie plików z potwierdzeniem,
     * **Czysta obsługa błędów**: Komunikaty błędów (Snackbar/Toast) wyświetlane wyłącznie wtedy, gdy operacja rzeczywiście się nie powiodła.
  3. 📊 **Monitor Telemetrii i Kwot Dyskowych**:
     * Karta CPU & RAM: Odczyt z `ps aux` i `free -h` sformatowany w przejrzyste paski postępu procentowego,
     * Karta Przestrzeni Dyskowej: Wykres kołowy lub pasek zajętości partycji `/` (`df -h /`),
     * Karta Kwoty Dyskowej POSIX: Aktualne zużycie vs limit dyskowy (`quota -u`),
     * Pływający przycisk (FAB) lub akcja "Odśwież".
  4. ⚙️ **Ustawienia i Zarządzanie Hostem**:
     * Podgląd aktywnych parametrów: IP, port, użytkownik, stan klucza Ed25519,
     * Akcja: **"Dodaj nowego użytkownika na Raspberry Pi"** — formularz tworzący nowe konto w systemie Linux i grupie Samby,
     * Akcja: **"Resetuj konfigurację aplikacji"** — monolit ostrzegawczy z potwierdzeniem: czyści lokalny magazyn i cofa do Kreatora bez ingerencji w pliki serwera.

---

## 🚀 ZADANIE DLA CIEBIE (KROK PO KROKU):

1. **Struktura Projektu**: Podaj proponowaną architekturę pakietów (`core`, `network/ssh`, `network/smb`, `data/preferences`, `ui/theme`, `ui/screens/*`).
2. **Konfiguracja Gradle**: Podaj kompletne pliki `build.gradle.kts` (app i project) z wszystkimi niezbędnymi zależnościami (`sshj`, `smbj`, `bouncycastle`, Compose, Material3, Coroutines).
3. **Plik `AndroidManifest.xml`**: Wymagane uprawnienia internetowe i dostępu do pamięci.
4. **Warstwa Danych i Bezpieczeństwa**:
   * `KeyManager.kt`: Generowanie klucza Ed25519, eksport klucza publicznego, bezpieczne trzymanie klucza prywatnego.
   * `ConfigRepository.kt`: Zapis i odczyt konfiguracji (IP, port, username) oraz reset.
5. **Warstwa Sieciowa**:
   * `SSHManager.kt`: Nawiązywanie sesji SSH, egzekucja poleceń zdalnych, obsługa terminala i provisioning użytkownika.
   * `SMBStorageManager.kt`: Przeglądanie zasobów Samby, pobieranie i wysyłanie plików.
6. **Warstwa UI (Jetpack Compose)**:
   * `SetupWizardScreen.kt`: Formularz kroków konfiguracji i generowania kluczy.
   * `DashboardScreen.kt`: Główny ekran nawigacji z kafelkami.
   * `TerminalScreen.kt`: Ekran interaktywnego terminala.
   * `FileManagerScreen.kt`: Przeglądarka plików (Samba/SFTP) z pobieraniem i uploadem.
   * `ResourceMonitorScreen.kt`: Telemetria sprzętowa i kwoty dyskowe.
   * `SettingsScreen.kt`: Ustawienia, dodawanie użytkownika i reset.
