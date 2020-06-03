#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <sstream>
#include <conio.h>
#include <fstream>
#pragma warning(disable: 4996)
using namespace std;

void start_menu(SOCKET Connection);
bool administrator_auth(SOCKET Connection);
string user_auth(SOCKET Connection);
void create_new_user(SOCKET Connection);
bool is_login_free(SOCKET Connection, string login);
void administrator_menu(SOCKET Connection);
void user_menu(SOCKET Connection, string workon_user_login);
void actions_with_users_menu(SOCKET Connection);
void edit_administrator_password(SOCKET Connection);
void edit_user_password(SOCKET Connection, string workon_user_login);
void show_all_users(SOCKET Connection);
void delete_user(SOCKET Connection);
void task_menu(SOCKET Connection, string auth_role, string workon_user_login);
void see_station_info(SOCKET Connection);
void edit_station_info(SOCKET Connection);
void see_initial_values(SOCKET Connection);
void edit_initial_values(SOCKET Connection);
void task_result(SOCKET Connection);

SOCKET Connection;

int main(int argc, char* argv[]) {
	// Русификация
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	// Подключение к серверу
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Ошибка." << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0) {
		cout << "Ошибка: невозможно присоединиться к серверу.\n";
		Sleep(2000);
		return 1;
	}
	else {
		cout << "Присоединен к серверу." << endl;
		Sleep(3000);
	}

	// Отображение стартового меню
	start_menu(Connection);
	return 0;
}

// Стартовое меню
void start_menu(SOCKET Connection) {
	string workon_user_login;
	while (true) {
		system("cls");
		cout << "1. Администратор\n2. Пользователь\n3. Создать пользователи\n0. Выйти из программы\n	~ Выбор: ";
		int ch = 0;
		while (!(cin >> ch) || (cin.peek() != '\n')) {
			system("cls");
			cin.clear();
			while (cin.get() != '\n');
			cout << "1. Администратор\n2. Пользователь\n3. Создать пользователя\n0. Выйти из программы\n	~ Выбор: ";
		}
		string ex = "exit";
		switch (ch) {
		case 1:
			system("cls");
			if (administrator_auth(Connection)) {
				administrator_menu(Connection);
			}
			break;
		case 2:
			system("cls");
			workon_user_login = user_auth(Connection);
			if (workon_user_login != "false") {
				user_menu(Connection, workon_user_login);
			}
			break;
		case 3:
			system("cls");
			create_new_user(Connection);
			break;
		case 0:
			system("cls");
			cout << "Спасибо за работу!" << endl;
			send(Connection, ex.c_str(), ex.length()+1, NULL);
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
		}
	}
}

bool administrator_auth(SOCKET Connection) {
	system("cls");
	string password;
	cout << "Пароль: ";
	cin >> password;
	system("cls");
	string on_send = "administrator_auth_password";
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL);		Sleep(50);
	send(Connection, password.c_str(), password.length() + 1, NULL);	Sleep(50);
	char msg[256];
	recv(Connection, msg, sizeof(msg), NULL);
	system("cls");
	cout << " Подождите... ";
	Sleep(500);
	string response; 
	stringstream ss; ss << msg; ss >> response;
	if (response == "auth") {
		cout << "OK" << endl;
		Sleep(1000);
		return true;
	}
	else {
		cout << "Неверный пароль." << endl;
		Sleep(1000);
		return false;
	}
}

string user_auth(SOCKET Connection) {
	system("cls");
	string login, password;
	cout << "Логин: ";
	cin >> login;
	cout << "Пароль: ";
	cin >> password;
	system("cls");
	string on_send = "user_auth_password";
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL);		Sleep(50);
	send(Connection, login.c_str(), login.length() + 1, NULL);			Sleep(50);
	send(Connection, password.c_str(), password.length() + 1, NULL);	Sleep(50);
	char msg[256];
	recv(Connection, msg, sizeof(msg), NULL);
	system("cls");
	cout << " Подождите... ";
	Sleep(500);
	string response;
	stringstream ss; ss << msg; ss >> response;
	if (response == "auth") {
		cout << "OK" << endl;
		Sleep(1000);
		return login;
	}
	else {
		cout << "Неверный логин или пароль." << endl;
		Sleep(1500);
		return "false";
	}
}

