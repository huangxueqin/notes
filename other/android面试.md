1. Android应用程序框架
    1. 应用程序层
    2. 应用程序框架层——view，content providers，resource manager activity manager，notification manager。。。
    3. 系统运行库层——系统库：SQLite，SurfaceManager，媒体，libc，Web; Android运行库：虚拟机等
    4. Linux内核
2. 如果后台的Activity由于某原因被系统回收了，如何在被系统回收之前保存当前状态？
用onSaveInstanceState(Bundle outState)方法保存目前状态，如果Activity被杀死了，那么重新启动该Activity时会调用onCreate(Bundle savedInstance)，这时savedInstance就是之前的outState
3. ContentProvider相关
    1. 读取ContentProvider的内容使用：  
        Cursor cursor = getContentResolver().query(Uri, ...)得到
    2. Uri：
    ```
    scheme     ——content://
    Authority  ——用户定义，并在Android Manifest里面注册
    path           ——使用UriMatcher进行匹配说明
    ```
    3. 检测ContentProvider数据库内容的变化：  
    使用ContentObserver，在Activity中使用  
        getContentResolver().registerContentObserver(Uri, ..,cob)  
    注册一个ContentObserver。然后在insert，delete，update等改变数据的方法执行末尾调用  
    getContext().getContentResolver().notifyChange(Uri, ..)  
    方法通知发生变动
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
    当执行onReceive(Context context, Intent，data)  
    时是活跃的，活跃的BroadcastReceive进程是收系统保护的，无法被杀死。当退出onReceive方法时是不活跃的。onReceive默认在MainThread中执行，当执行较繁重的任务时，最好新起一个线程。如果担心这个线程会被杀死，则最好新起一个serveice执行该任务。
5. Service相关
    1. bind Service的几种方法
        * 继承 Binder class，在Service的onBind中返回实例。在Activity的ServiceConnection方法的回调函数中接受该实例，然后通过该实例执行相应操作，或通过该实例得到Service对象。
        继承binder方法只能使用在Activity和service属于同一Application的情况  
        * 使用Messenger  
        在Service中包含一个Handler，用该Handler实例化一个Messenger对象  
        final Messenger mMessenger = new Messenger( mHandler);  
        在Service的onBind方法中，返回mMessenger.getBinder()，在Activity中使用接受到的binder初始化发送段的Messenger  
        mMessenger = new Messenger(binder)  
        当要发送消息给service时，使用`mMessenger.send(msg)`即可。如果需要service回送消息执行结果，可以在新建一个Messenger mCallback，然后把上面的msg的replyTo设为该mCallback。在Service中，当处理msg完毕后，可以通过msg的replyTo传递结果给Activity
        Messenger方法可以用于跨进程IPC
        * 使用AIDL  
        AIDL中可用的数据类型——primitive type in Java， String， CharSequence， List， Map
        两个Application都要实现相同的.aidl文件，client端使用asInterface（bind）方法将service传入的接口转化为本地定义的相同结构，然后操作该接口
6. Handler机制相关
    1. 主线程启动时已经建立了主线程对应的Looper，而自定义的新线程并没有。若要定义属于自定义线程的Looper，应该在run方法里用`Looper.prepare()`方法创建一个Looper对象，
    用Looper.loop()方法执行消息循环
    2. 每个Handler都会关联一个MessageQueue，这个MessageQueue被封装在Looper中。每个Message都有一个target对象，这个target就是将Message添加到MessageQueue的Handler对象
    3. 
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
