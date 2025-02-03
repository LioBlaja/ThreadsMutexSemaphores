# ThreadsMutexSemaphores

# Probleme cu thread, mutex, semafoare

## Cerințe pentru finalizare

1. Să se scrie un program C ce se va apela în linie de comandă astfel:
 ./prog <output_file> ..


Programul va parcurge recursiv directorul dat ca și argument.
Pentru fiecare fișier obișnuit (regular) găsit se va crea un thread ce va număra de câte ori caracterele date ca și argumente apar în conținutul acestuia.
Dacă numărul total de caractere numărate pentru fiecare fișier depășește numărul `<threshold>`, programul va crea o legătură simbolică în același director cu fișierul procesat, cu același nume, dar cu terminația "_th".
Se va crea de asemenea și un fisier de statistică referit prin `<output_file>`. Fișierul va avea o formă tabelară, iar capul de tabel va fi generat dinamic în funcție de argumente, astfel:
forma generala:

<file_path>;<nr_ch1>;<nr_ch2>;...;<nr_chn>

Pentru un caz de test, se poate descărca o arhivă rulând în terminal:

wget https://staff.cs.upt.ro/~valy/so/test_so_3_1.tar.gz

Pentru dezarhivare se poate folosi următoarea comandă:

tar xf test_so_3_1.tar.gz

2. Să se scrie un program care primește ca și argument un caracter și citește câte o linie de la intrarea standard. Pentru fiecare linie citită programul va crea un thread care va număra de câte ori caracterul dat ca și argument se regăsește în linia citită și va printa acest număr la ieșirea standard.

3. Același enunț ca și problema 2 dar programul va afișa în plus și numărul total de apariții al caracterului în toate liniile citite.

4. Să se scrie un program care primește ca și argumente două numere ce reprezintă capetele unui interval A și B și un alt treilea număr N ce va reprezenta un număr de thread-uri pe care programul le va crea. Programul va împărți intervalul [A, B] în N intervale egale. Apoi programul va crea N threaduri. Fiecare thread va procesa câte un subinterval din intervalul [A,B] și va identifica numere prime din subinterval. În momentul în care un thread va identifica câte un număr prim îl va printa la ieșirea standard.

5. Același enunț ca și la problema 4 cu modificarea că în momentul în care un thread va identifica un număr prim din subinterval acesta îl va adăuga într-un tablou comun tuturor thread-urilor. Când toate thread-urile au terminat calculul programul principal va afișa la sfârșit tabloul cu toate numerele prime găsite.

6. Să se scrie un program care primește ca și argument un director și o cale către un fișier de ieșire. Programul va scana recursiv directorul dat ca și argument și pentru fiecare fișier regular găsit va crea un thread care va realiza o histogramă a caracterelor întâlnite. Toate thread-urile vor realiza histograma într-o zonă de memorie comună (un tablou de histogramă comun). La sfârșit, programul va scrie histograma finală în fișierul identificat prin calea dată ca și al doilea argument al programului.

Pentru un caz de test, se poate descărca o arhivă rulând în terminal:
wget https://staff.cs.upt.ro/~valy/so/test_so_3_1.tar.gz


Pentru dezarhivare se poate folosi următoarea comandă:
tar xf test_so_3_1.tar.gz


7. Să se scrie un program care primește ca și argument un fișier text și un număr N reprezentat un număr de thread-uri. Programul va împărți fișierul în N partiții egale și pentru fiecare partiție va lansa un thread care va realiza histograma caracterelor din fișier. Thread-urile vor completa rezultatul într-un tablou comun de histogramă.

8. Să se scrie un program care primește ca și argument un fișier text și un număr N reprezentând un număr de threaduri. Programul va citi din fișier câte un buffer de dimensiunea CHUNK (configurabilă printr-un define) și îl va da spre procesare câte unui thread. Dacă nu mai sunt thread-uri disponibile din cele N programul va aștepta până când există vreun thread disponibil pentru a prelucra următorul buffer disponibil. Toate thread-urile vor completa rezultatul într-un tablou comun de histogramă. Este necesar ca programul să țină evidența stării de execuție și de join a fiecărui thread. Numărul N nu va fi ales ca și la problema 7 astfel încât fișierul să fie împărțit în partiții egale. Se va considera că există un pool de N thread-uri care se va ocupa de procesare. Dacă nu există thread-uri disponibile din pool (toate sunt ocupate cu procesarea unui buffer) atunci programul va aștepta eliberarea unui thread. Se poate utiliza o funcție care să obțină care thread este liber din pool-ul de thread-uri. Se va ține evidența stării de join și se va face join pe fiecare thread care își termină execuția.

9. Același enunț ca și la problema 8 dar se va implementa folosind thread-uri DETACHED.

10. Același enunț ca și problema 8 și 9 dar operațiunea de verificare a unui thread liber/ocupat (în execuție sau nu) se va realiza folosind semafoare.

