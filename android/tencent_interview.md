1. Android中Looper的实现原理，为什么调用Looper.prepare()就在当前线程关联了一个Looper对象，它是如何实现的。  
    - Android Looper类内部有一个静态的ThreadLocal对象，当调用Looper.prepare()方法时，用ThreadLocal的get方法尝试获取该方法运行的
    线程的Looper对象，如果存在直接返回，如果不存在则使用sThreadLocal.put(new Looper());创建一个新的Looper对象。因此Looper使每个
    线程都有唯一的Looper对象。prepare方法的伪码如下所示：

            prepare() {
                Looper looper = sThreadLocal.get();
                if(looper == null) {
                    sThreadLocal.put(new Looper());
                }
                return sThreadLocal.get();
            }

    - Looper的loop()方法负责处理整个消息循环，loop()方法的伪码如下所示：

            loop() {
                Message msg = mMessageQueue().next;
                if(msg != null) {
                    if(msg.target == null) {
                        // 当Handler使用sendMessage，或者post(Runnable)都会将相应Message的target设置成该Handler对象，并将msg存入
                        // looper对象的MessageQueue中。当调用looper的stop方法时，looper会向MessageQueue中添加一个target为空的msg
                        // 对象。当loop方法判断到这个msg对象时就推出循环
                        return;
                    }
                    // 将Message交给发送其的Handler处理
                    target.dispatchMessage(msg);
                    msg.recycle();
                    // other code...
                }
                // other code...
            }

2. 简述Andriod如何处理UI与耗时操作的通信，有哪些方式及各自的优缺点（这种题目往往有时间多写点代码做例子吧）
    - 利用Java的Thread类，新开一个线程处理。 Thread类是Java早期为多线程而设计的，比较简单，但是不支持concurrent的许多特性。很多功能比如消息
    的传递等需要自己实现。下面的代码说明如何使用Thread

            // 继承Thread类，并实现其覆盖run方法。
            class MyThread extends Thread {
                // some member's here
                @Override
                public void run() {
                    // background task;
                }
            }

            // 使用Runnable对象, 一般推荐这种方法，因为Runnable是接口，可以继承其他类的同时实现Runnable接口
            Runnable r = new Runnable {
                @Override
                public void run() {

                }
            };
            new Thread(r).start();

    - 利用Handler和Looper线程配合：优点是结构清晰，功能定义明确。缺点是代码复杂，尤其是之进行一个任务时，代码想对于AsyncTask多很多。

            // 使用Android提供的HandlerThread方法可以轻松创建一个Looper线程
            HandlerThread mHandlerThread = new HandlerThread("workThread");
            mHandlerThread.start();

            Handler mWorkHandler = new Handler(mHandlerThread.getLooper());
            Handler mMainHandler = new Handler(getMainLooper());
            // 我们可以将后台任务作为Runnable对象，当需要执行后台任务时使用
            mWorkHandler.post(r);
            //在r中我们使用mMainHandler通知主线程更新UI等操作。

    - 使用AsyncTask：AsyncTask是Android提供的封装好的后台任务类，它比Handler更轻量级，使用起来也更为方便。同时AsyncTask使用了线程池机制，
    默认是5个线程。避免了频繁创建新线程的消耗。但是这个也是其缺点，当异步的任务大于5个时，使用AsyncTask会导致部分任务阻塞。AsyncTask提供了
    丰富的回调方法——onPreExecute(), doInbackground(), onPostExecute(), onProgessUpdate(),onCancelled().publishProgress()可以实现
    对后台任务的方便的控制。AsyncTask的缺点是在使用多个异步操作和并需要进行Ui变更时,就变得复杂起来.

            // AsyncTask是一个范型类，他有3个参数<param, Progress, Result>
            // Param是doInbackground接受的参数类型
            // Result是diebackground返回值类型
            // Progress是onProgressUpdate()方法的参数类型。在doInbackground中可以调用publishProgress()提醒工作进度。
            public class MyTask extends AsyncTask<Param, Progress, Result> {
                // members
                public MyTask(param) {

                }
                @Override
                void onPreExecute() {}
                @Override
                void onPostExecute() {}
                @Override
                Result doInbackground(Param ...) {}
                // other method...
            }

            MyTask myTask = new MyTask();
            myTask.execute(Param ...);

3. 用Object-C定义并实现一个基于数组的循环队列类，当队列放满需支持动态的扩展队列长度。  
使用C语言实现循环队列

        #define DEFAULT_DATA_LEN 100
        #define MAX(a, b) ((a) >= (b) ? (a) : (b))

        struct circled_queue {
            int *data;
            size_t start;
            size_t end;
            size_t data_len;
        }

        struct circled_queue *init_circled_queue(size_t len) {
            int data_len = MAX(DEFAULT_DATA_LEN, len);
            struct circled_queue *cq = (struct circled_queue *) malloc(sizeof(struct circled_queue));
            memset(cq, 0, sizeof(struct circled_queue));
            cq->data = (int *) malloc(sizeof(int) * data_len);
            memset(cq->data, 0, sizeof(int) * data_len);
            cq->data_len = data_len;
            return cq;
        }

        size_t cq_size(struct circled_queue *cq) {
            return cq->start <= cq->end ? (cq->end - cq->start) : (cq->data_len - (cq->start - cq->end));
        }

        void assure_capacity(struct circled_queue *cq) {
            size_t new_size = cq->data_len * 2 + 1;
            int *new_data = (int *) malloc(sizeof(int) * new_size);
            memset(new_data, 0, sizeof(int) * new_size);
            if(cq->end == cq->data_len-1) {
                memcpy(cp->data, new_data, cq->data_len-1);
            }
            else {
                int len1 = cp->data_len - cp->start;
                memcpy(cp->data + cp->start, new_data, len1);
                int len2 = cp->end;
                memcpy(cp->data, new_data+len1, len2);
            }
            free(cp->data);
            cp->data = new_data;
            cp->start = 0;
            cp->end = cp->data_len-1;
            cp->data_len = new_size;
        }

        void cq_push(struct circled_queue *cq, int x) {
            if(cp_size(cp) == cp->data_len-1) {
                assure_capacity(cp);
            }
            cp->data[cp->end] = x;
            cp->end++;
            if(cp->end == cp->data_len) cp->end = 0;
        }

        void cq_pop(struct circled_queue *cq) {
            if(cq_size(cq) <= 0) {
                return;
            }
            else {
                cq->start++;
                if(cq->start >= cq->data_len) cq->start = 0;
            }
        }

        void cq_free(struct circled_queue *cq) {
            free(cq->data);
            free(cq);
        }


4. typedef char *String_t; 和#define String_d char * 这两句在使用上有什么区别？  
```
typedef char *string_t说明用string_t来代表char *类型，是类型安全的而
define String_d char *是指用char *来替换String_d, 这不是类型安全的。  
如String_t a; 和 String_d a;都表示char *a;，而  
String_t a, b;和 String_d a, b;前者表示(char *) a, b;后者表示char *a, b;
```

5. 已知rand7()可以产生1~7的7个数(均匀概率),利用rand7() 产 生 rand10() 1~10(均匀概率)。
```
可以将rand7()rand7()看成一个7进制的数，代码如下

        void rand10() {
            int t = 40;
            while(t >= 40) {
                int t1 = rand7()-1;
                int t2 = rand7()-1;
                t = t1 * 7 + t2;
            }
            return t / 4 + 1;
        }
```
