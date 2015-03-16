// **********************************************************************
//
// File: LinkedList.h
// Programmer: T.J. Eason
// Project: Game Engine
// Description: Linked list data structure
// Date: 1-15-10
// Revision 1: 1-19-10
// Revision 2: 3-24-10
// Revision 3: 3-25-10
// Revision 4: 4-21-10
//
// **********************************************************************

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

// Linked list class
template < class Type > class LinkedList
{
	public:

		// Element structure
		struct Element
		{
			Type *data;		// Pointer to data held in element

			Element *next;		// Pointer to next element in list
			Element *prev;		// Pointer to previous element in list

			// Element constructor
			Element( Type *element )
			{
				data = element;		
				next = prev = NULL;
			}

			// Element destructor
			~Element()
			{
				SAFE_DELETE( data );

				// Next pointer is assigned to previous
				if( next)
					next -> prev = prev;

				// Previous pointer is assigned to next
				if( prev )
					prev ->next = next;
			}
		};

		// Linked list constructor
		LinkedList()
		{
			m_first = m_last = m_iterate = m_temp = NULL;
			m_totalElements = 0;
		}

		// Linked list destructor
		~LinkedList()
		{
			Empty();

		}

		// Add elemnts to the end of the list
		Type *Add( Type *element )
		{
			// Empty list
			if( element == NULL )
				return NULL;

			// First element creates new list
			if( m_first == NULL )
			{
				m_first = new Element( element );
				m_last = m_first;
			}

			// Extend the list if the first element exists
			else
			{
				m_last -> next = new Element( element );
				m_last -> next -> prev = m_last;
				m_last  = m_last -> next;
			}

			m_totalElements++;

			return m_last -> data;

		}

		// Insert new element into linked list
		Type *InsertBefore( Type *element, Element *nextElement )
		{
			m_temp = nextElement -> prev;

			m_totalElements++;

			// Check if next element exists
			if( m_temp == NULL )
			{
				m_first = new Element( element );
				m_first -> next = nextElement;
				nextElement -> prev = m_first;

				return m_first -> data;

			}

			// Insert next element into the list
			else
			{
				m_temp -> next = new Element( element );
				m_temp -> next -> prev = m_temp;
				m_temp -> next -> next = nextElement;
				nextElement -> prev = m_temp -> next;

				return m_temp -> next -> data;

			}
		}

		// Remove element from list and destroy its data
		void Remove( Type **element )
		{
			m_temp = m_first;

			// Pointer still sees data in an element
			while( m_temp != NULL )
			{
				// Check for data in an element
				if( m_temp -> data == *element )
				{
					// Temp pointer reached the head of the linked list
					if( m_temp == m_first )
					{
						m_first = m_first -> next;

						// Remove element from head
						if( m_first )
							m_first -> prev = NULL;
					}

					// Temp pointer reached the tail of the linked list
					if( m_temp == m_last )
					{
						m_last = m_last -> prev;

						// Remove element from tail
						if( m_last )
							m_last -> next = NULL;
					}

					SAFE_DELETE( m_temp );

					*element = NULL;

					m_totalElements--;

					return;

				}

				m_temp = m_temp -> next;
			}
		}

		// Destroy all elements and data in the linked list
		void Empty()
		{
			// While an element still exists
			while( m_last != NULL )
			{
				m_temp = m_last;
				m_last = m_last -> prev;

				SAFE_DELETE( m_temp );
			}

			m_first = m_last = m_iterate = m_temp = NULL;
			m_totalElements = 0;

		}

		// Remove all elements and clear data pointers
		void ClearPointers()
		{
			// While an element still exists
			while( m_last != NULL )
			{
				m_temp = m_last;
				m_temp -> data = NULL;
				m_last = m_last -> prev;

				SAFE_DELETE( m_temp );
			}

			m_first = m_last = m_iterate = m_temp = NULL;
			m_totalElements = 0;

		}

		// Remove an elements and its pointers
		void ClearPointer( Type **element)
		{
			m_temp = m_first;

			// While an element exists
			while( m_temp != NULL )
			{
				// If an element has a pointer to data
				if( m_temp -> data == *element )
				{
					// Temp pointer is at the head of the linked list
					if( m_temp == m_first )
					{
						m_first = m_first -> next;

						// Head pointer is cleared
						if( m_first ) 
							m_first -> prev = NULL;
					}

					// Temp pointer is at the tail of the linked list
					if( m_temp == m_last )
					{
						m_last = m_last -> prev;

						// Tail pointer is cleared
						if( m_last )
							m_last -> next = NULL;
					}

					m_temp -> data = NULL;

					SAFE_DELETE( m_temp );

					*element = NULL;

					m_totalElements--;

					return;
				}

				m_temp = m_temp -> next;
			}
		}

		// Iterate through the elements of the linked list
		Type *Iterate( bool restart = false )
		{
			// If restart is true, then clear iteration and start at the beginning 
			if( restart )
				m_iterate = NULL;

			// Iteration is starting
			else
			{
				// Iteration is at the front end of the list
				if( m_iterate == NULL )
					m_iterate = m_first;

				// After first iteration, move on to the next element
				else
					m_iterate = m_iterate -> next;
			}

			// No iteration, do nothing
			if( m_iterate == NULL )
				return NULL;

			// Iteration happend, so retrieve data
			else
				return m_iterate -> data;

		}

		// Get current iterated element in the linked list
		Type *GetCurrent()
		{
			//  Iteration happened, so data exists
			if( m_iterate )
				return m_iterate -> data;
	
			// Current data does not exist
			else
				return NULL;

		}

		// Get the first element in the linked list
		Type *GetFirst()
		{
			// First element exists, so retrieve data
			if( m_first )
				return m_first -> data;

			// First element does not exist
			else
				return NULL;

		}

		// Get the last element in the linked list 
		Type *GetLast()
		{
			// Last element exists, so retrieve data
			if( m_last )
				return m_last -> data;

			// Last element does not exist
			else
				return NULL;

		}

		// Get the next element in the linked list
		Type *GetNext( Type *element )
		{
			m_temp = m_first;

			// While data still exists
			while( m_temp != NULL )
			{
				// Temp pointer to data within an element
				if( m_temp -> data == element )
				{
					// Pointer move to next element but it reaches the end of the list
					if( m_temp -> next == NULL )
						return NULL;

					// Next element contains data, so retrieve it
					else
						return m_temp -> next -> data;
				}

				m_temp = m_temp -> next;

			}

			return NULL;

		}

		// Get a random element from the linked list
		Type *GetRandom()
		{
			// No elements exists, so do nothing
			if( m_totalElements == 0 )
				return NULL;

			// One element exists, get its data
			else if( m_totalElements == 1 )
				return m_first -> data;

			unsigned long element = rand() % m_totalElements;

			m_temp = m_first;

			// Move pointer to next random element
			for( unsigned long e = 0; e < element; e++)
				m_temp = m_temp -> next;

			return m_temp -> data;

		}

		// Get entire element including the next and previous pointers
		Element *GetCompleteElement( Type *element )
		{
			m_temp = m_first;

			// While pointer does not reach the end of the list
			while( m_temp != NULL )
			{
				// Temp pointer locates an element and retrieves its data
				if( m_temp -> data == element )
					return m_temp;

				m_temp = m_temp -> next;
			}

			return NULL;

		}

		// Get total number of elements in the linked list
		unsigned long GetTotalElements()
		{
			return m_totalElements;

		}

	private:
			Element *m_first;		// First element in list
			Element *m_last;		// Last element in list
			Element *m_iterate; // Iterate through list
			Element *m_temp;   // Store data temporarily in list

			unsigned long m_totalElements;   // Total amount of elements in the list
};

#endif
						