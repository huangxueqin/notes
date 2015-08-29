We assume a string named "s" width length "len".
There are 2 steps to implement KMP string matching algorithm:  
**Step 1: compute next array**  
        Assume the next array is pi[]. The using of next array is to find next matching point
    in constant time. For example, if we have successfully matched m(m < len) characters at i,
    and failed at (m+1)th character. Then we should find the longest leading substring with
    length p(p < m), has s[0:p] == s[m-p:m]. When such p is found, we continue to compare s[p+1]
    and s[i]. Below is how to compute next array in linear time.

        void get_next_array(const char *s, const int len, int *next) {
            next[0] = -1;    // we make next[0] = -1
            int k = -1;
            for(int i = 1; i < len; i++) {
                while(k >= 0 && s[k+1] != s[i]) k = next[k];
                if(s[k+1] == s[i]) k += 1;
                next[i] = k;
            }
        }

**Step 2: using computed next array we can perform string matching in linear time.**  

        void kmp_match(const char *src, int src_len, const char *pat, int pat_len) {
            int *next = (int *) malloc(sizeof(int) * pat_len);
            get_next_array(pat, pat_len, next);

            int k = -1;
            for(int i = 0; i < src_len; i++) {
                while(k >= 0 && pat[k+1] != src[i]) k = next[k];
                if(pat[k+1] == src[i]) k += 1;
                if(k == pat_len) {
                    printf("%d\n", i-pat_len+1);
                    k = next[k-1];
                }
            }
            free(next);
        }
