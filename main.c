#include <stdlib.h>
#include <stdio.h>

#define MAX_PRODUCTS 2

typedef struct cart_t
{
    int products[MAX_PRODUCTS];
    int totalValue;
    void (*sumValues)(void *);
} Cart;

void sumValues(void *this)
{
    int tmpSum = 0;
    for (int i = 0; i < MAX_PRODUCTS; i++)
    {
        tmpSum += ((Cart *)this)->products[i];
    }
    ((Cart *)this)->totalValue = tmpSum;
}

Cart cartConstructor(void)
{
    Cart cart;
    cart.sumValues = &sumValues;
    return cart;
}

void main()
{
    Cart cart = cartConstructor();
    cart.products[0] = 10;
    cart.products[1] = 25;
    cart.sumValues(&cart);
    printf("total value: %d\n", cart.totalValue);
}