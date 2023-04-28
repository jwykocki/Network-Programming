Jakub Wykocki, Informatyka Stosowana WFAIS UJ
______________________________________________
04/2023

Programowanie sieciowe - serwer TCP/IPv4 korzystajacy z funkcji epoll
obslugujacy wielu klientow jednoczesnie

------------------------------------------------------------------------------

Opis: 

Program serwera TCP/IPv4, ktory bedzie w stanie obsluzyc wiele klientow jednoczesnie
i bedzie umozliwial klientowi przeslanie wyrazenia arytmetycznego, a serwer - 
obliczy wartosc tego wyrazenia i przesle wynik z powrotem do klienta. Obsluge
wielu polaczen serwer realizuje za pomoca funkcji epoll, ktora sygnalizuje na
ktorych deskryptorach oczekuja na niego bajty odebrane za pomoca funkcji read().
Obliczanie wyrazen arytmetycznych wykonuje funkcja read_calc_write(), dzialajaca
na zasadzie automatu, przetwarzajaca znaki i na biezaco liczaca wynik.

------------------------------------------------------------------------------

Specyfikacja:

-> wyrazenia arytmetyczne moga skladac sie tylko z cyfr i znakow + lub -
-> kazde dzialanie zakonczone powinno byc dwuznakiem \r\n
-> w przypadku blednego wyrazenia serwer wysyla wiadomosc o tresci ERROR\r\n
-> limit znakow zapytania to domyslnie 4096 zapisane w MSG_SIZE
-> kazdy czynnik dzialania oraz wynik musi zmiescic sie w typie int16_t
-> serwer domyslnie nasluchuje na porcie 2020 (zapisany w funkcji main())

naprawiono:
-> nie ma juz wyciekow pamieci, przy uruchamianiu programu pod valgrindem
czasem wypisywane sa komunikaty o bledach, ale nie wplywaja one na poprawna
prace serwera, przynajmniej przy testach ktore do tej pory wykonywalem
-> dynamicznie alokowana tablica wskaznikow do struktur z danymi klientow
moze byc juz rozszerzana i zmniejszana, w zaleznosci jakie jest aktualne
zapelnienie numerow deskryptorow
-> drobne zmiany kosmetyczne w niektorych programach, wiekoszsc funkcjonalnosci
pozostala niezmieniona

------------------------------------------------------------------------------

Zawartosc:

tcp_serv.c - kod programu serwera
tcp_client.c - kod programu klienta
tcp_test.c - kod programu klienta-testera
test_menager.c - kod programu uruchamiajacego programy testujace
functions.c - plik z definicjami funkcji uzywanych w programach
header.h - plik naglowkowy z deklaracjami funkcji
test.txt - plik z przykladowymi zapytaniami do serwera
expected_results.txt - plik z oczekiwanymi odpowiedziami od serwera
Makefile - plik makefile

------------------------------------------------------------------------------

Obsluga:

*** kompilacja i wywolanie: ***

make tcp_s  
-  uruchamia program serwera na porcie 2020

make tcp_c  
-  uruchamia program klienta na IP 127.0.0.1 oraz porcie 2020. Wysyla do serwera
 wyrazenie zapisane w ciele funkcji main

make udp_t
- uruchamia program klienta-testera, ktory otwiera plik tekstowy test.txt
pobiera z niego zapytania, dopisuje do kazdego z nich dwuznak \r\n i wysyla do serwera,
odbiera odpowiedz i zapisuje w pliku results.txt. Pozniej wykonywana jest komenda wypisujaca dokladnie 
zawartosc results.txt (z podzialem na bajty), aby mozna bylo dokladnie przesledzic
czy zadne dodatkowe znaki nie zostaly odebrane. Na koniec komedna diff -s sprawdza
czy pliki results.txt oraz expected_results.txt sa identyczne.
Daje do mozliwosc w miare automatycznego przetestowania odpowiedzi odbieranych
od serwera. Nie testuje on szczegolowo lacznosci pomiedzy serwerem a klientem.

make tests n=?
- uruchamia program test_manager ktory funkcja exec uruchamia ? programow tcp_test.c
Kazdy z nich otwiera plik tekstowy z zapytaniami, i wysyla zapytania do serwera. Pomiedzy
wysylaniem wyrazen czeka randomowa ilosc sekund sleep(). Wyniki zapisuje do pliku tekstowego
?results.txt, w zaleznosci ktorym w kolei jest programem. Na koniec kazdy ze stworzonych wynikowych
plikow tekstowych porownywany jest z plikiem expected_results.txt
W pewien sposób symuluje prace serwera z wieloma klientami

make valg
- uruchamia program serwera pod valgrindem z wlaczona opcja wyszukiwania wyciekow
pamieci. Przydatna komenda przy naprawianiu tego problemu

opcjonalnie: laczyc sie z serwerem mozna poprzez narzedza ncat lub socat

*** czyszczenie ***
make clean
- usuwa pliki obiektowe i wykonywalne

*** kompresowanie ***
make tar
- kompresuje folder do .tar.gz

------------------------------------------------------------------------------
