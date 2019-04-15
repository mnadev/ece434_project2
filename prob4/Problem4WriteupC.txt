In order to implement arbitrary process tree generation of threads, the same procedure as problem 1 was conducted.  There's a struct containing process name, amount of children processes, and specific children process names, and the same procedure to parse the text file is used as in problem 1. Upon parsing the text, the For each thread, pfunction is used recursively to execute the arbitarary process treee generation of threads.

Normal stdout (for example tree text file given; thread sleeps for 2 secs):

Started thread A, tid=245344064
Thread A, tid=245344064: Thread created B, tid=236885760
Started thread B, tid=236885760
Thread A, tid=245344064: Thread created C, tid=228493056
: Waiting for children to end 
Started thread C, tid=228493056
: has no child thread, sleep for 2 seconds 
Thread B, tid=236885760: Thread created D, tid=220100352
: Waiting for children to end 
Started thread D, tid=220100352
: has no child thread, sleep for 2 seconds 
Thread D, tid=220100352: ending thread
Thread C, tid=228493056: ending thread
Thread B(tid=236885760)'s child thread D with tid=220100352 has exited
Thread B, tid=236885760: ending thread
Thread A(tid=245344064)'s child thread B with tid=236885760 has exited
Thread A(tid=245344064)'s child thread C with tid=228493056 has exited
Thread A, tid=245344064: ending thread

Experiment 1)

Ctrl - C :
Sending a Ctrl-C signal to the process kills the process immidiately; Stopping runtime immidiately.  Handled by a random process in execution.

SIG_QUIT :
Program is immidiately interrupted upon receiving the signal.  Resulting event similar to Ctrl - C. Last line of STDOUT is "QUIT".

SIG_STOP :

SIG_STOP resulted in a complete termination, however more forceful. With the last line of STDOUT reading "[1]+  Stopped                 ./main test.txt".  At the time of the signaling, thread B (in example thread tree) had just joined thread D, but never joined with A.  The program was terminated before thread D could join.

SIG_STP :

Exact result as SIG_STOP.  Just as expected, because SIG_STP is equivalent to SIG_STOP, the only difference being that SIG_STP can also be called using a keyboard combination similar to CTRL-C.

SIG_ABRT : 

Similar to SIG_QUIT. Stops all threads under execution.  Last line of STDOUT reads, "Aborted."

SIGTERM : 

Similar to SIG_QUIT. Stops all threads under execution.  Last line of STDOUT reads, "Killed."

SIG_KILL :

Similar to SIG_QUIT. Stops all threads under execution.  Last line of STDOUT reads, "Killed."

SIG_EGV : 

Similar to SIG_QUIT. Stops all threads under execution.  Last line of STDOUT reads, "Segmentation fault."

SIG_USR1 :

Under the current OS (Ubuntu 18.04), The signal is caught, and then thread continues execution at the instruction it left off at.  It is also worth noting that the signal is always caught by the parent thread. In the case of the example thread tree, thread A.

Experiment 2)

In order to be consistant, thread A sends the signlas to thread C each time.  It does so 2 seconds after spawning thread B and C.  For this experiment, each thread sleeps for 5 seconds.

Ctrl - C (SIG_INT):

STDOUT:

I am the main thread.  PID: 7215
Started thread A, tid=1527121728
Thread A, tid=1527121728: Thread created B, tid=1518663424
Started thread B, tid=1518663424
Thread A, tid=1527121728: Thread created C, tid=1510270720
: Waiting for children to end 
Started thread C, tid=1510270720
: has no child thread, sleep for 5 seconds 
Thread B, tid=1518663424: Thread created D, tid=1501878016
: Waiting for children to end 
Started thread D, tid=1501878016
: has no child thread, sleep for 5 seconds

Thread C handles the signal, which terminates all threads (entire process).

SIG_QUIT :

STDOUT:

I am the main thread.  PID: 8070
Started thread A, tid=-1067346112
Thread A, tid=-1067346112: Thread created B, tid=-1075804416
Thread A, tid=-1067346112: Thread created C, tid=-1084197120
: Waiting for children to end 
Started thread B, tid=-1075804416
Thread B, tid=-1075804416: Thread created D, tid=-1092589824
: Waiting for children to end 
Started thread C, tid=-1084197120
: has no child thread, sleep for 5 seconds 
Started thread D, tid=-1092589824
: has no child thread, sleep for 5 seconds 
Quit

Thread C handles the signal, which terminates all threads (entire process).

SIG_STOP :

