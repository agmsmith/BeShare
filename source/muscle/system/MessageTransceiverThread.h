/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */

#ifndef MuscleMessageTransceiverThread_h
#define MuscleMessageTransceiverThread_h

#include "system/Thread.h"
#include "reflector/StorageReflectSession.h"

BEGIN_NAMESPACE(muscle);

class ThreadSupervisorSession;
class ThreadWorkerSession;
class ThreadWorkerSessionFactory;
class DrainTag;

/** These are reply codes returned by MessageTransceiverThread::GetNextEventFromInternalThread() 
  * @see MessageTransceiverThread::GetNextEventFromInternalThread()
  */
enum {
   MTT_EVENT_INCOMING_MESSAGE = 1835233840, // A new message from a remote computer is ready to process
   MTT_EVENT_SESSION_ACCEPTED,              // A new session has been created by one of our factory objects
   MTT_EVENT_SESSION_ATTACHED,              // A new session has been attached to the local server
   MTT_EVENT_SESSION_CONNECTED,             // A session on the local server has completed its connection to the remote one
   MTT_EVENT_SESSION_DISCONNECTED,          // A session on the local server got disconnected from its remote peer
   MTT_EVENT_SESSION_DETACHED,              // A session on the local server has detached (and been destroyed)
   MTT_EVENT_FACTORY_ATTACHED,              // A ReflectSessionFactory object has been attached to the server
   MTT_EVENT_FACTORY_DETACHED,              // A ReflectSessionFactory object has been detached (and been destroyed)
   MTT_EVENT_OUTPUT_QUEUES_DRAINED,         // Output queues of sessions previously specified in RequestOutputQueuesDrainedNotification() have drained
   MTT_EVENT_SERVER_EXITED,                 // The ReflectServer event loop has terminated
   MTT_LAST_EVENT
};
   
/** These are command codes used in the MessageTransceiverThread's internal protocol */
enum {
   MTT_COMMAND_SEND_USER_MESSAGE = 1835230000, // contains a user message to be sent out
   MTT_COMMAND_ADD_NEW_SESSION,                // contains info on a new session to be created
   MTT_COMMAND_PUT_ACCEPT_FACTORY,             // request to start accepting session(s) on a given port
   MTT_COMMAND_REMOVE_ACCEPT_FACTORY,          // remove the acceptor factory on a given port(s)
   MTT_COMMAND_SET_DEFAULT_PATH,               // change the default distribution path
   MTT_COMMAND_NOTIFY_ON_OUTPUT_DRAIN,         // request a notification when all currently pending output has been sent
   MTT_COMMAND_SET_INPUT_POLICY,               // set a new input IO Policy for worker sessions
   MTT_COMMAND_SET_OUTPUT_POLICY,              // set a new output IO Policy for worker sessions
   MTT_COMMAND_REMOVE_SESSIONS,                // remove the matching worker sessions
   MTT_COMMAND_SET_OUTGOING_ENCODING,          // set the MUSCLE_MESSAGE_ENCODING_* setting on worker sessions
   MTT_LAST_COMMAND
};

/** These are field names used in the MessageTransceiverThread's internal protocol */
#define MTT_NAME_PATH        "path"  // field containing a session path (e.g. "/*/*")
#define MTT_NAME_MESSAGE     "mssg"  // field containing a message object
#define MTT_NAME_SOCKET      "sock"  // field containing a Socket reference
#define MTT_NAME_IP_ADDRESS  "addr"  // field containing an int32 IP address
#define MTT_NAME_HOSTNAME    "host"  // field containing an ASCII hostname or IP address
#define MTT_NAME_PORT        "port"  // field containing an int16 port number
#define MTT_NAME_SESSION     "sess"  // field containing an AbstractReflectSession tag
#define MTT_NAME_FROMSESSION "sfrm"  // field containing the root path of the session this message is from (e.g. "192.168.1.103/17")
#define MTT_NAME_FACTORY     "fact"  // field containing a ReflectSessionFactory tag
#define MTT_NAME_DRAIN_TAG   "dtag"  // field containing a DrainTag reference
#define MTT_NAME_POLICY_TAG  "ptag"  // field containing an IOPolicy reference
#define MTT_NAME_ENCODING    "enco"  // field containing the MUSCLE_MESSAGE_ENCODING_* value

