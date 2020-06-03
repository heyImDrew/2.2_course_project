#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#pragma warning(disable: 4996)
using namespace std;

const char *IP		=	"127.0.0.1";
int			PORT	=	1111;
SOCKET Connections[100];
int Counter = 0;

void server_auth();
void get_information_from_files();
void user_disconnect();
void administrator_auth_password(int index);
void user_auth_login_password(int index);
void is_login_free(int index);
void create_new_user(int index);
void save_information_in_files();
void edit_administrator_password(int index);
void edit_user_password(int index);
void show_all_users(int index);
void delete_user(int index);
void station_info(int index);
void set_station_info(int index);
void see_initial_values(int index);
void edit_initial_values(int index);
void task_result(int index);

template<typename T>
class Server {
private:
	string server_name;
	T password;
public:
	Server() {
		this->server_name = "admin";
		this->password = "admin";
	}
	void set_server_name(string server_name) {
		this->server_name = server_name;
	}
	void set_server_password(T password) {
		this->password = password;
	}
	bool is_authorized(string server_name, T password) {
		if (this->server_name == server_name && this->password == password) {
			return true;
		}
		else {
			return false;
		}
	}
};
Server<string> server_info;

class DefaultUser {
private:
	string login;
	string password;
public:
	void set_password(string password) {
		this->password = password;
	}
	void set_login(string login) {
		this->login = login;
	}
	string get_login() {
		return this->login;
	}
	string get_password() {
		return this->password;
	}
};

class Administrator : public DefaultUser {
private:
public:
	Administrator() {
		this->set_password("admin");
	}
	bool is_authorized(string password) {
		if (this->get_password() == password) {
			return true;
		}
		else {
			return false;
		}
	}
};
Administrator administrator;

class User : public DefaultUser {
private:
public:
	User() {
		this->set_login("default");
		this->set_password("1111");
	}
	User(string login, string password) {
		this->set_login(login);
		this->set_password(password);
	}
	bool is_authorized(string login, string password) {
		if (this->get_login() == login && this->get_password() == password) {
			return true;
		}
		else {
			return false;
		}
	}
};
vector<User> users;

class Station {
private:
	string number_of_buses;
	string bus_capacity;
	string total_capacity;
	string capacity_percent;
	bool set;
public:
	Station() {
		number_of_buses = "";
		bus_capacity = "";
		total_capacity = "";
		capacity_percent = "";
		set = false;
	};
	void set_information(string number_of_buses, string bus_capacity, string capacity_percent) {
		this->number_of_buses = number_of_buses;
		this->bus_capacity = bus_capacity;
		int number_of_buses_int, bus_capacity_int;
		istringstream(number_of_buses) >> number_of_buses_int;
		istringstream(bus_capacity) >> bus_capacity_int;
		this->total_capacity = to_string(number_of_buses_int * bus_capacity_int);
		this->capacity_percent = capacity_percent;
		this->set = true;
	}
	string get_number_of_buses() {
		return this->number_of_buses;
	}
	string get_bus_capacity() {
		return this->bus_capacity;
	}
	string get_total_capacity() {
		return this->total_capacity;
	}
	string get_capacity_percent() {
		return this->capacity_percent;
	}
	bool is_set() {
		return set;
	}
	void free() {
		this->number_of_buses = "";
		this->bus_capacity = "";
		this->total_capacity = "";
		this->capacity_percent = "";
		this->set = false;
	}
};
Station station;

class WeekReport {
private:
	Station week_station;
	string buses_on_road;
	string passenger_traffic;
	string percent;
	bool set;
public:
	WeekReport() {
		this->week_station = station;
		buses_on_road = "Пусто";
		passenger_traffic = "Пусто";
		percent = "Пусто";
		set = false;
	}
	void set_station(Station temp) {
		this->week_station = station;
	}
	void set_information(string buses_on_road, string passenger_traffic, string percent) {
		this->buses_on_road = buses_on_road;
		this->passenger_traffic = passenger_traffic;
		this->percent = percent;
		set = true;
	}
	Station get_station() {
		return this->week_station;
	}
	string get_buses_on_road() {
		return this->buses_on_road;
	}
	string get_passenger_traffic() {
		return this->passenger_traffic;
	}
	string get_percent() {
		return this->percent;
	}
	bool is_set() {
		return set;
	}
};
vector<WeekReport> week_reports;

