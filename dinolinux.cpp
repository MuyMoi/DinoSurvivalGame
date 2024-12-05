#include <ncurses.h> //para cambiar coordenadas de la consola, equivalente a windows.h
#include <fstream> //manejo de archivos
#include <list> //listas
#include <thread> //hilos
#include <mutex>
#include <cstdlib>
#include <ctime>
#include <chrono> //para pausar el tiempo
using namespace std;

#define COORDPISO 30
#define Sleep(milisegundos) this_thread::sleep_for(milisegundos)

mutex mtx;
bool gameover = false;

int obtenerAlto() {
  int alto, ancho;
  getmaxyx(stdscr,alto,ancho);
  return alto;
}

int obtenerAncho() {
  int alto, ancho;
  getmaxyx(stdscr,alto,ancho);
  return ancho;
}

//inicializar el modo color y definir los colores
//que se utilizaran, asi como los pares texto-fondo
void iniciarColores()
{
  start_color();
  init_color(COLOR_BLACK, 0,0,0); //inicializar el rgb del negro
  init_pair(1, COLOR_BLACK, COLOR_WHITE); //negro sobre blanco
  init_pair(2, COLOR_WHITE, COLOR_BLACK); //blanco sobre negro
  init_pair(3, COLOR_WHITE, COLOR_RED); //blanco sobre rojo
  init_pair(4, COLOR_WHITE, COLOR_BLUE); //blanco sobre azul
}

void pintarEscenario() {
  bkgd(COLOR_PAIR(1)); // Fondo blanco, texto negro
  move(COORDPISO, 0);
  for (int i = 0; i < obtenerAncho(); i++) {
    addch(ACS_HLINE); // Este es el suelo
  }
}

void pantallaIntroduccion() {
  // Cambiar el color a blanco sobre azul
  bkgd(COLOR_PAIR(4));

  // Obtener las dimensiones actuales de la consola
  int ancho = obtenerAncho();
  int alto = obtenerAlto();
  int mitadAlto = alto/2;
  int mitadAncho = ancho/2;

  // Dibujar un marco que cubra toda la consola;
  box(stdscr, 0, 0);

  // Texto de bienvenida
  move(mitadAlto - 4, mitadAncho - 15);
  printw("BIENVENIDO AL JUEGO DINO SURVIVAL");

  move(mitadAlto - 2, mitadAncho - 15);
  printw("   CREADO POR APRIL Y MOISES");

  move(mitadAlto, mitadAncho - 15);
  printw("ASEGURATE DE MAXIMIZAR LA VENTANA");

  move(mitadAlto + 2, mitadAncho - 15);
  printw(" Y PRESIONA ENTER PARA EMPEZAR");


  // Dibujo del dinosaurio grande en la parte inferior
  move(alto - 8, mitadAncho - 10);
  printw("               __");
  move(alto - 7, mitadAncho - 10);
  printw("              / _)");

  move(alto - 6, mitadAncho - 10);
  printw("     _.----._/ / ");

  move(alto - 5, mitadAncho - 10);
  printw("    /         /  ");

  move(alto - 4, mitadAncho - 10);
  printw(" __/ (  |    (   ");

  move(alto - 3, mitadAncho - 10);
  printw("/__.-'|_|--'_|   ");

  refresh();

  // Esperar a que el usuario presione ENTER
  do {
    Sleep(1000ms);
  } while( getch() != 10);
}


class Dinosaurio {
private:
  int x, y;
public:
  Dinosaurio(): x(15), y(COORDPISO-10) {}
  void pintar();
  void borrar();
  void moverPata1(); // Animar las patas del dinosaurio
  void moverPata2();
  void saltar();
  int X() { return x; }
  int Y() { return y; }
};



