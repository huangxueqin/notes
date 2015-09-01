###计算所有的Permutation的算法
1. 将所有元素按从小到大的顺序排序
2. 从最右边开始搜索，直到发现有A[i-1] < A[i],记下i-1作为leftmost
3. 从最右边开始搜索，知道有A[i] > A[leftmost]。交换A[i]和A[leftmost]
4. 将leftmost右边的序列到过来变成升序
####实现代码
        #include <stdlib.h>

        int comp(void *lhs, void *rhs) {
            return *((int *)lhs) < *((int *)rhs);
        }

        void swap(int A[], int i, int j) {
            int t = A[i];
            A[i] = A[j];
            A[j] = t;
        }

        void print_array(int A[], int n) {
            for(int i = 0; i < n; i++) {
                printf("%d", A[i]);
            }
            printf("\n");
        }

        void reverse(int A[], int i, int j) {
            while(i < j) {
                swap(A, i, j);
                i++;
                j--;
            }
        }

        void print_permutation(int A[], int n) {
            qsort(A, n, sizeof(int), comp);
            while(true) {
                int i = n-1;
                while(i > 0 && A[i] <= A[i-1]) i--;
                if(i == 0) break;
                int leftmost = i-1;
                i = n-1;
                while(i > leftmost && A[i] <= A[leftmost]) i--;
                swap(A, leftmost, i);
                reverse(A, leftmost+1, n-1);
                print_array(A, n);
            }
        }
