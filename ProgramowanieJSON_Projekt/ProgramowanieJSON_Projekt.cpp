#define ARDUINO_JSON_ENABLE_STD_STRING
#include "ArduinoJson-v6.21.2.h"
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <Windows.h>


using namespace std;

const int WIELKOSC_PLANSZY = 10; // WIEM MIALO NIE BYC GLOBALNYCH 
                                // ALE TUTAJ NIECH MI PAN WYBACZY PROSZE
void Menu();
void zapisKonfiguracji(StaticJsonDocument<1024>& doc);
void nowyConfig();
void inicjacjaPlanszy(char plansza[][WIELKOSC_PLANSZY], int& foodX, int& foodY);
void rysowaniePlanszy(const char plansza[][WIELKOSC_PLANSZY], int X, int Y);
void inicjacjaConfigu(StaticJsonDocument<1024>& doc);
void zapiszWynik(const string& nickname, int punkty);


int main()
{
    srand(time(NULL));
    system("chcp 1250"); // do uzywania polskich znakow
    system("CLS"); // czysci konsole

    StaticJsonDocument<1024> doc;
    inicjacjaConfigu(doc);

    char wyborGracza;
    while (true)
    {
        Menu();
        cin >> wyborGracza;
        switch (wyborGracza)
        {
        case '1':
        {
            string nickname;
            int x = 0, y = 0;
            int foodX, foodY;
            int punkty = 0;

            char plansza[WIELKOSC_PLANSZY][WIELKOSC_PLANSZY];
            char input;
            bool gameover = false;

            cout << "Witaj w grze! Podaj swój nick: ";
            cin >> nickname;

            inicjacjaPlanszy(plansza, foodX, foodY);

            int czasPoczatkowy = GetTickCount();
            int czasKoncowy = czasPoczatkowy + 60000; // licznik do 1 minuty

            while (!gameover && GetTickCount() < czasKoncowy) // warunek zakonczenia gry
            {

                rysowaniePlanszy(plansza, x, y);

                cout << "Gracz: " << nickname << "\n";
                cout << "Punkty: " << punkty << "\n";
                cout << "\nGRA TRWA MINUTE. ZBIERZ JAK NAJWIECEJ PUNKTOW!";

                input = _getch(); // oczekuje wcisniecia klawisza

                switch (input) {
                case 'W':
                case 'w':
                    if (y > 0) {
                        y--;
                    }
                    break;
                case 'S':
                case 's':
                    if (y < 9) {
                        y++;
                    }
                    break;
                case 'A':
                case 'a':
                    if (x > 0) {
                        x--;
                    }
                    break;
                case 'D':
                case 'd':
                    if (x < 9) {
                        x++;
                    }
                    break;
                default:
                    break;
                }


                if (x == foodX && y == foodY) {
                    punkty++;
                    cout << "Zdobyłeś punkt!\n";

                    plansza[foodY][foodX] = ' '; // po zdobyciu jedzenia zamien na puste pole

                    foodX = rand() % 10;
                    foodY = rand() % 10;

                    plansza[foodY][foodX] = 'x'; // wypelnij nowe pole jedzeniem
                }

                cout << endl;
            }

            cout << "\nKONIEC GRY! \nGracz " << nickname << " zdobył " << punkty << " punktów!\n";
            zapiszWynik(nickname, punkty);
            Sleep(3000); // przeczekanie az przeczyta sie podsumowanie gry
            system("cls"); // wyczysc konsole
            break;
        }

        case '2':
        {
            system("CLS");
            cout << "#---# LISTA WYNIKÓW #---# \n";

            ifstream plik("wyniki.json");
            if (plik.good())
            {
                string zawartoscPliku;
                getline(plik, zawartoscPliku);

                StaticJsonDocument<1024> doc;
                deserializeJson(doc, zawartoscPliku);

                JsonArray wyniki = doc.as<JsonArray>();

                // pokazuje wyniki
                for (JsonVariant wynik : wyniki)
                {
                    cout << "Nickname: " << wynik["nickname"].as<string>() << ", Punkty: " << wynik["punkty"].as<int>() << "\n";
                }

                plik.close();
            }
            else
            {
                cout << "Brak wynikow.\n";
            }

            cout << "Naciśnij dowolny przycisk, aby wrócić do menu.\n\n";
            _getch(); // oczekuje wcisniecia klawisza
            system("CLS");
            break;
        }

        case '3':
        {
            cout << "#---# USTAWIENIA #---#";

            while (true)
            {
                system("CLS");
                cout << "Opcje\n";

                int i = 1;
                for (JsonPair p : doc.as<JsonObject>()) // wyswietlenie opcji
                {
                    cout << i << ". " << p.key() << " " << p.value() << "\n";
                    i++;
                }
                cout << "\nNaciśnij 0, aby wrócić.\n";

                int wyborOpcji;
                cout << "\nWybierz opcje, którą chcesz zmienić:\n";
                cin >> wyborOpcji;
                if (wyborOpcji == 0) // warunek wyjscia z opcji
                {
                    system("CLS");
                    break;
                }
                i = 1;

                for (JsonPair d : doc.as<JsonObject>())
                {
                    if (i == wyborOpcji) // wybor ponumerowanej opcji 
                    {
                        cout << "Wprowadź nową wartość dla " << d.key() << ": ";
                        string nowaWartosc;
                        cin >> nowaWartosc;
                        doc[d.key()] = nowaWartosc; // przypisanie nowej wartosci dla wybranej opcji

                        break;
                    }
                    i++;
                }
            }


            break;
        }

        case '4':
        {
            cout << "WYJŚCIE Z GRY. DO ZOBACZENIA PONOWNIE! \n";
            zapisKonfiguracji(doc); // zapisuje wproawdzone zmiany
            return 0;
        }

        default: // jak ktos wybierze inna cyfre niz z zakresu 1-4
            cout << "\nKolego! Nie ma przecież takiej opcji w menu!\n";
            break;
        }
    }

    return 0;
}

