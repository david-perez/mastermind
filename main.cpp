/* --------------- AUTHORS ---------------
- David Pérez (hallaplay835@gmail.com).
- Iván Calvo (calvuxi@gmail.com).
*/

/*
#### BUGS #### (~ indica resuletos)
1. ~

#### SUGGESTIONS #### (~ indica llevadas a cabo)
1. Preguntar cuál es el propósito de que un archivo de texto del que se van a copiar strings acabe en centinela.
*/

// #### Libraries ####
#include <iostream>
#include <cstdlib>
// #include <regex>
#include <ctime>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

typedef unsigned short int usi;

// #### Constants ####
const string INVALID_TYPE = "ERROR. Tipo de datos no válido.";
const string HELP_FILE = "ayuda.txt";
const string CENTINEL = "xxx";
const string PROMPT = "Código (? para ayuda, ! para pista, 0 para cancelar): ";
const usi CHIPS = 4, TRIES = 30, COLORS = 6, MAX_HINTS = 2, MIN_TRIES_BTW_HINTS = 5;
const bool REPTS = false; // false = sin repeticiones, true = con repeticiones.

// #### other typedef declarations ####
typedef enum tColores { Rojo, Azul, Verde, Negro, Granate, Marrón };
typedef tColores tCodigo[CHIPS];
typedef enum tStatus { good, cancel, help, length_err, key_err, rept_err, hint };

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
tColores toColor(char id);
string colorToColorName(tColores color);
bool correctKeys(string input);
void totCodigo(string input, tCodigo code);
tStatus readCode(tCodigo code);
bool calcPerformance(tCodigo code, tCodigo key, usi &correct_keys, usi &disordered_keys);
void printPerformanceMsg(tCodigo code, tCodigo key, usi tries, usi score);
void printHint(tCodigo key);
void manageStatus(tStatus status);
usi calcScore(usi score, usi correct_keys, usi disordered_keys, bool won);
usi playMastermind();

