# Project 1 - Monte Carlo Tree Search Algorithmus
Für die englische Version dieser README, siehe [README.md](README.md) Datei.
## Inhaltsverzeichnis

## Installation
### Anforderungen
Sie müssen mindestens `cmake 3.22.1` installiert haben.
Da das Projekt cmake verwendet, können Sie es auf jeder Plattform erstellen, die cmake unterstützt.
Das Projekt wurde bisher nur auf Linux-basierten Systemen (Ubuntu 24.04 und 24.10) getestet. <br>
Die folgenden Schritte sind für Linux-basierte Systeme. Wenn Sie kein Linux-basiertes System verwenden, müssen Sie die Schritte entsprechend anpassen.
### Installation auf Linux
```bash
git clone https://github.com/RishabhBoop/info2_projects.git # clone the repo
cd info2_projects/MCTS_checkers # change to the project directory
mkdir build # create a build directory
cmake -S . -B build # run cmake to generate the build files
cmake --build build # build the project
```
The executable will be created in the `/bin` directory.
```bash
# ----- for release build -----
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
./build/checkers_exec
# ----- for debug build -----
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
./build/checkers_exec
# ----- for testing -----
cd build && ctest -C build --output-on-failure
```
## Was ist dieses Projekt?
Dieses Projekt implementiert einen Monte Carlo Tree Search (MCTS) Algorithmus, um das Spiel Dame zu spielen. Es ist das erste Projekt von Informatik 2 im SS25 an der HKA mit Prof. Hanuschkin.
## MCTS Algorithmus
Wenn wir den Computer dazu bringen wollen, rollenbasierte Spiele mit einem definitiven Endzustand (z.B. TicTacToe, Dame, Schach, Go) zu spielen, könnten wir naiv jeden möglichen Zustand des Spiels in einem Baum speichern und dann den Baum durchlaufen, um den besten Zug zu finden. Dies ist jedoch unmöglich für Spiele mit einer großen Anzahl möglicher Zustände (hoher Branching factor) (z.B. Schach, Go). Daher verwenden wir den Monte Carlo Tree Search (MCTS) Algorithmus, um den besten Zug zu finden. <br>
Die Grundidee ist, dass wie eine große Anzahl von Spielen (~1000-1000000) simulieren und schauen welcher Pfad uns zum Erfolg führt. Man muss somit jede einzelne Zustand nicht bewerten, sondern nur die Pfade, die zu einem Gewinn führen. <br>
Es wurde unter anderem in Alphago verwendet, um den Weltmeister Lee Sedol zu besiegen. <br>

### Die vier Schritte des MCTS Algorithmus
1. **Selection**: Durchlaufe den Baum und entscheide dich immer für den Knoten mit dem höchsten "rating". Mache das bis du zum einem Blattknoten kommst.
2. **Expansion**: Erzeuge einen zufällig ausgewählten Kindknoten und initialisiere diesen
3. **Simulation**: Simuliere ein Spiel von diesem Knoten aus, indem du zufällige Züge machst, bis das Spiel zu Ende ist. Die so erstellten Züge werden nicht in dem Baum gespeichert.
4. **Backpropagation**: Durchlaufe jeden Knoten und aktualisiere den Wert des Knotens (gewonnene Spiele / gespielte Spiele).
### Selektionsformel
Die Selektion basiert auf die "Upper confidence bounds for trees" (UCT) Formel:
$$
UCB = v_k + C \cdot \sqrt{\frac{ln(n_p)}{n_k}}
$$
wobei:
- $v_k$ Anteil der gewonnenen Spiele am Knoten $k$
- $n_p$ Anzahl der gespielten Spiele am Elternknoten
- $n_k$ Anzahl der gespielten Spiele am Knoten $k$
- $C$ ist ein Konstante, die den Einfluss der Exploration und Exploitation steuert