#include <iostream> // entrada-salida
#include <fstream> // para manejo de archivos
#include <windows.h> // manejo de coordenadas de la consola
#include <list> // listas de facil manejo
#include <thread> // hilos que pueden ejecutar funciones asincronicamente
#include <mutex> // para que un hilo se comunique con los otros cuando vaya a usar una funcion compartida
#include <cstdlib>  // Para generar números aleatorios
#include <ctime>    // Para obtener el tiempo que inicializara la semilla aleatoria

using namespace std;

#define COORDPISO 35

mutex mtx;  // Mutex global para la sincronización de salida en consola
bool gameover;  // variable global de estado del juego

// Función para verificar si una tecla está presionada
bool presionada(int codigoTecla) {
    return GetKeyState(codigoTecla) < 0;
}

// Función para mover el cursor en la consola
void gotoxy(int x, int y) {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD dwPos;
    dwPos.X = x;
    dwPos.Y = y;
    SetConsoleCursorPosition(hCon, dwPos);
}

// Función para ocultar el cursor
void OcultarCursor() {
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = 25;
    cci.bVisible = FALSE;
    SetConsoleCursorInfo(hCon, &cci);
}

// para obtener el alto de la consola ...
int obtenerAlto() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

// ... y su ancho
int obtenerAncho() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

// Función para pintar el suelo (escenario)
void pintarEscenario() {
    system("color f0"); // Fondo blanco brillante, texto negro
    for (int i = 0; i < obtenerAncho(); i++) {
        gotoxy(i, COORDPISO); printf("%c", 205); // Este es el suelo
    }
}

// Función para la pantalla introductoria
void pantallaIntroduccion() {
    // Cambiar el color de fondo a azul y texto a blanco
    system("color 1F");

    // Obtener las dimensiones actuales de la consola
    int ancho = obtenerAncho();
    int alto = obtenerAlto();

    // Dibujar un marco que cubra toda la consola;
    gotoxy(0, 0);
    printf("%c", 201);  // Esquina superior izquierda
    for (int i = 1; i < ancho - 2; i++) printf("%c", 205);  // Línea superior
    printf("%c", 187);  // Esquina superior derecha

    gotoxy(0, alto - 1); printf("%c", 200);  // Esquina inferior izquierda
    for (int i = 1; i < ancho - 2; i++) printf("%c", 205);  // Línea inferior
    printf("%c", 188);  // Esquina inferior derecha

    for (int i = 1; i < alto - 1; i++) {
        gotoxy(0, i); printf("%c", 186);  // Lado izquierdo
        gotoxy(ancho - 2, i); printf("%c", 186);  // Lado derecho
    }

    // Texto de bienvenida
    gotoxy(ancho / 2 - 15, alto / 2 - 4);
    printf("BIENVENIDO AL JUEGO DINO SURVIVAL");

    gotoxy(ancho / 2 - 15, alto / 2 - 2);
    printf("    CREADO POR APRIL Y MOISES");

    gotoxy(ancho / 2 - 15, alto / 2);
    printf("ASEGURATE DE MAXIMIZAR LA VENTANA");

    gotoxy(ancho / 2 - 12, alto / 2 + 2);
    printf("  Y PRESIONA ENTER PARA EMPEZAR");


    // Dibujo del dinosaurio grande en la parte inferior
    gotoxy(ancho / 2 - 10, alto - 8);
    printf("               __");
    gotoxy(ancho / 2 - 10, alto - 7);
    printf("              / _)");

    gotoxy(ancho / 2 - 10, alto - 6);
    printf("     _.----._/ / ");

    gotoxy(ancho / 2 - 10, alto - 5);
    printf("    /         /  ");

    gotoxy(ancho / 2 - 10, alto - 4);
    printf(" __/ (  |    (   ");

    gotoxy(ancho / 2 - 10, alto - 3);
    printf("/__.-'|_|--'_|   ");

    // Esperar a que el usuario presione ENTER
    getchar();
}


// Clase Dinosaurio
class Dinosaurio {
private:
    int x, y;
public:
    Dinosaurio(int _x, int _y) : x(_x), y(_y) {}
    void pintar();
    void borrar();
    void moverPata1(); // Animar las patas del dinosaurio
    void moverPata2();
    void saltar(int);
    int X() { return x; }
    int Y() { return y; }
};

