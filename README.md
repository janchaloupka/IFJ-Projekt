# IFJ Projekt 2018
Překladač jazyka IFJ18 - Projekt do IFJ 2018, první varianta zadání


## Struktura repozitáře

* **.vscode** - Konfigurační soubory VS Code (debugování, doporučené pluginy, nastavení workspace)
* **doc** - Dokumentace projektu
* **src** - Zdrojový kód překladače


## Pravidla pro zápis kódu

- Struktura souboru
	- Kódování UTF-8
	- Na typu odřádkování nezáleží, může být \n i \r\n
	- Na začátku každého souboru umístit hlavičku doxygen zápisem se stručným popisem obsahu souboru
- Struktura kódu
	- Odsazení TABem (ve VS Code automaticky)
		- Odsazuje se každý blok kódu (uvnitř složených závorek)
		- Odsazuje se příkaz následující ihned za podmínkou nebo cyklem, pokud není uvnitř složených závorek
	- Složené závorky přilepit na předchozí kód př. `if(true){...}else{...}` nebo `void foo(){...}`
	- Prvky ve výrazech (mimo závorky) a znak `=` (přiřazení) oddělit mezerou, př. `(1 + i) * bar / 1.2` nebo `int foo = 2;`
	- Pokud se má vykonat jen jeden příkaz (v podmínce, v cyklu, ...), je povoleno vynechat složené závorky. Samotný příkaz musí být na novém řádku
- Pojmenování funkcí, proměnných, atd.
	- Psát anglicky
	- Funkce a proměnné ve stylu *camelCase* - začít malým písmenem, př. `nextLine`
	- Definice struct ve stylu *CamelCase* - začít velkým písmenem, př. `struct StringList`
	- Konstanty (ať už zadefinovaná pomocí `#define` nebo `const <type>`) *VELKÝMI_PÍSMENY*, př. `#define STR_LEN`
- Komentáře a dokumentace
	- Psát česky
	- Pro dokumentaci použít doxygen zápis. Pro ty co používají VS Code je doporučený plugin *Doxygen Documentation Generator*. S pluginem stačí napsat `\** ↵` např. před definici funkce a vytvoří se odpovídající šablona.

Příklad dokumentace funkce *(plugin automaticky generuje)*
```c
/**
 * Převede celé číslo ze stringu na integer
 * 
 * @param str Vstupní string
 * @param success Uloží true pokud operace proběhla úspěšně
 * @return int Převedené číslo
 */
int strToNum(char *str, bool *success);
```

Příklad dokumentace proměnné na stejném řádku
```c
pToken nextToken = NULL //<! Další token v pořadí
```

Příklad hlavičky souboru *(plugin automaticky generuje)*
```c
/**
 * @file strlib.h
 * 
 * Pomocná knihovna pro práci se stringy
 * 
 * @version 0.1
 * @date 2018-12-30
 */
```


## Kompilace

Součástí repozitáře je *Makefile*, který automaticky zkompiluje všechny zdrojové soubory ve složce */src*. Pokud dojde k úpravě závislostí v souborech (např. změnou `#inclde`), je **nutné** před kompilací zavolat `make dep`, aby make zkompiloval soubory ve správném pořadí (toto aktualizuje soubor *dep.list*, kde jsou závislosti uloženy). 

Součástí je i příkaz `make clean`, který vymaže přeložené soubory. 


## Debugování ve VS Code

### Ve Windows
1. Nainstalovaný Cygwin - [Stažení 64 bit verze](https://cygwin.com/setup-x86_64.exe)
	* Při instalaci v oknu *Select Packages* jsou důležité balíčky
		* gdb
		* gcc-core
		* make
2. Funkcionalitu `make` lze ověřit funkcionalitu spuštěním příkazu v termínálu VS Code (zkratka pro otevření terminálu `Ctrl + ;`)
	* Pokud kompilace proběhla bez chyb, měl by se v kořenu projektu objevit soubor *compiler.exe*
3. VS Code podporuje breakpointy které lze umístit kliknutím před číslo řádku
4. Debugování programu se spustí klávesou `F5` nebo výběrem položky debug v levém sloupci
	- Debugování lze ovládat panelem, který se objeví nahoře

Spustěním debugování se program nepřeloží, před každým debugováním je tedy potřeba program přeložit