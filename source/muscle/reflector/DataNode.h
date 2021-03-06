/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */

#ifndef MuscleDataNode_h
#define MuscleDataNode_h

#include "reflector/DumbReflectSession.h"
#include "reflector/StorageReflectConstants.h"
#include "regex/PathMatcher.h"

BEGIN_NAMESPACE(muscle);

class StorageReflectSession;
class DataNode;

/** Type for a Reference to a DataNode object */
typedef Ref<DataNode> DataNodeRef;

/** Iterator type for our child objects */
typedef HashtableIterator<const char *, DataNodeRef> DataNodeRefIterator;

/** Each object of this class represents one node in the server-side data-storage tree.  */
class DataNode : public RefCountable
{
public:
   /**
    * Put a child without changing the ordering index
    * @param child Reference to the child to accept into our list of children
    * @param optNotifyWithOnSetParent If non-NULL, a StorageReflectSession to use to notify subscribers that the new node has been added
    * @param optNotifyWithOnChangedData If non-NULL, a StorageReflectSession to use to notify subscribers when the node's data has been alterered
    * @return B_NO_ERROR on success, B_ERROR if out of memory
    */
   status_t PutChild(DataNodeRef & child, StorageReflectSession * optNotifyWithOnSetParent, StorageReflectSession * optNotifyWithOnChangedData);

   /**
    * Create and add a new child node for (data), and put it into the ordering index
    * @param data Reference to a message to create a new child node for.
    * @param optInsertBefore if non-NULL, the name of the child to put the new child before in our index.  If NULL, (or the specified child cannot be found) the new child will be appended to the end of the index.
    * @param optNodeName If non-NULL, the inserted node will have the specified name.  Otherwise, a name will be generated for the node.
    * @param optNotifyWithOnSetParent If non-NULL, a StorageReflectSession to use to notify subscribers that the new node has been added
    * @param optNotifyWithOnChangedData If non-NULL, a StorageReflectSession to use to notify subscribers when the node's data has been alterered
    * @param optAddNewChildren If non-NULL, any newly formed nodes will be added to this hashtable, keyed on their absolute node path.
    * @return B_NO_ERROR on success, B_ERROR if out of memory
    */
   status_t InsertOrderedChild(const MessageRef & data, const char * optInsertBefore, const char * optNodeName, StorageReflectSession * optNotifyWithOnSetParent, StorageReflectSession * optNotifyWithOnChangedData, Hashtable<String, DataNodeRef> * optAddNewChildren);
 
   /** 
    * Moves the given node (which must be a child of ours) to be just before the node named
    * (moveToBeforeThis) in our index.  If (moveToBeforeThis) is not a node in our index,
    * then (child) will be moved back to the end of the index. 
    * @param child A child node of ours, to be moved in the node ordering index.
    * @param moveToBeforeThis name of another child node of ours.  If this name is NULL or
    *                         not found in our index, we'll move (child) to the end of the index.
    * @param optNotifyWith If non-NULL, this will be used to sent INDEXUPDATE message to the
    *                      interested clients, notifying them of the change.
    * @return B_NO_ERROR on success, B_ERROR on failure (out of memory)
    */
   status_t ReorderChild(const DataNode & child, const char * moveToBeforeThis, StorageReflectSession * optNotifyWith);

   /** Returns true iff we have a child with the given name */
   bool HasChild(const char * key) const {return ((_children)&&(_children->ContainsKey(key)));}

   /** Retrieves the child with the given name.
    *  @param key The name of the child we wish to retrieve
    *  @param returnChild On success, a reference to the retrieved child is written into this object.
    *  @return B_NO_ERROR if a child node was successfully retrieved, or B_ERROR if it was not found.
    */
   status_t GetChild(const char * key, DataNodeRef & returnChild) const {return ((_children)&&(_children->Get(key, returnChild) == B_NO_ERROR)) ? B_NO_ERROR : B_ERROR;}

   /** Removes the child with the given name.
    *  @param key The name of the child we wish to remove.
    *  @param optNotifyWith If non-NULL, the StorageReflectSession that should be used to notify subscribers that the given node has been removed
    *  @param recurse if true, the removed child's children will be removed from it, and so on, and so on...
    *  @param optCounter if non-NULL, this value will be incremented whenever a child is removed (recursively)
    *  @return B_NO_ERROR if the given child is found and removed, or B_ERROR if it could not be found.
    */
   status_t RemoveChild(const char * key, StorageReflectSession * optNotifyWith, bool recurse, uint32 * optCounter);

   /** Returns an iterator that can be used for iterating over our set of children. */
   DataNodeRefIterator GetChildIterator() const {return _children ? _children->GetIterator() : DataNodeRefIterator();}

   /** Returns the number of child nodes this node contains. */
   uint32 CountChildren() const {return _children ? _children->GetNumItems() : 0;}