void Dinosaurio::pintar() {
    mtx.lock();
    gotoxy(x, y); printf("     %c%c%c%c%c%c%c", 219,219,219,219,219,219,220);
    gotoxy(x, y+1); printf("     %c%c%c%c%c%c%c", 219,220,219,219,219,219,219);
    gotoxy(x, y+2); printf("     %c%c%c%c%c%c%c", 219,219,219,219,220,220,220);
    gotoxy(x, y+3); printf("     %c%c%c%c   ", 219,219,219,220);
    gotoxy(x, y+4); printf("%c   %c%c%c%c%c%c%c ", 219,219,219,219,219,219,223,219);
    gotoxy(x, y+5); printf("%c%c %c%c%c%c%c%c   ", 219,219,219,219,219,219,219,219);
    gotoxy(x, y+6); printf("%c%c%c%c%c%c%c%c%c   ", 219,219,219,219,219,219,219,219,219);
    gotoxy(x, y+7); printf(" %c%c%c%c%c%c%c%c   ", 219,219,219,219,219,219,219,219);
    gotoxy(x, y+8); printf("  %c%c%c%c%c%c    ", 219,219,219,219,219,223);
    gotoxy(x, y+9); printf("   %c  %c     ", 219,219);
    mtx.unlock();
}

void Dinosaurio::borrar() {
    mtx.lock();
    gotoxy(x,y); printf("            ");
    gotoxy(x,y+9); printf("            ");
    mtx.unlock();
}

void Dinosaurio::moverPata1() {
    mtx.lock();
    gotoxy(x, y+9);
    printf("   %c  %c", 223,219);
    mtx.unlock();
}

void Dinosaurio::moverPata2() {
    mtx.lock();
    gotoxy(x, y+9);
    printf("   %c  %c", 219,223);
    mtx.unlock();
}

void Dinosaurio::saltar(int cuanto) {
    for (int i = 0; i < cuanto && !gameover; i++) { // Subir
        borrar(); y--; pintar(); Sleep(30);
    }
    for (int i = 0; i < cuanto && !gameover; i++) { // Bajar
        borrar(); y++; pintar(); Sleep(30);
    }
    pintar();
}

// Clase Cactus
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
            case 0: altura = 4; ancho = 5; break; // Cactus alto y delgado
            case 1: altura = 6; ancho = 8; break; // Cactus bajo y ancho
            case 2: altura = 6; ancho = 10; break; // Cactus con ramas laterales
        }
        y = COORDPISO - altura;
    }
};

