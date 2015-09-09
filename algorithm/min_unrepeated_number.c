//给定N是一个正整数，求比N大的最小“不重复数”，这里的不重复是指没有两个相等的相邻位，
//如1102中的11是相等的两个相邻位故不是不重复数，而12301是不重复数。
//基本的思想是从最高位往后扫，直到遇到两个相同的位，将较低的位上的数加1。要注意该位
//可能为9,这时我们就要向前进为，并记录进位的位置。如果不进位，则该位就是进为的位置。如
//112345：位置是第二位
//199234：位置就是第一位，因为会进位到第一位。
//然后，将该位置后面的所有数按01010101...排列就行了。我们可以证明，因为进位的那一位
//肯定是大于0的，因此这样的做法一定能得到不重复数，同时该不重复数是最小的。
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(void) {
    int N;
    int digits[20], num_digit = 0;
    scanf("%d", &N);
    while(N > 0) {
        digits[num_digit] = N % 10;
        N /= 10;
        num_digit++;
    }
    int first_repeat = num_digit-1;
    while(first_repeat > 0 && digits[first_repeat] != digits[first_repeat-1])
        first_repeat--;
    if(first_repeat) {
        int t = first_repeat-1;
        while(t < num_digit && digits[t] == 9) t++;
        if(t < num_digit) {
            digits[t]++;
        }
        else {
            digits[t] = 1;
            num_digit++;
        }
        t--;
        bool p = false;
        while(t >= 0) {
            digits[t] = p;
            p = !p;
            t--;
        }
        
    }

    int i;
    for(i = num_digit-1; i>= 0; i--) {
        N = N * 10 + digits[i];
    }
    printf("%d\n", N);

    return 0;
}