/** 
  * This is a class that holds a ReflectServer object in an internal thread, and mediates
  * between it and the calling code.  It is primarily used for integrating MUSCLE networking  
  * with another messaging system.
  * @see AMessageTransceiverThread for use with AtheOS APIs
  * @see BMessageTransceiverThread for use with BeOS APIs
  * @see QMessageTransceiverThread for use with Qt's APIs
  */
class MessageTransceiverThread : public Thread
{
public:
   /** Constructor */
   MessageTransceiverThread();

   /** Destructor.  If the internal thread was started, you must make sure it has been
     * shut down by calling ShutdownInternalThread() before deleting the MessageTransceiverThread object.
     */
   virtual ~MessageTransceiverThread();

   /** Overridden to do some additional setup, before starting the internal thread.
     * @returns B_NO_ERROR on success, B_ERROR on error (out of memory, or thread is already running)
     */
   virtual status_t StartInternalThread();

   /** Asynchronously sends the given message to the specified target session(s) inside the held ReflectServer.  
     * May be called at any time; if called while the internal thread isn't running, the given
     * message will be queued up and delivered when the internal thread is started.
     * @param msgRef Reference to the message to send
     * @param optDistPath Optional node path to match against to decide which ThreadWorkerSessions to send to.  
     *                    If left as NULL, the default distribution path will be used.
     * @returns B_NO_ERROR if the message was enqueued successfully, or B_ERROR if out-of-memory
     */
   virtual status_t SendMessageToSessions(const MessageRef & msgRef, const char * optDistPath = NULL);

   /**
     * Adds a new session that will use the given socket for its I/O.
     * May be called at any time, but behaves slightly differently depending on whether the internal
     * thread is running or not.  If the internal thread is running, the session will be added asynchronously
     * to the server.  If not, the call is immediately passed on through to ReflectServer::AddNewSession(). 
     * @param socket The TCP socket that the new session will be using, or -1, if the new session is to have no
     *               associated TCP connection.  This socket becomes property of this object on success.
     * @param optSessionRef Optional reference for a session to add.  If it's a NULL reference, a default ThreadWorkerSession
     *                      will be created and used.  If you do specify a session here, you will want to use either a 
     *                      ThreadWorkerSession, a subclass of ThreadWorkerSession, or at least something that acts 
     *                      like one, or else things won't work correctly.
     *                      The referenced session becomes sole property of the MessageTransceiverThread on success.
     * @return B_NO_ERROR on success, or B_ERROR on failure.  Note that if the internal thread is currently running,
     *         then success merely indicates that the add command was enqueued successfully, not that it was executed (yet).
     */  
   virtual status_t AddNewSession(int socket, const AbstractReflectSessionRef & optSessionRef);

   /** Convenience method -- calls the above method with a NULL session reference. */
   status_t AddNewSession(int socket) {return AddNewSession(socket, AbstractReflectSessionRef());}

   /**
     * Adds a new session that will connect out to the given IP address and port.
     * May be called at any time, but behaves slightly differently depending on whether the internal
     * thread is running or not.  If the internal thread is running, the session will be added asynchronously
     * to the server.  If not, the call is immediately passed on through to ReflectServer::AddNewConnectSession(). 
     * @param targetIPAddress IP address to connect to
     * @param port Port to connect to at that IP address.
     * @param optSessionRef optional Reference for a session to add.  If it's a NULL reference, a default ThreadWorkerSession 
     *                      will be created and used.  If you do specify a session here, you will want to use either a 
     *                      ThreadWorkerSession, a subclass of ThreadWorkerSession, or at least something that acts 
     *                      like one, or things won't work correctly.
     *                      The referenced session becomes sole property of the MessageTransceiverThread on success.
     * @return B_NO_ERROR on success, or B_ERROR on failure.  Note that if the internal thread is currently running,
     *         then success merely indicates that the add command was enqueued successfully, not that it was executed (yet).
     */
   virtual status_t AddNewConnectSession(uint32 targetIPAddress, uint16 port, const AbstractReflectSessionRef & optSessionRef);

