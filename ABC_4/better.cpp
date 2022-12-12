#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>

class Client {
private:
    int id_;
    int rent_days_;
    int current_stay_lasts_for_days_;
    int came_at_time_;

public:
    Client(int id, int rent_days) {
        this->id_ = id;
        this->rent_days_ = rent_days;
        this->current_stay_lasts_for_days_ = 0;
        came_at_time_ = rand() % 50;
    }

    int getTimeSleep() {
        return came_at_time_;
    }
    
    int getID() {
        return id_;
    }

    void stayedForOneMoreDay() {
        ++current_stay_lasts_for_days_;
    }

    int getCurrentStay() {
        return current_stay_lasts_for_days_;
    }
    int getRentDays() {
        return rent_days_;
    }

};

class Room{
private:
    Client *current_client_ = nullptr;

public:
    Room() {
        current_client_ = nullptr;
    }

    bool isEmpty() {
        if (current_client_ == nullptr) {
            return true;
        } else {
            return false;
        }
    }

    void clientLeaves() {
        current_client_ = nullptr;
    }

    void clientArrives(Client *new_client) {
        current_client_ = new_client;
    }

    Client* getClient() {
        return current_client_;
    }
};


// Количество номеров в отеле.
const int NUMBER_OF_ROOMS_IN_HOTEL = 30;

// Максимальная количество порций, которые насыщают Винни-Пуха.
const int MAX_DAYS_RENT = 10;

const int MAX_NUMBER_OF_CLIENTS = 1000;


void *serviceEveryGuest(void *param) {
    Client *client = (Client *) param;

    std::string str_thread;

    sleep(client->getTimeSleep());

    printf("\nClient with id %d came to the Hotel\n", client->getID());
    
    while (client->getCurrentStay() != client->getRentDays()) {
        printf("\nClient with id %d has stayed for %d days out of %d days planed", client->getID(), client->getCurrentStay(), client->getRentDays());
        // Проходит день для клиента
        sleep(24);

        client->stayedForOneMoreDay();



    }
    
    
    
}

int main(int argc, char *argv[]) {
    // Начальное время
    clock_t start_time = clock();

    // Количество клиентов
    int number_of_clients;

    // Количество дней, на которое гость снимает номер
    // Число в диапазоне [1,MAX_DAYS_RENT]
    //int rent_days[number_of_clients];

    std::vector<Client> clients;
    std::vector<pthread_t> clients_threads;

    std::vector<Room> rooms(NUMBER_OF_ROOMS_IN_HOTEL);
    std::vector<pthread_t> rooms_threads(NUMBER_OF_ROOMS_IN_HOTEL);


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
        printf("Random input or through command line (1 - cmd, 2 - random):");
        int answer = 0;
        scanf("%d", &answer);

        // Рандомная генерация либо ввод/вывод полностью через консоль
        // Если вводится 1, то работа идет полность с конслоью
        // Если вводится 2, то данные генерируюся случайным образом;
        // Пользователя информируют о сгенерированных данных
        if (answer == 1) {
            printf("Enter the number of clients :");
            scanf("%d", &number_of_clients);

            clients = std::vector<Client>(number_of_clients);
            clients_threads = std::vector<pthread_t>(number_of_clients);

            // Создаем клиентов с их планами на пребывание в отеле
            int stay_time;
            for (int i = 0; i < number_of_clients; ++i) {
                std::cout << "Enter the time of stay for Client with id " + std::to_string(i) + ": ";
                std::cin >> stay_time;
                std::cout << "\n";
                Client *new_client = new Client(i, stay_time);
                clients[i] = *new_client;
            }
        } else {
            int seed;
            printf("Enter seed f:");
            scanf("%d", &seed);
            srand(seed);

            number_of_clients = rand() % MAX_NUMBER_OF_CLIENTS;
            printf("Generated number: %d\n", number_of_clients);

            clients = std::vector<Client>(number_of_clients);
            clients_threads = std::vector<pthread_t>(number_of_clients);
            int stay_time;
            for (int i = 0; i < number_of_clients; ++i) {
                stay_time = rand() % MAX_DAYS_RENT;
                Client *new_client = new Client(i, stay_time);
                clients[i] = *new_client;
            }

            printf("Guests will stay for ... days (first number corresponding to first guest, etc.):\n");

            for (int i = 0; i < number_of_clients; ++i) {
                printf("%d ", clients[i].getRentDays());
            }
            printf("\n");




        }
    } else if (argc == 2) {
        int seed;
        printf("Enter seed f:");
        scanf("%d", &seed);
        srand(seed);

        time_t t_start = clock();

        number_of_clients = rand() % MAX_NUMBER_OF_CLIENTS;
        printf("Generated number: %d\n", number_of_clients);

        clients = std::vector<Client>(number_of_clients);
        clients_threads = std::vector<pthread_t>(number_of_clients);

        int stay_time;
        for (int i = 0; i < number_of_clients; ++i) {
            stay_time = rand() % MAX_DAYS_RENT;
            Client *new_client = new Client(i, stay_time);
            clients[i] = *new_client;
        }

        printf("Guests will stay for ... days (first number corresponding to first guest, etc.):\n");

        for (int i = 0; i < number_of_clients; ++i) {
            printf("%d ", clients[i].getRentDays());
        }
        printf("\n");

        for (int i = 0; i < 1000000; ++i) {

        }


        time_t t_end = clock();
        printf("Time: %d ms\n", (int) (difftime(t_end, t_start)) / 1000);


    } else if (argc == 3) {
        if (!strcmp(argv[1], "random")) {
            srand(time(0));
            number_of_clients = rand() % MAX_NUMBER_OF_CLIENTS;

            clients = std::vector<Client>(number_of_clients);
            clients_threads = std::vector<pthread_t>(number_of_clients);

            int stay_time;
            for (int i = 0; i < number_of_clients; ++i) {
                stay_time = rand() % MAX_DAYS_RENT;
                Client *new_client = new Client(i, stay_time);
                clients[i] = *new_client;
            }



            FILE *output;
            output = fopen(argv[2], "w");


            fclose(output);
            return 0;
        } else {
            FILE *input;
            input = fopen(argv[1], "r");
            if (input == NULL) {
                printf("File was not found\n");
                fclose(input);
                return 0;
            } else {
                // Считываем из файла number_of_clients
                fscanf(input, "%d", &number_of_clients);
                // Считываем из файла массив значений rent_days
                int stay_time;
                for (int i = 0; i < number_of_clients; ++i) {
                    fscanf(input, "%d", &stay_time);
                    Client *new_client = new Client(i, stay_time);
                    clients[i] = *new_client;
                }
            }
            fclose(input);



            FILE *output;
            output = fopen(argv[2], "w");



            fclose(output);
            return 0;
        }
    }
    return 0;
}