# Abxhttpd测试文档

我们采用ApacheBench进行标准压力测试

注意：对静态页面的测试不能说明大部分问题，但可以告诉我们瓶颈在哪里

测试环境：

| 操作系统 | Linux(Ubuntu 22.04.1)                    |
| -------- | ---------------------------------------- |
| CPU      | Intel(R) Core(TM) i7-9750H CPU @ 2.60GHz |
| 内存     | 16GB                                     |

测试版本：abxhttpd/2.1.1  (启用O2级别优化, GCC9.4.0编译)

运行命令：abxhttpd -p 9998 -D /var/www/html -l logs/access.log -e logs/error.log -d

(后台运行)

## 小并发测试

测试命令：（并发量5，测试10000次）

```
ab -n 10000 -c 5 127.0.0.1:9998/
```

测试结果

```shell
This is ApacheBench, Version 2.3 <$Revision: 1879490 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 1000 requests
Completed 2000 requests
Completed 3000 requests
Completed 4000 requests
Completed 5000 requests
Completed 6000 requests
Completed 7000 requests
Completed 8000 requests
Completed 9000 requests
Completed 10000 requests
Finished 10000 requests


Server Software:        abxhttpd/2.1.1
Server Hostname:        127.0.0.1
Server Port:            9998

Document Path:          /
Document Length:        10671 bytes

Concurrency Level:      5
Time taken for tests:   0.640 seconds
Complete requests:      10000
Failed requests:        0
Total transferred:      108560000 bytes
HTML transferred:       106710000 bytes
Requests per second:    15633.72 [#/sec] (mean)
Time per request:       0.320 [ms] (mean)
Time per request:       0.064 [ms] (mean, across all concurrent requests)
Transfer rate:          165741.87 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.0      0       0
Processing:     0    0   0.1      0       1
Waiting:        0    0   0.0      0       0
Total:          0    0   0.1      0       1

Percentage of the requests served within a certain time (ms)
  50%      0
  66%      0
  75%      0
  80%      0
  90%      0
  95%      0
  98%      1
  99%      1
 100%      1 (longest request)
```

## 普通并发测试

测试命令2：（并发量150，测试100000次）

```shell
ab -n 100000 -c 150 127.0.0.1:9998/
```

测试结果

```shell
This is ApacheBench, Version 2.3 <$Revision: 1879490 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 10000 requests
Completed 20000 requests
Completed 30000 requests
Completed 40000 requests
Completed 50000 requests
Completed 60000 requests
Completed 70000 requests
Completed 80000 requests
Completed 90000 requests
Completed 100000 requests
Finished 100000 requests


Server Software:        abxhttpd/2.1.1
Server Hostname:        127.0.0.1
Server Port:            9998

Document Path:          /
Document Length:        10671 bytes

Concurrency Level:      150
Time taken for tests:   5.882 seconds
Complete requests:      100000
Failed requests:        0
Total transferred:      1085600000 bytes
HTML transferred:       1067100000 bytes
Requests per second:    17001.37 [#/sec] (mean)
Time per request:       8.823 [ms] (mean)
Time per request:       0.059 [ms] (mean, across all concurrent requests)
Transfer rate:          180241.08 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    0   0.1      0       3
Processing:     2    9   0.4      9      15
Waiting:        0    9   0.5      9      15
Total:          4    9   0.4      9      15

Percentage of the requests served within a certain time (ms)
  50%      9
  66%      9
  75%      9
  80%      9
  90%      9
  95%      9
  98%     10
  99%     10
 100%     15 (longest request)
```
