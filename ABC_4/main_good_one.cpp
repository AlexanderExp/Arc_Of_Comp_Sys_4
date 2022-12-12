#include <fstream>
#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
#include <vector>

// Количество номеров в отеле.
const int NUMBER_OF_ROOMS_IN_HOTEL = 30;

bool rooms[NUMBER_OF_ROOMS_IN_HOTEL];

int current_room_to_be_occupied = 0;

// Максимальное количество дней остановки клиента.
const int MAX_DAYS_RENT = 3;

// Максимальное количество клиентов
const int MAX_NUMBER_OF_CLIENTS = 100;

// Мьютекс для портье
pthread_mutex_t receptionist_sem;

// Филнальный вывод. Переменная, в которую записывается весь вывод программы
std::string backlog = "";

// Количество комнат в отеле (По условию 30)
int number_of_free_rooms = 30;

// Структура клиента
struct Client {
private:
    int id_;
    int time_of_stay_ = 0;
    bool has_stayed_for_his_time_ = false;
    int number_of_room_;

public:
    Client() = default;
    Client(int id, int time_of_stay) {
        id_ = id;
        time_of_stay_ = time_of_stay;
    }
    int getID() {
        return id_;
    }
    int getTimeOfStay() {
        return time_of_stay_;
    }
    void hasLeft() {
        has_stayed_for_his_time_ = true;
    }
    bool isReadyToLeave() {
        return has_stayed_for_his_time_;
    }
    void fixNumberOfRoom(int num) {
        number_of_room_ = num;
    }
    int getNumberOfRoom() {
        return number_of_room_;
    }
};


// Сообщает о том, что клиент покинул отель
// Меняет значение has_stayed_for_his_time_ на true
void clientLeft(Client *client) {
    pthread_mutex_lock(&receptionist_sem);
    backlog += "Client with id " + std::to_string(client->getID()) + " has left room number " + std::to_string(client->getNumberOfRoom()) + "\n";
    pthread_mutex_unlock(&receptionist_sem);
    number_of_free_rooms++;
    rooms[client->getNumberOfRoom()] = true;
    client->hasLeft();
}

int getFreeRoomNumber() {
    for (int i = 0; i < NUMBER_OF_ROOMS_IN_HOTEL; ++i) {
        if (rooms[i]) {
            return i;
        }
    }
}

// Сообщает о том, что клиент пытется заселиться
// Если попытка успешна, сообщает о том, что клиента заселили
// Запускает метод, выселяющий клиента по прошествии заданного времени
// Главный метод по работе с потоками
void *clientSettled(void *param) {
    Client *client = (Client *) param;

    while (!client->isReadyToLeave()) {
        // Блокируем поток записи
        pthread_mutex_lock(&receptionist_sem);
        // Записываем информацию о попытке заселиться
        backlog += "Client with id " + std::to_string(client->getID()) + " is trying to settle\n";
        // Разблокируем поток
        pthread_mutex_unlock(&receptionist_sem);
        // Проверяем, есть ли свободные комнаты
        if (number_of_free_rooms > 0) {
            // Если свободные комнаты есть, уменьшаем их количество и заселяем
            number_of_free_rooms--;

            current_room_to_be_occupied = getFreeRoomNumber();

            client->fixNumberOfRoom(current_room_to_be_occupied);

            rooms[client->getNumberOfRoom()] = false;

            // Блокируем поток записи
            pthread_mutex_lock(&receptionist_sem);
            // Записываем информацию о заселении
            backlog += "Client with id " + std::to_string(client->getID()) + " has settled in room number " + std::to_string(client->getNumberOfRoom()) + "\n";
            // Разблокируем поток
            pthread_mutex_unlock(&receptionist_sem);
            // Клиент проводит планируемое время в отеле
            sleep(client->getTimeOfStay());
            // Блокируем поток записи
            pthread_mutex_lock(&receptionist_sem);
            // Записываем информацию о том, что клиент пробыл в отеле планируемое время
            backlog += "Client with id " + std::to_string(client->getID()) + " has stayed long enough\n";
            // Разблокируем поток
            pthread_mutex_unlock(&receptionist_sem);
            // Вызываем метод для выселения клиента
            clientLeft(client);
        } else {
            // Если свободных комнат в отеле нет, то клиент ждёт некоторое время
            sleep(1);
        }
    }
    return nullptr;
}

