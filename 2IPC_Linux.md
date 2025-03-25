


#
[Системное программирование в ОС	2](#_toc190170005)

[Средства межпроцессного взаимодействия в LINUX ОС	2](#_toc190170006)

[1. Сигналы	2](#_toc190170007)

[Сигналы POSIX реального времени	12](#_toc190170008)

[2. Неименованные каналы	13](#_toc190170009)

[3. Именованные каналы	17](#_toc190170010)

[4. Очереди сообщений	24](#_toc190170011)

[5. Семафоры и разделяемая память	30](#_toc190170012)

[6. Сокеты	43](#_toc190170013)


























# <a name="_toc190170005"></a>**Системное программирование в ОС** 

<a name="_toc190170006"></a>**Средства межпроцессного взаимодействия в LINUX ОС**



В Unix-подобных ОС  средства межпроцессных взаимодействий (IPC – Inter  Process Communication) включают следующий базовый набор: 

- сигналы,
- анонимные (неименованные) каналы (иногда их называют программные),
- именованные каналы,
- очереди сообщений,
- семафоры  
- разделяемая память,
- сокеты.

IPC в той или иной степени совмещают функции уведомления о событии и его обработки, передачи информации и синхронизации. В Unix-подобных ОС ***все перечисленные средства, за исключением сокетов, являются локальными***, т.к. используют буферизацию в пространстве ядра и адресуются в локальном пространстве памяти. 

Для исследования IPC на примерах используем ОС Linux (Ubuntu).

# **<a name="_toc190170007"></a>1. Сигналы**
Сигналы позволяют осуществить самый примитивный способ коммуникации между двумя процессами. Сигналы в системе UNIX используются для того, чтобы: сообщить процессу о том, что возникло асинхронное событие; или необходимо обработать исключительное состояние.

Изначально сигналы были разработаны для уведомления об ошибках. В дальнейшем их стали использовать и как простейшую форму межпроцессного взаимодействия (IPС), например, для синхронизации процессов или для передачи простейших команд от одного процесса другому.
Сигнал позволяет передать уведомление о некотором произошедшем событии между процессами или между ядром системы и процессами. Это означает, что посредством сигналов можно выполнять две основные функции IPC: передачу информации и синхронизацию процессов или потоков. 

Для отправки и доставки сигнала требуется системный вызов. Для доставки – прерывание и его обработка. При этом требуется проведение довольно большого числа операций со стеком – копирование пользовательского стека в системную область, извлечение параметров и результатов работы системных вызовов и прерываний. Поскольку объем передаваемой информации при этом способе взаимодействия не велик, а затраты на его реализацию существенны, сигналы считаются одним из самых ресурсоемких способов IPC. 

Каждый сигнал имеет уникальное символьное имя и соответствующий ему номер. Базовый перечень сигналов, поддерживаемый практически в любой POSIX-ориентированной ОС, составляет не более  тридцати двух (количество бит в тридцати двух-разрядном слове) и в большинстве современных систем их номера смещены к началу нумерации. Наряду с базовыми в POSIX ОС дополнительно может поддерживаться свой уникальный набор сигналов. 

Кроме того, с расширением стандарта POSIX и современными возможностями наращивания разрядной сетки (до шестидесяти четырех) перечень сигналов во многих ОС тоже расширился. Появился еще один тип сигналов – сигналы реального времени,  которые могут принимать значения между SIGRTMIN и SIGRTMAX включительно. POSIX требует, чтобы предоставлялось по крайней мере RTSIG\_MAX сигналов, и минимальное значение этой константы равно 8. 

Ознакомиться с полным перечнем сигналов можно с помощью команды *kill -l* в командном интерпретаторе той реализации ОС, с которой вы работаете, например, один из возможных вариантов:

```c
$ kill -l 

\1) SIGHUP 2) SIGINT 3) SIGQUIT 4) SIGILL 5) SIGTRAP 
\6) SIGABRT 7) SIGBUS 8) SIGFPE 9) SIGKILL 10) SIGUSR1 
\11) SIGSEGV 12) SIGUSR2 13) SIGPIPE 14) SIGALRM 15) SIGTERM 
\16) SIGSTKFLT 17) SIGCHLD 18) SIGCONT 19) SIGSTOP 20) SIGTSTP 
\21) SIGTTIN 22) SIGTTOU 23) SIGURG 24) SIGXCPU 25) SIGXFSZ 
\26) SIGVTALRM 27) SIGPROF 28) SIGWINCH 29) SIGIO 30) SIGPWR 
\31) SIGSYS 34) SIGRTMIN 35) SIGRTMIN+1 36) SIGRTMIN+2 
\37) SIGRTMIN+3 38) SIGRTMIN+4 39) SIGRTMIN+5 40) SIGRTMIN+6 
\41) SIGRTMIN+7 42) SIGRTMIN+8 43) SIGRTMIN+9 44) SIGRTMIN+10 
\45) SIGRTMIN+11 46) SIGRTMIN+12 47) SIGRTMIN+13 48) SIGRTMIN+14
\49) SIGRTMIN+15 50) SIGRTMAX-14 51) SIGRTMAX-13 52) SIGRTMAX-12 
\53) SIGRTMAX-11 54) SIGRTMAX-10 55) SIGRTMAX-9 56) SIGRTMAX-8 
\57) SIGRTMAX-7 58) SIGRTMAX-6 59) SIGRTMAX-5 60) SIGRTMAX-4 
\61) SIGRTMAX-3 62) SIGRTMAX-2 63) SIGRTMAX-1 64) SIGRTMAX 
```


Следует заметить, что именование базовых сигналов, как правило, совпадает в разных Unix-подобных ОС, чего нельзя сказать о нумерации, поэтому целесообразно сначала ознакомиться со списком.

Кроме того, сигнал может быть отправлен процессу либо ядром, либо другим процессом с помощью системного вызова kill():	

```c
#include <signal.h>	
int kill(pid\_t pid, int sig);	                 
```

Аргумент *pid* адресует процесc, которому посылается сигнал. Аргумент *sig* определяет тип отправляемого сигнала. С помощью системного вызова *kill()* процесс может послать сигнал, как самому себе, так и другому процессу или группе процессов. В этом случае процесс, посылающий сигнал, должен иметь те же реальный и эффективный идентификаторы, что и процесс, которому сигнал отправляется. Разумеется, данное ограничение не распространяется на ядро или процессы, обладающие привилегиями суперпользователя. Они имеют возможность отправлять сигналы любым процессам системы.

Аналогичное действие можно произвести из командной строки в терминальном режиме, используя команду интерпретатора  

` kill pid `

К генерации сигнала могут привести различные ситуации:

1\. Ядро отправляет процессу (или группе процессов) сигнал при нажатии пользователем определенных клавиш или их комбинаций. 

2\. Аппаратные особые ситуации, например, деление на 0, обращение недопустимой области памяти и т.д., также вызывают генерацию сигнала. Обычно эти ситуации определяются аппаратурой компьютера, и ядру посылается соответствующее уведомление (например, виде прерывания). Ядро реагирует на это отправкой соответствующего сигнала процессу, который находился в стадии выполнения, когда произошла особая ситуация.

3\. Определенные программные состояния системы или ее компонентов также могут вызвать отправку сигнала. В отличие от предыдущего случая, эти условия не связаны с аппаратной частью, а имеют программный характер. В качестве примера можно привести сигнал SIGALRM, отправляемый процессу, когда срабатывает таймер, ранее установленный с помощью вызова alarm().

Процесс может выбрать одно из трех возможных действий при получении сигнала:

```
1\. игнорировать сигнал,

2\. перехватить и самостоятельно обработать сигнал,		

3\. позволить действие по умолчанию.
```

Текущее действие при получении сигнала называется **диспозицией сигнала**.

Порожденный вызовом *fork()* процесс *наследует диспозицию* сигналов от своего родителя.  Однако при вызове *exec()* диспозиция всех перехватываемых сигналов будет установлена ядром на действие *по умолчанию*. 

В ОС поддерживается ряд функций, позволяющих управлять диспозицией сигналов.

Наиболее простой в использовании является функция *signal().* Она позволяет устанавливать и изменять диспозицию сигнала. 

```c
#include <signal.h>

void (\*signal (int sig,  void (\*disp)(int)))(int);
```

Аргумент sig определяет сигнал, диспозицию которого нужно изменить. Аргумент disp определяет новую диспозицию сигнала. Возможны следующие три варианта:

|Значение|Назначение|
| :-: | :-: |
|SIG\_DFL|Указывает ядру, что при получении процессом сигнала необходимо вызвать системный обработчик, т. e. выполнить действие по умолчанию.|
|SIG\_IGN|Указывает, что сигнал следует игнорировать. Не все сигналы можно игнорировать.|
|Имя ф-ции|Указывает на определенную пользователем функцию-обработчик|

Возвращаемое функцией *signal()* значение может быть различным в различных ОС. В UNIX-подобных ОС, как правило, в случае успешного завершения *signal()* возвращает предыдущую диспозицию - это может быть функция-обработчик сигнала или системные значения SlG\_DFL или SIG\_IGN. Это значение в случае необходимости может быть использовано для  восстановления предыдущей диспозиции после однократного выполнения пользовательского обработчика. Для многократного использования требуется предусмотреть повторный вызов *signal()* в теле обработчика.

Более гибкое управление сигналами предоставляет функция *sigaction()*:

```c
int sigaction( int sig,
               const struct sigaction \* act,
               struct sigaction \* oact );
```

Данная функция позволяет вызывающему процессу получить информацию или установить (или и то и другое) действие, соответствующее какому-либо сигналу или группе сигналов. При этом каждый сигнал ассоциируется с битом 32-х/(64-х) –разрядного слова-маски, соответствующим номеру сигнала. 

Аргумент *sig* определяет тип сигнала (все типы сигналов определены в библиотеке signal.h). 
Аргумент *act* – если он не нулевой, то действие при  указанном сигнале изменятся.
Аргумент *oact* - если он не нулевой, то предыдущее действие при  указанном сигнале сохраняется в структуре типа *sigaction*, на которую указывает указатель *oact*.

Комбинация *act* и *oact* позволяет запрашивать или устанавливать новые действия при поступлении сигнала.

Состав структуры *sigaction*:

|void (\*sa\_handler)()|<p>адрес обработчика сигнала или </p><p>действие для незапрашиваемых сигналов</p>|
| - | - |
|<p>void (\*sa\_sigaction)(int signo, </p><p>siginfo\_t\* info,  void\* other)</p>|<p>адрес обработчика сигнала или </p><p>действие для запрашиваемых сигналов</p>|
|sigset\_t sa\_mask|дополнительный набор сигналов, который будет заблокирован при обработке поступившего сигнала|
|int sa\_flags|специальные флаги для воздействия на режим работы сигнала|

Компоненты `sa\_handler` и `sa\_sigaction` структуры `sigaction` вызываются со следующими аргументами:

```c
void handler(int signo, siginfo\_t\* info, void\* other); 
```

если обработчик сигнала представлен:	`void handler(int signo)`, 

то аргументы `siginfo\_t\* info` и `void\* other` будут игнорироваться.

Для работы с сигналами реального времени существует дополнительный набор функций. 

Выбор функции управления сигналами определяет свойства сигнала как средства IPC: *signal()* иногда обеспечивает так называемую *ненадежную* передачу сигнала, тогда как *sigaction()* гарантирует *надежную* передачу. Последнее означает, что если при возникновении сигнала система занята обработкой другого сигнала (назовем его «текущим»), то возникший сигнал не будет потерян, а его обработка будет отложена до окончания текущего обработчика. 

Разберем эти свойства более подробно **на примерах**.

**Ненадежные сигналы**

Пример. ТЗ: Создать программу, позволяющую изменить диспозицию сигналов, а именно,  установить:  
```
- обработчик пользовательских сигналов SIGUSR1 и SIGUSR2;
- реакцию по умолчанию на сигнал SIGINT;
- игнорирование сигнала SIGCHLD;
```

Породить процесс-копию и уйти в ожидание сигналов. Обработчик сигналов должен содержать восстановление диспозиции и оповещение на экране о полученном (удачно или неудачно) сигнале и идентификаторе родительского процесса. Процесс-потомок, получив идентификатор родительского процесса, должен отправить процессу-отцу сигнал SIGUSR1 и извещение об удачной или неудачной отправке указанного сигнала. Остальные сигналы можно сгенерировать из командной строки.

**Исходный код** (в файле sigExam.c):

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

static void sigHandler(int sig) {
`  `printf("Catched signal %s\n",sig == SIGUSR1 ? "SIGUSR1": "SIGUSR2");
`	`printf("Parent = %d\n",getppid());
`	`// востанавливаем старую диспозицию
`	`signal(sig,SIG\_DFL);
}

int main() {
`   `printf("\nFather started: pid = %i,ppid = %i\n",getpid(),getppid());
`	`signal(SIGUSR1,sigHandler);
`	`signal(SIGUSR2,sigHandler);
`	`signal(SIGINT,SIG\_DFL);
`	`signal(SIGCHLD,SIG\_IGN);
`	`int forkRes = fork();
`	`if(forkRes == 0) {

`	`// программа-потомок
`	`printf("\nSon started: pid = %i,ppid = %i\n",getpid(),getppid());
`	`// отправляем сигналы родителю
`	`if(kill(getppid(),SIGUSR1) != 0) {
`		`printf("Error while sending SIGUSR1\n");
`		`exit(1);
`	`}

`	`printf("Successfully sent SIGUSR1\n");
`	`return 0;
`	`}

`	`// программа-родитель
`	`wait(NULL);
`	`// ждем сигналов
`	`for(;;){
`		`pause();
`	`}	
`	`return 0;
}
```

Обе программы (родителя и потомка) зададим в одном файле. С одной стороны, это делает код более компактным, с другой – упрощает наследование за счет использования только *fork()* –вызова и позволяет потомку скопировать диспозицию родителя.

Ветвление происходит сразу же за вызовом fork(). Если он вернул 0, значит, выполняется код программы-сына, иначе — код программы отца.

Скомпилируем и выполним программу:

dе@dе:~/lab4$ cc -o sigExam sigExam.c

dе@dе:~/lab4$ ./sigExam 

Father started: pid = 14589,ppid = 12231

Son started: pid = 14590,ppid = 14589

Successfully sent SIGUSR1

Catched signal SIGUSR1

Parent = 12231

Процесс-потомок отправил сигнал SIGUSR1, а процесс-отец его успешно принял. Отправим еще 3 сигнала процессу-отцу: SIGCHLD, SIGUSR2, SIGINT:

dе@dе:~/lab4/sig$ kill -SIGUSR2 14589

dе@dе:~/lab4/sig$ kill -SIGCHLD 14589

dе@dе:~/lab4/sig$ kill -SIGINT 14589

Результат:

Catched signal SIGUSR2

Parent = 12231

de@de:~/lab4/sig$ 

Сигнал SIGUSR2 также был «пойман», на сигнал SIGCHLD не последовало никакой реакции (так как он был проигнорирован), и сигнал SIGINT привел к завершению работы.

Запустим программу еще раз и дважды отправим ей сигнал SIGUSR2:

de@de:~/lab4/sig$ ./sigExam 

Father started: pid = 16225,ppid = 12231

Son started: pid = 16226,ppid = 16225

Successfully sent SIGUSR1

Catched signal SIGUSR1

Parent = 12231

Catched signal SIGUSR2

Parent = 12231

User defined signal 2

В результате первый сигнал был «пойман», второй обработался по умолчанию. Это происходит потому, что в обработчике прерываний после первого приема сигнала происходит восстановление диспозиции сигналов. Аналогичная ситуация была бы при двукратной отправке процессу сигнала SIGUSR1.

**Самостоятельно** 

повторите эксперимент для других сигналов и процессов, порождаемых в разных файлах; а также для потоков одного и разных процессов.

**Надежные сигналы**

**Пример.** 

ТЗ: Создать программу, позволяющую продемонстрировать возможность отложенной обработки (временного блокирования) сигнала (например, SIGINT). 

Вся необходимая для управления сигналами информация передается через указатель на структуру *sigaction*. Блокировку реализуем, вызвав "засыпание" процесса на одну минуту из обработчика пользовательских сигналов. В основной программе установим диспозицию этих сигналов. С рабочего терминала отправим процессу sigact сигнал SIGUSR1 или SIGUSR2, а затем сигнал SIGINT. 

**Исходный код** (sigact.c):

#include <stdio.h>

#include <signal.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <unistd.h>

#include <stdlib.h>

#include <fcntl.h>

void (\*mysig(int sig,void (\*hnd)(int)))(int) {

`	`// надежная обработка сигналов

`	`struct sigaction act,oldact;

`	`act.sa\_handler = hnd;

`	`sigemptyset(&act.sa\_mask);

`	`sigaddset(&act.sa\_mask,SIGINT);

`	`act.sa\_flags = 0;

`	`if(sigaction(sig,&act,0) < 0)

`		`return SIG\_ERR;

`	`return act.sa\_handler;

}

void hndUSR1(int sig) {

`	`if(sig != SIGUSR1) {

`		`printf("Catched bad signal %d\n",sig);

`		`return;

`	`}

`	`printf("SIGUSR1 catched\n");

`	`sleep(60);

}

int main() {

`	`mysig(SIGUSR1,hndUSR1);

`	`for(;;) {

`		`pause();

`	`}

`	`return 0;

}

**Результаты выполнения**:

de@de:~/lab4/sig$ cc -w -o sigact sigact.c

de@de:~/lab4/sig$ ./sigact &

[1] 25329

de@de:~/lab4/sig$ kill -SIGUSR1 %1

SIGUSR1 catched

de@de:~/lab4/sig$ kill -SIGINT %1

de@de:~/lab4/sig$ jobs

[1]+  Running                 ./sigact &

de@de:~/lab4/sig$ jobs

[1]+  Interrupt               ./sigact

Чтобы иметь возможность отправить сигналы с терминала следует запустить программу в фоновом режиме. По результатам сигнал SIGUSR1 принят корректно, но после посылки сигнала SIGINT программа продолжала выполняться еще минуту, и только после этого завершилась. В этом отличие надежной обработки сигналов от ненадежной: есть возможность отложить прием некоторых других сигналов. Отложенные таким образом сигналы записываются в маску PENDING и обрабатываются после завершения обработки сигналов, которые отложили обработку. Механизм ненадёжных сигналов не позволяет откладывать обработку других сигналов (можно лишь установить игнорирование некоторых сигналов на время обработки).

**Пример**

ТЗ: Изменить обработчик сигнала так, чтобы из него производилась отправка другого сигнала. 

Пусть из обработчика сигнала SIGUSR1 функцией kill() генерируется сигнал SIGINT. Проанализируем наличие и очередность обработки сигналов.

**Исходный код** программы:

#include <stdio.h>

#include <signal.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <unistd.h>

#include <stdlib.h>

#include <fcntl.h>

void (\*mysig(int sig,void (\*hnd)(int)))(int) {

`	`// надежная обработка сигналов

`	`struct sigaction act,oldact;

`	`act.sa\_handler = hnd;

`	`sigemptyset(&act.sa\_mask);

`	`sigaddset(&act.sa\_mask,SIGINT);

`	`act.sa\_flags = 0;

`	`if(sigaction(sig,&act,0) < 0)

`		`return SIG\_ERR;

`	`return act.sa\_handler;

}

void hndUSR1(int sig) {

`	`if(sig != SIGUSR1) {

`		`printf("Catched bad signal %d\n",sig);

`		`return;

`	`}

`	`printf("SIGUSR1 catched, sending SIGINT\n");

`	`kill(getpid(),SIGINT);

`	`sleep(10);

}

int main() {

`	`mysig(SIGUSR1,hndUSR1);

`	`for(;;) {

`		`pause();

`	`}

`	`return 0;

}

**Результаты выполнения** программы:

de@de:~/lab4/sig$ cc -w -o sigact2 sigact2.c

de@de:~/lab4/sig$ ./sigact2 &

[1] 28822

de@de:~/lab4/sig$ kill -SIGUSR1 %1

de@de:~/lab4/sig$ SIGUSR1 catched, sending SIGINT

jobs

[1]+  Running                 ./sigact2 &

de@de:~/lab4/sig$ jobs

[1]+  Running                 ./sigact2 &

de@de:~/lab4/sig$ kill -SIGINT %1

[1]+  Interrupt               ./sigact2

При генерации сигнала (в данном случае SIGINT) из обработчика другого сигнала обработка сгенерированного сигнала задерживается до конца выполнения текущего обработчика (как и в предыдущем эксперименте).

# <a name="_toc190170008"></a>**Сигналы POSIX реального времени**
`	`Некоторые реализации POSIX ОС могут обрабатывать все сигналы как сигналы реального времени, но для UNIX-подобных ОС это не является обязательным. Если мы хотим, чтобы сигналы ***гарантированно*** обрабатывались как сигналы реального времени, мы должны: 

- использовать сигналы с номерами в диапазоне от SIGRTMIN до SIGRTMAX 
- должны указать флаг SA\_SIGINFO при вызове ***sigaction()*** с установкой обработчика сигнала
- обработчик сигнала реального времени, устанавливаемый с флагом SA\_SIGINFO, объявляется как:

  void func(int *signo,* siginfo\_t *\*info,* void *\*context*);   где

  *signo—* номер сигнала, 

  siginfo\_t — структура, определяемая как

  typedef struct {

  ` 	`int si\_signo; /\* то же, что и signo \*/

  `	`int si\_code; /\* SI\_{USER,QUEUE,TIMER,ASYNCIO,MESGQ} \*/

  ` 	`union sigval si\_value; /\* целое или указатель от отправителя \*/

  `	`} siginfo\_t;

  на что указывает *context* — зависит от реализации.

  Таким образом, сигналы реального времени несут больше информации, чем прочие сигналы (при отправке сигнала, не обрабатываемого как сигнал реального времени, единственным аргументом обработчика является номер сигнала).

- SIGRTMIN и SIGRTMAX – это еще и макросы  (вызывающие *sysconf*), которые позволяют изменять сами эти значения.

` `«**Характеристики сигналов реального времени**» означает следующее:

- Сигналы помещаются в очередь. 
- Если сигнал будет порожден несколько раз, он будет несколько раз получен адресатом. Более того, повторения одного и того же сигнала доставляются в порядке очереди (FIFO). Если же сигналы в очередь не помещаются, неоднократно порожденный сигнал будет получен лишь один раз.
- Когда в очередь помещается множество неблокируемых сигналов в диапазоне SIGRTMIN—SIGRTMAX, сигналы с меньшими номерами доставляются раньше сигналов с большими номерами. То есть сигнал с номером SIGRTMIN имеет «больший приоритет», чем сигнал с номером SIGRTMIN+1, и т.д.

**Самостоятельно**

1) проведите эксперимент, позволяющий определить возможность организации *очереди для различных типов сигналов,* обычных и реального времени,* (более двух сигналов, для этого увеличьте «вложенность» вызовов обработчиков);
1) экспериментально подтвердите, что обработка равноприоритетных сигналов реального времени происходит в порядке FIFO;
1) опытным путем подтвердите наличие приоритетов сигналов реального времени. 

`	`**Каналы**

Различают **два типа** каналов анонимные (иначе их называют «программные» или «неименованные») и именованные. Они по-разному реализованы, но доступ к ним организуется одинаково с помощью обычных функций read и write (унифицированный подход по типу *файловой модели*). Одним из свойств программных каналов и FIFO является то, что данные по ним передаются в виде потоков байтов (аналогично соединению TCP). Деление этого потока на самостоятельные записи полностью предоставляется приложению (в отличие, например, от очередей сообщений, которые автоматически расставляют границы между записями, аналогично тому, как это делается в дейтаграммах UDP). 

Рассмотрим оба типа каналов более детально с **примерами программ**.

# <a name="_toc190170009"></a>**2. Неименованные каналы**
Программные (неименованные) каналы – однонаправленные, используются только для связи родственных процессов,  в принципе могут использоваться и неродственными процессами, если предоставить им возможность передавать друг другу дескрипторы (т.к. имен они не имеют). Неименованный канал создается посредством системного вызова pipe(2), который возвращает 2 файловых дескриптора filedes[1] для записи в канал и filedes[0] для чтения из канала: 

#include

int pipe(int fd[2]);

/\* возвращает 0 в случае успешного завершения. –1 – в случае ошибки:\*/

Доступ к дескрипторам канала может получить как процесс, вызвавший *pipe()* , так и его дочерние процессы. Канал создается одним процессом, может использоваться им единолично (но редко). Как правило, это средство применяется для связи между двумя процессами, следующим образом: процесс создает канал, а затем вызывает *fork()*, создавая свою копию — дочерний процесс. Затем родительский процесс закрывает открытый для чтения конец канала, а дочерний, в свою очередь, — открытый на запись конец канала. Это обеспечивает одностороннюю передачу данных между процессами. При необходимости передачи данных в обе стороны нужно создавать пару каналов и использовать каждый из них для передачи данных в одну сторону. 

**Пример.**

ТЗ: Организуем программу (файл pipe.c) так, чтобы процесс-родитель создавал  неименованный канал, создавал потомка, закрывал канал на запись и записывал в произвольный текстовый файл считываемую из канала информацию. В функции процесса-потомка будет входить считывание данных из файла и запись их в канал. (Функционирование осуществляется через стандартные потоки ввода/вывода, как было показано выше).

**Алгоритм** работы программы:

![A description...](data:image/jpeg;base64,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)

*Рис. 1. Взаимодействие посредством неименованных каналов*

**Исходный текст** программы (pipe.c):

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#define DEF\_F\_R "from.txt"

#define DEF\_F\_W "to.txt"

int main(int argc, char\*\* argv) {

`	`char fileToRead[32];

`	`char fileToWrite[32];

`    `if(argc < 3 ){

`	`printf("Using default fileNames '%s','%s'\n",DEF\_F\_R,DEF\_F\_W);

`	`strcpy(fileToRead,DEF\_F\_R);

`	`strcpy(fileToWrite,DEF\_F\_W);

`	`}

`	`else {

`	`strcpy(fileToRead,argv[1]);

`	`strcpy(fileToWrite,argv[2]);

`	`}

`	`int filedes[2];

`	`if(pipe(filedes) < 0) {

`		`printf("Father: can't create pipe\n");

`		`exit(1);

`	`}

`	`printf("pipe is successfully created\n");

`	`if(fork() == 0) {

`	`// процесс сын

`		`// закрывает пайп для чтения

`		`close(filedes[0]);

`	 	`FILE\* f = fopen(fileToRead,"r");

`		`if (!f) {

`			`printf("Son: cant open file %s\n",fileToRead);

`			`exit(1);

`		`}

`		`char buf[100];

`		`int res;

`		`while(!feof(f)) {

`			`// читаем данные из файла

`			`res = fread(buf,sizeof(char),100,f);

`			`write(filedes[1],buf,res); // пишем их в пайп

`		`}		

`		`close(f);

`		`close(filedes[1]);

`		`return 0;

`	`}

`	`// процесс отец

`		`// закрывает пайп для записи

`	`close(filedes[1]);

`	`FILE \*f = fopen(fileToWrite,"w");

`	`if (!f) {

`		`printf("Father: cant open file %s\n",fileToWrite);

`		`exit(1);

`	`}

`	`char buf[100];

`	`int res;	

`	`while(1) {

`		`bzero(buf,100);

`		`res = read(filedes[0],buf,100);

`		`if(!res)

`			`break;

`		`printf("Read from pipe: %s\n",buf);

`		`fwrite(buf,sizeof(char),res,f);

`	`}

`	`fclose(f);

`	`close(filedes[0]);

`	`return 0;

}

**Результат выполнения**:

dm@dm:~/lab4/pipe$ cc -w -o pipe pipe.c

dm@dm:~/lab4/pipe$ ./pipe 

Using default fileNames 'from.txt','to.txt'

pipe is successfully created

Read from pipe: first string

second

third

That's all

dm@dm:~/lab4/pipe$ 

Содержимое файла from.txt:

first string

second

third

That's all

Содержимое файла from.txt успешно переписалось в изначально пустой файл to.txt с использованием неименованного канала. 

Содержимое файла to.txt после запуска программы:

first string

second

third

That's all

Так как процесс-родитель только читает из канала, то дескриптор для записи (filedes[1]) он закрывает, аналогично процесс-сын в начале работы закрывает дескриптор для чтения из канала (filedes[0]).

Главное применение неименованных каналов в ОС Unix – реализация конвейеров команд в интерпретаторах командной строки. 

Например, при выполнении конвейера 

ls|sort|grep

интерпретатор создаст три процесса с двумя каналами между ними так, что открытый для чтения конец каждого канала будет подключен к стандартному потоку ввода, а открытый на запись — к стандартному потоку вывода. 

# `	`<a name="_toc190170010"></a>**3. Именованные каналы**
`	`Именованные каналы в Unix функционируют подобно неименованным — они позволяют передавать данные только в одну сторону. Однако в отличие от неименоаванных каналов каждому каналу FIFO сопоставляется полное имя в файловой системе, что позволяет двум неродственным процессам обратиться к одному и тому же FIFO. Аббревиатура FIFO расшифровывается как «first in, first out» — «первым вошел, первым вышел», то есть эти каналы работают как очереди.

`	`После создания канал FIFO должен быть открыт на чтение или запись с помощью либо функции open, либо одной из стандартных функций открытия файлов из библиотеки ввода-вывода (например, fopen). FIFO может быть открыт либо только на чтение, либо только на запись. Нельзя открывать канал на чтение и запись одновременно, поскольку именованные каналы могут быть только односторонними. 

**Пример.**

ТЗ: Создать клиент-серверное приложение, демонстрирующее дуплексную (двунаправленную) передачу информации двумя однонаправленными именованными каналами между клиентом и сервером.

В файле *server.c* в основной программе:

создадим 2 именованных канала, используя системный вызов mknod(), аргументы которого: имя файла FIFO в файловой системе; флаги владения, прав доступа (установим открытые для всех права доступа на чтение и на запись S\_IFIFO | 0666). Откроем один канал на запись (chan1), другой - на чтение (chan2) и запустим серверную часть программы.

В серверной части программы:

запишем имя файла в канал 1 (для записи) функцией write(); прочитаем данные из канала 2 и выведем на экран.

В файле  client.c запрограммируем функции: открытия каналов для чтения (chan1) и записи (chan2). Из первого канал читается имя файла, во второй канал пишется его содержимое.** Алгоритм работы показан ниже на рис. 2. 

**Исходный код** программы server.c

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#define DEF\_FILENAME "testFile.txt"

int main(int argc, char\*\* argv) {

`	`char fileName[30];

`	`if(argc < 2) {

`		`printf("Using default file name '%s'\n",DEF\_FILENAME);	

`		`strcpy(fileName,DEF\_FILENAME);

`	`}

`	`else 

`		`strcpy(fileName,argv[1]);

`	`// создаем два канала

`	`int res = mknod("channel1",S\_IFIFO | 0666,0);

`	`if(res) {

`		`printf("Can't create first channel\n");

`		`exit(1);

`	`}

`	`res = mknod("channel2",S\_IFIFO | 0666,0);

`	`if(res) {

`		`printf("Can't create second channel\n");

`		`exit(1);

`	`}

`	`// открываем первый канал для записи

`	`int chan1 = open("channel1",O\_WRONLY);

`	`if(chan1 == -1) {

`		`printf("Can't open channel  for writing\n");

`		`exit(0);

`	`}

`	`// открываем второй канал для чтения

`	`int chan2 = open("channel2",O\_RDONLY);

`	`if(chan2 == -1) {

`		`printf("Can't open channe2 for reading\n");

`		`exit(0);

`	`}

`	`// пишем имя файла в первый канал

`	`write(chan1,fileName,strlen(fileName));

`	`// читаем содержимое файла из второго канала

`	`char buf [100];

`	`for(;;) {

`		`bzero(buf,100);

`		`res = read(chan2,buf,100);

`		`if(res <= 0)

`			`break;

`		`printf("Part of file: %s\n");

`	`}

`	`close(chan1);

`	`close(chan2);

`	`unlink("channel1");

`	`unlink("channel2");

`	`return 0;

}

**Исходный код** программы client.c

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

int main() {

`	`// каналы сервер уже создал, открываем их

`	`int chan1 = open("channel1", O\_RDONLY);

`	`if(chan1 == -1) {

`		`printf("Can't open channel1 for reading\n");

`		`exit(0);

`	`} 	

`	`int chan2 = open("channel2", O\_WRONLY);

`	`if(chan2 == -1) {

`		`printf("Can't open channel2 for reading\n");

`		`exit(0);

`	`}

`	`// читаем имя файла из первого канала

`	`char fileName[100];

`	`bzero(fileName,100);

`	`int res = read(chan1,fileName,100);

`	`if(res <= 0) {

`		`printf("Can't read fileName from channel1\n");

`		`exit(0);

`	`}

`	`// открываем файл на чтение

`	`FILE \*f = fopen(fileName,"r");

`	`if(!f) {	

`		`printf("Can't open file %s\n",fileName);

`		`exit(0);

`	`}

`	`// читаем из файла и пишем во второй канал

`	`char buf[100];

`	`while(!feof(f)) {

`		`// читаем данные из файла

`		`res = fread(buf,sizeof(char),100,f);

`		`// пишем их в канал

`		`write(chan2,buf,res);

`	`}

`		`fclose(f);

`		`close(chan1);

`		`close(chan2);

`	`return 0;

}

Запустим программу *server* из одного терминала и *client* из другого:

**Результат работы сервера**:

de@de:~/lab4/fifo$ ./server 

Using default file name 'testFile.txt'

Part of file: first string

second

Last string

de@de:~/lab4/fifo$ 

**Результат работы клиента**:

de@de:~/lab4/fifo$ ./client 




![A description...](data:image/jpeg;base64,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SclO/OcZouL+AMcKVgHdTNZVnA8FKxmmaDNMCsZpmgzTArGaZoM0PCsZryokHHh81SQc+/wcf3HbJ+PK/vLtdErnX+Dh+49Zckb1ysef9cu10WqFKUoFKUoPi84GmluuK2UJSVKPgAqmo5TNNkq2G7q4AojbRb3FJJG7ccb6s+oUgWKeMf8Au7n9U1C8ljDCtB2wllskpXnKR90VQandL09973j6Od9VO6Xp773vH0c76quXY7H3Fv5Ip2Ox9xb+SKCm90vT33vePo531U7penvve8fRzvqq5djsfcW/kivKmWAccy18kUFP7penvve8fRzvqp3S9Pfe94+jnfVVuLTOcBhrdx70U5pjH7C1njjZFDVR7penvve8fRzvqp3S9Pfe94+jnfVVvDLJ/wAw1x/gCgbYP/u7fDPuRRNVDul6e+97x9HO+qndL09973j6Od9VW7m2T/mG93HvBTm2ePMNY/FFTVVHul6e+97x9HO+qndL09973j6Od9VW7mmethobv4ApzTP3Br5IpoqPdL09973j6Od9VO6Xp773vH0c76qt/NMZwGGvkjfTmmd47HayP4oqilv8ounHWVtORbypKwUK/wAWu8D8FU7Rd109YL5cpr0a7EOObMfFuc3IIB8HhJrsnNMEE8y0U+HZG+hYZI3Mtbv4grrz5bzzeZ/rn1xOrtVDuk6eCh+trxuHi531Vnul6e+97x9HO+qrcllrO9hoebZBrPMs8eYbPm2BXJvVQ7penvve8fRzvqp3S9Pfe94+jnfVVu5tnG9hof7IxQtMceYbA/EFF1Ue6Xp773vH0c76qd0vT33vePo531VbubY6mWgfOgVktM7hzDXh9yKmiod0vT33vePo531U7penvve8fRzvqq3BpgjPMtY6jsinNsZwGGsjj3oqio90vT33vePo531U7penvve8fRzvqq3Fpnf9ga+SKc2zk/YWcD+KKCo90vT33vePo531U7penvve8fRzvqq282zvJZZA/FFe0MsKGeYax1HZG+gp/dL09973j6Od9VO6Xp773vH0c76quXY7H3Fv5Ip2Ox9xb+SKCm90vT33vePo531VIac1nZ9QXNdugGW3JQzzxRIjqaJTtAZAUN+81Yux2PuLfyRVSKEI5Y29hCU/5Pq4DH/vCaC3jGfhr1XlCQOFeqBSlKBVZ1xpp+/mC9EuC4Mq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)

*Рис.2. Взаимодействие посредством именованных каналов*

В результате выполнения программы содержимое файла testFile.txt:

first string

second

Last string

Сервер создает два канала, записывает в один из них имя файла и ждёт данные от клиента. Каналы создаются в рабочей папке сервера, и использовать их может любой процесс, а не только дочерний по отношению к серверу.  Клиент после запуска также открывает уже созданные каналы, считывает имя файла и отсылает серверу его содержимое, используя второй канал. После завершения передачи, сервер уничтожает каналы с помощью функции unlink().

`	`Несмотря на то, что именованные каналы являются отдельным типом файлов и могут быть видимы разными процессами даже в распределенной файловой системе,

использование FIFO для взаимодействия удаленных процессов и обмена информацией между ними невозможно. Так как и в этом случае для передачи данных задействовано ядро. Создаваемый файл служит для получения данных о расположении FIFO в адресном пространстве ядра и его состоянии. 

`	`Продемонстрируем это **на примере**. Изменим ранее использованную программу так, чтобы сервер, перед тем как читать данные из канала, ожидал ввода пользователя. Исходный код клиента оставим неизменным.

**Исходный код сервера**:

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#define DEF\_FILENAME "testFile.txt"

int main(int argc, char\*\* argv) {

`	`char fileName[30];

`	`if(argc < 2) {

`		`printf("Using default file name '%s'\n",DEF\_FILENAME);	

`		`strcpy(fileName,DEF\_FILENAME);

`	`}

`	`else 

`		`strcpy(fileName,argv[1]);

`	`// создаем два канала

`	`int res = mknod("channel1",S\_IFIFO | 0666,0);

`	`if(res) {

`		`printf("Can't create first channel\n");

`		`exit(1);

`	`}

`	`res = mknod("channel2",S\_IFIFO | 0666,0);

`	`if(res) {

`		`printf("Can't create second channel\n");

`		`exit(1);

`	`}

`	`// открываем первый канал для записи

`	`int chan1 = open("channel1",O\_WRONLY);

`	`if(chan1 == -1) {

`		`printf("Can't open channel  for writing\n");

`		`exit(0);

`	`}

`	`// открываем второй канал для чтения

`	`int chan2 = open("channel2",O\_RDONLY);

`	`if(chan2 == -1) {

`		`printf("Can't open channe2 for reading\n");

`		`exit(0);

`	`}

`	`// пишем имя файла в первый канал

`	`write(chan1,fileName,strlen(fileName));

`	`// читаем содержимое файла из второго канала

`	`char buf [100];

`	`printf("Waiting for clint write to channnel\n");

`	`getchar();

`	`for(;;) {

`		`bzero(buf,100);

`		`res = read(chan2,buf,100);

`		`if(res <= 0)

`			`break;

`		`printf("Part of file: %s\n");

`	`}

`	`close(chan1);

`	`close(chan2);

`	`unlink("channel1");

`	`unlink("channel2");

`	`printf("Servr finished\n");

`	`return 0;

}

Запустим клиент и сервер. Пока сервер ожидает ввода, посмотрим размер файла канала.

Сервер:

de@de:/lab4/4-fifodop$ ./server

Using default file name 'testFile.txt'

Waiting for clint write to channnel

Клиент:

de@de:/lab4/4-fifodop$ ./client

Client finished

de@de:/lab4/4-fifodop$ 

Размер файла каналов:

de@de:/lab4/4-fifodop$ ls -sl | grep chan

0 prwxrwx--- 1 root plugdev    0 Dec 26 22:18 channel1

0 prwxrwx--- 1 root plugdev    0 Dec 26 22:18 channel2

`	`Размер файла канала не изменяется, несмотря на записанные данные, это свидетельствует о том, что файл используется не как хранилище пересылаемых данных, а только для получения информации системой о них. Сами данные проходят через ядро ОС.

Позволим серверу выполняться дальше, нажав на *enter*, и убедимся, что данные получены:

Waiting for clint write to channnel

Part of file: first string

second

Last string

Servr finished

de@de:/lab4/4-fifodop$ 

В первую строку вывода умышленно внесены орфографические ошибки для идентификации работы измененного сервера.

**Ограничения каналов**

**На неименованные каналы и каналы FIFO** системой накладываются всего **два ограничения**: OPEN\_MAX — максимальное количество дескрипторов, которые могут быть одновременно открыты некоторым процессом (POSIX устанавливает для этой величины ограничение снизу); PIPE\_BUF — максимальное количество данных, для которого гарантируется атомарность операции записи (POSIX требует по менее 512 байт).

Значение OPEN\_MAX можно узнать, вызвав функцию sysconf, его можно изменить из интерпретатора команд или из процесса. Значение PIPE\_BUF обычно определено в заголовочном файле. Для  FIFO с точки зрения стандарта POSIX оно представляет собой переменную (ее значение можно получить в момент выполнения программы), зависимую от полного имени файла, поскольку разные имена могут относиться к разным файловым системам, и эти файловые системы могут иметь различные характеристики. 

# <a name="_toc190170011"></a>**4. Очереди сообщений**

Очередь сообщений находится в адресном пространстве ядра и имеет ограниченный размер. В отличие от каналов, которые обладают теми же самыми свойствами, очереди сообщений сохраняют границы сообщений. Это значит, что ядро ОС гарантирует, что сообщение, поставленное в очередь, не смешается с предыдущим или следующим сообщением при чтении из очереди. Кроме того, с каждым сообщением связывается его тип. Процесс, читающий очередь сообщений, может отбирать только сообщения заданного типа или все сообщения кроме сообщений заданного типа.

Очередь сообщений можно рассматривать как связный список сообщений. Каждое сообщение представляет собой запись, очереди сообщений автоматически расставляют границы между записями, аналогично тому, как это делается в дейтаграммах UDP. Для записи сообщения в очередь не требуется наличия ожидающего его процесса в отличие от неименованных каналов и FIFO, в которые нельзя произвести запись, пока не появится считывающий данные процесс. Поэтому процесс может записать в очередь какие-то сообщения, после чего они могут быть получены другим процессом в любое время, даже если первый завершит свою работу. С завершением процесса-источника данные не исчезают (данные, остающиеся в именованном или неименованном канале, сбрасываются, после того как все процессы закроют его).

Следует заметить, что, к сожалению, не определены системные вызовы, которые позволяют читать сразу из нескольких очередей сообщений, или из очередей сообщений и файловых дескрипторов. Видимо, отчасти и поэтому очереди сообщений широко не используются.

**Пример.** 

ТЗ: Создать клиент-серверное приложение, демонстрирующее передачу информации между процессами посредством очередей сообщений.

Аналогично предыдущему разделу программа включает 2 файла: серверный и клиентский. В общем случае одновременно могут работать несколько клиентов. 

**Серверный** файл содержит:

\- подключение библиотек (см. листинг ниже)

\- обработчик сигнала SIGINT (с восстановлением диспозиции и удалением очереди сообщений системным вызовом  msgctl() для корректного завершения сервера при получении сигнала SIGINT);

\- основную программу со следующей структурой: 


void main(void) 
{ 
` `Message msg\_rcv; //принимаемое сообщение 
` `Message msg\_snd; //посылаемое сообщение 
` `key\_t key;  //ключ, необходимый для создания очереди 
` `int length, n; 
` `signal(SIGINT, sig\_hndlr); 
` `//получение ключа 
` `if((key = ftok("/home/your\_path/test.txt", 'A')) < 0) 

` `//ftok - преобразует имя файла и идентификатор проекта в ключ для системных вызовов (для работы с очередью)

` `{ 
`  `printf("Server : can't receive a key\n"); 
`  `exit(-1); 
` `} 


далее создается очередь сообщений, используя системный вызов 
`   `msgget(key, PERM | IPC\_CREAT), 
` `организовывается цикл ожидания сообщения и его чтение. 
Сервер в цикле читает сообщения из очереди (тип = 1) функцией msgrcv() и посылает на каждое сообщение ответ клиенту (тип = 2) функцией  msgsnd() . Целесообразно дублировать вывод сообщений на экран для контроля. В случае возникновения любых ошибок функцией kill()  инициируется посылка сигнала SIGINT. Обработчик сигнала выполняет восстановление диспозиции сигналов и удаление очереди сообщений системным вызовом msgctl().

`	`В файле **client.c**  аналогично серверному коду должен быть получен  ключ, затем доступ к очереди сообщений, отправка сообщения серверу (тип 1). Затем организовывается цикл ожидания сообщения клиентом с последующим чтением (тип 2). 

`	`Таким образом, функции чтения и отправки сообщения реализуются системными вызовами:  msgrcv(),msgsnd().

**Алгоритм** работы программы:

![](data:image/png;base64,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)

*Рис.3. Взаимодействие посредством именованных каналов*

**Исходный код**:** server.c

#include <stdio.h>

#include <stdlib.h>

#include <sys/ipc.h>

#include <sys/msg.h>

#include <sys/types.h>

#include <signal.h>

#define DEF\_KEY\_FILE "key"

typedef struct {

`	`long type;

`	`char buf[100];

} Message;

int queue;

void intHandler(int sig) {

`	`signal(sig,SIG\_DFL);

`	`if(msgctl(queue,IPC\_RMID,0) < 0) {

`		`printf("Can't delete queue\n");

`		`exit(1);

`	`}

}

int main(int argc, char\*\* argv) {

`	`char keyFile[100];

`	`bzero(keyFile,100);

`	`if(argc < 2) {

`		`printf("Using default key file %s\n",DEF\_KEY\_FILE);

`		`strcpy(keyFile,DEF\_KEY\_FILE);

`	`}

`	`else

`		`strcpy(keyFile,argv[1]);

`	`key\_t key;

`	`key = ftok(keyFile,'Q');

` `if(key == -1) {

`	`printf("no got key for the key file %s and id 'Q'\n",keyFile);

`		`exit(1);

`	`}



`	`queue = msgget(key,IPC\_CREAT | 0666);

`	`if (queue < 0) {

`		`printf("Can't create queue\n");	

`		`exit(4);

`	`}	

// до этого момента вызывали exit(), а не kill, т.к. очередь

`	`// еще не была создана

`	`signal(SIGINT,intHandler);

`	`// основной цикл работы сервера

`	`Message mes;

`	`int res;

`	`for(;;) {

`		`bzero(mes.buf,100);

`		`// получаем первое сообщение с типом 1

`		`res = msgrcv(queue,&mes,sizeof(Message),1L,0);

`		`if(res < 0) {

`			`printf("Error while recving msg\n");

`			`kill(getpid(),SIGINT);

`		`}

`		`printf("Client's request: %s\n",mes.buf);

`		`// шлем клиенту сообщение с типом 2, что все ок

`		`mes.type = 2L;

`		`bzero(mes.buf,100);

`		`strcpy(mes.buf,"OK");

`		`res = msgsnd(queue,(void\*)&mes,sizeof(Message),0);	

`		`if(res != 0) {

`			`printf("error while sending msg\n");

`			`kill(getpid(),SIGINT);

`		`}

`	`}

`	`return 0;

}

**Исходный код** программы **client.c**:

#include <stdio.h>

#include <stdlib.h>

#include <sys/ipc.h>

#include <sys/msg.h>

#include <sys/types.h>

#include <signal.h>

#define DEF\_KEY\_FILE "key"

typedef struct {

`	`long type;

`	`char buf[100];

} Message;

int queue;

int main(int argc, char\*\* argv) {

`	`char keyFile[100];

`	`bzero(keyFile,100);

`	`if(argc < 2) {

`		`printf("Using default key file %s\n",DEF\_KEY\_FILE);

`		`strcpy(keyFile,DEF\_KEY\_FILE);

`	`}

`	`else

`		`strcpy(keyFile,argv[1]);

`	`key\_t key;

`	`key = ftok(keyFile,'Q');

if(key == -1) {

`	`printf("no got key for key file %s and id 'Q'\n",keyFile);

`		`exit(1);

`	`}	

`	`queue = msgget(key,0);

`	`if (queue < 0) {

`		`printf("Can't create queue\n");	

`		`exit(4);

`	`}	

`	`// основной цикл работы программы

`	`Message mes;

`	`int res;

`	`for(;;) {

`		`bzero(mes.buf,100);

`		`// читаем сообщение с консоли

`		`fgets(mes.buf,100,stdin);

`		`mes.buf[strlen(mes.buf) - 1] = '\0';

`		`// шлем его серверу

`		`mes.type = 1L;

`		`res = msgsnd(queue,(void\*)&mes,sizeof(Message),0);

`		`if(res != 0) {

`			`printf("Error while sending msg\n");

`			`exit(1);

`		`}	

`		`// получаем ответ, что все хорошо

`		`res = msgrcv(queue,&mes,sizeof(Message),2L,0);

`		`if(res < 0) {

`			`printf("Error while recving msg\n");

`			`exit(1);

`		`}

`		`printf("Server's response: %s\n",mes.buf);

`	`}

`		`return 0;

}

*Описание работы сервера*: Сервер получает ключ, по имени файла. С помощью ключа и идентификатора = 'Q' получает очередь сообщений и ждет сообщений с типом 1 от клиентов. При получении сообщения сервер выводит его на экран и отсылает обратное сообщение с типом 2, содержащее фразу «ОК».

*Описание работы клиента*: Клиент получает ту же очередь, что и сервер и ждет ввода пользователя. Считав ввод, он шлет сообщение с типом 1, содержащее считанные данные и ожидает от сервера подтверждения о принятии. 

Запустим два процесса *client* и серверный процесс c разных терминалов. **Результаты выполнения**

Сервер:

de@de:~/lab4/msgs$ ./server 

Using default key file key

Client's request: first client

Client's request: second client

Первый клиент:

de@de:~/lab4/msgs$ ./client 

Using default key file key

first client

Server's response: OK

Второй клиент:

de@de:~/lab4/msgs$ ./client 

Using default key file key

second client

Server's response: OK


\*	**Количественные ограничения средств IPC** 

Максимальные и минимальные значения констант можно выяснить различными способами, в частности, просматривая соответствующие файлы каталога **/proc/sys/kernel.**

Наиболее простой способ – воспользоваться  утилитой ***ipcs*** с ключом  ***-l.***

**Пример выполнения команды *ipcs***:

de@de:/lab4/6-shm2s$ ipcs -l



------ Shared Memory Limits --------

max number of segments = 4096

max seg size (kbytes) = 32768

max total shared memory (kbytes) = 8388608

min seg size (bytes) = 1

------ Semaphore Limits --------

max number of arrays = 128

max semaphores per array = 250

max semaphores system wide = 32000

max ops per semop call = 32

semaphore max value = 32767

------ Messages Limits --------

max queues system wide= 1685

max size of message (bytes) = 8192

default max size of queue (bytes) = 16384

`	`Например, если необходимо определить *максимальные* размер сообщения и одномоментное количество сообщений в очереди, то, как видим по результатам вывода утилиты, по умолчанию размер одного сообщения не может быть больше 8192 байт, а очередь не может содержать больше 1685 сообщений в один момент времени.

# <a name="_toc190170012"></a>**5. Семафоры и разделяемая память**

`	`Рассмотрим несколько вариантов постановки задачи синхронизации доступа к разделяемой памяти.

***5.1. Вариант 1***. Есть один процесс, выполняющий запись в разделяемую память и один процесс, выполняющий чтение из нее. Под чтением понимается извлечение данных из памяти. Программа должна обеспечить невозможность повторного чтения одних и тех же данных и невозможность перезаписи данных, т.е. новой записи, до тех пор, пока читатель не прочитает предыдущую. 

В таком варианте задания для синхронизации процессов достаточно двух семафоров.

Покажем, почему не достаточно одного **на примере**.

` `Так как мы используем один семафор, то алгоритм работы читателя и писателя может быть только таким – захват семафора, выполнение действия (чтение / запись), освобождение семафора. Теперь допустим, что читатель прочитал данные, освободил семафор и еще не до конца использовал квант процессорного времени. Тогда он перейдет на новую итерацию, снова захватит только что освобожденный семафор и снова прочитает данные – ошибка.

Теперь покажем, почему достаточно двух семафоров. Придадим одному из них смысл «запись разрешена», т.е. читатель предыдущие данные уже использовал; второму – «чтение разрешено», т.е. писатель уже сгенерировал новые данные, которые нужно прочитать. 

Тогда **алгоритм** работы читателя и писателя будет выглядеть так:

Я семафорами![A description...](data:image/jpeg;base64,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)

*Рис.4. Организация доступа к разделяемой памяти одного «читателя» и одного «писателя»*

Оба семафора бинарные и используют стандартные операции, захват семафора – это ожидание освобождения ресурса (установки семафора в 1) и последующий захват ресурса (установки семафора в 0), освобождение ресурса – это установка семафора в 1.

Пару семафоров, использованных таким образом, иногда называют *разделенным бинарным семафором*, поскольку в любой момент времени только один из них может иметь значение 1.

При таком алгоритме работы, оба процесса после выполнения своей задачи и освобождения одного из семафоров, будут ждать освобождения другого семафора, которое произведет другой процесс, но только после выполнения своей работы. Таким образом повторное чтение, или повторная запись стала невозможной.

**Исходный код** программы:

Процесс-**читатель**:

#include <stdio.h>

#include <stdlib.h>

#include <signal.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/sem.h>

#include <sys/shm.h>

#include <sys/time.h>

#include "shm.h"

Message\* p\_msg;

int shmemory;

int semaphore;

void intHandler(int sig) {

`  `//отключаем разделяемую память

`  `if(shmdt(p\_msg) < 0) {

`    `printf("Error while detaching shm\n");

`    `exit(1);

`  `}

`  `//удаляем shm и семафоры

`  `if(shmctl(shmemory, IPC\_RMID, 0) < 0) {

`    `printf("Error while deleting shm\n");

`    `exit(1);

`  `}

`  `if(semctl(semaphore, 0, IPC\_RMID) < 0) {

`    `printf("Error while deleting semaphore\n");

`    `exit(1);

`  `}

}

int main(int argc, char\*\* argv) {

`	`char keyFile[100];

`	`bzero(keyFile,100);

`	`if(argc < 2) {

`		`printf("Using default key file %s\n",DEF\_KEY\_FILE);

`		`strcpy(keyFile,DEF\_KEY\_FILE);

`	`}

`	`else

`		`strcpy(keyFile,argv[1]);

`  `key\_t key;

`  `//будем использовать 1 и тот же ключ для семафора и для shm

`  `if((key = ftok(keyFile, 'Q')) < 0) {

`    `printf("Can't get key for key file %s and id 'Q'\n",keyFile);

`    `exit(1);

`  `}

`  `//создаем shm

if((shmemory = shmget(key, sizeof(Message), IPC\_CREAT | 0666)) < 0) {

`   	 `printf("Can't create shm\n");

`	 `exit(1);

`  `}

`  `//присоединяем shm в наше адресное пространство

`  `if((p\_msg = (Message\*)shmat(shmemory, 0, 0)) < 0) {

`    `printf("Error while attaching shm\n");

`    `exit(1);

`  `}

`	`// устанавливаем обработчик сигнала

`  `signal(SIGINT, intHandler);

`  `//создаем группу из 2 семафоров

`  `//1 - показывает, что можно читать

`  `//2 - показывает, что можно писать 

`  `if((semaphore = semget(key, 2, IPC\_CREAT | 0666)) < 0) {

`    `printf("Error while creating semaphore\n");

`    `kill(getpid(),SIGINT);

`  `}

`  `// устнавливаем 2 семафор в 1, т.е. можно писать

`    `if(semop(semaphore, setWriteEna, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}

`	`// основной цикл работы

`  `for(;;) {

`	`// ждем пока клиент начнет работу

`    `if(semop(semaphore, readEna, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}

`    `//читаем сообщение от клиента

`    `printf("Client's message: %s", p\_msg->buf);

`    `// говорим клиенту, что можно снова писать

`    `if(semop(semaphore, setWriteEna, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}

`  `}

}

Процесс-**писатель**:

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/sem.h>

#include <sys/shm.h>

#include "shm.h"

int main(int argc, char\*\* argv) {

`  `Message\* p\_msg;

`  `char keyFile[100];

`	`bzero(keyFile,100);

`	`if(argc < 2) {

`		`printf("Using default key file %s\n",DEF\_KEY\_FILE);

`		`strcpy(keyFile,DEF\_KEY\_FILE);

`	`}

`	`else

`		`strcpy(keyFile,argv[1]);

`  `key\_t key;

int shmemory;

int semaphore;

` `//будем использовать 1 и тот же ключ для семафора и для shm

`  `if((key = ftok(keyFile, 'Q')) < 0) {

`    `printf("Can't get key for key file %s and id 'Q'\n",keyFile);

`    `exit(1);

`  `}

`  `//создаем shm

`  `if((shmemory = shmget(key, sizeof(Message), 0666)) < 0) {

`   	 `printf("Can't create shm\n");

`	 `exit(1);

`  `}

`  `//присоединяем shm в наше адресное пространство

`  `if((p\_msg = (Message\*)shmat(shmemory, 0, 0)) < 0) {

`    `printf("Error while attaching shm\n");

`    `exit(1);

`  `}

`  `if((semaphore = semget(key, 2, 0666)) < 0) {

`    `printf("Error while creating semaphore\n");

`     `exit(1);

`  `}

`  	`char buf[100];

`	`for(;;) {

`		`bzero(buf,100);

`	`printf("Type message to serever. Empty string to finish\n");

`		`fgets(buf,100,stdin);

`		`if(strlen(buf) == 1 && buf[0] == '\n') {

`			`printf("bye-bye\n");

`			`exit(0);

`		`}

`  `//хотим отправить сообщение

`  `if(semop(semaphore, writeEna, 1) < 0) {

`    `printf("Can't execute a operation\n");

`    `exit(1);

`  `}

`  `//запись сообщения в разделяемую память

`  `sprintf(p\_msg->buf,"%s", buf);

`  `//говорим серверу, что он может читать

`  `if(semop(semaphore, setReadEna, 1) < 0) {

`    `printf("Can't execute a operation\n");

`    `exit(11);

`  `}

` `}



`  `//отключение от области разделяемой памяти

`  `if(shmdt(p\_msg) < 0) {

`    `printf("Error while detaching shm\n");

`    `exit(1);

`  `}

}

Файл shm.h

#define DEF\_KEY\_FILE "key"

typedef struct {

`	`long type;

`	`char buf[100];

} Message;

static struct sembuf readEna[1] = {0,-1,0};

static struct sembuf writeEna[1] = {1,-1,0};

static struct sembuf setReadEna[1] = {0,1,0};

static struct sembuf setWriteEna[1] = {1,1,0};

**Результат работы**

Поток-писатель 1:

de@de:/lab4/6-shm$ ./client

Using default key file key

Type message to serever. Empty string to finish

hi

Type message to serever. Empty string to finish

first

Type message to serever. Empty string to finish

client

Type message to serever. Empty string to finish

bye-bye

de@de:/media/data/lab4/6-shm$ 

Поток-писатель 2:

de@de:/lab4/6-shm$ ./client

Using default key file key

Type message to serever. Empty string to finish

hey

Type message to serever. Empty string to finish

second 

Type message to serever. Empty string to finish

client

Type message to serever. Empty string to finish

bye-bye

dm@dm-Lenovo-B550:/media/data/lab4/6-shm$ 

Поток-читатель:

de@de:/lab4/6-shm$ ./server

Using default key file key

Client's message: hi

Client's message: first

Client's message: client

Client's message: hey

Client's message: second 

Client's message: client

^Cexecution complete

Все сообщения от клиента сервером прочитаны.

***5.2. Вариант 2.*** К условиям предыдущей задачи добавляется условие корректной работы нескольких читателей и нескольких писателей одновременно. Как и в предыдущем варианте под чтением понимается извлечение данных из памяти, т. е. одну порцию данных может прочитать только один читатель.

`	`Легко понять, что это условие не приводит к необходимости использования дополнительных средств синхронизации. Теперь вместо одного процесса, за каждый семафор будут конкурировать несколько. Но повторная запись или чтение все также невозможно. Так как, чтобы очередной процесс-писатель отработал, нужно освобождение семафора, которое выполняется из процесса-читателя, и наоборот.

***5.3. Вариант 3.*** К условиям предыдущей задачи добавляется наличие не единичного буфера, а буфера некоторого размера. Тип буфера (очередь, стек, кольцевой буфер) не имеет значения.

`	`Двух семафоров по прежнему достаточно, но это приведет к вырождению буфера, так как все процессы будут работать только с одной ячейкой.

`	`Так как размер буфера не равен единице, то больше нет необходимости в чередовании операций чтения и записи, допустима ситуация нескольких записей подряд, и после этого нескольких чтений. Нужно только следить, чтобы не было записи в уже заполненный буфер и не было чтения из пустого буфера. Для этого выберем другие типы семафора и придадим им другую семантику. Возьмем два считающих семафора. Максимальное значение обоих – размер буфера. Первый инициализируется нулем и имеет смысл «количество заполненных ячеек», второй инициализируется N, где N – размер буфера и имеет смысл «количество пустых ячеек». Процессы-читатели перед своей работой захватывают семафор «количество заполненных ячеек», т.е. ждут появления хотя бы одной порции данных, а после чтения освобождают семафор «количество пустых ячеек». Процессы- писатели перед записью захватывают семафор «количество пустых ячеек», т.е. ждут появления хотя бы одной пустой ячейки для записи, а после записи освобождают семафор «количество полных ячеек». Таким образом, решается проблема чтения из пустого буфера и запись в полный.

`	`Так как семафоры не бинарные, захватить их может сразу несколько процессов, т.е. несколько процессов попадут в секцию записи или чтения. В этом случае, если операция записи или чтения не атомарная (а зачастую так оно и есть), может произойти нарушение нормальной работы программы, к примеру, несколько процессов-писателей попытаются произвести запись в одну и ту же ячейку буфера, или несколько читателей выполнят чтение одной и той же ячейки.  Таким образом, операции записи-чтения становятся критическими секциями, доступ к которым также необходимо синхронизировать. Для этого будет достаточно еще одного бинарного семафора, имеющего смысл «доступ к памяти разрешен». Оба типа процессов должны захватывать его при попытке взаимодействия с памятью и освобождать после.

`	`В итоге, **алгоритм** работы процессов-писателей, и процессов-читателей выглядит следующим образом:

![A description...](data:image/jpeg;base64,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)

*Рис.5. Организация доступа к разделяемой памяти многих «читателей» и «писателей»*

`	`Важно отметить, что *порядок операций освобождения семафоров не важен, в то же время изменение порядка захвата семафоров может привести к взаимной блокировке процессов* (dead lock). Взаимная блокировка в частности может произойти в таком случае: процесс-читатель захватил семафор «доступ к памяти разрешен», далее он проверяет, есть ли заполненные ячейки, т.е. пытается захватить семафор «количество заполненных ячеек», предположим, что свободных ячеек не оказалось и процесс переключился в неактивный режим, ожидая пока какой-нибудь процесс-писатель не запишет данные и не освободит семафор «количество заполненных ячеек». Но, этого никогда не произойдет, так как перед записью данных, процесс-писатель должен захватить семафор «доступ к памяти разрешен», который уже захвачен ожидающим процессом-читателем.

`	`**Пример решения последнего варианта** задачи. В качестве разделяемого ресурса используется массив, находящийся в разделяемой памяти. Ячейка памяти, расположенная за последним элементом массива, интерпретируется как индекс последнего записанного элемента.

**Исходный код** программы:

Поток-**читатель**:

#include <stdio.h>

#include <stdlib.h>

#include <signal.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/sem.h>

#include <sys/shm.h>

#include <sys/time.h>

#include "shm.h"

int\* buf;

int shmemory;

int semaphore;

void intHandler(int sig) {

`  `shmdt(buf);

`  `shmctl(shmemory, IPC\_RMID, 0);

`  `semctl(semaphore, 0, IPC\_RMID);

}

int main(int argc, char\*\* argv) {

`	`char keyFile[100];

`	`bzero(keyFile,100);

`	`if(argc < 2) {

`		`printf("Using default key file %s\n",DEF\_KEY\_FILE);

`		`strcpy(keyFile,DEF\_KEY\_FILE);

`	`}

`	`else

`		`strcpy(keyFile,argv[1]);

`  `key\_t key;

`  `//будем использовать 1 и тот же ключ для семафора и для shm

`  `if((key = ftok(keyFile, 'Q')) < 0) {

`    `printf("Can't get key for key file %s and id 'Q'\n",keyFile);

`    `exit(1);

`  `}

`  `//создаем shm

if((shmemory = shmget(key,(BUF\_SIZE+1)\*sizeof(int),IPC\_CREAT|0666))< 0)

` `{

`   	 `printf("Can't create shm\n");

`	 `exit(1);

`  `}

`  `//присоединяем shm в наше адресное пространство

`  `if((buf = (int\*)shmat(shmemory, 0, 0)) < 0) {

`    `printf("Error while attaching shm\n");

`    `exit(1);

`  `}

`	`// устанавливаем обработчик сигнала

`  `signal(SIGINT, intHandler);

`  `//создаем группу из 3 семафоров

`  `//1 - число свободных ячеек

`  `//2 - число занятых ячеек

`  `// 3 работа с памятью

`  `if((semaphore = semget(key, 3, IPC\_CREAT | 0666)) < 0) {

`    `printf("Error while creating semaphore\n");

`    `kill(getpid(),SIGINT);

`  `}

`  `// устанавливаем индекс в -1, 

//первый записывающий клиент установит его в ноль

`  `buf[BUF\_SIZE] = -1;

`  `// инициализируем массив -1

`  `int j = 0;

`  `for(j = 0; j < BUF\_SIZE; ++j) {

`  `buf[j] = -1;

`  `}

//устнавливаем 1 семафор в число свободных ячеек,т.е. можно писать

`    `if(semop(semaphore, setFree, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}

`    `// говорим, что память свободна

`    `if(semop(semaphore, mem\_unlock, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}

`    `printf("Press enter to start working\n");

`    `getchar();

`	`// основной цикл работы

`	`int i = 0;

`  `for(i = 0; i < 15; ++i) {

`	`// ждем, пока будет хоть одна непустая ячейка

`    `if(semop(semaphore, waitNotEmpty, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}

`    `// ждем возможности поработать с памятью

`    `if(semop(semaphore, mem\_lock, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}   



`    `//читаем сообщение от клиента

`    `int res = buf[buf[BUF\_SIZE]];

`    `buf[BUF\_SIZE] = buf[BUF\_SIZE] - 1;

`    `printf("Remove %d from cell %d\n", res,buf[BUF\_SIZE]+1);

`    `// освобождаем память

`     `if(semop(semaphore, mem\_unlock, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}  

`    `// увеличиваем число пустых ячеек

`     `if(semop(semaphore, releaseEmpty, 1) < 0) {

`      `printf("execution complete\n");

`      `kill(getpid(),SIGINT);

`    `}  

`  `}

}

Поток-**писатель**:

#include <stdio.h>

#include <stdlib.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/sem.h>

#include <sys/shm.h>

#include "shm.h"

int\* buf;

int main(int argc, char\*\* argv) {

`  `char keyFile[100];

`	`bzero(keyFile,100);

`	`if(argc < 2) {

`		`printf("Using default key file %s\n",DEF\_KEY\_FILE);

`		`strcpy(keyFile,DEF\_KEY\_FILE);

`	`}

`	`else

`		`strcpy(keyFile,argv[1]);

`  `key\_t key;

int shmemory;

int semaphore;

` `//будем использовать 1 и тот же ключ для семафора и для shm

`  `if((key = ftok(keyFile, 'Q')) < 0) {

`    `printf("Can't get key for key file %s and id 'Q'\n",keyFile);

`    `exit(1);

`  `}

`  `//создаем shm

`  `if((shmemory = shmget(key, (BUF\_SIZE+1)\*sizeof(int), 0666)) < 0) {

`   	 `printf("Can't create shm\n");

`	 `exit(1);

`  `}

`  `//присоединяем shm в наше адресное пространство

`  `if((buf = (int\*)shmat(shmemory, 0, 0)) < 0) {

`    `printf("Error while attaching shm\n");

`    `exit(1);

`  `}

`  `if((semaphore = semget(key, 2, 0666)) < 0) {

`    `printf("Error while creating semaphore\n");

`     `exit(1);

`  `}

`  `printf("Press enter to start working\n");

`  `getchar();

`  `int send = 0;

`  `char tb[10];

`  `int i = 0;

`	`for(i = 0; i < 10;++i) {

`  `//ждем, пока будет хоть одна свободная ячейка

`  `if(semop(semaphore, waitNotFull, 1) < 0) {

`    `printf("Can't execute a operation\n");

`    `exit(1);

`  `}

`  `// ждем доступа к разделяемой памяти

`  `if(semop(semaphore, mem\_lock, 1) < 0) {

`    `printf("Can't execute a operation\n");

`    `exit(1);

`  `}

`  `printf("Add %d to cell %d\n",send,buf[BUF\_SIZE]+1);

`	`++buf[BUF\_SIZE];

`  `buf[buf[BUF\_SIZE]] = send++;

`  `//освобождаем доступ к памяти

`  `if(semop(semaphore, mem\_unlock, 1) < 0) {

`    `printf("Can't execute a operation\n");

`    `exit(11);

`  `}

`   `//увеличиваем число занятых ячеек

`  `if(semop(semaphore, releaseFull, 1) < 0) {

`    `printf("Can't execute a operation\n");

`    `exit(11);

`  `}

` `}

`  `//отключение от области разделяемой памяти

`  `shmdt(buf);

}

**Результаты выполнения** 

Процесс-писатель 1:

de@de:/lab4/9-shmdop$ ./client

Using default key file key

Press enter to start working

Add 0 to cell 0

Add 1 to cell 1

Add 2 to cell 2

Add 3 to cell 3

Add 4 to cell 4

Add 5 to cell 0

Add 6 to cell 1

Add 7 to cell 2

Add 8 to cell 3

Add 9 to cell 4

Процесс-писатель 2:

de@de:/lab4/9-shmdop$ ./client

Using default key file key

Press enter to start working

Add 0 to cell 0

Add 1 to cell 1

Add 2 to cell 2

Add 3 to cell 3

Add 4 to cell 4

Add 5 to cell 0

Add 6 to cell 1

Add 7 to cell 2

Add 8 to cell 3

Add 9 to cell 4

Процесс-читатель:

de@de:/lab4/9-shmdop$ ./server

Using default key file key

Press enter to start working

Remove 4 from cell 4

Remove 3 from cell 3

Remove 2 from cell 2

Remove 1 from cell 1

Remove 0 from cell 0

Remove 9 from cell 4

Remove 8 from cell 3

Remove 7 from cell 2

Remove 6 from cell 1

Remove 5 from cell 0

Remove 4 from cell 4

Remove 3 from cell 3

Remove 2 from cell 2

Remove 1 from cell 1

Remove 0 from cell 0

`	`Процессы-писатели записывают по 10 чисел в массив, процесс-читатель считывает первые 15 из записанных. По результатам проверяем, что синхронизация работает корректно, выхода за пределы массива нет, записанные данные не затираются до их прочтения. К примеру, в нулевую ячейку сначала первый поток-писатель записал 0, потом он же записал 5, а потом второй поток-писатель записал также 0. Проверяем, что все данные были прочитаны.

# <a name="_toc190170013"></a>**6. Сокеты**


`	`**Пример** использования сокетов — простейший эхо-сервер. 

ТЗ: Сервер прослушивает заданный порт, при запросе нового соединения, создается новый поток для его обработки. Работа с клиентом должна быть организована как бесконечный цикл, в котором выполняется прием сообщения от клиента, вывод его на экран и пересылка обратно клиенту.

`	`Клиентская программа после установления соединения с сервером также в бесконечном цикле выполняет чтение ввода пользователя, пересылку серверу, и получение работы. Если была введена пустая строка, клиент завершается.

Алгоритм работы программы:

![A description...](data:image/jpeg;base64,/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAUDBAQEAwUEBAQFBQUGBwwIBwcHBw8LCwkMEQ8SEhEPERETFhwXExQaFRERGCEYGh0dHx8fExciJCIeJBweHx7/2wBDAQUFBQcGBw4ICA4eFBEUHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh7/wAARCAGtAn4DASIAAhEBAxEB/8QAHAABAQACAwEBAAAAAAAAAAAAAAYEBQEDBwII/8QAYxAAAQMDAQIGCwgMCQkHBAMBAQACAwQFEQYSIQcTFBYxlRUXIkFRVVaU0dLTMlRXYXGBkZIjNDU2dHWTsbKz1OEkJTNSU2Vyc6E3QkNigqKjpOImREWFtcHwRmNkhIPC8aX/xAAYAQEBAQEBAAAAAAAAAAAAAAAAAQIDBP/EACMRAQACAgICAwADAQAAAAAAAAABEQIhAxIxMhMiQVJhkVH/2gAMAwEAAhEDEQA/AP2Wh6EXDvclBr79co7ZSMlcySWWSQRwQxjupXkHDR9BPxAErSNsFyvBdPqG7VbWHIbRW+d9PDGD3i5pD3nw5OPABlZFhJueprjc5C58VE7kVK072tI3yPHxknZ+Ro8JVJgeBBH9rnSJ6bbKfj5VL6y57XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvWHda6kt8BmrKmOnj6C97tkIJztc6Q8WS+dS+sna50h4sl86l9ZVkLmvia9rg5rhkEdBC+0Eh2udIeLJfOpfWTtc6Q8WS+dS+sq9EEh2udIeLJfOpfWTtc6Q8WS+dS+sq9EEh2udIeLJfOpfWTtc6Q8WS+dS+sq9EEh2udIeLJfOpfWTtc6Q8WS+dS+sq9EEf2utIjeLbKD4eVS7v95dj7BcrTiew3arkY3e6iuFQ+eOQfFI/aew46MHA/mlVi+ZOgIMCwXOC6Uj5Yo5YZI3mOaCUAPheOlp/9u9jGFsVNXIm16pt9XESIbi40lQ0d+QNLo3H48NcPoVHFnY39PfQfSIiAiIgLT6jurqF0FJSU7quvqSRBA12yBjpe53+awd89PgBW2l6B8qmtIbNykq9RPIfymZ8VMfBBG8taR8TiC/PfDgg6maUmuTTLqS819dK8d1DT1D6enYP5rWMI2h/bLj8a6xwc6RxvtkvnUvrKuZjZX0gkO1zpDxZL51L6ydrnSHiyXzqX1lXogkO1zpDxZL51L6ydrnSHiyXzqX1lWSkNjLiQ0DeSegLX0l3t1VTS1UNdTvgiOHvEgIbjpyUGj7XOkPFkvnUvrJ2udIeLJfOpfWVNbaulrqbj6Sdk8RJAew5Bx8a+queKmjdNNKyKNvunPOAP/m5BL9rnSHiyXzqX1k7XOkPFkvnUvrKmoamCrpmVFNM2aJ4y17TkEfKslBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1kHB3pRhDo6GeNwOQ5lXK1wPxEOyFXogkexV5sH2a03GsudG3fJQ1spmfs+GKVx2g74nEg/F0qltlZDX0MVXTu2o5BkZ3EHoII7xByCFkOwPiUzA3sRrd1Mw4pLvA6ZjM7o6mP3eP7bHA/wD8ZPfQU64d7k9HR30b7kb8rVaouRs2n625Mi46WGMmKLP8rIe5Yz/acWj50GFdq+41Vc61WIxxzMGamrlZtspwRnZDcjaefoHfz0LDGhrZVMJvVbdbvKTtF9RWyBufCI2kMb/stC3WmbaLXbIaV8pmqANuolPTLK7e5/znK2uAgkO1zpDxZL51L6ydrnSHiyXzqX1lXogkO1zpDxZL51L6ydrnSHiyXzqX1lXogkO1zpDxZL51L6ydrnSHiyXzqX1lXogkO1zpDxZL51L6ydrnSHiyXzqX1lWv6FicupHFzW1MZc2biHAO3iTAOz8uCgne1zpDxZL51L6ydrnSHiyXzqX1lWQ5LN/RncvtBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIdrnSHiyXzqX1k7XOkPFkvnUvrKvRBIN4P9NwkvpYK6klxgS09fNG8fO12Vy2e66YcOyVbLdbS4hoqpGgT0xO7u8AB7P9bGR3ye9XLqq4Yp6eSGaNskcjS1zSMgg9IQKc5G1uORuI6CF2qc0PPJGy4WSdxdLaank7XOO+SFzGvid9V2znvljlRoCHoRcP9yUE3wbgc3C/OXvq6h0nxO41ypVNaVLaC7XayuOHNm5ZCMYzFLv3fI4OHzDwqkb4coOUREBERAREQFF61dQ02q7XW3yNjrS2mmZtys24mTktLcjBwS0PAOPi76tF8vYx/umh3yhB5HQ3S/x6ho6W1Qvs9oHFtpKKsEjJDAQ7aIjEbiN4yC97A0BoIGVr26jv9VaLbU0N/qZ6WoET6iqnc2FrZXUsrnx7QidgBwYfcneMZXtE0cb8hzAQRj5vAuugoKWit9PQwQtbBTxMjjbjOGtAA/MrbNPF6jU+uSyapfWCidFTOc+OQPEojFPkycXxBaO77oOLwM4bsguwPStA1k1ZHXu5fUXCiZU7FJUzMDTI3ioy7vDIDy8Zx3iO9v31xoaWuoZ6Opia+KeMxyDoy09IXbCxrAGtGy0DAA6Esp2oiKNCIiAiIgIi4ccIJrhGGLLRSMbmRl2oOLHy1UbT/uucqVvQpvUw7I36z2phBENRy6oGehjAQzPyvLT/ALKo4yXMBKD6REQEREHxOSInFrdogEgeFT3BbHFFwaaYjgftxiz0my4nJcOJbvJ75PSqN/uVMcHzG2+zP097k2eV1LEzoAgBzBj4uLLW/K0oKgAAYCLhnR05XKAiIgn+EWGpqNH1sdLFLMcxuliiOHyRCRpla3wksDhjpOcLznVzKK61cU+nGCCzMbTC6PpqLudjjgd7SzBcwd0dx2QMkEDC9mIyvkRxgFoY0AnJAHSrEpMPHae+XOCWShobrVh1PsG0U8dMwNrmmct7vDNzXDdtAt3DaWBU3K6XuuNPX3CqbZ21VPUVJe1spgZtztcHAxBoIcYct7vHcu8K9mNvpGXI3BsLW1DomxF4/mtJIH+J+lZTY49nGw349yWlPIrdddQMbHDPWV9HL9gZaYI6VrY6uIvcHPd3G52MZA2dkbJxvWyqpNX27T1bc7fU1lyc11xhMc0jGui2KhzYpGjY7rDGdHf2l6bsMyDsjI6N3Qmw3GNkJZTxs6g1tJOHQuquKjudVSsZLEGCciu7kl2CQwRFrAQD/nHvLeXm66hj0NYai2VFwmrJ3lle7k448fYpONAZjG21zTst6C5rRkg5Po4YwdDR4ehc7Lf5oSynj14vWoKSnqLtbbndKqFxqDE0Rtc10bH07WuA2c57qT5d+7cVj1F617HXvpZ65lOTLM+LjXOYDJxgAibsQvMuwC0BowXbROT3vaNhmANluB0bkMbDjLGnByNyWU8mlumtJ6IirmfSkW+4SsfASXvkjqIAzaDowBhrngYJyCSsp94vUGuKO3RVla+CGopoJmTdL2vhaS4NbGctyTmQvb3WWgbt/p+wzGNkY+ROLjznYbnw4SZWIfSIiiiIiApzVpc286bdH7vsiR8xhfn/AAVC44PSpurkdcdcUdPEXGK1QPqJ927jZBsRtz4dnjCR3u5PfCCmHQpvXZdyK1sDe4feaIP+QTNcP94NVI3oC0etKOrr9NVUVuwa+LZqaPPQZoniSMH4i5gB+IlBvMDwIsGzV0Nzt1LX0r3OgqIw9m0O6wR0HwEdBHhWcgIiICIiAiIg+ZOgfKvGNqutmp9Q3iraex7LjWtpHmM4p6p1PBsSnpDshpY07gCSDna3e0kZXyWMIILGnPxKxKTDyitrtW2+80tsoay41JqoISHyxNeInvim23HAG4FgOPCAO+tHQ6i4QTa6qsqK9rKltsfK2J226RwFK1zJGxtg2Rl28kvIyS3GRhe6bLSQS0ZHRuXHFx5zsNz0dCtpTzx0upKu800dZW1NETejTOjpHh0QiFI6Qd0+PJ7trd+O/heihfIaM5wF9KTLQiIoCIiAiIgLh3QuV8ye4KCdtzY28Jd5cw5e+z0G3g9GJqvBx4Tv3/EqRTGjGurbretRPOW10zIKQ43mmgBa35jI6dw+J4+NU6AhAIwURBp9RWuSp4m4W8tiudJniHnoe0+6jd4Wn/AgHvLEtWqrdPUigrpOx1yaO7pKk7Dt3SWk+6b8Y3KjIBGCsO7Wm13ejNHdrbR3CmccmGqhbKwn+y4EIO1tXTEZFTCf9sLnlVN74h+uFpuZWkO9pq1NHgbSsAHzAJzK0j5N2vzdvoQbnlVN74h+uE5VTe+IfrhabmVpHybtfm7fQnMrSPk3a/N2+hBueVU3viH64TlVN74h+uFpuZWkfJu1+bt9CcytI+Tdr83b6EG55VTe+IfrhOVU3viH64Wm5laR8m7X5u30JzK0j5N2vzdvoQbnlNNn7Yh+uF88rptr7Zi+uFqOZWkfJu1+bt9C10ej9KnUUsJ09beLFG12zyduMlzwT0eBVFVyqm98Q/XCcpps55RD9cLTcytI+Tdr83b6E5laR8m7X5u30KK3PKqb3xD9cJyqm98Q/XC03MrSPk3a/N2+hOZWkfJu1+bt9CDc8qpvfEP1wnKqb3xD9cLTcytI+Tdr83b6E5laR8m7X5u30INzyqm98Q/XCcqpvfEP1wtNzK0j5N2vzdvoTmVpHybtfm7fQg3D6umDSTUwjG/+UC0V21Vb4KkW+hebjcnb2UtMdp2P5ziNzW7/AHR3LsdojR7mlr9M2l7SMFrqVhBHgII3raWq12y00baO1W+loKZpy2GmhbEwfI1oAQY1htrqfjq2tMclwqsGd7PctA3CNv8Aqt/xOSenC2o3LhrQ0YAAXKAiIgIiIBAPStHfaGrZXw3W17LqhjSyanJAFSzp2cnocMdyTu8O5bxcFoJzjeg0Vm1RaLi50AqhS1sf8tS1P2OWI/G0/wCB7627aqn2Rmphz/bCxb1YbHe42R3mzW65MYcsbV0zJQ0+EbQOCsHmVpHybtfm7fQg3PKqb3xD9cJyqm98Q/XC03MrSPk3a/N2+hOZWkfJu1+bt9CDc8qpvfEP1wnKqb3xD9cLTcytI+Tdr83b6E5laR8m7X5u30INzymm98Q/XCCppgN1RD9cLTcytI+Tdr83b6E5laR8m7X5u30INzyqm98Q/XCGqpvfEP1wtNzK0j5N2vzdvoXB0VpHH3t2vzdvoQbgVVPn7Yi+uF9cqp/fEP1wpem0bpQ3esYdPW0t2I3YMDSATtZx4OgLO5laRz97dr83b6EmEibbrlVN74h+uE5VTe+IfrhabmVpHybtfm7fQnMrSPk3a/N2+hFbnlVN74h+uE5VTe+IfrhabmVpHybtfm7fQnMrSPk3a/N2+hBueVU3viH64TlVN74h+uFpuZWkfJu1+bt9CcytI+Tdr83b6EG55VTe+IfrhfMtXTNbk1MIHhMgWo5laR8m7X5u30L5fofRz2lkml7PKwjBbJRsc0/MRhB0VuqYKqsfbNPBtzr29y8xnMVPu6ZHdA727pPeW105a22y38W+Tj6mR5kqZj0yyHpP5gPiAWZQ0VHQUsdJQ0sFLTxDZjhhjDGNHgDRuC7wABgIA3LgtGDgb1yh3jBQSk7a7TVZNV0lLNXWqocZJoYW5kp3k909relzSTkgbx09GVtbRqCz3WAVFBdKWoj6MtlHT3wR3itrstIwRkLT3LSmmLlWmuuGnrVVVZbsmolpGOlLfBtEZx86DZNqqf3zD9cLnlVN74h+uFpeZOkfJu1+bN9C55laR8m7X5u30INzyqm98Q/XCcqpvfEP1wtNzK0j5N2vzdvoTmVpHybtfm7fQg3PKqb3xD9cJyqm98Q/XC03MrSPk3a/N2+hOZWkfJu1+bt9CDc8qpvfEP1wnKqb3xD9cLTcytI+Tdr83b6E5laR8m7X5u30INwaqm98w/XC5FVTEfbEP1wtI/RWkfJy2b//AMdvoWFYNG6Uks9I9+nra974g5xNO0kk7ylfqTNTSo5VTe+IfrhOVU3viH64Wl5laR8m7X5u30LnmVpHybtfm7fQitzyqm98Q/XCcqpvfEP1wtNzK0j5N2vzdvoTmVpHybtfm7fQg3PKqb3xD9cJyqm98Q/XC03MrSPk3a/N2+hOZWkfJu1+bt9CDc8qpvfEP1wnKqf3xD9cLTcytI+Tdr83b6E5laR8nLX5u30INlUXOhp4XzT1tPGxgy5zpAAAp513l1VE+ksL5Bb3dzPcgO4c3O9kR/zj3tobh4c7lnN0Ro4TRzHS1mfJE7aje+ijcWHwtJG4/GFvmsY1oaGgAbgEHVRwxwQshhaGRxtDGtHQAOgLvXAAHQuUBEXDs7O7pQcooCjvXCRc4DW2u06U5E97xDyqvqGS7LXlvdBsJAPcnoJXDbvwmObARRaFPKCRCeytTiQgEkD7Bv3AlB6AihjVcK4x/FeicfHcqn2CCr4VjjFr0Tv3jNyqej8glC5RQ3K+FfxVoo+HFyqfYLk1fCuAP4q0VvOPulVewShcIoflXCuBk2rRQwN+blU+wQVfCsR9ydFA/jKp9glC4RQ3K+Fjdm06K3/1lVewQ1fCtn7l6J8P3TqfYILglQkOrQ7hHfZ226fjnNFOTtDADXOdtdHRsnK+nVfCsMjsTorI/rKq9gtZHbOEZupJL6LRow1T4eJ+6NTjGckj7B0nd9C6cfWLnJjOJ1T01FDCr4V/FWiiPxlU+wTlfCuei1aJP/mdV7Bc21yihzVcK4/8K0Tn8ZVPsFwKvhWxnsVoo+DFyqd//AQXKKCqLlwpQU8lTPbtDxQxNL3yPudSGtaBkkniOjHfXYKrhYIGLVoreNxNyqRn/gILlFDcs4VvFWih8tyqfYLHZeeEp8AnZSaFdEZBEHC61ONsnZ2c8T053YQegooblfCtv/irRXWVT7BfENw4UpoWTQ23REkbwC17bnUkEHv/AMggvEXntw1Dwh2VtNWXq0aWNA+upaWU0dwnfK3j544QWh0QBwZAd5HQvQWHIzuQcoiICIiAixrnUmjoJ6oN2uJjdIW+HAzj/BRFru3CncbZS19PaNGNiqYWTMD7lUhwa5ocM4g6cFB6Ai8/lu3CbG+dklHoVjoGcZKDdKnuG4JyfsG4bl2wV3CrNC2WO26HkY9oexwudTgtPQf5DvoLtFD8q4V8fcrRXy9kqn2CCr4ViMi1aK6yqd4/IILhFDiq4V8Em1aK6yqfYIarhY2ci06KP/mVT7BBcIoY1fCvjPYnRfydkqnp/IIavhX6RatE4/GdT7BBcrh3uThQ/K+Ffo7FaKJ+K5VXsFh1N/4RYLhDbZ4NBx1k4zHC661Ae/p6BxOe9+dWiWZpTV7Lxqyst7LfPDI1gbIXn3GwXA5+cq4HSvLaal1/Zq6434W3RMJqnAzukudS1rMbunie+d5+NbnlXCuP/CtFZ/GNV7Ba5ZxnKOrGEVC6RQxrOFYe6tOigPxnU+wXIq+FbxVovA/rKpznwfyCw2uEUPyrhY3/AMU6K3f1lVewXHK+FgdNq0T1lVewQXKKH5Vwr5+5Wiusqr2C45XwrkgditFAkdBuVV7BKFyihzVcLG7+KtE9ZVXsE5Vwr4+5Wi/k7JVPsEFwihIa/hUmDjFbdEPDXFpxc6k4I6R/IdIX3T3rXlFebbBqC2abZR1s/EbdDWzySNdslw3PiaMbvCguEQdCnta3i52m2QOtNNR1FfU1cdNCyqkdHEC89LnNa4gAA95BQovP57vwmQR1D5qTQsbaZm3O510qRxTcElzvsO4YGV3RV3CrLG2SO2aIcxw2muFzqSCPyCULpFD8r4Vz0WvRPWdT7BBV8K3irRXWVT7BKFwihuV8K+PuTorOM/dKq9gueVcK+7Fq0Vn8ZVXsEFwih+V8K27+KtF7+j+Mqn2C45Vwr4P8VaKG7xlU+wShcooflXCxtfcnRePxlU+wXHK+FfP3K0SP/M6rf/wEoVGorj2KtVRcDC6ZsDNtzWnBx31pODe/NvlkDoqaSBlMGxZeQdpwG/6N30rXVEvClUQvims+inse0te03KpwQdxB+wLB03buEmw2qO20Vq0c6KMk7b7jUhziSSSfsK6ROPxzE+XOYvKJelN6AuVDGr4VhuFq0V1lVewXPKuFjd/FOit58ZVPsFzp0XCKHFVwseKtFHw/xlU+wXArOFYgfxXonrKp9gguUUHNcOFKGLjJbfodjN2Xm51ON5wP9B4dy++V8K53i1aJx4eydT7BBcooY1fCuD9ydFdHjKp9gukXfhMNOJhRaF4tz+La/spU4Ly7ZA/kOku3Y8KC/RQzqvhWa0ufa9EtaBknsnU7v+AviC48KVRA2ent2h5Y3AFr2XSpII+XiEF4imNJXXUk95rrTqWitNPUQU8NTE631EkrHMe6RuHbbGkEGPvZG9U6AhGQiFBM6CGNF0+MnfP+tevNI6e+VFms+ooqathqaUT0dBAGODoWCCfakLe86R+xvxua1nhOfTuD1odpClaejbm/XPW/4to6NyQPHrzcteSS3SmiiraaplknZEYHGVlO0UNK4Oa5zBtHbMhAx7okb8L6uGp7xBpp1Oai59lWVVeNgU79t7RKTGBgYOWdGO8vX9gZzkr4kpYZHMc9jXOY7aaSN4OCM/4rVpt4vqO76zpaW8ukN4ZcduoqqJkZPFwHio3RxNDIjxxae5w4hpO1k+DZakr9RQOrbrb5LxIHy1AbG1shGwx8GwGsIwP9JvA37160YmkAeBcbAa07vCnaiLeRvr9T072S1lVe5Lgy6bYpmANi5O6UFzRiMh7mty0bTgMEuzuTTOobzFfGzVsl0jtnHUz3tmbJMWl8dXxmTxTe+2HLW5a04wd69Jqr1aKetlo6ivgjqIYjK9jzgtYBkn6N6x5NUadho46ue5UzYZXOY0nPumkA97djI+lOxbzamvOqrjIx1DW3CChjNJLLVGn2hgxlr2Brhk4cC52MEbIHfW70pVapisd+lqKetjrRZ4p6OOoqXT7c5bPk90BsklrMsG4ZCr6e92CJ8tJBX0zHRuL3sBxvc/B+U7TvpK75b5Z2cft3CBvEFwl7r3Gy4A5+QuH0qTIj6aBtTcLVyC7akqqVtZs1L6h0kbCziXuOctaSC5rc5GAXYHgWlluOrnUcPJ6ivldSVb7dUvYw7T2QwTcZMAOmQ9y5oJwX7IXp090t0VLyqSrhZBtvj4zO7aZtbY+UbD/oKwtNyWClpaW3WeohcySHlUTGv2jJG4/yh8IyelLEhWVFystFdb3DU3R8FHc43MinkfJxtO5mxgbR6A6Xb+VgC081y1DTx1cdLdrvV3amnlgmgLCY2RCDbJBDcEh5bg9PQ3oyDf3O4aVukDuWVsEsVumZO8bRw1wcWtJHfG1kfKluvuk4ahwpblSh9wk487/dlx2M/Fksxv74VGr0jfrpe79StqaOamp4KOpjlILzFPIDSlrwXNaQQHvHR/OxlTVvut245lDc6u98lfJtVlfAyQb8SYAZsbURyG7TWFzdw3jv+it1JYOKmkZcafZgIbJg+5Lujd3847yS3+xRuMbrjStPEcfja3cXs7W14Ojf8ingeW3Or1BcdHXOO7vvBuVTbXMpaSKmPFTxupydqTDcBznZz0OBIAGF6boyWvktD33MTNrxPI2oa/OwHB2Pse4ZZjGz38dJzlZ9FcrdXTsjpqqGaR0DahoY7JMT/cu+QrruNhpa+pNTNUVrHFuzsxVDmN+gd9O1jzPsxqGa53FjZLvTUkmwZnOL3yRs5YxkjmYiAY4ROduZndvyTvWFb6ave2GhlrL2yGnrqee3sEDsVDeUvdI6XucHcB0kYABAzvPtjYmtaG7zuxk7yU4tu7p3dCtptPaCirWaYpZbhVVdRUzN46V1U7umud0tAwMAeD41J6dZf5aq2S1FRcYY6WnoWcnDdiN21xok2hgZI2Wd/duV1XWGmrKw1T6quY/dujqHNaMfEFs2RBjQMk4753lS1hJ8K4B0rTk9PZu0b/8AzGmVeBhSPCuMaUp/x3aP/UadVykqIiICIiDX6kGdPXH8Fl/QKxdDt/7F2M/1dT/q2rL1H971y/BJf0CsbQ33lWP8XU/6tqCI1pbbjdtVXu0spZ22ypoIpKyZowJWMa/ELT4XOLc46Ghw3ZBWlkuWtKPStM2OjrGQQPnDZI3O45wbSvLWcWWAAbQGDnpGMb17QWgnK+eLarEpLyqnvV/t1fXS3qqq2NlZXyRBsL3RxuLKJ0bAdnobtSgHw7XSsN171dKHSOFzZQwVdZTnYYQ6fac5zXh2w7DGsLACAel2M7IXr8lPFKxzJGh7XDBBHSPAuWwxtjEYaNlowBjoHgVuE28XhrtRV1vvEQqbxE6hpq6emMbptxEbHRDbcxhl7svxkZ+JZ77pqWSvqjVzXaktkclfTgwREPeeUN2H7Ww4hoY7AcASG7RG8BetiNoBAHSuOKapcLDxjs1qjjLTl90M7Kinie523svifKWO7jigHDZIy97g4biAFsH6lvs7LK21Pramq2aOOoifC5uHvgqWue/I9zxgjJPR3IXq5iae90fEuttJC2czhgEpaGOcBgkAkgf4n6VbJt5PY59XPudsNTLcapkU9G2efjXxNAcCJGmIMw/JG05xdgBzR3jnvZQXWPV975DVVkV1rrlI37XxG2mMDeLeZNnfggY7rdjZx0hei3O5223SwxVtXHTuqHFsQcfdEYG76R9K19XqLTs0NXBNc4AGjipmhxyA7LcDHTk53hSx5bq69XS+aUqbxWV1zt1vrKd76WGCIv4t8b44y1+GHPdCZ28bwW+BZV6uWq4qp0Fnra19HE93Y6Wp40Pkd9jOHARkyby9uHFu7JzuyvQY6rSdXYIaBlXSdjnCKnhY12G73FkbR4DtMIHxgrPpb/YpgWwXKldxLXOOHDuQ0d0fkA76tjzl94ubIKuWkrb9WiSWeOqG9jYXctbHGGP4txa3YMgcWAkNGdx3jb0t5qo+D6n7K1NbTVVTcm0sbvshl2ONydk7IccRteckAkNKubJT0TLZEbe5ppZszsLd4dxhLyfn2ifnWTPSU80kT5o2vdC4vjJG9rtktyPAcOcPnKljyGgr7vcKaeSmuF4eygonlhDpW5k5U4N2sjuzsDHfxjet7ofnFUXyXsk+sjp6ihnDag1DnCSQTACQMLAItxJa3J3dPQvQoaWCFjmQxtYHOLiGjGXEkk/OSuzi27vAO8rZt5hWs1XOZKW4urIX08dvYH0VTIRKTK9kz8hrcZG8jfgYK096uOuoLreoqV0+Y5JmxRtklJMQewQlo4stBLel204nLs9GB7Pxbf8AHK4ZCxuNndgAfME7EPIb1ddcvqL/AEtPFVUkhgqJWyRAzRwiNocGxuLRtOedlg3btpxG8LeaPnvztazy3N1W63upqwiR8zjHI5k0YaRGWgR4aXAYJ2sEr0MRAZ3neuOJaOjcPApatXpCFzLJHNIHCSpe+oftDG97i7/3WDrRo7Kab/Gbf1b1SsaGN2W7gpvWn3U03+NG/q3qCkbuaApjXjG7Ni3DAvNNux8ZVQOhTWvN4sf46pvzlBHa7tlwvd81LY20dQbbUW9s9VI0bpwyN2zA3w7T8bQ/mgjvrAZc9Y0VjkPJKxsFPbqzieK23SOc0U+w3i9jDSBthu853r2LixvJO899NgfKr2HkMN91LbrjWOvdRXRMkgrXxgQvcxjtqmLGghv+aDIB8jvDu6JL7quWYVkjLt2NbNWU7WRNcx0p43LZS7YJDNktDSAdwcQDkY9ilp4pInRvYHMcCCCMggrlkDGRtY1oa1ow0DvBXszUvIYam/3GxTRGovMNRR0lxkYI3TAuIn+wAuLW8Z3G4bskAEgHK+n1+pJ7iX3CovFNbYq24U+ImOBlLahzoi5wa5zYyzYYC0dGV69xYzkbinFtxgbhnOEs28btd61I+pse229OmZG2OrfIJA15MEm8RiMNI2tg7bnB2QO5G8Dsn1TfpmWWmtctbPWSugina+B7Qxz6GbL35bkNEjck7sloHfXp1xu1rt1VDT1tZFBNOQI2vOM5OB8mTu3rAk1BpiOsqpXXGkjqKdjWznoIaH7Lc7t4234+UpaojSE+r33u0ur+yU0DalkM88sj2BzTSyktMWzg4cGkyE9JAA6VsbnJWyzXmOpr73HcmuquIgpw5sIibHtQuBG7vNOQclzi05CsG6hsZ5M0XKnPKf5Huvd78D5N+7f311SaisLWTudc6YOgcGyZd7kkkD5d7T0eAqTIibjJXx6gitdLdbw2oZTUQhhYXPbsyvl4573kHB2Ruyd2yMLDF71XWupZnzVcUEUxt9wMLDuMOy2omYMYPdF4GO80kb8BX/ZLTgra+rFdSGUMp4qt3GDuQ/JhB/tcYcfKu2nlsViZS2SOWCl2gRDE5292TknPhLiTk9JJTsI27TXjTrbhdqGStqmw3F1GynqJnyNe2WFghxtHJHHuY3PThx37lpq24agpGXJtuvF3r62lZVQ1IdGeLaxkbNh7SBgPL8dGSdp27cr65XbSVfHTV1XX00rKSoD4t5xxmySN3f3AkfJnvLIt1103BVy0tJXUzZKguqXt2tzst2i7PR0DPyZVstp9M3y7Xi7XOWekqaOCOhY2LDHOjdIJJQXs2g0ncG7iAfm3qSobtfRt0VfUXuCiBMk1fC173SS8W7Ya1row9mXNJc3umghgDsOIXo/OrTscMUputM2OUubHk7yW42hj4tofTlZcN4tM9zNsjrIX1QG1xQdknGCfoyD86ljzCprtR11uq2XN1x7NvmgbDRcQRSNjHEua52G/z9okhxdnI3NAaPRdFS1UmmKV9a6q5adttSagd2Jg9wkxuHchwds4GNnZxuwt3sbW/PzrW19gpa2tNXJVV7JN2BFUua0YHeAS4JeWWi73yrq421c14prZUOhkrD3ckkQdxhLdrimlh2hG1zWZAGd+/K6oKasFup6WWtvhNNcqWShidTv2Z4xV7Tny9xja37ZJ2SNkdHQfaeIj7w8K+jG0nPfVuE2ndLQ1kGhqXlNVWVVXJRiWSSoOZNtzASMYHQSd2FK6Ti1A+aCuqKy5x8RW00HJi3ERidTRF/c4H+e5xz3iN2FdVthpaut5W+esY/d3Mc7mt3fEFsuKaN4UtYaChGOEW5fiml3Z/wDvVColO0f+Ua5fiik/XVKolFFw44GVyuHDLSAgneDpwOkqTH8+b9c9URUgzQkcJe2k1LqKlhc9z2wxVpDGbRyQBjcMkr65kyeV+qPPv3IKzeucqS5kyeV+qPPv3JzJk8r9UeffuQVuVw7JacdONyk+ZMnlfqjz79yHRMnlhqjz/wDcgwNWaYvt2vfLqapt8DIYpRA92dsl8JZh7dgjOSDt5OA3GzvU+7SWrIeylPHDb5WPp55BI+pkcXPnLMtB4ve5ogHgzxgVedEP8rdT7uj+MD6FwdEPJ++3U/n59CtiYqdB36uqWz1NbSsYTJskSuDmtfI14AbsANIDd+85O8ld9Vwc3KUV4ZcxiZ1Q6LMhBdxksTwJO537mHo+JUA0PIDnndqc56c153/4f/MpzHkxg6u1Qf8AzA+hLkTTtBX+QVEJuVHHSOqamcRiZ7xIZXTEOILe4IEoBDSQd62Og9D12nby2snrKaZkdCaONsbXZYNoOA3joB2v8FtTomQuLjq3U5Oc/b5H5gsJmkZjeZKPnZqbYbTtlB5e7OS5w+foSymkh0pq2jr21BNurX1UkEcrnTyBrGwyvnDsCPDQcBmAOkg5KxrVpbUFVWXi01NJR08dVRwxz1ZL3bLeVVbjxPc90cFp34xlqsDoiTf/ANrtT/Ly85/MuDoeQ9OrtT58PLzn8ytlJiz8HV4tlVT3GGqo5aqi4ttOyWokcyUNLido7HcdIwAHY3796ya/RN9q7bX26ee1Mjqy+cSx7QcyR1KIeLDQ3AjBzvz0YGFvzod+N2rdT+fn/wBgh0RIenV2p8d8cvO/4uhZspj6I0pWWK+1FfU1UU8fJ+R0wZnMdOx+YmHd0tGQrYdCkOY7z06u1P0Yzy85/MvrmTJn779UD/8AfPoQVuUypLmTJ5X6o8+/cnMmTyv1R59+5BW5TKkuZMnlfqjz79ycyZPK/VHn37kDhZdjSlPno7NWk/8A/RplWg5UfLoGCofT8u1FqCthhqYakQz1pcxz4pGyMyMbwHMafmVg0bIxk/OcoOUREBChXzteEYQYGpHY09cfwWX9ArG0Of8AsXY8eLqf9W1bSqijqaeSCQZjkaWPGekEYIUjSaDbS0sVNBqzU0cUTAxjG12A1oGABu6NyWLPJTJUjzKf5Yao8/8A3JzKf5Yao8//AHJa6V2SmSpHmU/yw1R5/wDuTmU/yw1R5/8AuSzSuyUyVI8yn+WGqPP/ANycyn+WGqPP/wByWaV2SmSpHmU/yw1R5/8AuTmU/wAsNUef/uQ07td2W5XoUkNDyQRtdmZ8ri2RuHscNkhpyDsnLTjO7fuUzbdDX9t/orjcK2heKdrWuLHuJdiVr9zdhoaMA7t+PCVvzop56dXanPx8vI/MF8nRbxj/ALW6nAz7/wD3fOnb8Z/toYuDuvfcZJ6mvhdC2dk0cbdrc8VktRtdG8hkuyPjyuyi0Nd5RYqS4T2ptuttM2kljpw4yVUIjDcFxAwHFrct37i7ecrZwaRmkr6iA6s1NsRsYW4r3dJzn8yyuZD9rPO3VHn59C1MymM23ek6CS1aYtdrlex8lHRQwOczOy4sYGkjPe3LZlSQ0RJ5XanHyV59CcyZPK/VHn37llpWb1zlSXMmTyv1R59+5OZMnlfqjz79yCtymVJcyZPK/VHn37k5kyeV+qPPv3IK3KZUlzJk8r9UeffuTmTJ5X6o8+/cgrcqa1mf4003nxoP1b1j8yZPK/VHn37l20Oi4ae5U1dU3y93B1M/bijq6svYHYI2seHBKCob0BTOu3Z7A4HTeqb87lTgYGFq9R2SnvlA2knlng2JmzRywP2ZI3tO5zT3ig2eUypLmTJ5X6o8+/cnMmTyv1R59+5ShWplSXMmTyv1R59+5OZMnlfqjz79yorcplSXMmTyv1R59+5OZMnlfqjz79yDD4RdM3q/SjsfU0zIOJaCyWQsy9sm33mnaG7vkYO/epyuseqsUVXU2uie+RsGI2yyODXm400xbJ3AwAC7eM7mFV/MmTpGrtTg/FXn0LjmRJ5X6n8B/hx3jwdCtiYrODi41dy5bNUU2avZNUyOaRjIiJny9w0M7sd3uyW7xnvrawaZ1HA+3uY2xPZayGwNeX7U42ZGkvfsdzgPBDQHb87wtlzIkznnbqbP4efQnMiTd/2v1R5+R/7JciQbwXXIybBr6R0L3UzpWd0NswRwBne7zo5MfE4Kt1Fp651l2qZqGWiENdTR09Q6dri+EMe47cYxgnDj0kYIaVydEyDp1fqjwfb59CxrVpCWrtsFS/VmpmPljDyGV7gASO8qmkza9F6g0/WWaopoqSsqGTMhDXzyPiYxlPK3ac7YywZIAAB6cZXM3Bld58UzrlSsgLH7TmyO7l76aSJwEezj/SHB2twxu7yr+ZDz06u1P5+fQh0RJjHO/VHn59CdlSlJpG/Wu80sVPSUE7qqnrRM+WaSSOIObTRj7IWA7R2TgbI3AjK3Fk0jfKTU1tuNVVUktLQyTFga9wcWvjLRhgYAD0ZJJzjvLZ8x5PK7U/n59C55kPx99upvmryP/b4lLFY3cucqSGiJB/8AV+p/Pv3JzJk8r9UeffuUFblMqS5kyeV+qPPv3JzJk8r9UeffuQVuUypLmTJ5X6o8+/ch0TIenV+qD/8AvfuQZlFntjXLd/4RSfrqlUS0Wm9NQ2Wtqq3sjcbhUVLGRukrJzIWsaXENHgGXlb1AREJwMoCLgOyucoCJlMoCJlMoCJlMoCJlMoOMrWRnGqJvwJg/wB962ZXncd5v54VJLRyemEJjGZC05EAJcD09JLtn5VvDCc7Yyy609FRMplYbETKZQETKZQETKZQETKZQEXy54aMncFy0gj848CDlERB0XGZ1PQVE7QC6ONzwD38DK1cJ1K+Nr/4oG0A7/SeBbC9fcis/uH/AKJXdSfasP8AYH5k/E/WtxqX+qP+ImNS/wBUf8RbZxxhMq2U1ONS/wBUf8RMal/qj/iLb5TKWU1GNS/1R/xExqX+qP8AiLb5TKWU1GNS/wBUf8RMal/qj/iLb5TKWU1GNS/1R/xExqX+qP8AiLb5TKWU045y/wBU/wDEUrr1us3stwt3J+P5T3JpdrcNk+6zuwvQF8yNBBzjC1hn1yukzxuKaTTHZITzC6ugNXxMO3xWdnPdZ+fK3wXnmib7qG4a3uNvuFLTxMphszOa13+aSG4z3jnK9DBCvNjOOVSxxTeLlEymVzdREymUBEymUBEymUBEyuC7CDlEXy2Rp6EH0iZTKAiZTKAiZTKAiZTKAiZTKD4kIBWBp0kWOiJH+gZu+YL41RU1tJZauqt8bJKmKIvYx/QcbyPoytDwVXS5XfT5qK6CKKKNwipyxpG01o3nf/8ANy648czxzk5TP3pZjoRcA7lzlcnURMplARMplARMplARMrgnAyg5RcA5XKAuHjLSPCuUKDzCGm1JU2GbUlbwmXu2UxdK4wQUFC9kbRI5oa3bgc49GN5JJK+LRBergyUu4VtUUMkT2tfFWW22xPG0MtP2vgg78YPePgW2oLfUXLgyggpI2Szx1PHsje7ZEhjqjJsk97Ozha/Wdv1LqSmgljtDaVjKjAiLmOlc3iJmFzsnGNp7cDvYJViBmO0/qBsxgdwv3wShpcWGitu0G4zkjk+cYXbFpfU8rQ6Lha1A8FocNmgtpyD0EfwfoKnqHR+oqeY1VRRQ1EXGPPJXPDnuBga1zjITvG30NPgX3p+wa0tUsLaeNwbKzYkD5GFvFmqnecjO4iORuNnozjvK0ltwyw355eGcMN8dsN23Yo7acNxnJ/g+4YX0zTuonx8YzhevzmbJdtChtuMDpOeT9CjqHResKOAAUEM0kdLHE8SyN2XfwaBj9+cvw5jsA7t3xrPbo/VzqO6mOVtKJ6OrjZF3B4wybOy3AOGZwd46MqUWpHaa1K0ZdwuX8DZLt9DbegdJ+1+gINNal5Pyjtt6g4nZ2uM5DbdnHhzyfGFM3DReq53y1BeRIY6iJjGSggRcsppWtwTgl7WS/SAVSUNiusegILdLA+oqGVwqJadz2sL4uO2yzccDLd2PmSi3I05qIlgHC9fSXt22DkVty5vhH8H3hcO09qFpAdwv3wEkNA5HbMkkZA+1+kg5WnqNM36c1kcFlZSSVFQySkmZVA8kiEeyYjv752nHG7Mh8C1lbofVhtdLSHu5I4JozLG9hMj5AxzHOye5LACzaHeYCOlKLU7bHfHMke3hjvRbH7s8ktmG78b/AOD7lw7R19jlN1dwqXtjzGGcodQ20ZZnIGeT4xn86+rtpq5xyXOagoKZ8lTdI5muLWOPFCADIaSBnbzuPhJXyyw3iDS2m6eqoOXOt5kFXRccMSZY9rTknBxn/H4lbpPLtqtO6ipWtNTwvX2AO9zxlFbW5+mnTm7qIuewcL1+LmN23N5Fbchvh+1+jo3rJven6y6G1mWhg2YLXVQvi4zLY5XiLYAz042XDPpUbddPXqh0/GyqtYdJSulfVVQnGamN7Q1sQwc4yWk94bAUpbUzrHfGmNp4Y71mUZjHJLZlwzjd/B9+9fbNO6ie4tbwvX5xDzGQKG2nDgMlv2v0/Euq3aYryIJTRw0Z5fUzxRkhxpY3QOawZ7/2Tu8Do2viWhrtJ6qdLQVtstraOS2xROfCakfwueMOJdnOO69zk/z895KLUw0zqYxl44WtQbIdsk8htu52cY+1+nO7HhXD9N6jZE6V/C9fWxsdsucaK2gNOcYJ5PuOVi0GnNRWzQNXb4mR1N1ddmVzC6XuZXcpjlc4nvAkPOO8siHSdxp9NvtJc6rc6+wVcj5Hj7LEJopJHH4zsvOPjwlFjdO6hcYg3hfvpMoJjxRW07YHSR/B96O09qFrHvPC/fdlhw93Irbhp8BPJ93SPpUzU6K1NDX8a2V7IDIx8fFFp4hjZXOO8nI7k9A6c4XRRaavt2sVJX26kfb4GspnSQ7bXurHNdLtPwTjcHt3Hp+YK0Wrm6c1E6cQDhev3Glu0Gcitu0R4ccnXybBfxTuqDww3viWu2S/kds2QfBnk/SpLmPrFsjWMdJtGCANlc9mWlsbmuaXA5GMgbunpW2dbbq27tpubDo6GeGOR1K2ZpG3GxzHSO34yS9gx3wzKUWyr/Q6q0/RUN6ZwjXe6RdkqCB1PUUVCI5WT1UULsujga73MhIIcN4C9LjBa3BOd683u8csPA1pmGeN8c0VVYI5Gv6Q5tbSgg/OCvSlmdKIiIMS9fcis/uH/old1J9qw/2B+ZdN6+5FZ/cP/RK7qT7Vh/sD8yfiR5dN6mkprTV1MWNuGF8jc+ENJChdO6c1hcrDb7jJwpagjdVU0cxYy327ZaXNDsDNPnG9W2o/veuX4JL+gVjaG+8qx/i6n/VtRWh5n6t+FXUfV9u/Zk5n6t+FXUfV9u/ZlbIgh+aOrPhV1H1fbv2Zc80NW5x21dR9X279mXxf9W3qkvd6oLZZ4KmG0U0VRUSyT7JIkDiAB8jCt1Q6ssNQABcqcSgYezb3tdtbJb8oduVoanmfq34VdR9X279mTmfq34VdR9X279mW8k1XpyN5Y68Um02Z0Dvsg7l7SA4HwYJA+cLtg1FZZ54qeG4075ZamWlYwO3uliBMjPlaGnKgnuZ+rfhV1H1fbv2ZOZ+rfhV1H1fbv2ZUNTqOx01TPTT3KnjlgaXStc7GyAMnPzLpOrNOiGGU3WnDZnFrO635BAOfBgkZQaLmlqzOO2tqPq+3fsy55n6tx/lV1H1fbv2Zdlo1tR3O81EJfR09FGXMZJJUDjJHB4b7jvAk9/4lvp9RWSnjD5blTtb3eO66dl+w76HbvlRErBoPUkNXNVRcJl+jnqNnj5G2+3Zk2Rhuf4P3gsjmfq34VdRdX279mVLBfrPPEJYrjTvYdneH/wA55Y36XAj5QsR2sNNCOJ/ZilPHRRyxgPyXMkGWOA8BG8fErNykREeGm5n6t+FXUfV9u/Zk5n6t+FXUfV9u/ZlQzais0NsguUlfEKWo3RPz7s4JwPDuB+grih1JZa64cgpLhFNUcWJAxhzlpzg/Pg/QVFtP8z9W/CrqPq+3fsycz9W/CrqPq+3fsy30+qLBDG6R90pyGua07LskFzi0D53NI+ZfDNWacfFDI28UmzM+OOPuxvdINpg+cb0LaTmfq34VdR9X279mTmfq34VdR9X279mW/oNS2Oul4qluUEj+80O3ncTkfFgE/MuuPVunpHlrLpA77AyoBB3GN+NhwPfB2hj5ULaTmfq34VdR9X279mTmfq34VdR9X279mVXarlQ3Sm5RQVDJ4tosLmnoI6QstFRPM/Vvwq6j6vt37MsV9u1PYL9Zn1OvLteKerq+Ilpqqjo2MILHHOYoWuB7nvFegKa1p91NN/jRv6t6ClHQpjhAluUdtoae13OW11FZcIaY1UMUb3xtcd5DZGubndjeCqcdCmtedFi/HVN+coNadIatAz21dR9X279mU/Ui8QXjsc/hS1admVsMtQ21W4wxSO3NY53J9xJIHQRvGSF6s8ZaQop1PfbfU3K2UVqZUi4VbqiKrfIOLjDtnJcOnLe94cBBjv05qJkckjuF6+hsTtmQmitoDD4D/B9xXy7T+oG1Ap3cMF8EpGQw0Vt2iPk5PlahumL6aeVr7SCG0nEzAztcamo2nFs+CcZbv3Hp4z4lrpbfqB1NC6KwZqo65tM2SSQFpayAQkE9OOMDjnvgrVJapj07qGSPjI+F++vZv7ptFbSN287+T95fY0zqYv2O21qDbzshvIbbknGcD+D+Ag/Op6DR2r5qavaxzKMVBnezdHh23SMi2dkHDcuZ82crp1BoXWNyqrnNFMIAZZOSgVA7prqWNjid+4lzS36SlQWp4tM6llg4+Phbv74cZ4xtDbS3HhzyfC+W6c1G7i9nhdvx40ExYoradvHTs/wfesa2aYvo0XdaGSWTjamoZLFTvLWZY0sLo+5OGh4aW/OsWq09fKqpqnwWRlKKl0fInCpH8CLZNpzsZ6T7rA6ehSoLbI6e1A0Zdwv3xvQN9Fbd2ej/ALuvk2G+h8kZ4Yr1txZMjeR2zLMdOf4PuUzXaG1Y23wNJ4x0U05fsOY8yiVgDCQTgbGyR/t5HQVuq3St4p+Wz0tLBUyzUlvi23hrnOfG+TjnYJwTgs6enHxJRbOk0xqU0vKDwuX7iC0HjDQ23Ywe/nk+MdCw6LS9zt1FBS0fC3dqamIIp42Ult2Tv3huaffvX3TUN4tvB3b7HUWySqq5a2QPiDgcRCofKN43Y2A0AfHhauj07d7la7hTi18mlgNWynbLIBsOlrXTMDfABG4DPzK3qkrdt/FpvUcsr4ouF2/SSM921tFbSW/KOT7l1Nsd8ex8jeGS8uZGAXuFJbCG5OBn+D7l9aA05drRc56qsDBBNSuZC3dtMBmcWte4e6dskEuWrp9G3OiNol7FRVMNPBS8tpmytHHPZFUNd07jh8kbvj2VKW21fp7UDJGRv4YL418jdpjXUVtBcPCP4PvXE1gv8LA6bhgvkQJwC+jto34z738G9S5td9koDDUWWQSQ10EEM220iKNkm8Dfn/SEZ6MALZUmj79yKG3V1M2p4inrHGpfMDx0ksTGs3d4tOW58DQrRbcc3NR5jHbdv2ZXFsf8CtvdEdIH8H3lcO07qJkZkfwvX5jA0PLnUVtADT0H7X6PjWlptI6khw2qgbVtlmeaY8YGmizVcYXdPdbTMfGMYXXe9D6ohrKIUtxfW2+ip4IGtLGBzmRhwAc0nDulu89KUWoZNMamjJD+FrUDSOkGgtu7dn3t4AfoXxLp7UEMZkl4YL5GwEAudR20AE7xv5OtEzQ+r222KI1zptmGJpZI9oc4inmZvdnvF7Rjv9K+KPQmpxPDLM+FtVM6J9RK9wkjjApyxzQwnechrQflPfSoLUTtNakaATwt6gw5u0P4Bbd7fD9r7xvH0hdnNHVjh/lV1H09+3W79mWLddNX2pg0qYACbbQGOshdIAJjmnPFk+A8W7f8Q8K9AAI6QpKpPR0N7tup7lZ7tqWtv0baKnqYpKqngifGXvma5o4mNgI7gHeCVXqeo/8AKNcvxRSfrqlUKgIUQ9CCd4PBnSNJ/bm/WvVA1pH/APqluD+5W6LS1NHLX0rHtfMHNdM0EfZX97K3/ZW1+MqP8u30oMrB8H+K5AKxOytr8ZUf5dvpTsra/GVH+Xb6VKKdGortS2O0T3StLxDABkMblziXBrQPlcQPnU/Sa+tdSySSGjrHx08D5qx7TE5tM1pIO0Q/uugn7HtLZamjsd/tE9sq7nSiGUtORKw7LmuDmnByDhzQcEEHvgrSUGmdO0tJW07r1DK2tpH0s2JYo8tcXZIDAAD3R6AtQkt1Fqem7CXG5T0VbTOtwJqKaZrRK3DQ4Ywdk5BB6e/vwuKfVVvksM92fT1MQglMElO4MdIJAdnYGy4tdvPSHEfHuK17rTapbNeKCs1GyrlurNiepfJCH42NgYAAbuAHeXXSWHT8Gm6myi9Q4qJ+UOlEkTS2TIILWgBoxsjdjB356VUZFXrSGkqY6eey3WN5jEk202JvEAy8UNrL+67roLNoEbxuXbBrO1T1VupoW1Ej6+ATAsaCIsgkNcc7nZY8Y8LSDha6KwWl76qa4apkr56lkbOMmniBY1svG4bsgAAuxu6ABgYWutem6N8dXV1N9gt1dPcpqyPk9THKIWP2gGZeCDve9/RuLyO8mhu5NdW4WwXCnoa+rbyWKq2Y2NBZHJGXguLnBrTjwnJJwMlfI19ZpYTUU8FdPTMaJqiWOIAQQlrXca/JB2cOzgAuw127ctGzQ1h5HTU8mrJJDTwsgje51PlsbY9hrcbOAQP87GQe+u+i0dpujo5KSn1HxLaiEU9YWzRZqogxrNh27cMNO8YPdFBQt1fanM4wtnDRxW8sGPslQ6AfQ5p+Zamv1/aZzJQUe1yowxSB1TCeJbtv2Q1+D8jjjOARuPQtfcNJUjmSyUms3B5DeKie6n2AWzmZoJ2drG049/JC+26F0iGQRdl28XHsl4NSzMpbNxuXHp3nd8m5NDe27VtPPpKgvbKeqrHVrxDFHDEGOkk2i3cHOw0ZBOS7o6VinhFsLp4oI46l8sjWgx9wx8chLg2Itc4HaLmkbgQCRkjK6q+100Vitlps+oKemFLXCoM/HRbbG7bnEAEFp910ELBZonTEc0c1PfxDK1oL5BLEXvlbtES7RG012XZwCBuG5BuqPXdjragRUb5pwJoYnSMYAA6UxhmQTnplG7GRsuBG5YsHCVpypjrHxuqQylpJ6wkx44yOJ2y7Y37yTjZHf2gsA6E0axtRFT3SOmgqZY3zRQ1LG8Y2PitlpPTj7F05z3bjlfc+itJy1Qm7MRxgTiUMZPGGgBznhmP5u0WnH+qFJVeW6pjr7bTVsTSI6mFsrQ4b8OAIz9K7o4xG0MjY1jANwHeWutdVaKC2UtBFdKV0dNCyFpdO3JDWgDO/4lk9lbX4yo/y7fSotMogjoC4DT0kb+hY3ZW1+MqP8u30p2VtfjKj/Lt9KgneFYY0nTDGP47tG7wfxjTKuURwp3G3zaZpYoa6mkkde7TstZK0k/xjT94FW4OVQREQYl6+5FZ/cP8A0Su6k+1Yf7A/Mum9fcis/uH/AKJXdSfasP8AYH5k/EjyxdR/e9cvwSX9ArG0N95Vj/F1P+rasnUf3v3Ef/iy/oFarRF0tjdG2RrrjRhzbfACDM3IPFt+NFUiLD7K2vxlR/l2+lOytr8ZUf5dvpQaGq0xUTXXU9XymMMvNFBTRtx/JmNsoJPy8YPoUrQ6O1BdLGaOeqpaAUtfWPpJBGeOBdVPIc4HdjGXDw5bnG9ekG62vxlR/l2+lfAulsDvulR/l27/APFW0mLedXHgnZLWTOgmhfTS5i4uWadgbGWxAuIje3bdmMnecHI39OdjaeD+qodVQ3g1kL4Y7hUVohAd3LpjPtEE9JLZYwRj/M6fDb9lbX4yo/y7fSnZW1+MqP8ALt9KWRCRvekb1WSVkFLXUMVFLXtuUYkiL5DO1zHNY7fgR7TMkjeRu+XGfoi9OqLlVCuoRLeYjDXAscWwjbJaYvDgOeCDjJ2TuxhW/ZW1+MqP8u30p2VtfjKj/Lt9KWUgqng9uRhpKanrKRtNa5Hvt7H7W1JtyMeeNcBuwGkdyDknO7GDh1mg9R1k9M2r7HvjidPI10VTIzupKgz4JADwP83LST3/AAhek9lbX4yo/wAu30p2VtfjKj/Lt9KWlIWg4N204imMrGzsdBI0RSyhgc2pfO/aDnnazt4yc5OTgZwsWz8F01JR2mGW6F0tPFAypqY8slPFAtAYR0dxhm/vbRxkr0MXa1n/AMSo/wAu30oLra8/dKj/AC7fSlkQmYtJXCistjioKqndXWnjAx0zTxcjZGlrs98HeN+/oI7+7O0tpyayyh7p2TNFBBTYDdnu2Ple5wHQAeN3D4lueytr8ZUf5dvpTsra/GVH+Xb6UtaRtn0NXW6qcG10LqWW4R3CRpBLmyMc44b8ThxYO/dsnGcrVDgrmk4wVVza5pjgbGGAjiy1pa93xkggD5/CvR+ytr8ZUf5dvpTsra/GVH+Xb6Usp5/ZeDBkM9PLXvjBhYIjxFXUkvaGOaDtOf3O92dgbhvGTlZw0Rc47BHRU9fDHUMtdFRbWXDJgc0u7reQHAEZG8ZyN4Csuytr8ZUf5dvpTsra/GVH+Xb6Uspq9D2KewW+op6iWOV81U+cbDnkNDgO5y8knBB3k/R0KgWH2VtfjKj/AC7fSnZW1+MqP8u30qEQzFNa0+6mm/xo39W9bnsra/GVH+Xb6VParrqKpvGnI6esp5ni5glscocQOLfvwCiq0dCmtedFi/HVN+cqlHQpfhAkjjisckj2sYLzTEuccAbz0lBUFfOyf/hWL2VtfjKj/Lt9KdlbX4yo/wAu30oMpwJA3L5LTjoWP2VtfjKj/Lt9KdlbX4yo/wAu30olMtoIHQuVh9lbX4yo/wAu30p2VtfjKj/Lt9KKy8FMb1idlbX4yo/y7fSnZW1+MqP8u30olO6tnhpaSWpneI4oml73eAAb1H1/CDaLfTQvuFHX0z5i58ccrGNLoWta4ytJdsuaA9u4EuycbOcqiuNZaKyjlppLjRlkrCx32dvQQQe/8ajq7Sdkr2QGs1S6eWnDoonyvgeI4XNYDG1hGy33AO0BtfGrA2FRr+2xRzzOt1yMEUksMU3FN2KiSN+y5rMuHR05dgbiM5BC+XcJOn2FrZhPE9ryypY90each4Zk4fh4yR/J7e75CtTHpShraKqp7tqdvFOrKmamhZNE5sIfK5zSCRk7jvacjJO5I9C6UbU8cb1A9z5NucfwdrZBlp2QA3DB3OCW4Jyc9KqN9Fri2urY4H0VwhjlqjTQ1D4hxch43iS4YdkNEmG7wDvBxjeqCz18V0oW1lO0iJ73BpJHdBri3O49BwcLzip084abuUTr1S11wqGVMNIx9VGxlIJZnPbI0gZJaeLdvz7nAV/aqm0W+30tBDcqUxU8TYmF07c4aMDv9O5RabPY3g7K+gCFi9lbX4yo/wAu30p2VtfjKj/Lt9KyrLwVwAcrF7K2vxlR/l2+lOytr8ZUf5dvpVSmXvTBWJ2VtfjKj/Lt9KdlbX4yo/y7fSi0ywCuT0LD7K2vxlR/l2+lDdbXj7pUf5dvpQaqj/yjXL8UUn66pVCpi1VNNVcIlzfTVEUzRaaQExvDgDx1T4FToONoZxlcOd3O7J+RaK31V8rKKCqjZQsZPG2RrXF5IDhlZP8AHuOi3/Q9dfi/uGI5IliSaQ0zNK+SWy0he9xc47OMk9JXHMvSviSk+g+lZmb54Lf9D0/jzwW/6HqfHH8o/wBO/wDTD5l6V8SUn0H0pzL0r4kpPoPpWZ/Hngt/0PT+PPBb/oenxx/KP9O/9MPmXpXxJSfQfSueZelfElJ9BWX/AB54Lf8AQ9P488Fv+h6fHH8o/wBTvH/GJzL0r4kpPoKcy9K+JKT6Csv+PPBb/oen8eeC3/Q9X44/lH+neGJzL0r4kpPoKcy9K+JKT6CsvN88Fv8Aoen8eeC3/Q9Pjj+Uf6d4YZ0ZpQdNkpPoPpWvZpLTR1DJALNSmIUjHbOzuyXuGen4ljcIjdUOsA5CG8fx7Nnkm3t/m6Pl3LK0W2/tuDucLoDVmij/AJMb8bb/AHXez8m5ani64du0JHJF02HMrS3iOk+gpzK0t4jpPoKoUXB1T3MrS3iOk+gpzK0t4jpPoKoURKT3MrS3iOk+gpzK0t4jpPoKoURU9zK0t4jpPoKcytLeI6T6CqFEE9zK0t4jpPoKcytLeI6T6CqFEGgi0dpmCeKeKy0jZYntkjOznZc0gh3yggH5lvmDDcEYXKICIiDEvX3IrP7h/wCiV3Un2rD/AGB+ZdN6+5FZ/cP/AESu6k+1Yf7A/Mn4keX1M0PYWObtNIwQR0jwLQN0TpUDAsdJgbvclUSIqe5laW8R0n0FOZWlvEdJ9BVCiCe5laW8R0n0FOZWlvEdJ9BVCiCe5laW8R0n0FOZWlvEdJ9BVCiCe5laW8R0n0FOZWlvEdJ9BVCiCe5laW8R0n0FDorS2PuHSfQVQoehBJU+j9MOutVG6zUpa2OMgbJ3Z2s9/wCJZg0XpXxJSfQfSthTHN5qyO/FF/8A3WwHSrlNTTGG4T/MrS3iOk+gpzK0t4jpPoKoUUbT3MrS3iOk+gpzK0t4jpPoKoUQT3MrS3iOk+gpzK0t4jpPoKoUQT3MrS3iOk+gpzK0t4jpPoKoUQT3MrS3iOk+gruoNL2C31jKujtNNDPH7h7W7wt2iAOhYdzoKW5U7qSvpo6incQSx4yCQsxEE9zK0t4jpPoKcytLeI6T6CqFEE9zK0t4jpPoKcytLeI6T6CqFEE9zK0t4jpPoKcytLeI6T6CqFEE9zK0t4jpPoKcytLeI6T6CqFEE9zK0t4jpPoKcytLeI6T6CqFESk6dF6WH/glIPmPpWHZNH6YmtFJM+y0pc+FriS098fKquTwLB06R2Dot4/kG/mVifrLMz9ohrxorS3iOk+gpzK0t4jpPoKoUUbT3MrS3iOk+gpzK0t4jpPoKoUQT3MrS3iOk+gpzK0t4jpPoKoUQT3MrS3iOk+gpzK0t4jpPoKoUQT3MrS3iOk+godFaW8R0n0FUKINbZbHarO+V1soIaUygCQxtxtYzj85+lbJEQazTP3u2z8Ei/QC2J8JK12mfvdtv4JF+gFsXDIwmXtKROj50+deb6V0Hpe9WaO6XOhnqKyolmdLJy6cbR41w6A8DveBbXtX6J8UTef1HtEVZ/OnzqM7V+iPFE3n9R7RO1fojxRN5/Ue0QWfzp86jO1fojxRN5/Ue0TtX6I8UTef1HtEFn86fOoztX6I8UTef1HtE7V+iPFE3n9R7RBZ/OnzqM7V+iPFE3n9R7RO1fojxRN5/Ue0QWZyvOmUl/HCu9pr6nscI+POCcFmd0fx91n5srZdq/RHiibz+o9ovk8F+iC44s8oOME8vqO90DO2unHydL/tjKJmqW6KK7V+iPFE3n9R7RO1fojxRN5/Ue0XPTa1RRXav0R4om8/qPaJ2r9EeKJvP6j2iaFqiiu1fojxRN5/Ue0TtX6I8UTdYVHtE0LVFFdq/RHiibz+o9onav0R4om8/qPaJoWqKK7V+iPFE3n9R7RO1fonxRN5/Ue0QWqLy3W2itO6dtlFdbNRVFLWRXi2MZKK2Z2A+ugjcMOeQctc4bx316iz3KDlERBiXr7kVn9w/wDRK7qT7Vh/sD8y6b19yKz+4f8Aold1J9qw/wBgfmT8SPLtO7pTIWBqJzo7HXSMcWvZTyOaR0ghp3qG0pwc6PrdMWqsqbZPJNPRQySP5fUd05zASf5Tw5RXpCKK7V+iPFE3n9R7RO1fojxRN5/Ue0TQtUUV2r9EeKJvP6j2idq/RHiibz+o9omhaoortX6I8UTef1HtE7V+iPFE3n9R7RNC1RRXav0R4om8/qPaJ2r9EeKJvP6j2iaFquHe5O/vKL7V+iPFE3n9R7RO1fojH3Im8/qPaJoa3QtPqAa7u0dxuFRNTUh2e7O6TOSz/dOV6K3cVF9rDQ/iiXPh5dUZPy92ue1fojxRN5/Ue0XTl5Pkm2MMesUtUUV2r9EeKJvP6j2idq/RHiibz+o9ouem1qiiu1fojxRN5/Ue0TtX6I8UTef1HtE0LVFFdq/RHiibz+o9onav0R4om8/qPaJoWqKK7V+iPFE3n9R7RO1fojxRN5/Ue0TQtUyPCortX6I8UTef1HtFrq/R9g05f9P1lmpJqWaSvET3crmeHNMb9xDnkd4d5B6MmR4Ub0BR/CTSxV1utduqQ801VdqeKZjZHMLmEkkZaQcbh0FBYIortX6J8UTef1HtE7V+ifFE3n9R7RBaoortX6J8UTef1HtE7V+ifFE3n9R7RBaoortX6I8UTef1HtE7V+iPFE3n9R7RBaoortX6I8UTef1HtE7V+iPFE3n9R7RNC1RRXav0R4om8/qPaJ2r9EeKJusKj2iaFBqmCsnsdWy3zPiq+KJhc047obwPn6FO8ELbq7TfKrpUTSmV+zC2Q+5Y3ufmyc/4L67V+iM/cibz+o9oh4L9EZ+48o+IV1QB+sXSOSuOcHOcft2WjdzQPiXKiu1fojxRN5/Ue0TtX6I8UTef1HtFzdFqiiu1fojxRN5/Ue0TtX6I8UTef1HtE0LVFFdq/RHiibz+o9onav0R4om8/qPaILVFFdq/RHiibz+o9onav0T4om8/qPaILVFFdq/RPiibz+o9ouDwX6Jx9yZs/jCo9ogtkUXo2yW7T2tbtQWmKWGmkttJM6N08kg2+MqAXd24kZAA+ZWiDWaZ+922/gkX6AWyWt0z97tt/BIv0Atkrl5lI8J/g6+9Kk/tzfrnqhU9wdfelSf25v1z1QqKmtUVN2nvdFZLPWRUEs0ElTLUyRcaWsY5jdlrSQMkyDfncB0bwtJS8INFS0MDL9FI2vdUT0pFM0Fk80M0kR4sF21vMYIb3tsDJ3kU2o7I26T01VFVz0dZTBzYp4SMhrsbTSDuIOB9AWom0BYJaaKnljmkbGCGue8l20TI57to79pzpC4nwgFXX6OmfhDs8FXNTS0dc2Wngqpqhmy0mMU4BeD3XSc4Hhwe9vS4a+ordQzVNfb6ynNPLJFNG+SEOaWNa44y/ujhwwASTvXVcOD2impqhsFXOyqqpnvqZ3b3SNkLeMb8ha0jH+sV8T8H0NVVV7628Vchrg5ryAGnZe1oePAM7I3jemhmycIOntt0TJpHuayZ0jWFpdGI5YoiXDayMulBB8APfwFr5OFPTUbmteZo3ukkBEj44zsNl4vbG0/ugSCQG5OAcgLNn4P7LPSMp5HzFgimic7a7pwllildk9/fC36Sux2hbU2u5bTyT07ztCRsRA22F+2G/Fg5xjwnwp9R233WdqtOoKazVEUj5Z44pGva5gGzI9zW4BdtO3tOcA43FddNregrWbFtt1xqphKYHwsjaxzJWgukY4ucAHM7+/GXDBPeyL1pWlut5bc5qmeNwiiidG0DZIjkc9hz0je8rFg0PQ0pY+3VlXRTNw50sZG0+QtIfI7wvcHd0e/gJoOe9AWUdQyjrpKOrpmVDJ2sb3IdDJKGlu1tZ2YzuA3HA3rqtmv7VWyzt5NUsMNPJU7nRvBjawP3FjyM4cN3/wDq6ItBy0sccdFep4GwzNMDA0ERxsiMLBk78iM9P87JXdPwe2lrgLfPUW9vJ3U+IiDtMcwNcDnpJDd5TSO2o1/ZqeOVz4awiKnkmIbGNp2zxPcgZ3uJnYB8eVsLBqe33q4T0FJDVNngiD5xIzAia7HFknOO7GXDHeac43A6a3cHtDFcRVV1XNWGISCDawNnjGMY7OOn+Tbhbaw6VobRXNraaWU1DonRTyE75m9zsh3h2QMN8AJ8KaVjjWUToYJY7Rc3CreGURDWAVG89B2t27fvA3fOBi0HCRpusvFNa4ZHtnme2JzZHsY6KV2fsZaXbRIxgloLR4TvxkDRdI1sYZcq9vJj/Afsm6l357nw53Df3hhY0OgaOgjkNprqilnfEcuyDtTYc0SuPTnf/gE0S+6ThAtErY5aulrKKGWOOWOSYNwYpI3yNedlxI7lh3dOTgBdVJwl6ampYZZJJ6fjBtFkwa2RreKklLi0OJwGxuG7O8gb1xT8H1I26UM9XcKmphouTuhiOGDah2gzONxaA47vkWVBwf2JlKKWRskkW0XODzvcDFJERnwYlP0BPqkQw7fwnaYqS1jqlsDnl2C6oiLN0e3gua4gEjoHhBCV3CTZYGUwZFWmSpc9myImu4vZh4zacNr3O9rQOkn4gSs+l0VQMfTuqJ56mSDbDXy4wQ+Pi8Y6Oj/HesWbg4sclvqKQumxNI17357rDYHQhoPeGHOPykp9VbnRl6F6grA54kloKt1LLKyExxynYY8OYC9x2dmRu/O/6FQLVaes9HZ21LaEFsdRKyQtJ3NLYY4gB/sxt/xW1UElws/epT/ju0f+o0yrVJcLP3qU/wCO7R/6jTKtSfAIiJAxL19yKz+4f+iV3Un2rD/YH5l03r7kVn9w/wDRK7qT7Vh/sD8yfiR5YmpCBp+4596y/oFeXQ6mu9qoaEUtaxsFLbLa1tJJQvfHJxrWhxdK3AjwO+44HSQQvUdSfe/cPDyWXH1Couz6Op7vp621MtyrooK21UsVVSxSYjmY2IDB7+CDg/Eitw/W1ibbJa1lRxxiqRTy07HsMrXGXigS3a3DaOcnvLAtvCJbLnRtkoqCtqKh20eTROjc7Yaxj3PyH7OBxkYO/OXAYXY/g/tb5JDPVVUrTIZI25AEZdM2Z3R05cxo394L5puD63U4ZJFcaxlU1hi5QwhpMRY1hjwN2CGNPytBV+qS2FLq6kqrpTUlPR1T4amVsLKnudjjDDx2yRnPuCN+FiV+u6KkrZ4n2q5PihqJKYSRtYeMlYAS1rdrPf3Zwvig0ZLSVNLPDd5dmmuHKmwlo2cbHF7Oen+Tw1feotGxVlunZQzSxVUla6rY8PxsPfshxB+IDI+NNEPi4cIFrttLcJa+iuMNRQxPkmpyxjpO5ax+Bh2Cdl4O7vAnvLKq9b2ilZLJLFWFsZmDsMGPsc3Eu7/87o+JY1Vwf2qrfUzV9RU1VXUwVEUk73bzxzBGXY6NzGtaPkXXXcH9JVVEr33Ot4qWWR7oRs4Akl41zQcfz0+qlu4R7FXuq46LjaiaHZMcbJI3PmBlETcAP7kl7m+6xuI3LNtutrfVRyg0lZE+EwNkaQ0kOlnfC0bj/OYSfiWJUaGa0wR0F0qKSCOqZNxTWjZa1ri8Nb//ACbBGe83C6qXg+DBNtXuqDpZGPc5jGjaEczpmZ3dIc8pocVXCdpuKujpi+VryW8YC+Nrml2dkbJdtO6N+AdxHx4zIdc08jIpHWi5RxuihmkfiMtjjme5sbz3XQS13Rv3fJn4ptA22mlDo6upw9jBOMj7M5ocA4+Dp3gbjgeBZTtH0JkpQyrq444qeGCWNr8NmZE4ujDvkcXHd4Smh33bUlLa6uenmpqySKlgbUVVTE1hZTxu2sOd3Wcdw47gejwLDZrJjpxQizXEXORokjo3tYHvjLc7fuiGgdBB6CQsq+6VpLxWSTTVFQyCeFsNVBG/DKhjS4ta74gXOHyFYUei6dkzKtt3uRuEbRGysL/sjYwzZ2PB8efCMppNtC3X7qu69kWS1lNp+GQREso2OMshi4wNc9z8g97AZuIOXeCirNdWeihM1RHUtjBILsDA/g5qO+7o2AR8vxb11P0BZeKbSsdUMoms7mma7ueMEZjEp8LsEnPh395YM/B5FLxYkvFVKwSbTmPa3DmmLiDjA6eLJA8BOU+qsmm4S9Myxl8tS2ENbM4kzxObmNsTiwOa4hxIlbgA+Ed5c13CHZaVsLjDWPMz5m7LWtBYYiW91l24OLXbJJAIDvAVkw6MoSM1dRUVT+JqIduTHRKIgSMDdgRDHzrqbwfWMQTRfZxx1RJNK8u3ybYc0tPxYc7d8Z8KaGVR60tk1gmvMsckFPBVill2nMIa4ua3O0DjZy4ZOVjTa/s7H0ZY2WWGqq+SxysfEAXcfxOQC7Lxtb+5B3b1sYdKWuG0z22FhbTS1orNgdAcHNdj5MtC1dToG2TzmRtVVwAzmd4jdgF3HmcY8GHnveBPqNjf9WUNluNNSV1NVMjqZWQxTgt2HSPIDQAXBx3kA4BAyM7s410GvqCqo4auktVzqoJIH1DXRNjIETAzbdnb7xeBgd8HwZWyvOmKa6XGOqqKmoEbXxyupw77G6SM5Yfiw4NO7vgLRt0BHBW0cNFcKqC1soZaeVjX73bXEjZ+QiNxJ7xPxpod83CLYqevkpKjjGsYwyCXbjLS0PYzaIDstGZGkZAyA7wLsul0pLw7TdbRPEkHZqSFrw4OD+L41hIIJyCWkg+Bdfa6tDquOV1RVCKJzzFACA1gc9ry34xtMb094YXferfBbKjTtNTZ4t17knx4DJxrz/i4oLIdCmtedFi/HVN+cqlHQprXnRYvx1TfnKgpUREBERBNa8q7nBDbqa1VjKKesrmQGcwiTYaWknuTu7y0VHqursvZuivZq7xLa5nySVFPTsZsU7YIZC4guAyC87hnOFZXa2wXCajfMXZpKhtRHsn/ADgCN/0laK7aKobhXV1TyyrhdcCRVtjfgSxuZGxzPiBEY39O8q6HXddc0FrrauKro6tsFOHuNSNksdsmLOBnO7jW9Pxr5qNc0cL43MtdwfAWQSOlwwNayaXio3YLsnJ3/Jjv7lhy6CfVtiFwvEs7XumNSzYAa4yOD3bPfHdMZjPeGFs4dGW5lPNTmWcxyGDYbte4ZFNxrGD4g7I+RPqNeeEiy8jFxgpK6WgLxDykbBYJSwv2D3WQcbvADuX1euEvTFqqHU89QHFskrdps0QaRG2Fzy0ueNrAnYMDJJ2sDcsq3aItVHNTmN876aBwk5MT9jfKGFnGOHfOyf8ADK+Z9BWflLqqjdLRPfK97uIAA2Xsia5gz0A8Sw56Qc+FPqPntg6fD44eOc6Z9RLCY43NLo9ifisvBOQCe6HxH6cJ3CtpYStBklYwyujc+VzGYaHhgeAXAuByCNkE4347yzbtwfWO5UDaWpM4YJZ5XOY/ZceNm45wJHezu+RfY0NbI699bTTTwSyTOll2CBtguDtj4hkbviJCfVNtlWaipKfUlLYDBUy1tQ3jQGNGGs3gvOT0AgZ+ULBu+s6C1Mr56ujrBRUTzDJVbLOL43dhmS4EZLmgE4GTvIWdcNPUNbqGlvbpJWVlOGhjmnHcgnI+QgkH5lr7zo2hudRVcdWVjYKiTj+IY/uGz9z9kx4RsggeHemlYdNwjWqrhc+ioKyrbHG+SpfA+N7YGNdsklwfh3h3Z3LqZwnafxU5EoEWXAmWLZd9kbHvO33HdOGNvZyM46CB9yaJklvkZNfUigNC6KoLXgOnc6UOLSB0NwO8u6Lg/trRI3lVR3IPJ8ADiCZGy5A6D3TG9Pgx4U+o5o+EC11jWyUdFXVUBawyzRcW5kRfNJCGkh+/u4nDIyMbJzvW609qCgvRa6hMrmmliqXF7cbIkyWtP+sNk5HeWipdGlt+dUuuFS6gFLTR7G2Ps8jJp5SXjwAyNxjvbltNB2OSz0NdJURtjqq+4T1krA7aEYfIS1oPxNx85KaFI33IyuUHQigIehEPQgnqP/KNcvxRSfrqlUKnqP8AyjXL8UUn66pVCg1mmfvdtv4JF+gFsXe5OFrtM/e7bfwSL9ALZDpVy8ykeEDpLU9FZ7HFbq+ivEU8Mkwc0Wudw/lXEEENIIwQtvz6s3va9dU1HqKoRRUvz5svva9dU1HqJz5s3va89U1HqKoRBLc+bL72vXVNR6ic+bL71vPVNR6iqUTQlufNl963nqmo9ROfNl97Xrqmo9RVKJoS3Piy96lvPVNR6ic+bN72vXVNR6iqUTQlue9lO/k156pqPUXWdc2PjOL4i8beM7PYqozg7s+4VYtXH99M34Ez9N6QjV8+bL71vPVNR6ic+bL71vPVNR6iqUTSpbnzZT/3a89U1HqJz5sw/wC7Xnqmo9RVKJoS3Pmy+9bz1TUeonPmze9r11TUeoqlE0JbnzZfet56pqPUTnzZfet56pqPUVSiaEsNcWUdFLeR/wCU1HqLnn1Zve166pqPUVQiDzjXeoaW+2ijttsobxLUuvFskDXW2djQ1ldA97iS3AAa1xJPeC9Fj9wOn5+lfRAPSMoAB0DCAiIgxL19yKz+4f8Aold1J9qw/wBgfmXTevuRWf3D/wBErupPtWH+wPzJ+JHljagY+SyV0cTHPkdTyBjWjJcdk4AUjpLV9so9LWmkqKO9MmgooYpG9ipzhzWAHoZ4VeEA9IQADoCKlufNl97Xnqmo9ROfNl97Xrqmo9RVKJoS3Piye9bz1TUeonPmzD/u156pqPUVSiaEtz4svvW89U1HqJz5s3va89U1HqKpRNCX582bvU166pqPUXHPize9rz1TUeoqlE0JbnxZve156pqPUQ65spG+mvOPxTUeoqlD0JoSbNd2PjXMFPedpvSOxNRuzv8A5i++fFlxjkt56pqPUW5pgOzVZj+ji/8A7LPHThWYpItL8+bL72vPVNR6ic+bL72vPVNR6iqUU0qW582X3teeqaj1E582X3reeqaj1FUomhLc+bL72vPVNR6ic+bN72vXVNR6iqUTQlufFm6eTXnqmo9ROfNl97Xnqmo9RVKJoS3Pmy+9r11TUeotZcr7TXy+2Gnt1HdCYa8SyOloJYmNaGOGSXNA6SFeJgeBBw33IUrwhSPhorXVinqZ46a6080wghdK5rATk7LQTgfIqtMDOcBBL8+rN72vXVNR6ic+rN72vXVNR6iqEQS/Pqze9r11TUeonPqze9r11TUeoqhEEtz5svva9dU1HqJz5svva9dU1HqKpRBLc+bL71vPVNR6ic+bL72vPVNR6iqUTQlufNl97Xrqmo9Rc8+bN72vXVNR6iqETQlXa4s2ftW9dU1GP0F8w67sUsTZI6e8OY4ZBFpqN4+oqmTccqZptQWq12y10lVVNZUTtijiiacuy7AzjwZPfWscO3iGMs4iamTnxZvet56pqPUTnzZfet53/wBU1HqKmByAQuVltMc+bL71vPVNR6ic+LL71vPVNR6ip96JQmOfFlH/AHW89U1HqLnnzZfe166pqPUVMiCZ59Wb3teuqaj1E59Wb3teuqaj1FTJvQTPPqze9r11TUeouHa5s2Pta9dU1HqKnRBJ6XuLLvra519PTV0VOLbTQ7VTSyQ5eJZyQA8Anc4KuXymUGu0z97tt/BIv0AtkOla3TP3u238Ei/QC2LshpIVy8ykeH0ijq7hR4NqCtmoq/hA0vS1UDiyWGa6wsexw6QWl2QV09t3gr+EjSPXFP6yirdFEdt3gr+EjSPXFP6ydt3gr+EjSPXFP6yC3RRHbd4K/hI0j1xT+snbd4K/hI0j1xT+sgt0UR23eCv4SNI9cU/rJ23eCv4SNI9cU/rILdFEdt3gr+EjSPXFP6ydt3gr+EjSPXFP6yC2WmZUwDVMw42PPJGN90Onbdu+VaHtucFnwkaQ65p/XUGzVfBj2z33s8IukzQcXx4JvUGyZejHu+97r5cLfHhjMzcsZ5THh7oiiO27wV/CRpHrin9ZO27wV/CRpHrin9ZYbW6KI7bvBX8JGkeuKf1k7bvBX8JGkeuKf1kFuiiO27wV/CRpHrin9ZO27wV/CRpHrin9ZBboojtu8FfwkaR64p/WTtu8FfwkaR64p/WQW6KI7bvBX8JGkeuKf1k7bvBX8JGkeuKf1kFuijqDhQ4OrjcKegtuvdMVlXUvEUEEF1he+R56GtaHZJPgCr4nbTAd/wA6D6REQYl6+5FZ/cP/AESu6k+1Yf7A/Mum9fcis/uH/old1J9qw/2B+ZPxI8u1F8TPEcZe5wa1oJJPQB8fgCimcLvBZj/KTpH57xT5/SRVwiiO27wV/CRpHrin9ZO27wV/CRpHrin9ZBboojtu8FfwkaR64p/WTtu8FfwkaR64p/WQW6KI7bvBX8JGkeuKf1k7bvBX8JGkeuKf1kFuiiO27wV/CRpHrin9ZO27wV/CRpHrin9ZBbrh25pKie27wV/CRpHrin9ZHcLnBWWkdsjSHXFP6yEqKiqIJL1VBkzHExxAYcDn3X7ltB0rwvRmquDSza1ulwm4RdKij6KNzrzBhwdvOO737PQr7tucFfwkaR64g9ddOXHHHL6yxxzNbW6KI7bvBX8JGkeuKf1k7bvBX8JGkeuKf1lzbW6KI7bvBX8JGkeuKf1k7bvBX8JGkeuKf1kFuiiO27wV/CRpHrin9ZO27wV/CRpHrin9ZBboojtu8FfwkaR64p/WTtu8FfwkaR64p/WQW6KI7bvBX8JGkeuKf1lm2ThF0FfblHbLJrbTtyrpM8XT0lyhlkdjwNa4k/MgqkQdC1l9vdqsNvfcr7daG10UZAfUVdQ2KJpPQC5xAGe8g2aKI7bvBX8JGkeuKf1k7bvBX8JGkeuKf1kFuiiO27wV/CRpHrin9ZO27wV/CRpHrin9ZBboojtu8FfwkaR64p/WTtu8FfwkaR64p/WQW6KI7bvBX8JGkeuKf1k7bvBX8JGkeuKf1kFuiiO27wV/CRpHrin9ZO27wV/CRpHrin9ZBZTuazunua1oG8k4woKPRdku7LZcmHYqYOJfLsu2myBuCQ4fHg710ak4S+Cq7WaqoHcJWkW8dEWtIvUAw7vH3XhwtDwWa64NrDp4tuHCHpSKtneXyxPvEALMbg3G38/zr0cczhxzlGVS5ZxeURT1IWCzAAdj4f8AFc9gLP4vh/xU523OCv4SNI9c0/rJ23OCv4SNI9cwesvPEy6VCj7AWfxfD/inYCz+L4f8VOdtzgr+EjSPXMHrJ23OCv4SNI9cweslyVCj7AWfxfD/AIp2As/i+H/FTnbc4K/hI0j1zB6ydtzgr+EjSPXMHrJclQo+wFn8Xw/4p2As/i+H/FTnbc4K/hI0j1zB6ydtzgr+EjSPXMHrJclQo+wFn8Xw/wCKdgLP4vh/xU523OCv4SNI9cwesuHcLnBWBu4SNI9c0/rpsqFJ2As/vCH/ABWi1rardSWuKSmpWRPM4aS3IONl27/BbbS2q9MapjnfprUVpvTKcgTOoKxk4jJzgOLCcZwenwLG4QPuPD+EN/Rct8e8ohnKIpsdM/e7bfwSL9ALY4zuK12mfvdtv4JF+gFslnLzLUeE3wdwxHSdKTGwkvm3kf8A3XqgfFEMYiZ9ULR8HX3pUn9ub9c9b95xjvfGoqLu9Rdrjqa6W203FlrZaaSGYl1PG5tRLJtkB5cDiMBgGW4O92/cuXasdxxbT2TlEMk0tNSzcY1vHSxO2ZN2O5YCH78nOydy2GprHZrjVCavrZ6OSeIUz+JqOK5RHkkMd/OGSfpPhXVNoyzy1M0rJK2ASyOmEcU5ayKR2C57B/muJGT8ZKsIwKbWjKmkt1XBZ4nwVUsUUruOH2IvqhTgtGz3Q2jnfs7guqn1/bjdGUdZRQUzZJWMEgmbIGBzZS0vwMA5ixgE+6G9ddToe2U4mp+zldTSzzMfStdVDAe2TjW5b/nfZe6+dbS3aCsNDU09Q0VUpp9kxtml2mtDQ8AYx0Ykd9Kukao8JFnEMcnY97jJVRQhrACRG8gcad25oLmj5XDw7s3SuuLXfKG5VYouIjoIWVDyO6BjeHluDgd13Dst727fvWXBobT0XKS2mf8Awh0Bk+yd6GTjGD4htdI7+FkWnS9qtlJW00PKJY6uMQyiaTaxGNrDB4ANt30obaqfVtXHJycach5Vtwdw6qAaWTFwjeDsnvseCMbi3vr5vOtobTPVPq7SzkULnxMmbKNqSRoZkbOzuG0/Zzk9BOO8tvQ6UtdI093VVD3TRSmWaXbf9iGGNz/NG/d8ZWph0BRGKnbWXGvqiBKZ2yTbTXSSO2nuaMdz3e8eBDbAl4RYDbaitpbJHMKSnqaiq/hAa1rIWhzi0lmXZa4d4b8j41tX60sTKOSdzYmviuDaKSJ24s2qgQ7e8bxvB3Z8C7maHsvJ66GZ1dUNrqSWlmMs207Ykbsvxu3OIwM/EE5jWN1byqXlUzhUcoZG+baYx/HNmOBjcC9rSR4FImDcMFmuqOaGarprYJqOlLTVyh4Bja6RzA5ox3Xudo7xu8KQ6zkfSmfsA0xxwCqmc2cHi4HPLGO9zvcSyQ7PeDenes86IsXG5YKmONzg6WFk2I5tl5eNsd8An6F3VGjrPLFTsaaqJsUXEERzFvGx7W0GP/nAHP0nwpo2xtQ6oba7o2lhtMdVCKWOqlm40MLWPl4sYbsna3kHpCwqjW/F2ySvZYA5jqYVdK184bx8JeG5PcnYd3TTs79x6VQXXT9suNXJU1McokfTspzsvwNhsgkA+ssGLRdljp5aXbq3wPgMDI3zkiGPaDthn80bm/QEalrrxraO1U1XV1VmDqakjlbK+OQH7PHAZnRjd0bII2vCDuC4qNd0cHI5zboH0lXUMijk5Q0OcHzcU1zWbOSM79+MDv53LZXbRNkuUla6q5U5layRskQmIjaZIuKc9o7ztjdldNToCwT1jqsurI3OkbLsMn2WZbJxjd2NwD9+EuEhur/V9jLaamKiiqJXTRQsjLtgF0kjYxk4OAC4d5TDtbO4njo7FE5kVKyoqSagDiw6V8YDRs91vjJ7y3V306bnRUFLU3Or2aSr5SZI5NiR5DX7GT8Rc0/7IWFZ9DW2hpa6CSpq6kVbRFmWYksja972tHg3vJJ76aJc6U1TBfb7LbRQU8OxSipBZO2RwBeWbL2ho2HbujJVZxMP9Ez6oWksemrRZ61tXQwGObkxgc7azttLy8k+F20elb1pyUkhIcK8UTdLU5bGwHs1aRkDvdkadV7QGjAGApPhZ+9Sn/Hdo/8AUaZVqiiIiDEvX3IrP7h/6JXdSfasP9gfmXTevuRWf3D/ANErupPtWH+wPzJ+JHliakAOnrj8VLKR8XcFYWiYojoyxkxsJNvgz3I/o2rO1H971y/BJf0CsTRH3l2P8XU/6tqK2fFxf0bN3+qF9iGLG+Jn1QvII6bYo6rUrqqsfWw6mZEBx52TGKljdnHRjBIVFS6yvU09DR8RbWzV4p3wybbjHE2WKokw/d7oCnwPCXfErSTNL7iYf6Jn1QnEw/0TPqhefRcI/wDA6ttRStZWR1Rp4CwOMEpAaTh/RnBccfEsS78Il3tUTKyegpp6aeWqZEyMu2wIZ+KGfDnGfiSi3pnEw/0TPqhOJh/omfVCiafWNwOsaWyyU1PJDM9sTpYg8ta/iGynuiMf524dOMFLprC5UtRWujgoBBFcIrfEJpS15ke6IB7v9UcYc439ylFrR0UQ/wBEz6oXwY49rHFsH+yFCVOtrvG26s5LbjJaYXzVBMrsTBpO5nx4B6e+QtRYb3X225Q3CvPK6i5NdsF1Q53EsbWQwlrm9A/lgRj+aUot6sIYcfyTPqhDDDj+SZ9VecXLhJrKZjGxWoTSSYDBEHSEE1Jh9y0ZIwNpcs4R7g1kTaixTxzSRUzi0xuAYZJ6iIl27uQRCHNz/OSpLeiCnhGDxTN3+qFy6KIDPFMJ/sheX0/CdX10VLJSW6BsFRT0zhUEuc3bkyH4aN5aHgt+VrsqgodUXG5ad05UwQ0sFXeKUVDjM4iOLDA4jwk5I3eDKnWbOyuMcWQOLZvP80L7bDEemJn1QvGYdXXGon511UAfFFMIKek5Q4BrjAXbQbjBbtA7z3lv63X11o4HxPgoH1TWCVrYNuQOYYhJgADO7OCegDB74Vot6RxMP9Ez6oTiYf6Jn1QvP5dbXenqZJZqSnlpWw0EojhDnSYqXvbgDG8t2D8uVzSa7uNY6aSGhp4Y6bZdKycubI5r6qaABo8IEOSD33AJRa/4mH+iZ9UJxMP9Ez6oUVqbWVba7/XW2Gmgc2ltvLw+UkB+GTEsz0ZzG35slYFw1ze7ZcjbKihpamrkizTMjcW8ZI5oLWHPQAScnvAEpRb0TiYf6Jn1QnEw/wBEz6oXlFHwq3SekfWCwvfAyne8vEbwwObTCb3RGCCctwN/QqBurbxW1tCyhpIII6m6S2/Zq2vZI0tgllDiMbt0WMfGlFrfiYf6Jn1QprWUcbbppstjaM3MDo/+29dukdRS3yWrY6nZGKRrI5S0ndPl22zf/NbxZ/218ayz2S03np7KD9W9RVMOhTOvANmxjvG80wI73SVTDoU1rzosX46pvzlBRcTD/RM+qE4mH+iZ9UL7RB8cTD/RM+qF8OiiBH2NmP7IXctBquSspqu01tPBU1EFPUuNTFTtLnljontB2RvIDiDhBuxFCRnimfVC54mH+iZ9ULzK06tvFFbeUcnbJTmS8VT45siZohqp8R+AbtlvxYK++2RcqcSCrsdSS2CWVpbTSNLy2MvADSM7PRl3gIKtJMvSuJh/omfVCcTD/RM+qF5bX8KlZTU007LVFOIIpZJgxxJBbR8oYwDpJcQ4fJhbyj1pcX6Wu10qLTJDNRSNjibLG6ISbWzsuw7eAC7f8iUWtuJh/omfVCcTD/RM+qFFVepr5SXC300otb2TCpMz43uIHEjPzZGPk3rUUHCTcC+VlVanSGKVrMQxP2nNdTSSh2yRnGY8Z6MHKUW9KMEJP8kz6oTiIR/omfVC88dr+4ugbXimoo6bAY5r3uy1/JG1BJP80bQC4tmub3X3GS1xUlFFUMlcA6p24g5oja/oIzvzjPRjelSdnoohh/omfVC54mH+iZ9ULzyLXV3EEdxkoITRyttz2xMa4zAVT2DoxvLQ47vCuIuEC4z01TXR0NPHT0WHTxTOLZZGmZzAGt7zsN6D39yUW9E4mH+iZ9UJxMP9Ez6oXnrNbXo0c1TxFvAgpTWyBzyNqPjHMaxvhd9jcT4Mt8KydW63rrRfqShpaETQy0rKh7nMcdkOkDMEjc0byclKLXPEw/0TPqhOJh/omfVC8zs3CFeLhU09A6ip4KmtbC+nkma+Nga4O2ukd0O53Ebt6zKPXlwrOQzQ01G2J80MFSC8klz5ZI9ph6C37GSPCCEot6BxMP8ARM+qE4mH+iZ9UKM4PdX1uoquogqqaOMMpoalj2Me0OEgJ3bQ3t3bnDpVsDkApRE2+OJh/omfVC4MMOP5Jn1V2IehRU5QMYzhFuYYxrc2ikJwOn7NUrjhA+48P4Q39Fy+qT/KLcvxRSfrqlfPCB9x4fwhv6Ll04veGc/VsdM/e7bfwSL9ALZLW6Z+922/gkX6AWxJwFjLzKx4aDg6+9Kk/tzfrnr611WT0NlbPDUSUjDURMmqWNBMMTngPfvBAwCTkggY3rUafm1TZrY23DTUNS2J8mzK24BoeHPc4HGxu3ELOfd9UPGDpGPrFvqKK89qpqmqvEFyrbiaqSKCJtoEtKx/L8VszC8ZZuPFcU47GzkODs4C7bdqi4S2ShqGaoqql1TSxOq5Q2GJtJUF3uC8t2WDAdvO1tYBG5wV32U1L3I5nxYb0Dsi3d/uLFtst8t9ugt9LoyNtNAwRsY65h3cjozlm9W0pB2itq665UF3uVaKm5ialioIJIWP5QBWOjle1zowctYA4lgZggOOBuWxdfr5Uabq6ql1FWOmZTUks7hHGDS1L5NmWnwWbsA+5OXDA3q07J6l3f8AY+Ld0fxi3d/uJ2T1Nv8A+x8W85P8Yt3/AO4llI2t1PqG3XC4sklr5eTXCGCjiZA1/KYzUsjeTho3jaY3vYDs9/LdzR119q9F2x9RcpmVs1zbT1FTBG3IYZi07O00gDAxkjK3JuWpT06OiO/O+4t9RfXZbU+ABpCLAOcdkW9P1Esp5/Q6mv3JpTJfKqS4Qx05honsiaZg+Il7nDY2jgjayMAd/pC5pNZ6iZNFV3auZQUc1rZLA2VrWZdxUuzI4kY2nuYHhu7c5oxkFWdO+9U9wqq+LRUTair2OOd2SB2thuy3/M3bvAso3LUrhh+jonDGMOuLSP0Esp55V6yv1TRsmpL5TR0vFxmWrdPHEyOTk7HFu1xbwTtlx2cZJGBjGF3VWsri9ks8OpY+XckrRLQxGMMjlipC/LWObxgxIHbnb+5ORuwrw3HUhGDo6HHfHZBuD/uLEZX3YVrKZuiKUTQZqGYrm9yX7YJHcdJy7PylW0S+otb3R8la2wXWkqzCapx4qRjxHEJKZjZCWg4DWySHOCN2TnC5t161NPJOJby6TktumqYuRlszZ8TbILiYm7eAP8wAHHxqz7Jak72jYd4x90G+ouW3TUzQA3R8QA3bri31FLWmmo7zcTp3UUtFc5bnDSSRNpbgWNeXNdHG6TAY0A7G045xuJIO9pWpuGo62OSVlv1TU1cUMJkt8joGAXGbjADFtbOJBvAzHjc7p7kqvbdNTNaWt0hEAfBcW+ouOyepcNHM6EBpyB2Qbu/3EVA2PV1+kppHX2+0dAameKKRgqGiWie6oG23ZdGBFsx7e6Rzj3IO8FddRrDUUtDUw2u7SVbY28Ya3i2vHFisdGAHMbsl7m7AwBjZ2j4M30lZqCRzHyaKp3uY7aaXV7SQejPuEhrdQQiQQ6LgjErtp4bcGjaOMZPcfEPoViYglEXPVWtG0sOXQwRSPibJUvlDGxxmeZvGOfxbmxklsbN4cMFd9FqTWBp5hJUQTMZLQtfPE7OwHvIdsgxja2h0ndjpG5WpumpiCDo+Ig4yOyLd/wDuIbpqYgjmhGM/1i31EnJKef0upNaV81MTNVxtqaWldTwU7GMnkyakOeC9paHExteQ7HcADpyvUdF1FTVaVtNVWvL6ueiikmc4AOLi0Ekgbuk95a5t01M3BGkI8gY+6LfUX0276oad2kWfJ2SHqLN2rq4WfvUp/wAd2j/1GmVaoLVXOm/0FLbjpyCjYLnQ1Mkz68ODWQ1UUztwZvJbGQPjIV405HgSRyiIgxL19yKz+4f+iV3Un2rD/YH5l03nfZ6z+4f+iV3Un2rD/YH5k/EjyxdR/e9cvwSX9ArF0QM6Ksf4up/1bVnXmF9TaaulixtzQPjbnoyWkDP0qV07W6st2n7db5NKRPfTUsULnC4gZLWAZ9x8SKpW2W2Np3U4pI+KdPygtxu4zaDtr5cjKxG6T062klpW2qBsMsjZHtbkHabnZwQcjGTjHRk+FYnZrVXkjH1kPUTs1qryRj6yHqKoymaQ02x73ttUDS4HcCcDOMloz3JOyN4wdyyRYLOG0reQxEUoIhyPcZIJ+XJAJz0lazs1qryRj6yHqJ2a1V5Ix9ZD1ENM6l0xYqWqiqqe3RRzRBgjeM9zssEbcd4dyAPkAXNbpmw1tVPU1dsgmmnj4uV7hnaG7f4Adw3jfuG/cFgdmtVeSMfWQ9ROzWqvJGPrIeomzTMOlNPFlOx1rgLacksByc5O0dr+dv391lcP0np54qdq2QnlRBlO8EkODxjB7nugDuxvGVidmtVeSMfWQ9ROzWqvJGPrIeohpyzQumY5+NZQEZjdG5plcQ4Eh3fOQQW5BBG8k9K2MWnrNFEIo7fE1gbGwADvRuc5n0Oe4/K4rW9mtVeSMfWQ9ROzWqfJGPrIeomzTYUem7JR8j5LboIRRRiKmaxuBG0ZwAOjdk/SV9VWnrNU2yC2zUEZpacBsMYyNgBuzgEbxu3LUR6h1PJUSQN0izbjALv4xHfz/qfEV29mtVeSMfWQ9RJuDTIu2lbRWQTcXSRQTvpHUscrW54phYWdy3oGGnG7fjd0Lro9GaeitFPbpLbDJHDv2sEOcdkA5OckHAyCcbgO8up141Scf9kY939ZD1EbeNUt/wDpGPrIeopZTcCzWwPDxRxBwbEwEDvREmMf7JccfKuiTTVjkqYal9tgMsL3PY7Hfc/bOfD3RJ39B6Fr+zWqvJGPrIeonZrVXkjH1kPUQpsrpp6zXR0jq+gjndK1rXkkgkN2sDIPR3bvpK7ZbNbJbi24y0UL6toIbK5uXDIwceDcSFqOzWqvJGPrIeonZrVXkjH1kPUQpmM0pp5kjnttVOC6Dk5GN2xs7OMdGdkYz043ZWU+1UIljlZTMEjJ3VDXeCRzSwu+XZc4fOtT2a1V5Ix9ZD1Fw68apP8A9Ix9ZD1EKbDTNmZaKWoYZRNPUVMlTPKGBu295yd3g6B8ywNZjFz03+NG/q3rgXnVI/8ApGPrIeosOr5x3i82c1VhioaekqxPJLy0SHAa4YxsjwhFWY6FNa86LF+Oqb85VI33IU7reluFVR0EltpmVM1LcIaninSbG21pOQDg46UFGimezWqvJGPrIeonZrVXkjH1kPUQUy+XMDnhxzkdA7ym+zWqvJGPrIeonZrVXkjH1kPUQbeOzWyMgso4wQ6V3RnfK4uk+s4kn5Vi0elrBRsLILbE0Oa5hJJcS1zQ0tyTnGyAMd4dCwuzWqvJGPrIeonZrVXkjH1kPUQbFmnLIxjmC20+y5xc4Fmdo8XxW/w9wS3f3l90NhtFFbpLfTUMbKWUESRklweCMd0TvO7dvWr7Naq8kY+sh6idmtVeSMfWQ9REpsodP2aGnhgioImRwh4jAzu2xh/y57+V1Uel7DSO26a2xRvxguBJJ7lzd+Tv7l7h8hx3gsLs1qryRj6yHqJ2a1V5Ix9ZD1EKfEWgNMRgBtE8jj5JiDK4h23tAtI6NnZdsjdnAAyviv0Np+oraWQ0bRFDt8YzacTKXNwNo5ycAkfIcdGF3dmtU7zzSi6yGf0F0Ul/1NV00dTHpFuxK3abm4gHB/2Fd+TUTTfOs9se7aNHEMuid3IwMxEGPo/mkbl0TaasU00M0tsgc+F20wlvf2trf4cOyRnoJyteL1qryRj6yHqJ2a1V5Ix9ZD1FCmfPpqxTmn462wv5OCI8g7gXBxB8IyAcHduXZXWG011fDXVlGyaohADHOJ6AcgEdBGd+9azs1qryRj6yHqJ2a1V5Ix9ZD1EKZMekdOx00lO22s4uQNBy5xIDTkbJJy3GTjGMLGdoXTpndOylkifx0MrOLlLWs4oMDGho3bOGDpHfPhTs1qryRj6yHqJ2a1V5Ix9ZD1EKbOz2C0Wgk22iZTksDCWkk7I6BvPQO8O8tmpns1qryRj6yHqJ2a1V5Ix9ZD1EVTIehTPZrVXkjH1kPUQ3rVOPvRj6xHqIOyk/yi3L8UUn66pXzwgfceH8Ib+i5fGnI7xUapuF2udrZb45KGCnjaKjjS4skmcT0DG54X3wgfceH8Ib+i5b4veGc/VstM/e7bfwSL9ALYEZGFo7ay/UVvp6MUVvlEEbYg81b27WyAM44s46Fk8ff/Ftu8+f7JZyjcrHhtGtAXK1XKL/AOLLd58/2Scov/iy3efP9klFtqi1XKL/AOLLd58/2Scov/i23efP9klFtqi1XKL/AOLbd58/2Scov/iy3efP9klFtqi1XKL/AOLLd58/2Scov/iy3efP9klFtqi1XKL/AOLLd58/2Scov/iy3efP9klFtqtWwf8AaqY5/wC5R/pvXHKL/wCLbd58/wBklviuDrrLXVsNNEDA2JrYZjJ0OJzva3wpQ2qIiiiIiAiIgIiICIiD5cwOXLRgYXKICFEQYl4+49b/AHD/ANErtpD/AAaIbtzB+Zfc8TJ4JIJRtMkaWuGcZBGCtQ3S9pbnHLx3sC41GB/v7kjwjcO3jwLkLT82LV4bh1lUeunNm1eG4dZVHrorcb03rT82bV4bh1lUeunNm1eG4dZVHrojcb03rT82bV4bh1lUeunNm1eG4dZVHroNxvTetPzZtXhuHWVR66c2bV4bh1lUeug3G9N60/Nm1eG4dZVHrpzZtXhuHWVR66Dcb0PRvWn5s2rw3DrKo9dDpm1EYzcOsqj10GTSfdmsx/RxZ/31sFh2y20tua9tMJe7OXGSZ8hPzuJKzFZkgREUUREQEREBERAXGN65RAC+dgZG9fSICIiAiIgIiICIiAiIg+XDflYOnfuJRf3DPzLsvFxpbXRPra2TiqePG2/ZJxk4HQFqdDXu3Xazwtt9SJ+TxsZLhrm7LsdG8DwLcYz0mfxzyn7RCiHQiDoRYdBERAREQEREBCMoiD5Dcb85U7wgfceH8Ib+i5Uh6FN8IH3Hh/CG/ouW+L3hnP1UeB8SYCYTCw0YCYCYTCBgJgJhMIGAmAmEwgYCYCYTCBgJgJhMIGEwm9c71AREVBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAWo1Nfqew21tbPSVtUXzMhjgpYw+V73dAAJH51t1L67ZvsR/rqm/OUGHz+k8htZ9Xs9onP6TyG1n1ez2is9lNlBGc/pPIbWfV7PaJz+k8htZ9Xs9orF27CxYbjQzV0tDFUxPqYhl8TXguaPjCCY5/SeQ2s+r2e0Tn9J5Daz6vZ7RWIIPR4cL522/H86CQ5/SeQ2s+r2e0Tn9J5Daz6vZ7RWBcAMuBH/suA8YzgoJDn9J5Daz6vZ7ROf0nkNrPq9ntFYBzT0b+/uTaHeBKCFuusG3K3z0VRoTWJjmjLHDsezvj+8Wp0PfH6csbKE6K1i+Yvc+V7be3DiTu/0ngwF6RV19HRvjbVVMUDpXBjOMcBtEnAA+NZAwQtRyZRj1Z6xM2jxr6QD7xtZ9Xs9onP6TyG1n1ez2isM47xK42wBv3LLSQ5/SeQ2s+r2e0Tn9J5Daz6vZ7RV5e0DJBTa3+5P0oJDn9J5Daz6vZ7ROf0nkNrPq9ntFYbQzuB6cJtbyACUEfz+k8htZ9Xs9onP6TyG1n1ez2ir9seA46Vy0g9H50Efz+k8htZ9Xs9onP6Tv6G1n1ez2is9lcObuQaHS+qYL9V1dGLVdrZUUsccjoq+ARucx5eGubgnIyxw+ZdfCB9x4fwhv6LlzRDHCJch/VFJ+uqVxwgfceH8Ib+i5b4veGc/VSIiLDQiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICmtedFi/HVN+cqlUzrpwPYLHjqm/OUFMiIg6K5r3QObEQJC0hhIyA7G4ry+y09sfb7BbqGF1PqGGoaayaKHE0OAeOdI8t3gjIycgnGN4XqrxtALq4hgeXtjYHnpdgZPyoS8tjn1M2z2ubszdXTOoI6l4McfdvM0LC1w2OjZLt3TvKwqDUt7ktsVRDfa+sM7dmqk4uKKOlk44tAL+KIaNnILsOO4Hp3r2ARN77W7gAP8A58wWHabPR2u1st9HC1sLM4Dt/SSd/h6Vbhmnk1u1Lq6S4W0G4yPzNE10ZLSZIzVPjcQ3iQXgRgEuzHjG1jpafmbVeqDpyF01VVwXRtK6ct2GsY7uYtgNPEuMjjku2BjpIJ3BezCnYHhwjjBGQDgbsr5dTscO6iZu6Nw3fJ4E7QUjNYXO4RXagDblV26hfROlifTQCQ1FRtMxEQWu/wA3OAMZ2id+zun+z16bLSsF7ruJnjBuzjSxh1tdxrWgNPF43gvBByQIw7dnf6tJFtbOcHZORkdBXy6mZskBrO6OXDHuvl8KWU8hiNbLfKa711bJNXhkENu26KPNXEKyZpccx5B4ssfhhb7oHvDHbS62upo7SyK5Nmq5qK3h0TmtG3O+OTjGE43Ev4sEZGMjoyvWRC3aYXsYS3o3e5HxLqloKWZo26aFw2muGWDcWnIPygjKdoKl5VpfUOupX28SyUtY6edjZonzOaWuMEjntc7k7RH3TQdnDnAjZyMrFh1JriqjoJn1k1LDFJ/CJnUzcPc+hkeG42M7LXN2iQN5Le8CD7M2BjSS1jBk7RwB0+H5VwYW4A2GH5h4MfuTtBUvPNPXHUjdIaiqqtta2spw11LHPsSyR5gY442WNB3udux8XxLVXm/ajjrqaCw3p9Xbg1/F1tXiLjZsx/Y+4gdxgAJwAGE5dvOzu9aLOnDWjPT8a+RTxgACNgDTkDG4FLWnjjtS6ifbKepo75U1VRM4i4NLGRR0ZE4a1oPFEt7nI35yBtYC7LVqLWstfaopqpga9zA0yvxyhnKpWOIAp/sv2JrCHDih3QdgA4XqtstNJbaEUNLA0U4cXbJ3kkkuOSeneSsswty3DGdzuBwNw+LwJZTyGy6uvFPpug1CbzVXikioaWpuLXU7GnjJGljmM2WD/Ocx2PAPjyvVrHFXx2ajZc5hNXCFvKHgAAyYG1geDOcLDj03bY7VQWuOItpKF0LomB3TxWNgO/nYwDv74W6G4dKTNkQIehFwTuUVPUn+UW5fiik/XVK+eED7jw/hDf0XL6pP8oty/FFJ+uqV88IH3Hh/CG/ouW+L3hnP1UiIiw0IiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAiIgIiICIiAtHqyzVd3pKZlHVx0lRS1cdTFI+LjG5YTgEZHhW8RBL9j9ceUVq6uPrp2P1x5RWrq4+uqhEEv2P1x5RWrq4+unY/XHlFaurj66qEQS/Y/XHlFaurj66dj9ceUVq6uPrqoRBL9j9ceUVq6uPrp2P1x5RWrq4+uqhEEv2P1x5RWrq4+unY/XHlFaurj66qEQSrrfrfO/UVr6tPrrotkGt6u3wVJv9qYZWB5b2PJxn/bVe89C1+nXDsHRb/wDQN/MtR4Zmd01PY/XHlFaurj66dj9ceUVq6uPrqoCLLSX7H648orV1cfXTsfrjyitXVx9dVCIJfsfrjyitXVx9dOx+uPKK1dXH11UIgl+x+uPKK1dXH107H648orV1cfXVQiCX7H648orV1cfXXDrdrgjHOK1dXH11UogndOWW70l6rLreLpT1ss9NFTsENPxQa1jpHbxtHJ+yL54QPuPD+EN/RcqQ9Cm+ED7jw/hDf0XLfF7wzn6qRERYaEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAXGRnGVyp7Wd0uNottO+z0dLV11TVxUsMVXO+GLaeelz2teQAAehpQUKKI5XwseTuievar9kTlfCx5O6J69qv2RBboojlfCx5O6J69qv2ROWcK/k9onr2q/ZEFuih+W8K/k9onr2q/ZFzyvhY8ndE9e1X7Igt0URyvhY8ndE9e1X7InK+Fjyd0T17VfsiC3RRHK+Fjyd0T17Vfsicr4WPJ3RPXtV+yIKTUtwdarRUXEU76kQM2nRsOCR31peDi/C+2XbZRzQR0+zFtSEEPIG/GPmWDUz8Kk0T4pdN6Kcx7S1zezlTvB732osHTtJwm2O0xW6i05owxRkkl18qcuJJJJ/gnhXWJx+OY/XOYnu9Ib0DK5URyvhY8ndFde1X7InK+Fjyd0T17Vfsi5Oi3RRHK+Fjyd0T17Vfsicr4WPJ3RPXtV+yILdFEcr4WPJ3RPXtV+yJyvhY8ndE9e1X7Igt0URyvhY8ndE9e1X7InK+Fjyd0T17VfsiC3RRHK+Fjyd0T17VfsiGr4WMfe7orr2p/ZEFtkEblOcIH3Hh/CG/ouXXpK7akqLzX2rUtttNHPBTw1MTrfWyVDXNe6RuHbcUZBBj72envLs4QPuPD+EN/Rct8XvDOfqpERFhoREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBERAREQEREBTWvOixfjqm/OVSqa150WL8dU35ygpUREBcO/xXK4d0hBK1+u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)

*Рис. 6    Алгоритм простейшего эхо-сервера*

**Исходный код**   (файл server.c):

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#define DEF\_PORT 8888

#define DEF\_IP "127.0.0.1"

// обработка одного клиента

void\* clientHandler(void\* args) {

`	`int sock = (int)args;

`	`char buf[100];

`	`int res = 0;

`	`for(;;) {

`		`bzero(buf,100);

`		`res = readFix(sock, buf,100, 0);

`	   `if ( res <= 0 ) {

`		`perror( "Can't recv data from client, ending thread\n" );

`			`pthread\_exit(NULL);

`		`}

`		`printf( "Some client sent: %s\n",buf);

`		`res = sendFix(sock,buf,0);

`		`if ( res <= 0 ) {

`			`perror( "send call failed" );

`			`pthread\_exit(NULL);

`		`}

`	`} 

}

int main( int argc, char\*\* argv) {

`	`int port = 0;

`     `if(argc < 2) {

`		`printf("Using default port %d\n",DEF\_PORT);

`		`port = DEF\_PORT;

`     `} else

`		`port = atoi(argv[1]);

`	`struct sockaddr\_in listenerInfo;

`	`listenerInfo.sin\_family = AF\_INET;

`	`listenerInfo.sin\_port = htons( port );

`    `listenerInfo.sin\_addr.s\_addr = htonl( INADDR\_ANY );

`	`int listener = socket(AF\_INET, SOCK\_STREAM, 0 );

`	`if ( listener < 0 ) {

`		`perror( "Can't create socket to listen: " );

`		`exit(1);

`	`}

int res = bind(listener,(struct sockaddr \*)&listenerInfo, sizeof(listenerInfo));

`	`if ( res < 0 ) {

`		`perror( "Can't bind socket" );

`		`exit( 1 );

`	`} 

`	`// слушаем входящие соединения   

`	`res = listen(listener,5);

`	`if (res) {

`		`perror("Error while listening:");

`		`exit(1);

`	`}

`	`// основной цикл работы     

`	`for(;;) {

`		`int client = accept(listener, NULL, NULL );

`		`pthread\_t thrd;

`		`res = pthread\_create(&thrd, NULL, clientHandler, (void \*)(client));

`		`if (res){

`			`printf("Error while creating new thread\n");

`		`}

`	`}

`	`return 0;

}

int readFix(int sock, char\* buf, int bufSize, int flags) {

`	`// читаем "заголовок" - сколько байт составляет наше сообщение

`	`unsigned msgLength = 0;

int res=recv(sock,&msgLength,sizeof(unsigned),flags|MSG\_WAITALL );

`	`if (res <= 0)return res;

`	`if(res > bufSize) {

`	  `printf("Recieved more data, then we can store, exiting\n");

`		`exit(1);

`	`}	

`	`// читаем само сообщение

`	`return recv(sock, buf, msgLength, flags | MSG\_WAITALL);

}

int sendFix(int sock, char\* buf, int flags) {

`	`// шлем число байт в сообщении

`	`unsigned msgLength = strlen(buf);

`	`int res = send(sock, &msgLength, sizeof(unsigned), flags );

`	`if (res <= 0)

`	`return res;

`	`send(sock, buf, msgLength, flags);

}

**Исходный код**   (файл client.c):

#include <sys/types.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <unistd.h>

#define DEF\_PORT 8888

#define DEF\_IP "127.0.0.1"

int main( int argc, char\*\* argv) {

`     `char\* addr;

`     `int port;

`     `char\* readbuf;

`	 `if(argc <3) {

`		`printf("Using default port %d\n",DEF\_PORT);

`		`port = DEF\_PORT;

`	`} else

`		`port = atoi(argv[2]);

`     `if(argc < 2) {

`		`printf("Using default addr %s\n",DEF\_IP);

`          `addr = DEF\_IP;

`     `} else

`		`addr = argv[1];

`	`// создаем сокет 

`	`struct sockaddr\_in peer;

`     `peer.sin\_family = AF\_INET;

`     `peer.sin\_port = htons( port );

`     `peer.sin\_addr.s\_addr = inet\_addr( addr );

`     `int sock = socket( AF\_INET, SOCK\_STREAM, 0 );

`     `if ( sock < 0 ){

`          `perror( "Can't create socket\n" );

`          `exit( 1 );

`     `}

`	`// присоединяемся к серверу

`     `int res = connect( sock, ( struct sockaddr \* )&peer, sizeof( peer ) );

`     `if (res) {

`          `perror( "Can't connect to server:" );

`          `exit( 1 );

`     `}

`	`// основной цикл программы

`     `char buf[100];	

`	`for(;;) {

`		`printf("Input request (empty to exit)\n");

`		`bzero(buf,100);

`        `fgets(buf, 100, stdin);

`		`buf[strlen(buf)-1] = '\0';

`		`if(strlen(buf) == 0) {

`			`printf("Bye-bye\n");

`			`return 0;

`		`}

`		`res = sendFix(sock, buf,0);

`		`if ( res <= 0 ) {

`			`perror( "Error while sending:" );

`			`exit( 1 );

`		`}

`		`bzero(buf,100);

`		`res = readFix(sock, buf, 100, 0);

`		`if ( res <= 0 ) {

`			`perror( "Error while receiving:" );

`			`exit(1);

`		`}

`		`printf("Server's response: %s\n",buf);

`	`}

`	`return 0;

}

int readFix(int sock, char\* buf, int bufSize, int flags) {

`	`// читаем "заголовок" - сколько байт составляет наше сообщение

`	`unsigned msgLength = 0;

int res=recv(sock,&msgLength,sizeof(unsigned),flags|MSG\_WAITALL);

`	`if (res <= 0)return res;

`	`if(res > bufSize) {

`	  `printf("Recieved more data, then we can store, exiting\n");

`		`exit(1);

`	`}	

`	`// читаем само сообщение

`	`return recv(sock, buf, msgLength, flags | MSG\_WAITALL);

}

int sendFix(int sock, char\* buf, int flags) {

`	`// число байт в сообщении

`	`unsigned msgLength = strlen(buf);

`	`int res = send(sock, &msgLength, sizeof(unsigned), flags );

`	`if (res <= 0)

`	`return res;

`	`send(sock, buf, msgLength, flags);

}



`	`Для взаимодействия используются TCP сокеты, это значит, что между сервером и клиентом устанавливается логическое соединение, при этом при получении данных из сокета с помощью вызова recv, есть вероятность получить сразу несколько сообщений, или не полностью прочитать сообщение. Поэтому для установления взаимной однозначности между отосланными и принятыми данными используются функции recvFix и sendFix. Принцип их работы следующий: функция sendFix перед посылкой собственно данных посылает «заголовок» - количество байт в посылке. Функция recvFix вначале принимает этот «заголовок», и вторым вызовом recv считывает переданное количество байт. Считать ровно то, количество байт, которое указанно в аргументе функции recv, позволяет флаг  MSG\_WAITALL. Если его не использовать и данных в буфере недостаточно, то будет прочитано меньшее количество.

**Результат выполнения** клиент-серверного приложения

Клиент:

de@de:~/lab4/6-socket$ ./client 

Using default port 8888

Using default addr 127.0.0.1

Input request (empty to exit)

hi

Server's response: hi

Input request (empty to exit)

it's client

Server's response: it's client

Input request (empty to exit)

bye

Server's response: bye

Input request (empty to exit)

Bye-bye

Сервер:

de@de:~/lab4/6-socket$ ./server 

Using default port 8888

Some client sent: hi

Some client sent: it's client

Some client sent: bye

Can't recv data from client, ending thread

: Success

^C

de@de:~/lab4/6-socket$ 

**Пример использования программы в локальной сети.** 

Для этого программу-сервер запустим на одном компьютере, программу-клиент – на другом. Перед запуском программы-клиента необходимо узнать адрес сервера с помощью команды    **ifconfig**:

g4081\_13@SPOComp8:~$ ifconfig

br0       Link encap:Ethernet  HWaddr 48:5b:39:78:43:3e  

`          `inet addr:192.168.100.1  Bcast:192.168.100.255  Mask:255.255.255.0

`          `inet6 addr: fe80::4a5b:39ff:fe78:433e/64 Scope:Link

`          `UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1

`          `RX packets:5 errors:0 dropped:0 overruns:0 frame:0

`          `TX packets:6 errors:0 dropped:0 overruns:0 carrier:0

`          `collisions:0 txqueuelen:0 

`          `RX bytes:264 (264.0 B)  TX bytes:468 (468.0 B)

eth0      Link encap:Ethernet  HWaddr 48:5b:39:78:43:3e  

`          `inet addr:10.1.208.8  Bcast:10.1.208.31  Mask:255.255.255.224

`          `inet6 addr: fe80::4a5b:39ff:fe78:433e/64 Scope:Link

`          `UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1

`          `RX packets:880 errors:0 dropped:0 overruns:0 frame:0

`          `TX packets:48 errors:0 dropped:0 overruns:0 carrier:0

`          `collisions:0 txqueuelen:1000 

`          `RX bytes:67154 (67.1 KB)  TX bytes:4993 (4.9 KB)

`          `Interrupt:41 

Таким образом, IP адресс сервера – 10.1.208.8

После запуска сервера и клиента убеждаемся, что программа работает корректно:

Сервер:

de@de: /lab4/7-socket$ ./server

Using default port 8888

Some client sent: hi

Some client sent: it's me

Some client sent: how are y

Can't recv data from client, ending thread

Клиент:

de@de: /lab4/7-socket$ ./client 10.1.208.8

Using default port 8888

Input request (empty to exit)

hi

Server's response: hi

Input request (empty to exit)

it's me

Server's response: it's me

Input request (empty to exit)

how are y

Server's response: how are y

Input request (empty to exit)

Bye-bye

de@de:/lab4/7-socket$ ^C

de@de: /lab4/7-socket$ 

**Самостоятельно** 

Попробуйте модифицировать предложенное приложение и реализовать обмен сервера с множеством клиентов. Количество клиентов: 10, 100, 1000.

Выполните аналогичное взаимодействие на основе UDP.

Сравните с IPC очереди сообщений. 

**Заключение по разделам ОС UNIX**

В ОС Unix адресные пространства различных процессов изолированы друг от друга. Для взаимодействия процессов используются специальные средства IPC, включающие в себя сигналы, именованные и неименованные каналы, сообщения, сокеты, семафоры и разделяемую память.

В Unix поддерживается два вида сигналов: надёжные и ненадёжные. Ненадежные сигналы более просты в использовании, в тоже время надежные сигналы позволяют отложить прием других сигналов до окончания обработки текущего.

Сигналы самое простое средство IPC, являются достаточно медленными и ресурсоёмкими, не позволяют передавать произвольные данные, служат главным образом для уведомления, обработки нештатных ситуаций и синхронизации.

`	`Именованные и неименованные каналы реализуют запись и чтение по принципу FIFO. Запись и чтение, таким образом, происходит быстро, однако при создании именованного канала затрачивается несколько больше времени. Кроме того, каналы работают в полудуплексном режиме, т.е. передают данные только в одну сторону. Каналы FIFO представляют собой вид IPC, который может использоваться только в пределах одного узла. Хотя FIFO и обладают именами в файловой системе, они могут применяться только в локальных файловых системах

Сообщения являются мощным средством межпроцессного обмена данными. Время доставки сообщения сравнимо с временем доставки сигнала, однако сообщение несёт гораздо больше информации, чем сигнал. С помощью сообщений гораздо проще организовать асинхронный обмен данными между процессами, чем с помощью каналов.

Семафоры и разделяемая память зачастую используются вместе. Семафоры позволяют синхронизировать доступ к разделяемому ресурсу и гарантировать «взаимное исключение» нескольких процессов при разделении ресурса (пока предыдущий процесс не закончит работу с ресурсом, следующий не начнет ее).

`	`Сокеты являются средством IPC, которое можно использовать не только между процессами на одном компьютере, но и в сетевом режиме. Многие сетевые приложения построены на основе сокетов.


50

