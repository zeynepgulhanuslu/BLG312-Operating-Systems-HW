//
// Created by Zeynep Gülhan Uslu
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <time.h>
#include "customer.h"


/**
 * This method for buying one product for one customer. When this function run on thread,
 * lock will secure shared resources update.
 * */
void order_product(struct Customer *customer, struct Product *product, int quantity, pthread_mutex_t *lock) {

    // Update the customer's ordered items list
    struct Order *new_order = malloc(sizeof(struct Order));
    new_order->product_ID = product->product_ID;
    new_order->quantity = quantity;
    new_order->next = customer->ordered_Items;
    customer->ordered_Items = new_order;

    // Lock the mutex
    pthread_mutex_lock(lock);

    if (quantity > MAX_PRODUCT_BUY) {
        printf("Customer%d(%d,%d) fail! Maximum purchase count is 5 for each product.\n",
               customer->customer_ID, product->product_ID, quantity);
    } else {
        if (quantity <= product->product_Quantity &&
            product->product_Price * quantity <= customer->customer_Balance) {

            // If the purchase is successful, update the customer balance and the product quantity
            customer->customer_Balance -= product->product_Price * quantity;

            // Add the product to the purchased items list
            struct Purchase *new_purchase = malloc(sizeof(struct Purchase));
            new_purchase->product_ID = product->product_ID;
            new_purchase->quantity = quantity;
            new_purchase->price = product->product_Price;
            new_purchase->next = customer->purchased_Items;
            customer->purchased_Items = new_purchase;

            // Update the customer's purchased count
            customer->purchased_count += 1;

            // Update the product quantity
            product->product_Quantity -= quantity;

            printf("Customer%d(%d,%d) success. Paid $%.2f for each.\n", customer->customer_ID,
                   product->product_ID, quantity, product->product_Price);

        } else {
            if (product->product_Quantity < quantity) {
                // when stock is not sufficient for a product, it will print this
                printf("Customer%d(%d,%d) fail! Only %d left in stock.\n",
                       customer->customer_ID,
                       product->product_ID,
                       quantity,
                       product->product_Quantity);
            } else {
                // If balance not sufficient print this.
                printf("Customer%d(%d,%d) fail! Insufficient funds : %.2f, %.2f\n",
                       customer->customer_ID,
                       product->product_ID,
                       quantity,
                       customer->customer_Balance,
                       (product->product_Price * quantity));
            }
        }
    }
    // print updated customer information
    print_customer_information(customer);
    // Unlock the mutex
    pthread_mutex_unlock(lock);
}

pthread_mutex_t lock_multi;

/**
 * This method for buying multiple products for one customer.
 * */
void order_products(struct Customer *customer, struct Product *products, int *quantities, int num_products) {

    // Lock the mutex
    pthread_mutex_lock(&lock_multi);

    int i = 0;
    printf("num products:%d\n", num_products);

    while (i < num_products) {
        struct Product *p = &products[i];
        order_product(customer, p, quantities[i], &lock_multi);
        i++;
    }
    // Unlock the mutex
    pthread_mutex_unlock(&lock_multi);
}

/**
 * Thread function for purchase_multiple_randomly.
 * Each thread purchases randomly selected multiple products with random quantities for a single customer.
 */
void *purchase_multiple_randomly_thread(void *arg) {
    purchase_multiple_randomly_thread_args_t *args = (purchase_multiple_randomly_thread_args_t *) arg;
    struct Customer *customer = &args->customers[args->customer_index];
    struct Product *products = args->products;
    pthread_mutex_t *lock = args->lock;
    int num_products = args->num_products;

    // Generate random number of products to purchase
    unsigned int seed = time(NULL) ^ customer->customer_ID;

    int random_product_index = rand_r(&seed) % num_products;
    int random_quantity = (rand_r(&seed) % MAX_PRODUCT_BUY) + 1;
    order_product(customer, &products[random_product_index], random_quantity, lock);


    return NULL;
}
// I will shuffle customer indices for randomly running customers
void shuffle_customer_indices(int *indices, int num_customers) {
    unsigned int seed = time(NULL);
    for (int i = num_customers - 1; i > 0; i--) {
        int j = rand_r(&seed) % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }
}

/**
 * This method is each customer can purchase randomly selected multiple products with random quantities.
 */
void purchase_multiple_randomly(struct Customer *customers, struct Product *products, pthread_mutex_t *lock,
                                int num_customers, int num_products) {
    pthread_t threads[num_customers];
    unsigned int seed = time(NULL);
    purchase_multiple_randomly_thread_args_t args[num_customers];
    int customer_indices[num_customers];
    for (int i = 0; i < num_customers; i++) {
        customer_indices[i] = i;
    }

    shuffle_customer_indices(customer_indices, num_customers);
    for (int i = 0; i < num_customers; i++) {
        args[i] = (purchase_multiple_randomly_thread_args_t) {
                .customers = customers,
                .products = products,
                .lock = lock,
                .customer_index = customer_indices[i],
                .num_products = num_products
        };
        pthread_create(&threads[i], NULL, purchase_multiple_randomly_thread, &args[i]);
    }

    for (int i = 0; i < num_customers; i++) {
        pthread_join(threads[i], NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_customers> : Number of customers. %s <num_products> : Number of products.\n",
               argv[1], argv[2]);
        return EXIT_FAILURE;
    }

    printf("Starting to simulate online shopping with multi threaded...\n");
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

    struct Customer customers[num_customers];
    struct Product products[num_products];
    pthread_mutex_t lock;
    // initialize mutex
    pthread_mutex_init(&lock, NULL);

    // initialize customers and products
    initialize_customers(customers, num_customers, num_products);
    initialize_products(products, num_products);

    printf("Initial state:\n");
    print_customers_information(customers, num_customers);
    print_products_information(products, num_products);
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    // run multi-thread version of random buy
    purchase_multiple_randomly(customers, products, &lock, num_customers, num_products);
    end = clock();


    printf("\nFinal state:\n");
    print_products_information(products, num_products);
    pthread_mutex_destroy(&lock);

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("CPU time used: %f seconds\n", cpu_time_used);
    return 0;
}