int main(int argc, char *argv[]) {

    // Количество клиентов
    int number_of_clients;

    // Количество дней, на которое гость снимает номер
    // Число в диапазоне [1,MAX_DAYS_RENT]
    //int rent_days[number_of_clients];

    std::vector<Client> clients;

    for (int i = 0; i < NUMBER_OF_ROOMS_IN_HOTEL; ++i) {
        rooms[i] = true;
    }
    int answer = 0;

    // Опции ввода/вывода данных
    // Возможные варианты
    // argc == 1
    // Ввод осуществляется из консоли, пользователю предлагается выбор:
    // Ввод всех данных через консоль
    // или
    // Случайная генерация данных по введенному ключу
    // argc == 2
    // Случайная генерация данных по введенному ключу + замер времени работы программы
    // argc == 3
    // Работа с файлами:
    // Если первое вводимое значени == "random", а второе <File Name>,
    // То данные будут сгенерированы случайно, а вывод будет осуществляться в файл с указанным File Name
    // Если первое значение != "random", то первое значение == <Input FIle Name>, второе == <Output File Name>
    // Входные данные будут взяты из файла с указанным <Input FIle Name>
    // Вывод будет осуществляться в файл с указанным <Output File Name>
    if (argc == 1) {
        printf("Random input or through command line (1 - cmd; 2 - random):");
        scanf("%d", &answer);

        // Рандомная генерация либо ввод/вывод полностью через консоль
        // Если вводится 1, то работа идет полность с конслоью
        // Если вводится 2, то данные генерируюся случайным образом;
        // Пользователя информируют о сгенерированных данных
        if (answer == 1) {
            printf("Enter the number of clients :");
            scanf("%d", &number_of_clients);

            clients = std::vector<Client>(number_of_clients);

            // Создаем клиентов с их планами на пребывание в отеле
            int stay_time;
            for (int i = 0; i < number_of_clients; ++i) {
                std::cout << "Enter the time of stay for Client with id " + std::to_string(i) + ": ";
                std::cin >> stay_time;
                std::cout << "\n";
                clients.emplace_back(i, stay_time);
            }
            printf("\n");
        } else if (answer == 2) {
            int seed;
            printf("Enter seed f:");
            scanf("%d", &seed);
            srand(seed);

            number_of_clients = rand() % MAX_NUMBER_OF_CLIENTS;
            printf("Generated number: %d\n", number_of_clients);

            clients = std::vector<Client>(number_of_clients);
            int stay_time;
            for (int i = 0; i < number_of_clients; ++i) {
                stay_time = rand() % MAX_DAYS_RENT;
                clients.emplace_back(i, stay_time);
            }

            printf("Guests will stay for ... days (first number corresponding to first guest, etc.):\n");

            for (int i = 0; i < number_of_clients; ++i) {
                printf("%d ", clients[i].getTimeOfStay());
            }
            printf("\n");
        }
    } else {
        if (!strcmp(argv[1], "random")) {
            srand(time(0));
            number_of_clients = rand() % MAX_NUMBER_OF_CLIENTS;

            clients = std::vector<Client>(number_of_clients);

            int stay_time;
            for (int i = 0; i < number_of_clients; ++i) {
                stay_time = rand() % MAX_DAYS_RENT;
                clients.emplace_back(i, stay_time);
            }
        } else {
            std::ifstream input;
            input.open(argv[1]);
            // Считываем из файла number_of_clients
            input >> number_of_clients;
            // Считываем из файла массив значений rent_days
            int stay_time;
            for (int i = 0; i < number_of_clients; ++i) {
                input >> stay_time;
                clients.emplace_back(i, stay_time);
            }
            input.close();
        }
    }

    // Объявляем потоки клиентов
    pthread_mutex_init(&receptionist_sem, nullptr);
    std::vector<pthread_t> threads(number_of_clients);

    // Создаем потоки клиентов
    for (int i = 0; i < number_of_clients; ++i) {
        pthread_create(&threads[i], nullptr, clientSettled, &clients[i]);
    }

    // Ждёем все потоки
    for (int i = 0; i < number_of_clients; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // Выводим результат в консоль
    std::cout << backlog;

    // Выводим результат в файл
    if (argc == 3) {
        std::ofstream output(argv[2]);
        output << backlog;
    }
    return 0;
}
