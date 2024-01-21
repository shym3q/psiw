# psiw
## This is a project for the university.
- **Course**: System and concurrent programming
- **Topic**: Chat application using IPC mechanisms based on server/client structure
## TODO:
- [x] Connecting clients to the server.
- [x] Partial implementation of registering a topic.
- [ ] Splitting messages into separate channels.
- [x] The client messing up a request does not block the server.
- [x] The server handling concurrent requests.
- [ ] GUI of the application defined.
- [ ] A bus / a message broker implemented.
- [ ] Closing the client with the server being informed.


## Content description
**utils.h**
- dzięki temu pliku zapewnia się, że jest on dołączany tylko raz w jednym pliku źródłowym
- funkcje pomocnicze: ochrona przed wielokrotnym dołączaniem, czyli powtórnym zdefiniowaniu tych samych struktur, funkcji lub zmiennych, zapobiega błędom kompilacji 
- funkcja panic: obsługa błędów, wypisanie komunikatu błędu
- funkcja handle_timeout: obsługa przerwań czasowych
- #ifndef: sprawdza, czy utils_h nie jest jeszcze zdefiniowana 


**types.h**
- msg: składa się z identyfikatora (id) i tekstu o max długości 1024 znaków (text)
- prot: składa się z liczby klientów (cn), identyfikatora klienta (cid) i wskaźnika do nazwy (*name)
- msg_type: definiuje różne typy wiadomości, które można przesłać między procesami, każdyu ma swoją unikalną wartość
- t_msgbuf: reprezentuje bufor wiadomości, type określa typ wiadomości, cmsg zawiera treść wiadomości
- pingbuf: reprezentuje bufor do wysyłania pustej wiadomości bez treści
- dec_msgbuf: reprezentuje bufor wiadomości, określa typ wiadomości (type) i przechowuje wartości liczbową (i)
- i_msgbuf: reprezentuje bufor wiadomości, określa type (type) i reprezentuje wewnętrzną wiadomość (imsg)


**handlers.h**
- funkcje obsługujące różne rodzaje żądań: 
- send_clients_number: wysyłanie liczby klientów, 
- await_client_credentials: oczekiwanie na poświadczenie klienta, 
- await_client_topic: oczekiwanie na temat subskrypcji klienta, 
- await_client_msg: oczekiwanie na wiadomość klienta, 
- handle_request: główna funkcja obsługująca żądania, w zależności od otrzymanego komunikatu podejmuje różne działania: rejestracja klienta, subskrypcja tematu, odbiór wiadomości od klienta


**object.h**
- struct server: obsługa serwera
- msgid: identyfikator kolejki komunikatów dla zwykłych wiadomości od klientów
- imsgid: identyfikator kolejki komunikatów dla wewnętrznych wiadomości od serwera do procesów potomnych
- cn: liczba klientów
- funkcja new_server: funkcja pomocnicza, która tworzy i inincjalizuje nowy obiekt serwera: alokuje pamięć dla struktury server, inicjalizuje pola struktury server i otwiera dwie kolejki komunikatów za pomocą funkcji msgget, zwraca wskaźnik do utworzonej struktury serwera


**bus.h**
- struct node:
- reprezentuje węzeł w bazie klientów subskrybujących wiadomości
- cid: identyfikator klienta
- subs: wskaźnik na innych klientów o takiej samej subskrypcji
- next: wskaźnik na następny węzeł w liście
- new_db: alokuje pamięć do nowego węzła będącego bazą danych, zwraca wskaźnik do utworzonego węzła
- add_subscriber: służy do dodawania nowego subskrybenta do bazy danych
- distribute: przekierowuje przechwycone wiadomości do subskrybentów o tej samej subskrypcji


**server.c**
- zmienne globalne:
struct server *s: reprezentująca serwer, 
struct node *db: reprezentująca bazę danych
- funkcja server_exit: obsługuje sygnał SIGINT, odpowiada za zwolnienie kolejek komunikatów i pamięci oraz zakończenie programu\
- funkcja main:
> - tworzy i inicjalizuje strukturę server za pomocą funkcji new_sever() w pliku object.h
> - tworzy bazę danych za pomocą funkcji new_db zdefiniowanej w pliku bus.h
> - sprawdza, czy udało się połączyć z kolejkami komunikatów, w przypadku niepowodzenia programn kończy działanie
> - tworzy nowy proces potomny za pomocą fork(): proces potomny obsługuje konkretne żądanie klienta za pomocą funkcji handle_request, proces rodzica czeka na zakończenie procesu potomnego, odbiera od niego protokół i wypisuje na ekran
- gdy program otrzyma sygnał SIGINT, czyści kolejki komunikatów, zwalnia pamięć i kończy działanie

**client.c**
- identyfikatory kolejek komunikatów
smsgid: serwera, 
cmsgid: klienta
- cid: identyfikator klienta
- uname: nazwa klienta, topic: temat subskrypcji, cmsg: wiadomość klienta
- funkcja main:
> - get_user_data: pobiera od użytkownika nazwę i temat subskrypcji
> - establish_connection: nazwiązuje połączenie z serwerem wysyłając żądanie rejestracji
> - send_client_credentials: wysyła do serwera dane klienta 
> - subscribe: żądanie subskrypcji na dany temat
> - wprowadzanie i wysyłanie wiadomości do serwera
> - establish_connection: próba uzyskania dostępu do kolejki komunikatów serwer, wysłanie do serwera pinga
> w reakcji na SIGINT kończy działanie i zwalnia kolejkę komunikatów klienta