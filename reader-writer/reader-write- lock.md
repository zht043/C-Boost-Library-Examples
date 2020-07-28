# Reader Writer Lock using Boost

```c++
/* Synchronization for Reader/Writer problems */
typedef boost::shared_mutex reader_writer_mutex; 
// get exclusive access
#define writer_lock(mutex) do { \
    boost::upgrade_lock<reader_writer_mutex> __writer_lock(mutex); \
    boost::upgrade_to_unique_lock<reader_writer_mutex> __unique_writer_lock( __writer_lock ); \
}while(0)

// get shared access
#define reader_lock(mutex) boost::shared_lock<reader_writer_mutex>  __reader_lock(mutex); 
```



Reader writer lock ensure when a thread writes to the **shared resource**, all **other** threads got locked; when readers from **different threads** try to read the **shared resource**, they can read it **simultaneously**.  



Usage

```
reader_writer_mutex mu;
void writer_thread_1(...) {
	/* lock will be automatically released after this function returns
	 * function scope lock
	 * or you can mannually unlock by calling mu.unlock();
	 */
	writer_lock(mu); 
	...
}

void writer_thread_2(...) {
	writer_lock(mu); 
	...
}

void reader_thread_1(...) {
	reader_lock(mu);
	...
}

void reader_thread_2(...) {
	reader_lock(mu);
	...
}

...
```

