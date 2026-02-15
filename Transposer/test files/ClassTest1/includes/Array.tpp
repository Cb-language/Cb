#pragma once
#include "Array.h"

template <typename T>
Array<T>::Array() : defaultValueSet(T())
{
	this->size = 1;
	data = new T[size];

	data[0] = defaultValueSet;
}

template<typename T>
Array<T>::Array(size_t size, T defaultValue) : defaultValueSet(defaultValue)
{
	this->size = size;
	data = new T[size];

	for (size_t i = 0; i < size; ++i)
	{
		data[i] = defaultValueSet;
	}
}

template <typename T>
Array<T>::Array(size_t size, const Array& other) : size(size), defaultValueSet(other.defaultValueSet)
{
	data = new T[size];

	const size_t copyCount = std::min(size, other.size);

	for (size_t i = 0; i < copyCount; i++)
	{
		data[i] = other[i];
	}

	for (size_t i = copyCount; i < size; i++)
	{
		data[i] = defaultValueSet;
	}
}

template<typename T>
Array<T>::Array(const Array<T>& other)
{
	this->size = other.size;
	this->defaultValueSet = other.defaultValueSet;

	data = new T[size];

	for (size_t i = 0; i < size; ++i)
	{
		data[i] = other[i];
	}
}

template<typename T>
Array<T>::Array(const Array<T>& other, const std::string& sliced_str)
{
	Array<T> temp = other[sliced_str];
	this->size = temp.size;
	this->defaultValueSet = temp.defaultValueSet;

	data = new T[size];

	for (size_t i = 0; i < size; i++)
	{
		data[i] = temp[i];
	}
}

template <typename T>
Array<T>& Array<T>::operator=(const Array<T>& other)
{
	if (this == &other)
	{
		return *this;
	}

	delete[] data;

	size = other.size;
	defaultValueSet = other.defaultValueSet;

	data = new T[size];
	for (size_t i = 0; i < size; ++i)
	{
		data[i] = T(other.data[i]);
	}

	return *this;
}

template <typename T>
Array<T>& Array<T>::operator=(const T& defaultValue)
{
	delete[] data;

	defaultValueSet = defaultValue;

	data = new T[size];
	for (size_t i = 0; i < size; i++)
	{
		data[i] = T(defaultValue);
	}

	return *this;
}

template <typename T>
template <typename U>
Array<T>& Array<T>::operator=(const Array<U>& other)
{
	if (reinterpret_cast<const void*>(this) == reinterpret_cast<const void*>(&other))
		return *this;

	delete[] data;
	size = other.size;
	data = new T[size];

	for (std::size_t i = 0; i < size; i++)
	{
		if constexpr (std::is_same_v<T, std::string>)
		{
			std::ostringstream oss;
			oss << other[i];
			data[i] = oss.str();
		}
		else
		{
			data[i] = static_cast<T>(other[i]);
		}
	}

	if constexpr (std::is_same_v<T, std::string>)
	{
		std::ostringstream oss;
		oss << other.defaultValueSet;
		defaultValueSet = oss.str();
	}
	else
	{
		defaultValueSet = static_cast<T>(other.defaultValueSet);
	}

	return *this;
}


template <typename T>
template <typename U>
Array<T>& Array<T>::operator=(const U& defaultValue)
{
	delete[] data;

	if constexpr (std::is_same_v<T, std::string>)
	{
		std::ostringstream oss;
		oss << defaultValue;
		defaultValueSet = oss.str();

		data = new T[size];
		for (size_t i = 0; i < size; i++)
		{
			data[i] = defaultValueSet;
		}

		return *this;
	}

	defaultValueSet = defaultValue;

	data = new T[size];
	for (size_t i = 0; i < size; i++)
	{
		data[i] = T(static_cast<T>(defaultValue));
	}

	return *this;
}

template <typename T>
bool Array<T>::operator==(const Array<T>& other) const
{
	if (size != other.size)
	{
		return false;
	}

	for (size_t i = 0; i < size; i++)
	{
		if (data[i] != other.data[i])
		{
			return false;
		}
	}

	return true;
}

template <typename T>
bool Array<T>::operator!=(const Array<T>& other) const
{
	return !(*this == other);
}

