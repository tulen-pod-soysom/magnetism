#include <array>

template <typename T, size_t s>
struct adaptive_resolution_vector : public std::array<T, s>
{
private:
	size_t skipped_inputs = 0;
	size_t _size = 0;
	size_t adapting_order = 0;
public:

	adaptive_resolution_vector() : std::array<T, s>() {}

	void push_back(const T& value)
	{
		if (_size + 1 >= s / 2)
		{
			if (skipped_inputs < (1 << (adapting_order)) / 2)
			{
				skipped_inputs++;	return;
			}
			else skipped_inputs = 0;
		}

		if (_size + 1 >= s)
		{
			for (size_t i = 0; i < s / 2; i++)
			{
				(*this)[i] = (*this)[2 * i];
			}
			_size = s / 2;
			adapting_order++;
		}
		(*this)[_size++] = value;
	}

	void clear()
	{
		for (auto& i : *this)
		{
			i = {};
		}
		_size = 0;
	}

	size_t size() { return _size; }

	T& operator[] (size_t index)
	{
		if (index + 1 > _size) throw "out of bounds";
		return std::array<T, s>::operator[](index);
	}

    auto end()
    {return this->begin() + _size;}
};