I am the main thread.  PID: 8244
Started thread A, tid=-1240406208
Thread A, tid=-1240406208: Thread created B, tid=-1248864512
Started thread B, tid=-1248864512
Thread A, tid=-1240406208: Thread created C, tid=-1257257216
: Waiting for children to end 
Thread B, tid=-1248864512: Thread created D, tid=-1265649920
: Waiting for children to end 
Started thread D, tid=-1265649920
: has no child thread, sleep for 5 seconds 
Started thread C, tid=-1257257216
: has no child thread, sleep for 5 seconds 
Quit

Thread C handles the signal, which terminates all threads (entire process).

SIG_STP :

I am the main thread.  PID: 8605
Started thread A, tid=-1773832384
Thread A, tid=-1773832384: Thread created B, tid=-1782290688
Thread A, tid=-1773832384: Thread created C, tid=-1790683392
: Waiting for children to end 
Started thread C, tid=-1790683392
: has no child thread, sleep for 5 seconds 
Started thread B, tid=-1782290688
Thread B, tid=-1782290688: Thread created D, tid=-1799076096
: Waiting for children to end 
Started thread D, tid=-1799076096
: has no child thread, sleep for 5 seconds 
Quit

Thread C handles the signal, which terminates all threads (entire process).

SIG_ABRT : 

I am the main thread.  PID: 11378
Started thread A, tid=-748632256
Thread A, tid=-748632256: Thread created B, tid=-757090560
Started thread B, tid=-757090560
Thread A, tid=-748632256: Thread created C, tid=-765483264
: Waiting for children to end 
Started thread C, tid=-765483264
: has no child thread, sleep for 5 seconds 
Thread B, tid=-757090560: Thread created D, tid=-773875968
: Waiting for children to end 
Started thread D, tid=-773875968
: has no child thread, sleep for 5 seconds 
Aborted

Thread C handles the signal, which terminates all threads (entire process).

SIGTERM : 

I am the main thread.  PID: 11645
Started thread A, tid=-215578816
Thread A, tid=-215578816: Thread created B, tid=-224037120
Started thread B, tid=-224037120
Thread A, tid=-215578816: Thread created C, tid=-232429824
: Waiting for children to end 
Started thread C, tid=-232429824
: has no child thread, sleep for 5 seconds 
Thread B, tid=-224037120: Thread created D, tid=-240822528
: Waiting for children to end 
Started thread D, tid=-240822528
: has no child thread, sleep for 5 seconds 
Terminated

Thread C handles the signal, which terminates all threads (entire process).

SIG_KILL :

I am the main thread.  PID: 11892
Started thread A, tid=773924672
Thread A, tid=773924672: Thread created B, tid=765466368
Started thread B, tid=765466368
Thread A, tid=773924672: Thread created C, tid=757073664
: Waiting for children to end 
Started thread C, tid=757073664
Started thread D, tid=748680960
: has no child thread, sleep for 5 seconds 
Thread B, tid=765466368: Thread created D, tid=748680960
: Waiting for children to end 
: has no child thread, sleep for 5 seconds 
Killed

Thread C handles the signal, which terminates all threads (entire process).

SIG_EGV : 

I am the main thread.  PID: 12114
Started thread A, tid=1602922304
Thread A, tid=1602922304: Thread created B, tid=1594464000
Started thread B, tid=1594464000
Thread A, tid=1602922304: Thread created C, tid=1586071296
: Waiting for children to end 
Started thread C, tid=1586071296
: has no child thread, sleep for 5 seconds 
Thread B, tid=1594464000: Thread created D, tid=1577678592
: Waiting for children to end 
Started thread D, tid=1577678592
: has no child thread, sleep for 5 seconds 
Segmentation fault

Thread C handles the signal, which terminates all threads (entire process).

SIG_USR1 :

I am the main thread.  PID: 12423
Started thread A, tid=241096512
Thread A, tid=241096512: Thread created B, tid=232638208
Thread A, tid=241096512: Thread created C, tid=224245504
: Waiting for children to end 
Started thread C, tid=224245504
: has no child thread, sleep for 5 seconds 
Started thread B, tid=232638208
Thread B, tid=232638208: Thread created D, tid=215852800
: Waiting for children to end 
Started thread D, tid=215852800
: has no child thread, sleep for 5 seconds 
Caught signal 10 TID: 224245504
Thread C, tid=224245504: ending thread
Thread D, tid=215852800: ending thread
Thread B(tid=232638208)'s child thread D with tid=215852800 has exited
Thread B, tid=232638208: ending thread
Thread A(tid=241096512)'s child thread B with tid=232638208 has exited
Thread A(tid=241096512)'s child thread C with tid=224245504 has exited
Thread A, tid=241096512: ending thread

