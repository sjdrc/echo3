#ifndef _ECHO_THREAD_H_
#define _ECHO_THREAD_H_
#include <echo/Kernel/ExecutionModel.h>
#include <echo/Kernel/Kernel.h>
#include <echo/Chrono/Chrono.h>
#include <echo/cpp/functional>
#include <string>

namespace Echo
{
	class Kernel;
	
	class Thread
	{
	public:
		typedef function<void()> ThreadFunction;
		typedef function<void(Thread&)> ThreadCompleteCallback;
		
		/**
		 * Constructor
		 * @param name name of the thread.
		 */
		explicit Thread(const std::string& name);

		/**
		 * Constructor
		 * @param name name of the thread.
		 * @param kernel the kernel object to execute.
		 */
		explicit Thread(const std::string& name, Kernel* kernel);
		
		/**
		 * Constructor
		 * @param name name of the thread.
		 * @param threadFunction function run on the thread when Execute is called.
		 */
		explicit Thread(const std::string& name, ThreadFunction threadFunction);
		
		/**
		 * Destructor.
		 */
		~Thread();

		/**
		 * Get the name of the thread.
		 * @return the name of the thread.
		 */
		const std::string& GetName();
		
		/**
		 * Set the function that will be called on the next call to Execute()
		 * @param threadFunction
		 */
		void SetThreadFunction(ThreadFunction threadFunction) {mThreadFunction=threadFunction;}
		
		/**
		 * Provide the Thread with a shared kernel object.
		 */
		void SetSharedKernel(shared_ptr<Kernel> kernel) {mSharedKernel = kernel;}

		/**
		 * Execute the thread.
		 * Implementations should call _Execute() on the thread.
		 * @return false if the thread is already executing.
		 */
		bool Execute();
		
		/**
		 * Wait for this thread to be notified.
		 * Causes the calling thread to wait until Notify() is called.
		 */
		void Wait();

		/**
		 * Suspend execution until continue is called.
		 */
		void Suspend();

		/**
		 * Continue execution if it is suspended.
		 */
		void Continue();
		
		/**
		 * Notify any waiting threads.
		 * Causes any threads that had previously called Wait() to continue execution.
		 */
		void Notify();
		
		/**
		 * The calling thread waits until this thread is complete.
		 */
		void Join();

		/**
		 * Requests termination of the thread.
		 * If the thread is executing a Kernel object then this will cause the thread to
		 * terminate at the end of the current frame.
		 * @param waitForTermination
		 */
		void Terminate(bool waitForTermination = false);
		
		/**
		 * Set the 
		 * @param callback
		 */
		void SetCompleteCallback(ThreadCompleteCallback callback)
		{
			mThreadCompleteCallback = callback;
		}
		
		/**
		 * Get the execution result of the thread.
		 * The execution result is only set when a kernel is executed.
		 */
		Kernel::ExecutionResult GetKernelExecutionResult() const {return mKernelExecutionResult;}
		
		/**
		 * Suspend a thread from processing.
		 * @param sleepTime the number of seconds to sleep for.
		 */
		static void Sleep(Seconds sleepTime);
	private:
		/**
		 * An execution model used for Kernel objects that don't have one.
		 * 
		 * Any kernel that is run in a separate thread will be run continuously using this model.
		 * 
		 * The motivation behind this is that usually the main thread is responsible for dealing with the
		 * system, so Platform::CreateDefaultExecutionModel() is used to create the appropriate model for
		 * the target platform. There isn't anything preventing you from using a thread to interact with
		 * the system and leave the main thread to do something else, if that is the case then the Kernel
		 * that you pass to the thread for execution should have the execution model set beforehand.
		 * 
		 * This model allows threads to automatically set the model to save you from creating one yourself.
		 * Creating models unless you're a platform developer is discouraged as it may mean the wrong model
		 * is used on the platform thread by mistake.
		 */
		class ThreadExecutionModel : public ExecutionModel
		{
		public:
			ThreadExecutionModel() : ExecutionModel(Models::NONE){}
			~ThreadExecutionModel(){}
			bool SupportsModel(Model model) override {return (model==Models::NONE);}
			bool ProcessEvents(f32/*timeOutSuggestion*/) override {return false;}
			bool EnterSystemEventManager() override {return false;}
			bool SendUpdateRequest() override {return false;}
		};

		class Implementation;
		shared_ptr<Implementation> mImplementation;
		Kernel* mKernel;
		shared_ptr<Kernel> mSharedKernel;
		ThreadFunction mThreadFunction;
		ThreadCompleteCallback mThreadCompleteCallback;
		std::string mName;
		bool mTerminate;
		bool mExecuting;
		Kernel::ExecutionResult mKernelExecutionResult;

		//Internal Execute.
		void _Execute();
		void InitialiseImplementation();
		void CleanImplementation();
	};
}
#endif
