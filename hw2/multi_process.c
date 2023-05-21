//
// Created by Zeynep Gülhan Uslu
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/mman.h>
#include "customer.h"

sem_t order_sem;    // use semaphore for shared memory
int completed_orders;

void order_product(struct Customer *customer, struct Product *product, int quantity, pthread_mutex_t *lock) {
    // Wait for the order semaphore
    /**This line waits for the order_sem semaphore to be available.
     * This ensures that only a limited number of customers can place an order at the same time.
     * */
    sem_wait(&order_sem);

    // Update the customer's ordered items list
    // This line allocates memory for a new order and assigns it to the new_order pointer.
    struct Order *new_order = malloc(sizeof(struct Order));
    new_order->product_ID = product->product_ID;
    new_order->quantity = quantity;
    new_order->next = customer->ordered_Items;
    customer->ordered_Items = new_order;

    // Lock the mutex
    pthread_mutex_lock(lock);

    // Check if the product can be purchased
    if (quantity > MAX_PRODUCT_BUY) {
        // when maximum quantity reached for a product, it will print this
        printf("Customer%d(%d,%d) fail! Maximum purchase count is 5 for each product.\n",
               customer->customer_ID, product->product_ID, quantity);
    } else if (quantity > product->product_Quantity) {
        // when stock is not sufficient for a product, it will print this
        printf("Customer%d(%d,%d) fail! Only %d left in stock.\n",
               customer->customer_ID,
               product->product_ID,
               quantity,
               product->product_Quantity);
    } else if (product->product_Price * quantity > customer->customer_Balance) {
        // If balance not sufficient print this.
        printf("Customer%d(%d,%d) fail! Insufficient funds : %.2f, %.2f\n",
               customer->customer_ID,
               product->product_ID,
               quantity,
               customer->customer_Balance,
               (product->product_Price * quantity));
    } else {
        // If the purchase is successful, update the customer balance and the product quantity
        customer->customer_Balance -= product->product_Price * quantity;
        product->product_Quantity -= quantity;

        // Add the product to the purchased items list
        struct Purchase *new_purchase = malloc(sizeof(struct Purchase));
        new_purchase->product_ID = product->product_ID;
        new_purchase->quantity = quantity;
        new_purchase->price = product->product_Price;
        new_purchase->next = customer->purchased_Items;
        customer->purchased_Items = new_purchase;

        // Update the customer's purchased count
        customer->purchased_count += 1;

        printf("Customer%d(%d,%d) success. Paid $%.2f for each.\n", customer->customer_ID,
               product->product_ID, quantity, product->product_Price);
        (completed_orders)++;
    }

    print_customer_information(customer);
    // Unlock the mutex
    pthread_mutex_unlock(lock);

    // Signal that the order is finished
    sem_post(&order_sem);

}
/**
 * One customer will buy multiple products
 * */
void order_products(struct Customer *customer, struct Product *products, int num_products, pthread_mutex_t *lock) {
    for (int i = 0; i < num_products; i++) {
        // Wait for the order semaphore
        sem_wait(&order_sem);

        // Update the customer's ordered items list
        struct Order *new_order = malloc(sizeof(struct Order));
        new_order->product_ID = products[i].product_ID;
        new_order->quantity = 1; // purchase one of each product
        new_order->next = customer->ordered_Items;
        customer->ordered_Items = new_order;

        // Lock the mutex
        pthread_mutex_lock(lock);

        // Check if the product can be purchased
        if (products[i].product_Quantity < 1) {
            printf("Customer%d(%d) fail! Out of stock.\n", customer->customer_ID, products[i].product_ID);
        } else if (products[i].product_Price > customer->customer_Balance) {
            printf("Customer%d(%d) fail! Insufficient funds : %.2f, %.2f\n",
                   customer->customer_ID,
                   products[i].product_ID,
                   customer->customer_Balance,
                   products[i].product_Price);
        } else {
            // If the purchase is successful, update the customer balance and the product quantity
            customer->customer_Balance -= products[i].product_Price;
            products[i].product_Quantity -= 1;

            // Add the product to the purchased items list
            struct Purchase *new_purchase = malloc(sizeof(struct Purchase));
            new_purchase->product_ID = products[i].product_ID;
            new_purchase->quantity = 1;
            new_purchase->price = products[i].product_Price;
            new_purchase->next = customer->purchased_Items;
            customer->purchased_Items = new_purchase;

            // Update the customer's purchased count
            customer->purchased_count += 1;

            printf("Customer%d(%d) success. Paid $%.2f.\n", customer->customer_ID,
                   products[i].product_ID, products[i].product_Price);
            (completed_orders)++;
        }

        print_customer_information(customer);
        // Unlock the mutex
        pthread_mutex_unlock(lock);

        // Signal that the order is finished
        sem_post(&order_sem);
    }
}

