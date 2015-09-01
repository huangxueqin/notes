1. Android应用程序框架  
    1. 应用程序层
    2. 应用程序框架层——view，content providers，resource manager activity manager，notification manager。。。
    3. 系统运行库层——系统库：SQLite，SurfaceManager，媒体，libc，Web; Android运行库：虚拟机等
    4. Linux内核
2. 如果后台的Activity由于某原因被系统回收了，如何在被系统回收之前保存当前状态？  
用`onSaveInstanceState(Bundle outState)`方法保存目前状态，如果Activity被杀死了，那么重新启动该Activity时会调用`onCreate(Bundle savedInstance)`，这时savedInstance参数不为null，而是之前保存的outState
3. ContentProvider相关
    1. 读取ContentProvider的内容使用：  

            Cursor cursor = getContentResolver().query(Uri, ...)
    2. Uri：
    ```
    scheme     ——content://
    Authority  ——用户定义，并在Android Manifest里面注册
    path           ——使用UriMatcher进行匹配说明
    ```
    3. 检测ContentProvider数据库内容的变化：  
    使用ContentObserver，在Activity中使用`getContentResolver().registerContentObserver(Uri, ..,cob)`注册一个ContentObserver。
    然后在insert，delete，update等改变数据的方法执行末尾调用`getContext().getContentResolver().notifyChange(Uri, ..)` 方法通知发生变动
4. BroadcastReceiver相关
    1. 注册方式
    非常驻广播注册方式——程序中注册

            sdcardStateReceiver = new SdcardStateChanageReceiver();
            IntentFilter filter = new IntentFilter();
            filter.addAction(Intent.ACTION_MEDIA_REMOVED);
            filter.addAction(Intent.ACTION_MEDIA_EJECT);
            filter.addAction(Intent.ACTION_MEDIA_MOUNTED);
            filter.addDataScheme("file");  
            registerReceiver(sdcardStateReceiver, filter);  

    常驻型广播注册方式——Manifest中注册
    2. 生命周期
    当执行`onReceive(Context context, Intent，data)`时是活跃的，活跃的BroadcastReceive进程是收系统保护的，无法被杀死。当退出onReceive方法时
    是不活跃的。onReceive默认在MainThread中执行，当执行较繁重的任务时，最好新起一个线程。如果担心这个线程会被杀死，则最好新起一个serveice执行该任务。
5. Service相关
    1. Service的启动方式
        - startService：这种方法启动的Service一旦启动了，就和启动该Service的Component脱离关系了，唯一可以传递参数的地方就是在startService的Intent
        参数中携带。Service启动后会调用onStartCommand方法，在这个方法中可以获得启动Component传递的参数。用这种方法启动的Service有两种结束的方法。一
        种是在启动Component中调用`stopService`，另一种是在Service中调用`stopSelf`方法。
        - BoundService：这种方法是在启动Component中使用`bindService`方法，这种方法启动的Service，可以和Component通过`Binder`通信。同时这个Service的
        生命周期也和启动Component相关。当所有绑定了该Service的Component都解除绑定或者退出了，该Service也会随之结束。
    2. bind Service的几种方法
        * **继承`Binder class`**：在Service的onBind中返回实例。在Activity的`ServiceConnection`对象的回调函数中接受该实例，然后通过该实例执行相应操作，或通
        过该实例得到Service对象。继承binder方法只能使用在Activity和service属于同一Application的情况  
        * **使用Messenger**：在Service中包含一个Handler，用该Handler实例化一个Messenger对象`final Messenger mMessenger = new Messenger(mHandler);`
        在Service的onBind方法中，返回`mMessenger.getBinder()`，然后在Activity中的ServiceConnection回调方法中会接受到该Binder对象。通过这个对象可以初始化
        发送端的Messenger对象`mMessenger = new Messenger(binder)`。当要发送消息给service时，使用`mMessenger.send(msg)`即可。如果需要service回送消息执行
        结果，可以发送端也使用Handler新建一个Messenger对象mCallback，然后把上面的msg的`replyTo`设为该mCallback。在Service中，当处理msg完毕后，可以通过msg的
        replyTo传递结果给Activity，Messenger方法可以用于跨进程IPC。但是要注意的是，该方法只能顺序处理各个msg，若要多线程处理消息，则使用AIDL。
        * **使用AIDL**：AIDL中可用的数据类型——primitive type in Java， String， CharSequence， List， Map。 两个Application都要实现相同的.aidl文件，client端使用asInterface（bind）方法将service传入的接口转化为本地定义的相同结构，然后操作该接口