   /** Convenience method -- calls the above method with a NULL session reference. */
   status_t AddNewConnectSession(uint32 targetIPAddress, uint16 port) {return AddNewConnectSession(targetIPAddress, port, AbstractReflectSessionRef());}

   /**
     * Adds a new session that will connect out to the given hostname and port.
     * May be called at any time, but behaves slightly differently depending on whether the internal
     * thread is running or not.  If the internal thread is running, the session will be added asynchronously
     * to the server.  If not, the call is passed immediately on through to ReflectServer::AddNewConnectSession(). 
     * @param targetHostName ASCII hostname or ASCII IP address to connect to.  (e.g. "blah.com" or "132.239.50.8")
     * @param port Port to connect to at that IP address.
     * @param optSessionRef optional Reference for a session to add.  If it's a NULL reference, a default ThreadWorkerSession 
     *                      will be created and used.  If you do specify session here, you will want to use either a 
     *                      ThreadWorkerSession, a subclass of ThreadWorkerSession, or at least something that acts
     *                      like one, or things won't work correctly.
     *                      The referenced session becomes sole property of the MessageTransceiverThread on success.
     * @return B_NO_ERROR on success, or B_ERROR on failure.  Note that if the internal thread is currently running,
     *         then success merely indicates that the add command was enqueued successfully, not that it was executed (yet).
     */
   virtual status_t AddNewConnectSession(const String & targetHostName, uint16 port, const AbstractReflectSessionRef & optSessionRef);

   /** Convenience method -- calls the above method with a NULL session reference. */
   status_t AddNewConnectSession(const String & targetHostName, uint16 port) {return AddNewConnectSession(targetHostName, port, AbstractReflectSessionRef());}

   /** Installs a new ReflectSessionFactory onto the ReflectServer (or replaces an existing one) on the given port.
     * May be called at any time, but behaves slightly differently depending on whether the internal
     * thread is running or not.  If the internal thread is running, the factory will be added to
     * the ReflectServer asynchronously; otherwise the call is passed directly through to ReflectServer::PutAcceptFactory().
     * @param port The port to place the new factory on.
     * @param optFactoryRef Optional reference to a factory object to use to instantiate new sessions.
     *                      Note that in order for things to work as expected, (optFactory) should create
     *                      only ThreadWorkerSessions (or sessions which are subclasses thereof)
     *                      If left as NULL (the default), a default ThreadWorkerFactory will be created and used.
     *                      The referenced factory becomes sole property of the MessageTransceiverThread on success.
     * @return B_NO_ERROR on success, or B_ERROR on failure.  Note that if the internal thread is currently running,
     *         then success merely indicates that the put command was enqueued successfully, not that it was executed (yet).
     */
   virtual status_t PutAcceptFactory(uint16 port, const ReflectSessionFactoryRef & optFactoryRef);

   /** Convenience method -- calls the above method with a NULL factory reference. */
   status_t PutAcceptFactory(uint16 port) {return PutAcceptFactory(port, ReflectSessionFactoryRef());}

   /** Removes an existing ReflectSessionFactory from the held ReflectServer.
     * May be called at any time, but behaves slightly differently depending on whether the internal
     * thread is running or not.  If the internal thread is running, the factory will be removed from
     * the ReflectServer asynchronously; otherwise the call is passed directly through to ReflectServer::RemoveAcceptFactory().
     * @param port The port to remove the factory from, or zero to remove all factories.
     * @return B_NO_ERROR on success, or B_ERROR on failure.  Note that if the internal thread is currently running,
     *         then success merely indicates that the remove command was enqueued successfully, not that it was executed (yet).
     */
   virtual status_t RemoveAcceptFactory(uint16 port);

