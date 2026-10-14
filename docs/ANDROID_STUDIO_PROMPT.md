\# Prompt dla Nowego Projektu: SmartSSH Mobile (Android Studio)



Jesteś doświadczonym programistą Android (Kotlin / Jetpack Compose). Twoim zadaniem jest stworzenie kompletnej, natywnej aplikacji mobilnej w \*\*Android Studio\*\* o nazwie \*\*SmartSSH Mobile\*\*, będącej mobilnym klientem systemu z mojej pracy inżynierskiej:

> \*"Implementacja systemu zdalnego dostępu do zasobów na platformie Raspberry Pi"\*



\---



\## 🎯 GŁÓWNY ZAMYSŁ I MODEL SYSTEMU (SINGLE-HOST BINDING MODEL)



Aplikacja \*\*NIE JEST\*\* ogólnym menedżerem wielu połączeń SSH (jak Termius czy PuTTY).

Aplikacja jest dedykowana do powiązania z \*\*jednym, konkretnym serwerem Raspberry Pi\*\*:

1\. \*\*Pierwsze uruchomienie\*\*: Aplikacja wykrywa brak lokalnej konfiguracji i uruchamia \*\*Setup Wizard\*\* (Kreator Konfiguracji).

2\. \*\*Kreator pobiera\*\*:

&#x20;  - Adres IP / domenę Raspberry Pi (np. adres w sieci ZeroTier VPN, domyślnie `172.30.0.1` lub `10.147.19.x`),

&#x20;  - Port SSH (domyślnie `22`),

&#x20;  - Nazwę użytkownika (np. `minow`),

&#x20;  - Uwierzytelnianie kluczem SSH (wygenerowanie pary kluczy Ed25519 w aplikacji lub import, wdrożenie klucza publicznego na Raspberry Pi),

&#x20;  - Test połączenia.

3\. \*\*Zapis powiązania\*\*: Konfiguracja jest trwale zapisywana w bezpiecznym magazynie lokalnym (`EncryptedSharedPreferences` / `DataStore`).

4\. \*\*Kolejne uruchomienia\*\*: Aplikacja startuje od razu na \*\*Ekranie Głównym (Dashboard)\*\* przypisanego hosta (bez pytania o IP).

5\. \*\*Reset konfiguracji\*\*: W Settings dostępna jest opcja \*Reset Configuration\* (czyści lokalną konfigurację i cofa do Wizarda, \*\*BEZ\*\* usuwania danych na zdalnym Raspberry Pi).



\*(UWAGA: Pomiń pisanie testów jednostkowych – skup się na w 100% działającej aplikacji mobilnej).\*



\---



\## 🛠️ STOS TECHNOLOGICZNY (ANDROID)



\- \*\*Język\*\*: Kotlin (najnowsza wersja)

\- \*\*UI\*\*: Jetpack Compose + Material 3

\- \*\*Architektura\*\*: MVVM (ViewModel, StateFlow, Coroutines)

\- \*\*Bezpieczeństwo\*\*: Android Keystore / `EncryptedSharedPreferences` (klucz prywatny SSH \*\*nie może\*\* leżeć otwartym tekstem)

\- \*\*Biblioteki sieciowe / SSH / Samba\*\*:

&#x20; - \*\*SSH / SFTP\*\*: `com.jcraft:jsch` lub nowoczesne `com.hierynomus:sshj` (do zdalnych komend, SFTP i terminala)

&#x20; - \*\*Samba (SMB/CIFS)\*\*: `com.hierynomus:smbj` (do przeglądania udziałów SMB z Raspberry Pi)

&#x20; - \*\*Kryptografia\*\*: BouncyCastle (do generowania kluczy Ed25519)



\---



\## 🖥️ ŚRODOWISKO HOST-SERWERA (RASPBERRY PI)



Aplikacja komunikuje się z serwerem Raspberry Pi (Raspberry Pi OS / Debian) skonfigurowanym następująco:

1\. \*\*OpenSSH Server\*\*: Port 22, autoryzacja bezhasłowa kluczem `ed25519`, użytkownik `minow` lub `rachel`.

2\. \*\*Sieć VPN\*\*: ZeroTier P2P VPN (apka łączy się przez wirtualne IP).

3\. \*\*Samba 4 (SMB/CIFS)\*\*:

