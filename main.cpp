/* --------------- AUTHORS ---------------
- Iván Calvo (calvuxi@gmail.com).
- David Pérez (hallaplay835@gmail.com).
*/

/* --------------- CHANGELOG ---------------
v1.0 # 24/12/2013 (hallaplay835)
	- Funcionalidad básica front-end.
v2.0 # 27/12/2013 | (hallaplay835)
	- Funcionalidad básica back-end.
v3.0 # xx/xx/xxxx | ()
v3.1 # xx/xx/xxxx | ()
v3.11 # xx/xx/xxxx | ()
v3.12 # xx/xx/xxxx | ()
v3.13 # xx/xx/xxxx | ()
v3.14 # xx/xx/xxxx | ()
v3.141 # xx/xx/xxxx | ()
v3.142 # xx/xx/xxxx | ()

PI = 3.14159 26535 89793 23846 26433 83279 50288

#### BUGS #### (~ indica resuletos)
1. ~

#### SUGGESTIONS #### (~ indica llevadas a cabo)
1. ~
*/

// #### Libraries ####
#include <iostream>
#include <cstdlib>
// #include <regex>
#include <ctime>
#include <string>
#include <fstream>
// #include <iomanip>

using namespace std;

typedef unsigned short int usi;

// #### Constants ####
const string INVALID_TYPE = "ERROR. Tipo de datos no válido.";
const string CENTINEL = "xxx";
const usi CHIPS = 4, TRIES = 30, COLORS = 6;
const bool REPTS = false; // false = sin repeticiones, true = con repeticiones.

// #### other typedef declarations ####
typedef enum tColores { Rojo, Azul, Verde, Negro, Granate, Marrón };
typedef tColores tCodigo[CHIPS];
typedef enum tChoice { good, cancel, help, error, repts };

// #### Prototypes ####
void pause();
void chcp1252();
int readInt(string ERR_MSG = INVALID_TYPE, int m = INT_MIN, int n = INT_MAX);
int readNat(string ERR_MSG = INVALID_TYPE, unsigned int n = INT_MAX);
usi menu();
bool displayTXTFile(string fileName, unsigned int i = 1, unsigned int j = INT_MAX);
bool displayTXTFileWCentinel(string fileName, string centinel);
void genRndKey(tCodigo key);
void printKey(tCodigo key);
char toColorId(tColores color);
tChoice readCode(tCodigo code);

// #### main() ####
int main() {
	chcp1252();

	// Mostrar el menú y leer una opción:
	usi opt = menu();
	while (opt != 0) {
		switch (opt) {
			case 1: // Jugar una partida a Mastermind.
				/*tCodigo key;
				while (true) {
					genRndKey(key);
					printKey(key);
					pause();
				}*/
				break;
		}
		
		// Mostrar el menú y leer otra opción:
		opt = menu();
	}
	
	// pause();

	return 0;
}

// #### UDFs ####

/** Pausa la ejecución del programa hasta que se pulse Intro. **/
void pause() {
	cin.clear();
	cin.sync();
	cout << "Pulsa Intro para continuar...";
	cin.ignore(INT_MAX, '\n'); // Consume INT_MAX caracteres o hasta el salto de línea (incluido).
	// cin.ignore(); -> cin.ignore(1, EOF);
}

/** Cambia la página de códigos activa a 1252. **/
void chcp1252(){
	system("chcp 1252");
	system("cls");
}

/** Lee y devuelve un entero entre m y n (ambos inclusive). En caso de fallo en la lectura, muestra ERR_MSG y pide otro entero. **/
int readInt(string ERR_MSG, int m, int n) {
	int input;
	cin >> input;
	while (cin.fail() || input < m || input > n) {
		cin.clear(); cin.sync(); // Restablecer flujo, descartar más entrada.
		cout << ERR_MSG;
		cin >> input;
	}
	cin.sync();

	// n es entero y válido.
	return input;
}

/** Lee y devuelve un natural menor o igual que n. **/
int readNat(string ERR_MSG, unsigned int n) {
	return readInt(ERR_MSG, n);
}

