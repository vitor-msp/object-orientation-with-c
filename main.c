#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_PRODUCTS 2
#define MAX_LOG_MESSAGE 20

float totalValue;

float getTotalValue()
{
    return totalValue;
}

void setTotalValue(float newTotalValue)
{
    // validation by process stack
    totalValue = newTotalValue;
}

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
    void (*sumValues)(void *);
    float discount;
    void (*applyDiscount)(void *, struct ILogger);
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

VipCart vipCartConstructor()
{
    VipCart vipCart;
    vipCart.super = cartConstructor();
    vipCart.sumValues = &sumValues;
    vipCart.discount = 0;
    vipCart.applyDiscount = &applyDiscount;
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
    vipCart.applyDiscount = &applyDiscount;
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

void main()
{
    // Cart cart = cartConstructor();
    // cart.products[0] = 10;
    // cart.products[1] = 25;
    // cart.sumValues(&cart);
    // printf("cart - total value: %f\n", cart.totalValue);

    struct ILogger logger1 = loggerConstructor1();
    VipCart vipCart = vipCartConstructor();
    vipCart.super.products[0] = 10;
    vipCart.super.products[1] = 40;
    vipCart.sumValues(&vipCart);
    vipCart.discount = 0.2;
    vipCart.applyDiscount(&vipCart, logger1);
    printf("vip cart - total value: %f\n", vipCart.super.getTotalValue());

    // struct ILogger logger2 = loggerConstructor2();
    // VipCart polyVipCart = polymorphicVipCartConstructor();
    // polyVipCart.super.products[0] = 10;
    // polyVipCart.super.products[1] = 40;
    // polyVipCart.sumValues(&polyVipCart);
    // polyVipCart.discount = 0.5;
    // polyVipCart.applyDiscount(&polyVipCart, logger2);
    // printf("poly vip cart - total value: %f\n", polyVipCart.super.totalValue);
}