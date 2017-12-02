/* This is the file StackTypes.h   */

typedef int ItemType;
/* char is the type for our first application */
/* float is the type for our second application */

typedef struct StackNodeTag {
           ItemType Item;
           struct StackNodeTag *Link;
        } StackNode;

typedef struct {
           StackNode *ItemList;
        } Stack;
