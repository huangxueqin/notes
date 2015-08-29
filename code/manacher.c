#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

int manacher(const char *s, const int offset, const int len) {
    char *str = (char *) malloc(len * 2 + 1);
    // pre process
    for(int i = 0; i < len; i++) {
        str[2*i] = '#';
        str[2*i+1] = s[offset+i];
    }
    str[len*2] = '#';

    int *array = (int *) malloc(sizeof(int) * (len * 2 + 1));
    memset(array, 0, sizeof(int) * (len * 2 + 1));
    // core algorithm
    int mid = -1, maxi = -1;
    for(int i = 0; i <= 2 * len; i++) {
        int h = 0;
        if(i <= maxi) {
            h = min(array[mid*2-i], maxi-i);
        }
        while(i-h-1 >= 0 && i+h+1 <= 2*len && str[i-h-1] == str[i+h+1]) h++;
        if(i+h > maxi) {
            mid = i;
            maxi = i+h;
        }
        array[i] = h;
    }

    int result = 1;
    for(int i = 0; i <= 2*len; i++) {
        if(array[i] > result) {
            result = array[i];
        }
    }
    free(str);
    free(array);
    return result;
}


int main(void) {
    char *s = "aaaaaaaaa";
    printf("largest palindramic substring is %d\n", manacher(s, 0, strlen(s)));
    return 0;
}