Thread C handles the signal.  STDOUT reports that Signal 10 is caught by TID 224245504 which is the TID of thread C, verifying that thread C is the handling thread. It is also worth noting that the thread C doesn't block or terminate any thread upon completation or handling the signal.

Experiment 3)

For consistency, thread A has been chosen to send signals to thread B, C, and F.  Thread F is a direct child thread that was added for this experiment.

a.)

I am the main thread.  PID: 5685
Started thread A, tid=-1887946944
Thread A, tid=-1887946944: Thread created B, tid=-1896405248
Started thread B, tid=-1896405248
Thread A, tid=-1887946944: Thread created C, tid=-1904797952
Started thread C, tid=-1904797952
: has no child thread, sleep for 20 seconds 
Thread B, tid=-1896405248: Thread created D, tid=-1913190656
: Waiting for children to end 
Started thread D, tid=-1913190656
: has no child thread, sleep for 20 seconds 
Started thread F, tid=-1921583360
Thread A, tid=-1887946944: Thread created F, tid=-1921583360
: Waiting for children to end 
Caught signal 2; TID: 140550908356352
Caught signal 3; TID: 140550908356352
Caught signal 20; TID: 140550908356352
Caught signal 6; TID: 140550908356352
Caught signal 15; TID: 140550908356352
Caught signal 11; TID: 140550908356352

In the first case, the signals are caught six times.  One time for each signal.  When the second thread is signaled, the signals are not caught.  Instead the default signal handler is used.  In our case, the first signal we send is SIG_INT (CTRL_C) thus it quietly exits when the second thread attempts to handle the signal.

b)

I am the main thread.  PID: 6657
Started thread A, tid=93599552
Thread A, tid=93599552: Thread created B, tid=85141248
Started thread B, tid=85141248
Thread A, tid=93599552: Thread created C, tid=76748544
Started thread C, tid=76748544
: has no child thread, sleep for 20 seconds 
Thread A, tid=93599552: Thread created F, tid=59963136
Started thread F, tid=59963136
Thread B, tid=85141248: Thread created D, tid=68355840
Started thread D, tid=68355840
: has no child thread, sleep for 20 seconds 
: Waiting for children to end 
: Waiting for children to end 
Caught signal 2; TID: 85141248
Caught signal 2; TID: 76748544
Caught signal 3; TID: 85141248
Caught signal 20; TID: 85141248
Caught signal 6; TID: 85141248
Caught signal 15; TID: 85141248
Caught signal 2; TID: 76748544
Caught signal 2; TID: 76748544
Caught signal 3; TID: 76748544
Caught signal 20; TID: 76748544
Caught signal 6; TID: 76748544
Caught signal 15; TID: 76748544
Caught signal 2; TID: 59963136
Caught signal 2; TID: 76748544
Caught signal 3; TID: 59963136
Caught signal 20; TID: 59963136
Caught signal 6; TID: 59963136
Caught signal 15; TID: 59963136
Caught signal 2; TID: 85141248
Caught signal 2; TID: 76748544
Caught signal 3; TID: 85141248
Caught signal 20; TID: 85141248
Caught signal 6; TID: 85141248
Caught signal 15; TID: 85141248
Caught signal 2; TID: 76748544
Caught signal 2; TID: 76748544
Caught signal 3; TID: 76748544
Caught signal 20; TID: 76748544
Caught signal 6; TID: 76748544
Caught signal 15; TID: 76748544
Caught signal 2; TID: 59963136
Caught signal 2; TID: 76748544
Caught signal 3; TID: 59963136
Caught signal 20; TID: 59963136
Caught signal 6; TID: 59963136
Caught signal 15; TID: 59963136
Caught signal 2; TID: 85141248
Caught signal 2; TID: 76748544
Caught signal 3; TID: 85141248
Caught signal 20; TID: 85141248
Caught signal 6; TID: 85141248
Caught signal 15; TID: 85141248
.......

In the second case, all the signals sent by the parent thread are caught by the signal handler.  This is expected behavior has the signal handlers reinstall upon completion of the signal handler.  A unique behavior that is worth noting is that the the signal handler are not catched by their respective threads in order.  This is also expected because the thread that is responsible for handling the signal will only be able to handle the signal when it is in context of execution.

c)

In the third case, all signals sent by the parent thread are caught by the signal handler just as the second case.  The big difference between the second and third case is that the sa_mask parameter ensures that if SIGABRT and / or SIGSEGV is sent it is blocked when the sig handler is being executed.  However, dispite this change, no visible change existed in the STDOUT of the process.  This is due to a sleep between each individual signal made to the children threads.  This sleep is inserted to prevent a segfault when printing out diagnostic information in the signal handler.  In the absence of the sleep function, there would be a delay in the handling of SIGABRT and / or SIGSEGV.
