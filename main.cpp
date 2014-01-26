/* --------------- AUTHORS ---------------
- David P�rez (hallaplay835@gmail.com).
- Iv�n Calvo (calvuxi@gmail.com).
*/

/*
#### BUGS #### (~ indica resuletos)
1. ~

#### SUGGESTIONS #### (~ indica llevadas a cabo)
1. Preguntar cu�l es el prop�sito de que un archivo de texto del que se van a copiar strings acabe en centinela.
*/

// #### Libraries ####
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <fstream>
#include <iomanip>
#include <climits>

using namespace std;

typedef unsigned short int usi;

// #### Constants ####
const string PROMPT = "C�digo (? para ayuda, ! para pista, 0 para cancelar): ", INVALID_TYPE = "ERROR. Tipo de datos no v�lido.";
const string SAVE_CONFIG_PROMPT = "Configuraci�n guardada.";
const string HELP_FILE = "ayuda.txt", USER_FILE = "usuarios.txt", CENTINEL = "xxx";
const usi CHIPS = 4, MAX_CHIPS = 6, MAX_TRIES = 30, COLORS = 6, MAX_HINTS = 2, MIN_TRIES_BTW_HINTS = 5, USER_FILE_COLS = 4, MAX_USER_LENGTH = 20;
unsigned MAX_SCORE = 999999999;
const bool REPTS = false; // false = sin repeticiones, true = con repeticiones.

// #### other typedef declarations ####
typedef enum tColores { Rojo, Azul, Verde, Negro, Granate, Marron };
typedef tColores tCodigo[MAX_CHIPS];
typedef enum tStatus { good, cancel, help, length_err, key_err, rept_err, hint };
typedef unsigned tScore[USER_FILE_COLS - 1];

typedef struct {
	usi chips;
	usi max_tries;
	usi max_hints;
	usi min_tries_btw_hints;
	bool repts;
} tConfig;

// #### Prototypes ####
void pause();
void chcp1252();

int readInt(string ERR_MSG = INVALID_TYPE, int m = INT_MIN, int n = INT_MAX);
bool readBool(string prompt, string opt1, string opt2);
usi menu();
usi configMenu();
void changeConfig(tConfig& config);
void displayConfig(const tConfig& config);
bool displayTXTFile(string fileName, unsigned int i = 1, unsigned int j = INT_MAX);
bool displayTXTFileWCentinel(string fileName, string centinel);

void genRndKey(tCodigo key, const tConfig& config);
void printKey(tCodigo key, const tConfig& config);

char toColorId(tColores color);
tColores toColor(char id);
string colorToColorName(tColores color);
void totCodigo(string input, tCodigo code, const tConfig& config);
bool correctKeys(string input, const tConfig& config);
tStatus readCode(tCodigo code, const tConfig& config);
bool calcPerformance(tCodigo code, tCodigo key, usi &correct_keys, usi &disordered_keys, const tConfig& config);
void printPerformanceMsg(tCodigo code, tCodigo key, usi tries, usi score, const tConfig& config);
void printHint(tCodigo key, const tConfig& config);
void manageStatus(tStatus status, const tConfig& config);
usi calcScore(usi score, usi correct_keys, usi disordered_keys, bool won);
usi playMastermind(string user, const tConfig& config);

string getUserName();
bool setScore(string user, tScore score);
bool getScore(string user, tScore score);
bool updateScore(string user, bool won, unsigned score);
string unsignedToString(unsigned num, char filler, bool side);
bool displayLeaderBoard();

