/**
 * @file main.h
 * 
 * Hlavní část překladače
 * 
 * IFJ Projekt 2018, Tým 13
 * 
 * @author <xforma04> Klára Formánková
 * @author <xlanco00> Jan Láncoš
 * @author <xsebel04> Vít Šebela
 * @author <xchalo16> Jan Chaloupka
 */

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "scanner.h"
#include "expressions.h"

/**
 * Hlavní funkce programu
 * 
 * @param argc Počet zadaných argumentů
 * @param argv Pole argumentů (první je cesta ke spuštěnému programu)
 * @return int Stavový kód programu
 */
int main(/*int argc, char const *argv[]*/);

/**
 * Funkce pro debugování člena týmu xchalo16
 * 
 * @return int Stavový kód programu
 */
int janchDebug();

/**
 * Funkce pro debugování člena týmu xlanco00
 * 
 * @return int Stavový kód programu
 */
int yellDebug();

/**
 * Funkce pro debugování člena týmu xsebel04
 * (A taky sada testů pro otestování compileru)
 * 
 * @return int Stavový kód programu
 */
int vitaDebug();