void *create_shared_memory(size_t size) {
    // Set protection and visibility flags for the shared memory object
    // This line sets the protection flags for the shared memory object to allow both reading and writing.
    int protection = PROT_READ | PROT_WRITE;
    // This line sets the visibility flags for the shared memory object to shared memory and anonymous memory.
    int visibility = MAP_SHARED | MAP_ANON;

    // Memory-map the shared memory object
    void *addr = mmap(NULL, size, protection, visibility, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    return addr;
}

/**
 * This method is for one customer buys randomly selected product with maximum quantity.
 * */
void purchase_randomly(struct Customer *customers, struct Product *products,
                       pthread_mutex_t *lock, int num_customers, int num_products) {
    for (int i = 0; i < num_customers; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            srand(time(NULL) ^ getpid()); // Seed the random generator for each child process

            int random_product_index = rand() % num_products;
            int random_quantity = (rand() % MAX_PRODUCT_BUY) + 1;
            // order product
            order_product(&customers[i], &products[random_product_index],
                          random_quantity, lock);

            exit(0);
        } else if (pid < 0) {
            // process not created successfully
            perror("fork");
            exit(1);
        }
    }

    // wait all process finish
    for (int i = 0; i < num_customers; i++) {
        wait(NULL);
    }
}

/**
 * This method is each customer can purchase randomly selected multiple products with random quantities.
 * */
void purchase_multiple_randomly(struct Customer *customers, struct Product *products, pthread_mutex_t *lock,
                                int num_customers, int num_products) {
    for (int i = 0; i < num_customers; i++) {
        pid_t pid = fork();
        unsigned int seed = time(NULL) ^ customers[i].customer_ID;
        if (pid == 0) {
            // Child process
            srand(time(NULL) ^ getpid()); // Seed the random generator for each child process
            int num_products_to_purchase = 1 + rand_r(&seed) % (MAX_ITEM_QUANTITY - 1 + 1);

            // Create an array to store indices of the randomly selected products
            int *random_product_indices = malloc(sizeof(int) * num_products_to_purchase);
            for (int j = 0; j < num_products_to_purchase; j++) {
                int random_product_index =  rand_r(&seed) % num_products;
                random_product_indices[j] = random_product_index;
            }

            // Purchase the randomly selected products
            for (int j = 0; j < num_products_to_purchase; j++) {
                int random_quantity = ( rand_r(&seed) % MAX_PRODUCT_BUY) + 1;
                order_product(&customers[i], &products[random_product_indices[j]],
                              random_quantity, lock);
            }
            free(random_product_indices);
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        }
    }

    // Parent process
    for (int i = 0; i < num_customers; i++) {
        wait(NULL);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_customers> : Number of customers. %s <num_products> : Number of products.\n",
               argv[1], argv[2]);
        return EXIT_FAILURE;
    }

    printf("Starting to simulate online shopping...\n");
    const int num_customers = atoi(argv[1]);
    const int num_products = atoi(argv[2]);
    printf("Number of customers: %d\n", num_customers);
    printf("Number of products: %d\n", num_products);
    if (num_customers < MIN_CUSTOMERS) {
        printf("Minimum customer count should be= %d\n", MIN_CUSTOMERS);
        return EXIT_FAILURE;
    }

    if (num_products < MIN_PRODUCTS) {
        printf("Minimum product count should be= %d\n", MIN_PRODUCTS);
        return EXIT_FAILURE;
    }
    // create shared memory for initial customers and products arrays.
    struct Customer *customers = (struct Customer *) create_shared_memory(sizeof(struct Customer) * num_customers);
    struct Product *products = (struct Product *) create_shared_memory(sizeof(struct Product) * num_products);

    pthread_mutex_t *lock = (pthread_mutex_t *) create_shared_memory(sizeof(pthread_mutex_t));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(lock, &attr);


    sem_init(&order_sem, 1, 1);
    completed_orders = 0;

    // initialize customers and products arrays.
    initialize_customers(customers, num_customers, num_products);
    initialize_products(products, num_products);
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    printf("Initial state:\n");
    print_customers_information(customers, num_customers);
    print_products_information(products, num_products);

    purchase_randomly(customers, products, lock, num_customers, num_products);

    printf("\nFinal state:\n");
    print_products_information(products, num_products);

    // Finally, it destroys the mutex lock and the semaphore, and frees the shared memory using the munmap function.
    pthread_mutex_destroy(lock);
    pthread_mutexattr_destroy(&attr);
    sem_destroy(&order_sem);
    munmap(customers, sizeof(struct Customer) * num_customers);
    munmap(products, sizeof(struct Product) * num_products);
    munmap(lock, sizeof(pthread_mutex_t));
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("CPU time used: %f seconds\n", cpu_time_used);
    return 0;
}