   /** Stops the internal thread if it is running, destroys internal the internal ReflectServer object, and more or
     * less make this MessageTransceiverThread look like it had just been constructed anew.
     */
   virtual void Reset();

   /** 
     * Sets our ThreadSupervisorSession's default distribution path to (optPath).  
     * If called while the internal thread is running, the path change will be done asynchronously.
     * This path determines which sessions get the messages sent by SendMessageToSession() if no 
     * path is specified explicitely there.
     * Setting the path to "" indicates that you want all outgoing messages to go to all ThreadWorkerSessions.
     * The change of target path will only affect the routing of messages enqueued after this call has
     * returned, not ones that are currently enqueued for distribution or transmission.
     * @param distPath Node path to use by default, or NULL to send to all.
     * @return B_NO_ERROR if the set-target-path command was enqueued successfully, or B_ERROR if there was
     *                    an error enqueueing it.  Note that the actual change-of-path is done asynchronously.
     */
   status_t SetDefaultDistributionPath(const String & distPath);

   /** Returns our current default distribution path, or "" if it is unset. */
   const String & GetDefaultDistributionPath() const {return _defaultDistributionPath;}

   /**
     * Call this to get the next event notification message from the internal thread.  Typically you will want to call this
     * whenever your main thread has been notified that a new event may be pending.  You should keep calling this method
     * in a loop until it returns MTT_EVENT_NO_MORE_EVENTS; at that point it is okay to go back to waiting for the next
     * event notification signal.
     * @param retEventCode On success, this uint32 will be set to the event code of the returned event.
     *                     The event code will typically be one of the following constants:
     *  <ol>
     *   <li>MTT_EVENT_INCOMING_MESSAGE A new message from a remote computer is ready to process</li>
     *   <li>MTT_EVENT_SESSION_ACCEPTED A new session has been created by one of our ReflectSessionFactory objects</li>
     *   <li>MTT_EVENT_SESSION_ATTACHED A new session has been attached to the local server</li>
     *   <li>MTT_EVENT_SESSION_CONNECTED A session on the local server has completed its connection to the remote one</li>
     *   <li>MTT_EVENT_SESSION_DISCONNECTED A session on the local server got disconnected from its remote peer</li>
     *   <li>MTT_EVENT_SESSION_DETACHED A session on the local server has detached (and been destroyed)</li>
     *   <li>MTT_EVENT_FACTORY_ATTACHED A ReflectSessionFactory object has been attached to the server</li>
     *   <li>MTT_EVENT_FACTORY_DETACHED A ReflectSessionFactory object has been detached (and been destroyed)</li>
     *   <li>MTT_EVENT_OUTPUT_QUEUES_DRAINED Output queues of sessions previously specified in RequestOutputQueuesDrainedNotification() have drained</li>
     *   <li>MTT_EVENT_SERVER_EXITED The ReflectServer event loop has terminated</li>
     *  </ol>
     * May return some other code if the ThreadSupervisorSession or ThreadWorkerSessions have
     * been customized to return other message types.
     * @param retEventCode On successful return, the MTT_EVENT_* code for this event will be written here.
     * @param optRetMsgRef If non-NULL, on success the MessageRef this argument points to is written into so that
     *                     it references a Message associated with the event.  This is mainly used with the 
     *                     MTT_EVENT_INCOMING_MESSAGE event code.
     * @param optFromSession If non-NULL, the string that this argument points to will be have the root node path of
     *                       the relevant session written into it (e.g. "/192.168.1.105/17").
     * @param optFromPort If non-NULL, the uint16 that this arguments points will have the port number of the 
     *                    relevant factory written into it.
     * @returns The number of events left in the event queue (after our having removed one) on success, or -1 on failure.
     */
   int32 GetNextEventFromInternalThread(uint32 & retEventCode, MessageRef * optRetMsgRef = NULL, String * optFromSession = NULL, uint16 * optFromPort = NULL);