void Dinosaurio::pintar() {
  mtx.lock();
  mvprintw(y,x,   "     %c%c%c%c%c%c%c", 219,219,219,219,219,219,220);
  mvprintw(y+1,x, "     %c%c%c%c%c%c%c", 219,220,219,219,219,219,219);
  mvprintw(y+2,x, "     %c%c%c%c%c%c%c", 219,219,219,219,220,220,220);
  mvprintw(y+3,x, "     %c%c%c%c   ", 219,219,219,220);
  mvprintw(y+4,x, "%c   %c%c%c%c%c%c%c ", 219,219,219,219,219,219,223,219);
  mvprintw(y+5,x, "%c%c %c%c%c%c%c%c   ", 219,219,219,219,219,219,219,219);
  mvprintw(y+6,x, "%c%c%c%c%c%c%c%c%c   ", 219,219,219,219,219,219,219,219,219);
  mvprintw(y+7,x, " %c%c%c%c%c%c%c%c   ", 219,219,219,219,219,219,219,219);
  mvprintw(y+8,x, "  %c%c%c%c%c%c    ", 219,219,219,219,219,223);
  mvprintw(y+9,x, "   %c  %c     ", 219,219);
  refresh();
  mtx.unlock();
}

void Dinosaurio::borrar() {
  mtx.lock();
  mvprintw( y,  x, "            ");
  mvprintw( y+9,x, "            ");
  refresh();
  mtx.unlock();
}

void Dinosaurio::moverPata1() {
  mtx.lock();
  mvprintw( y+9,x,"   %c  %c", 223,219);
  refresh();
  mtx.unlock();
}

void Dinosaurio::moverPata2() {
  mtx.lock();
  mvprintw( y+9, x, "   %c  %c", 219,223);
  refresh();
  mtx.unlock();
}

void Dinosaurio::saltar(int cuanto) {
  for (int i = 0; i < cuanto && !gameover; i++) { // Subir
    borrar(); y--; pintar(); Sleep(60ms);
  }
  for (int i = 0; i < cuanto && !gameover; i++) { // Bajar
    borrar(); y++; pintar(); Sleep(60ms);
  }
  pintar();
  
}

class Cactus {
private:
  int x, y;
  int anchoConsola = obtenerAncho();
  int tipo;  // Almacena el tipo de cactus
  int altura; // Altura especifica de cada tipo
  int ancho; // Ancho SUPERIOR especifico
public:
  static int vel;  // velocidad compartida por todos los cactus
  Cactus(int _x, int _y) : x(_x), y(_y) {
    tipo = rand() % 3; // Selecciona aleatoriamente entre 3 tipos de cactus
    establecerDimensiones();
  }
  void borrar();
  void pintar();
  void mover();
  bool colision(Dinosaurio &);
  int X() { return x; }
  int Y() { return y; }

  // Establece las dimensiones según el tipo de cactus
  void establecerDimensiones() {
    switch (tipo) {
      case 0: altura = 4; ancho = 5; break;
      case 1: altura = 6; ancho = 8; break;
      case 2: altura = 6; ancho = 10; break;
      }
      y = COORDPISO - altura;
  }
};

// Función para borrar correctamente el cactus sin afectar el suelo
void Cactus::borrar() {
    for (int i = 0; i < altura; ++i) {
        mtx.lock();
        move(y + i, x);
        printw("%*s    ", ancho, " ");  // Borra el ancho específico del cactus
        mtx.unlock();
    }
}

