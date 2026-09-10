# Fluid Simulation (SDL3)

Dwuwymiarowa symulacja fizyki cieczy w czasie rzeczywistym napisana w języku C z wykorzystaniem biblioteki **SDL3**. Symulacja obejmuje grawitację, spływ ukośny, poziomowanie tafli, interpolację pędzla oraz hydrostatyczne cieniowanie głębokości (w tym pod kopułami i przeszkodami).

---

## Sterowanie

| Klawisz / Mysz | Akcja |
| :--- | :--- |
| **LPM** (Lewy Przycisk) | Rysowanie wybranym narzędziem |
| **PPM** (Prawy Przycisk) | Szybka gumka (usuwanie wody i ścian) |
| **SPACJA** | Przełączanie narzędzia (`WODA` $\leftrightarrow$ `ŚCIANA`) |
| **X** | Włączenie trybu gumki |
| **Kółko myszy** | Zmiana promienia pędzla |
| **ESC** | Wyjście z programu |

---

## Wymagania

* **CMake** $\ge$ 3.20
* **Kompilator C** wspierający standard C11 (MinGW-w64, GCC, Clang lub MSVC)
* **Git** oraz połączenie z internetem (biblioteka SDL3 jest pobierana i budowana automatycznie przez `FetchContent`)

---

## Struktura projektu

```text
MyProject/
├── CMakeLists.txt
└── src/
    └── main.c
```

## Konfiguracja
W terminalu w katalogu głównym projektu:

Windows (MinGW / GCC):
```Bash
cmake -B build -G "MinGW Makefiles"
```

Windows (Visual Studio) / Linux / macOS:
```Bash
cmake -B build
```

## Budowanie
```Bash
cmake --build build --config Release
```

## Uruchomienie
Plik wykonywalny znajduje się w
```
./build/(Release/)MyProject.exe