// Función para borrar correctamente el cactus sin afectar el suelo
void Cactus::borrar() {
    for (int i = 0; i < altura; ++i) {
        mtx.lock();
        gotoxy(x, y + i);
        printf("%*s    ", ancho, " ");  // Borra el ancho específico del cactus
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
        gotoxy(x, y + i);
        printf("%s", cactusTipos[tipo][i]);
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

int Cactus::vel;

// Clase para el manejo del puntaje
class Puntaje {
private:
    int puntaje;
    bool colorBlanco; // Estado actual del color (blanco o negro)
public:
    Puntaje() : puntaje(0), colorBlanco(true) { // Constructor que inicializa el puntaje a 0 y el color a blanco
        system("color f0"); // Fondo blanco, texto negro
    }

    void incrementar();
    void mostrar();
    void verificarColor();
    int obtenerPuntaje();
};

void Puntaje::incrementar() {
    puntaje += 1;
}
void Puntaje::mostrar() {
    mtx.lock();
    gotoxy(80, 1); // Posición en la parte superior derecha de la consola
    printf("Puntaje %05d", puntaje);
    mtx.unlock();
}
void Puntaje::verificarColor() {
    // Cambia la velocidad aleatoriamente cada 400 puntos y a un cielo rojo
    if (puntaje % 400 == 0 && puntaje != 0) {
        Cactus::vel = 1 + rand() % 4;  // Cambia la velocidad de los cactus a un valor aleatorio entre 1 y 4
        if (puntaje > 2000)
          system("color 4F"); // cambia a un fondo rojo con texto blanco
    }

    // Alterna entre blanco y negro, y negro y blanco cada 700 puntos
    if (puntaje % 700 == 0 && puntaje != 0) {
        colorBlanco = !colorBlanco;  // Invierte el color
        if (colorBlanco)
            system("color F0");  // Fondo blanco, texto negro
        else
            system("color 0F");  // Fondo negro, texto blanco
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
    gotoxy(x, y);     printf("                ");
    gotoxy(x, y + 1); printf("                ");
    gotoxy(x, y + 2); printf("                ");
    gotoxy(x, y + 3); printf("                ");
    gotoxy(x, y + 4); printf("                ");
    gotoxy(x, y + 5); printf("                ");
    mtx.unlock();
}
// Función para pintar la nube

void Nube::pintar() {
    mtx.lock();
    gotoxy(x, y);
    printf("        **** ");
    gotoxy(x, y + 1);
    printf("   *****    ** ");
    gotoxy(x, y + 2);
    printf("***           * ");
    gotoxy(x, y + 3);
    printf(" ************** ");
    mtx.unlock();
}

// Función para mover la nube
void Nube::mover() {
    x--;
    if (x <= 1) {
        borrar();
        x = anchoConsola - 16;  // Reaparece al lado derecho de la pantalla
        y = rand() % 4 + 5;     // Coloca la nube en una posición aleatoria arriba del dinosaurio
    }
    pintar();
}

// funcion para mover el dinosaurio en un hilo separado
void moverDino(Dinosaurio &dino) {
    while (!gameover) {
        if (presionada(VK_UP)) { // al presionar la flecha hacia arriba
            dino.saltar(16);
        } else {
            dino.moverPata1();
            Sleep(30);
            dino.moverPata2();
            Sleep(30);
        }
    }
}

// Función para actualizar el puntaje y la velocidad del cactus en intervalos de tiempo en otro hilo
void actualizarPuntaje(Puntaje& p) {
    while (!gameover) {
        p.incrementar();   // Incrementa el puntaje en 100 puntos
        p.mostrar();       // Muestra el puntaje en pantalla
        p.verificarColor(); // Cambia el color si el puntaje es múltiplo de 700
        Sleep(30);       // Intervalo de 30 milisegundos
    }
}

// funcion para mover el cactus en un hilo separado
void moverCactus(Cactus& cactus, Dinosaurio &dino) {
  while (!gameover) {
    cactus.mover();
      if (cactus.colision(dino)) {
        gameover = true;
        break;
    }
    Sleep(30);
  }
}

// Función para mover y pintar nubes continuamente en un hilo separado
void moverNubes(list<Nube>& nubeList) {
    while (!gameover) {
        for (auto& nube : nubeList) {
            nube.mover();
        }
        Sleep(100);  // Mueve las nubes más lentamente que los cactus
    }
}

// En el main, creamos la instancia de las clases y ejecutamos las funciones en hilos
int main() {
    OcultarCursor();
    system("cls");
    srand(time(0)); // Inicializa la semilla para números aleatorios
    // si no se inicializa, los numeros aleatorios seran los mismos cada vez
    // que se ejecute el programa

    pantallaIntroduccion();

    //leer el puntaje maximo desde un archivo
    int puntajeMaximo;
    ifstream archivo("puntajeMax.txt");
    if (archivo.is_open()) {
      archivo >> puntajeMaximo;
    }
    else {
      puntajeMaximo = 0;
    }
    archivo.close();

    while (true) {  // Bucle principal, se repite cada vez que pierde
        Cactus::vel = 1 + rand() % 4; //se resetea la velocidad
        gameover = false;

        // instancias de las clases
        Dinosaurio dino(16, COORDPISO - 10);
        Cactus cact(obtenerAncho()-16, COORDPISO - 6);
        Puntaje puntaje;

        // Crea una lista de nubes
        list<Nube> nubeList;
        int xInicial = 0;
        for (int i = 0; i < 3; ++i) {
          xInicial += rand() % 30 + 15;  // Asegura que las nubes no aparezcan encima una de la otra
          int yInicial = rand() % 4 + 3;  // Asegura que las nubes estén en una posición elevada
          nubeList.push_back(Nube(xInicial, yInicial));
        }

        system("cls");
        pintarEscenario();
        dino.pintar();
        gotoxy(60,1); printf("HI %05d", puntajeMaximo);
        OcultarCursor();

        // Ejecución de los hilos
        thread cactusThread(moverCactus, ref(cact), ref(dino));
        thread dinoThread(moverDino, ref(dino));
        thread puntajeThread(actualizarPuntaje, ref(puntaje));
        thread nubeThread(moverNubes, ref(nubeList));

        // al perder, esperar a la finalizacion de los hilos
        cactusThread.join();
        dinoThread.join();
        puntajeThread.join();
        nubeThread.join();

        // comprobar si se supero el puntaje maximo
        if (puntaje.obtenerPuntaje() > puntajeMaximo) {
          puntajeMaximo = puntaje.obtenerPuntaje();
          gotoxy(60,1); printf("HI %05d", puntajeMaximo);
          // escribir el nuevo puntaje maximo en un archivo
          ofstream archivo("puntajeMax.txt");
          if (archivo.is_open()){
            archivo << puntajeMaximo;
          }
          archivo.close();
        }

        //pantalla de GAME OVER
        gotoxy(obtenerAncho() / 2, 15); printf("  GAME "); Sleep(1000);
        printf("OVER"); Sleep(1000);
        gotoxy(obtenerAncho() / 2, 17); printf("Presiona ENTER");
        getchar();
    }
    return 0;
}
