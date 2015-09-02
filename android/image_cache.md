**Android开发中，为了避免图片导致的OOM，一般使用SoftReference，LruCache, DiskCache等机制**  
1. SoftReference和WeakReference的区别：  
当对象仅仅由SoftReference引用，当内存不足时会被gc回收，而当内存充足时gc并不会回收该对象  
当对象仅仅由WeakReference引用，无论何时，只要gc运行，就会被回收。
2. LruCache：LruCache as a Memory Cache method, keeping recently referenced objects in a strong referenced `LinkedHashMap`.  
LruCache can be initialized by a specific cache size with unit KB. Following is a simple example of using LruCache:

        private LruCache<String, Bitmap> mMemoryCache;
        @Override
        protected void onCreate(Bundle savedInstanceState) {
            // savedInstanceState is used when activity was force closed early
            // and onSaveInstance(outState) called, this method save previous state of the activity into outState,
            // next time when activity resumed by user, this time you can grab the data from savedInstanceState

            final int maxMemory = (int) (Runtime.getRuntime().maxMemory() / 1024);
            // use 1/8 of the available Memory
            final int cacheSize = maxMemory / 8;
            mMemoryCache = new LruCache<String, Bitmap>(cacheSize) {
                @Override
                protected int sizeOf(String key, Bitmap bitmap) {
                    // Override this method is need.
                    return bitmap.getByteCount() / 1024;
                }
            };
            // other codes
        }

        public void addBitmapToMemoryCache(String key, Bitmap bitmap) {
            if (getBitmapFromMemCache(key) == null) {
                mMemoryCache.put(key, bitmap);
            }
        }

        public Bitmap getBitmapFromMemCache(String key) {
            return mMemoryCache.get(key);
        }

3. DiskLruCache:A disk cache can be used in these cases to persist processed bitmaps and help decrease loading times where images
are no longer available in a memory cache. Of course, fetching images from disk is slower than loading from memory and should
be done in a background thread, as disk read times can be unpredictable.

        DiskLruCache mDiskCache = DiskLruCache.open(cache_dir, size);  // cache_dir is a File Object, and the unit of size is Byte