// Función para pintar diferentes tipos de cactus con diferentes diseños
void Cactus::pintar() {
  // Diseños de cactus variados
  static const char* cactusTipos[3][6] = {
    { // Cactus tipo alto y delgado
        " |**|    ",
        "[****]    ",
        " |**|    ",
        " |**|    "
    },
    { // Cactus tipo bajo y ancho
        "  ||    ||     ",
        " |**|  |**|    ",
        "[****][****]    ",
        " |**|  |**|    ",
        " |**|  |**|    ",
        " |**|  |**|    "
    },
    { // Cactus tipo con ramas laterales
        "    |**|    ",
        "  [******]    ",
        "    |**|    ",
        " |**| |**|    ",
        "[****][****]    ",
        "    |**|    "
    }
  };

  for (int i = 0; i < altura; ++i) {
    mtx.lock();
    move(y + i, x);
    printw("%s", cactusTipos[tipo][i]);
    mtx.unlock();
  }
}
void Cactus::mover() {
  x -= vel;
  if (x <= 0) {
    x = 0;
    pintar(); // pinto por ultima vez en x=0
    borrar();
    x = anchoConsola - 16;
    tipo = rand() % 3;  // Cambia a un nuevo tipo al reaparecer
    establecerDimensiones();  // Actualiza las dimensiones según el nuevo tipo
  }
  pintar();
}

bool Cactus::colision(Dinosaurio &d) {
  // verifico esquina por esquina
  return (((x+1 > d.X()+2 && x+1 < d.X()+6) || (x+ancho > d.X()+2 && x+ancho < d.X()+11)) && (y <= d.Y()+9));
}

int Cactus::vel; //pata poder utilizar la variable velocidad

class Puntaje {
private:
    int puntaje;
    bool colorBlanco; // Estado actual del color (blanco o negro)
public:
    Puntaje() : puntaje(0), colorBlanco(true) {}
    void incrementar();
    void mostrar();
    void verificarColor(Dinosaurio&, Cactus&);
    int obtenerPuntaje();
};

void Puntaje::incrementar() {
    puntaje += 1;
}
void Puntaje::mostrar() {
    mtx.lock();
    move(1, 80); // Posición en la parte superior derecha de la consola
    printw("Puntaje %05d", puntaje);
    mtx.unlock();
}
void Puntaje::verificarColor(Dinosaurio &dino, Cactus &cactus) {
    // Cambia la velocidad aleatoriamente cada 400 puntos y a un cielo rojo
    if (puntaje % 400 == 0 && puntaje != 0) {
        if (cactus.X() - dino.X() < 50) { //si estan muy cerca el
          Sleep(1000ms); //dinosaurio y el cactus, esperar para
        }               //que el jugador alcance a reaccionar
        Cactus::vel = 1 + rand() % 3;  // Cambia la velocidad de los cactus a un valor aleatorio entre 1 y 3
        if (puntaje > 2000)
          bkgd(COLOR_PAIR(3)); // cambia a un fondo rojo con texto blanco
    }

    // Alterna entre blanco y negro, y negro y blanco cada 700 puntos
    if (puntaje % 700 == 0 && puntaje != 0) {
        colorBlanco = !colorBlanco;  // Invierte el color
        if (colorBlanco)
            bkgd(COLOR_PAIR(1));  // Fondo blanco, texto negro
        else
            bkgd(COLOR_PAIR(2));  // Fondo negro, texto blanco
    }
}

int Puntaje::obtenerPuntaje() {
    return puntaje;
}

class Nube {
    int x, y;
    int anchoConsola = obtenerAncho();
public:
    Nube(int _x, int _y) : x(_x), y(_y) {}

    void borrar();
    void pintar();
    void mover();
};

void Nube::borrar() {
    mtx.lock();
    move(y, x);     printw("                ");  // Borra la primera línea de la nube 17 ancho
    move(y + 1, x); printw("                ");  // Borra la segunda línea de la nube
    move(y + 2, x); printw("                ");  // Borra la tercera línea de la nube
    move(y + 3, x); printw("                ");  // Borra la cuarta línea de la nube
    mtx.unlock();
}

void Nube::pintar() {
    mtx.lock();
    move(y, x);
    printw("        **** ");
    move(y+1, x);
    printw("   *****    ** ");
    move(y+2, x);
    printw("***           * ");
    move(y+3, x);
    printw(" ************** ");
    mtx.unlock();
}

void Nube::mover() {
    x--;
    if (x <= 1) {
        borrar();
        x = anchoConsola - 16;  // Reaparece al lado derecho de la pantalla
        y = rand() % 4 + 5;     // Coloca la nube en una posición aleatoria arriba del dinosaurio
    }
    pintar();
}

