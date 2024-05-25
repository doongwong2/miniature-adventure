#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>

using namespace std;

template <class T>
class int_array_cell
{
private:
	int _counter;
	T *_memory;

public:
	int_array_cell(T* memory);
	~int_array_cell();
	int getCounter();
	void addCounter();
	void subCounter();
	void output();
	void big();
	void clear();
	void setValue(int index, char ch);
	bool find(const char ch[]);
	void pop(int top);
	T& operator[](const int x);
};

template <class T>
class counter_ptr
{
private:
	char* _name;
	int_array_cell<T> *_cell_ptr;

public:
	counter_ptr(const char name[], T* memory);
	counter_ptr(const char name[]);
	void input(const char ch[]);
	void output();
	void big();
	void clear();
	void pop(int top);
	bool find(const char ch[]);
	~counter_ptr();
	void release();
	T *operator=(T* memory);
	counter_ptr<T> &operator=(counter_ptr<T> &b);
	T& operator[](const int x);
};

class stack
{
private:
	int top;
	counter_ptr<char> counter_ptr;

public:
	stack();
	void command();
};

int main()
{

	// PART 一  (35分)

	counter_ptr<int> b("b", new int[10]);	//需要撰寫 counter_ptr constructor(建構子) 接受兩個參數
											// stdout 輸出: int_array_cell is allocated
											// stdout 輸出: counter_ptr b is assigned to an int_array_cell: counter 1
	
	b = new int[100]; 	//需要撰寫 counter_ptr operator=
		// stdout 輸出: int_array_cell counter 0: deleted
		// stdout 輸出: int_array_cell is allocated
		// stdout 輸出: counter_ptr b is assigned to an int_array_cell: counter 1

	counter_ptr<int> a("a");	//需要撰寫 counter_ptr constructor(建構子) 接受一個參數
		// stdout 輸出: counter_ptr a is not assigned to an int_array_cell

	a = b;			//需要撰寫 counter_ptr operator= 
		// stdout 輸出: int_array_cell counter is increased: counter 2
		// stdout 輸出: counter_ptr a is assigned to an int_array_cell: counter 2

	
	// a leaves it scope:	//需要撰寫 counter_ptr destructor
	// stdout 輸出: counter_ptr a is deleted
	// stdout 輸出: int_array_cell counter is decreased: counter 1


	// PART 二  (35分)

	for (int i = 0; i < 10; i++)
		b[i] = i;  //需要撰寫 counter_ptr operator[] 寫值(回傳資料的參考 return type: int &)

	for (int i = 0; i < 10; i++)
		cout << b[i] << ' ';    //需要撰寫 counter_ptr operator[] 讀值(回傳資料)
	cout << endl;
	// stdout 輸出: 0 1 2 3 4 5 6 7 8 9

	counter_ptr<int> c("c");
	// stdout 輸出: counter_ptr c is not assigned to an int_array_cell

	c = b;
	// stdout 輸出: int_array_cell counter is increased: counter 2
	// stdout 輸出: counter_ptr c is assigned to an int_array_cell: counter 2

	b.release();			//需要撰寫 counter_ptr release function: 不使用記憶體
	// stdout 輸出: int_array_cell counter is decreased: counter 1
	// stdout 輸出: counter_ptr b is not assigned to an int_array_cell
	
	// c leaves it scope:	//需要撰寫 counter_ptr destructor
	// stdout 輸出: counter_ptr c is deleted
	// stdout 輸出: int_array_cell counter 0: deleted
	// stdout 輸出: counter_ptr b is deleted
	system("pause");
	stack stack1;
	while (1)
	{
		stack1.command();
	}
	system("pause");

	return 0;
}

template <class T>
int_array_cell<T>::int_array_cell(T *memory)
{
	this->_counter = 1;
	this->_memory = memory;

	cout << "int_array_cell memory has been allocated." << endl;
}

template <class T>
int_array_cell<T>::~int_array_cell()
{
	delete[] this->_memory;
	this->_memory = NULL;

	cout << "int_array_cell counter : 0 deleted" << endl;
}

template <class T>
int int_array_cell<T>::getCounter()
{
	return this->_counter;
}

template <class T>
void int_array_cell<T>::addCounter()
{
	this->_counter = this->_counter + 1;
	cout << "int_array_cell counter increased, new counter:" << this->_counter << endl;
}

template <class T>
void int_array_cell<T>::subCounter()
{
	this->_counter = this->_counter - 1;
	if (this->_counter == 0) delete this;
	else cout << "int_array_cell counter decreased, new counter:" << this->_counter << endl;
}

template <>
void int_array_cell<char>::output()
{
	cout <<"memory: " << this->_memory << endl;
}

