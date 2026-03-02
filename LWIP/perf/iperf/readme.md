# Test1 

iperf -c 192.168.1.100 -w 200k -t 10s -i 1s
```
------------------------------------------------------------
Client connecting to 192.168.1.98, TCP port 5001
TCP window size:  200 KByte
------------------------------------------------------------
[  3] local 192.168.1.204 port 54958 connected with 192.168.1.98 port 5001
[ ID] Interval       Transfer     Bandwidth
[  3]  0.0- 1.0 sec  9.75 MBytes  81.8 Mbits/sec
[  3]  1.0- 2.0 sec  9.12 MBytes  76.5 Mbits/sec
[  3]  2.0- 3.0 sec  9.38 MBytes  78.6 Mbits/sec
[  3]  3.0- 4.0 sec  9.25 MBytes  77.6 Mbits/sec
[  3]  4.0- 5.0 sec  9.38 MBytes  78.6 Mbits/sec
[  3]  5.0- 6.0 sec  9.38 MBytes  78.6 Mbits/sec
[  3]  6.0- 7.0 sec  9.38 MBytes  78.6 Mbits/sec
[  3]  7.0- 8.0 sec  9.38 MBytes  78.6 Mbits/sec
[  3]  8.0- 9.0 sec  9.38 MBytes  78.6 Mbits/sec
[  3]  9.0-10.0 sec  9.25 MBytes  77.6 Mbits/sec
[  3]  0.0-10.0 sec  93.6 MBytes  78.5 Mbits/sec
```
# Test2 - Do a bidirectional test individually

iperf -c 192.168.1.100 -r
```
------------------------------------------------------------
Server listening on TCP port 5001
TCP window size:  208 KByte (default)
------------------------------------------------------------
------------------------------------------------------------
Client connecting to 192.168.1.98, TCP port 5001
TCP window size:  208 KByte (default)
------------------------------------------------------------
[  4] local 192.168.1.204 port 55030 connected with 192.168.1.98 port 5001
[ ID] Interval       Transfer     Bandwidth
[  4]  0.0-10.0 sec  88.6 MBytes  74.3 Mbits/sec
[  4] local 192.168.1.204 port 5001 connected with 192.168.1.98 port 52432
[  4]  0.0-10.0 sec  71.1 MBytes  59.6 Mbits/sec
```
# Test3 -  Do a bidirectional test simultaneously

iperf -c 192.168.1.100 -d
```
------------------------------------------------------------
Server listening on TCP port 5001
TCP window size:  208 KByte (default)
------------------------------------------------------------
------------------------------------------------------------
Client connecting to 192.168.1.98, TCP port 5001
TCP window size:  208 KByte (default)
------------------------------------------------------------
[  4] local 192.168.1.204 port 55122 connected with 192.168.1.98 port 5001
[  5] local 192.168.1.204 port 5001 connected with 192.168.1.98 port 52433
[ ID] Interval       Transfer     Bandwidth
[  4]  0.0-10.0 sec  71.0 MBytes  59.5 Mbits/sec
[  5]  0.0-10.0 sec  20.9 MBytes  17.5 Mbits/sec
```