// #### main() ####
int main() {
	chcp1252();
	// Inicializar semilla:
	srand((unsigned int)time(NULL));
	// Establecer la configuraci�n por defecto:
	tConfig config;
	config.chips = CHIPS;
	config.max_tries = MAX_TRIES;
	config.max_hints = MAX_HINTS;
	config.min_tries_btw_hints = MIN_TRIES_BTW_HINTS;
	config.repts = REPTS; // false = sin repeticiones, true = con repeticiones.

	// Solicitar el nombre del jugador:
	string user = getUserName();
	cout << "�Hola " << user << "! Elige una opci�n..." << endl;

	// Mostrar el men� y leer una opci�n:
	usi opt = menu();
	while (opt != 0) {
		cout << endl;
		switch (opt) {
		case 1: // Jugar una partida a Mastermind.
			playMastermind(user, config);
			break;
		case 2:
			displayLeaderBoard();
			break;
		case 3: // Mostrar configuraci�n.
			displayConfig(config);
			break;
		case 4: // Cambiar configuraci�n.
			changeConfig(config);
			break;
		}

		// Mostrar el men� y leer otra opci�n:
		opt = menu();
	}

	return 0;
}

// #### UDFs ####

/** Pausa la ejecuci�n del programa hasta que se pulse Intro. **/
void pause() {
	cin.clear();
	cin.sync();
	cout << "Pulsa Intro para continuar...";
	cin.ignore(INT_MAX, '\n'); // Consume INT_MAX caracteres o hasta el salto de l�nea (incluido).
	// cin.ignore(); -> cin.ignore(1, EOF);
}

/** Cambia la p�gina de c�digos activa a 1252. **/
void chcp1252(){
	system("chcp 1252");
	system("cls");
}

/** Lee y devuelve un entero entre m y n (ambos inclusive). En caso de fallo en la lectura, muestra ERR_MSG y pide otro entero. **/
int readInt(string ERR_MSG, int m, int n) {
	int input;
	cin >> input;
	while (cin.fail() || input < m || input > n) {
		cin.clear(); cin.sync(); // Restablecer flujo, descartar m�s entrada.
		cout << ERR_MSG;
		cin >> input;
	}
	cin.sync();

	// n es entero y v�lido.
	return input;
}

/** Lee y devuelve un booleano, mostrando la petici�n prompt y las opciones opt1 (true) y opt2 (false). **/
bool readBool(string prompt, string opt1, string opt2) {
	cout << prompt << " (" << opt1 << "/" << opt2 << "): ";

	string i;

	cin >> i;
	while (cin.fail() || !(i == opt1 || i == opt2)) { // Validar que la entrada es de tipo correcto.
		cin.clear(); cin.sync(); // Restablecer flujo, descartar m�s entrada.
		cout << "ERROR. " << prompt << " (" << opt1 << "/" << opt2 << "): ";
		cin >> i;
	}
	cin.sync();

	if (i == opt1) return true; else return false;
}

/** Muestra el men� y devuelve un usi representando la selecci�n del usuario. **/
usi menu() {
	cout << endl;
	cout << "--- MEN� ---";
	cout << endl << endl;
	cout << "1 - Jugar." << endl;
	cout << "2 - Puntuaciones." << endl;
	cout << "3 - Mostrar configuraci�n." << endl;
	cout << "4 - Cambiar configuraci�n." << endl;
	cout << "0 - Salir." << endl;
	cout << "Opci�n: ";
	return (usi)readInt("Opci�n no valida. Opci�n: ", 0, 4);
}

/** Muestra el men� de configuraci�n y devuelve un usi representando la selecci�n del usuario. **/
usi configMenu() {
	cout << "--- MEN� DE CONFIGURACI�N ---";
	cout << endl << endl;
	cout << "1 - Cambiar n�mero de fichas." << endl;
	cout << "2 - Cambiar n�mero m�ximo de intentos." << endl;
	cout << "3 - Cambiar n�mero m�ximo de pistas." << endl;
	cout << "4 - Cambiar n�mero m�nimo de intentos entre pistas." << endl;
	cout << "5 - Cambiar modo de juego (con o sin repeticiones)." << endl;
	cout << "0 - Volver al men� principal." << endl;
	cout << "Opci�n: ";
	return (usi)readInt("Opci�n no valida. Opci�n: ", 0, 5);
}