void ClientHandler(int index) {
	while (true) {
		char msg[256];
		recv(Connections[index], msg, sizeof(msg), NULL);
		string message;
		stringstream ss; ss << msg; ss >> message;
		if (message == "exit") {
			user_disconnect();
			save_information_in_files();
			return;
		}
		if (message == "administrator_auth_password") {
			administrator_auth_password(index);
		}
		if (message == "user_auth_password") {
			user_auth_login_password(index);
		}
		if (message == "is_login_free") {
			is_login_free(index);
		}
		if (message == "create_new_user") {
			create_new_user(index);
		}
		if (message == "edit_administrator_password") {
			edit_administrator_password(index);
		}
		if (message == "edit_user_password") {
			edit_user_password(index);
		}
		if (message == "show_all_users") {
			show_all_users(index);
		}
		if (message == "delete_user") {
			delete_user(index);
		}
		if (message == "station_info") {
			station_info(index);
		}
		if (message == "set_station_info") {
			set_station_info(index);
		}
		if (message == "see_initial_values") {
			see_initial_values(index);
		}
		if (message == "edit_initial_values") {
			edit_initial_values(index);
		}
		if (message == "task_result") {
			task_result(index);
		}
	}
}

int main(int argc, char* argv[]) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	// Заполнение данными из файлов
	get_information_from_files();

	// Получение доступа для запуска сервера
	server_auth();

	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		cout << "Ошибка" << endl;
		exit(1);
	}
	cout << "Сервер запущен. Порт: 1111. IP: 127.0.0.1.\nОжидание пользователей." << endl;

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(IP);
	addr.sin_port = htons(PORT);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 0; i < 100; i++) {
		newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			cout << "Ошибка" << endl;
		}
		else {
			Counter++;
			cout << "Пользователь присоединен. Количество пользователей: " << Counter << endl;
			Connections[i] = newConnection;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		}
	}
	system("pause");
	return 0;
}

void server_auth() {
	string server_name, password;
	cout << "Авторизируйтесь чтобы запустить сервер.\n Имя сервера: "; cin >> server_name;
	cout << " Пароль: "; cin >> password;
	cout << "Подождите..." << endl;
	Sleep(500);
	while (!(server_info.is_authorized(server_name, password))) {
		system("cls");
		cout << "Неверное имя или пароль.\nАвторизируйтесь чтобы запустить сервер.\n Имя сервера: "; cin >> server_name;
		cout << " Пароль: "; cin >> password;
		cout << "Подождите..." << endl;
		Sleep(500);
	}
	system("cls");
}

void get_information_from_files() {
	ifstream server_auth_file;
	server_auth_file.open("server_name_password.txt");
	if (server_auth_file.is_open()) {
		while (!server_auth_file.eof()) {
			string login, password;
			server_auth_file >> login >> password;
			server_info.set_server_name(login);
			server_info.set_server_password(password);
		}
	}
	server_auth_file.close();

	ifstream administrator_password_file;
	administrator_password_file.open("administrator_password.txt");
	if (administrator_password_file.is_open()) {
		while (!administrator_password_file.eof()) {
			string password;
			administrator_password_file >> password;
			administrator.set_password(password);
		}
	}
	administrator_password_file.close();

	ifstream user_log_pass_file;
	user_log_pass_file.open("user_login_password.txt");
	if (user_log_pass_file.is_open()) {
		while (!user_log_pass_file.eof()) {
			string login, password;
			user_log_pass_file >> login >> password;
			User temp(login, password);
			users.push_back(temp);
		}
	}
	user_log_pass_file.close();

	return;
}