   /**
     * Requests that the MessageTranceiverThread object send us a MTT_EVENT_OUTPUT_QUEUES_DRAINED event
     * when all the specified outgoing message queues have become empty.  Which output queues are specified
     * is handled the same way as SendMessageToSessions() does it -- if you specify a path here, sessions
     * that match that path will be used, otherwise the default distribution path will be used.
     * @param notificationMsg MessageRef to return with the MTT_EVENT_OUTPUT_QUEUES_DRAINED event.  May be a NULL ref.
     * @param optDistPath If non-NULL, only sessions matching this path will be watched for drainage.
     * @param optDrainTag If non-NULL, this DrainTag will be used to track drainage, instead of a 
     *                    default one.  Don't supply a value for this argument unless you think you 
     *                    know what you are doing.  ;^)  On success, (optDrainTag) becomes property
     *                    of this MessageTransceiverThread.
     * @returns B_NO_ERROR on success (in which case an MTT_EVENT_OUTPUT_QUEUES_DRAINED event will be
     *          forthcoming) or B_ERROR on error (out of memory).
     */
   status_t RequestOutputQueuesDrainedNotification(const MessageRef & notificationMsg, const char * optDistPath = NULL, DrainTag * optDrainTag = NULL);

   /**
     * Tells the specified worker session(s) to install a new input IOPolicy.
     * @param pref Reference to the new IOPolicy object.  Since IOPolicies are generally
     *             not thread safe, the referenced IOPolicy should no be used after it has
     *             been successfully passed in via this call.  May be a NULL ref to remove
     *             the existing input policy.
     * @param optDistPath If non-NULL, only sessions matching this path will be affected.
     *                    A NULL path (the default) means affect all worker sessions.
     * @return B_NO_ERROR on success, or B_ERROR on failure.
     */
   status_t SetNewInputPolicy(const PolicyRef & pref, const char * optDistPath = NULL);

   /**
     * Tells the specified worker session(s) to install a new output IOPolicy.
     * @param pref Reference to the new IOPolicy object.  Since IOPolicies are generally
     *             not thread safe, the referenced IOPolicy should no be used after it has
     *             been successfully passed in via this call.  May be a NULL ref to remove
     *             the existing output policy.
     * @param optDistPath If non-NULL, only sessions matching this path will be affected.
     *                    A NULL path (the default) means affect all worker sessions.
     * @return B_NO_ERROR on success, or B_ERROR on failure.
     */
   status_t SetNewOutputPolicy(const PolicyRef & pref, const char * optDistPath = NULL);

   /**
     * Tells the specified worker session(s) to switch to a different message encoding 
     * for the Messages they are sending to the network.  Note that this only works if
     * the workers are using the usual MessageIOGateways for their I/O.
     * Note that ZLIB encoding is only enabled if your program is compiled with the
     * -DMUSCLE_ENABLE_ZLIB_ENCODING compiler flag set.
     * @param encoding one of the MUSCLE_MESSAGE_ENCODING_* constant declared in MessageIOGateway.h
     * @param optDistPath If non-NULL, only sessions matching this path will be affected.
     *                    A NULL path (the default) means affect all worker sessions.
     * @return B_NO_ERROR on success, or B_ERROR on failure.
     */
   status_t SetOutgoingMessageEncoding(int32 encoding, const char * optDistPath = NULL);

   /**
     * Tells the specified worker session(s) to go away.
     * @param optDistPath If non-NULL, only sessions matching this path will be affected.
     *                    A NULL path (the default) means all worker sessions will be destroyed.
     * @return B_NO_ERROR on success, or B_ERROR on failure.
     */
   status_t RemoveSessions(const char * optDistPath = NULL);

protected:
   /** Overridden to begin execution of the ReflectServer's event loop. */
   virtual void InternalThreadEntry();
 
   /** Creates and returns a new ThreadSupervisorSession to use
     * to do the internal-thread-side handling of the messages this API sends.
     * May be overridden if you wish to use a customized subclass instead.
     * @returns a new ThreadSupervisorSession on success, or NULL on failure.
     */
   virtual ThreadSupervisorSession * CreateSupervisorSession();