// #### main() ####
int main() {
	chcp1252();

	// Mostrar el menú y leer una opción:
	usi opt = menu();
	while (opt != 0) {
		cout << endl;
		switch (opt) {
			case 1: // Jugar una partida a Mastermind.
				usi tries;
				tries = playMastermind();
		}
		
		// Mostrar el menú y leer otra opción:
		opt = menu();
	}

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

/** Muestra el menú y devuelve un usi representando la selección del usuario. **/
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
 ** Si se lee una línea acabada en CENTINEL, será la última línea que se imprimirá. **/
bool displayTXTFileWCentinel(string fileName, string centinel) {
	ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open()) {
		return false;
	} else {
		// Imprimir archivo, copiando línea a línea:
		string buff;
		getline(file, buff);
		while (!file.fail() && buff.substr(buff.length() - 3, 3) != centinel) {
			cout << buff << endl;
			getline(file, buff);
		}
		if (!file.fail() && buff != "centinel") cout << buff.substr(0, buff.length() - 3);
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
	usi rnd;

	for (usi i = 0; i <= CHIPS - 1; i++) {

		// Generar un número aleatorio entre 0 y COLORS - 1 inclusive:
		rnd = rand() % COLORS;

		if (!REPTS) { // La clave no puede contener repeticiones.
			while (chart[rnd]) { // El color ya se ha usado en la clave.
				rnd = rand() % COLORS; // Generar otro número.
			}
		}
		
		chart[rnd] = true; // Marcar color como usado.
		key[i] = (tColores)rnd; // Convertir y añadir a la clave.
	}

}

/** Imprime los identificadores de key a consola, seperados por un espacio. **/
void printKey(tCodigo key) {
	for (usi i = 0; i <= CHIPS - 1; i++) {
		cout << toColorId(key[i]);
		if (i != CHIPS - 1) cout << " ";
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

/** Devuelve el tColor del identificador id. En caso de no existir el identificador, devuelve Rojo. **/
tColores toColor(char id) {
	if (id == 'r' || id == 'R') {
		return Rojo;
	} else if (id == 'a' || id == 'A') {
		return Azul;
	} else if (id == 'v' || id == 'V') {
		return Verde;
	} else if (id == 'n' || id == 'N') {
		return Negro;
	} else if (id == 'g' || id == 'G') {
		return Granate;
	} else if (id == 'm' || id == 'M') {
		return Marrón;
	} else {
		return Rojo;
	}
}

/** Devuelve el string del color asociado a color. **/
string colorToColorName(tColores color) {
	switch (color) {
		case Rojo:
			return "rojo";
			break;
		case Azul:
			return "azul";
			break;
		case Verde:
			return "verde";
			break;
		case Negro:
			return "negro";
			break;
		case Granate:
			return "granate";
			break;
		case Marrón:
			return "marrón";
			break;
	}
}

/** Devuelve true si input contiene identificadores correctos, false en otro caso. Se asume que la longitud de input es CHIPS. **/
bool correctKeys(string input) {
	bool correctKeys = true;
	for (usi i = 0; i <= CHIPS - 1 && correctKeys; i++) {
		if (input[i] != 'r' && input[i] != 'a' && input[i] != 'v' && input[i] != 'n' && input[i] != 'g' && input[i] != 'm'
		 && input[i] != 'R' && input[i] != 'A' && input[i] != 'V' && input[i] != 'N' && input[i] != 'G' && input[i] != 'M') {
			correctKeys = false;
		}
	}
	return correctKeys;
}

/** Copia en code los colores asociados a los ids de los chars de string. **/
void totCodigo(string input, tCodigo code) {
	for (usi i = 0; i <= CHIPS - 1; i++) {
		code[i] = toColor(input[i]);
	}
}

/** Pide y lee un código del usuario. Devuelve un estado y copia el código a code si es del tipo correcto. **/
tStatus readCode(tCodigo code) {
	cout << PROMPT;
	string input;
	getline(cin, input);
	if (input == "0") {
		return cancel;
	} else if (input == "?") {
		return help;
	} else if (input == "!") {
		return hint;
	} else if (input.length() != CHIPS) { // Longitud del código inadecuada.
		return length_err;
	} else if (!correctKeys(input)) { // Longitud del código adecuada, identificadores incorrectos.
		return key_err;
	} else if (!REPTS) { // Longitud del código adecuada, identificadores correctos. Comprobar repeticiones si procede.
		bool chart[COLORS];
		bool repts = false; // Asumir que no hay repeticiones.

		// Inicializar todas las entradas de chart a false.
		for (usi i = 0; i <= COLORS - 1; i++) {
			chart[i] = false;
		}

		for (usi i = 0; i <= CHIPS - 1 && !repts; i++) {
			if (chart[toColor(input[i])]) { // Se había leído el identificador antes.
				repts = true;
			} else {
				chart[toColor(input[i])] = true;
			}
		}

		if (repts) {
			return rept_err;
		} else {
			totCodigo(input, code);
			return good;
		}
	} else { // Longitud del código adecuada, identificadores correctos. No hace falta comprobar repeticiones.
		totCodigo(input, code);
		return good;
	}
}

/** Calcula en correct_keys y disordered_keys el número de aciertos totales y parciales de code con respecto a key.
 ** Devuelve true si code y key son idénticos, false en caso contrario. **/
bool calcPerformance(tCodigo code, tCodigo key, usi &correct_keys, usi &disordered_keys) {
	bool chart[CHIPS];

	// Inicializar todas las entradas de chart a false.
	for (usi i = 0; i <= CHIPS - 1; i++) {
		chart[i] = false;
	}

	// Calcular aciertos totales:
	for (usi i = 0; i <= CHIPS - 1; i++) {
		if (code[i] == key[i]) {
			chart[i] = true;
			correct_keys++;
		}
	}

	// Calcular aciertos parciales:
	bool found;
	for (usi i = 0; i <= CHIPS - 1; i++) {
		found = false;
		for (usi j = 0; j <= CHIPS - 1 && !found; j++) {
			if (!chart[j] && code[i] == key[j]) {
				disordered_keys++;
				chart[j] = true;
				found = true;
			}
		}
	}

	// Return:
	if (correct_keys == CHIPS) {
		return true;
	}
	else {
		return false;
	}
}

/** Muestra un mensaje en la consola indicando el número de aciertos totales y parciales
 ** (colores del codigo que están en la clave, pero descolocados), así como el número de intentos. **/
void printPerformanceMsg(tCodigo code, usi correct_keys, usi disordered_keys, usi tries, usi score) {
	cout << "   " << tries << ": ";
	printKey(code);
	cout << setfill(' ') << setw(8) << correct_keys << " ¬" << setw(6) << disordered_keys << " ~" << setw(6) << score << " punto" << (score > 1 ? "s" : "") << endl;
}

/** Imprime en la consola una pista acerca de la clave. El sistema de pistas es el rudimentario de la especificación. **/
void printHint(tCodigo key) {

	// Inicializar semilla:
	srand((unsigned int)time(NULL));

	// Generar un número aleatorio entre 0 y CHIPS - 1 inclusive:
	usi rnd = rand() % CHIPS;

	// Imprimir pista:
	cout << "El color en la posición " << rnd + 1 << " es: " << colorToColorName(key[rnd]) << ".";

}

/** Imprime el mensaje correspondiente al estado status. No se contemplan los status good ni hint. **/
void manageStatus(tStatus status) {
	switch (status) {
		case cancel:
			cout << "Has abandonado el juego.";
			cout << endl;
			break;
		case help:
			cout << endl << "--- AYUDA ---" << endl << endl;
			if (!displayTXTFileWCentinel(HELP_FILE, CENTINEL)) {
				cout << "No se ha encontrado el archivo de ayuda.";
			}
			cout << endl << endl;
			break;
		case length_err:
			cout << "La longitud de los códigos debe ser de " << CHIPS << " caracter";
			cout << (CHIPS > 1 ? "es" : "") << ".";
			cout << endl;
			break;
		case key_err:
			cout << "Los códigos sólo pueden tener ciertos colores (? para consultar los permitidos).";
			cout << endl;
			break;
		case rept_err:
			cout << "El modo de juego actual no admite códigos con colores repetidos.";
			cout << endl;
			break;
	}
}

/** Calcula la puntuación agregada de un jugador. **/
usi calcScore(usi score, usi correct_keys, usi disordered_keys, bool won) {
	return score += disordered_keys + 5 * correct_keys + (won ? 100 : 0);
}

/** Conduce el desarrollo de una partida a Mastermind. Devuelve el número de intentos empleados
 ** por el jugador (TRIES si no la acertó). Devuelve 0 si se selecciona la opción de salir, independientemente
 ** del número de intentos del jugador hasta ese momento. **/
usi playMastermind() {
	cout << "--- NUEVA PARTIDA ---" << endl << endl;
	tCodigo key;
	genRndKey(key);
	// **************** <DEBUG> ****************
	cout << "La clave es: ";
	printKey(key);
	cout << endl;
	// **************** </DEBUG> ****************
	tStatus status;
	tCodigo code;
	usi score = 0;
	usi tries = 0;
	short int lastHint = -1; // Representa el intento en el cual se proporcionó la última pista.
							 // -1 indica que aún no se ha proporcionado ninguna pista.
	usi hints = 0; // Representa la cantidad de pistas que se han proporcionado.
	usi correct_keys = 0;
	usi disordered_keys = 0;
	bool won = false;
	do {
		status = readCode(code);
		if (status == good) { // Se ha leído un código de tipo correcto.

			// Reinicializar contadores:
			correct_keys = 0;
			disordered_keys = 0;

			// Calcular intentos, aciertos, puntuación e imprimir mensaje de rendimiento.
			tries++;
			won = calcPerformance(code, key, correct_keys, disordered_keys);
			score = calcScore(score, correct_keys, disordered_keys, won);
			printPerformanceMsg(code, correct_keys, disordered_keys, tries, score);
		} else if (status == hint) {
			if (tries == 0) {
				cout << "Consume al menos un intento para solicitar una pista.";
			} else if (hints == MAX_HINTS) {
				cout << "Se ha agotado el número máximo de pistas (" << hints << ").";
			} else if (tries - lastHint < MIN_TRIES_BTW_HINTS && lastHint != -1) {
				cout << "Sólo se puede solicitar una pista tras " << MIN_TRIES_BTW_HINTS;
				cout << " intento" << (MIN_TRIES_BTW_HINTS > 1 ? "s" : "") << " desde que se dio la última.";
			} else { // Se satisfacen los requisitos para solicitar una pista.
				lastHint = tries;
				hints++;
				printHint(key);
			}
			cout << endl;
		} else { // El código leído no era correcto, o se seleccionó la opción de ayuda o la de salir. Imprimir el mensaje que corresponda.
			manageStatus(status);
		}
	} while (tries < TRIES && !won && status != cancel);
	
	// Preguntar por la razón de salida del bucle.
	if (won) { // El jugador ha acertado la clave.
		cout << "¡ENHORABUENA! Has ganado en " << tries << " intento" << (tries > 1 ? "s" : "") << "." << endl;
	} else if (tries == TRIES) { // El jugador no acertó la clave en el intento TRIES.
		cout << "Se te han acabado los " << tries << " intentos. La clave era: ";
		printKey(key);
		cout << "." << endl;
	} else { // El jugador ha abandonado el juego.
		tries = 0;
	}
	return tries;
}