6. Handler, Looper, MessageQueue, Thread相关
    1. 主线程启动时已经建立了主线程对应的Looper，而自定义的新线程并没有。若要定义属于自定义线程的Looper，应该在run方法里用`Looper.prepare()`方法创建一个Looper对象，
    然后用Looper.loop()方法执行消息循环，例如Android ThreadHandler的run方法是这样实现的：

            public void run() {
                mTid = Process.myTid();
                Looper.prepare(); // 创建一个Looper对象
                synchronized(this) {
                    mLooper = Looper.myLooper(); // 取得属于该线程的Looper对象
                    notifyAll();
                }
                Process.setThreadPriority(mPriority);
                onLooperPrepared(); // 该方法是空的，子类可以实现，作用是在线程循环之前做一些准备工作
                Looper.loop(); // 启动Looper的消息循环
                mTid = -1;
            }
    Looper中有一个静态对象`private static final ThreadLocal sThreadLocal = new ThreadLocal();`，该对象是一个ThreadLocal对象，它保证了每个线程都有且
    仅有一个Looper对象。Looper的`prepare`方法首先检查本线程的Looper对象是否存在，如果不存在则用`sThreadLocal.set(new Looper());`创建一个。
    2. 每个Handler都会关联一个MessageQueue，这个MessageQueue被封装在Looper中。每个Message都有一个target对象，这个target就是将Message添加到MessageQueue
    的Handler对象。 MessageQueue主要用在loop()循环中如下：

            // pseudo code
            while(true) {
                Message msg = messageQueue.next();  // 该方法在MessageQueue为空时，应该会休眠。
                if(msg != null) {
                    if(msg.target == null) return;  // 当msg的target是null时退出循环，即结束loop()。该判断很重要，因为Looper的quit方法就是用该原理
                    msg.target.dispatchMessage(msg);
                    msg.recycle();
                }
            }

    3. Handler：Handler中包含一个Looper对象，一个MessageQueue（取自Looper对象），还有一个Callback mCallback。Handler能发送两种消息，一种是Runnable，一种
    是Message对象。事实上，在post方法内部，Runnable对象也是被封装成了Message。

            // handler的sendMessage方法 pseudo code
            public boolean sendMessageAtTime(Message msg, long uptimeMillis) {
                boolean sent = false;
                if(mQueue != null) {
                    msg.target = this;
                    sent = queue.enqueueMessage(msg, uptimeMillis);
                }
                return sent;
            }

            // Handler的dispatchMessage方法（在Looper的loop方法中被调用，因此这个方法是运行在Looper的线程中的） pseudo code
            public void dispatchMessage(Message msg) {
                if(msg.callback != null) {
                    // 在post中callback设为Runnable对象
                    handlerCallback(msg);
                }
                else {
                    // 这里的mCallback是Handler的内部成员，可以在Activity中实现Handler.Callback接口，这样就不需要继承Handler重写HandlerMessage方法了
                    if(mCallback != null) {
                        if(mCallback.handlerMessage(msg)) return;
                    }
                    // 子类实现的handlerMessage方法
                    handlerMessage(msg);
                }
            }
            
7. Android下结束进程的方法
finish方法
android.os.Process.killProcess(android.os.Process.myPid()) 对于非root用户，该方法只能杀掉同一个包中的进程和由这个app创造的进程。
void killBackgroundProcesses(string packageName)
8 Intent 的Flag
9. Android View的绘制过程
第一步：当activity启动的时候，触发初始化view过程的是由Window对象的DecorView调用View（具体怎样从xml中读取是用LayoutInflater.from(context).inflate）对象的public final void measure(int widthMeasureSpec, int heightMeasureSpec)方法开始的，这个方法是final类型的，也就是所有的子类都不能继承该方法，保证android初始化view的原理不变。具体参数类值，后面会介绍。
第二步：View的measure方法 onMeasure(widthMeasureSpec, heightMeasureSpec)，该方法进行实质性的view大小计算。注意：view的大小是有父view和自己的大小决定的，而不是单一决定的。这也就是为什么ViewGroup的子类会重新该方法，比如LinearLayout等。因为他们要计算自己和子view的大小。View基类有自己的实现，只是设置大小。其实根据源码来看，measure的过程本质上就是把Match_parent和wrap_content转换为实际大小
第三步：当measure结束时，回到DecorView，计算大小计算好了，那么就开始布局了，开始调用view的 public final void layout(int l, int t, int r, int b)，该还是也是final类型的，目的和measure方法一样。layout方法内部会调用onlayout(int l, int t, int r, int b )方法，二ViewGroup将此方法abstract的了，所以我们继承ViewGroup的时候，需要重新该方法。该方法的本质是通过measure计算好的大小，计算出view在屏幕上的坐标点
第四步：measure过了，layout过了，那么就要开始绘制到屏幕上了，所以开始调用view的  public void draw(Canvas canvas)方法，此时方法不是final了，原因是程序员可以自己画，内部会调用ondraw，我们经常需要重写的方法。 简单描述可以解释为：计算大小（measure），布局坐标计算（layout），绘制到屏幕（draw）；
10. View，SurfaceView和GLSurfaceView的区别
view是最基础的，必须在UI主线程内更新画面，速度较慢。
SurfaceView 是view的子类，类似使用双缓机制，在新的线程中更新画面所以刷新界面速度比view快
GLSurfaceView 是SurfaceView的子类，opengl 专用的
11. Android程序OOM的问题。
使用DDMS的Heap内存检测工具
1). Android 单个进程的内存使用分为Dalvik内存和Native内存，单个进程内存上限的原因，APP使用的内存不能无线增长。当使用过多的内存时，必然导致OOM的发生。同时Android系统中规定Bitmap所占的内存不能超过固定的数目，一般是8M。因此过多的Bitmap对象也会导致OOM错误。
避免这个方式导致的OOM的方法，一个是在Manifest中申明largeHeap属性，增加最大heapsize。
另外就是注意一些浪费内存的编程习惯。
a). Bitmap的使用优化。包括及时Recycle，对于大图要Sample，网络下载图片的话，小图使用LRUCache+softReference+sd卡缓存，大图按需下载。
b). 使用保守的Service，避免Service一直在后台运行。当任务结束时，就停止运行Service。
c). 谨慎使用外部依赖库
d). 使用多进程
2). 由于Android内存泄露导致的OOM
a. Cursor没有guanbi
b. 构造Adapter时没有使用缓存的convertView
c. Bitmap没有recycle()。事实上，Bitmap对象将其引用设为null后，当gc运行时确实会自动回收的。但是gc运行时间是不确定的，而bitmap又非常占内存。因此recycle方法的使用是必要的。
d. 引用的context对象activity过了生命周期，这里，我们的解决方法是使用weakreference或者是使用ApplicationContext
