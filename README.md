# automaton
For an Object-Oriented-Programming class.
Compiles on both windows and unix systems.
Supply between 0 and 2 cmd-line arguments ((name) nothing || (name) [size] || (name) [width] [height]).

# evolution
## v2.1
now follows the rules given [here](https://defelice.up8.site/2024-2025-fichiers-poo-GAV/projet-eco.pdf) instead of being a GoL placeholder
run with 6 additional cmd line args (width height sheepNum wolfNum steps delayMs) or 0. if 0 are supplied, default values are used instead.
all used cell-types (Sheep, Wolf, Empty, Grass, Mineral) inherit from a generic Cell type. cells are dispersed throughout the grid randomly.
uses namespace std for shorter lines of code
in all tests I've done of this, the universe has died way too quickly meaning there's a bug somewhere I still need to find and fix.

## v1
nothing special here, literally just a Game of Life implementation.
dead and alive cells both inherit from a generic cell-type parent. importantly, cells do not know their position, only the automaton-instance does.
will add more rules later.