   /** Creates and returns a ThreadWorkerSession object.  Called when a new
     * session is requested (e.g. in AddNewSession(), but no session is specified
     * by the call.  This method may be overridden to customize the type of session used.
     */
   virtual AbstractReflectSession * CreateDefaultWorkerSession();

   /** Creates and returns a ThreadWorkerSessionFactory object.  Called when a new
     * factory is requested (e.g. in PutAcceptFactory(), but none is specified.  
     * This method may be overridden to customize the type of factory used.
     */
   virtual ReflectSessionFactory * CreateDefaultSessionFactory();

   /** Creates and returns a new ReflectServer object.  
     * This method may be overridden if a custom ReflectServer is needed.
     */
   virtual ReflectServer * CreateReflectServer();

private:
   friend class ThreadSupervisorSession;
   status_t EnsureServerAllocated();
   status_t SendAddNewSessionMessage(const AbstractReflectSessionRef & sessionRef, int socket, const char * hostName, uint32 hostIP, uint16 port);
   status_t SetNewPolicyAux(uint32 what, const PolicyRef & pref, const char * optDistPath);

   ReflectServer * _server;
   String _defaultDistributionPath;
};

/** This is a session that represents a connection to another computer or etc.
  * ThreadWorkerSessions are added to a MessageTransceiverThread's held ReflectServer
  * object by the ThreadSupervisorSession on demand.
  */
class ThreadWorkerSession : public StorageReflectSession
{
public:
   /** Constructor */
   ThreadWorkerSession();

   /** Destructor */
   virtual ~ThreadWorkerSession();

   /** Overridden to send a message back to the user */
   virtual status_t AttachedToServer();

   /** Overridden to send a message back to the user */
   virtual void AboutToDetachFromServer();

   /** Overridden to send a message back to the user */
   virtual bool ClientConnectionClosed();

   /** Overridden to send a message back to the user to let him know the connection is ready */
   virtual void AsyncConnectCompleted();

   /** Overridden to wrap incoming messages and pass them along to our supervisor session */
   virtual void MessageReceivedFromGateway(const MessageRef & msg, void * userData);

   /** Overriden to handle messages from our supervisor session */
   virtual void MessageReceivedFromSession(AbstractReflectSession & from, const MessageRef & msg, void * userData);

   /** Returns a human-readable label for this session type:  "ThreadWorker" */
   virtual const char * GetTypeName() const {return "ThreadWorker";}

   /** Overridden to clear our _drainNotifiers Queue when appropriate */
   virtual int32 DoOutput(uint32 maxBytes);

private:
   friend class ThreadWorkerSessionFactory;
   Queue<MessageRef> _drainedNotifiers;
   bool _sendAcceptedMessage;
};

/** A factory class that returns new ThreadWorkerSession objects. */
class ThreadWorkerSessionFactory : public StorageReflectSessionFactory
{
public:
   /** Default Constructor.  */
   ThreadWorkerSessionFactory();

   /** Overridden to send a message back to the user */
   virtual status_t AttachedToServer();

   /** Overridden to send a message back to the user */
   virtual void AboutToDetachFromServer();

   /** Implemented to call CreateThreadWorkerSession() to create
     * a new session, and on success to send a MTT_EVENT_SESSION_ACCEPTED
     * Message back to the main thread.  Subclasses that wish to
     * to have this factory class return a different type of
     * session object should override CreateThreadWorkerSession(const String &)
     * instead of this method.
     */
   virtual AbstractReflectSession * CreateSession(const String &);

   /** Default implementation returns a new ThreadWorkerSession object.
     * Subclasses may override this method to return a different type of
     * object, as long as the returned object is a subclass of ThreadWorkerSession.
     */
   virtual ThreadWorkerSession * CreateThreadWorkerSession(const String &);
};

/** This is the session that acts as the main thread's agent inside the MessageTransceiverThread's
  * held ReflectServer object.  It accepts commands from the MessageTransceiverThread object, and
  * routes messages to and from the ThreadWorkerSessions.
  */