void save_information_in_files() {
	ofstream administrator_password_file("administrator_password.txt");
	if (administrator_password_file.is_open()) {
		administrator_password_file << administrator.get_password();
	}
	administrator_password_file.close();

	ofstream user_log_pass_file("user_login_password.txt");
	if (user_log_pass_file.is_open()) {
		for (int i = 0; i < users.size(); i++) {
			if (i == 0) {
				user_log_pass_file << users[i].get_login() << " " << users[i].get_password();
			}
			else {
				user_log_pass_file << endl << users[i].get_login() << " " << users[i].get_password();
			}
		}
	}
	user_log_pass_file.close();

	return;
}

void user_disconnect() {
	Counter--;
	cout << "Пользователь отключен. Количество пользователей: " << Counter << endl;
	return;
}

void administrator_auth_password(int index) {
	char pass[256];
	string password_administrator, auth_admin = "auth", not_auth_admin = "not_auth";

	recv(Connections[index], pass, sizeof(pass), NULL);
	stringstream ss; ss << pass; ss >> password_administrator;

	if (administrator.is_authorized(password_administrator)) {
		station.free();
		week_reports.clear();
		WeekReport week_1, week_2, week_3;
		week_reports.push_back(week_1);
		week_reports.push_back(week_2);
		week_reports.push_back(week_3);
		send(Connections[index], auth_admin.c_str(), auth_admin.length() + 1, NULL);
	}
	else {
		send(Connections[index], not_auth_admin.c_str(), not_auth_admin.length() + 1, NULL);
	}
}

void user_auth_login_password(int index) {
	char log[256], pass[256];
	string login_user, password_user, auth_user = "auth", not_auth_user = "not_auth";

	recv(Connections[index], log, sizeof(log), NULL);
	stringstream ss;  ss << log; ss >> login_user;

	recv(Connections[index], pass, sizeof(pass), NULL);
	stringstream ss1; ss1 << pass; ss1 >> password_user;

	for (int i = 0; i < users.size(); i++) {
		if (users[i].is_authorized(login_user, password_user)) {
			station.free();
			week_reports.clear();
			WeekReport week_1, week_2, week_3;
			week_reports.push_back(week_1);
			week_reports.push_back(week_2);
			week_reports.push_back(week_3);
			send(Connections[index], auth_user.c_str(), auth_user.length() + 1, NULL);
		}
	}
	send(Connections[index], not_auth_user.c_str(), not_auth_user.length() + 1, NULL);
}

void is_login_free(int index) {
	char log[256];
	string login, is_free = "free", is_not_free = "not_free";

	recv(Connections[index], log, sizeof(log), NULL);
	stringstream ss; ss << log; ss >> login;

	for (int i = 0; i < users.size(); i++) {
		if (users[i].get_login() == login) {
			send(Connections[index], is_not_free.c_str(), is_not_free.length() + 1, NULL);
		}
	}
	send(Connections[index], is_free.c_str(), is_free.length() + 1, NULL);
}

void create_new_user(int index) {
	char log[256], pass[256];
	string login, password;

	recv(Connections[index], log, sizeof(log), NULL);
	stringstream ss; ss << log; ss >> login;

	recv(Connections[index], pass, sizeof(pass), NULL);
	stringstream ss1; ss1 << pass; ss1 >> password;

	User temp(login, password);
	users.push_back(temp);
}

void edit_administrator_password(int index) {
	char pass[256];
	string password;

	recv(Connections[index], pass, sizeof(pass), NULL);
	stringstream ss1; ss1 << pass; ss1 >> password;

	administrator.set_password(password);
}

void edit_user_password(int index) {
	char log[256], pass[256];
	string login, password;

	recv(Connections[index], log, sizeof(log), NULL);
	stringstream ss; ss << log; ss >> login;

	recv(Connections[index], pass, sizeof(pass), NULL);
	stringstream ss1; ss1 << pass; ss1 >> password;

	for (int i = 0; i < users.size(); i++) {
		if (users[i].get_login() == login) {
			users[i].set_password(password);
		}
	}
}

void show_all_users(int index) {
	string on_send = "end";
	for (int i = 0; i < users.size(); i++) {
		send(Connections[index], users[i].get_login().c_str(), users[i].get_login().size() + 1, NULL); Sleep(100);
		send(Connections[index], users[i].get_password().c_str(), users[i].get_password().size() + 1, NULL); Sleep(100);
	}
	send(Connections[index], on_send.c_str(), on_send.size() + 1, NULL);
}

