#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <execinfo.h>
#include <unistd.h>
#include "analyze-stack.h"

#define MAX_PRODUCTS 2
#define MAX_LOG_MESSAGE 20
#define BUFFER_STACK_SIZE 1000
#define READ 0
#define WRITE 1
#define FUNCTIONS_QTT 2

int pipe_fd[2];

int validateCall()
{
    char buffer[BUFFER_STACK_SIZE];
    alarm(1);
    pause();
    read(pipe_fd[READ], buffer, BUFFER_STACK_SIZE);
    char *functionIsValid = NULL;
    char validFunctions[FUNCTIONS_QTT][BUFFER_STACK_SIZE] = {"sumValues", "applyDiscount"};
    for (int i = 0; i < FUNCTIONS_QTT; i++)
    {
        functionIsValid = strstr(buffer, validFunctions[i]);
        if (functionIsValid)
            return 1;
    }
    return 0;
}

/////////////////// ENCAPSULATED VARIABLE ///////////////////
float totalValue;
float getTotalValue()
{
    return totalValue;
}
void setTotalValue(float newTotalValue)
{
    if (validateCall())
        totalValue = newTotalValue;
}
/////////////////// ENCAPSULATED VARIABLE ///////////////////

typedef struct cart_t
{
    float products[MAX_PRODUCTS];
    float (*getTotalValue)(void);
    void (*sumValues)(void *);
} Cart;

void sumValues(void *this)
{
    float tmpSum = 0;
    for (int i = 0; i < MAX_PRODUCTS; i++)
    {
        tmpSum += ((Cart *)this)->products[i];
    }
    setTotalValue(tmpSum);
}

Cart cartConstructor(void)
{
    Cart cart;
    cart.sumValues = &sumValues;
    cart.products[0] = 0;
    cart.products[1] = 0;
    cart.getTotalValue = &getTotalValue;
    return cart;
}

struct ILogger
{
    void (*log)(char text[MAX_LOG_MESSAGE], float);
};

typedef struct vip_cart_t
{
    Cart super;
    void (*sumValues)(void *, struct ILogger);
    float discount;
} VipCart;

void applyDiscount(void *this, struct ILogger logger)
{
    float oldValue = ((VipCart *)this)->super.getTotalValue();
    float discount = ((VipCart *)this)->discount;
    float newValue = oldValue * (1 - discount);
    setTotalValue(newValue);
    char logMessage[MAX_LOG_MESSAGE] = "discount applied";
    logger.log(logMessage, discount);
}

void executeApplyDiscount(void *this, struct ILogger logger)
{
    // validation by stack
    applyDiscount(this, logger);
}

void sumValuesWithDiscount(void *this, struct ILogger logger)
{
    sumValues(this);
    executeApplyDiscount(this, logger);
}

VipCart vipCartConstructor()
{
    VipCart vipCart;
    vipCart.super = cartConstructor();
    vipCart.sumValues = &sumValuesWithDiscount;
    vipCart.discount = 0;
    return vipCart;
}

void polymorphicSumValues(void *this)
{
    // add one more unit per product
    float tmpSum = 0;
    for (int i = 0; i < MAX_PRODUCTS; i++)
    {
        tmpSum += ((Cart *)this)->products[i] + 1;
    }
    setTotalValue(tmpSum);
}

VipCart polymorphicVipCartConstructor()
{
    VipCart vipCart;
    vipCart.super = cartConstructor();
    vipCart.sumValues = &polymorphicSumValues;
    vipCart.discount = 0;
    return vipCart;
}

void log1(char text[MAX_LOG_MESSAGE], float discount)
{
    printf("log 1 --> %s - value: %f\n", text, discount);
}

struct ILogger loggerConstructor1()
{
    struct ILogger logger;
    logger.log = &log1;
    return logger;
}

void log2(char text[MAX_LOG_MESSAGE], float discount)
{
    printf("log 2 --> %s - value: %f\n", text, discount);
}

struct ILogger loggerConstructor2()
{
    struct ILogger logger;
    logger.log = &log2;
    return logger;
}

void main(int argc, char *argv[])
{
    pipe(pipe_fd);
    set_user_fd(pipe_fd[WRITE]);
    analyze_stack(argv[0]);

    // Cart cart = cartConstructor();
    // cart.products[0] = 10;
    // cart.products[1] = 25;
    // cart.sumValues(&cart);
    // printf("cart - total value: %f\n", cart.totalValue);

    setTotalValue(-1);
    printf("total value (-1): %f\n", totalValue);
    struct ILogger logger1 = loggerConstructor1();
    VipCart vipCart = vipCartConstructor();
    vipCart.super.products[0] = 10;
    vipCart.super.products[1] = 40;
    vipCart.discount = 0.2;
    vipCart.sumValues(&vipCart, logger1);
    printf("vip cart - total value (40): %f\n", vipCart.super.getTotalValue());
    printf("total value (40): %f\n", totalValue);
    setTotalValue(0);
    printf("total value (0): %f\n", totalValue);

    // struct ILogger logger2 = loggerConstructor2();
    // VipCart polyVipCart = polymorphicVipCartConstructor();
    // polyVipCart.super.products[0] = 10;
    // polyVipCart.super.products[1] = 40;
    // polyVipCart.sumValues(&polyVipCart);
    // polyVipCart.discount = 0.5;
    // polyVipCart.applyDiscount(&polyVipCart, logger2);
    // printf("poly vip cart - total value: %f\n", polyVipCart.super.totalValue);

    close(pipe_fd[READ]);
    close(pipe_fd[WRITE]);
}