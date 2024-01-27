# Autor: Jakub Jaśków - 268416
# Instrukcja obsługi kompilatora
## Kompilacja
### Niezbędne programy
Kompilacja programu testowana była pod poniżej wymienionymi wersjami programów. Zaleca się stosowanie tych samych wersji oprogramowania.
+ Bison -       3.8.2
+ Flex -        2.6.4
+ g++ -         11.4.0
+ GNU Make -    4.3

### Kompilacja programu
Aby skompilować program należy umieścić wszystkie pliki wymienione w sekcji **[Pliki](#pliki)** w jednym folderze a następnie wywołać następujące komendy:

>$ make

Po kompilacji otrzymamy plik binarny o nazwie **compiler**.

### Użytkowanie programu

>$ ./compiler [OPCJA] [PLIK_WEJŚCIOWY] [PLIK_WYJŚCIOWY]

##### Opcje:

    -v      tryb verbose - drukuje wszystkie logi na standardowe wyjście.

**UWAGA!**
Czasami otrzymany przez nas plik może nie chcieć się wykonać. Może to być spowodowane brakiem zezwolenia na wykonanie od strony systemu operacyjnego. Należy wtedy nadać plikowi binarnemu (**compiler**) odpowiednie prawa wykonawcze. **LINUX**: 

>$ chmod a+x [nazwa_pliku]

## Pliki
+ **grammar.y**, **lexer.lex** - pliki definiujące analizatora leksykalnego i parsera. Zawierają: gramatykę, słowa kluczowe oraz funkcje z innych plików.

+ **grammar.cpp**, **grammar.hpp**, **lexer.cpp** - prekompilowane wersje powyższych plików wygenerowanie przez parser i lekser.

+ **CodeBlock.hpp**, **AST.hpp**, **AST.cpp** - nagłowki i źródła elementów programu. **CodeBlock.hpp** zawiera definicje code blocku oraz elementów w nim używanych. **AST.hpp** i **AST.cpp** - definiują struktury i klasy używane podczas konstrukcji drzewa AST oraz tłumaczenia elementów drzewa na kod maszyny wirtualnej (pesudo assembler).

+ **handlers.hpp**, **handlers.cpp** - definicje i deklaracje funkcji obłsługujących tworzenie elementów drzewa AST oraz sprawdzania poprawnej semantyki programów (sprawdzanie inicjalizacji zmiennych itp.).

+ **definitions.hpp** - deklaracje używane przez wiele plików oraz makra

+ **types.hpp** - łączona definicja CodeBlocku oraz AST 

+ **main.cpp** - definicja i deklaracja funkcji main, error, warning oraz kolejności wykonywania części programu
