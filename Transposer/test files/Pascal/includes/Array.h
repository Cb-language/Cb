#pragma once
#include <sstream>

template <typename T>
class Array
{
	template<typename U>
	friend class Array;

private:
	T* data;
	size_t size;
	T defaultValueSet;

	size_t getNormalIndex(size_t index) const;

public:
	Array();
	explicit Array(size_t size, T defaultValue = T());
	Array(size_t size, const Array& other);
	Array(const Array& other);
	Array(const Array& other, const std::string& sliced_str);

	Array& operator=(const Array& other);
	Array& operator=(const T& defaultValue);

	template <typename U>
	Array& operator=(const Array<U>& other);

	template <typename U>
	Array& operator=(const U& defaultValue);

	bool operator==(const Array& other) const;
	bool operator!=(const Array& other) const;

	template <typename U>
	explicit Array(const Array<U>& other, const bool isValArray = false);

	~Array();

	T& operator[](int index);
	const T& operator[](size_t index) const;

	size_t Length() const;
	size_t NegLength() const;

	void remove(size_t removeIndex = -1);
	void add(const T& value, size_t addIndex = -1);

	Array slice(int start, int stop, int step) const;

	friend std::ostream& operator<<(std::ostream& os, const Array& other)
	{
		bool first = true;
		for (size_t i = 0; i < other.size; i++)
		{
			os << (first ? "" : ", ") << "[" << other[i] << "]";
			first = false;
		}
		return os;
	}
};

#include "Array.tpp"