class ThreadSupervisorSession : public StorageReflectSession
{
public:
   /** Default Constructor */
   ThreadSupervisorSession();

   /** Destructor */
   virtual ~ThreadSupervisorSession();

   /** Overridden to neutralize any outstanding DrainTag objects */
   virtual void AboutToDetachFromServer();

   /** Overridden to create a custom gateway for interacting with the MessageTransceiverThread */
   virtual AbstractMessageIOGateway * CreateGateway();         

   /** Overridden to deal with the MessageTransceiverThread.  If you are subclassing
     * ThreadSupervisorSession, don't override this method; override MessageReceivedFromOwner() instead.
     */
   virtual void MessageReceivedFromGateway(const MessageRef & msg, void * userData);

   /** Overridden to handle messages coming from the ThreadWorkerSessions. */
   virtual void MessageReceivedFromSession(AbstractReflectSession & from, const MessageRef & msg, void * userData);

   /** Overriden to handle messages from factories */
   virtual void MessageReceivedFromFactory(ReflectSessionFactory & from, const MessageRef & msg, void * userData);

   /** Overridden to end the server (and hence, the thread) if our connection to the thread is broken. 
     * (this shouldn't ever happen, but just in case...)
     */
   virtual bool ClientConnectionClosed();

   /** Sets the default distribution path for this session.  This path, if set, determines which ThreadWorkerSessions
     * are to receive outgoing data messages when the messages themselves contain no distribution path --
     * only those sessions who match the path will get the user messages.  Setting the path to "" means
     * to revert to sending all data messages to all ThreadWorkerSessions.
     * @param path The new default distribution path.
     */
   void SetDefaultDistributionPath(const String & path) {_defaultDistributionPath = path;}

   /** Returns the current default distribution path. */
   const String & GetDefaultDistributionPath() const {return _defaultDistributionPath;}

   /** Returns a human-readable label for this session type:  "ThreadSupervisor" */
   virtual const char * GetTypeName() const {return "ThreadSupervisor";}

protected:
   /** Handles control messages received from the main thread. 
     * @param msg Reference to the message from the owner.
     * @param numLeft Number of messages still pending in the message queue
     * @returns B_NO_ERROR on success, or B_ERROR if the thread should go away.
     */
   virtual status_t MessageReceivedFromOwner(const MessageRef & msg, uint32 numLeft);

private:
   friend class MessageTransceiverThread;
   friend class DrainTag;

   void DrainTagIsBeingDeleted(DrainTag * tag);
   void DistributeMessageToWorkers(const MessageRef & distMsg);
   status_t AddNewWorkerConnectSession(const AbstractReflectSessionRef & sessionRef, uint32 hostIP, uint16 port);

   Hashtable<DrainTag *, bool> _drainTags;
   String _defaultDistributionPath;
   MessageTransceiverThread * _mtt;
};

/** This little class is used to help us track when workers' output queues are empty.
  * When it gets deleted (inside the internal thread), it triggers the supervisor session
  * to send back an MTT_EVENT_OUTPUT_QUEUES_DRAINED event.
  * It's exposed publically here only because certain (ahem) poorly written programs 
  * need to subclass it in order to be able to safely block until it has gone away.
  */
class DrainTag : public RefCountable
{
public:
   /** Constructor */
   DrainTag() : _notify(NULL) {/* empty */}
 
   /** Destructor.  Notifies our supervisor that we are being deleted. */
   virtual ~DrainTag() {if (_notify) _notify->DrainTagIsBeingDeleted(this);}

private:
   friend class ThreadSupervisorSession;
   friend class MessageTransceiverThread;

   void SetNotify(ThreadSupervisorSession * notify) {_notify = notify;}
   MessageRef GetReplyMessage() const {return _replyRef;}
   void SetReplyMessage(const MessageRef & ref) {_replyRef = ref;}

   ThreadSupervisorSession * _notify;
   MessageRef _replyRef;
};

typedef Ref<DrainTag> DrainTagRef;

END_NAMESPACE(muscle);

#endif