&#x20;  - Udział prywatny użytkownika: `\\\\<IP>\\<username>` (np. `/home/minow`)

&#x20;  - Udział współdzielony: `\\\\<IP>\\shared` (znajduje się w `/home/minow/shared`)

4\. \*\*Kwoty dyskowe i system\*\*:

&#x20;  - Dysk: `df -h /`

&#x20;  - Zużycie CPU i RAM: `ps aux --sort=-%cpu | head -n 12; free -h`

&#x20;  - Kwota dyskowa użytkownika: `quota -u <username> 2>/dev/null || du -sh /home/<username>`



\---



\## 📱 EKRANY I FUNKCJONALNOŚCI APLIKACJI



\### 1. Ekran Kreatora (First-Run Setup Wizard)

\- Krok 1: Wprowadzenie adresu IP serwera, portu i nazwy użytkownika.

\- Krok 2: Generowanie klucza Ed25519. Wyświetlenie klucza publicznego (opcja "Kopiuj do schowka" lub jednorazowe wysłanie hasłem przez SSH za pomocą `ssh-copy-id`).

\- Krok 3: Przycisk "Test Connection". Po sukcesie przejście do ekranu głównego.



\### 2. Ekran Główny (Host Dashboard \& Operations)

\- \*\*Nagłówek\*\*:

&#x20; - Wyświetlanie aktualnego adresu IP serwera (np. `Host: 172.30.0.1`),

&#x20; - Status połączenia: Badge `\[ONLINE]` (zielony) / `\[OFFLINE]` (czerwony) po szybkim sprawdzeniu portu 22.

&#x20; - Zalogowany użytkownik (np. `minow`).

\- \*\*Karty / Kafelki menu\*\*:

&#x20; 1. 💻 \*\*Interactive SSH Terminal\*\*:

&#x20;    - Terminal tekstowy z możliwością wpisywania poleceń powłoki (z obsługą ANSI / kolorów lub czysty emulator terminala),

&#x20;    - Wysyłanie poleceń w aktywnej sesji SSH.

&#x20; 2. 📁 \*\*Remote File \& Network Storage Manager\*\*:

&#x20;    - Przełącznik: \*\*Dysk Domowy (`\[minow]`)\*\* vs \*\*Dysk Współdzielony (`\[shared]`)\*\*.

&#x20;    - Wbudowana przeglądarka plików (SMB lub SFTP): lista plików i folderów, rozmiary, data modyfikacji.

&#x20;    - Podstawowe akcje: Pobierz plik na telefon, Wyślij plik z telefonu na serwer, Usuń plik, Utwórz folder.

&#x20; 3. 📊 \*\*Resource \& Storage Quota Monitor\*\*:

&#x20;    - Karta obciążenia procesora (CPU) i pamięci operacyjnej (RAM) – ładnie sformatowane wykresy lub paski postępu procentowego na podstawie komend `ps aux` i `free -h`.

&#x20;    - Karta przestrzeni dyskowej – zajętość partycji głównej na podstawie `df -h /`.

&#x20;    - Karta kwoty dyskowej użytkownika – aktualne limity na podstawie `quota -u`.

&#x20;    - Przycisk "Odśwież".

&#x20; 4. ⚙️ \*\*Settings \& Reconfiguration\*\*:

&#x20;    - Podgląd zapisanych parametrów połączenia (IP, port, użytkownik, ścieżka/status klucza).

&#x20;    - Przycisk \*\*"Reset Configuration"\*\* z monitem potwierdzającym: czyści lokalną konfigurację i restartuje aplikację do Setup Wizarda (informując, że dane na Raspberry Pi pozostają nietknięte).



\---



\## 🚀 ZADANIE DLA CIEBIE:



Rozpocznij tworzenie projektu krok po kroku:

1\. Podaj strukturę pakietów w projekcie (`core`, `network`, `ui`, `data`).

2\. Podaj pliki konfiguracyjne Gradle (`build.gradle.kts` na poziomie aplikacji i projektu) z niezbędnymi zależnościami (`sshj`/`jsch`, `smbj`, Jetpack Compose, Material3, Coroutines).

3\. Przedstaw kod kluczowych serwisów (`SSHManager`, `SMBStorageManager`, `KeyManager`, `ConfigRepository`).

4\. Przedstaw kod interfejsu w Jetpack Compose (Kreator, Dashboard, Terminal, File Browser, Monitor Zasobów).