template<typename T>
template <typename U>
Array<T>::Array(const Array<U>& other, const bool isValArray)
{
	this->size = other.size;
	data = new T[size];

	if constexpr (std::is_same_v<T, std::string>)
	{
		std::ostringstream oss;
		oss << other.defaultValueSet;

		this->defaultValueSet = oss.str();

		for (size_t i = 0; i < size; ++i)
		{
			std::ostringstream ossData;
			ossData << other[i];
			data[i] = ossData.str();
		}
	}
	else if (isValArray)
	{
		this->defaultValueSet =  static_cast<T>(other.size, other);

		for (size_t i = 0; i < size; ++i)
		{
			data[i] = static_cast<T>(other);
		}
	}
	else
	{
		this->defaultValueSet = static_cast<T>(other.defaultValueSet);

		for (size_t i = 0; i < size; ++i)
		{
			data[i] = static_cast<T>(other[i]);
		}
	}
}

template<typename T>
Array<T>::~Array()
{
	delete[] data;
	data = nullptr;
	size = 0;
	defaultValueSet = T();
}

template<typename T>
T& Array<T>::operator[](int index)
{
	const int sz = static_cast<int>(size);
	if (index < 0)
	{
		if (index < sz * -1)
		{
			throw std::out_of_range("Index out of bounds");
		}

		index += sz;
	}
	else if (index >= sz)
	{
		throw std::out_of_range("Index out of bounds");
	}

	return data[index];
}

// Const version of operator[]
template<typename T>
const T& Array<T>::operator[](size_t index) const
{
	const int sz = static_cast<int>(size);
	if (index < 0)
	{
		if (index < sz * -1)
		{
			throw std::out_of_range("Index out of bounds");
		}

		index += sz;
	}
	else if (index >= sz)
	{
		throw std::out_of_range("Index out of bounds");
	}

	return data[index];
}

// Size
template<typename T>
size_t Array<T>::Length() const
{
	return size;
}

// -Size
template<typename T>
size_t Array<T>::NegLength() const
{
	return -size - 1;
}


template <typename T>
size_t Array<T>::getNormalIndex(size_t index) const
{
	if (index < 0)
	{
		index += static_cast<size_t>(size);
	}

	if (index < 0 || static_cast<size_t>(index) >= size)
	{
		throw std::out_of_range("Array index out of range");
	}

	return static_cast<size_t>(index);
}


template<typename T>
void Array<T>::remove(size_t removeIndex)
{
	T* newData = new T[size - 1];
	int newIndex = 0;

	removeIndex = getNormalIndex(removeIndex);

	for (int i = 0; i < size; i++)
	{
		if (i != removeIndex)
		{
			newData[newIndex] = data[i];
			newIndex++;
		}
	}

	delete[] data;
	data = newData;
	--this->size;
	newData = nullptr;
}

template <typename T>
void Array<T>::add(const T& value, size_t addIndex)
{
	if (addIndex == -1 || addIndex == size)
	{
		addIndex = size;
	}
	else if (addIndex == NegLength())
	{
		addIndex = 0;
	}
	else
	{
		addIndex = getNormalIndex(addIndex);
	}

	T* newData = new T[++this->size];


	for (size_t i = 0; i < addIndex; i++)
	{
		newData[i] = data[i];
	}

	newData[addIndex] = value;

	for (size_t i = addIndex; i < size; i++)
	{
		newData[i + 1] = data[i];
	}

	delete[] data;
	data = newData;
	newData = nullptr;
}

/*
	Slice the array like in Python:
	From 'start' (inclusive) to 'stop' (exclusive), stepping by 'step' each time.
	i.e. taking from ['start', 'stop') jumping 'step' each time
*/
template<typename T>
Array<T> Array<T>::slice(int start, int stop, int step)  const
{
	if (step == 0)
	{
		throw std::out_of_range("Step cannot be zero");
	}

	const int sz = static_cast<int>(size);

	// Handle default stop
	if (stop == -1)
	{
		stop = sz;
	}

	// Normalize negative indices
	if (start < 0)
	{
		start += sz;
	}

	if (stop < 0)
	{
		stop += sz;
	}

	if (start < 0 || start >= sz || stop < 0 || stop > sz)
	{
		throw std::out_of_range("Slice indices out of range");
	}

	// Calculate length of the slice
	int length = 0;
	if (step > 0 && start < stop)
	{
		length = (stop - start + step - 1) / step;
	}
	else if (step < 0 && start > stop)
	{
		length = (start - stop - step - 1) / (-step);
	}
	else
	{
		length = 0;
	}

	Array<T> result(length, defaultValueSet);

	int idx = start;
	for (int i = 0; i < length; i++)
	{
		result.data[i] = data[idx];
		idx += step;
	}

	return result;
}
