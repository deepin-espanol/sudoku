# Sudoku

Juego de Sudoku desarrollado con Qt6 y Deepin Tool Kit (DTK6) para Deepin Linux.

## Caracteristicas

- Generacion automatica de puzzles con solucion unica
- 3 niveles de dificultad: Facil (35 celdas), Media (45 celdas), Dificil (55 celdas)
- Interfaz limpia y nativa de Deepin

## Dependencias

```bash
sudo apt install qt6-base-dev libdtk6widget-dev cmake g++
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
| Ingresar numero | `1` - `9` |
| Navegar entre celdas | Click o `Tab` |
| Cambiar dificultad | Juego > Facil / Media / Dificil |
| Nuevo juego | Juego > Nuevo juego |