void create_new_user(SOCKET Connection) {
	system("cls");
	string login, password, confirm_password;
	cout << "Укажите информацию ниже." << endl;
	cout << " Логин: "; cin >> login;
	cout << " Пароль: "; cin >> password;
	cout << " Подтвердите пароль: "; cin >> confirm_password;
	system("cls");
	cout << "Подождите... ";
	if (is_login_free(Connection, login)) {
		if (password == confirm_password) {
			string on_send = "create_new_user";
			send(Connection, on_send.c_str(), on_send.size() + 1, NULL); Sleep(50);
			send(Connection, login.c_str(), login.size() + 1, NULL); Sleep(50);
			send(Connection, password.c_str(), password.size() + 1, NULL); Sleep(50);
			cout << "OK" << endl;
			Sleep(850);
			return;
		}
		else {
			cout << "Подтверждение пароля не пройдено." << endl;
			Sleep(1500);
			return;
		}
	}
	else {
		cout << "Логин уже существует." << endl;
		Sleep(1500);
		return;
	}
}

bool is_login_free(SOCKET Connetction, string login) {
	string on_send = "is_login_free";
	send(Connection, on_send.c_str(), on_send.size() + 1, NULL); Sleep(50);
	send(Connection, login.c_str(), login.size() + 1, NULL);

	string response;
	char msg[256];
	recv(Connection, msg, sizeof(msg), NULL);
	stringstream ss; ss << msg; ss >> response;

	if (response == "free") {
		return true;
	}
	else {
		return false;
	}
}

void administrator_menu(SOCKET Connection) {
	while (true) {
		system("cls");
		cout << "1. Действия с пользователями\n2. Расчет\n3. Изменить пароль\n0. Назад\n	~ Выбор: ";
		int ch = 0;
		while (!(cin >> ch) || (cin.peek() != '\n')) {
			system("cls");
			cin.clear();
			while (cin.get() != '\n');
			cout << "1. Действия с пользователями\n2. Расчет\n3. Изменить пароль\n0. Назад\n	~ Выход: ";
		}
		switch (ch) {
		case 1:
			system("cls");
			actions_with_users_menu(Connection);
			Sleep(1000);
			break;
		case 2:
			system("cls");
			task_menu(Connection, "admin", "");
			Sleep(1000);
			break;
		case 3:
			system("cls");
			edit_administrator_password(Connection);
			Sleep(1000);
			break;
		case 0:
			start_menu(Connection);
			break;
		default:
			break;
		}
	}
}

void actions_with_users_menu(SOCKET Connection) {
	while (true) {
		system("cls");
		cout << "1. Показать доступных\n2. Удалить\n3. Создать\n0. Назад\n	~ Выбор: ";
		int ch = 0;
		while (!(cin >> ch) || (cin.peek() != '\n')) {
			system("cls");
			cin.clear();
			while (cin.get() != '\n');
			cout << "1. Показать доступных\n2. Удалить\n3. Создать\n0. Назад\n	~ Выбор: ";
		}
		switch (ch) {
		case 1:
			system("cls");
			show_all_users(Connection);
			break;
		case 2:
			system("cls");
			delete_user(Connection);
			Sleep(1000);
			break;
		case 3:
			system("cls");
			create_new_user(Connection);
			Sleep(1000);
			break;
		case 0:
			administrator_menu(Connection);
			break;
		default:
			break;
		}
	}
}

void user_menu(SOCKET Connection, string workon_user_login) {
	while (true) {
		system("cls");
		cout << "1. Расчет\n2. Изменить пароль\n0. Назад\n	~ Выбор: ";
		int ch = 0;
		while (!(cin >> ch) || (cin.peek() != '\n')) {
			system("cls");
			cin.clear();
			while (cin.get() != '\n');
			cout << "1. Расчет\n2. Изменить пароль\n0. Назад\n	~ Выбор: ";
		}
		switch (ch) {
		case 1:
			system("cls");
			task_menu(Connection, "user", workon_user_login);
			Sleep(1000);
			break;
		case 2:
			system("cls");
			edit_user_password(Connection, workon_user_login);
			Sleep(1000);
			break;
		case 0:
			start_menu(Connection);
			break;
		default:
			break;
		}
	}
}

