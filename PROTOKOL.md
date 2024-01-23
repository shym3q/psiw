# Protokół

## Składowe aplikacji: serwer, klient, pośrednik komunikatów (magistrala)

1. **Serwer:**
   - Serwer oczekuje na połączenie (ping) od klienta, czyli na prośbę o obsłużenie klienta (prośba o rejestrację/zalogowanie, subskrypcja tematu, wysyłanie wiadomości do klientów, którzy zasubskrybowali).
   - Serwer w danym momencie może obsłużyć tylko jednego klienta.

2. **Zabezpieczenie przed utratą połączenia:**
   - W sytuacji utraty połączenia z danym klientem, serwer przerywa to połączenie dzięki zaimplementowanemu mechanizmowi zabezpieczenia.
   - Serwer ma ustawiony alarm, aby zbyt długie połączenie zostało przerwane i możliwe było łączenie się z innymi klientami.

3. **Klient:**
   - Klient może skorzystać z następujących opcji:
     1. **Rejestracja:**
        - Serwer wysyła klientowi liczbę obecnie zalogowanych klientów, na podstawie której generowany jest numer będący jednocześnie id klienta i adresem kolejki przypisanej do tego klienta.
        - Na tę kolejkę klient będzie otrzymywać wiadomości na zasubskrybowany przez siebie temat.

     2. **Subskrypcja tematu:**
        - Klient podaje temat, który chciałby otrzymywać wiadomości, a ta informacja jest przekazywana do serwera.
        - Serwer ma połączenie z bazą danych i jeśli podany temat (który posiada swój adres) nie istnieje w bazie, to jest tworzony, a klient jest do niego przypisywany.
        - Jeśli temat już istnieje, klient jest dołączany do pozostałych klientów, którzy ten temat już zasubskrybowali.

     3. **Odbieranie wiadomości:**
        - Serwer odbiera wiadomości wysyłane przez klientów, a następnie rozsyła je do klientów, którzy zasubskrybowali ten sam temat.

4. **Baza danych serwera:**
   - Serwer posiada bazę danych, która działa w oparciu o zmodyfikowaną strukturę danych typu `linked list`.
   - Jest to lista dwuwymiarowa / zagnieżdżona: składa się z tematów w postaci węzłów, na których są zapisane są listy subskrybentów danego tematu.
   - W chwili odbioru wiadomości serwer odczytuje nadawcę i jego zasubskrybowany temat, odszukuje odpowiedni wpis w bazie opatrzony tym tematem i rozsyła wiadomość do subskrybentów tego odszukanego tematu.

5. **Proces dołączania użytkownika:**
   - Kiedy użytkownik chce dołączyć do serwera, program klienta zczytuje z klawiatury wprowadzone przez niego dane - nazwę użytkownika i temat, który chce subskrybować.
   - Następnie program klienta wykorzystuje te dane, aby nawiązać połączenie z serwerem.
   - Po zarejestrowaniu w serwerze program klient zczytuje wiadomości wprowadzane przez użytkownika, aby przekazać je innym subskrybentom za pośrednictwem serwera.