template <>
void int_array_cell<char>::big()
{
	int it = 0;
	while (this->_memory[it] != NULL)
	{
		fprintf(stdout, "(%d %d) ", this->_memory[it], this->_memory[it + 1]);
		it += 2;
	}
	cout << endl;
}

template <>
void int_array_cell<char>::pop(int top)
{
	cout << "pop: " << this->_memory + top - 2 << endl;
	this->_memory[top - 2] = NULL;
}

template <>
void int_array_cell<char>::clear()
{
	this->_memory[0] = NULL;
}

template <>
void int_array_cell<char>::setValue(int index, char ch)
{
	this->_memory[index] = ch;
}

template <>
bool int_array_cell<char>::find(const char ch[])
{
	return strstr(this->_memory, ch) == NULL ? false : true;
}

template <class T>
T& int_array_cell<T>::operator[](const int x)
{
	return this->_memory[x];
}

template <class T>
counter_ptr<T>::counter_ptr(const char name[], T *memory)
{
	this->_name = new char[strlen(name) + 1];
	strcpy(this->_name, name);
	this->_cell_ptr = new int_array_cell<T>(memory);

	fprintf(stdout, "counter_ptr %s is assigned to an int_array_cell: counter %d\n", this->_name, this->_cell_ptr->getCounter());
}

template <class T>
counter_ptr<T>::counter_ptr(const char name[])
{
	this->_name = new char[strlen(name) + 1];
	strcpy(this->_name, name);
	this->_cell_ptr = NULL;

	fprintf(stdout, "counter_ptr %s is  NOT assigned to an int_array_cell\n", this->_name);
}

template <class T>
counter_ptr<T>::~counter_ptr()
{
	cout << "count_ptr" << this->_name << " is deleted" << endl;
	delete[] _name;

	if (this->_cell_ptr != NULL) this->_cell_ptr->subCounter();

}

template <class T>
void counter_ptr<T>::release()
{
	this->_cell_ptr->subCounter();
	this->_cell_ptr = NULL;

	fprintf(stdout, "counter_ptr %s is  NOT assigned to an int_array_cell\n", this->_name);
}

template <>
void counter_ptr<char>::input(const char ch[])
{
	for (int i = 0; i < strlen(ch); i++)
	{
		this->_cell_ptr->setValue(i, ch[i]);
	}
	this->_cell_ptr->setValue(strlen(ch), '\0');
}

template <>
void counter_ptr<char>::output()
{
	this->_cell_ptr->output();
}

template <>
void counter_ptr<char>::big()
{
	this->_cell_ptr->big();
}

template <>
void counter_ptr<char>::pop(int top)
{
	this->_cell_ptr->pop(top);
}

template <>
void counter_ptr<char>::clear()
{
	this->_cell_ptr->clear();
}

template <>
bool counter_ptr<char>::find(const char ch[])
{
	return this->_cell_ptr->find(ch);
}

template <class T>
T* counter_ptr<T>::operator=(T* memory)
{
	if (this->_cell_ptr != NULL)
		this->_cell_ptr->subCounter();

	this->_cell_ptr = new int_array_cell<T>(memory);

	fprintf(stdout, "counter_ptr %s is assigned to an int_array_cell: counter %d\n", this->_name, this->_cell_ptr->getCounter());

	return memory;
}

template <class T>
counter_ptr<T>& counter_ptr<T>::operator=(counter_ptr<T> &b)
{
	if (this->_cell_ptr != NULL) this->_cell_ptr->subCounter();

	this->_cell_ptr = b._cell_ptr;
	this->_cell_ptr->addCounter();

	fprintf(stdout, "counter_ptr %s is assigned to an int_array_cell: counter %d\n", this->_name, this->_cell_ptr->getCounter());
	return *this;
}

template <class T>
T& counter_ptr<T>::operator[](const int x)
{
	return this->_cell_ptr->operator[](x);
}

stack::stack()
	:counter_ptr("stack", new char[1000])
{
	top = -1;
}

void stack::command()
{
	char ch;
	char str[1000];
	cin >> ch;
	switch (ch)
	{
	case 'I':
		cin >> str;
		this->counter_ptr.input(str);
		top = strlen(str) - 1;
		break;

	case 'P':
		if (top == -1) cout << "empty data!" << endl;
		else this->counter_ptr.output();
		break;

	case 'B':
		if (top == -1)cout << "empty data!" << endl;
		else this->counter_ptr.big();
		break;

	case '?':
		fprintf(stdout, "find which character?\n");
		cin >> str;
		if (this->counter_ptr.find(str)) cout << "YES" << endl;
		else cout << "NO" << endl;
		break;

	case 'R':
		if (top == -1) cout << "empty data!" << endl;
		else
		{
			this->counter_ptr.pop(top + 1);
			top -= 2;
		}
		break;

	case 'C':
		this->counter_ptr.clear();
		this->top = -1;
		break;

	default:
		break;
	}
}