//funciones para hilos

void moverDino(Dinosaurio &dino) {
  while (!gameover) {
    if ( getch() == KEY_UP)
      dino.saltar();
    else {
      dino.moverPata1();
      Sleep(40ms);
      dino.moverPata2();
      Sleep(40ms);
    }
  }
}

void actualizarPuntaje(Puntaje& p, Dinosaurio &dino, Cactus &cactus) {
    while (!gameover) {
        p.incrementar();   // Incrementa el puntaje en 100 puntos
        p.mostrar();       // Muestra el puntaje en pantalla
        p.verificarColor(dino, cactus); // Cambia el color si el puntaje es múltiplo de 700
        Sleep(30ms);       // Intervalo de 30 milisegundos
    }
}

void moverCactus(Cactus& cactus, Dinosaurio &dino) {
  while (!gameover) {
    cactus.mover();
      if (cactus.colision(dino)) {
        gameover = true;
        break;
    }
    Sleep(40ms);
  }
}

void moverNubes(list<Nube>& nubeList) {
    while (!gameover) {
        for (auto& nube : nubeList) {
            nube.mover();
        }
        Sleep(200ms);  // Mueve las nubes más lentamente que los cactus
    }
}

//programa principal
int main()
{
  initscr(); //iniciar el modo ncurses
  keypad(stdscr, TRUE); //permitir deteccion de las flecha hacia arriba
  curs_set(0); //ocultar el cursor
  noecho(); //no mostrar la entrada del teclado
  iniciarColores();
  pantallaIntroduccion();
  clear(); //borrar pantalla
  
  cbreak();
  halfdelay(1);

  //leer el puntaje maximo del archivo
  int puntajeMaximo;
  ifstream archivo("puntajeMax.txt");
  if (archivo.is_open()) {
    archivo >> puntajeMaximo;
  }
  else {
    puntajeMaximo = 0;
  }
  archivo.close();

  //bucle principal, se repite cada vez que pierdes
  while (true) {
    Cactus::vel = 1 + rand() % 3;
    gameover = false;
    
    Dinosaurio dino;
    Cactus cactus(obtenerAncho()-16, COORDPISO - 6);
    Puntaje puntaje;

    list<Nube> nubeList;
    int xInicial = 0;
    for (int i = 0; i < 3; ++i) {
      xInicial += rand() % 30 + 15;  // Asegura que las nubes no se toquen entre si
      int yInicial = rand() % 4 + 3;  // y que estén en una posición elevada
      nubeList.push_back(Nube(xInicial, yInicial));
    }

    clear();
    pintarEscenario();
    dino.pintar();
    move(1,60); printw("HI %05d", puntajeMaximo);
    
    thread dinoThread(moverDino, ref(dino));
    thread cactusThread(moverCactus, ref(cactus), ref(dino));
    thread puntajeThread(actualizarPuntaje, ref(puntaje), ref(dino), ref(cactus));
    thread nubeThread(moverNubes, ref(nubeList));

    cactusThread.join();
    dinoThread.join();
    puntajeThread.join();
    nubeThread.join();

    if (puntaje.obtenerPuntaje() > puntajeMaximo) {
    puntajeMaximo = puntaje.obtenerPuntaje();
    move(1, 60); printw("HI %05d", puntajeMaximo);
    ofstream archivo("puntajeMax.txt");
    if (archivo.is_open()){
      archivo << puntajeMaximo;
    }
      archivo.close();
    }
    
    move(15, obtenerAncho() / 2);
    printw("  GAME "); refresh();
    Sleep(1000ms);
    printw("OVER"); refresh();
    Sleep(1000ms);
    move(17, obtenerAncho() / 2);
    printw("Presiona ENTER"); refresh();
    do {
      Sleep(1000ms);
    } while( getch() != 10);
  }

  endwin();
}
