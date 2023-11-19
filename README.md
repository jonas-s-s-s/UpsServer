# UpsServer

Server UPS semestrálka C++ ZS2023

## cmake

* Po kompilaci obsahuje debug / release výstup v build složkách
* Složka module obsahuje cmake moduly, které lze pomocí include() použít v cmake skriptech

## doc

* dokumentace a diagramy

## extern

* složka pro externí knihovny a soubory které nejsou součástí projektu
* cmake zde ukládá všechny používané externí knihovny ve složce cmake_deps, výhodou tohoto přístupu je že knihovny není
  třeba zvlášť stahovat pro release / debug build, knihovny jsou plně odděleny od build složky, kterou tedy lze smazat bez
  toho aniž by bylo znovu potřeba knihovny stahovat

## src

* všechny zdrojové soubory aplikace

### game_logic

* soubory popisující chování serveru hry, používají prostředky ze složek linux_commons_lib a network_commons_lib, tím je
  oddělena implmenetace obecných síťových a linuxových struktur od implementace konkrétního serveru pro tuto hru

### linux_commons_lib

* Obsahuje třídu pro epoll a thread-safe frontu synchronizovanou pomocí eventfd. Epoll je efektivní nástroj pro odchycení
  a zpracování událostí spojených s linux file descriptory. Síťový socket nebo eventfd je možné v epoll zaregistrovat a tak
  odchytávat jejich události.

### network_commons_lib

* Obecná implementace prvků TCP serveru a klienta pomocí BSD socketů. Od tříd je možno dědit a tak jim dodat bližší
  specifikaci.

## test

* Obsahuje unit testy pro některé části aplikace, včetně automatických testů samotného server protokolu a skriptu pro
  manuální testování
