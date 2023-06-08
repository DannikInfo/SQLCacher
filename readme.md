# SQLCacher  
Исполнитель SQL запросов.  

Используется с целью разбора буфера из файлов запросов ```.sql```, которые генерируют другие программы.

Ограничений нет, но лучшее применение - ```INSERT``` запросы, которые подходят для записи различной статистики


----
## Сборка:
```shell
mkdir build
cd build
cmake ..
make
```
### Зависимости:
- <a href="//github.com/DannikInfo/tinyLoggerCpp">TinyLogger</a>, <a href="//github.com/DannikInfo/tinyConfigCpp">TinyConfig</a>, <a href="//github.com/DannikInfo/tinyUtilsCpp">TinyUtils</a>
- mysqlCppConn, Threads

---
## Использование:
- Запустить программу
- Выключить
- Настроить конфигурацию
- Запускать с помощью crontab на старте

### Конфигурация:

| Параметр       | Тип             | По умолчание                                                          | Описание                                                    |
|----------------|-----------------|-----------------------------------------------------------------------|-------------------------------------------------------------|
| dbName         | string          | db                                                                    | Название БД                                                 |
| dbUser         | string          | root                                                                  | Пользователь СУБД                                           |
| dbPass         | string          | root                                                                  | Пароль пользователя СУБД                                    |
| dbUrl          | string          | localhost                                                             | Адрес БД                                                    |
| debug          | bool            | ```false```                                                           | Режим отладки                                               |
| cachePaths     | array of string | ```"minDelayInSec:path",``` </br>```"10:/home/user/AIServer/cache"``` | Маркеры директорий                                          |

---
## TODO:
- возможность подключать несколько БД
