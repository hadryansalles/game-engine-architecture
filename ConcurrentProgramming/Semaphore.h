#pragma once

#include <condition_variable>
#include <mutex>

class Semaphore {
private:
	int m_Count;
	std::mutex m_Mutex;
	std::condition_variable m_CV;
	
public:
	explicit Semaphore(int initialCount)
		: m_Count(initialCount)
	{}
	
	void Take() {
		// lock the mutex to modify the counter
		std::unique_lock<std::mutex> lock(m_Mutex);
	
		// if the counter is zero, wait for some other thread release it
		// releasing the mutex while wait
		while (m_Count == 0) {
			m_CV.wait(lock); 
		}
	
		// decrease the counter
		--m_Count;		

		// implict release the lock
	}

	void Give() {
		// lock the mutex to modify the counter
		std::unique_lock<std::mutex> lock(m_Mutex);

		// increase the counter
		++m_Count;
		// notify other waiting threads if the counter is one
		if (m_Count == 1) {
			m_CV.notify_all();
		}

		// implicit release the lock
	}

	void Wait()   { Take(); }
	void Post()   { Give(); }
	void Signal() { Give(); }
	void Down()   { Take(); }
	void Up()     { Give(); }
	void P()      { Take(); } // Dutch "proberen" = "test"
	void V()      { Give(); } // Dutch "verhogen" = "increment"
};