void edit_administrator_password(SOCKET Connection) {
	system("cls");
	string password, confirm_password;
	cout << "Укажите новый пароль: ";
	cin >> password;
	cout << "Подтвердите новый пароль: ";
	cin >> confirm_password;
	system("cls");
	cout << "Подождите... ";
	Sleep(500);
	if (password == confirm_password) {
		string on_send = "edit_administrator_password";
		send(Connection, on_send.c_str(), on_send.length() + 1, NULL);	Sleep(50);
		send(Connection, password.c_str(), password.length() + 1, NULL); Sleep(50);
		cout << "OK" << endl;
		Sleep(900);
		return;
	}
	else {
		cout << "Подтверждение пароля не пройдено." << endl;
		Sleep(1000);
		return;
	}
}

void edit_user_password(SOCKET Connection, string workon_user_login) {
	system("cls");
	string password, confirm_password;
	cout << "Укажите новый пароль: ";
	cin >> password;
	cout << "Подтвердите пароль: ";
	cin >> confirm_password;
	system("cls");
	cout << "Подождите... ";
	Sleep(500);
	if (password == confirm_password) {
		string on_send = "edit_user_password";
		send(Connection, on_send.c_str(), on_send.length() + 1, NULL);	Sleep(50);
		send(Connection, workon_user_login.c_str(), workon_user_login.length() + 1, NULL); Sleep(50);
		send(Connection, password.c_str(), password.length() + 1, NULL); Sleep(50);
		cout << "OK" << endl;
		Sleep(900);
		return;
	}
	else {
		cout << "Подтверждение пароля не пройдено." << endl;
		Sleep(1000);
		return;
	}
}

void show_all_users(SOCKET Connection) {
	char msg[256];
	string on_send = "show_all_users", recieved;
	stringstream ss;
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL);	Sleep(50);
	int count = 0;
	while (recv(Connection, msg, sizeof(msg), NULL)) {
		count++;
		ss.clear(); ss << msg; ss >> recieved;
		if (recieved == "end") {
			Sleep(500);
			cout << " Нажмите любую кнопку чтобы продолжить..." << endl;
			_getch();
			return;
		}
		if (count % 2 != 0) {
			cout << "Логин: " << recieved << ". Пароль: ";
		}
		else {
			cout << recieved << "." << endl;
		}
	}
}

void delete_user(SOCKET Connection) {
	char msg[256];
	string on_send = "delete_user", recieved;
	stringstream ss;
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL);	Sleep(50);
	int count = 0;
	while (recv(Connection, msg, sizeof(msg), NULL)) {
		count++;
		ss.clear(); ss << msg; ss >> recieved;
		if (recieved == "end") {
			break;
		}
		if (count % 2 != 0) {
			cout << "Логин: " << recieved << endl;
		}
	}
	cout << " Введите логин пользователя для удаления: ";
	string user_login; cin >> user_login;
	system("cls");
	cout << "Подождите... ";
	Sleep(500);
	send(Connection, user_login.c_str(), user_login.length() + 1, NULL); Sleep(100);
	recv(Connection, msg, sizeof(msg), NULL);
	string result;
	stringstream ss1; ss1 << msg; ss1 >> result;
	if (result == "good") {
		cout << "OK" << endl;
		Sleep(1000);
		return;
	}
	else {
		cout << "Пользователь не найден.";
		Sleep(1000);
		return;
	}
}

void task_menu(SOCKET Connection, string auth_role, string workon_user_login) {
	while (true) {
		system("cls");
		cout << "1. Просмотр информации о станции\n2. Указать информацию о станции\n3. Просмотр информации для расчета\n4. Указать информацию для расчета\n5. Получить результаты\n0. Назад\n	~ Выбор: ";
		int ch = 0;
		while (!(cin >> ch) || (cin.peek() != '\n')) {
			system("cls");
			cin.clear();
			while (cin.get() != '\n');
			cout << "1. Просмотр информации о станции\n2. Указать информацию о станции\n3. Просмотр информации для расчета\n4. Указать информацию для расчета\n5. Получить результаты\n0. Назад\n	~ Выбор: ";
		}
		switch (ch) {
		case 1:
			system("cls");
			see_station_info(Connection);
			Sleep(1000);
			break;
		case 2:
			system("cls");
			edit_station_info(Connection);
			Sleep(1000);
			break;
		case 3:
			system("cls");
			see_initial_values(Connection);
			Sleep(1000);
			break;
		case 4:
			system("cls");
			edit_initial_values(Connection);
			Sleep(1000);
			break;
		case 5:
			system("cls");
			task_result(Connection);
			Sleep(1000);
			break;
		case 0:
			if (auth_role == "user") {
				user_menu(Connection, workon_user_login);
			}
			else {
				administrator_menu(Connection);
			}
			break;
		default:
			break;
		}
	}
}

