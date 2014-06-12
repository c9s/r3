/*
 * ip.c
 * Copyright (C) 2014 c9s <c9s@c9smba.local>
 *
 * Distributed under terms of the MIT license.
 */


// #include "ip.h"
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

int r3_ip_cmp(const char* a, const char* b) { 
    long al = inet_addr(a);
    long bl = inet_addr(b);
    return al == bl;
}

int r3_ip_mask_cmp(const char *a, const char* b, const char* mask_str) { 
    long m = inet_addr(mask_str);
    long al = inet_addr(a);
    long bl = inet_addr(b);
    return !((al ^ bl) & m);
}


/*
    long in;
    in = inet_addr("192.168.1.1");
    printf("My unreadable addres is %ld\n", in);

    in = inet_addr("8.8.8.8");
    printf("My unreadable addres is %ld\n", in);
    printf("My unreadable addres is %u %d\n", inet_addr("255.255.255.255"), 2 << 16 );
    printf("My unreadable addres is %u %d\n", inet_addr("255.0.0.0") , (2 << 7) - 1 );
    printf("My unreadable addres is %u %d\n", inet_addr("255.255.0.0") , (2 << 15) - 1 );
    printf("My unreadable addres is %u %d\n", inet_addr("255.255.255.0") , (2 << 23) - 1 );


    printf("My unreadable addres is %u\n", inet_addr("255.255.255.0") & inet_addr("192.168.0.1") );

    struct in_addr in2;
    in2.s_addr = inet_addr("192.168.0.1");

    char *dot_ip = inet_ntoa(in2);
    printf("%s\n", dot_ip);
*/
