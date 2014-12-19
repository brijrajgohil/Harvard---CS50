/*This is my Implementation of Caesar Cipher Thanks For Grading */
#include<stdio.h>
#include<cs50.h>
#include<string.h>
#include<ctype.h>
#include<stdlib.h>
#define NUM_LETTERS 26
int main(int argc, string argv[])
{
    if(argc!=2)
    {
        printf("Usage %s <key>\n",argv[0]);
        return 1;
    }
    int key = atoi((argv[1]));
    if(key<0)
    {
        printf("Key can't be A Negative integer \n ");
        return 1;
    }
    string message;
    do
    {
        message = GetString();
    }while(message==NULL);
    for(int i=0,n=strlen(message);i<=n;i++)
    {
        if(isupper(message[i]))
        {
            message[i] = ((message[i] - 'A' + key) % NUM_LETTERS ) + 'A';
        }
        else if(islower(message[i]))
        {
            message[i] = ((message[i] - 'a' + key) % NUM_LETTERS ) + 'a';
        }
    }
    printf("%s\n",message);
    return 0;
    
}