   /** Returns the ASCII name of this node (e.g. "joe") */
   const String & GetNodeName() const {return _nodeName;}

   /** Generates and returns the full node path of this node (e.g. "/12.18.240.15/1234/beshare/files/joe").
     * @param retPath On success, this String will contain this node's absolute path.
     * @param startDepth The depth at which the path should start.  Defaults to zero, meaning the full path.
     *                   Values greater than zero will return a partial path (e.g. a startDepth of 1 in the
     *                   above example would return "12.18.240.15/1234/beshare/files/joe", and a startDepth
     *                   of 2 would return "1234/beshare/files/joe")
     * @returns B_NO_ERROR on success, or B_ERROR on failure (out of memory?)
     */
   status_t GetNodePath(String & retPath, uint32 startDepth = 0) const;

   /** Returns the name of the node in our path at the (depth) level.
     * @param depth The node name we are interested in.  For example, 0 will return the name of the
     *              root node, 1 would return the name of the IP address node, etc.  If this number
     *              is greater than (depth), this method will return NULL.
     */
   const char * GetPathClause(uint32 depth) const;

   /** Replaces this node's payload message with that of (data).
    *  @param data the new Message to associate with this node.
    *  @param optNotifyWith if non-NULL, this StorageReflectSession will be used to notify subscribers that this node's data has changed.
    *  @param isBeingCreated Should be set true only if this is the first time SetData() was called on this node after its creation.
    *                        Which is to say, this should almost always be false.
    */
   void SetData(const MessageRef & data, StorageReflectSession * optNotifyWith, bool isBeingCreated);

   /** Returns a reference to this node's Message payload. */
   MessageRef GetData() const {return _data;}

   /** Returns our node's parent, or NULL if this node doesn't have a parent node. */
   DataNode * GetParent() const {return _parent;}

   /** Returns this node's depth in the tree (e.g. zero if we are the root node, 1 if we are its child, etc) */
   uint32 GetDepth() const {return _depth;}

   /** Returns us to our virgin, pre-Init() state, by clearing all our children, subscribers, parent, etc.  */
   void Reset();  

   /**
    * Modifies the refcount for the given sessionID.
    * Any sessionID's with (refCount > 0) will be in the GetSubscribers() list.
    * @param sessionID the sessionID whose reference count is to be modified
    * @param delta the amount to add to the reference count.
    */
   void IncrementSubscriptionRefCount(const char * sessionID, long delta);

   /** Returns an iterator that can be used to iterate over our list of active subscribers */
   HashtableIterator<const char *, uint32> GetSubscribers() const {return _subscribers.GetIterator();}

   /** Returns a pointer to our ordered-child index */
   const Queue<const char *> * GetIndex() const {return _orderedIndex;}

   /** Insert a new entry into our ordered-child list at the (nth) entry position.
    *  Don't call this function unless you really know what you are doing!
    *  @param insertIndex Offset into the array to insert at
    *  @param optNotifyWith Session to use to inform everybody that the index has been changed.
    *  @param key Name of an existing child of this node that should be associated with the given entry.
    *             This child must not already be in the ordered-entry index!
    *  @return B_NO_ERROR on success, or B_ERROR on failure (bad index or unknown child).
    */
   status_t InsertIndexEntryAt(uint32 insertIndex, StorageReflectSession * optNotifyWith, const char * key);

   /** Removes the (nth) entry from our ordered-child index, if possible.
    *  Don't call this function unless you really know what you are doing!
    *  @param removeIndex Offset into the array to remove
    *  @param optNotifyWith Session to use to inform everybody that the index has been changed.
    *  @return B_NO_ERROR on success, or B_ERROR on failure.
    */
   status_t RemoveIndexEntryAt(uint32 removeIndex, StorageReflectSession * optNotifyWith);

private:
   /** Default Constructor.  Don't use this, use StorageReflectSession::GetNewDataNode() instead!  */
   DataNode();

   /** Destructor.   Don't use this, use StorageReflectSession::ReleaseDataNode() instead!  */
   ~DataNode();

   friend class StorageReflectSession;
   friend class ObjectPool<DataNode>;
   friend class Ref<DataNode>;

   void Init(const char * nodeName, const MessageRef & initialValue);
   void SetParent(DataNode * _parent, StorageReflectSession * optNotifyWith);
   status_t RemoveIndexEntry(const char * key, StorageReflectSession * optNotifyWith);

   DataNode * _parent;
   MessageRef _data;
   Hashtable<const char *, DataNodeRef> * _children;  // lazy-allocated
   Queue<const char *> * _orderedIndex;  // only used when tracking the ordering of our children (lazy-allocated)
   uint32 _orderedCounter;
   String _nodeName;
   uint32 _depth;  // number of ancestors our node has (e.g. root's _depth is zero)

   Hashtable<const char *, uint32> _subscribers; 
};

END_NAMESPACE(muscle);

#endif