void see_station_info(SOCKET Connection) {
	system("cls");
	string on_send = "station_info";
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL); Sleep(50);

	char msg[256];
	recv(Connection, msg, sizeof(msg), NULL);
	stringstream ss; ss << msg; string rec; ss >> rec;
	
	if (rec == "set") {
		char msg1[256];
		recv(Connection, msg1, sizeof(msg1), NULL);
		stringstream ss1; ss1 << msg1; string number_of_buses; ss1 >> number_of_buses;

		char msg2[256];
		recv(Connection, msg2, sizeof(msg2), NULL);
		stringstream ss2; ss2 << msg2; string bus_capacity; ss2 >> bus_capacity;

		char msg3[256];
		recv(Connection, msg3, sizeof(msg3), NULL);
		stringstream ss3; ss3 << msg3; string capacity_percent; ss3 >> capacity_percent;

		char msg4[256];
		recv(Connection, msg4, sizeof(msg4), NULL);
		stringstream ss4; ss4 << msg4; string total_capacity; ss4 >> total_capacity;


		cout << "Количество автобусов - " << number_of_buses << " автобусов\nВместимость автобуса - " << bus_capacity << " человек\nСуммарная вместимость - " << total_capacity << " людей\nНеобходимый процент загруженности - " << capacity_percent << "%" << endl;
		Sleep(2500);
	}
	else {
		cout << "Информация о станции не указана";
		Sleep(1000);
		return;
	}
}

void edit_station_info(SOCKET Connection) {
	system("cls");
	string on_send = "set_station_info";
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL);	Sleep(50);
	int number_of_buses, bus_capacity, capacity_percent;
	cout << "Количество автобусов: ";
	while (!(cin >> number_of_buses) || (cin.peek() != '\n')) {
		cin.clear();
		while (cin.get() != '\n');
		cout << "Количество автобусов: ";
	}
	send(Connection, to_string(number_of_buses).c_str(), to_string(number_of_buses).length() + 1, NULL);	Sleep(50);
	cout << "Вместимость автобусов: ";
	while (!(cin >> bus_capacity) || (cin.peek() != '\n')) {
		cin.clear();
		while (cin.get() != '\n');
		cout << "Вместимость автобусов: ";
	}
	send(Connection, to_string(bus_capacity).c_str(), to_string(bus_capacity).length() + 1, NULL);	Sleep(50);
	cout << "Необходимый процент загруженности: ";
	while (!(cin >> capacity_percent) || (cin.peek() != '\n')) {
		cin.clear();
		while (cin.get() != '\n');
		cout << "Необходимый процент загруженности: ";
	}
	send(Connection, to_string(capacity_percent).c_str(), to_string(capacity_percent).length() + 1, NULL);	Sleep(50);
	cout << " Информация устанавливается... ";
	Sleep(500);
	cout << "OK" << endl;
	Sleep(500);
	return;
}

void see_initial_values(SOCKET Connection) {
	system("cls");
	string on_send = "see_initial_values";
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL);	Sleep(50);
	char msg[256];
	recv(Connection, msg, sizeof(msg), NULL);
	stringstream ss; ss << msg; string recieved; ss >> recieved;

	if (recieved == "set") {
		for (int i = 0; i < 3; i++) {
			cout << "  " << i + 1 << " неделя:" << endl;
			char msg1[256], msg2[256], msg3[256];
			recv(Connection, msg1, sizeof(msg1), NULL);
			stringstream ss1; ss1 << msg1; string buses_on_road; ss1 >> buses_on_road;
			cout << "Выпущенные на дорогу автобусы - " << buses_on_road << endl;
			recv(Connection, msg2, sizeof(msg2), NULL);
			stringstream ss2; ss2 << msg2; string passenger_traffic; ss2 >> passenger_traffic;
			cout << "Ежедневный поток пассажиров -   " << passenger_traffic << endl;
			recv(Connection, msg3, sizeof(msg3), NULL);
			stringstream ss3; ss3 << msg3; string percent; ss3 >> percent;
			cout << "Процент загруженности -         " << percent << endl;
		}
		_getch();
		return;
	}
	else {
		cout << "Укажите информацию о станции.";
		Sleep(500);
		return;
	}
}

