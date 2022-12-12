# Arc_Of_Comp_Sys_4

### Фролов Александр Сергеевич, БПИ217

### Вариант 12

### Условие задачи:
Задача о гостинице - 1. В гостинице 30 номеров, клиенты гостиницы снимают номер на одни или несколько суток. Если в гостинице нет свобод- ных номеров, клиенты не уходят, а устраиваются на рядом с гостиницей на скамейках и ждут, пока любой номеров не освободится. Создать многопо- точное приложение, моделирующее работу гостиницы.

### Запуск через терминал: 

1) ./ABC_4
  Пользователю будет придложен выбор: 1) Ввод данных через консоль
                                      2) Ввод данных случайным образом с указанием ключа "seed"
2) ./ABC_4 random <output.txt> 
  Входные данные будут сгенерированы случайным образом, вывод будет осуществлен в файл и в консоль
3) ./ABC_4 <input.txt> <output.txt>
  Входные данные берутся из файла <input.txt>, вывод будет осуществлен в файли в консоль
  
### Задача на языке программирования

Каждый клиент является потоком. Клиент периодически обращается к портье, стоящему на ресепшене в гостинице с 30 номерами, выводится сообщение о том, что клиент пытается заселиться. Если один из номеров свободен, то поток данного клиента блокируется, выводится соответствующее сообщение о том, что клиент был заселен.Если ни одна из комнат не была свободна, то клиент засыпает и потом вновь обращается к портье. Когда клиент был заселен, его поток засыпает на время пребывания клиента в отеле. После того, как время остановки клиента истечет, его поток будет разблокирован, выведется соответствующее сообщение. Так же у клиента изменится значение "has_stayed_for_his_time_", сигнализирующее о том, что клиент уже был в отеле планируемое время и более не обратится к портье за время работы программы.


### Модель приложения

Была использована модель "Портфель задач". Задача делится на конечное число потоков. Клиенты -потоки, обращающиеся к портье, который заселяет гостей, если есть место. Распределение клиентов по комнатам происходит динамически. Если номеров нет, то поток ждёт и обращается вновь через некоторое время. С такой моделью мы обезопасили себя от ситуации, когда клиентов заселяют в занятые номера.


### Замечания:
Для решения задачи были использованы "мьютексы" ("pthread_mutex_t").
При работе программы с output.txt весь вывод дублируется в консоль
