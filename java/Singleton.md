**Previously I have this Solution:**

            public final class Singleton {
                private Singleton {}
                private static Singleton instance;
                public static synchronized final Singleton getInstance() {
                    if(instance == null) {
                        instance = new Singleton();
                    }
                    return instance;
                }
            }
But this solution actually can't ensure singleton because of Reflection Attack. And what's more,
using synchronized method make the multi-threaded program inefficient.

**An efficient substitute is showing in the following:**

            public final class Singleton {
                private static final Singleton instance = new Singleton();
                private Singleton {
                    if(instance != null) {
                        throw new IllegalStateException("Already instantiated");
                    }
                }
                public static Singleton getInstance() {
                    return instance;
                }
            }

This solution is quite simple and work. Static variable instance initialized as soon as the class
loaded into memory. So it is threaded safe, and also, get rid of Reflection Attack.
However, there still has a problem. When the class is very large and complex and also has other
accessible static methods or fields that might be used before an instance is need, then a lazy
initialize mode may prefer.

**The following code shows a lazy initialize mode as well as take advantage of above solution:**

            public final class Singleton {
                private static class SingletonLoader {
                    private static final Singleton instance = new Singleton();
                }
                private Singleton() {
                    if(SingletonLoader.instance != null) {
                        throw new IllegalStateException("Already instantiated");
                    }
                }
                public Singleton getInstance() {
                    return SingletonLoader.instance();
                }
            }

**There is still a better solution since Java5. Using enum:**

            public enum Singleton {
                instance;
            }

When in Java5 or latter, This solution is the simplest, if lazy initialization is not needed.
This solution is definitely the best!