/** Realiza cambios en la configuraci�n del juego. **/
void changeConfig(tConfig& config) {

	// Mostrar el men� de configuraci�n y leer una opci�n:
	usi opt = configMenu();
	while (opt != 0) {
		cout << endl;
		switch (opt) {
		case 1: // Cambiar n�mero de fichas.
			cout << "- Cambiar n�mero de fichas. -" << endl << endl;
			cout << "Introduce el n�mero de fichas: ";
			config.chips = readInt("El n�mero de fichas debe estar entre 1 y 6. N�mero de fichas: ", 1, 6);
			cout << SAVE_CONFIG_PROMPT << endl << endl;
			break;
		case 2: // Cambiar n�mero m�ximo de intentos.
			cout << "- Cambiar n�mero m�ximo de intentos. -" << endl << endl;
			cout << "Introduce el n�mero m�ximo de intentos: ";
			config.max_tries = readInt("N�mero m�ximo de intentos no v�lido. N�mero m�ximo de intentos: ", 1);
			cout << SAVE_CONFIG_PROMPT << endl << endl;
			break;
		case 3: // Cambiar n�mero m�ximo de pistas.
			cout << "- Cambiar n�mero m�ximo de pistas. -" << endl << endl;
			cout << "Introduce el n�mero m�ximo de pistas: ";
			config.max_hints = readInt("N�mero m�ximo de pistas no v�lido. N�mero m�ximo de pistas: ", 0);
			cout << SAVE_CONFIG_PROMPT << endl << endl;
			break;
		case 4: // Cambiar n�mero m�nimo de intentos entre pistas.
			cout << "- Cambiar n�mero m�nimo de intentos entre pistas. -" << endl << endl;
			cout << "Introduce el n�mero m�nimo de intentos entre pistas: ";
			config.min_tries_btw_hints = readInt("N�mero m�nimo de intentos entre pistas no v�lido. Introduce otro valor: .", 0);
			cout << SAVE_CONFIG_PROMPT << endl << endl;
			break;
		case 5: // Cambiar modo de juego (con o sin repeticiones).
			cout << "- Cambiar modo de juego (con o sin repeticiones). -" << endl << endl;
			config.repts = readBool("�Deseas jugar con repeticiones?", "s", "n");
			cout << SAVE_CONFIG_PROMPT << endl << endl;
			break;
		case 0: // Volver al men� principal.
			cout << "- Volver al men� principal. -" << endl << endl;
			break;
		}

		// Mostrar el men� y leer otra opci�n:
		opt = configMenu();
	}
}

/** Muestra la configuraci�n actual del juego en la consola. **/
void displayConfig(const tConfig& config) {
	cout << "--- MOSTRAR CONFIGURACI�N ACTUAL ---" << endl << endl;
	cout << "N�mero de fichas: " << config.chips << "." << endl;
	cout << "N�mero m�ximo de intentos: " << config.max_tries << "." << endl;
	cout << "N�mero m�ximo de pistas: " << config.max_hints << "." << endl;
	cout << "N�mero m�nimo de intentos entre pistas: " << config.min_tries_btw_hints << "." << endl;
	cout << "Modo de juego: c�digos " << (config.repts ? "con" : "sin") << " repeticiones." << endl;
}

/** Muestra en consola los contenidos del archivo de texto fileName, desde la l�nea i hasta la j (inclusive).
** Devuelve true si el archivo se muestra con �xito, false en caso contrario. **/
bool displayTXTFile(string fileName, unsigned int i, unsigned int j) {
	ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open()) {
		return false;
	}
	else {
		// Imprimir archivo, copiando l�nea a l�nea:
		string buff;
		getline(file, buff);
		for (unsigned int l = 1; l < j && !file.fail(); l++, getline(file, buff)) {
			if (l >= i) {
				cout << buff << endl;
			}
		}
		if (!file.fail()) {
			cout << buff; // �ltima l�nea sin endl.
		}
		file.close();
		return true;
	}
}

