/* operations on double ended queue*/
#include <stdio.h>
#define SIZE 5

int deque[SIZE];
int front = -1, rear = -1;

// (a) Insertion at Front
void insertFront(int value) {
    if ((front == 0 && rear == SIZE - 1) ||
        (front == rear + 1)) {
        printf("Deque Overflow!\n");
        return;
    }

    if (front == -1) {
        front = rear = 0;
    }
    else if (front == 0) {
        front = SIZE - 1;
    }
    else {
        front--;
    }

    deque[front] = value;
    printf("%d inserted at front\n", value);
}

// (b) Insertion at Rear
void insertRear(int value) {
    if ((front == 0 && rear == SIZE - 1) ||
        (front == rear + 1)) {
        printf("Deque Overflow!\n");
        return;
    }

    if (front == -1) {
        front = rear = 0;
    }
    else if (rear == SIZE - 1) {
        rear = 0;
    }
    else {
        rear++;
    }

    deque[rear] = value;
    printf("%d inserted at rear\n", value);
}

// (c) Deletion from Front
void deleteFront() {
    if (front == -1) {
        printf("Deque Underflow!\n");
        return;
    }

    printf("Deleted from front: %d\n", deque[front]);

    if (front == rear) {
        front = rear = -1;
    }
    else if (front == SIZE - 1) {
        front = 0;
    }
    else {
        front++;
    }
}

// (d) Deletion from Rear
void deleteRear() {
    if (front == -1) {
        printf("Deque Underflow!\n");
        return;
    }

    printf("Deleted from rear: %d\n", deque[rear]);

    if (front == rear) {
        front = rear = -1;
    }
    else if (rear == 0) {
        rear = SIZE - 1;
    }
    else {
        rear--;
    }
}

// Display Deque
void display() {
    if (front == -1) {
        printf("Deque is empty\n");
        return;
    }

    int i = front;

    printf("Deque elements: ");

    while (1) {
        printf("%d ", deque[i]);

        if (i == rear)
            break;

        i = (i + 1) % SIZE;
    }

    printf("\n");
}

int main() {
    insertRear(10);
    insertRear(20);
    insertFront(5);

    display();

    deleteFront();
    deleteRear();

    display();

    return 0;
}