void Menu()
{
    cout << "1. Nowa gra\n";
    cout << "2. Wyniki\n";
    cout << "3. Opcje\n";
    cout << "4. Wyjście\n";
}


void zapisKonfiguracji(StaticJsonDocument<1024>& doc)
{

    ofstream plik("config.json");
    if (plik.good())
    {
        string tmp;
        serializeJson(doc, tmp);
        plik << tmp;
        plik.close();
    }

}

void nowyConfig()
{
    // gdyby sie okazalo ze nie ma pliku to tworzy nowy
    ofstream plik("config.json");
    if (plik.good())
    {
        StaticJsonDocument<1024> doc;
        doc["Resolution"] = "1920x1080";
        doc["FullScreen"] = true;
        doc["Difficulty"] = "Hard";
        doc["GameMode"] = 3;
        doc["EasyPlaceMode"] = false;
        doc["Brightness"] = 75;
        doc["Show hints"] = false;

        string ustawienia;
        serializeJson(doc, ustawienia);
        plik << ustawienia;
        plik.close();
    }

}


void inicjacjaPlanszy(char plansza[][WIELKOSC_PLANSZY], int& foodX, int& foodY)
{
    for (int i = 0; i < WIELKOSC_PLANSZY; i++) {
        for (int j = 0; j < WIELKOSC_PLANSZY; j++) {
            plansza[i][j] = ' ';
        }
    }

    // generowanie miejsca jedzenia
    foodX = rand() % WIELKOSC_PLANSZY - 1;
    foodY = rand() % WIELKOSC_PLANSZY - 1;

    // lokalizacja graficzna jedzenia
    plansza[foodY][foodX] = 'x';

}


void rysowaniePlanszy(const char plansza[][WIELKOSC_PLANSZY], int X, int Y)
{
    system("cls");
    for (int i = 0; i < WIELKOSC_PLANSZY; i++) {
        for (int j = 0; j < WIELKOSC_PLANSZY; j++) {
            if (i == Y && j == X) {
                cout << "P ";
            }
            else {
                cout << plansza[i][j] << " ";
            }
        }
        cout << endl;
    }
}


void inicjacjaConfigu(StaticJsonDocument<1024>& doc)
{

    fstream plik;
    plik.open("config.json", ios::in);
    if (!plik.good())
    {
        nowyConfig();
    }
    else
    {
        string zawartoscPliku;
        getline(plik, zawartoscPliku);

        deserializeJson(doc, zawartoscPliku);
    }

}

void zapiszWynik(const string& nickname, int punkty)
{
    StaticJsonDocument<1024> doc;

    //inny sposob odczytu danych, tak jak w liceum uczyli
    ifstream plik("wyniki.json");
    if (plik.good())
    {
        string zawartoscPliku;
        getline(plik, zawartoscPliku);
        deserializeJson(doc, zawartoscPliku);
        plik.close();
    }
    else
    {
        cout << "Wystąpił błąd. Nie udało się otworzyć pliku! \n";
    }

    // sprawdzenie wyniku tego samego gracza
    bool dodajWynik = true;
    for (JsonVariant wynik : doc.as<JsonArray>())
    {      // sprawdza czy wynik jest wiekszy niz poprzedni
        if (wynik["nickname"] == nickname && wynik["punkty"] >= punkty)
        {
            dodajWynik = false;
            break;
        }
    }

    // dodanie wyniku
    if (dodajWynik)
    {
        JsonObject nowyWynik = doc.createNestedObject(); // tworzy nowy obiekt w pliku
        nowyWynik["nickname"] = nickname;
        nowyWynik["punkty"] = punkty;
    }

    // zapisywanie zawartosci pliku
    ofstream zapiszPlik("wyniki.json");
    if (zapiszPlik.good())
    {
        string zawartoscPliku;
        serializeJson(doc, zawartoscPliku);
        zapiszPlik << zawartoscPliku;
        zapiszPlik.close();
    }
}

