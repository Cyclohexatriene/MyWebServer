#!/bin/bash

g++ main.cpp ./http/http.cpp ./lock/locker.cpp ./log/log.cpp ./mysql/mysql_conn_pool.cpp ./threadpool/threadpool.cpp -o main.o -lpthread -lmysqlclient -g
