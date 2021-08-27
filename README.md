# hash_verification_for-files
uses controle data to verify files integrity by comparing hash sums 

Верификация хеш-сумм файлов

Выполнено на:
C++17, IDE - Visual Studio 2019, configuration - Release x64

Использована библиотека CryptoPP 8.5 (https://www.cryptopp.com/) для работы с хеш-суммами.

Дано: Файл сумм. Файл содержит имена файлов, алгоритм хэширования (один из MD5/SHA1/SHA256) 
и соответствующие им хеш-суммы. Эти хеш-суммы вычислены по соответствующему алгоритму, 
они указаны в файле через пробел. 

Что делает программа: Читает файл сумм и проверяет целостность файлов.

Файл сумм (см. controle_data_hash.txt):
file_01.bin md5 41C5DDAF2675EAE534A74AEF985367D9
file_02.bin md5 5F6B5AC33BBDCEBEE91EA6D3ADA7125F
file_03.bin sha256 4917E8437BCBF0EAB60F28BCD990B1A3789E54BEDACFC442645A988046E3AA39
file_04.txt sha1 F0CB60E9B2C7527D60E26B14A1C7A465A1955453

Файл сумм и файлы, которые необходимо проверить, содержатся в папке "files_for_controle_check"

Изначальные файлы, от которых получены контрольные (т.е. правильные) хеш-суммы,
содержатся в папке "init_files_to-be-encoded". 

Формат ввода после запуска программы:  
<path to the input file> <path to the directory containing the files to check>

Программа выводит название проверяемого файла и его статус, через пробел:
file_01.bin OK
file_02.bin NOT FOUND
file_03.bin OK
file_04.txt FAILED
