// C++ program to reverse a linked list
// from position m to position n
//#include <bits/stdc++.h>
#include <iostream>
using namespace std;

struct Node {
    int data;
    struct Node *next;
    Node(int val) {
        data = val;
        next = nullptr;
    }
};

// Function to reverse a linked list
Node *reverse(struct Node *head) {
    Node *prevNode = NULL;
    Node *currNode = head;
    while (currNode) {
        Node *nextNode = currNode->next;
        currNode->next = prevNode;
        prevNode = currNode;
        currNode = nextNode;
    }
    return prevNode;
}

// Function to reverse a linked list from position m to n
Node *reverseBetween(Node *head, int m, int n) {
  
    // If m and n are the same, no reversal is needed
    if (m == n)
        return head;

    Node *revs = NULL, *revs_prev = NULL;
    Node *revend = NULL, *revend_next = NULL;

    // Traverse the list to locate the nodes
    // and pointers needed for reversal
    int i = 1;
    Node *currNode = head;
    while (currNode && i <= n) {

        // Track the node just before the start of
        // the reversal segment
        if (i < m)
            revs_prev = currNode;
      
        // Track the start of the reversal segment
        if (i == m)
            revs = currNode;
      
        // Track the end of the reversal
        // segment and the node right after it
        if (i == n) {
            revend = currNode;
            revend_next = currNode->next;
        }
        currNode = currNode->next;
        i++;
    }

    // Detach the segment to be reversed
    // from the rest of the list
    revend->next = NULL;

    // Reverse the segment from position m to n
    revend = reverse(revs);

    // Reattach the reversed segment back to the list
    // If the reversal segment was not at the head of the list
    if (revs_prev)
        revs_prev->next = revend;
    else
        head = revend;

    // Connect the end of the reversed
    // segment to the rest of the list
    revs->next = revend_next;

    return head;
}

void print(Node *head) {
    while (head != NULL) {
        cout << head->data << " ";
        head = head->next;
    }
    cout << "NULL" << endl;
}

int main() {
  
    // Initialize linked list:
    // 10 -> 20 -> 30 -> 40 -> 50 -> 60 -> 70
    Node *head = new Node(10);
    head->next = new Node(20);
    head->next->next = new Node(30);
    head->next->next->next = new Node(40);
    head->next->next->next->next = new Node(50);
    head->next->next->next->next->next = new Node(60);
    head->next->next->next->next->next->next = new Node(70);

    cout << "Original list: ";
    print(head);
  
    head = reverseBetween(head, 3, 6);

    cout << "Modified list: ";
    print(head);

    return 0;
}