void edit_initial_values(SOCKET Connection) {
	system("cls");
	string on_send = "edit_initial_values";
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL);	Sleep(50);
	char msg[256];
	recv(Connection, msg, sizeof(msg), NULL);
	stringstream ss; ss << msg; string recieved; ss >> recieved;

	if (recieved == "set") {
		for (int i = 0; i < 3; i++) {
			cout << "  " << i + 1 << " Неделя:" << endl;
			int buses_on_road, passenger_traffic;
			cout << "Выпущенные на дорогу автобусы:     ";
			while (!(cin >> buses_on_road) || (cin.peek() != '\n')) {
				cin.clear();
				while (cin.get() != '\n');
				cout << "Выпущенные на дорогу автобусы:     ";
			}
			send(Connection, to_string(buses_on_road).c_str(), to_string(buses_on_road).length() + 1, NULL);	Sleep(50);
			cout << "Ежедневный поток пассажиров: ";
			while (!(cin >> passenger_traffic) || (cin.peek() != '\n')) {
				cin.clear();
				while (cin.get() != '\n');
				cout << "Ежедневный поток пассажиров: ";
			}
			send(Connection, to_string(passenger_traffic).c_str(), to_string(passenger_traffic).length() + 1, NULL);	Sleep(50);
		}
		Sleep(2000);
		return;
	}
	else {
		cout << "Укажите информацию о станции";
		Sleep(500);
		return;
	}
}

void task_result(SOCKET Connection) {
	system("cls");
	string on_send = "task_result";
	send(Connection, on_send.c_str(), on_send.length() + 1, NULL);	Sleep(50);
	char msg[256];
	recv(Connection, msg, sizeof(msg), NULL);
	stringstream ss; ss << msg; string recieved; ss >> recieved;

	if (recieved == "set") {
		recv(Connection, msg, sizeof(msg), NULL);
		string message; stringstream ss1; ss << msg; ss >> message;
		if (message == "too_much") {
			cout << "В наличии недостаточно автобусов" << endl;
			Sleep(2000);
			return;
		}
		else {
			char msg1[256], msg2[256], msg3[256], msg4[256];
			recv(Connection, msg1, sizeof(msg1), NULL);
			stringstream ss4; ss4 << msg1; string result_buses; ss4 >> result_buses;
			recv(Connection, msg2, sizeof(msg2), NULL);
			stringstream ss2; ss2 << msg2; string min_percent; ss2 >> min_percent;
			recv(Connection, msg3, sizeof(msg3), NULL);
			stringstream ss3; ss3 << msg3; string max_percent; ss3 >> max_percent;
			recv(Connection, msg4, sizeof(msg4), NULL);
			stringstream ss5; ss5 << msg4; string capacity; ss5 >> capacity;
			int min_percent_int, max_percent_int, capacity_int;
			istringstream(min_percent) >> min_percent_int;
			istringstream(max_percent) >> max_percent_int;
			istringstream(capacity) >> capacity_int;
			
			string result_1 = "Чтобы загруженность стала равна " + to_string(capacity_int) + "% " + result_buses + " автобусов должно быть запущено.";
			string result_2 = "Процент может меняться в рамках от " + to_string(min_percent_int) + "% до " + to_string(max_percent_int) + "%";
			cout << result_1 << endl << result_2 << endl;

			ofstream result;
			result.open("results.txt");
			result << result_1 << endl << result_2 << endl;
			result.close();
			
			_getch();
			return;
		}
	}
	else {
		cout << "Укажите информацию о станции и для расчетов.\n";
		Sleep(500);
		return;
	}
}