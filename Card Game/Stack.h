#pragma once
#include "stdafx.h"
#include "Card.h"

						/*STACK*/

// Keeping the code generic   could also use: "typedef Card Item;"
// 
template <typename Item>

class Stack
{

private:

	struct Node
	{
		Item m_data;		// The thing we're storing
		Node* m_next;		// Pointer to the next node in the list
	};

	Node* m_top;			// The most recently added thing
	int m_currSize;	// Number of things currently allocated
	const int m_maxSize;

public:

	// Def ctor
	// Creates an empty list
	Stack(int _max = 0);

	// Dtor
	~Stack();

private:

	// Disabled Copy ctor
	Stack(const Stack& _q) : m_maxSize(0) { }

	// Disabled assignment operator
	Stack& operator=(const Stack& q) { return *this; }
	/* Accessors */
public:
	int GetCurrSize() const { return m_currSize; }

	// Add something to the top of the stack
	// In:	_info		The value to add
	//
	// Return: True, if added
	bool Push(const Item& _info);


	// Remove the top-most value from the list
	// In:	_info		A "blank" value
	//
	// Out: _info		The value that was at the top
	// Return: True, if something was removed
	bool Pop(Item& _info);

	// Clear out the stack for re-use
	void Clear();

	// Look at the thing on top without removing it
	const Item* Peek() const;
};


// Def ctor
// Creates an empty list
template<typename Item>
Stack<Item>::Stack(int _max) : m_maxSize(_max)
{
	m_top = NULL;
	m_currSize = 0;
}

// Dtor
template<typename Item>
Stack<Item>::~Stack()
{
	Clear();
}

// Add something to the top of the stack
// In:	_info		The value to add
//
// Return: True, if added
template<typename Item>
bool Stack<Item>::Push(const Item& _info)
{
	// Create space for the thing to add
	Node* newTop = new Node;

	// Did the dynamic allocation fail?
	if (NULL == newTop)
		return false;

	// Copy over the value into the node
	newTop->m_data = _info;

	// Link the node up with the rest of the list
	newTop->m_next = m_top;

	// Update the list to reflect that a new node was added
	m_top = newTop;

	// Success
	++m_currSize;
	return true;
}


// Remove the top-most value from the list
// In:	_info		A "blank" value
//
// Out: _info		The value that was at the top
// Return: True, if something was removed
template<typename Item>
bool Stack<Item>::Pop(Item& _info)
{
	// If the list is empty, leave
	if (!m_top)
		return false;

	// Copy over the value
	_info = m_top->m_data;

	// Update the stack by using a temporary node pointer
	Node* temp = m_top;

	m_top = m_top->m_next;

	delete temp;

	// Success
	--m_currSize;
	return true;
}

// Clear out the stack for re-use
template<typename Item>
void Stack<Item>::Clear()
{
	// Loop until we're empty
	while (m_top)
	{
		Node* temp = m_top;

		m_top = m_top->m_next;

		delete temp;
	}

	// Reset back to 0 size
	m_currSize = 0;
}

// Look at the thing on top without removing it
template<typename Item>
const Item* Stack<Item>::Peek() const
{
	// If there's something there
	if (m_top)
		return &m_top->m_data;
	else
		return NULL;
}
