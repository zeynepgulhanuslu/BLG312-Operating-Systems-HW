# Operating Systems Homework 2

This homework is about online shopping routine application with multiprocess and multihreaded way.
It includes semaphores, lock and mutexes.
## Compiling Code
Makefile written for this code. So you can compile it like below:
```bash
cd ./hw2
make
```

## Usage

This homework contains 2 part. 

### Question 1
First part is creating this routine with multi-threaded way. 
It will select multiple products randomly for each customers and run in threads.
Each customer has one thread.


You can run multi_thread with this command: 


```bash

Usage: ./multi_thread Usage: <num_customers> : Number of customers. <num_products> : Number of products.

./multi_thread 40 50
```

### Question 2

Second part will create the same app with multi-process way.
You can run multi_process with this command:


```bash

Usage: ./multi_process  Usage: <num_customers> : Number of customers. <num_products> : Number of products.


./multi_process 30 40
```