/** Muestra el menú y devuelve un entero representando la selección del usuario. **/
usi menu() {
	cout << endl;
	cout << "--- MENÚ ---";
	cout << endl << endl;
	cout << "1 - Jugar." << endl;
	cout << "0 - Salir." << endl;
	cout << "Opción: ";
	return (usi) readInt("Opción no valida. Opción: ", 0, 1);
}

/** Muestra en consola los contenidos del archivo de texto fileName, desde la línea i hasta la j (inclusive).
 ** Devuelve true si el archivo se muestra con éxito, false en caso contrario. **/
bool displayTXTFile(string fileName, unsigned int i, unsigned int j) {
	ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open()) {
		return false;
	} else {
		// Imprimir archivo, copiando línea a línea:
		string buff;
		getline(file, buff);
		for (unsigned int l = 1; l < j && !file.fail(); l++, getline(file, buff)) {
			if (l >= i) {
				cout << buff << endl;
			}
		}
		if (!file.fail()) {
			cout << buff; // Última línea sin endl.
		}
		file.close();
		return true;
	}
}

/** Muestra en consola los contenidos del archivo de texto fileName.
 ** Si existen múltiples espacios entre string y string del archivo, se imprimirá solo uno.
 ** No se imprimen caracteres de control ni espacios al principio del archivo.
 ** Se dejarán de mostrar los contenidos del archivo en caso de leer el string centinel, independientemente
 ** de si se ha llegado al final del archivo.
 ** Devuelve true si el archivo se muestra con éxito, false en caso contrario. **/
bool displayTXTFileWCentinel(string fileName, string centinel) {
	ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open()) {
		return false;
	}
	else {
		// Imprimir archivo, copiando línea a línea:
		string buff;
		file >> buff;
		while (!file.fail() && buff != centinel) {
			cout << buff << " ";
			file >> buff;
		}
		file.close();
		return true;
	}
}

/** Genera una clave de Mastermind en key, con o sin repeticiones dependiendo del valor de REPTS. **/
void genRndKey(tCodigo key) {
	bool chart[COLORS];

	// Inicializar todas las entradas de chart a false.
	for (usi i = 0; i <= COLORS - 1; i++) {
		chart[i] = false;
	}

	// Inicializar semilla:
	srand((unsigned int)time(NULL));

	for (usi i = 0; i <= CHIPS - 1; i++) {

		// Generar un número aleatorio entre 0 y COLORS - 1 inclusive:
		usi rnd = rand() % COLORS;

		if (!REPTS) { // La clave no puede contener repeticiones.
			while (chart[rnd]) { // El color ya se ha usado en la clave.
				rnd = rand() % COLORS; // Generar otro número.
			}
		}
		
		chart[rnd] = true; // Marcar color como usado.
		key[i] = (tColores)rnd; // Convertir y añadir a la clave.
	}

}

/** Imprime los identificadores de key a consola. **/
void printKey(tCodigo key) {
	for (usi i = 0; i <= CHIPS - 1; i++) {
		cout << toColorId(key[i]);
	}
}

/** Devuelve el identificador de color. **/
char toColorId(tColores color) {
	switch (color) {
		case Rojo:
			return 'R';
			break;
		case Azul:
			return 'A';
			break;
		case Verde:
			return 'V';
			break;
		case Negro:
			return 'N';
			break;
		case Granate:
			return 'G';
			break;
		case Marrón:
			return 'M';
			break;
		default:
			return 'R';
	}
}

/* tChoice readCode(tCodigo code) {
	cout << "Código (? para ayuda, 0 para cancelar): ";
	string input;
	getline(cin, input);
	if (input == "0") {
		return cancel;
	} else if (input == "?") {
		return help;
	} else if (input.length() != CHIPS) {
		return error;
	} else {
		// LLenar code con el código.
		for (usi i = 0; i <= CHIPS - 1; i++) {
			// if (input[i] != 'R' || input[i] != 'A' || input )
		}
		return good;
	}
} */