void delete_user(int index) {
	string on_send = "end";
	for (int i = 0; i < users.size(); i++) {
		send(Connections[index], users[i].get_login().c_str(), users[i].get_login().size() + 1, NULL); Sleep(100);
		send(Connections[index], users[i].get_password().c_str(), users[i].get_password().size() + 1, NULL); Sleep(100);
	}
	send(Connections[index], on_send.c_str(), on_send.size() + 1, NULL);

	char msg[256];
	string user_login, good = "good", not_good = "not_good";
	recv(Connections[index], msg, sizeof(msg), NULL);
	stringstream ss; ss << msg; ss >> user_login;
	
	for (int i = 0; i < users.size(); i++) {
		if (users[i].get_login() == user_login) {
			users.erase(users.begin() + i);
			send(Connections[index], good.c_str(), good.length() + 1, NULL);
			return;
		}
	}
	send(Connections[index], not_good.c_str(), not_good.length() + 1, NULL);
	return;
}

void station_info(int index) {
	string not_set = "not_set", set = "set";
	if (station.is_set() == false) {
		send(Connections[index], not_set.c_str(), not_set.size() + 1, NULL); Sleep(250);
		return;
	}
	else {
		send(Connections[index], set.c_str(), set.size() + 1, NULL); Sleep(250);
		send(Connections[index], station.get_number_of_buses().c_str(), station.get_number_of_buses().size() + 1, NULL); Sleep(100);
		send(Connections[index], station.get_bus_capacity().c_str(), station.get_bus_capacity().size() + 1, NULL); Sleep(100);
		send(Connections[index], station.get_capacity_percent().c_str(), station.get_capacity_percent().size() + 1, NULL); Sleep(100);
		send(Connections[index], station.get_total_capacity().c_str(), station.get_total_capacity().size() + 1, NULL); Sleep(100);
		return;
	}
}

void set_station_info(int index) {
	char msg[256];
	string number_of_buses, bus_capacity, capacity_percent;

	recv(Connections[index], msg, sizeof(msg), NULL);
	stringstream ss; ss << msg; ss >> number_of_buses;

	recv(Connections[index], msg, sizeof(msg), NULL);
	stringstream ss1; ss1 << msg; ss1 >> bus_capacity;

	recv(Connections[index], msg, sizeof(msg), NULL);
	stringstream ss2; ss2 << msg; ss2 >> capacity_percent;
	
	station.set_information(number_of_buses, bus_capacity, capacity_percent);

	return;
}

void see_initial_values(int index) {
	string not_set = "not_set", set = "set";
	if (station.is_set() == true) {
		send(Connections[index], set.c_str(), set.size() + 1, NULL); Sleep(250);
		for (int i = 0; i < week_reports.size(); i++) {
			string buses_on_road = week_reports[i].get_buses_on_road(),
				passenger_traffic = week_reports[i].get_passenger_traffic(),
				percent = week_reports[i].get_percent();
			send(Connections[index], buses_on_road.c_str(), buses_on_road.size() + 1, NULL); Sleep(100);
			send(Connections[index], passenger_traffic.c_str(), passenger_traffic.size() + 1, NULL); Sleep(100);
			send(Connections[index], percent.c_str(), percent.size() + 1, NULL); Sleep(100);
		}
		return;
	}
	else {
		send(Connections[index], not_set.c_str(), not_set.size() + 1, NULL); Sleep(250);
		return;
	}
}

