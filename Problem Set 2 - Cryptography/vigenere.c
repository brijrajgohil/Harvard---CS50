/* This is MY Implementation of Vigenere Cipher Thanks For Grading */
#include<stdio.h>
#include<cs50.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#define NUM_LETTERS 26 
int main(int argc, string argv[])
{
    if(argc!=2)
    {
        printf("Usage %s <keyword>\n",argv[0]);
        return 1;
    }
    string keyword = argv[1];
    int keyword_length = strlen(keyword);
    for(int i=0;i<keyword_length;i++)
    {
        if(!isalpha(keyword[i]))
        {
            printf("Keyword Can Only Contain Letters \n");
            return 1;
        }
    }
    string message;
    do
    {
        message = GetString();
    }while(message==NULL);
    int nls = 0;
    for(int i =0,n=strlen(message);i<=n;i++)
    {
        if(isalpha(message[i]))
        {
            int key;
            if(isupper(keyword[nls%keyword_length]))
            {
                key = keyword[nls%keyword_length]-'A';
            }
            else
            {
                 key = keyword[nls%keyword_length]-'a';
            }
            
            
            if(isupper(message[i]))
            {
                 message[i] = ((message[i] - 'A' + key) % NUM_LETTERS ) + 'A';
            }
            else
            {
                 message[i] = ((message[i] - 'a' + key) % NUM_LETTERS ) + 'a';
            }
            nls++;
        }
    }
    printf("%s\n",message);
    return 0;
}
