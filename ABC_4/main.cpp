#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>
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

class Hotel {
private:
    int number_of_rooms_;
    std::vector<Room> rooms_;

public:
    Hotel(int number_of_rooms) {
        number_of_rooms_ = number_of_rooms;
        for (int i = 0; i < number_of_rooms; ++i) {
            rooms_.push_back(*new Room());
        }
    }

    bool is_full() {
        for (int i = 0; i < number_of_rooms_; ++i) {
            if (rooms_[i].isEmpty()) {
                return false;
            }
        }
        return true;
    }

    int getNumberOfEmptyRoom() {
        for (int i = 0; i < number_of_rooms_; ++i) {
            if (rooms_[i].isEmpty()) {
                return i;
            }
        }
        printf("Something went wrong in getNumberOfEmptyRoom function\n");
    }

    void add_client(Client new_client) {

        if (is_full()) {
            printf("Hotel is currently full\n");
        } else {
            int number_of_empty_room = getNumberOfEmptyRoom();
            rooms_[number_of_empty_room].clientArrives(&new_client);
            printf("Client with id %d was settled in room number %d\n", new_client.getID(), number_of_empty_room);
        }
    }

    void dayPassed() {
        for (int i = 0; i < number_of_rooms_; ++i) {
            rooms_[i].getClient()->stayedForOneMoreDay();
            if (rooms_[i].getClient()->getCurrentStay() == rooms_[i].getClient()->getRentDays()) {
                rooms_[i].clientLeaves();
                printf("Client with id %d has left, room %d is now empty\n", rooms_[i].getClient()->getID(), i);
            }
        }
    }

};


// Количество номеров в отеле.
const int NUMBER_OF_ROOMS_IN_HOTEL = 30;

// Максимальная количество порций, которые насыщают Винни-Пуха.
const int MAX_DAYS_RENT = 10;

const int MAX_NUMBER_OF_CLIENTS = 1000;

// Мьютекс для синхронизации заполнения пчёлами горшка.
pthread_mutex_t mutex;


void *serviceEveryGuest(void *param) {
    Client *client = (Client *) param;

    std::string str_thread;

    sleep(client->getTimeSleep());

    printf("\nClient with id %d came to the Hotel\n", client->getID());

    while (client->getCurrentStay() != client->getRentDays()) {
        printf("\nClient with id %d has stayed for %d days out of %d days planed", client->getID(), client->getCurrentStay(), client->getRentDays());
        // Для клиента проходит день
        sleep(10);
    }



}



// Метод, используемый в многопоточности. Моделирует работу пчёл.
void *fillHoneyPot(void *param) {
    auto *package = (Package *) param;

    // В один момент времени только одна пчела может положить мёд в горшок.
    pthread_mutex_lock(&mutex);
    // Пчёлы приносят мёд пока Винни-Пух не насытится.
    if (!package->winnie_the_pooh->isSatiated()) {
        package->bee->bringHoney(package->file_out);
        package->honey_pot->addSip();
        package->honey_pot->print(package->file_out);

        int sleep_milliseconds = package->bee->flyAway(package->file_out);

        if (package->honey_pot->isFull()) {
            package->winnie_the_pooh->eatPot(package->file_out);
            package->honey_pot->emptyPot();
        }
        pthread_mutex_unlock(&mutex);

        usleep(sleep_milliseconds);

        fillHoneyPot(param);
        return nullptr;
    }
    pthread_mutex_unlock(&mutex);

    // Окончание выполнения потока (пчёлы перкращают нести мёд).
    package->bee->stopBringHoney(package->file_out);
    return nullptr;
}




int main(int argc, char *argv[]) {
    // Начальное время
    clock_t start_time = clock();

    // Количество клиентов
    int number_of_clients;

    // Количество дней, на которое гость снимает номер
    // Число в диапазоне [1,MAX_DAYS_RENT]
    int rent_days[number_of_clients];

    Hotel *lovely_place;

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

            lovely_place = new Hotel(number_of_clients);

            printf("Enter an array of numbers showing the duration of each guest's stay (first guest - first entered number in array)\n"
                   "Your array :");
            for (int i = 0; i < number_of_clients; ++i) {
                scanf("%d", &rent_days[i]);
                Client *new_client
            }


        } else {
            int seed;
            printf("Enter seed f:");
            scanf("%d", &seed);
            srand(seed);

            number_of_clients = rand() % MAX_NUMBER_OF_CLIENTS;
            printf("Generated number: %d\n", number_of_clients);

            for (int i = 0; i < number_of_clients; ++i) {
                rent_days[i] = rand() % MAX_DAYS_RENT;
            }

            printf("Guests will stay for ... days (first number corresponding to first guest, etc.):\n");

            for (int i = 0; i < number_of_clients; ++i) {
                printf("%d ", rent_days[i]);
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

        for (int i = 0; i < number_of_clients; ++i) {
            rent_days[i] = rand() % MAX_DAYS_RENT;
        }

        printf("Guests will stay for ... days (first number corresponding to first guest, etc.):\n");

        for (int i = 0; i < number_of_clients; ++i) {
            printf("%d ", rent_days[i]);
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

            for (int i = 0; i < number_of_clients; ++i) {
                rent_days[i] = rand() % MAX_DAYS_RENT;
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
                for (int i = 0; i < number_of_clients; ++i) {
                    fscanf(input, "%d", &rent_days[i]);
                }
            }
            fclose(input);

            FILE *output;
            output = fopen(argv[2], "w");



            fclose(output);
            return 0;
        }
    }




    FILE *file_out = fopen(argv[argc - 1], "w");

    auto *honey_pot = new HoneyPot(pot_capacity);
    auto *winnie_the_pooh = new WinnieThePooh(saturate_pots);

    auto *threads = new pthread_t[bees];
    auto *thread_info = new Package[bees];

    // Создание дочерних потоков.
    for (int i = 0; i < bees; ++i) {
        thread_info[i] = {new Bee(i + 1), honey_pot, winnie_the_pooh, file_out};
        pthread_create(&threads[i], nullptr, fillHoneyPot, (void *) &thread_info[i]);
    }

    // Время начла работы потоков.
    clock_t thread_started_time = clock();

    // Ожидание завершения работы дочерних потоков.
    for (int i = 0; i < bees; i++) {
        pthread_join(threads[i], nullptr);
    }

    fclose(file_out);
    for (int i = 0; i < bees; ++i) {
        delete thread_info[i].bee;
    }
    delete[] thread_info;
    delete[] threads;
    delete honey_pot;
    delete winnie_the_pooh;

    // Конечное время.
    clock_t end_time = clock();

    printf("Threads started: %.6f sec\n", (double) (thread_started_time - start_time) / CLOCKS_PER_SEC);
    printf("Time taken: %.6f sec\n", (double) (end_time - start_time) / CLOCKS_PER_SEC);

    return 0;
}