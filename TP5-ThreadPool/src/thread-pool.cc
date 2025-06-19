#include "thread-pool.h"
#include <queue>
#include <stdexcept>
#include <mutex>
#include <memory>

using namespace std;

struct WorkerInfo {
    mutex mtx;
    bool available; //worker disponible para nueva tarea?
    unique_ptr<Semaphore> workReady; //puntero al semaforo del worker
    function<void(void)> assignedTask; //tarea del worker

    WorkerInfo() //constructor default
            : available(true), //arranco libre
            workReady(unique_ptr<Semaphore>(new Semaphore(0))) {} //semaforo en 0

    WorkerInfo(WorkerInfo&& other) noexcept //constructor para pasar variables de un worker a otro sin copia
            : available(other.available), //copia del valor
            workReady(move(other.workReady)), //se mueve el valor
    assignedTask(move(other.assignedTask)) {}  //se mueve el valor

    WorkerInfo& operator=(WorkerInfo&& other) noexcept { //operador de igualar, pasa variables de un worker a otro sin copia
        if (this != &other) {
            available = other.available; //mismo concepto que en las asignaciones del constructor
            workReady = move(other.workReady);
            assignedTask = move(other.assignedTask);
        }
        return *this;
    }

    WorkerInfo(const WorkerInfo&) = delete; //ambas lineas impiden la copia del worker
    WorkerInfo& operator=(const WorkerInfo&) = delete;
};

static queue<function<void(void)>> taskQueue;
static mutex queueMutex;
static Semaphore taskAvailable(0);
static vector<WorkerInfo> workerInfos;
static mutex poolMutex;
static condition_variable allTasksComplete;
static int activeTasks = 0;

ThreadPool::ThreadPool(size_t numThreads): wts(numThreads), done(false) {

    // dejo white las workerInfos
    workerInfos.resize(numThreads);
    activeTasks = 0;

    // creo N workers
    for (size_t workerId = 0; workerId < numThreads; ++workerId)
        wts[workerId].ts = thread([this, workerId] { worker(workerId); });

    // Iniciar hilo dispatcher
    dt = thread([this] { dispatcher(); });
}

void ThreadPool::schedule(const function<void(void)>& thunk) {
    if (done) throw runtime_error("Cannot schedule on a terminated ThreadPool.");
    if (!thunk) throw invalid_argument("Scheduled function is null.");

    {
        lock_guard<mutex> lock(queueMutex);
        taskQueue.push(thunk);
        {
            lock_guard<mutex> poolLock(poolMutex);
            activeTasks++;
        }
    }

    taskAvailable.signal();
}


void ThreadPool::dispatcher() {
    while (true) {
        // espero hasta que haya trabajo disponible
        taskAvailable.wait();

        if (done) break;

        // extraigo trabajo de la cola
        function<void(void)> task;
        bool hasTask = false;
        {
            lock_guard<mutex> lock(queueMutex);
            if (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
                hasTask = true;
            }
        }

        if (!hasTask) continue;

        // espero a que un worker esté disponible y asignarle el trabajo
        bool assigned = false;
        while (!assigned && !done) {
            for (size_t i = 0; i < workerInfos.size(); ++i) {
                lock_guard<mutex> lock(workerInfos[i].mtx);
                if (workerInfos[i].available) {
                    // marco worker como no disponible
                    workerInfos[i].available = false;
                    // asigno la tarea
                    workerInfos[i].assignedTask = task;
                    wts[i].thunk = task; // También asignar al struct del header
                    // señalo al worker para que ejecute
                    workerInfos[i].workReady->signal();
                    assigned = true;
                    break;
                }
            }

            if (!assigned)
                // si no hay workers disponibles, espero
                this_thread::yield();
        }
    }
}

void ThreadPool::worker(int workerId) {
    while (true) {
        // espero hasta que el dispatcher le asigne trabajo
        workerInfos[workerId].workReady->wait();

        if (done) break;

        // ejecuto la función asignada
        function<void(void)> taskToExecute;
        {
            lock_guard<mutex> lock(workerInfos[workerId].mtx);
            taskToExecute = workerInfos[workerId].assignedTask;
        }

        // ejecuto la tarea
        taskToExecute();

        // marco como disponible nuevamente
        {
            lock_guard<mutex> lock(workerInfos[workerId].mtx);
            workerInfos[workerId].available = true;
        }

        // decremento el contador de tareas activas
        {
            lock_guard<mutex> lock(poolMutex);
            activeTasks--;
            if (activeTasks == 0)
                allTasksComplete.notify_all();
        }
    }
}

void ThreadPool::wait() {
    unique_lock<mutex> lock(poolMutex);
    while (activeTasks != 0 || !taskQueue.empty()) {
        allTasksComplete.wait(lock);
    }
}

ThreadPool::~ThreadPool() {
    // espero a que todas las tareas terminen
    wait();

    // marco como terminado
    done = true;

    // Despertar al dispatcher
    taskAvailable.signal();

    // Esperar a que termine el dispatcher
    if (dt.joinable())
        dt.join();

    // despierto a todos los workers
    for (size_t i = 0; i < workerInfos.size(); ++i)
        workerInfos[i].workReady->signal();

    // espero a que terminen todos los workers
    for (auto& w : wts)
        if (w.ts.joinable())
            w.ts.join();

    // limpio la cola de tareas
    while (!taskQueue.empty())
        taskQueue.pop();

    activeTasks = 0;
}