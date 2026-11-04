# Sudoku

Juego de Sudoku desarrollado con Qt6 y Deepin Tool Kit (DTK6) para Deepin Linux.

## Caracteristicas

- Generacion automatica de puzzles con solucion unica
- 3 niveles de dificultad: Facil (55 celdas), Media (45 celdas), Dificil (35 celdas)
- Interfaz limpia y nativa de Deepin

## Dependencias

```bash
sudo apt install qt6-base-dev qt6-multimedia-dev qt6-tools-dev qt6-l10n-tools libdtk6widget-dev cmake g++
```

## Compilar

```bash
mkdir build && cd build
cmake ..
make
```

## Ejecutar

```bash
./sudoku
```

O si se instalo globalmente:

```bash
sudo make install
sudoku
```

## Controles

| Accion | Tecla |
|--------|-------|
| Ingresar numero | `1` - `9` o teclado numerico en pantalla |
| Borrar celda | `Supr`, `Retroceso`, `0` o boton *Borrar* |
| Navegar entre celdas | Click o flechas |
| Modo notas | Boton *Notas* |
| Deshacer / Rehacer | `Ctrl+Z` / `Ctrl+Shift+Z` |
| Cambiar dificultad | Selector Facil / Media / Dificil |
| Nuevo juego | Boton *Nuevo* o `Ctrl+N` |
| Salir | `Ctrl+Q` |

Cada puzzle generado tiene **solucion unica**.