/** Muestra en consola los contenidos del archivo de texto fileName.
** Si se lee una l�nea acabada en CENTINEL, ser� la �ltima l�nea que se imprimir�. **/
bool displayTXTFileWCentinel(string fileName, string centinel) {
	ifstream file;
	file.open(fileName.c_str());
	if (!file.is_open()) {
		return false;
	}
	else {
		// Imprimir archivo, copiando l�nea a l�nea:
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

/** Genera una clave de Mastermind en key de chips longitud, admitiendo o no repeticiones dependiendo del valor de repts. **/
void genRndKey(tCodigo key, const tConfig& config) {
	bool chart[COLORS];

	// Inicializar todas las entradas de chart a false.
	for (usi i = 0; i <= COLORS - 1; i++) {
		chart[i] = false;
	}

	usi rnd;

	for (usi i = 0; i <= config.chips - 1; i++) {

		// Generar un n�mero aleatorio entre 0 y COLORS - 1 inclusive:
		rnd = rand() % COLORS;

		if (!config.repts) { // La clave no puede contener repeticiones.
			while (chart[rnd]) { // El color ya se ha usado en la clave.
				rnd = rand() % COLORS; // Generar otro n�mero.
			}
		}

		chart[rnd] = true; // Marcar color como usado.
		key[i] = (tColores)rnd; // Convertir y a�adir a la clave.
	}

}

/** Imprime los identificadores de key a consola, separados por un espacio. **/
void printKey(tCodigo key, const tConfig& config) {
	for (usi i = 0; i < config.chips - 1; i++) {
		cout << toColorId(key[i]) << " ";
	}
	cout << toColorId(key[config.chips - 1]);
}

/** Devuelve el identificador de color. **/
char toColorId(tColores color) {
	char result = 'R';
	switch (color) {
	case Rojo:
		result = 'R';
		break;
	case Azul:
		result = 'A';
		break;
	case Verde:
		result = 'V';
		break;
	case Negro:
		result = 'N';
		break;
	case Granate:
		result = 'G';
		break;
	case Marron:
		result = 'M';
		break;
	}
	return result;
}

/** Devuelve el tColor del identificador id. En caso de no existir el identificador, devuelve Rojo. **/
tColores toColor(char id) {
	if (id == 'r' || id == 'R') {
		return Rojo;
	}
	else if (id == 'a' || id == 'A') {
		return Azul;
	}
	else if (id == 'v' || id == 'V') {
		return Verde;
	}
	else if (id == 'n' || id == 'N') {
		return Negro;
	}
	else if (id == 'g' || id == 'G') {
		return Granate;
	}
	else if (id == 'm' || id == 'M') {
		return Marron;
	}
	else {
		return Rojo;
	}
}

/** Devuelve el string del color asociado a color. **/
string colorToColorName(tColores color) {
	if (color == Rojo) {
		return "rojo";
	}
	else if (color == Azul) {
		return "azul";
	}
	else if (color == Verde) {
		return "verde";
	}
	else if (color == Negro) {
		return "negro";
	}
	else if (color == Granate) {
		return "granate";
	}
	else { // color == Marron
		return "Marron";
	}
}

/** Copia en code los colores asociados a los ids de los chars de string. **/
void totCodigo(string input, tCodigo code, const tConfig& config) {
	for (usi i = 0; i <= config.chips - 1; i++) {
		code[i] = toColor(input[i]);
	}
}

/** Devuelve true si input contiene identificadores correctos, false en otro caso. Se asume que la longitud de input es chips. **/
bool correctKeys(string input, const tConfig& config) {
	bool correctKeys = true;
	for (usi i = 0; i <= config.chips - 1 && correctKeys; i++) {
		if (input[i] != 'r' && input[i] != 'a' && input[i] != 'v' && input[i] != 'n' && input[i] != 'g' && input[i] != 'm'
			&& input[i] != 'R' && input[i] != 'A' && input[i] != 'V' && input[i] != 'N' && input[i] != 'G' && input[i] != 'M') {
			correctKeys = false;
		}
	}
	return correctKeys;
}

/** Pide y lee un c�digo del usuario. Devuelve un estado y copia el c�digo a code si es del tipo correcto. **/
tStatus readCode(tCodigo code, const tConfig& config) {
	cout << PROMPT;
	string input;
	getline(cin, input);
	if (input == "0") {
		return cancel;
	}
	else if (input == "?") {
		return help;
	}
	else if (input == "!") {
		return hint;
	}
	else if (input.length() != config.chips) { // Longitud del c�digo inadecuada.
		return length_err;
	}
	else if (!correctKeys(input, config)) { // Longitud del c�digo adecuada, identificadores incorrectos.
		return key_err;
	}
	else if (!config.repts) { // Longitud del c�digo adecuada, identificadores correctos. Comprobar repeticiones si procede.
		bool chart[COLORS];
		bool flag = false; // Asumir que no hay repeticiones.

		// Inicializar todas las entradas de chart a false.
		for (usi i = 0; i <= COLORS - 1; i++) {
			chart[i] = false;
		}

		for (usi i = 0; i <= config.chips - 1 && !flag; i++) {
			if (chart[toColor(input[i])]) { // Se hab�a le�do el identificador antes.
				flag = true;
			}
			else {
				chart[toColor(input[i])] = true;
			}
		}

		if (flag) {
			return rept_err;
		}
		else {
			totCodigo(input, code, config);
			return good;
		}
	}
	else { // Longitud del c�digo adecuada, identificadores correctos. No hace falta comprobar repeticiones.
		totCodigo(input, code, config);
		return good;
	}
}

/** Calcula en correct_keys y disordered_keys el n�mero de aciertos totales y parciales de code con respecto a key.
** Devuelve true si code y key son id�nticos, false en caso contrario. **/
bool calcPerformance(tCodigo code, tCodigo key, usi &correct_keys, usi &disordered_keys, const tConfig& config) {
	bool chart[MAX_CHIPS]; // El tama�o debe ser constante. Que sea el m�ximo.

	// Inicializar todas las entradas de chart a false.
	for (usi i = 0; i <= config.chips - 1; i++) {
		chart[i] = false;
	}

	// Calcular aciertos totales:
	for (usi i = 0; i <= config.chips - 1; i++) {
		if (code[i] == key[i]) {
			correct_keys++;
			chart[i] = true;
		}
	}

	// Calcular aciertos parciales:
	bool found[MAX_CHIPS]; // El tama�o debe ser constante. Que sea el m�ximo.

	// Inicializar todas las entradas a las que se podr�a acceder de found a false.
	for (usi i = 0; i <= config.chips - 1; i++) {
		found[i] = false;
	}

	bool matched;
	for (usi i = 0; i <= config.chips - 1; i++) {
		matched = false;
		for (usi j = 0; j <= config.chips - 1 && !chart[i] && !matched; j++) {
			if (key[i] == code[j] && !chart[j] && !found[j]) {
				disordered_keys++;
				found[j] = true;
				matched = true;
			}
		}
	}

	// Return:
	if (correct_keys == config.chips) {
		return true;
	}
	else {
		return false;
	}
}

/** Muestra un mensaje en la consola indicando el n�mero de aciertos totales y parciales
** (colores del codigo que est�n en la clave, pero descolocados), as� como el n�mero de intentos. **/
void printPerformanceMsg(tCodigo code, usi correct_keys, usi disordered_keys, usi tries, usi score, const tConfig& config) {
	cout << "   " << tries << ": ";
	printKey(code, config);
	cout << right << setfill(' ') << setw(8) << correct_keys << " �" << setw(6) << disordered_keys << " ~" << setw(6) << score << " punto" << (score == 0 || score > 1 ? "s" : "") << endl;
}

/** Imprime en la consola una pista acerca de la clave. El sistema de pistas es el rudimentario de la especificaci�n. **/
void printHint(tCodigo key, const tConfig& config) {

	// Generar un n�mero aleatorio entre 0 y chips - 1 inclusive:
	usi rnd = rand() % config.chips;

	// Imprimir pista:
	cout << "El color en la posici�n " << rnd + 1 << " es: " << colorToColorName(key[rnd]) << ".";

}

/** Imprime el mensaje correspondiente al estado status. No se contemplan los status good ni hint. **/
void manageStatus(tStatus status, const tConfig& config) {
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
		cout << "La longitud de los c�digos debe ser de " << config.chips << " caracter";
		cout << (config.chips > 1 ? "es" : "") << ".";
		cout << endl;
		break;
	case key_err:
		cout << "Los c�digos s�lo pueden tener ciertos colores (? para consultar los permitidos).";
		cout << endl;
		break;
	case rept_err:
		cout << "El modo de juego actual no admite c�digos con colores repetidos.";
		cout << endl;
		break;
	}
}

/** Calcula la puntuaci�n agregada de un jugador. **/
usi calcScore(usi score, usi correct_keys, usi disordered_keys, bool won) {
	return score += disordered_keys + 5 * correct_keys + (won ? 100 : 0);
}

/** Conduce el desarrollo de una partida de Mastermind. Devuelve el n�mero de intentos empleados
** por el jugador (max_tries si no la acert�). Devuelve 0 si se selecciona la opci�n de salir, independientemente
** del n�mero de intentos del jugador hasta ese momento. **/
usi playMastermind(string user, const tConfig& config) {
	cout << "--- NUEVA PARTIDA ---" << endl << endl;
	tCodigo key;
	genRndKey(key, config);
	// **************** <DEBUG> ****************
	//cout << "La clave es: ";
	//printKey(key, config);
	//cout << endl;
	// **************** </DEBUG> ****************
	tStatus status;
	tCodigo code;
	unsigned score = 0;
	usi tries = 0;
	short int lastHint = -1; // Representa el intento en el cual se proporcion� la �ltima pista.
	// -1 indica que a�n no se ha proporcionado ninguna pista.
	usi hints = 0; // Representa la cantidad de pistas que se han proporcionado.
	usi correct_keys = 0;
	usi disordered_keys = 0;
	bool won = false;
	do {
		status = readCode(code, config);
		if (status == good) { // Se ha le�do un c�digo de tipo correcto.

			// Reinicializar contadores:
			correct_keys = 0;
			disordered_keys = 0;

			// Calcular intentos, aciertos, puntuaci�n e imprimir mensaje de rendimiento.
			tries++;
			won = calcPerformance(code, key, correct_keys, disordered_keys, config);
			score = calcScore(score, correct_keys, disordered_keys, won);
			printPerformanceMsg(code, correct_keys, disordered_keys, tries, score, config);
		}
		else if (status == hint) {
			if (tries == 0) {
				cout << "Consume al menos un intento para solicitar una pista.";
			}
			else if (hints == config.max_hints) {
				cout << "Se ha agotado el n�mero m�ximo de pistas (" << hints << ").";
			}
			else if (tries - lastHint < config.min_tries_btw_hints && lastHint != -1) {
				cout << "S�lo se puede solicitar una pista tras " << config.min_tries_btw_hints;
				cout << " intento" << (config.min_tries_btw_hints > 1 ? "s" : "") << " desde que se dio la �ltima.";
			}
			else { // Se satisfacen los requisitos para solicitar una pista.
				lastHint = tries;
				hints++;
				printHint(key, config);
			}
			cout << endl;
		}
		else { // El c�digo le�do no era correcto, o se seleccion� la opci�n de ayuda o la de salir. Imprimir el mensaje que corresponda.
			manageStatus(status, config);
		}
	} while (tries < config.max_tries && !won && status != cancel);

	// Preguntar por la raz�n de salida del bucle.
	if (won) { // El jugador ha acertado la clave.
		cout << "�ENHORABUENA! Has ganado en " << tries << " intento" << (tries > 1 ? "s" : "") << "." << endl;
		if (!updateScore(user, won, score)) cout << "Desafortunadamente, no se ha podido guardar tu puntuaci�n por un error inesperado." << endl;
	}
	else if (tries == config.max_tries) { // El jugador no acert� la clave en el intento TRIES.
		cout << "Se te han acabado los " << tries << " intentos. La clave era: ";
		printKey(key, config);
		cout << "." << endl;
		if (!updateScore(user, won, score)) cout << "No se ha podido guardar tu puntuaci�n por un error inesperado." << endl;
	}
	else { // El jugador ha abandonado el juego.
		tries = 0;
	}
	return tries;
}

/** Solicita y devuelve el nombre introducido por el jugador.
** El nombre debe ser una palabra de entre CENTINEL.length() + 1 y MAX_USER_LENGTH caracteres. **/
string getUserName() {
	string user;
	cout << "Bienvenido a Mastermind. Por favor, introduce tu nombre: ";
	getline(cin, user);
	while (user.length() < CENTINEL.length() + 1 || user.find(" ") != -1 || user.length() > 20) {

		// Distinguir entre los casos de error:
		if (user.length() < CENTINEL.length() + 1) {
			cout << "Los nombres no pueden tener menos de " << CENTINEL.length() + 1 << " caracteres." << endl;
		}
		else if (user.find(" ") != -1) {
			cout << "Los nombres no pueden tener espacios." << endl;
		}
		else {
			cout << "Los nombres no pueden tener m�s de " << MAX_USER_LENGTH << " caracteres." << endl;
		}
		cout << "Por favor, introduce tu nombre: ";
		getline(cin, user);
	}
	return user;
}

/** Actualiza los datos de user en el archivo de texto USER_FILE con las nuevas puntuaciones.
** Si user no se encuentra no har� nada.
** Devuelve true si user se encuentra en el archivo, false si no se encuentra
** o si no se puede acceder al archivo. **/
bool setScore(string user, tScore score) {
	bool found = false;
	fstream file;
	file.open(USER_FILE, ios::app); if (file.is_open()) file.close(); // Si el archivo no existe, fstream solo lo crea si se abre s�lo como de escritura.
	file.open(USER_FILE, ios::in | ios::out); // Abre el archivo en modo lectura y escritura.
	if (file.is_open()) {
		string buffUser;
		file >> ws;
		do {
			file >> buffUser;
			if (buffUser == user) {
				found = true;
				file >> ws;
				file.seekp(file.tellg());
				for (usi i = 0; i <= USER_FILE_COLS - 2; i++) {
					file << unsignedToString(score[i], '0', false) << " ";
				}
			}
			else { // Se est� leyendo el nombre de otro usuario. Descartar sus datos asociados.
				for (usi i = 1; i <= USER_FILE_COLS - 1 && !file.eof() && !file.fail(); i++) {
					file >> buffUser;
				}
			}
		} while (!file.eof() && !file.fail() && !found && buffUser != CENTINEL);
		file.close();
	}
	return found;
}

/** Copia en score los datos asociados a user en el archivo USER_FILE.
** Si user no se encuentra no har� nada.
** Devuelve true si user se encuentra en el archivo, false si no se encuentra
** o si no se puede acceder al archivo. **/
bool getScore(string user, tScore score) {
	bool found = false;
	ifstream file;
	file.open(USER_FILE);
	if (file.is_open()) {
		string buffUser;
		unsigned buffScore;
		file >> ws;
		do {
			file >> buffUser;
			if (buffUser == user) {
				found = true;
				for (usi i = 0; i <= USER_FILE_COLS - 2; i++) {
					file >> ws;
					file >> buffScore;
					score[i] = buffScore;
				}
			}
			else { // Se est� leyendo el nombre de otro usuario. Descartar sus tres datos asociados.
				for (usi i = 0; i < USER_FILE_COLS - 1 && !file.eof() && !file.fail(); i++) {
					file >> buffUser;
				}
			}
		} while (!file.eof() && !file.fail() && !found && buffUser != CENTINEL);
		file.close();
	}
	return found;
}

/** Actualiza la puntuaci�n de user en USER_FILE. Si user no existe, se le inscribe con su puntuaci�n. **/
/** Devuelve true si tiene �xito, false si no lo tiene o si user excede la m�xima puntuaci�n
** (en cuyo caso se le asigna MAX_SCORE como puntuaci�n) . **/
bool updateScore(string user, bool won, unsigned score) {
	tScore newScore;
	if (!getScore(user, newScore)) { // user no se encuentra en el archivo o no se puede acceder.
		ofstream file;
		file.open(USER_FILE, ios::in | ios::ate); // Coloca el cursor al final. Aunque no se vaya a leer, ios::in es necesario
		// para prevenir que el archivo se trunque.
		if (!file.is_open()) { // No se pod�a abrir el archivo.
			return false;
		}
		else { // S� se abri� el archivo, pero no se encontr� user.
			streampos pos = file.tellp();
			file.seekp((long)pos - CENTINEL.length()); // Coloca el cursor justo delante del centinela.

			// Inscribir a user en el archivo y a�adir su puntuaci�n.
			file << user.insert(user.length(), MAX_USER_LENGTH + 1 - user.length(), ' '); // Escribe un string de MAX_USER_LENGTH + 1, siendo los
			// primeros user y el resto espacios.
			file << unsignedToString(1, '0', false) << " "; // 0000000001 partidas jugadas.
			file << unsignedToString(won, '0', false) << " "; // 0000000000 o 0000000001 partidas ganadas.
			file << unsignedToString(score, '0', false) << " ";

			// Reescribir el centinela.
			file << '\n';
			file << CENTINEL;

			file.close();
			return true;
		}
	}
	else {
		if (newScore[2] + score > MAX_SCORE) { // user excedi� la m�xima puntuaci�n. Asignarle MAX_SCORE y devolver false.
			// Se dar� cuenta de por qu� no se pudo guardar la puntuaci�n cuando mire la tabla.
			newScore[2] = MAX_SCORE;
			newScore[0]++; if (won) newScore[1]++;
			setScore(user, newScore);
			return false;
		}
		else {
			newScore[0]++; newScore[2] += score; if (won) newScore[1]++;
			if (!setScore(user, newScore)) {
				return false;
			}
			else {
				return true;
			}
		}
	}
}

/** Convierte de unsigned a string, a�adiendo tantos filler por la izquierda o por la derecha como sean necesarios.
** side = true por la derecha; side = false por la izquierda;
** Siempre devuelve un string de 10 caracteres (rango de unsigned: 0 a 4,294,967,295). **/
string unsignedToString(unsigned num, char filler, bool side) {
	string text = to_string(num);
	usi pos;
	if (side) {
		pos = text.length();
	}
	else {
		pos = 0;
	}
	text.insert(pos, 10 - text.length(), filler);
	return text;
}

/** Muestra en consola las puntuaciones asociadas a todos los usuarios de USER_FILE. **/
bool displayLeaderBoard() {
	ifstream file;
	file.open(USER_FILE);
	if (!file.is_open()) {
		return false;
	}
	else {
		cout << setfill(' ') << left << setw(21) << "Usuario" << setw(12) << "Juegos" << setw(12) << "Ganados" << setw(12) << "Puntuaci�n" << endl;
		cout << setfill('-') << setw(58) << '-' << endl;
		string buffUser;
		unsigned buffScore;
		file >> buffUser;
		while (buffUser != CENTINEL || file.fail()) {
			cout << setfill(' ') << setw(MAX_USER_LENGTH + 1) << buffUser << "| "; // Imprimir el nombre.
			for (usi i = 0; i <= USER_FILE_COLS - 2; i++) { // Imprimir las puntuaciones asociadas.
				file >> buffScore;
				cout << unsignedToString(buffScore, ' ', true) << "| ";
			}
			cout << endl;
			file >> buffUser;
		}
		cout << setfill('-') << setw(58) << '-' << endl;
		return true;
	}
}