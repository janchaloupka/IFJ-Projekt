/**
 * @file common.h
 * 
 * Knihovna pro obecné věci
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma14> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#pragma once
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Zkratka pro nový řádek
 */
#define EOL '\n'

/**
 * Funguje stejně jako standartní funkce malloc, a navíc
 * pokud se nepovede alokovat paměť vypíše chybu na stderr a
 * ihned ukončí program se stavovým kódem 99
 * 
 * @param _Size Velikost místa pro alokaci
 * @return void* Ukazatel na alokované místo
 */
void *safeMalloc(size_t _Size);

/**
 * Funguje stejně jako standartní funkce realloc, a navíc
 * pokud se nepovede alokovat paměť vypíše chybu na stderr a
 * ihned ukončí program se stavovým kódem 99
 * 
 * @param _Block Ukazatel na alokované místo
 * @param _Size Velikost nového místa pro realokaci
 * @return void* Ukazatel na nově alokované místo
 */
void *safeRealloc(void *_Block, size_t _Size);

/**
 * Konvertuje řetězec ve zdrojovém kódu na řetězec interpretu
 * 
 * @param rawString Řetězcec ve zdrojovém kódu
 * @return char* Převedený kód ve stringu (je nutné uvolnit přes free()) 
 */
char *stringToInterpret(char *rawString);

/**
 * Konvertuje int ve zdrojovém kódu na int interpretu
 * Podporované soustavy: dec, bin, oct a hex
 * 
 * @param rawInt Celé číslo napsané v řetězci
 * @return char* Převedený kód ve stringu (je nutné uvolnit přes free()) 
 */
char *intToInterpret(char *rawInt);

/**
 * Konvertuje float ve zdrojovém kódu na float interpretu
 * 
 * @param rawFloat Desetinné číslo napsané v řetězci
 * @return char* Převedený kód ve stringu (je nutné uvolnit přes free()) 
 */
char *floatToInterpret(char *rawFloat);

/**
 * Vrátí záspis hodnoty true kompatibilní s interneretem
 * 
 * @return char* Převedený kód ve stringu (je nutné uvolnit přes free()) 
 */
char *trueToInterpret();

/**
 * Vrátí záspis hodnoty false kompatibilní s interneretem
 * 
 * @return char* Převedený kód ve stringu (je nutné uvolnit přes free()) 
 */
char *falseToInterpret();

/**
 * Vrátí záspis hodnoty nil kompatibilní s interneretem
 * 
 * @return char* Převedený kód ve stringu (je nutné uvolnit přes free()) 
 */
char *nilToInterpret();

/**
 * Vrátí zápis proměnné v lokálním rámci
 * 
 * @param id Identifikátor proměnné
 * @return char* Převedený kód ve stringu (je nutné uvolnit přes free()) 
 */
char *varToInterpret(char *id);

/**
 * Vrátí zápis funkce (v globálním rámci)
 * 
 * @param id Identifikátor funkce
 * @return char* Převedený kód ve stringu (je nutné uvolnit přes free()) 
 */
char *funcToInterpret(char *id);

/**
 * Vygeneruje na standartní výstup hlavičku kódu interpretu, která obsahuje základní 
 * funkce na kontrolu typů a vestavěné funkce jazyka IFJ18
 */
void generateBaseCode();
