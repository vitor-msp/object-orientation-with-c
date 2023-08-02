#include <stdlib.h>
#include <stdio.h>

#define MAX_PRODUCTS 2

typedef struct cart_t
{
    float products[MAX_PRODUCTS];
    float totalValue;
    void (*sumValues)(void *);
} Cart;

void sumValues(void *this)
{
    float tmpSum = 0;
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
    cart.products[0] = 0;
    cart.products[1] = 0;
    cart.totalValue = 0;
    return cart;
}

typedef struct vip_cart_t
{
    Cart super;
    float discount;
    void (*applyDiscount)(void *);
} VipCart;

void applyDiscount(void *this)
{
    float oldValue = ((VipCart *)this)->super.totalValue;
    float discount = ((VipCart *)this)->discount;
    float newValue = oldValue * (1 - discount);
    ((VipCart *)this)->super.totalValue = newValue;
}

VipCart vipCartConstructor()
{
    VipCart vipCart;
    vipCart.super = cartConstructor();
    vipCart.discount = 0;
    vipCart.applyDiscount = &applyDiscount;
    return vipCart;
}

void main()
{
    Cart cart = cartConstructor();
    cart.products[0] = 10;
    cart.products[1] = 25;
    cart.sumValues(&cart);
    printf("cart - total value: %f\n", cart.totalValue);

    VipCart vipCart = vipCartConstructor();
    vipCart.super.products[0] = 10;
    vipCart.super.products[1] = 40;
    vipCart.super.sumValues(&vipCart);
    vipCart.discount = 0.2;
    vipCart.applyDiscount(&vipCart);
    printf("vip cart - total value: %f\n", vipCart.super.totalValue);
}