void edit_initial_values(int index) {
	string not_set = "not_set", set = "set";
	if (station.is_set() == false) {
		send(Connections[index], not_set.c_str(), not_set.size() + 1, NULL); Sleep(250);
		return;
	}
	else {
		send(Connections[index], set.c_str(), set.size() + 1, NULL); Sleep(250);
		for (int i = 0; i < week_reports.size(); i++) {
			char msg1[256], msg2[256];
			string buses_on_road, passenger_traffic;
			recv(Connections[index], msg1, sizeof(msg1), NULL); Sleep(100);
			stringstream ss1; ss1 << msg1; ss1 >> buses_on_road;
			recv(Connections[index], msg2, sizeof(msg2), NULL); Sleep(100); 
			stringstream ss2; ss2 << msg2; ss2 >> passenger_traffic;

			int buses_on_road_int, passenger_traffic_int, capacity_int;
			istringstream(station.get_bus_capacity()) >> capacity_int;
			istringstream(buses_on_road) >> buses_on_road_int;
			istringstream(passenger_traffic) >> passenger_traffic_int;
			
			float percent_1 = passenger_traffic_int / 24;
			float percent_2 = buses_on_road_int * capacity_int;
			float percent_3 = percent_1 / percent_2;
			float percent_4 = percent_3 * 100;
			int percent = (int)percent_4;
			week_reports[i].set_information(buses_on_road, passenger_traffic, to_string(percent));
		}
		return;
	}
}

void task_result(int index) {
	string not_set = "not_set", set = "set";
	if (station.is_set() == false || week_reports[0].is_set() == false) {
		send(Connections[index], not_set.c_str(), not_set.size() + 1, NULL); Sleep(250);
		return;
	}
	else {
		send(Connections[index], set.c_str(), set.size() + 1, NULL); Sleep(250);



		int avg_passengers = 0;
		for (int i = 0; i < 3; i++) {
			int week_traffic;
			istringstream(week_reports[i].get_passenger_traffic()) >> week_traffic;
			avg_passengers += week_traffic;
		}
		avg_passengers = avg_passengers / 3;
		int avg_passengeres_per_hour = avg_passengers / 24;

		int totalcapacity;
		istringstream(station.get_total_capacity()) >> totalcapacity;
		if (totalcapacity < avg_passengeres_per_hour) {
			string too_much = "too_much";
			send(Connections[index], too_much.c_str(), too_much.size() + 1, NULL); Sleep(250);
			return;
		}
		string not_too_much = "not_too_much";
		send(Connections[index], not_too_much.c_str(), not_too_much.size() + 1, NULL); Sleep(250);

		int max_passengers, min_passengers;
		istringstream(week_reports[0].get_passenger_traffic()) >> max_passengers;
		istringstream(week_reports[0].get_passenger_traffic()) >> min_passengers;
		for (int i = 0; i < 3; i++) {
			int week_traffic;
			istringstream(week_reports[i].get_passenger_traffic()) >> week_traffic;
			if (week_traffic > max_passengers) {
				max_passengers = week_traffic;
			}
			if (week_traffic < min_passengers) {
				min_passengers = week_traffic;
			}
		}
		max_passengers /= 24;
		min_passengers /= 24;

		float result_buses_fl_1 = avg_passengers / 24;
		float result_buses_fl_2; istringstream(station.get_capacity_percent()) >> result_buses_fl_2;
		float result_buses_fl_3 = result_buses_fl_2 / 100;
		float result_buses_fl_4; istringstream(station.get_bus_capacity()) >> result_buses_fl_4;
		float result_buses_fl_5 = result_buses_fl_3 * result_buses_fl_4;
		float result_buses_fl = result_buses_fl_1 / result_buses_fl_5;
		int result_buses = (int)result_buses_fl;

		float max_percent, min_percent, total_buses_capacity, bus_capacity;
		istringstream(station.get_bus_capacity()) >> bus_capacity;
		total_buses_capacity = result_buses * bus_capacity;
		max_percent = max_passengers / total_buses_capacity;
		min_percent = min_passengers / total_buses_capacity;
		max_percent *= 100; min_percent *= 100;

		send(Connections[index], to_string(result_buses).c_str(), to_string(result_buses).size() + 1, NULL); Sleep(100);
		send(Connections[index], to_string(min_percent).c_str(), to_string(min_percent).size() + 1, NULL); Sleep(100);
		send(Connections[index], to_string(max_percent).c_str(), to_string(max_percent).size() + 1, NULL); Sleep(100);
		send(Connections[index], to_string(result_buses_fl_2).c_str(), to_string(result_buses_fl_2).size() + 1, NULL); Sleep(100);
		return;
	}
}